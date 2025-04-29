/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "rc522.h"
#include "string.h"
#include "i2c-lcd.h"
#include "stdio.h"
#include <stdbool.h>
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
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
//RFID
uint8_t status;
uint8_t str[MAX_LEN];
uint8_t CARDID[5];

char buffer[16];      // Mang de luu chuoi cho LCD

//MENU
uint8_t Mode0 = 0 ;
uint8_t Mode1 = 1 ;
uint8_t attempt = 0 ;
bool Mode = true  ;
uint16_t GPIO_Pin ;

//KEYPAD
GPIO_InitTypeDef GPIO_InitStructPrivate = {0};
uint32_t previousMillis = 0;
uint32_t currentMillis = 0 ;
uint8_t keyPressed = 0;

//PASSWORD
char input[6] = "000000";
char initial_password[6] = "311724";

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

void checkpass(){
    if (strcmp(input, initial_password) == 0)
               {
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1 ); // Mở khóa cửa
	            lcd_clear();
	            lcd_put_cur(0, 0);
	            lcd_send_string("MAT KHAU DUNG");
                HAL_Delay(100);
//
//              if ( keyPressed == 68){
//              input[0]= input[1]=input[2]=input[3]=input[4]=input[5]= 48;
//              HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1 );}
                }

	        else
	           {
	            lcd_clear();
	            lcd_put_cur(0, 0);
	            lcd_send_string("MAT KHAU SAI") ;
	            input[0]= input[1]=input[2]=input[3]=input[4]=input[5]= 48;
	            HAL_Delay(2000);
	            if ( keyPressed != 65){attempt++;}
	            }

	            if (attempt >= 5)
	            {   lcd_clear();
                    lcd_put_cur(0, 0);
                    lcd_send_string("KHOA 5 PHUT");
	                HAL_Delay(10000); // Khóa 10s
	                attempt = 0;
	            }
	            else
	            {
	                HAL_Delay(1000); // Delay 1 giây
	            }

}

