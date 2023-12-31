//
// Created by vnbk on 07/04/2023.
//
#include <stdio.h>

#include <unistd.h>
#include <fcntl.h> // Contains file controls like O_RDWR
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <sys/ioctl.h>
#include <sys/select.h>
#include <err.h>
#include <errno.h>

#include "linux_serial.h"

int32_t serial_init(const char* _port_name, int32_t _baud_rate, uint8_t _flag){
    int serial_port = -1;

    if(_flag & SERIAL_FLAG_BLOCKING){
        serial_port = open(_port_name, O_RDWR | O_NOCTTY ); //| O_NDELAY
    }else{
        serial_port = open(_port_name, O_RDWR | O_NOCTTY | O_NDELAY | O_SYNC);
    }

    if(serial_port < 0){
        printf("Could NOT open serial device file: %s\n", _port_name);
        return -1;
    }

    return serial_config(serial_port, _baud_rate);
}

int32_t serial_free(int32_t _fd){
    if(_fd > 0){
        printf("Close Serial Port\n");
        close(_fd);
    }
    return 0;
}

int32_t serial_config(int32_t _fd, int32_t _baud_rate){
    if( _fd < 0){
        return -1;
    }
    // Create new termios struct, we call it 'tty' for convention
    struct termios tty;

    // Read in existing settings, and handle any error
    if(tcgetattr(_fd, &tty) != 0) {
        return -1;
    }

    tty.c_cflag &= ~(PARENB | PARODD); // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
    tty.c_cflag |= CS8; // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= (CREAD | CLOCAL); // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
    // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

    tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    // Set in/out baud rate to be 9600
    switch (_baud_rate) {
        case 9600:
            cfsetispeed(&tty, B9600);
            cfsetospeed(&tty, B9600);
            break;
        case 19200:
            cfsetispeed(&tty, B19200);
            cfsetospeed(&tty, B19200);
            break;
        case 38400:
            cfsetispeed(&tty, B38400);
            cfsetospeed(&tty, B38400);
            break;
        case 57600:
            cfsetispeed(&tty, B57600);
            cfsetospeed(&tty, B57600);
            break;
        case 115200:
            cfsetispeed(&tty, B115200);
            cfsetospeed(&tty, B115200);
            break;
        case 230400:
            cfsetispeed(&tty, B230400);
            cfsetospeed(&tty, B230400);
            break;
        case 460800:
            cfsetispeed(&tty, B460800);
            cfsetospeed(&tty, B460800);
            break;
        case 921600:
            cfsetispeed(&tty, B921600);
            cfsetospeed(&tty, B921600);
            break;
        default:
            cfsetispeed(&tty, B115200);
            cfsetospeed(&tty, B115200);
            break;
    }

    // Save tty settings, also checking for error
    if (tcsetattr( _fd, TCSANOW, &tty) != 0) {
        return -1;
    }
    printf("Setup serial %d baud rate: %d\n", _fd, _baud_rate);

    return _fd;
}

bool serial_is_open(int32_t _fd){
    return  _fd > 0 ? true : false;
}

int32_t serial_data_available(int32_t _fd){
    int bytes = -1;
    ioctl( _fd, FIONREAD, &bytes);
    return bytes;
}

int32_t serial_flush(int32_t _fd){
    if( _fd > 0) {
        tcflush(_fd, TCIFLUSH);
    }
    return 0;
}

int32_t serial_send_byte(int32_t _fd, uint8_t _data){
    if( _fd > 0){
        return (int32_t)write( _fd, &_data, 1);
    }
    return -1;
}

int32_t serial_recv_byte(int32_t _fd, uint8_t* _data){
    if( _fd > 0){
        return (int32_t)read( _fd, &_data, 1);
    }
    return -1;
}

int32_t serial_send_bytes(int32_t _fd, const uint8_t* _data, int32_t _len){
    if( _fd > 0){
        return (int32_t)write( _fd, _data, (size_t)_len);
    }
    return -1;
}

int32_t serial_recv_bytes(int32_t _fd, uint8_t* _data, int32_t _max_len){
    if( _fd > 0){
        return (int32_t)read( _fd, _data, _max_len);
    }
    return -1;
}

int32_t serial_recv_bytesz(int32_t _fd, uint8_t* _data, int32_t _max_len, int32_t _timeout){
    if( _fd < 0){
        return -1;
    }
    fd_set set;
    struct timeval timeout = {
            .tv_sec = _timeout/1000,
            .tv_usec = (_timeout%1000)*1000
    };
    FD_ZERO(&set);       //clear the set
    FD_SET(_fd, &set);   //add our file descriptor to the set

    int ret = select(_fd + 1, &set, NULL, NULL, &timeout);
    if(ret > 0){
        return (int32_t)read( _fd, _data, _max_len );
    }
    return ret;

   /* fd_set set;
    struct timeval timeout = {
            .tv_sec = _timeout/1000,
            .tv_usec = (_timeout%1000)*1000
    };
    FD_ZERO(&set);
    FD_SET(_fd, &set);

    if(select(_fd + 1, &set, NULL, NULL, &timeout) <= 0){
        return -1;
    }

    int bytes = 0;
    int i = 0;
    const int max_tries = 10;
    while (bytes < _max_len)
    {
        int rc = read(_fd, &_data[bytes], (_max_len - bytes));
        if (rc == -1)
        {
            if (errno != EAGAIN && errno != EWOULDBLOCK)
                bytes = -1;
            break;
        }
        else
            bytes += rc;
        if (++i >= max_tries)
            break;
        if (rc == 0)
            break;
    }

    return bytes;*/

}
