/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2016 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */


#include "includes.h"
#include "driver/uart.h"
#include "spi_flash.h"
#include "server.h"
#include "client.h"
#include "my_http.h"

#define Project "transparent transmission"


#if ((SPI_FLASH_SIZE_MAP == 0) || (SPI_FLASH_SIZE_MAP == 1))
#error "The flash map is not supported"
#elif (SPI_FLASH_SIZE_MAP == 2)
#define SYSTEM_PARTITION_OTA_SIZE							0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR							0x81000
#define SYSTEM_PARTITION_RF_CAL_ADDR						0xfb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR						0xfc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR				0xfd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR           0x7c000
#elif (SPI_FLASH_SIZE_MAP == 3)
#define SYSTEM_PARTITION_OTA_SIZE							0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR							0x81000
#define SYSTEM_PARTITION_RF_CAL_ADDR						0x1fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR						0x1fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR				0x1fd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR           0x7c000
#elif (SPI_FLASH_SIZE_MAP == 4)
#define SYSTEM_PARTITION_OTA_SIZE							0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR							0x81000
#define SYSTEM_PARTITION_RF_CAL_ADDR						0x3fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR						0x3fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR				0x3fd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR           0x7c000
#elif (SPI_FLASH_SIZE_MAP == 5)
#define SYSTEM_PARTITION_OTA_SIZE							0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR							0x101000
#define SYSTEM_PARTITION_RF_CAL_ADDR						0x1fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR						0x1fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR				0x1fd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR           0xfc000
#elif (SPI_FLASH_SIZE_MAP == 6)
#define SYSTEM_PARTITION_OTA_SIZE							0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR							0x101000
#define SYSTEM_PARTITION_RF_CAL_ADDR						0x3fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR						0x3fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR				0x3fd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR           0xfc000
#else
#error "The flash map is not supported"
#endif

#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM                SYSTEM_PARTITION_CUSTOMER_BEGIN

uint32 priv_param_start_sec;

static const partition_item_t at_partition_table[] = {
    { SYSTEM_PARTITION_BOOTLOADER, 						0x0, 												0x1000},
    { SYSTEM_PARTITION_OTA_1,   						0x1000, 											SYSTEM_PARTITION_OTA_SIZE},
    { SYSTEM_PARTITION_OTA_2,   						SYSTEM_PARTITION_OTA_2_ADDR, 						SYSTEM_PARTITION_OTA_SIZE},
    { SYSTEM_PARTITION_RF_CAL,  						SYSTEM_PARTITION_RF_CAL_ADDR, 						0x1000},
    { SYSTEM_PARTITION_PHY_DATA, 						SYSTEM_PARTITION_PHY_DATA_ADDR, 					0x1000},
    { SYSTEM_PARTITION_SYSTEM_PARAMETER, 				SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR, 			0x3000},
    { SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM,             SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR,          0x1000},
};

void ICACHE_FLASH_ATTR user_pre_init(void)
{
    if(!system_partition_table_regist(at_partition_table, sizeof(at_partition_table)/sizeof(at_partition_table[0]),SPI_FLASH_SIZE_MAP)) {
		os_printf("system_partition_table_regist fail\r\n");
		while(1);
	}
}
struct espconn user_tcp_conn;

ETSTimer AP_check;
/***************************接收到station的连接每2s进入一次**********************************/
void ICACHE_FLASH_ATTR Wifi_conned(void *arg){
	static uint8 count=0;
	uint8 status;
	os_timer_disarm(&AP_check);
	count++;
	//获取ESP8266连接的station的IP
	status=wifi_station_get_connect_status();
	if(status==STATION_GOT_IP)
	{
		os_printf("Wifi connect success\r\n");
		Http_Read_File("https://www.baidu.com/");
	}
	else
	{
		os_timer_arm(&AP_check,2000,NULL);
		if(count>=7)
		{
			os_printf("Wifi connect fail\r\n");
			count=0;
		}
	}

}
void to_connect(void) {

	os_timer_disarm(&AP_check);
	/*对于同一个 timer，os_timer_arm 或 os_timer_arm_us 不能重复调用，必须先 os_timer_disarm。
	 *os_timer_setfn 必须在 timer 未使能的情况下调用，在 os_timer_arm 或 os_timer_arm_us之前或者 os_timer_disarm之后。*/
	os_timer_setfn(&AP_check,Wifi_conned,NULL);
	os_timer_arm(&AP_check,2000,NULL);
}
u16 N_Data_FLASH_SEC=0x77;
/*字符串必须存在以char生成的数组*/
u8 A_W_Data[16]="0123456789";
u32 A_R_Data[16]={0};//缓存读取Flash的数据
/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
user_init(void)
{
	uart_init(115200,115200);
	os_printf("\r\n--------------------------------\r\n");
	os_printf("SDK version:%s", system_get_sdk_version());//串口打印
	os_printf("\r\n--------------------------------\r\n");

	spi_flash_erase_sector(0x77);//写入前要对相应扇区进行擦除
	spi_flash_write(0x77*4096,(uint32*)A_W_Data,sizeof(A_W_Data));
	os_printf("\r\n----------Write Flash Data OVER-----------\r\n");

	spi_flash_read(0x77*4096,(uint32*)A_R_Data,sizeof(A_R_Data));
	os_printf("Read Data=%s",A_R_Data);
	os_printf("\r\n-------------Read Flash OVER--------------\r\n");
/*wifi模式设置和连接AP要在user_init中进行
 * 否则离开user_init时会自动设置为AP模式，ESP8266会出现短暂的不受控制的AP模式阶段，
 * 并且进入回调函数再设置成Station再连接到其他AP或路由器上的成功率很低*/

	wifi_set_opmode_current(STATION_MODE);
	struct station_config Sta_config;
	//	SSID:	MERCURY_247F82
	//	协议:	Wi-Fi 4 (802.11n)
	//	安全类型:	WPA2-个人
	//	网络频带:	2.4 GHz
	//	网络通道:	1
	//	本地链接 IPv6 地址:	fe80::40a3:52ef:364b:aa79%9
	//	IPv4 地址:	192.168.1.103
	//	IPv4 DNS 服务器:	101.198.198.198
	//	114.114.114.114
	//	制造商:	Qualcomm Atheros Communications Inc.
	//	描述:	Qualcomm Atheros AR956x Wireless Network Adapter
	//	驱动程序版本:	3.0.2.201
	//	物理地址(MAC):	18-CF-5E-A1-93-EB
		os_memcpy(&Sta_config.ssid, "MERCURY_247F82", strlen("MERCURY_247F82")+1);
		os_memcpy(&Sta_config.password, "090066076087", strlen("090066076087")+1);

		/*os_memcpy(&Sta_config.ssid, "HUAWEI", strlen("HUAWEI")+1);
		os_memcpy(&Sta_config.password, "13166982258", strlen("13166982258")+1);*/

		wifi_station_set_config_current(&Sta_config);
		wifi_station_connect();
	system_init_done_cb(to_connect);
}

