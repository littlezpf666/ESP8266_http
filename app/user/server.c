/*
 * server.c
 *
 *  Created on: 2019��6��12��
 *      Author: pengfeizhao
 */
#include "server.h"

extern struct espconn user_tcp_conn;
void ICACHE_FLASH_ATTR server_recv_cb(void*arg, char*pdata, unsigned short len) {
	struct espconn *pesp_conn = arg;
	os_printf("%s", pdata);
	/*
	 * espconn_send(pesp_conn, pdata, os_strlen(pdata));//�ش�
	 * os_printf("Recv��remote_port:%d,remote_IP:"IPSTR"\r\n", pesp_conn->proto.tcp->remote_port
	 ,IP2STR(pesp_conn->proto.tcp->remote_ip));
	 */
}
/*TCP���ͻص�������
 * һ�����������ǰһ�����ݷ��ͳɹ������� espconn_sent_callback ���ٵ��� espconn_send ������һ������*/
void ICACHE_FLASH_ATTR server_sent_cb(void*arg) {
	os_printf("���ͳɹ�\r\n");
}
/*TCP�Ͽ��ص�������
 * WiFi�ĶϿ��ƺ������ᴥ���ûص�*/
void ICACHE_FLASH_ATTR server_discon_cb(void*arg) {

	os_printf("�Ͽ�����\r\n");
}
/*TCP���ӻص�����*/
void ICACHE_FLASH_ATTR server_listen_cb(void *arg) {
	struct espconn *pesp_conn = arg;

	os_printf("\r\nListen:remote_port:%d,remote_IP:"IPSTR"\r\n",
			pesp_conn->proto.tcp->remote_port
			,IP2STR(pesp_conn->proto.tcp->remote_ip));
	espconn_regist_recvcb((struct espconn*) arg, server_recv_cb);
	espconn_regist_sentcb((struct espconn*) arg, server_sent_cb);
	espconn_regist_disconcb((struct espconn*) arg, server_discon_cb);
}
void ICACHE_FLASH_ATTR server_reconn_cb(void *arg, sint8 err) {
	os_printf("���Ӵ��󣬴������Ϊ%d\r\n", err);
}


void ICACHE_FLASH_ATTR server_init(int port) {

	//espconn��������
	user_tcp_conn.type = ESPCONN_TCP;
	user_tcp_conn.state = ESPCONN_NONE;
	user_tcp_conn.proto.tcp = (esp_tcp *) os_zalloc(sizeof(esp_tcp));
	user_tcp_conn.proto.tcp->local_port = port;

	//ע�����Ӻ������ӻص�����
	espconn_regist_connectcb(&user_tcp_conn, server_listen_cb);
	espconn_regist_reconcb(&user_tcp_conn, server_reconn_cb);

	//��������
	espconn_accept(&user_tcp_conn);
	espconn_regist_time(&user_tcp_conn, 0, 0); //��ʱ�Ͽ�����ʱ�䣬�������Ҫ���÷����Ĭ��10s�����Զ��Ͽ���д0��������

}

