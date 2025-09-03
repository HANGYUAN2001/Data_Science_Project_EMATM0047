#include "lcd_spi_200.h"

SPI_HandleTypeDef hspi6; 

#define LCD_SPI hspi6 

static pFONT *LCD_AsciiFonts;
static pFONT *LCD_CHFonts;   


uint16_t LCD_Buff[1024]; 

struct 
{
    uint32_t Color;       
    uint32_t BackColor;  
    uint8_t ShowNum_Mode; 
    uint8_t Direction;    
    uint16_t Width;      
    uint16_t Height;    
    uint8_t X_Offset;    
    uint8_t Y_Offset;    
} LCD;


HAL_StatusTypeDef LCD_SPI_Transmit(SPI_HandleTypeDef *hspi, uint16_t pData, uint32_t Size);
HAL_StatusTypeDef LCD_SPI_TransmitBuffer(SPI_HandleTypeDef *hspi, uint16_t *pData, uint32_t Size);


void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (hspi->Instance == SPI6)
    {
        __HAL_RCC_SPI6_CLK_ENABLE(); 

        __HAL_RCC_GPIOG_CLK_ENABLE();  
        GPIO_LDC_Backlight_CLK_ENABLE; 
        GPIO_LDC_DC_CLK_ENABLE;       



        GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_13 | GPIO_PIN_14;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;            
        GPIO_InitStruct.Pull = GPIO_NOPULL;               
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; 
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI6;        
        HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

   
        GPIO_InitStruct.Pin = LCD_Backlight_PIN;            
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;        
        GPIO_InitStruct.Pull = GPIO_PULLDOWN;               
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;        
        HAL_GPIO_Init(LCD_Backlight_PORT, &GPIO_InitStruct); 


        GPIO_InitStruct.Pin = LCD_DC_PIN;             
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   
        GPIO_InitStruct.Pull = GPIO_NOPULL;           
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;  
        HAL_GPIO_Init(LCD_DC_PORT, &GPIO_InitStruct); 
    }
}



void MX_SPI6_Init(void)
{
    LCD_SPI.Instance = SPI6;                      
    LCD_SPI.Init.Mode = SPI_MODE_MASTER;          
    LCD_SPI.Init.Direction = SPI_DIRECTION_1LINE; 
    LCD_SPI.Init.DataSize = SPI_DATASIZE_8BIT;    
    LCD_SPI.Init.CLKPolarity = SPI_POLARITY_LOW;  
    LCD_SPI.Init.CLKPhase = SPI_PHASE_1EDGE;      
    LCD_SPI.Init.NSS = SPI_NSS_HARD_OUTPUT;     

   
    LCD_SPI.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;

    LCD_SPI.Init.FirstBit = SPI_FIRSTBIT_MSB;                                          
    LCD_SPI.Init.TIMode = SPI_TIMODE_DISABLE;                                          
    LCD_SPI.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;                          
    LCD_SPI.Init.CRCPolynomial = 0x0;                                               
    LCD_SPI.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;                                    
    LCD_SPI.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;                                  
    LCD_SPI.Init.FifoThreshold = SPI_FIFO_THRESHOLD_02DATA;                            
    LCD_SPI.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN; 
    LCD_SPI.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN; 
    LCD_SPI.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;                    
    LCD_SPI.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;     
    LCD_SPI.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;             
    LCD_SPI.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;                
    LCD_SPI.Init.IOSwap = SPI_IO_SWAP_DISABLE;                                     

    HAL_SPI_Init(&LCD_SPI);
}



void LCD_WriteCommand(uint8_t lcd_command)
{
    LCD_DC_Command; 

    HAL_SPI_Transmit(&LCD_SPI, &lcd_command, 1, 1000);
}


void LCD_WriteData_8bit(uint8_t lcd_data)
{
    LCD_DC_Data; 

    HAL_SPI_Transmit(&LCD_SPI, &lcd_data, 1, 1000); 
}


void LCD_WriteData_16bit(uint16_t lcd_data)
{
    uint8_t lcd_data_buff[2]; 
    LCD_DC_Data;              

    lcd_data_buff[0] = lcd_data >> 8; 
    lcd_data_buff[1] = lcd_data;

    HAL_SPI_Transmit(&LCD_SPI, lcd_data_buff, 2, 1000); 
}


void LCD_WriteBuff(uint16_t *DataBuff, uint16_t DataSize)
{
    LCD_DC_Data; 

  
    LCD_SPI.Init.DataSize = SPI_DATASIZE_16BIT;
    HAL_SPI_Init(&LCD_SPI);

    HAL_SPI_Transmit(&LCD_SPI, (uint8_t *)DataBuff, DataSize, 1000); 


    LCD_SPI.Init.DataSize = SPI_DATASIZE_8BIT; 
    HAL_SPI_Init(&LCD_SPI);
}


