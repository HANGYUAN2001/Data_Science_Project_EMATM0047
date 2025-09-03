#ifndef __CAMERA_SCCB_H
#define __CAMERA_SCCB_H

#include "stm32h7xx_hal.h"

#define OV2640_DEVICE_ADDRESS     0x60    
#define OV5640_DEVICE_ADDRESS     0X78	


#define SCCB_SCL_CLK_ENABLE     	__HAL_RCC_GPIOF_CLK_ENABLE()		
#define SCCB_SCL_PORT   			 GPIOF                 			
#define SCCB_SCL_PIN     			 GPIO_PIN_14  				
         
#define SCCB_SDA_CLK_ENABLE     	 __HAL_RCC_GPIOF_CLK_ENABLE() 	
#define SCCB_SDA_PORT   			 GPIOF           
#define SCCB_SDA_PIN    			 GPIO_PIN_15              			


#define ACK_OK  	1  			
#define ACK_ERR 	0  


#define SCCB_DelayVaule  15  	


#define SCCB_SCL(a)	if (a)	\
										HAL_GPIO_WritePin(SCCB_SCL_PORT, SCCB_SCL_PIN, GPIO_PIN_SET); \
									else		\
										HAL_GPIO_WritePin(SCCB_SCL_PORT, SCCB_SCL_PIN, GPIO_PIN_RESET)	

#define SCCB_SDA(a)	if (a)	\
										HAL_GPIO_WritePin(SCCB_SDA_PORT, SCCB_SDA_PIN, GPIO_PIN_SET); \
									else		\
										HAL_GPIO_WritePin(SCCB_SDA_PORT, SCCB_SDA_PIN, GPIO_PIN_RESET)		

				
void 		SCCB_GPIO_Config (void);			
void 		SCCB_Delay(uint32_t a);								
void 		SCCB_Start(void);							
void 		SCCB_Stop(void);				
void 		SCCB_ACK(void);					
void 		SCCB_NoACK(void);							
uint8_t 	SCCB_WaitACK(void);					
uint8_t	SCCB_WriteByte(uint8_t IIC_Data); 
uint8_t 	SCCB_ReadByte(uint8_t ACK_Mode);		
		
uint8_t  SCCB_WriteReg (uint8_t addr,uint8_t value);     	
uint8_t  SCCB_ReadReg (uint8_t addr);                    	
									
uint8_t 	SCCB_WriteReg_16Bit(uint16_t addr,uint8_t value);								
uint8_t 	SCCB_ReadReg_16Bit (uint16_t addr);						
uint8_t 	SCCB_WriteBuffer_16Bit(uint16_t addr,uint8_t *pData, uint32_t size);	
									
#endif //__CAMERA_SCCB_H
