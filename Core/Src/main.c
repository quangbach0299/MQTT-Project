/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cJSON.h"
#include "stdio.h"
#include <stdint.h>
#include <math.h> 
#include <cJSON.h> 
#include <string.h>
#include <stdlib.h>



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
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart6;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#ifdef __GNUC__
     #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
     #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
  // CONG PC
	HAL_UART_Transmit(&huart2,(uint8_t *)&ch,1,0xFFFF);
	return ch;
}
//phong cach code kieu cu
cJSON *str_json,*str_TB1 , *str_TB2 , *str_C1 , *str_C2  , *str_Mode;

uint8_t rx_data1,rx_data2;
uint8_t rx_index1;
char rx_buffer1[100];
char rx_buffer2[100];

char ResponseRX[500];
uint32_t  rx_indexResponse;

uint8_t CheckConnect;


long last = 0;
unsigned int ND = 0;
unsigned int DA = 0;
unsigned int TTTB1 = 0;
unsigned int TTTB2 = 0;
unsigned int C1 = 0;
unsigned int C2 = 0;
unsigned int ConfigAt = 0;
unsigned int ErrorCode = 0;


char *ssid = "Lau2";
char *pass = "12khavancan";
char *mqtt_host = "ngoinhaiot.com";
char *mqtt_port = "1111";
char *mqtt_user = "tiepappleid";
char *mqtt_pass = "65F4856EF4024DDA";
char *mqtt_sub = "tiepappleid/quat"; //topic gui du lieu cho esp
char *mqtt_pub = "tiepappleid/maylanh";
int chedo = 0;// 0 = auto, 1 == man


void SettingESP(void);
void ConnectMQTT(char *host, char *port, char *user, char *pass, char *sub, char *pub);
void ChuongTrinhCamBien(void);
void Send_AT_Commands_Setting(char *AT_Commands, char *DataRespone, uint32_t timesend, uint32_t setting);
void Send_AT_Commands_ConnectMQTT(char *AT_Commands, char *DataResponse , uint32_t timeout , uint32_t setting , uint32_t count);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void Received_AT_Commands_ESP(void);
void Received_AT_Comands_ESP_MessageMQTT(void);
void ClearBuffer_UART_ESP(void);
void ClearRespone(void);
void ParseJson(char *DataMQTT);
void SettingESP(void);
void SendData(char *pub , unsigned int ND ,unsigned int DA, unsigned int TB1,  unsigned int TB2, unsigned int C1, unsigned int C2);
void Send_AT_Commands_SendMessager(char *AT_Commands, char *DataResponse , uint32_t timeout , uint32_t setting , uint32_t count);
	