void SPI_LCD_Init(void)
{
    MX_SPI6_Init(); 

    HAL_Delay(10);           
    LCD_WriteCommand(0x36);   
    LCD_WriteData_8bit(0x00); 

    LCD_WriteCommand(0x3A);   
    LCD_WriteData_8bit(0x05); 

	
    LCD_WriteCommand(0xB2);
    LCD_WriteData_8bit(0x0C);
    LCD_WriteData_8bit(0x0C);
    LCD_WriteData_8bit(0x00);
    LCD_WriteData_8bit(0x33);
    LCD_WriteData_8bit(0x33);

    LCD_WriteCommand(0xB7);  
    LCD_WriteData_8bit(0x35); 

    LCD_WriteCommand(0xBB);  
    LCD_WriteData_8bit(0x19); 

    LCD_WriteCommand(0xC0);
    LCD_WriteData_8bit(0x2C);

    LCD_WriteCommand(0xC2);  
    LCD_WriteData_8bit(0x01); 

    LCD_WriteCommand(0xC3);   
    LCD_WriteData_8bit(0x12); 

    LCD_WriteCommand(0xC4);   
    LCD_WriteData_8bit(0x20); 

    LCD_WriteCommand(0xC6);   
    LCD_WriteData_8bit(0x0F); 

    LCD_WriteCommand(0xD0);   
    LCD_WriteData_8bit(0xA4); 
    LCD_WriteData_8bit(0xA1); 

    LCD_WriteCommand(0xE0); 
    LCD_WriteData_8bit(0xD0);
    LCD_WriteData_8bit(0x04);
    LCD_WriteData_8bit(0x0D);
    LCD_WriteData_8bit(0x11);
    LCD_WriteData_8bit(0x13);
    LCD_WriteData_8bit(0x2B);
    LCD_WriteData_8bit(0x3F);
    LCD_WriteData_8bit(0x54);
    LCD_WriteData_8bit(0x4C);
    LCD_WriteData_8bit(0x18);
    LCD_WriteData_8bit(0x0D);
    LCD_WriteData_8bit(0x0B);
    LCD_WriteData_8bit(0x1F);
    LCD_WriteData_8bit(0x23);

    LCD_WriteCommand(0xE1); 
    LCD_WriteData_8bit(0xD0);
    LCD_WriteData_8bit(0x04);
    LCD_WriteData_8bit(0x0C);
    LCD_WriteData_8bit(0x11);
    LCD_WriteData_8bit(0x13);
    LCD_WriteData_8bit(0x2C);
    LCD_WriteData_8bit(0x3F);
    LCD_WriteData_8bit(0x44);
    LCD_WriteData_8bit(0x51);
    LCD_WriteData_8bit(0x2F);
    LCD_WriteData_8bit(0x1F);
    LCD_WriteData_8bit(0x1F);
    LCD_WriteData_8bit(0x20);
    LCD_WriteData_8bit(0x23);

    LCD_WriteCommand(0x21); 

    
    LCD_WriteCommand(0x11); 
    HAL_Delay(120);        

    LCD_WriteCommand(0x29); 


    LCD_SetDirection(Direction_V); 
    LCD_SetBackColor(LCD_BLACK);  
    LCD_SetColor(LCD_WHITE);      
    LCD_Clear();             

    LCD_SetAsciiFont(&ASCII_Font24); 
    LCD_ShowNumMode(Fill_Zero);     


    LCD_Backlight_ON; 
}


void LCD_SetAddress(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    LCD_WriteCommand(0x2a); 
    LCD_WriteData_16bit(x1 + LCD.X_Offset);
    LCD_WriteData_16bit(x2 + LCD.X_Offset);

    LCD_WriteCommand(0x2b); 
    LCD_WriteData_16bit(y1 + LCD.Y_Offset);
    LCD_WriteData_16bit(y2 + LCD.Y_Offset);

    LCD_WriteCommand(0x2c); 
}



void LCD_SetColor(uint32_t Color)
{
    uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0; 

    Red_Value = (uint16_t)((Color & 0x00F80000) >> 8); 
    Green_Value = (uint16_t)((Color & 0x0000FC00) >> 5);
    Blue_Value = (uint16_t)((Color & 0x000000F8) >> 3);

    LCD.Color = (uint16_t)(Red_Value | Green_Value | Blue_Value); 
}



void LCD_SetBackColor(uint32_t Color)
{
    uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0; 

    Red_Value = (uint16_t)((Color & 0x00F80000) >> 8); 
    Green_Value = (uint16_t)((Color & 0x0000FC00) >> 5);
    Blue_Value = (uint16_t)((Color & 0x000000F8) >> 3);

    LCD.BackColor = (uint16_t)(Red_Value | Green_Value | Blue_Value); 
}



