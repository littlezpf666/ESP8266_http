/*
 * my_http.c
 *
 *  Created on: 2020年2月13日
 *      Author: 16130
 */
#include"my_http.h"
extern struct espconn user_tcp_conn;
char host[32];
char filename[128];
char port;
void http_parse_request_url(char*URL,char*host,char*filename,char*port){
	char *PA;
	char *PB;
	/*sizeof()放的是数组名，将得到数组所存数据的内存大小以字节为结算单元，
	 * 而如果指向的是指向一段数据的指针那么得到的只是指针所占内存的大小*/
	memset(host,0,sizeof(host));
	memset(filename,0,sizeof(filename));
	os_printf("\r\n----------URL Parse start-----------\r\n");
	*port=0;
/*检查URL是否为空指针无内容，如果是跳出该函数，不继续*/
	if(!(*URL)){os_printf("URL is empty\r\n");return;}
    PA=URL;
/*	比对头几位是否为标准协议名"http://"//或"https://"移动指针PA至域名
 * 如未搜索到则PA保持原有指向URL头的位置*/
	if(!strncmp(URL,"http://",strlen("http://")))
	{
		PA=URL+strlen("http://");
	}
	if(!strncmp(URL,"https://",strlen("https://")))
	{
		PA=URL+strlen("https://");
	}
	/*	搜索'/'确定PB的位置   */
	PB=strchr(PA,'/');
	/*	搜索到'/'位置通过PA和PB将host和filename拷贝出来 */
	if(PB)
	{
		strncpy(host,PA,strlen(PA)-strlen(PB));
		host[strlen(PA)-strlen(PB)]='\0';
		if(PB+1)
		{
			strncpy(filename,PB+1,strlen(PB+1));
			filename[strlen(PB+1)]='\0';
		}
	}
	else
	{
		os_printf("Didn't find：\'\\\'\r\n");
		strncpy(host,PA,strlen(PA));
		host[strlen(PA)]='\0';
	}
	/*	搜索到':'位置 */
	PA=strchr(PA,':');
	/*	将port的内容拷贝出来 */
	if(PA)
	{
		*port=atoi(PA+1);
	}
	else
	{
		*port=80;
		os_printf("Default port value：%d\r\n",*port);
	}
	os_printf("\r\n----------URL Parse finish-----------\r\n");
};

void ICACHE_FLASH_ATTR user_esp_platform_dns_found(const char * name,ip_addr_t * ipaddr,void *arg)
{
	struct	espconn	*pespconn	=	(struct	espconn	*)arg;
	if	(ipaddr	!=	NULL)
	{
	os_printf("user_esp_platform_dns_found：%d.%d.%d.%d\n",
	*((uint8*)&ipaddr->addr),*((uint8*)&ipaddr->addr+1),*((uint8*)&ipaddr->addr	+2),*((uint8*)&ipaddr->addr	+3));

	/*------------------------使用TCP连接服务器------------------------*/
	tcp_client_init(ipaddr,port);
	}


}
void ICACHE_FLASH_ATTR Http_Read_File(char *URL)
{
	ip_addr_t site_server_ip;

	/*---------------------解析URL获取域名 、文件名、 端口号-------------------------*/
	http_parse_request_url(URL,host,filename,&port);
	os_printf("host: %s\r\n",host);

	/*--------------------------------获取服务器IP地址--------------------------------------*/
	espconn_gethostbyname(&user_tcp_conn,host,&site_server_ip,user_esp_platform_dns_found);
}