void SendData(char *pub , unsigned int ND ,unsigned int DA, unsigned int TB1,  unsigned int TB2, unsigned int C1, unsigned int C2)
{
	//AT+MQTTPUBRAW=0,"tiepappleid/maylanh",5,0,1$0D$0A
	/*
	tra ve 
	OK\r\n\r\n>
	=> send data => ABCD$0D$0A => tra ve +MQTTPUB:OK
	*/
	char MQTTPUBRAW[100];
	char JSON[100];
	char Str_ND[100];
	char Str_DA[100];
	char Str_TB1[100];
	char Str_TB2[100];
	char Str_C1[100];
	char Str_C2[100];
	char Length[100];

	
	for(int i = 0 ; i < 100; i++)
	{
		MQTTPUBRAW[i] = 0;
		JSON[i] = 0;
		Str_ND[i] = 0;
		Str_DA[i] = 0;
		Str_TB1[i] = 0;
		Str_TB2[i] = 0;
		Str_C1[i] = 0;
		Str_C2[i] = 0;
		Length[i] = 0;
	}
	sprintf(Str_ND, "%d", ND);
	sprintf(Str_DA, "%d", DA);
	sprintf(Str_TB1, "%d", TB1);
	sprintf(Str_TB2, "%d", TB2);
	sprintf(Str_C1, "%d", C1);
	sprintf(Str_C2, "%d", C2);
	
	strcat(JSON,"{\"ND\":\"");
	strcat(JSON,Str_ND);
	strcat(JSON,"\",");
	
	strcat(JSON,"\"DA\":\"");
	strcat(JSON,Str_DA);
	strcat(JSON,"\",");
	
	strcat(JSON,"\"TB1\":\"");
	strcat(JSON,Str_TB1);
	strcat(JSON,"\",");
	
	strcat(JSON,"\"TB2\":\"");
	strcat(JSON,Str_TB2);
	strcat(JSON,"\",");
	
	strcat(JSON,"\"C1\":\"");
	strcat(JSON,Str_C1);
	strcat(JSON,"\",");
	
  
	strcat(JSON,"\"C2\":\"");
	strcat(JSON,Str_C2);
	strcat(JSON,"\"}");
	strcat(JSON,"\r\n");
	
	printf("DataJson: %s\n", JSON);
	//do do dai chuoi vua gui
	int len = 0;
	len = strlen(JSON);
	sprintf(Length, "%d", len);
	
	//AT+MQTTPUBRAW=0,"tiepappleid/SEND",5,0,1$0D$0A
	strcat(MQTTPUBRAW,"AT+MQTTPUBRAW=0,\"");
	strcat(MQTTPUBRAW,pub);
	strcat(MQTTPUBRAW,"\",");
	strcat(MQTTPUBRAW,Length);
	strcat(MQTTPUBRAW,",0,1\r\n");
	printf("MQTTPUBRAW: %s\n",MQTTPUBRAW);
	
	//{"ND":"","DA":"","TB1":"","TB2":"","C1":"","C2":""}
	
	Send_AT_Commands_SendMessager(MQTTPUBRAW, "OK\r\n\r\n>" , 5000 , 0 , 3);
	
	ClearBuffer_UART_ESP();	
		
	if(ErrorCode == 0)
	{
		Send_AT_Commands_SendMessager(JSON, "+MQTTPUB:OK" , 5000 , 0 , 5);
		ClearBuffer_UART_ESP();
	}
}

void Send_AT_Commands_SendMessager(char *AT_Commands, char *DataResponse , uint32_t timeout , uint32_t setting , uint32_t count)
{
	ClearBuffer_UART_ESP();
	last = HAL_GetTick();
	uint32_t Size = 300;
	uint32_t Count = 0;
	ConfigAt = setting;
	char DataHTTP[Size];
	for(int i = 0 ; i < Size; i++)
	{
		DataHTTP[i] = 0;
	}
	// dua data lenh AT_Commands vao mang  SendHTTP
	snprintf(DataHTTP, sizeof(DataHTTP),"%s", AT_Commands);
	HAL_UART_Transmit(&huart6,(uint8_t *)&DataHTTP,strlen(DataHTTP),1000);
	printf("Send AT-Commands Data: %s\r\n", DataHTTP);
	last = HAL_GetTick();
	while(1)
	{
		//chay ham ngat uart
		if(HAL_GetTick() - last >= timeout)
		{
			Count++;
			HAL_UART_Transmit(&huart6,(uint8_t *)&DataHTTP,strlen(DataHTTP),1000);
			printf("Send AT-Commands Send Data MQTT: %s\r\n", DataHTTP);
			last = HAL_GetTick();
		}
		if(strstr(rx_buffer1,DataResponse) != NULL)
		{
			//printf("Reponse DataBlynk: %s\r\n",DataResponse);
			printf("SEND MQTT OK\r\n");
			ClearBuffer_UART_ESP();
			ErrorCode = 0;
			CheckConnect = 1;
			last = HAL_GetTick();
			break;
		}
		if(Count >= count)
		{
			printf("SEND MQTT ERROR\r\n"); // gui lenh setting lai
			ErrorCode = 1;
			ClearBuffer_UART_ESP();
			last = HAL_GetTick();
			break;
		}
	}
}
	

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(ConfigAt == 0)
	{
		if(huart->Instance == huart6.Instance)
		{
		Received_AT_Commands_ESP();
		HAL_UART_Receive_IT(&huart6,&rx_data1,1);
		}
	}
	else if(ConfigAt == 1)
	{
		if(huart->Instance == huart6.Instance)
		{
		Received_AT_Comands_ESP_MessageMQTT();
		HAL_UART_Receive_IT(&huart6,&rx_data1,1);
		}
	}
}
 