void LCD_SetDirection(uint8_t direction)
{
    LCD.Direction = direction; 

    if (direction == Direction_H) 
    {
        LCD_WriteCommand(0x36);   
        LCD_WriteData_8bit(0x70); 
        LCD.X_Offset = 0;         
        LCD.Y_Offset = 0;
        LCD.Width = LCD_Height; 
        LCD.Height = LCD_Width;
    }
    else if (direction == Direction_V)
    {
        LCD_WriteCommand(0x36);   
        LCD_WriteData_8bit(0x00); 
        LCD.X_Offset = 0;        
        LCD.Y_Offset = 0;
        LCD.Width = LCD_Width; 
        LCD.Height = LCD_Height;
    }
    else if (direction == Direction_H_Flip)
    {
        LCD_WriteCommand(0x36);   
        LCD_WriteData_8bit(0xA0); 
        LCD.X_Offset = 0;         
        LCD.Y_Offset = 0;
        LCD.Width = LCD_Height; 
        LCD.Height = LCD_Width;
    }
    else if (direction == Direction_V_Flip)
    {
        LCD_WriteCommand(0x36);   
        LCD_WriteData_8bit(0xC0); 
        LCD.X_Offset = 0;         
        LCD.Y_Offset = 0;
        LCD.Width = LCD_Width; 
        LCD.Height = LCD_Height;
    }
}



void LCD_SetAsciiFont(pFONT *Asciifonts)
{
    LCD_AsciiFonts = Asciifonts;
}



void LCD_Clear(void)
{
    LCD_SetAddress(0, 0, LCD.Width - 1, LCD.Height - 1); 

    LCD_DC_Data; 

   
    LCD_SPI.Init.DataSize = SPI_DATASIZE_16BIT; 
    HAL_SPI_Init(&LCD_SPI);

    LCD_SPI_Transmit(&LCD_SPI, LCD.BackColor, LCD.Width * LCD.Height); 

    LCD_SPI.Init.DataSize = SPI_DATASIZE_8BIT; 
    HAL_SPI_Init(&LCD_SPI);
}



void LCD_ClearRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    LCD_SetAddress(x, y, x + width - 1, y + height - 1); 

    LCD_DC_Data; 

    
    LCD_SPI.Init.DataSize = SPI_DATASIZE_16BIT; 
    HAL_SPI_Init(&LCD_SPI);

    LCD_SPI_Transmit(&LCD_SPI, LCD.BackColor, width * height); 


    LCD_SPI.Init.DataSize = SPI_DATASIZE_8BIT; 
    HAL_SPI_Init(&LCD_SPI);
}


void LCD_DrawPoint(uint16_t x, uint16_t y, uint32_t color)
{
    LCD_SetAddress(x, y, x, y); 

    LCD_WriteData_16bit(color);
}



void LCD_DisplayChar(uint16_t x, uint16_t y, uint8_t c)
{
    uint16_t index = 0, counter = 0, i = 0, w = 0; 
    uint8_t disChar;                            

    c = c - 32; 

    for (index = 0; index < LCD_AsciiFonts->Sizes; index++)
    {
        disChar = LCD_AsciiFonts->pTable[c * LCD_AsciiFonts->Sizes + index]; 
        for (counter = 0; counter < 8; counter++)
        {
            if (disChar & 0x01)
            {
                LCD_Buff[i] = LCD.Color; 
            }
            else
            {
                LCD_Buff[i] = LCD.BackColor; 
            }
            disChar >>= 1;
            i++;
            w++;
            if (w == LCD_AsciiFonts->Width) 
            {                        
                w = 0;
                break;
            }
        }
    }
    LCD_SetAddress(x, y, x + LCD_AsciiFonts->Width - 1, y + LCD_AsciiFonts->Height - 1); 
    LCD_WriteBuff(LCD_Buff, LCD_AsciiFonts->Width * LCD_AsciiFonts->Height);          
}



void LCD_DisplayString(uint16_t x, uint16_t y, char *p)
{
    while ((x < LCD.Width) && (*p != 0)) 
    {
        LCD_DisplayChar(x, y, *p);
        x += LCD_AsciiFonts->Width; 
        p++;                     
    }
}



void LCD_SetTextFont(pFONT *fonts)
{
    LCD_CHFonts = fonts; 
    switch (fonts->Width)
    {
    case 12:
        LCD_AsciiFonts = &ASCII_Font12;
        break; 
    case 16:
        LCD_AsciiFonts = &ASCII_Font16;
        break; 
    case 20:
        LCD_AsciiFonts = &ASCII_Font20;
        break; 
    case 24:
        LCD_AsciiFonts = &ASCII_Font24;
        break; 
    case 32:
        LCD_AsciiFonts = &ASCII_Font32;
        break; 
    default:
        break;
    }
}


void LCD_DisplayChinese(uint16_t x, uint16_t y, char *pText)
{
    uint16_t i = 0, index = 0, counter = 0; 
    uint16_t addr;                        
    uint8_t disChar;                        
    uint16_t Xaddress = 0;          

    while (1)
    {
        
        if (*(LCD_CHFonts->pTable + (i + 1) * LCD_CHFonts->Sizes + 0) == *pText && *(LCD_CHFonts->pTable + (i + 1) * LCD_CHFonts->Sizes + 1) == *(pText + 1))
        {
            addr = i; 
            break;
        }
        i += 2; 

        if (i >= LCD_CHFonts->Table_Rows)
            break; 
    }
    i = 0;
    for (index = 0; index < LCD_CHFonts->Sizes; index++)
    {
        disChar = *(LCD_CHFonts->pTable + (addr)*LCD_CHFonts->Sizes + index); 

        for (counter = 0; counter < 8; counter++)
        {
            if (disChar & 0x01)
            {
                LCD_Buff[i] = LCD.Color; 
            }
            else
            {
                LCD_Buff[i] = LCD.BackColor; 
            }
            i++;
            disChar >>= 1;
            Xaddress++;

            if (Xaddress == LCD_CHFonts->Width)
            {                                 
                Xaddress = 0;
                break;
            }
        }
    }
    LCD_SetAddress(x, y, x + LCD_CHFonts->Width - 1, y + LCD_CHFonts->Height - 1); 
    LCD_WriteBuff(LCD_Buff, LCD_CHFonts->Width * LCD_CHFonts->Height);          
}



