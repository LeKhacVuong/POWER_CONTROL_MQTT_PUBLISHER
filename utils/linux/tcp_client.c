//
// Created by vnbk on 17/03/2023.
//
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __linux
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#ifdef _WIN32
#include <Ws2tcpip.h>
#include <WinSock2.h>
#endif

#include "tcp_client.h"

struct tcp_client{
    int fd;
};

static tcp_client_t g_tcp_client_df = {
        .fd = -1
};

tcp_client_t* tcp_client_create(){
    tcp_client_t * obj = malloc(sizeof(tcp_client_t));
    if(!obj){
        return NULL;
    }
    obj->fd = -1;

    return obj;
}

tcp_client_t* tcp_client_create_default(){
    g_tcp_client_df.fd = -1;

    return &g_tcp_client_df;
}

void tcp_client_destroy(tcp_client_t* self){
    if(self){
        tcp_client_disconnect(self);
        free(self);
    }
}

int tcp_client_connect(tcp_client_t* self, const char* host_name, int port){
    struct sockaddr_in serv_addr;
    int rc = -1;
    if(self){
        if(tcp_client_is_connected(self)){
            tcp_client_disconnect(self);
        }

        struct addrinfo *result = NULL;
        struct hostent *host;
        struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};

        if ((rc = getaddrinfo(host_name, NULL, &hints, &result)) == 0)
        {
            struct addrinfo* res = result;

            bzero(&serv_addr, sizeof(serv_addr));
            /* prefer ip4 addresses */
            while (res)
            {
                if (res->ai_family == AF_INET)
                {
                    result = res;
                    break;
                }
                res = res->ai_next;
            }

            if (result->ai_family == AF_INET)
            {
                if ( (host = gethostbyname(host_name)) == NULL )
                {
                    perror(host_name);
                    freeaddrinfo(result);
                    return -1;
                }

                serv_addr.sin_port = htons(port);
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_addr = ((struct sockaddr_in*)(result->ai_addr))->sin_addr;
                serv_addr.sin_addr.s_addr = *(long*)(host->h_addr);
            }
            else
                rc = -1;

            freeaddrinfo(result);
        }

        if(!rc) {
            int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (fd < 0) {
                return -1;
            }

            if (connect(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
                printf("\nConnection Failed \n");
                return -1;
            }

            self->fd = fd;
            return fd;
        }
    }
    return -1;
}

int tcp_client_disconnect(tcp_client_t* self){
    if(self){
        if(self->fd <0)
            return 0;

        close(self->fd);
        self->fd = -1;
    }
    return -1;
}

int tcp_client_is_connected(tcp_client_t* self){
    return (self && self->fd < 0) ? 0 : 1;
}

int tcp_client_send(tcp_client_t* self, unsigned char* data, int len, int timeout){
    if(self && tcp_client_is_connected(self)){
        struct timeval tv = {timeout/1000, (timeout%1000)*1000};

        setsockopt(self->fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv,sizeof(struct timeval));
        int	rc = write(self->fd, data, len);
        return rc;
    }
    return -1;
}

int tcp_client_recv(tcp_client_t* _self, unsigned char* _buf, int _max_len, int ms){
    if(_self){
        struct timeval timeout = {
                .tv_sec = ms/1000,
                .tv_usec = (ms%1000)*1000};

        int ret = setsockopt(_self->fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
        if(ret < 0){
            printf("Error: ");
            return -1;
        }

        int bytes = 0;
        int i = 0; const int max_tries = 10;
        while (bytes < _max_len)
        {
            int rc;
            rc = recv(_self->fd, &_buf[bytes], (_max_len - bytes), 0);
            if (rc == -1)
            {
                if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    bytes = -1;
                }
                break;
            }
            else
                bytes += rc;
            if (++i >= max_tries)
                break;
            if (rc == 0)
                break;
        }

        if( bytes < 0){
            printf("Error occurred when received message, Disconnected session\n");
            tcp_client_disconnect(_self);
        }
        return bytes;

    }
    return -1;
}