void ChuongTrinhCamBien(void)
{
	ND = ND +1;
	DA = DA +2;
	if( DA > 100)
	{
		ND = 0;
		DA = 0;
	}
}
void SendMQTT(void)
{
	if(HAL_GetTick() - last >= 1000)
	{
		if(CheckConnect)
		{
		  HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
			ChuongTrinhCamBien();	
			SendData(mqtt_pub,ND,DA,TTTB1,TTTB2,C1,C2);
		}
	}
	ConfigAt = 1;
}

 


void ParseJson(char *DataMQTT)
{
	str_json = cJSON_Parse(DataMQTT);
	if(!str_json)
	{
		printf("JSON error\r\n");
		return;
	}
	else
	{
		printf("JSON OKE\r\n");
	 //{"TB1":"0"} {"TB1":"1"}
		str_TB1 = cJSON_GetObjectItem(str_json,"TB1");
		if(str_TB1->type == cJSON_String)
		{
			if(strstr(str_TB1->valuestring,"1") != NULL)
			{
				printf("ON D1 \r\n");
				TTTB1 = 1;
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_SET);
			}
			else if(strstr(str_TB1->valuestring,"0") != NULL)
			{
				printf("OFF D1 \r\n");
				TTTB1 = 0;
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_RESET);
				
			}
		}
		
		str_TB2 = cJSON_GetObjectItem(str_json,"TB2");
		if(str_TB2->type == cJSON_String)
		{
			if(strstr(str_TB2->valuestring,"1") != NULL)
			{
				printf("ON D2 \r\n");
				TTTB2 = 1;
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);
			}
			else if(strstr(str_TB2->valuestring,"0") != NULL)
			{
				printf("OFF D2 \r\n");
				TTTB2 = 0;
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);
			}
		}
		
		str_C1 = cJSON_GetObjectItem(str_json, "C1"); //Get information about the value corresponding to the name key
		if (str_C1->type == cJSON_String)
		{
			printf("C1:%s \r\n", str_C1->valuestring);
			C1 = atoi(str_C1->valuestring);
		}
		
		str_C2 = cJSON_GetObjectItem(str_json, "C2"); //Get information about the value corresponding to the name key
		if (str_C2->type == cJSON_String)
		{
			printf("C2:%s \r\n", str_C2->valuestring);
			C2 = atoi(str_C2->valuestring);
		}
  	cJSON_Delete(str_json);
	}
}
void ClearRespone(void)
{
	for(uint8_t i = 0; i<100; i++)
	{
		ResponseRX[i] = 0 ;
	}
	rx_indexResponse = 0;
}
void ClearBuffer_UART_ESP(void)
{
		for(int i = 0 ; i < 500 ; i++)
	{
		rx_buffer1[i] = 0;
	}
	rx_index1 = 0;
}
 

void Received_AT_Comands_ESP_MessageMQTT(void)
{
	if(rx_data1 != '\n')
	{
		ResponseRX[rx_indexResponse++] = rx_data1;
	}
	else
	{
		ResponseRX[rx_indexResponse++] = rx_data1;
		printf("Data MQTT: %s\r\n",ResponseRX);
		
		//Check Connect
		if(strstr(ResponseRX,"MQTTCONNECTED") != NULL)
		{
			printf("Connect MQTT to RECEIVED DATA\r\n");
			CheckConnect = 1;
			last =  HAL_GetTick();
		}
		else if(strstr(ResponseRX,"MQTTDISCONNECTED") != NULL)
		{
			printf("Not Connect MQTT\r\n");
			CheckConnect = 0;
			last = HAL_GetTick();
		}
		else if(strstr(ResponseRX,"+MQTTSUBRECV") != NULL)
		{
			char *DataMQTT;
			//+MQTTSUBRECV:0,"tiepappleid/quat",4,hiii
			DataMQTT = strtok(ResponseRX,",");
			DataMQTT = strtok(NULL,",");
			DataMQTT = strtok(NULL,",");
			DataMQTT = strtok(NULL,"\n");
			printf("DATA MQTT: %s\r\n", DataMQTT);
			
			ParseJson(DataMQTT);
			last = HAL_GetTick();
		}
		last = HAL_GetTick();
		ClearRespone();
	}
}
void Received_AT_Commands_ESP(void)
{
	rx_buffer1[rx_index1++] = rx_data1;
}