void LCD_DisplayText(uint16_t x, uint16_t y, char *pText)
{

    while (*pText != 0)
    {
        if (*pText <= 0x7F) 
        {
            LCD_DisplayChar(x, y, *pText);
            x += LCD_AsciiFonts->Width;   
            pText++;                
        }
        else 
        {
            LCD_DisplayChinese(x, y, pText); 
            x += LCD_CHFonts->Width;        
            pText += 2;                   
        }
    }
}


void LCD_ShowNumMode(uint8_t mode)
{
    LCD.ShowNum_Mode = mode;
}



void LCD_DisplayNumber(uint16_t x, uint16_t y, int32_t number, uint8_t len)
{
    char Number_Buffer[15]; 

    if (LCD.ShowNum_Mode == Fill_Zero) 
    {
        sprintf(Number_Buffer, "%0.*d", len, number); 
    }
    else 
    {
        sprintf(Number_Buffer, "%*d", len, number); 
    }

    LCD_DisplayString(x, y, (char *)Number_Buffer); 
}



void LCD_DisplayDecimals(uint16_t x, uint16_t y, double decimals, uint8_t len, uint8_t decs)
{
    char Number_Buffer[20];

    if (LCD.ShowNum_Mode == Fill_Zero)
    {
        sprintf(Number_Buffer, "%0*.*lf", len, decs, decimals); 
    }
    else 
    {
        sprintf(Number_Buffer, "%*.*lf", len, decs, decimals); 
    }

    LCD_DisplayString(x, y, (char *)Number_Buffer); 
}



#define ABS(X) ((X) > 0 ? (X) : -(X))

void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
            yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
            curpixel = 0;

    deltax = ABS(x2 - x1); /* The difference between the x's */
    deltay = ABS(y2 - y1); /* The difference between the y's */
    x = x1;                /* Start x off at the first pixel */
    y = y1;                /* Start y off at the first pixel */

    if (x2 >= x1) /* The x-values are increasing */
    {
        xinc1 = 1;
        xinc2 = 1;
    }
    else /* The x-values are decreasing */
    {
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1) /* The y-values are increasing */
    {
        yinc1 = 1;
        yinc2 = 1;
    }
    else /* The y-values are decreasing */
    {
        yinc1 = -1;
        yinc2 = -1;
    }

    if (deltax >= deltay) /* There is at least one x-value for every y-value */
    {
        xinc1 = 0; /* Don't change the x when numerator >= denominator */
        yinc2 = 0; /* Don't change the y for every iteration */
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax; /* There are more x-values than y-values */
    }
    else /* There is at least one y-value for every x-value */
    {
        xinc2 = 0; /* Don't change the x for every iteration */
        yinc1 = 0; /* Don't change the y when numerator >= denominator */
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay; /* There are more y-values than x-values */
    }
    for (curpixel = 0; curpixel <= numpixels; curpixel++)
    {
        LCD_DrawPoint(x, y, LCD.Color); /* Draw the current pixel */
        num += numadd;                  /* Increase the numerator by the top of the fraction */
        if (num >= den)                 /* Check if numerator >= denominator */
        {
            num -= den; /* Calculate the new numerator value */
            x += xinc1; /* Change the x as appropriate */
            y += yinc1; /* Change the y as appropriate */
        }
        x += xinc2; /* Change the x as appropriate */
        y += yinc2; /* Change the y as appropriate */
    }
}


void LCD_DrawLine_V(uint16_t x, uint16_t y, uint16_t height)
{
    uint16_t i; 

    for (i = 0; i < height; i++)
    {
        LCD_Buff[i] = LCD.Color; 
    }
    LCD_SetAddress(x, y, x, y + height - 1); 

    LCD_WriteBuff(LCD_Buff, height); 
}


void LCD_DrawLine_H(uint16_t x, uint16_t y, uint16_t width)
{
    uint16_t i; 

    for (i = 0; i < width; i++)
    {
        LCD_Buff[i] = LCD.Color; 
    }
    LCD_SetAddress(x, y, x + width - 1, y); 

    LCD_WriteBuff(LCD_Buff, width); 
}


void LCD_DrawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    
    LCD_DrawLine_H(x, y, width);
    LCD_DrawLine_H(x, y + height - 1, width);

    
    LCD_DrawLine_V(x, y, height);
    LCD_DrawLine_V(x + width - 1, y, height);
}



