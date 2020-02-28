/*
 * client.c
 *
 *  Created on: 2020��2��19��
 *      Author: 16130
 */
#include "client.h"

extern struct espconn user_tcp_conn;
void ICACHE_FLASH_ATTR station_recv_cb(void*arg, char*pdata, unsigned short len) {

	os_printf("Accept Data:%s\r\n", pdata);

}
/*TCP���ͻص�������
 * һ�����������ǰһ�����ݷ��ͳɹ������� espconn_sent_callback ���ٵ��� espconn_send ������һ������*/
void ICACHE_FLASH_ATTR station_sent_cb(void*arg) {
	os_printf("Station send successfully\r\n");
}
/*TCP�Ͽ��ص�������
 * WiFi�ĶϿ��ƺ������ᴥ���ûص�*/
void ICACHE_FLASH_ATTR station_discon_cb(void*arg) {

	os_printf("Station  disconnect abnormally\r\n");
}

void client_conned_server_cb (void	*arg)
{
	struct espconn *pesp_conn = arg;
	os_printf("\r\n----------TCP has connected to server-----------\r\n");

	espconn_regist_recvcb((struct espconn*) arg, station_recv_cb);
	espconn_regist_sentcb((struct espconn*) arg, station_sent_cb);
	espconn_regist_disconcb((struct espconn*) arg, station_discon_cb);
	espconn_send(pesp_conn, HEAD, os_strlen(HEAD));

}

void client_reconn_cb(void	*arg,sint8 err)
{
	os_printf("TCP failed to connected to server,error:%d\r\n",err);
}
void ICACHE_FLASH_ATTR tcp_client_init(ip_addr_t *remote_addr,int remote_port)
{
	user_tcp_conn.type = ESPCONN_TCP;
	user_tcp_conn.state = ESPCONN_NONE;

	user_tcp_conn.proto.tcp = (esp_tcp *) os_zalloc(sizeof(esp_tcp));
	memcpy(user_tcp_conn.proto.tcp->remote_ip, &remote_addr->addr,4);

	user_tcp_conn.proto.tcp->remote_port = remote_port;


	espconn_regist_connectcb(&user_tcp_conn,client_conned_server_cb);
	espconn_regist_reconcb (&user_tcp_conn,client_reconn_cb);
	espconn_connect(&user_tcp_conn) ;
}

