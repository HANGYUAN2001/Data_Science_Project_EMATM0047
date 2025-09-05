#include "dcmi_ov5640.h"
#include "dcmi_ov5640_cfg.h"

DCMI_HandleTypeDef hdcmi;          
DMA_HandleTypeDef DMA_Handle_dcmi; 

volatile uint8_t OV5640_FrameState = 0; 
volatile uint8_t OV5640_FPS;


void HAL_DCMI_MspInit(DCMI_HandleTypeDef *hdcmi)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (hdcmi->Instance == DCMI)
    {
        __HAL_RCC_DCMI_CLK_ENABLE();

        __HAL_RCC_GPIOE_CLK_ENABLE(); 
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOD_CLK_ENABLE();
        __HAL_RCC_GPIOG_CLK_ENABLE();

        GPIO_OV5640_PWDN_CLK_ENABLE;



        GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
        HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

        
        OV5640_PWDN_ON; 

        GPIO_InitStruct.Pin = OV5640_PWDN_PIN;             
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;        
        GPIO_InitStruct.Pull = GPIO_PULLUP;                
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;       
        HAL_GPIO_Init(OV5640_PWDN_PORT, &GPIO_InitStruct); 
    }
}


void MX_DCMI_Init(void)
{
    hdcmi.Instance = DCMI;
    hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;    
    hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;  
    hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_LOW;      
    hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_LOW;      
    hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;      
    hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B; 
    hdcmi.Init.JPEGMode = DCMI_JPEG_DISABLE;           
    hdcmi.Init.ByteSelectMode = DCMI_BSM_ALL;          
    hdcmi.Init.ByteSelectStart = DCMI_OEBS_ODD;       
    hdcmi.Init.LineSelectMode = DCMI_LSM_ALL;          
    hdcmi.Init.LineSelectStart = DCMI_OELS_ODD;   
    HAL_DCMI_Init(&hdcmi);

    HAL_NVIC_SetPriority(DCMI_IRQn, 0, 5); 
    HAL_NVIC_EnableIRQ(DCMI_IRQn);         

}


void OV5640_DMA_Init(void)
{
    __HAL_RCC_DMA2_CLK_ENABLE(); 

    DMA_Handle_dcmi.Instance = DMA2_Stream7;                        
    DMA_Handle_dcmi.Init.Request = DMA_REQUEST_DCMI;                
    DMA_Handle_dcmi.Init.Direction = DMA_PERIPH_TO_MEMORY;          
    DMA_Handle_dcmi.Init.PeriphInc = DMA_PINC_DISABLE;             
    DMA_Handle_dcmi.Init.MemInc = DMA_MINC_ENABLE;                 
    DMA_Handle_dcmi.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD; 
    DMA_Handle_dcmi.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;    
    DMA_Handle_dcmi.Init.Mode = DMA_CIRCULAR;                       
    DMA_Handle_dcmi.Init.Priority = DMA_PRIORITY_LOW;             
    DMA_Handle_dcmi.Init.FIFOMode = DMA_FIFOMODE_ENABLE;           
    DMA_Handle_dcmi.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;   
    DMA_Handle_dcmi.Init.MemBurst = DMA_MBURST_SINGLE;              
    DMA_Handle_dcmi.Init.PeriphBurst = DMA_PBURST_SINGLE;      

    HAL_DMA_Init(&DMA_Handle_dcmi);                     
    __HAL_LINKDMA(&hdcmi, DMA_Handle, DMA_Handle_dcmi); 

    HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 0); 
    HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);         
}


void OV5640_Delay(uint32_t Delay)
{
    volatile uint16_t i;

    while (Delay--)
    {
        for (i = 0; i < 20000; i++)
            ;
    }

}