void Send_AT_Commands_Setting(char *AT_Commands, char *DataRespone, uint32_t timesend, uint32_t setting)
{
	ConfigAt = setting;
	char DataSendAT[50];
	for(int i = 0; i < 50; i++)
	{
		DataSendAT[i] = 0;
	}
	//dua lenh AT Commands vao mang DataSendAT
	snprintf(DataSendAT,strlen(DataSendAT),"%s\r\n",AT_Commands);
	//send ESP qua cong UART1
	HAL_UART_Transmit(&huart6,(uint8_t *)DataSendAT,strlen(DataSendAT),1000);
	printf("Send AT-Commands Setting: %s\r\n",DataSendAT);
	//dung doi phan hoi + doi qua lau thi gui cac goi tin khac ( phan hoi tu ngat uart1)
	last = HAL_GetTick();
	while(1)
	{
		//qua 5s thi gui lai lenh cu khi nao ok thithoi
		if(HAL_GetTick() - last >= timesend)
		{
			HAL_UART_Transmit(&huart6,(uint8_t *)DataSendAT,strlen(DataSendAT),1000);
			printf("Send AT-Commands Setting TimeSend: %s\r\n", DataSendAT);
			last = HAL_GetTick();
		}
		if(strstr(rx_buffer1,DataRespone) != NULL)
		{
			//printf("Data Buffer2: %s\r\n",rx_buffer1);
			printf("Reponse Setting: %s\r\n",DataRespone);
			ClearBuffer_UART_ESP();
			break;
		}
	}
}	

void SettingESP(void)
{
	Send_AT_Commands_Setting("AT+RST\r\n", "OK", 10000 , 0);
	
	HAL_Delay(3000);
	
	//Send_AT_Commands_Setting("AT+CWQAP\r\n", "WIFI DISCONNECT\r\n\r\nOK" , 2000, 0);
	
	//HAL_Delay(3000);
	
	Send_AT_Commands_Setting("AT\r\n", "OK", 300, 0);
	
	HAL_Delay(3000);
	
	Send_AT_Commands_Setting("ATE0\r\n", "OK" , 2000, 0);
	
	HAL_Delay(3000);
	
	Send_AT_Commands_Setting("AT+CWMODE=1,1\r\n", "OK", 2000, 0);
	
	HAL_Delay(3000);
	
	Send_AT_Commands_Setting("AT+CWJAP=\"Lau2\",\"12khavancan\"\r\n", "WIFI CONNECTED", 10000, 0);
	
	HAL_Delay(3000);
	
	Send_AT_Commands_Setting("AT+CIPMUX=0\r\n", "OK", 2000 , 0);
	
	HAL_Delay(3000);
	
	ErrorCode = 0;
}

void ButtonMode(void)
{
	//chuyentrangthai mode
	
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
  MX_USART6_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart6,&rx_data1,1); //ESP
	HAL_UART_Receive_IT(&huart2,&rx_data2,1); //USB
	
	HAL_Delay(1000);\

	printf("SETTING ESP\r\n");

	SettingESP();

	HAL_Delay(1000);
	
	ConnectMQTT(mqtt_host,mqtt_port,mqtt_user,mqtt_pass,mqtt_sub,mqtt_pub);
	
	last = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		if(chedo == 0)
		{
			while(1)
			{
				SendMQTT();
				ButtonMode();
				if(chedo == 1)
				{
					break;
					
				}
			}
		}
		else if(chedo == 1)
		{
			while(1)
			{
				if(chedo == 0)
				{
					break;
					
				}
			}
			
		}
  }
  /* USER CODE END 3 */
}