void LCD_DrawCircle(uint16_t x, uint16_t y, uint16_t r)
{
    int Xadd = -r, Yadd = 0, err = 2 - 2 * r, e2;
    do
    {

        LCD_DrawPoint(x - Xadd, y + Yadd, LCD.Color);
        LCD_DrawPoint(x + Xadd, y + Yadd, LCD.Color);
        LCD_DrawPoint(x + Xadd, y - Yadd, LCD.Color);
        LCD_DrawPoint(x - Xadd, y - Yadd, LCD.Color);

        e2 = err;
        if (e2 <= Yadd)
        {
            err += ++Yadd * 2 + 1;
            if (-Xadd == Yadd && e2 <= Xadd)
                e2 = 0;
        }
        if (e2 > Xadd)
            err += ++Xadd * 2 + 1;
    } while (Xadd <= 0);
}



void LCD_DrawEllipse(int x, int y, int r1, int r2)
{
    int Xadd = -r1, Yadd = 0, err = 2 - 2 * r1, e2;
    float K = 0, rad1 = 0, rad2 = 0;

    rad1 = r1;
    rad2 = r2;

    if (r1 > r2)
    {
        do
        {
            K = (float)(rad1 / rad2);

            LCD_DrawPoint(x - Xadd, y + (uint16_t)(Yadd / K), LCD.Color);
            LCD_DrawPoint(x + Xadd, y + (uint16_t)(Yadd / K), LCD.Color);
            LCD_DrawPoint(x + Xadd, y - (uint16_t)(Yadd / K), LCD.Color);
            LCD_DrawPoint(x - Xadd, y - (uint16_t)(Yadd / K), LCD.Color);

            e2 = err;
            if (e2 <= Yadd)
            {
                err += ++Yadd * 2 + 1;
                if (-Xadd == Yadd && e2 <= Xadd)
                    e2 = 0;
            }
            if (e2 > Xadd)
                err += ++Xadd * 2 + 1;
        } while (Xadd <= 0);
    }
    else
    {
        Yadd = -r2;
        Xadd = 0;
        do
        {
            K = (float)(rad2 / rad1);

            LCD_DrawPoint(x - (uint16_t)(Xadd / K), y + Yadd, LCD.Color);
            LCD_DrawPoint(x + (uint16_t)(Xadd / K), y + Yadd, LCD.Color);
            LCD_DrawPoint(x + (uint16_t)(Xadd / K), y - Yadd, LCD.Color);
            LCD_DrawPoint(x - (uint16_t)(Xadd / K), y - Yadd, LCD.Color);

            e2 = err;
            if (e2 <= Xadd)
            {
                err += ++Xadd * 3 + 1;
                if (-Yadd == Xadd && e2 <= Yadd)
                    e2 = 0;
            }
            if (e2 > Yadd)
                err += ++Yadd * 3 + 1;
        } while (Yadd <= 0);
    }
}



void LCD_FillCircle(uint16_t x, uint16_t y, uint16_t r)
{
    int32_t D;     /* Decision Variable */
    uint32_t CurX; /* Current X Value */
    uint32_t CurY; /* Current Y Value */

    D = 3 - (r << 1);

    CurX = 0;
    CurY = r;

    while (CurX <= CurY)
    {
        if (CurY > 0)
        {
            LCD_DrawLine_V(x - CurX, y - CurY, 2 * CurY);
            LCD_DrawLine_V(x + CurX, y - CurY, 2 * CurY);
        }

        if (CurX > 0)
        {
            // LCD_DrawLine(x - CurY, y - CurX,x - CurY,y - CurX + 2*CurX);
            // LCD_DrawLine(x + CurY, y - CurX,x + CurY,y - CurX + 2*CurX);

            LCD_DrawLine_V(x - CurY, y - CurX, 2 * CurX);
            LCD_DrawLine_V(x + CurY, y - CurX, 2 * CurX);
        }
        if (D < 0)
        {
            D += (CurX << 2) + 6;
        }
        else
        {
            D += ((CurX - CurY) << 2) + 10;
            CurY--;
        }
        CurX++;
    }
    LCD_DrawCircle(x, y, r);
}



void LCD_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    LCD_SetAddress(x, y, x + width - 1, y + height - 1); 

    LCD_DC_Data; 

    
    LCD_SPI.Init.DataSize = SPI_DATASIZE_16BIT; 
    HAL_SPI_Init(&LCD_SPI);

    LCD_SPI_Transmit(&LCD_SPI, LCD.Color, width * height);

  
    LCD_SPI.Init.DataSize = SPI_DATASIZE_8BIT; 
    HAL_SPI_Init(&LCD_SPI);
}



