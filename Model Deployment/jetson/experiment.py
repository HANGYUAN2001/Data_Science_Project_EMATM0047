#!/usr/bin/env python3
# python3 /home/nvidia/Desktop/experiment.py
# pip3 install aiohttp -i https://pypi.tuna.tsinghua.edu.cn/simple




import cv2
import numpy as np
import tensorflow as tf
import time
import threading
from collections import deque

class CameraProcessor:
    def __init__(self, model_path, class_names):
        # Initialize TFLite Interpreter
        self.interpreter = tf.lite.Interpreter(model_path=model_path)
        self.interpreter.allocate_tensors()
        
        # Get input/output details
        self.input_details = self.interpreter.get_input_details()
        self.output_details = self.interpreter.get_output_details()
        
        # Check if model expects quantized input
        self.is_quantized = self.input_details[0]['dtype'] == np.uint8
        
        self.class_names = class_names
        
        # Define colors for each class (BGR format)
        self.class_colors = {
            "100": ((0, 0, 255), (0, 0, 255)),   # Red for border and text
            "50": ((0, 255, 0), (0, 255, 0)),     # Green for border and text
            "50-100": ((255, 0, 0), (255, 0, 0))  # Blue for border and text
        }
        
        # Thread-safe queues and lock
        self.frame_queue = deque(maxlen=1)
        self.result_queue = deque(maxlen=1)
        self.lock = threading.Lock()
        self.running = True
        
        # Try different camera initialization methods
        self.cap = self._init_camera()
        if not self.cap or not self.cap.isOpened():
            raise RuntimeError("Failed to open any camera device")
            
        # Get camera resolution
        self.frame_width = int(self.cap.get(cv2.CAP_PROP_FRAME_WIDTH))
        self.frame_height = int(self.cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
        
        # Performance statistics
        self.frame_count = 0
        self.start_time = time.time()
        self.avg_fps = 0
        self.avg_inference_time = 0
        self.inference_count = 0
        self.inference_start_time = time.time()
        
        # Detection region settings (centered) - expanded size
        roi_size = 300  # Detection region size
        self.detection_roi = {
            'x': max(0, (self.frame_width - roi_size) // 2),
            'y': max(0, (self.frame_height - roi_size) // 2),
            'width': min(roi_size, self.frame_width),
            'height': min(roi_size, self.frame_height)
        }

    def _init_camera(self):
        # Try different initialization methods
        camera_init_methods = [
            self._try_gstreamer_pipeline,
            self._try_direct_v4l2,
            self._try_default_camera
        ]
        
        for method in camera_init_methods:
            cap = method()
            if cap and cap.isOpened():
                print(f"Successfully initialized camera using {method.__name__}")
                return cap
        return None

    def _try_gstreamer_pipeline(self):
        try:
            # Try different GStreamer pipeline configurations
            gstreamer_configs = [
                "v4l2src device=/dev/video0 ! video/x-raw,width=640,height=480,framerate=30/1 ! videoconvert ! video/x-raw,format=BGR ! appsink drop=1",
                "v4l2src device=/dev/video0 ! image/jpeg,width=640,height=480,framerate=30/1 ! jpegdec ! videoconvert ! video/x-raw,format=BGR ! appsink",
                "autovideosrc ! videoconvert ! video/x-raw,format=BGR ! appsink"
            ]
            
            for config in gstreamer_configs:
                try:
                    cap = cv2.VideoCapture(config, cv2.CAP_GSTREAMER)
                    if cap.isOpened():
                        return cap
                except:
                    continue
        except Exception as e:
            print(f"GStreamer initialization failed: {e}")
        return None

    def _try_direct_v4l2(self):
        try:
            # Try direct V4L2 access
            cap = cv2.VideoCapture(0, cv2.CAP_V4L2)
            if cap.isOpened():
                cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc(*'MJPG'))
                cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
                cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
                cap.set(cv2.CAP_PROP_FPS, 30)
                return cap
        except Exception as e:
            print(f"V4L2 direct access failed: {e}")
        return None

    def _try_default_camera(self):
        try:
            # Fallback to default method
            cap = cv2.VideoCapture(0)
            if cap.isOpened():
                cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
                cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
                return cap
        except Exception as e:
            print(f"Default camera initialization failed: {e}")
        return None
    
    def preprocess(self, frame):
        # Extract ROI and resize to 96x96
        roi = frame[
            self.detection_roi['y']:self.detection_roi['y']+self.detection_roi['height'],
            self.detection_roi['x']:self.detection_roi['x']+self.detection_roi['width']
        ]
        roi = cv2.resize(roi, (96, 96))
        
        if self.is_quantized:
            # For quantized models: convert to RGB (uint8)
            return cv2.cvtColor(roi, cv2.COLOR_BGR2RGB)
        else:
            # For float models: normalize to [0,1] (matches training)
            roi = cv2.cvtColor(roi, cv2.COLOR_BGR2RGB).astype(np.float32) / 255.0
            return roi

    def draw_detection_roi(self, frame, label=None):
        # Default color (green)
        border_color = (0, 255, 0)
        text_color = (0, 255, 0)
        
        # Use class-specific color if label exists
        if label and label in self.class_colors:
            border_color, text_color = self.class_colors[label]
        
        # Draw detection region rectangle (thick border)
        cv2.rectangle(
            frame,
            (self.detection_roi['x'], self.detection_roi['y']),
            (self.detection_roi['x']+self.detection_roi['width'], 
             self.detection_roi['y']+self.detection_roi['height']),
            border_color,  # Use class-specific color
            3  # Thick border
        )
        
        return frame, text_color  # Return frame and text color

    def inference_worker(self):
        while self.running:
            try:
                frame = None
                with self.lock:
                    if self.frame_queue:
                        frame = self.frame_queue[-1]
                
                if frame is not None:
                    input_data = self.preprocess(frame)
                    
                    # Adjust input shape if needed
                    if len(input_data.shape) == 3:
                        input_data = np.expand_dims(input_data, axis=0)
                    
                    # Set input tensor
                    self.interpreter.set_tensor(self.input_details[0]['index'], input_data)
                    
                    # Run inference
                    start_time = time.time()
                    self.interpreter.invoke()
                    inference_time = time.time() - start_time
                    
                    # Get output
                    output_data = self.interpreter.get_tensor(self.output_details[0]['index'])
                    
                    # Calculate softmax probabilities
                    probabilities = np.exp(output_data) / np.sum(np.exp(output_data), axis=1, keepdims=True)
                    print("Class probabilities:", dict(zip(self.class_names, probabilities[0])))
                    
                    pred = np.argmax(output_data)
                    label = self.class_names[pred]
                    
                    # Update average inference time using exponential moving average
                    self.avg_inference_time = 0.9 * self.avg_inference_time + 0.1 * inference_time
                    
                    # Calculate inference FPS based on average inference time
                    inference_fps = 1.0 / max(self.avg_inference_time, 1e-5)
                    
                    # Update inference count for FPS calculation
                    self.inference_count += 1
                    if self.inference_count % 10 == 0:
                        elapsed = time.time() - self.inference_start_time
                        actual_inference_fps = 10.0 / max(elapsed, 1e-5)
                        self.inference_start_time = time.time()
                    
                    with self.lock:
                        self.result_queue.append((frame, label, inference_fps, self.avg_fps))
                else:
                    time.sleep(0.001)
            except Exception as e:
                print(f"Inference thread error: {e}")
                time.sleep(0.1)

    def run(self):
        inference_thread = threading.Thread(target=self.inference_worker, daemon=True)
        inference_thread.start()
        
        try:
            while True:
                ret, frame = self.cap.read()
                if not ret:
                    print("Camera read failed, reinitializing...")
                    time.sleep(1)
                    self.cap.release()
                    self.cap = self._init_camera()
                    if not self.cap or not self.cap.isOpened():
                        print("Failed to reinitialize camera")
                        break
                    continue
                
                # Default color (green)
                text_color = (0, 255, 0)
                
                if self.result_queue:
                    with self.lock:
                        display_frame, label, inference_fps, camera_fps = self.result_queue[-1]
                    
                    # Draw detection region and get text color
                    display_frame, text_color = self.draw_detection_roi(display_frame, label)
                    
                    # Calculate text positions
                    box_top = self.detection_roi['y']
                    box_bottom = self.detection_roi['y'] + self.detection_roi['height']
                    box_center_x = self.detection_roi['x'] + self.detection_roi['width'] // 2
                    
                    # Display FPS above detection box (use the same color as the border)
                    #fps_text = f"Infer FPS: {inference_fps:.1f} | Camera FPS: {camera_fps:.1f}"
                    fps_text = f"Infer FPS: {inference_fps:.1f}"
                    (fps_width, fps_height), _ = cv2.getTextSize(fps_text, cv2.FONT_HERSHEY_SIMPLEX, 0.7, 2)
                    fps_x = box_center_x - fps_width // 2
                    fps_y = box_top - 10
                    
                    # Use text_color (same as border) for FPS text
                    cv2.putText(display_frame, fps_text,
                               (fps_x, fps_y),
                               cv2.FONT_HERSHEY_SIMPLEX, 0.7, text_color, 2)
                    
                    # Display prediction below detection box (same color)
                    (label_width, label_height), _ = cv2.getTextSize(f"Pred: {label}", 
                                                                   cv2.FONT_HERSHEY_SIMPLEX, 0.9, 2)
                    label_x = box_center_x - label_width // 2
                    label_y = box_bottom + 30
                    
                    cv2.putText(display_frame, f"Pred: {label}",
                               (label_x, label_y),
                               cv2.FONT_HERSHEY_SIMPLEX, 0.9, text_color, 2)
                    
                    cv2.imshow("Camera Processing", display_frame)
                
                # Add current frame to queue
                with self.lock:
                    self.frame_queue.append(frame)
                
                # Update FPS statistics
                self.frame_count += 1
                if self.frame_count % 10 == 0:
                    elapsed = time.time() - self.start_time
                    self.avg_fps = 10.0 / max(elapsed, 1e-5)
                    self.start_time = time.time()
                
                # Exit on 'q' key press
                if cv2.waitKey(1) & 0xFF == ord('q'):
                    break
        except KeyboardInterrupt:
            print("Exiting...")
        finally:
            self.running = False
            inference_thread.join()
            if self.cap and self.cap.isOpened():
                self.cap.release()
            cv2.destroyAllWindows()
            print("Program exited")

if __name__ == "__main__":
    # Path to TFLite model
    MODEL_PATH = "/home/nvidia/Desktop/mobilenetv3_small_model_D.tflite"  
    CLASS_NAMES = ["100", "50", "50-100"]
    
    try:
        processor = CameraProcessor(MODEL_PATH, CLASS_NAMES)
        processor.run()
    except Exception as e:
        print(f"Program startup failed: {e}")