void ConnectMQTT(char *host, char *port, char *user, char *pass, char *sub, char *pub)
{
	uint32_t id = 0;
	id = rand()%100;
	char clientid[100];
	char MathRandom[100];
	char MQTTUSERCFG[100];
	char MQTTCONN[100];
	char MQTTSUB[100];
	
	for(int i = 0; i < 100; i++)
	{
	clientid[i] = 0;
	MathRandom[i]= 0;
	MQTTUSERCFG[i] = 0;
	MQTTCONN[i] = 0;
	MQTTSUB[i] = 0;
	}
	
	sprintf(MathRandom,"%d",id);
	strcat(clientid,"ESP");
	strcat(clientid,MathRandom);
	
	//AT+MQTTUSERCFG=0,1,"ESP8266","tiepappleid","65F4856EF4024DDA",0,0,""$0D$0A => OK
	
	strcat(MQTTUSERCFG,"AT+MQTTUSERCFG=0,1,\"");
	strcat(MQTTUSERCFG,clientid);
	strcat(MQTTUSERCFG,"\",\"");
	strcat(MQTTUSERCFG,user);
	strcat(MQTTUSERCFG,"\",\"");
	strcat(MQTTUSERCFG,pass);
	strcat(MQTTUSERCFG,"\",0,0,");
	strcat(MQTTUSERCFG,"\"\"");
	strcat(MQTTUSERCFG,"\r\n");
	//printf("MQTTUSERCFG: %s",MQTTUSERCFG);
	
	//AT+MQTTCONN=0,"ngoinhaiot.com",1111,1$0D$0A
	strcat(MQTTCONN, "AT+MQTTCONN=0,\"");
	strcat(MQTTCONN, host);
	strcat(MQTTCONN, "\",");
	strcat(MQTTCONN, port);
	strcat(MQTTCONN, ",1\r\n");
	//printf("MQTTCONN: %s",MQTTCONN);
	
	///AT+MQTTSUB=0,"tiepappleid/quat",0$0D$0A => OK
	strcat(MQTTSUB, "AT+MQTTSUB=0,\"");
	strcat(MQTTSUB, sub);
	strcat(MQTTSUB, "\",0\r\n");
	printf("MQTTSUB: %s",MQTTSUB);
	
	Send_AT_Commands_ConnectMQTT(MQTTUSERCFG, "OK" , 5000 , 0 , 5);
	HAL_Delay(1000);
	ClearBuffer_UART_ESP();
	
	Send_AT_Commands_ConnectMQTT(MQTTCONN, "+MQTTCONNECTED" , 5000 , 0 , 5);
	HAL_Delay(1000);
	ClearBuffer_UART_ESP();
	
	
	Send_AT_Commands_ConnectMQTT(MQTTSUB, "OK" , 5000 , 0 , 5);
	HAL_Delay(1000);
	ClearBuffer_UART_ESP();

	ConfigAt = 1;//doc du lieu tu host chuyen ve
	//ErrorCode = 1;	//ko co loi ?
}

void Send_AT_Commands_ConnectMQTT(char *AT_Commands, char *DataResponse , uint32_t timeout , uint32_t setting , uint32_t count)
{
	ClearBuffer_UART_ESP();
	last = HAL_GetTick();
	uint32_t Size = 300;
	uint32_t Count = 0;
	ConfigAt = setting;
	char DataHTTP[Size];
	for(int i = 0 ; i < Size; i++)
	{
		DataHTTP[i] = 0;
	}
	//dua lenh AT Command vao mang SendHTTP
	snprintf(DataHTTP, sizeof(DataHTTP),"%s", AT_Commands);
	HAL_UART_Transmit(&huart6,(uint8_t *)&DataHTTP,strlen(DataHTTP),1000);
	printf("Send AT-Commands Data: %s\r\n", DataHTTP);
	last = HAL_GetTick();
	
	while(1)
	{
		//chay ham ngat uart
		if(HAL_GetTick() - last >= timeout)
		{
			Count++;
			HAL_UART_Transmit(&huart6,(uint8_t *)&DataHTTP,strlen(DataHTTP),1000);
			printf("Send AT-Commands Data TimeOut: %s\r\n", DataHTTP);
			last = HAL_GetTick();
		}
		if(strstr(rx_buffer1,DataResponse) != NULL)
		{
			//printf("Reponse DataBlynk: %s\r\n",DataResponse);
			printf("MQTT Connect OK\r\n");
			ClearBuffer_UART_ESP();
			ErrorCode = 0;  
			CheckConnect = 1;
			last = HAL_GetTick();
			break;
		}
		if(Count >= count)
		{
			printf("MQTT Connect ERROR\r\n"); // gui lenh setting lai
			ErrorCode = 1;
			CheckConnect = 0; 
			ClearBuffer_UART_ESP();
			last = HAL_GetTick();
			break;
		}
}
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pins : PD12 PD13 PD14 PD15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
