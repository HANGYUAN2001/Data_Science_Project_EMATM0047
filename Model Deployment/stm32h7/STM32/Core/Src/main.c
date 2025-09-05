/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "crc.h"
#include "usart.h"
#include "gpio.h"
#include "app_x-cube-ai.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "dcmi_ov5640.h"
#include "lcd_spi_200.h"
#include "network.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
__attribute__((section(".RAM_D1"), aligned(32))) __IO uint32_t Camera_Buffer[Display_BufferSize];
__attribute__((section(".RAM_D1"), aligned(32))) __IO uint16_t detect_img_data[img_size * img_size];
        

extern const char * class_names[];    
char display_str[32];                 
char conf_str[32];        
int class_num = 0;

/* USER CODE END PV */



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void MPU_Config(void);
void Get_Detect_Img(uint16_t x, uint16_t y, uint16_t l, uint16_t *raw, uint16_t *detect_img);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void LCD_DisplayCenter(uint16_t y, const char* text, uint16_t *buffer, uint8_t font_width) {
    uint16_t x = (LCD_Width - strlen(text) * font_width) / 2;
    _LCD_DisplayString(y, x, (char *)text, buffer);
}

void LCD_DisplayBottomCenterFPS(uint16_t fps)
{
    char fps_str[16];
    sprintf(fps_str, "FPS: %d", fps);

    uint16_t x = (LCD_Width - strlen(fps_str) * 8) / 2; 
    uint16_t y = LCD_Height - 16; 

    _LCD_DisplayString(y, x, fps_str, (uint16_t *)Camera_Buffer);
}


/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MPU Configuration--------------------------------------------------------*/
    MPU_Config();

    /* Enable I-Cache---------------------------------------------------------*/
    SCB_EnableICache();

    /* Enable D-Cache---------------------------------------------------------*/
    SCB_EnableDCache();

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */
    SPI_LCD_Init();    
    DCMI_OV5640_Init(); 

    OV5640_AF_Download_Firmware();
    OV5640_AF_Trigger_Constant();  

    OV5640_DMA_Transmit_Continuous((uint32_t)Camera_Buffer, Display_BufferSize); 

    LCD_SetAsciiFont(&ASCII_Font24);
    LCD_SetColor(LIGHT_RED);
    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_CRC_Init();
    MX_USART1_UART_Init();
    MX_X_CUBE_AI_Init();
    /* USER CODE BEGIN 2 */
    
    int center_x = (int)((LCD_Width - img_size) / 2);
    int center_y = (int)((LCD_Height - img_size) / 2);

    int box_x = (int)((LCD_Width - 96) / 2);
    int box_y = (int)((LCD_Height - 96) / 2);

    printf("strat detect!");
		
    LCD_SetColor(LCD_RED);  
    LCD_SetAsciiFont(&ASCII_Font16);  
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
			
			
        /* USER CODE END WHILE */
        if (OV5640_FrameState == 1) 
        {
            OV5640_FrameState = 0;
					  
					  SCB_InvalidateDCache_by_Addr((uint32_t*)Camera_Buffer, Display_BufferSize * sizeof(uint32_t));

            Get_Detect_Img(center_x, center_y, img_size, (uint16_t *)Camera_Buffer, (uint16_t *)detect_img_data); 
            MX_X_CUBE_AI_Process(); 
            _LCD_DrawRect(box_y, box_x, 96, 96, (uint16_t *)Camera_Buffer);       
            LCD_DisplayCenter(center_y + 130, display_str, (uint16_t *)Camera_Buffer, 8);
            LCD_DisplayCenter(center_y + 150, conf_str, (uint16_t *)Camera_Buffer, 8);

					
LCD_DisplayCenter(0, "Hang Yuan", (uint16_t *)Camera_Buffer, 8);
LCD_DisplayCenter(20, "Supervised by: Gabriella Miles", (uint16_t *)Camera_Buffer, 8);
LCD_DisplayCenter(40, "Model: MobileNetV3-Small", (uint16_t *)Camera_Buffer, 8);
LCD_DisplayCenter(60, "Blink detection", (uint16_t *)Camera_Buffer, 8);
LCD_DisplayCenter(80, "Three categories", (uint16_t *)Camera_Buffer, 8);
LCD_DisplayBottomCenterFPS(OV5640_FPS);


					LCD_CopyBuffer(0, 0, LCD_Width, LCD_Height, (uint16_t *)Camera_Buffer);             
					
					

					
					  LED1_Toggle;
        }
			
        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    /** Supply configuration update enable
     */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

    /** Configure the main internal regulator output voltage
     */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
    {
    }

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 2;
    RCC_OscInitStruct.PLL.PLLN = 44;
    RCC_OscInitStruct.PLL.PLLP = 1;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        Error_Handler();
    }

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI6;
    PeriphClkInitStruct.Spi6ClockSelection = RCC_SPI6CLKSOURCE_D3PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */
void Get_Detect_Img(uint16_t x, uint16_t y, uint16_t l, uint16_t *raw, uint16_t *detect_img)
{
    uint32_t i, j;

    for (i = 0; i < l; i++)
    {
        for (j = 0; j < l - 15; j+=16) 
        {
            detect_img[j + i * l] = raw[(y + i) * 240 + x + j];
            detect_img[j + 1 + i * l] = raw[(y + i) * 240 + x + j + 1];
            detect_img[j + 2 + i * l] = raw[(y + i) * 240 + x + j + 2];
            detect_img[j + 3 + i * l] = raw[(y + i) * 240 + x + j + 3];
            detect_img[j + 4 + i * l] = raw[(y + i) * 240 + x + j + 4];
            detect_img[j + 5 + i * l] = raw[(y + i) * 240 + x + j + 5];
            detect_img[j + 6 + i * l] = raw[(y + i) * 240 + x + j + 6];
            detect_img[j + 7 + i * l] = raw[(y + i) * 240 + x + j + 7];
            detect_img[j + 8 + i * l] = raw[(y + i) * 240 + x + j + 8];
            detect_img[j + 9 + i * l] = raw[(y + i) * 240 + x + j + 9];
            detect_img[j + 10 + i * l] = raw[(y + i) * 240 + x + j + 10];
            detect_img[j + 11 + i * l] = raw[(y + i) * 240 + x + j + 11];
            detect_img[j + 12 + i * l] = raw[(y + i) * 240 + x + j + 12];
            detect_img[j + 13 + i * l] = raw[(y + i) * 240 + x + j + 13];
            detect_img[j + 14 + i * l] = raw[(y + i) * 240 + x + j + 14];
            detect_img[j + 15 + i * l] = raw[(y + i) * 240 + x + j + 15];
        }
    }
}
/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{
    MPU_Region_InitTypeDef MPU_InitStruct = {0};

    HAL_MPU_Disable(); 

    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = 0x24000000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT); 
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
