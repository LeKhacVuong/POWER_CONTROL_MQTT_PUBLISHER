//
// Created by vnbk on 29/03/2023.
//

#ifndef SELEX_BOOTLOADER_TCP_SERVER_H
#define SELEX_BOOTLOADER_TCP_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct tcp_server tcp_server_t;
typedef struct tcp_session tcp_session_t;

tcp_server_t* tcp_server_create_default(int32_t port);
int32_t tcp_server_destroy(tcp_server_t* _self);

int32_t tcp_server_listen(tcp_server_t* _self);
tcp_session_t* tcp_server_accepted(tcp_server_t* _self);
int32_t tcp_server_disconnect_session(tcp_server_t* _self, tcp_session_t* _session);


int32_t tcp_session_destroy(tcp_session_t* _self);
int32_t tcp_session_send(tcp_session_t* _self, const uint8_t* _data, int32_t _len, int32_t _ms);
int32_t tcp_session_recv(tcp_session_t* _self, uint8_t* _buf, int32_t _max_len, int32_t _ms);
int32_t tcp_session_is_connected(tcp_session_t* _self);
int32_t tcp_session_disconnect(tcp_session_t* _self);

#ifdef __cplusplus
};
#endif

#endif //SELEX_BOOTLOADER_TCP_SERVER_H