int8_t DCMI_OV5640_Init(void)
{
    uint16_t Device_ID; 

    SCCB_GPIO_Config();         
    MX_DCMI_Init();              
    OV5640_DMA_Init();          
    OV5640_Reset();            
    Device_ID = OV5640_ReadID(); 

    if (Device_ID == 0x5640) 
    {
        printf("OV5640 OK,ID:0x%X\r\n", Device_ID); 

        OV5640_Config();               
        OV5640_Set_Pixformat(Pixformat_RGB565);                                      
        OV5640_Set_Framesize(OV5640_Width, OV5640_Height);                           
        OV5640_DCMI_Crop(Display_Width, Display_Height, OV5640_Width, OV5640_Height); 

        return OV5640_Success; 
    }
    else
    {
        printf("OV5640 ERROR!!!!!  ID:%X\r\n", Device_ID); 
        return OV5640_Error;                             
    }
}

                           
void OV5640_DMA_Transmit_Continuous(uint32_t DMA_Buffer, uint32_t DMA_BufferSize)
{
    DMA_Handle_dcmi.Init.Mode = DMA_CIRCULAR; 

    HAL_DMA_Init(&DMA_Handle_dcmi); 

    HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)DMA_Buffer, DMA_BufferSize);
}


void OV5640_DMA_Transmit_Snapshot(uint32_t DMA_Buffer, uint32_t DMA_BufferSize)
{
    DMA_Handle_dcmi.Init.Mode = DMA_NORMAL;

    HAL_DMA_Init(&DMA_Handle_dcmi); 

    HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT, (uint32_t)DMA_Buffer, DMA_BufferSize);
}


void OV5640_DCMI_Suspend(void)
{
    HAL_DCMI_Suspend(&hdcmi); 
}


void OV5640_DCMI_Resume(void)
{
    (&hdcmi)->State = HAL_DCMI_STATE_BUSY;     
    (&hdcmi)->Instance->CR |= DCMI_CR_CAPTURE; 
}


void OV5640_DCMI_Stop(void)
{
    HAL_DCMI_Stop(&hdcmi);
}


int8_t OV5640_DCMI_Crop(uint16_t Displey_XSize, uint16_t Displey_YSize, uint16_t Sensor_XSize, uint16_t Sensor_YSize)
{
    uint16_t DCMI_X_Offset, DCMI_Y_Offset; 
    uint16_t DCMI_CAPCNT;               
    uint16_t DCMI_VLINE;        

    if ((Displey_XSize >= Sensor_XSize) || (Displey_YSize >= Sensor_YSize))
    {
       
        return OV5640_Error; 
    }

    DCMI_X_Offset = Sensor_XSize - Displey_XSize;

    DCMI_Y_Offset = (Sensor_YSize - Displey_YSize) / 2 - 1;

    DCMI_CAPCNT = Displey_XSize * 2 - 1;

    DCMI_VLINE = Displey_YSize - 1; 

    HAL_DCMI_ConfigCrop(&hdcmi, DCMI_X_Offset, DCMI_Y_Offset, DCMI_CAPCNT, DCMI_VLINE); 
    HAL_DCMI_EnableCrop(&hdcmi);                                                      

    return OV5640_Success;
}


void OV5640_Reset(void)
{
    OV5640_Delay(30); 

    OV5640_PWDN_OFF; 
	
    OV5640_Delay(5);

    OV5640_Delay(20);

    SCCB_WriteReg_16Bit(0x3103, 0x11);
    SCCB_WriteReg_16Bit(0x3008, 0x82); 
    OV5640_Delay(5);             
}


uint16_t OV5640_ReadID(void)
{
    uint8_t PID_H, PID_L; 

    PID_H = SCCB_ReadReg_16Bit(OV5640_ChipID_H); 
    PID_L = SCCB_ReadReg_16Bit(OV5640_ChipID_L); 

    return (PID_H << 8) | PID_L; 
}



void OV5640_Config(void)
{
    uint32_t i; 


    for (i = 0; i < (sizeof(OV5640_INIT_Config) / 4); i++)
    {
        SCCB_WriteReg_16Bit(OV5640_INIT_Config[i][0], OV5640_INIT_Config[i][1]); 
        OV5640_Delay(1);

    }
}


