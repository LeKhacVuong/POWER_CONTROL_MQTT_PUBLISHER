//
// Created by vnbk on 17/03/2023.
//

#ifndef CO_LIB_TCP_CLIENT_H
#define CO_LIB_TCP_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif
    
typedef struct tcp_client tcp_client_t;

tcp_client_t* tcp_client_create();

tcp_client_t* tcp_client_create_default();

void tcp_client_destroy(tcp_client_t* self);

int tcp_client_connect(tcp_client_t* self, const char* host, int port);

int tcp_client_disconnect(tcp_client_t* self);

int tcp_client_is_connected(tcp_client_t* self);

int tcp_client_send(tcp_client_t* self, unsigned char* data, int len, int timeout);

int tcp_client_recv(tcp_client_t* self, unsigned char* buf, int max_len, int timeout);

#ifdef __cplusplus
}
#endif


#endif //CO_LIB_TCP_CLIENT_H
