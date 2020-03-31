/*
 * client.h
 *
 *  Created on: 2020年2月19日
 *      Author: 16130
 */

#ifndef APP_INCLUDE_CLIENT_H_
#define APP_INCLUDE_CLIENT_H_
#include "includes.h"
#include "espconn.h"
//请求行 统一资源定位符 主机域名 首部行
#define HEAD "GET https://%s/HTTP/1.1\r\nHost:%s\r\nConnection:close\r\n\r\n"


void ICACHE_FLASH_ATTR tcp_client_init(ip_addr_t *remote_addr,int remote_port);

#endif /* APP_INCLUDE_CLIENT_H_ */