void LCD_DrawImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *pImage)
{
    uint8_t disChar;              
    uint16_t Xaddress = x;        
    uint16_t Yaddress = y;      
    uint16_t i = 0, j = 0, m = 0; 
    uint16_t BuffCount = 0;      
    uint16_t Buff_Height = 0;    

  
    Buff_Height = (sizeof(LCD_Buff) / 2) / height; 

    for (i = 0; i < height; i++) 
    {
        for (j = 0; j < (float)width / 8; j++)
        {
            disChar = *pImage;

            for (m = 0; m < 8; m++)
            {
                if (disChar & 0x01)
                {
                    LCD_Buff[BuffCount] = LCD.Color; 	
                }
                else
                {
                    LCD_Buff[BuffCount] = LCD.BackColor; 
                }
                disChar >>= 1;               
                Xaddress++;                  
                BuffCount++;                 
                if ((Xaddress - x) == width) 
                {
                    Xaddress = x;
                    break;
                }
            }
            pImage++;
        }
        if (BuffCount == Buff_Height * width) 
        {
            BuffCount = 0; 

            LCD_SetAddress(x, Yaddress, x + width - 1, Yaddress + Buff_Height - 1); 
            LCD_WriteBuff(LCD_Buff, width * Buff_Height);                        

            Yaddress = Yaddress + Buff_Height; 
        }
        if ((i + 1) == height) 
        {
            LCD_SetAddress(x, Yaddress, x + width - 1, i + y);      
            LCD_WriteBuff(LCD_Buff, width * (i + 1 + y - Yaddress));
        }
    }
}


void LCD_CopyBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t *DataBuff)
{

    LCD_SetAddress(x, y, x + width - 1, y + height - 1);

    LCD_DC_Data; 


    LCD_SPI.Init.DataSize = SPI_DATASIZE_16BIT; 
    HAL_SPI_Init(&LCD_SPI);

    LCD_SPI_TransmitBuffer(&LCD_SPI, DataBuff, width * height);

    LCD_SPI.Init.DataSize = SPI_DATASIZE_8BIT;
    HAL_SPI_Init(&LCD_SPI);
}



HAL_StatusTypeDef MY_SPI_WaitOnFlagUntilTimeout(SPI_HandleTypeDef *hspi, uint32_t Flag, FlagStatus Status,
                                                uint32_t Tickstart, uint32_t Timeout)
{
    /* Wait until flag is set */
    while ((__HAL_SPI_GET_FLAG(hspi, Flag) ? SET : RESET) == Status)
    {
        /* Check for the Timeout */
        if ((((HAL_GetTick() - Tickstart) >= Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
        {
            return HAL_TIMEOUT;
        }
    }
    return HAL_OK;
}

/**
 * @brief  Close Transfer and clear flags.
 * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
 *               the configuration information for SPI module.
 * @retval HAL_ERROR: if any error detected
 *         HAL_OK: if nothing detected
 */
void MY_SPI_CloseTransfer(SPI_HandleTypeDef *hspi)
{
    uint32_t itflag = hspi->Instance->SR;

    __HAL_SPI_CLEAR_EOTFLAG(hspi);
    __HAL_SPI_CLEAR_TXTFFLAG(hspi);

    /* Disable SPI peripheral */
    __HAL_SPI_DISABLE(hspi);

    /* Disable ITs */
    __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_EOT | SPI_IT_TXP | SPI_IT_RXP | SPI_IT_DXP | SPI_IT_UDR | SPI_IT_OVR | SPI_IT_FRE | SPI_IT_MODF));

    /* Disable Tx DMA Request */
    CLEAR_BIT(hspi->Instance->CFG1, SPI_CFG1_TXDMAEN | SPI_CFG1_RXDMAEN);

    /* Report UnderRun error for non RX Only communication */
    if (hspi->State != HAL_SPI_STATE_BUSY_RX)
    {
        if ((itflag & SPI_FLAG_UDR) != 0UL)
        {
            SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_UDR);
            __HAL_SPI_CLEAR_UDRFLAG(hspi);
        }
    }

    /* Report OverRun error for non TX Only communication */
    if (hspi->State != HAL_SPI_STATE_BUSY_TX)
    {
        if ((itflag & SPI_FLAG_OVR) != 0UL)
        {
            SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_OVR);
            __HAL_SPI_CLEAR_OVRFLAG(hspi);
        }
    }

    /* SPI Mode Fault error interrupt occurred -------------------------------*/
    if ((itflag & SPI_FLAG_MODF) != 0UL)
    {
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_MODF);
        __HAL_SPI_CLEAR_MODFFLAG(hspi);
    }

    /* SPI Frame error interrupt occurred ------------------------------------*/
    if ((itflag & SPI_FLAG_FRE) != 0UL)
    {
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FRE);
        __HAL_SPI_CLEAR_FREFLAG(hspi);
    }

    hspi->TxXferCount = (uint16_t)0UL;
    hspi->RxXferCount = (uint16_t)0UL;
}



