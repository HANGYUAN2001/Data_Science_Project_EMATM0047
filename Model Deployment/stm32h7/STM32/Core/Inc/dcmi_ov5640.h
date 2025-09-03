#ifndef __DCMI_OV5640_H
#define __DCMI_OV5640_H

#include "main.h"
#include "sccb.h"
#include "usart.h"
#include "lcd_spi_200.h"

#include <stdio.h>


extern volatile uint8_t OV5640_FrameState; 
extern volatile uint8_t OV5640_FPS;        


#define Pixformat_RGB565 0
#define Pixformat_JPEG 1
#define Pixformat_GRAY 2

#define OV5640_AF_Focusing 2 
#define OV5640_AF_End 1      
#define OV5640_Success 0     
#define OV5640_Error -1      

#define OV5640_Enable 1
#define OV5640_Disable 0


#define OV5640_Effect_Normal 0   
#define OV5640_Effect_Negative 1 
#define OV5640_Effect_BW 2       
#define OV5640_Effect_Solarize 3 


#define OV5640_Width 440  
#define OV5640_Height 330 


#define Display_Width LCD_Width   
#define Display_Height LCD_Height 

#define Display_BufferSize Display_Width *Display_Height * 2 / 4 


#define OV5640_ChipID_H 0x300A 
#define OV5640_ChipID_L 0x300B 

#define OV5640_FORMAT_CONTROL 0x4300     
#define OV5640_FORMAT_CONTROL_MUX 0x501F 

#define OV5640_JPEG_MODE_SELECT 0x4713   
#define OV5640_JPEG_VFIFO_CTRL00 0x4600  
#define OV5640_JPEG_VFIFO_HSIZE_H 0x4602 
#define OV5640_JPEG_VFIFO_HSIZE_L 0x4603 
#define OV5640_JPEG_VFIFO_VSIZE_H 0x4604 
#define OV5640_JPEG_VFIFO_VSIZE_L 0x4605 

#define OV5640_GroupAccess 0X3212    
#define OV5640_TIMING_DVPHO_H 0x3808 
#define OV5640_TIMING_DVPHO_L 0x3809 
#define OV5640_TIMING_DVPVO_H 0x380A 
#define OV5640_TIMING_DVPVO_L 0x380B 
#define OV5640_TIMING_Flip 0x3820   
#define OV5640_TIMING_Mirror 0x3821  

#define OV5640_AF_CMD_MAIN 0x3022  
#define OV5640_AF_CMD_ACK 0x3023  
#define OV5640_AF_FW_STATUS 0x3029 


int8_t DCMI_OV5640_Init(void); 

void OV5640_DMA_Transmit_Continuous(uint32_t DMA_Buffer, uint32_t DMA_BufferSize);                                     
void OV5640_DMA_Transmit_Snapshot(uint32_t DMA_Buffer, uint32_t DMA_BufferSize);                                      
void OV5640_DCMI_Suspend(void);                                                                                        
void OV5640_DCMI_Resume(void);                                                                                       
void OV5640_DCMI_Stop(void);                                                                                           
int8_t OV5640_DCMI_Crop(uint16_t Displey_XSize, uint16_t Displey_YSize, uint16_t Sensor_XSize, uint16_t Sensor_YSize); 

void OV5640_Reset(void);      
uint16_t OV5640_ReadID(void);
void OV5640_Config(void);     

void OV5640_Set_Pixformat(uint8_t pixformat);                 
void OV5640_Set_JPEG_QuantizationScale(uint8_t scale);       
int8_t OV5640_Set_Framesize(uint16_t width, uint16_t height); 
int8_t OV5640_Set_Horizontal_Mirror(int8_t ConfigState);     
int8_t OV5640_Set_Vertical_Flip(int8_t ConfigState);         
void OV5640_Set_Brightness(int8_t Brightness);               
void OV5640_Set_Contrast(int8_t Contrast);                   
void OV5640_Set_Effect(uint8_t effect_Mode);                 

int8_t OV5640_AF_Download_Firmware(void); 
int8_t OV5640_AF_QueryStatus(void);      
void OV5640_AF_Trigger_Constant(void);    
void OV5640_AF_Trigger_Single(void);     
void OV5640_AF_Release(void);            

#define OV5640_PWDN_PIN GPIO_PIN_13                              
#define OV5640_PWDN_PORT GPIOF                                   
#define GPIO_OV5640_PWDN_CLK_ENABLE __HAL_RCC_GPIOF_CLK_ENABLE() 

#define OV5640_PWDN_OFF HAL_GPIO_WritePin(OV5640_PWDN_PORT, OV5640_PWDN_PIN, GPIO_PIN_RESET)

#define OV5640_PWDN_ON HAL_GPIO_WritePin(OV5640_PWDN_PORT, OV5640_PWDN_PIN, GPIO_PIN_SET)

#endif //__DCMI_OV5640_H
