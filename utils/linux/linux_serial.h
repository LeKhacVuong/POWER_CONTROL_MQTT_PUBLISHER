//
// Created by vnbk on 07/04/2023.
//

#ifndef BOOT_MASTER_LINUX_SERIAL_H
#define BOOT_MASTER_LINUX_SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define SERIAL_NUMBER  3

#define SERIAL_PORT_COM0    "/dev/ttyUSB0"
#define SERIAL_PORT_COM1    "/dev/ttyUSB1"
#define SERIAL_PORT_COM2    "/dev/ttyUSB2"

#define SERIAL_FLAG_BLOCKING    0x01

int32_t serial_init(const char* _port_name, int32_t _baud_rate, uint8_t _flag);
int32_t serial_free(int32_t _fd);
int32_t serial_config(int32_t _fd, int32_t _baud_rate);
bool    serial_is_open(int32_t _fd);
int32_t serial_data_available(int32_t _fd);
int32_t serial_flush(int32_t _fd);
int32_t serial_send_byte(int32_t _fd, uint8_t _data);
int32_t serial_recv_byte(int32_t _fd, uint8_t* _data);
int32_t serial_send_bytes(int32_t _fd, const uint8_t* _data, int32_t _len);
int32_t serial_recv_bytes(int32_t _fd, uint8_t* _data, int32_t _max_len);
int32_t serial_recv_bytesz(int32_t _fd, uint8_t* _data, int32_t _max_len, int32_t _timeout);

#ifdef __cplusplus
};
#endif

#endif //BOOT_MASTER_LINUX_SERIAL_H