void OV5640_Set_Pixformat(uint8_t pixformat)
{
    uint8_t OV5640_Reg; 

    if (pixformat == Pixformat_JPEG)
    {
        SCCB_WriteReg_16Bit(OV5640_FORMAT_CONTROL, 0x30);     
        SCCB_WriteReg_16Bit(OV5640_FORMAT_CONTROL_MUX, 0x00); 

        SCCB_WriteReg_16Bit(OV5640_JPEG_MODE_SELECT, 0x02); 

        SCCB_WriteReg_16Bit(OV5640_JPEG_VFIFO_CTRL00, 0xA0); 

        SCCB_WriteReg_16Bit(OV5640_JPEG_VFIFO_HSIZE_H, OV5640_Width >> 8);      
        SCCB_WriteReg_16Bit(OV5640_JPEG_VFIFO_HSIZE_L, (uint8_t)OV5640_Width);  
        SCCB_WriteReg_16Bit(OV5640_JPEG_VFIFO_VSIZE_H, OV5640_Height >> 8); 
        SCCB_WriteReg_16Bit(OV5640_JPEG_VFIFO_VSIZE_L, (uint8_t)OV5640_Height); 
    }
    else if (pixformat == Pixformat_GRAY)
    {
        SCCB_WriteReg_16Bit(OV5640_FORMAT_CONTROL, 0x10);    
        SCCB_WriteReg_16Bit(OV5640_FORMAT_CONTROL_MUX, 0x00); 
    }
    else 
    {
        SCCB_WriteReg_16Bit(OV5640_FORMAT_CONTROL, 0x6F);     
        SCCB_WriteReg_16Bit(OV5640_FORMAT_CONTROL_MUX, 0x01); 
    }

    OV5640_Reg = SCCB_ReadReg_16Bit(0x3821); 
    SCCB_WriteReg_16Bit(0x3821, (OV5640_Reg & 0xDF) | ((pixformat == Pixformat_JPEG) ? 0x20 : 0x00));

    OV5640_Reg = SCCB_ReadReg_16Bit(0x3002);
    SCCB_WriteReg_16Bit(0x3002, (OV5640_Reg & 0xE3) | ((pixformat == Pixformat_JPEG) ? 0x00 : 0x1C));

    OV5640_Reg = SCCB_ReadReg_16Bit(0x3006); 
    SCCB_WriteReg_16Bit(0x3006, (OV5640_Reg & 0xD7) | ((pixformat == Pixformat_JPEG) ? 0x28 : 0x00));
}


void OV5640_Set_JPEG_QuantizationScale(uint8_t scale)
{
    SCCB_WriteReg_16Bit(0x4407, scale);
}


int8_t OV5640_Set_Framesize(uint16_t width, uint16_t height)
{
    
    SCCB_WriteReg_16Bit(OV5640_GroupAccess, 0X03);

    SCCB_WriteReg_16Bit(OV5640_TIMING_DVPHO_H, width >> 8); 
    SCCB_WriteReg_16Bit(OV5640_TIMING_DVPHO_L, width & 0xff);
    SCCB_WriteReg_16Bit(OV5640_TIMING_DVPVO_H, height >> 8); 
    SCCB_WriteReg_16Bit(OV5640_TIMING_DVPVO_L, height & 0xff);

    SCCB_WriteReg_16Bit(OV5640_GroupAccess, 0X13); 
    SCCB_WriteReg_16Bit(OV5640_GroupAccess, 0Xa3); 

    return OV5640_Success;
}


int8_t OV5640_Set_Horizontal_Mirror(int8_t ConfigState)
{
    uint8_t OV5640_Reg; 

    OV5640_Reg = SCCB_ReadReg_16Bit(OV5640_TIMING_Mirror); 


    if (ConfigState == OV5640_Enable) 
    {
        OV5640_Reg |= 0X06;
    }
    else 
    {
        OV5640_Reg &= 0xF9;
    }
    return SCCB_WriteReg_16Bit(OV5640_TIMING_Mirror, OV5640_Reg); 
}



int8_t OV5640_Set_Vertical_Flip(int8_t ConfigState)
{
    uint8_t OV5640_Reg; 

    OV5640_Reg = SCCB_ReadReg_16Bit(OV5640_TIMING_Flip); 


    if (ConfigState == OV5640_Enable)
    {
        OV5640_Reg |= 0X06;
    }
    else 
    {
        OV5640_Reg &= 0xF9;
    }
    return SCCB_WriteReg_16Bit(OV5640_TIMING_Flip, OV5640_Reg); 
}