HAL_StatusTypeDef LCD_SPI_Transmit(SPI_HandleTypeDef *hspi, uint16_t pData, uint32_t Size)
{
    uint32_t tickstart;
    uint32_t Timeout = 1000;  
    uint32_t LCD_pData_32bit; 
    uint32_t LCD_TxDataCount; 
    HAL_StatusTypeDef errorcode = HAL_OK;

    /* Check Direction parameter */
    assert_param(IS_SPI_DIRECTION_2LINES_OR_1LINE_2LINES_TXONLY(hspi->Init.Direction));

    /* Process Locked */
    __HAL_LOCK(hspi);

    /* Init tickstart for timeout management*/
    tickstart = HAL_GetTick();

    if (hspi->State != HAL_SPI_STATE_READY)
    {
        errorcode = HAL_BUSY;
        __HAL_UNLOCK(hspi);
        return errorcode;
    }

    /* Set the transaction information */
    hspi->State = HAL_SPI_STATE_BUSY_TX;
    hspi->ErrorCode = HAL_SPI_ERROR_NONE;
    //   hspi->pTxBuffPtr  = (uint8_t *)pData;
    hspi->TxXferSize = Size;
    hspi->TxXferCount = Size;

    LCD_TxDataCount = Size;                  
    LCD_pData_32bit = (pData << 16) | pData;

    /*Init field not used in handle to zero */
    hspi->pRxBuffPtr = NULL;
    hspi->RxXferSize = (uint16_t)0UL;
    hspi->RxXferCount = (uint16_t)0UL;
    hspi->TxISR = NULL;
    hspi->RxISR = NULL;

    /* Configure communication direction : 1Line */

    SPI_1LINE_TX(hspi); 


    MODIFY_REG(hspi->Instance->CR2, SPI_CR2_TSIZE, 0);

    /* Enable SPI peripheral */
    __HAL_SPI_ENABLE(hspi);

    if (hspi->Init.Mode == SPI_MODE_MASTER)
    {
        /* Master transfer start */
        SET_BIT(hspi->Instance->CR1, SPI_CR1_CSTART);
    }

    /* Transmit data in 16 Bit mode */
    /* Transmit data in 16 Bit mode */
    while (LCD_TxDataCount > 0UL)
    {
        /* Wait until TXP flag is set to send data */
        if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXP))
        {
            if ((LCD_TxDataCount > 1UL) && (hspi->Init.FifoThreshold > SPI_FIFO_THRESHOLD_01DATA))
            {
                *((__IO uint32_t *)&hspi->Instance->TXDR) = (uint32_t)LCD_pData_32bit;
                //  pData += sizeof(uint32_t);
                LCD_TxDataCount -= (uint16_t)2UL;
            }
            else
            {
                *((__IO uint16_t *)&hspi->Instance->TXDR) = (uint16_t)pData;
                //  pData += sizeof(uint16_t);
                LCD_TxDataCount--;
            }
        }
        else
        {
            /* Timeout management */
            if ((((HAL_GetTick() - tickstart) >= Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
            {
                /* Call standard close procedure with error check */
                MY_SPI_CloseTransfer(hspi);

                /* Process Unlocked */
                __HAL_UNLOCK(hspi);

                SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_TIMEOUT);
                hspi->State = HAL_SPI_STATE_READY;
                return HAL_ERROR;
            }
        }
    }
    if (MY_SPI_WaitOnFlagUntilTimeout(hspi, SPI_SR_TXC, RESET, tickstart, Timeout) != HAL_OK)
    {
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
    }

    SET_BIT((hspi)->Instance->CR1, SPI_CR1_CSUSP); // 请求挂起SPI传输
    /* 等待SPI挂起 */
    if (MY_SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_SUSP, RESET, tickstart, Timeout) != HAL_OK)
    {
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
    }
    MY_SPI_CloseTransfer(hspi); /* Call standard close procedure with error check */

    SET_BIT((hspi)->Instance->IFCR, SPI_IFCR_SUSPC); // 清除挂起标志位

    /* Process Unlocked */
    __HAL_UNLOCK(hspi);

    hspi->State = HAL_SPI_STATE_READY;

    if (hspi->ErrorCode != HAL_SPI_ERROR_NONE)
    {
        return HAL_ERROR;
    }
    return errorcode;
}


