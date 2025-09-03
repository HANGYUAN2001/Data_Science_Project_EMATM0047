#ifndef __spi_lcd
#define __spi_lcd

#include "stm32h7xx_hal.h"
#include "usart.h"

#include "lcd_fonts.h"	
#include "lcd_image.h"

#include <stdio.h>
#include <string.h>



#define LCD_Width     240		
#define LCD_Height    320		


#define	Direction_H				0				
#define	Direction_H_Flip	   1			
#define	Direction_V				2				
#define	Direction_V_Flip	   3					



#define  Fill_Zero  0		
#define  Fill_Space 1		




#define 	LCD_WHITE       0xFFFFFF	 
#define 	LCD_BLACK       0x000000    
                        
#define 	LCD_BLUE        0x0000FF	 
#define 	LCD_GREEN       0x00FF00    
#define 	LCD_RED         0xFF0000    
#define 	LCD_CYAN        0x00FFFF    
#define 	LCD_MAGENTA     0xFF00FF   
#define 	LCD_YELLOW      0xFFFF00  
#define 	LCD_GREY        0x2C2C2C   
												
#define 	LIGHT_BLUE      0x8080FF   
#define 	LIGHT_GREEN     0x80FF80    
#define 	LIGHT_RED       0xFF8080    
#define 	LIGHT_CYAN      0x80FFFF    
#define 	LIGHT_MAGENTA   0xFF80FF  
#define 	LIGHT_YELLOW    0xFFFF80   
#define 	LIGHT_GREY      0xA3A3A3   
												
#define 	DARK_BLUE       0x000080   
#define 	DARK_GREEN      0x008000   
#define 	DARK_RED        0x800000   
#define 	DARK_CYAN       0x008080    
#define 	DARK_MAGENTA    0x800080    
#define 	DARK_YELLOW     0x808000   
#define 	DARK_GREY       0x404040    

void  SPI_LCD_Init(void);      
void  LCD_Clear(void);			 
void  LCD_ClearRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height);	

void  LCD_SetAddress(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);	
void  LCD_SetColor(uint32_t Color); 				   
void  LCD_SetBackColor(uint32_t Color);  				
void  LCD_SetDirection(uint8_t direction);  	     


void  LCD_SetAsciiFont(pFONT *fonts);										
void 	LCD_DisplayChar(uint16_t x, uint16_t y,uint8_t c);				
void 	LCD_DisplayString( uint16_t x, uint16_t y, char *p);	 		


void 	LCD_SetTextFont(pFONT *fonts);									
void 	LCD_DisplayChinese(uint16_t x, uint16_t y, char *pText);		
void 	LCD_DisplayText(uint16_t x, uint16_t y, char *pText) ;	


void  LCD_ShowNumMode(uint8_t mode);		
void  LCD_DisplayNumber( uint16_t x, uint16_t y, int32_t number,uint8_t len) ;					
void  LCD_DisplayDecimals( uint16_t x, uint16_t y, double number,uint8_t len,uint8_t decs);	


void  LCD_DrawPoint(uint16_t x,uint16_t y,uint32_t color);   

void  LCD_DrawLine_V(uint16_t x, uint16_t y, uint16_t height);        
void  LCD_DrawLine_H(uint16_t x, uint16_t y, uint16_t width);         
void  LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);	

void  LCD_DrawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height);			
void  LCD_DrawCircle(uint16_t x, uint16_t y, uint16_t r);								
void  LCD_DrawEllipse(int x, int y, int r1, int r2);											


void  LCD_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height);			
void  LCD_FillCircle(uint16_t x, uint16_t y, uint16_t r);								


void 	LCD_DrawImage(uint16_t x,uint16_t y,uint16_t width,uint16_t height,const uint8_t *pImage)  ;


void	LCD_CopyBuffer(uint16_t x, uint16_t y,uint16_t width,uint16_t height,uint16_t *DataBuff);


void _LCD_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t * _LCD_Buff);
void _LCD_DisplayString(uint16_t x, uint16_t y, char *p, uint16_t * _LCD_Buff);
void _LCD_DrawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t * _LCD_Buff);
void _LCD_DrawImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t * _LCD_Buff, uint16_t *pImage);


#define  LCD_Backlight_PIN								GPIO_PIN_12				       
#define	LCD_Backlight_PORT							GPIOG				
#define 	GPIO_LDC_Backlight_CLK_ENABLE        	__HAL_RCC_GPIOG_CLK_ENABLE()

#define	LCD_Backlight_OFF		HAL_GPIO_WritePin(LCD_Backlight_PORT, LCD_Backlight_PIN, GPIO_PIN_RESET);	
#define 	LCD_Backlight_ON		HAL_GPIO_WritePin(LCD_Backlight_PORT, LCD_Backlight_PIN, GPIO_PIN_SET);		
 
#define  LCD_DC_PIN						GPIO_PIN_15				        
#define	LCD_DC_PORT						GPIOG								
#define 	GPIO_LDC_DC_CLK_ENABLE     __HAL_RCC_GPIOG_CLK_ENABLE()	

#define	LCD_DC_Command		   HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_RESET);	   
#define 	LCD_DC_Data		      HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);	

#endif //__spi_lcd




