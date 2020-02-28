/*
 * my_http.c
 *
 *  Created on: 2020��2��13��
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
	/*sizeof()�ŵ��������������õ������������ݵ��ڴ��С���ֽ�Ϊ���㵥Ԫ��
	 * �����ָ�����ָ��һ�����ݵ�ָ����ô�õ���ֻ��ָ����ռ�ڴ�Ĵ�С*/
	memset(host,0,sizeof(host));
	memset(filename,0,sizeof(filename));
	os_printf("\r\n----------URL Parse start-----------\r\n");
	*port=0;
/*���URL�Ƿ�Ϊ��ָ�������ݣ�����������ú�����������*/
	if(!(*URL)){os_printf("URL is empty\r\n");return;}
    PA=URL;
/*	�ȶ�ͷ��λ�Ƿ�Ϊ��׼Э����"http://"//��"https://"�ƶ�ָ��PA������
 * ��δ��������PA����ԭ��ָ��URLͷ��λ��*/
	if(!strncmp(URL,"http://",strlen("http://")))
	{
		PA=URL+strlen("http://");
	}
	if(!strncmp(URL,"https://",strlen("https://")))
	{
		PA=URL+strlen("https://");
	}
	/*	����'/'ȷ��PB��λ��   */
	PB=strchr(PA,'/');
	/*	������'/'λ��ͨ��PA��PB��host��filename�������� */
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
		os_printf("Didn't find��\'\\\'\r\n");
		strncpy(host,PA,strlen(PA));
		host[strlen(PA)]='\0';
	}
	/*	������':'λ�� */
	PA=strchr(PA,':');
	/*	��port�����ݿ������� */
	if(PA)
	{
		*port=atoi(PA+1);
	}
	else
	{
		*port=80;
		os_printf("Default port value��%d\r\n",*port);
	}
	os_printf("\r\n----------URL Parse finish-----------\r\n");
};

void ICACHE_FLASH_ATTR user_esp_platform_dns_found(const char * name,ip_addr_t * ipaddr,void *arg)
{
	struct	espconn	*pespconn	=	(struct	espconn	*)arg;
	if	(ipaddr	!=	NULL)
	{
	os_printf("user_esp_platform_dns_found��%d.%d.%d.%d\n",
	*((uint8*)&ipaddr->addr),*((uint8*)&ipaddr->addr+1),*((uint8*)&ipaddr->addr	+2),*((uint8*)&ipaddr->addr	+3));

	/*------------------------ʹ��TCP���ӷ�����------------------------*/
	tcp_client_init(ipaddr,port);
	}


}
void ICACHE_FLASH_ATTR Http_Read_File(char *URL)
{
	ip_addr_t site_server_ip;

	/*---------------------����URL��ȡ���� ���ļ����� �˿ں�-------------------------*/
	http_parse_request_url(URL,host,filename,&port);
	os_printf("host: %s\r\n",host);

	/*--------------------------------��ȡ������IP��ַ--------------------------------------*/
	espconn_gethostbyname(&user_tcp_conn,host,&site_server_ip,user_esp_platform_dns_found);
}
