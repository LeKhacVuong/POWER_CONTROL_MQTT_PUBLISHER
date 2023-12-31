//
// Created by vnbk on 29/03/2023.
//

#include <stdio.h>
#include <stdlib.h>

#include "tcp_server.h"
#include "sm_memory.h"

#ifdef __linux
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>

#endif

struct tcp_server{
    int m_fd;
    int32_t m_port;
};

struct tcp_session{
    int m_fd;
    struct sockaddr_in m_client;
};

static tcp_server_t g_tcp_server_default = {
        .m_port = 0,
        .m_fd = -1
};

tcp_server_t* tcp_server_create_default(int32_t port){
    g_tcp_server_default.m_fd = -1;
    g_tcp_server_default.m_port = port;
    return &g_tcp_server_default;
}
int32_t tcp_server_destroy(tcp_server_t* _self){
    if(_self){
        if(_self->m_fd){
            close(_self->m_fd);
        }
        mem_free(_self);
        return 0;
    }
    return -1;
}

int32_t tcp_server_listen(tcp_server_t* _self){
    struct sockaddr_in servaddr;

    // socket create and verification
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(_self->m_port);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd,  (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        return -1;
    }
        printf("Socket successfully binded..\n");

    /*
     * listen: make this socket ready to accept connection requests
     */
    if (listen(sockfd, 5) < 0){/* allow 5 requests to queue up */
        perror("ERROR on listen");
        return -1;
    }
    _self->m_fd = sockfd;
    return sockfd;
}

tcp_session_t* tcp_server_accepted(tcp_server_t* _self) {
    struct sockaddr_in cli;
    socklen_t len = sizeof(struct sockaddr_in);
    int childfd = accept(_self->m_fd, (struct sockaddr *) &cli, &len);
    if (childfd < 0){
        perror("ERROR on accept");
        return NULL;
    }

    tcp_session_t* session = mem_alloc(sizeof (tcp_session_t));
    session->m_fd = childfd;
    memcpy(&session->m_client, &cli, sizeof(cli));

    return session;
}

int32_t tcp_server_disconnect_session(tcp_server_t* _self, tcp_session_t* _session){
    if(_session){
        return tcp_session_disconnect(_session);
    }
    return -1;
}

int32_t tcp_session_destroy(tcp_session_t* _self){
    if(_self){
        mem_free(_self);
    }
    return 0;
}

int32_t tcp_session_send(tcp_session_t* _self, const uint8_t* _data, int32_t _len, int32_t _ms){
    int rc;
    struct timeval tv = { _ms/1000, (_ms%1000) * 1000};

    setsockopt(_self->m_fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv,sizeof(struct timeval));

    rc = send(_self->m_fd, _data, _len, 0);

    if(rc < 0){
        tcp_session_disconnect(_self);
    }

    return rc;

}
int32_t tcp_session_recv(tcp_session_t* _self, uint8_t* _buf, int32_t _max_len, int32_t _ms){
    struct timeval interval = {_ms/1000, (_ms % 1000) * 1000};

    setsockopt(_self->m_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&interval, sizeof(struct timeval));

//    return (int)recv(_self->m_fd, _buf, _max_len, 0);

    int bytes = 0;
    int i = 0; const int max_tries = 10;
    while (bytes < _max_len)
    {
        int rc;
        rc = recv(_self->m_fd, &_buf[bytes], (_max_len - bytes), 0);
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
        printf("Error occurred when received message, Disconnected session");
        tcp_session_disconnect(_self);
    }
    return bytes;

}

int32_t tcp_session_is_connected(tcp_session_t* _self){
    return _self->m_fd > 0 ? 1 : 0;
}
int32_t tcp_session_disconnect(tcp_session_t* _self){
   return close(_self->m_fd);
}