void loop(){
        if(Mode == true ){
      	  lcd_clear();
      	  	  	  	    	        lcd_put_cur(0, 0);  // Dat con tro len dong 1
      	  	  	  	    	        lcd_send_string("KHOA DANG DONG");
      	  	  	  	    	        lcd_put_cur(1, 0);  // Dat con tro len dong 2
      	  	  	  	    	        lcd_send_string("NHAP MA THE RFID");
      	  	  	  	    	        HAL_Delay(1000);

      	  status = MFRC522_Request(PICC_REQIDL, str);
      	  status = MFRC522_Anticoll(str);
      	  memcpy(CARDID, str, 5);
//      	  HAL_Delay(100);

        if (status == MI_OK) {
  	  if((str[0]==59) && (str[1]==3) && (str[2]==123) && (str[3]==135) && (str[4]==196) )
  	  	  	     {
  	  	  	    	 lcd_clear();
  	  	  	    	                  lcd_put_cur(0, 0);  // Dat con tro len dong 1
  	  	  	    	                  lcd_send_string("MA THE HOP LE 1");
  	  	  	    	                  lcd_put_cur(1, 0);  // Dat con tro len dong 2
  	  	  	    	                  lcd_send_string("MSSV: 20240000");
  	  	  	    	                  HAL_Delay(2000);

	  	  	    	                  Mode = false;
  	  	  	       }
  	  else if((str[0]==175) && (str[1]==11) && (str[2]==51) && (str[3]==31) && (str[4]==136) )
  	  	  	       {
  	  	  	    	 lcd_clear();
  	  	  	    		    	      lcd_put_cur(0, 0);  // Dat con tro len dong 1
  	  	  	    		    	      lcd_send_string("MA THE HOP LE 2 ");
  	  	  	    		    	      lcd_put_cur(1, 0);  // Dat con tro len dong 2
  	  	  	    		    	      lcd_send_string("MSSV: 20240001");
  	  	  	    		    	      HAL_Delay(2000);

 	  	  	    		    	 	  Mode = false;
  	  	  	     }
  	  	  	     else
  	  	  	     {   lcd_clear();
  	  	  	    	                  lcd_put_cur(0, 0);  // Dat con tro len dong 1
  	  	  	    	                  lcd_send_string("MA THE SAI");
  	  	  	    	                  lcd_put_cur(1, 0);  // Dat con tro len dong 2
  	  	  	    	                  lcd_send_string("YEU CAU NHAP LAI");
  	  	  	    	                  HAL_Delay(1000);
  	  	  	    	 HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
  	  	  	    	 HAL_Delay(200);
  	  	  	     }



        }

        }
        if (Mode == false){
        	lcd_init();

        	    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1);
        	    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 1);
        	    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);
        	    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);

        	    lcd_clear();
        	    lcd_put_cur(0, 0);
        	    lcd_send_string("NHAP MAT KHAU");
        	    while (Mode == false)
        	      {
        	                       if ( keyPressed != 65)
        	                          { lcd_clear();
        	                            lcd_put_cur(0, 0);
        	                            lcd_send_string("NHAP MAT KHAU");
        	                            HAL_Delay(1000);}

        	    	                    if(keyPressed == 51){input[0] = 51; }
        	    	                    if(keyPressed == 49){input[1] = input[2] = 49; }
        	    	                    if(keyPressed == 55){input[3] = 55; }
        	    	                    if(keyPressed == 50){input[4] = 50; }
        	    	                    if(keyPressed == 52){input[5] = 52; }

        	    	                    bool allDifferentFrom48 = true;
        	    	                    for (int i = 0; i < 6; i++) {
        	    	                        if (input[i] == 48) {
        	    	                            allDifferentFrom48 = false;
        	    	                            break;
        	    	                        }
        	    	                    }

        	    	                    if (allDifferentFrom48) {
        	    	                        lcd_put_cur(1, 0);
        	    	                        lcd_send_string("******");
        	    	                        HAL_Delay(100);
        	    	                    }

        	                            if ( keyPressed == 65 )
        	                            {checkpass();}

        	                            if ( keyPressed == 64){
        	                            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0 );
        	                            input[0]= input[1]=input[2]=input[3]=input[4]=input[5]= 48;
        	                            keyPressed = 48;
        	                            Mode = true;
        	                            }

        	      }
        }

    }

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  MFRC522_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


	  loop();

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB12 PB13 PB14
                           PB15 PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PA8 PA9 PA10 PA11 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  currentMillis = HAL_GetTick();
  if (currentMillis - previousMillis > 10) {
    /*Configure GPIO pins : PA8 PA9 PA10 PA11 to GPIO_INPUT*/
    GPIO_InitStructPrivate.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStructPrivate.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructPrivate.Pull = GPIO_NOPULL;
    GPIO_InitStructPrivate.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructPrivate);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 0);
    if(GPIO_Pin == GPIO_PIN_8 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8))
    {
      while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8)==0){}
      keyPressed = 49; //ASCII value of 1

    }
    else if(GPIO_Pin == GPIO_PIN_9 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9))
    {
      while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9)==0){}
      keyPressed = 53; //ASCII value of 5

    }
    else if(GPIO_Pin == GPIO_PIN_10 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10))
    {
      while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10)==0){}
      keyPressed = 57; //ASCII value of 9

    }
    else if(GPIO_Pin == GPIO_PIN_11 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11))
    {
      while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_11)==0){}
      keyPressed = 65; //ASCII value of A

    }

        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 1);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 0);
    if(GPIO_Pin == GPIO_PIN_8 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8))
    {
      while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8)==0){}
      keyPressed = 50; //ASCII value of 2

    }
    else if(GPIO_Pin == GPIO_PIN_9 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9))
    {
      while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9)==0){}
      keyPressed = 54; //ASCII value of 6

    }
    else if(GPIO_Pin == GPIO_PIN_10 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10))
    {
      while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10)==0){}
      keyPressed = 42; //ASCII value of *

    }
    else if(GPIO_Pin == GPIO_PIN_11 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11))
    {
      while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_11)==0){}
      keyPressed = 66; //ASCII value of B

    }
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 0);
    if(GPIO_Pin == GPIO_PIN_8 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8))
    {
      while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8)==0){}
      keyPressed = 51; //ASCII value of 3

    }
    else if(GPIO_Pin == GPIO_PIN_9 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9))
    {
      while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9)==0){}
      keyPressed = 55; //ASCII value of 7

    }
    else if(GPIO_Pin == GPIO_PIN_10 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10))
    {
       while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10)==0){}
       keyPressed = 35; //ASCII value of  #

    }
    else if(GPIO_Pin == GPIO_PIN_11 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11))
    {
       while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_11)==0){}
       keyPressed = 67; //ASCII value of C

    }

         HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);
         HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0);
         HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
         HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
    if(GPIO_Pin == GPIO_PIN_8 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8))
    {
      while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8)==0){}
      keyPressed = 52; //ASCII value of 4

    }
    else if(GPIO_Pin == GPIO_PIN_9 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9))
    {
      while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9)==0){}
      keyPressed = 56; //ASCII value of 8

    }
    else if(GPIO_Pin == GPIO_PIN_10 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10))
    {
      while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10)==0){}
      keyPressed = 64; //ASCII value of @

    }
    else if(GPIO_Pin == GPIO_PIN_11 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11))
    {
      while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_11)==0){}
      keyPressed = 68; //ASCII value of D

    }

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
    /*Configure GPIO pins : PA8 PA9 PA10 PA11 back to EXTI*/
    GPIO_InitStructPrivate.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStructPrivate.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructPrivate);
    previousMillis = currentMillis;



  }
}


/* USER CODE END 4 */

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

#ifdef  USE_FULL_ASSERT
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