void OV5640_Set_Brightness(int8_t Brightness)
{
    Brightness = Brightness + 4;
    SCCB_WriteReg_16Bit(OV5640_GroupAccess, 0X03); 

    SCCB_WriteReg_16Bit(0x5587, OV5640_Brightness_Config[Brightness][0]);
    SCCB_WriteReg_16Bit(0x5588, OV5640_Brightness_Config[Brightness][1]);

    SCCB_WriteReg_16Bit(OV5640_GroupAccess, 0X13); 
    SCCB_WriteReg_16Bit(OV5640_GroupAccess, 0Xa3); 
}


void OV5640_Set_Contrast(int8_t Contrast)
{
    Contrast = Contrast + 3;
    SCCB_WriteReg_16Bit(OV5640_GroupAccess, 0X03); 

    SCCB_WriteReg_16Bit(0x5586, OV5640_Contrast_Config[Contrast][0]);
    SCCB_WriteReg_16Bit(0x5585, OV5640_Contrast_Config[Contrast][1]);

    SCCB_WriteReg_16Bit(OV5640_GroupAccess, 0X13); 
    SCCB_WriteReg_16Bit(OV5640_GroupAccess, 0Xa3); 
}


void OV5640_Set_Effect(uint8_t effect_Mode)
{
    SCCB_WriteReg_16Bit(OV5640_GroupAccess, 0X03); 

    SCCB_WriteReg_16Bit(0x5580, OV5640_Effect_Config[effect_Mode][0]);
    SCCB_WriteReg_16Bit(0x5583, OV5640_Effect_Config[effect_Mode][1]);
    SCCB_WriteReg_16Bit(0x5584, OV5640_Effect_Config[effect_Mode][2]);
    SCCB_WriteReg_16Bit(0x5003, OV5640_Effect_Config[effect_Mode][3]);

    SCCB_WriteReg_16Bit(OV5640_GroupAccess, 0X13); 
    SCCB_WriteReg_16Bit(OV5640_GroupAccess, 0Xa3); 
}



int8_t OV5640_AF_Download_Firmware(void)
{
    uint8_t AF_Status = 0;             
    uint16_t i = 0;                    
    uint16_t OV5640_MCU_Addr = 0x8000; 

    SCCB_WriteReg_16Bit(0x3000, 0x20); 
                                       
    SCCB_WriteBuffer_16Bit(OV5640_MCU_Addr, (uint8_t *)OV5640_AF_Firmware, sizeof(OV5640_AF_Firmware));
    SCCB_WriteReg_16Bit(0x3000, 0x00); 

   
    for (i = 0; i < 100; i++)
    {
        AF_Status = SCCB_ReadReg_16Bit(OV5640_AF_FW_STATUS); 
        if (AF_Status == 0x7E)
        {
            printf("AF固件初始化中>>>\r\n");
        }
        if (AF_Status == 0x70) 
        {
            printf("AF固件写入成功！\r\n");
            return OV5640_Success;
        }
    }

    printf("自动对焦固件写入失败！！！error！！\r\n");
    return OV5640_Error;
}


int8_t OV5640_AF_QueryStatus(void)
{
    uint8_t AF_Status = 0; 

    AF_Status = SCCB_ReadReg_16Bit(OV5640_AF_FW_STATUS); 
    printf("AF_Status:0x%x\r\n", AF_Status);

    
    if ((AF_Status == 0x10) || (AF_Status == 0x20))
    {
        return OV5640_AF_End;
    }
    else
    {
        return OV5640_AF_Focusing; 
    }
}


                        
void OV5640_AF_Trigger_Constant(void)
{
    SCCB_WriteReg_16Bit(0x3022, 0x04); 
}



void OV5640_AF_Trigger_Single(void)
{
    SCCB_WriteReg_16Bit(OV5640_AF_CMD_MAIN, 0x03); 
}



void OV5640_AF_Release(void)
{
    SCCB_WriteReg_16Bit(OV5640_AF_CMD_MAIN, 0x08);
}



void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
    static uint32_t DCMI_Tick = 0;       
    static uint8_t DCMI_Frame_Count = 0; 
	
    if (HAL_GetTick() - DCMI_Tick >= 1000) 
    {
        DCMI_Tick = HAL_GetTick(); 

        OV5640_FPS = DCMI_Frame_Count; 

        DCMI_Frame_Count = 0; 
    }
    DCMI_Frame_Count++; 

    OV5640_FrameState = 1; 
}



void HAL_DCMI_ErrorCallback(DCMI_HandleTypeDef *hdcmi)
{

}