HAL_StatusTypeDef LCD_SPI_TransmitBuffer(SPI_HandleTypeDef *hspi, uint16_t *pData, uint32_t Size)
{
    uint32_t tickstart;
    uint32_t Timeout = 1000;      
    __IO uint32_t LCD_TxDataCount; 
    HAL_StatusTypeDef errorcode = HAL_OK;

    /* Check Direction parameter */
    assert_param(IS_SPI_DIRECTION_2LINES_OR_1LINE_2LINES_TXONLY(hspi->Init.Direction));

    /* Process Locked */
    __HAL_LOCK(hspi);

    /* Init tickstart for timeout management*/
    tickstart = HAL_GetTick();

    if (hspi->State != HAL_SPI_STATE_READY)
    {
        errorcode = HAL_BUSY;
        __HAL_UNLOCK(hspi);
        return errorcode;
    }

    /* Set the transaction information */
    hspi->State = HAL_SPI_STATE_BUSY_TX;
    hspi->ErrorCode = HAL_SPI_ERROR_NONE;

    LCD_TxDataCount = Size; 

    /*Init field not used in handle to zero */
    hspi->TxISR = NULL;
    hspi->RxISR = NULL;

    /* Configure communication direction : 1Line */

    SPI_1LINE_TX(hspi); 


    MODIFY_REG(hspi->Instance->CR2, SPI_CR2_TSIZE, 0);

    /* Enable SPI peripheral */
    __HAL_SPI_ENABLE(hspi);

    if (hspi->Init.Mode == SPI_MODE_MASTER)
    {
        /* Master transfer start */
        SET_BIT(hspi->Instance->CR1, SPI_CR1_CSTART);
    }

    /* Transmit data in 16 Bit mode */
    /* Transmit data in 16 Bit mode */
    while (LCD_TxDataCount > 0UL)
    {
        /* Wait until TXP flag is set to send data */
        if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXP))
        {
            if ((LCD_TxDataCount > 1UL) && (hspi->Init.FifoThreshold > SPI_FIFO_THRESHOLD_01DATA))
            {
                *((__IO uint32_t *)&hspi->Instance->TXDR) = *((uint32_t *)pData);
                pData += 2;
                LCD_TxDataCount -= 2;
            }
            else
            {
                *((__IO uint16_t *)&hspi->Instance->TXDR) = *((uint16_t *)pData);
                pData += 1;
                LCD_TxDataCount--;
            }
        }
        //      else
        //      {
        //        /* Timeout management */
        //        if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
        //        {
        //          /* Call standard close procedure with error check */
        //          MY_SPI_CloseTransfer(hspi);

        //          /* Process Unlocked */
        //          __HAL_UNLOCK(hspi);

        //          SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_TIMEOUT);
        //          hspi->State = HAL_SPI_STATE_READY;
        //          return HAL_ERROR;
        //        }
        //      }
    }

    //  	printf ("%d \r\n",LCD_TxDataCount);

    //
    if (MY_SPI_WaitOnFlagUntilTimeout(hspi, SPI_SR_TXC, RESET, tickstart, Timeout) != HAL_OK)
    {
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
    }

    SET_BIT((hspi)->Instance->CR1, SPI_CR1_CSUSP); 

    if (MY_SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_SUSP, RESET, tickstart, Timeout) != HAL_OK)
    {
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
    }
    MY_SPI_CloseTransfer(hspi); /* Call standard close procedure with error check */

    SET_BIT((hspi)->Instance->IFCR, SPI_IFCR_SUSPC); 

    /* Process Unlocked */
    __HAL_UNLOCK(hspi);

    hspi->State = HAL_SPI_STATE_READY;

    if (hspi->ErrorCode != HAL_SPI_ERROR_NONE)
    {
        return HAL_ERROR;
    }
    return errorcode;
}


void _LCD_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t * _LCD_Buff)
{

    uint32_t i, j;

    for (i = x; i < x + width; i++)
    {
        for (j = y; j < y + height; j++)
        {
            _LCD_Buff[i * LCD.Width + j] = LCD.Color;
        }
    }
}

void _LCD_Char2Ram(uint16_t x, uint16_t y, uint16_t *charBuff, uint16_t * _LCD_Buff)
{
    uint32_t i, j;

    for (i = x; i < x + LCD_AsciiFonts->Height; i++)
    {
        for (j = y; j < y + LCD_AsciiFonts->Width; j++)
        {
            uint16_t color = charBuff[(i - x) * LCD_AsciiFonts->Width + (j - y)];
            if (color == LCD.BackColor)
                continue;
            _LCD_Buff[i * LCD.Width + j] = color; 
        } 
    }
}


void _LCD_DisplayChar(uint16_t x, uint16_t y, uint8_t c, uint16_t * _LCD_Buff)
{
    uint16_t index = 0, counter = 0, i = 0, w = 0; 
    uint8_t disChar;                              

    c = c - 32; 

    for (index = 0; index < LCD_AsciiFonts->Sizes; index++)
    {
        disChar = LCD_AsciiFonts->pTable[c * LCD_AsciiFonts->Sizes + index]; 

        for (counter = 0; counter < 8; counter++)
        {
            if (disChar & 0x01)
            {
                LCD_Buff[i] = LCD.Color; 
            }
            else
            {
                LCD_Buff[i] = LCD.BackColor; 
            }

            disChar >>= 1;

            i++;
            w++;
            if (w == LCD_AsciiFonts->Width) 
            {                         
                w = 0;
                break;
            }
        }
    }

    _LCD_Char2Ram(x, y, LCD_Buff, _LCD_Buff);
}


void _LCD_DisplayString(uint16_t x, uint16_t y, char *p, uint16_t * _LCD_Buff)
{
    while (*p != 0) 
    {
        _LCD_DisplayChar(x, y, *p, _LCD_Buff); 
        y += LCD_AsciiFonts->Width; 
        p++;                       
    }
}

void _LCD_DrawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t * _LCD_Buff)
{
    uint32_t i, j;

    for (i = x; i < x + width; i++)
    {
        for (j = y; j < y + height; j++)
        {
            if (i == x || i == x + width - 1 || j == y || j == y + height - 1)
            {
                _LCD_Buff[i * LCD.Width + j] = LCD.Color;
            }
        }
    }
}

void _LCD_DrawImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t * _LCD_Buff, uint16_t *pImage) 
{
    uint32_t i, j;

    for (i = x; i < x + width; i++)
    {
        for (j = y; j < y + height; j++)
        {
            _LCD_Buff[i * LCD.Width + j] = *(pImage++);
        }
    }
}
