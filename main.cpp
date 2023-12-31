#include <stdio.h>
#include <mosquitto.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <ctime>
#include "host/sm_host.h"
#include "utils/linux/linux_serial.h"
#include "fcntl.h"
#include <thread>
#include <chrono>
#include "sm_host.h"
#include "sm_timer.h"
#include "time.h"

#define VOL_CMD 1
#define CUR_CMD 2
#define COS_CMD 3
#define FEQ_CMD 4
#define ENE_CMD 5
#define Q_CMD   6
#define P_CMD   7


using namespace std;
//#define M_MQTT_HOST_ADDR  "localhost"
#define M_MQTT_HOST_ADDR "broker.hivemq.com"
#define M_MQTT_HOST_USER  "admin"
#define M_MQTT_HOST_PASS  "admin13589"
#define M_MQTT_PORT        1883
#define M_HOST_PORT       "/dev/ttyUSB0"


int32_t g_fd;
sm_host_t* host_master;
struct mosquitto *m_mosq;
char * now_time;
uint8_t g_buff[100] = "TEST HOST";

void on_connect(struct mosquitto *_mosq, void *_obj, int _reason_code)
{
    printf("Mqtt on_connect: %s\n", mosquitto_connack_string(_reason_code));
    if(_reason_code != 0){
        printf("Mqtt connect to sever fail. \n");
        mosquitto_disconnect(_mosq);
    }
}


float voltage = 0;
float current = 0;
float cos_phi = 0;
float frequency = 0;
float energy = 0;
float p_value = 0;
float q_value = 0;

void on_publish(struct mosquitto *_mosq, void *_obj, int _mid)
{
//    printf("Message mqtt with mid %d has been published.\n", _mid);
}

int32_t host_send_data_interface(const uint8_t* _data, int32_t _len) {
    serial_send_bytes(g_fd,_data,_len);
    return _len;
}

int32_t get_tick_count(){
    struct timespec ts = {0, 0};
    clock_gettime(CLOCK_REALTIME, &ts);
    return (int32_t)(ts.tv_sec*1000 + ts.tv_nsec/1000000);
}

uint8_t count = 0;

int32_t host_receive_cmd_callback(int32_t _cmd, const uint8_t* _data, int32_t _len, void* _arg ) {
//    printf("receive cmd 0x%x\n",_cmd);
    if(_cmd == VOL_CMD){
//        printf("Voltage U: %3.2f\n",*(float*)_data);
        voltage = *(float*)_data;
    }
    if(_cmd == CUR_CMD){
//        printf("Current I: %f3.2\n",*(float*)_data);
        current = *(float*)_data;
    }
    if(_cmd == COS_CMD){
//        printf("Cos A: %f3.2\n",*(float*)_data);
        cos_phi = *(float*)_data;
    }
    if(_cmd == FEQ_CMD){
//        printf("Frequency F: %f3.2\n",*(float*)_data);
        frequency = *(float*)_data;
    }
    if(_cmd == Q_CMD){
//        printf("Reactive power Q: %f3.2\n",*(float*)_data);
        q_value = *(float*)_data;
    }
    if(_cmd == P_CMD){
//        printf("Consume power P: %f3.2\n",*(float*)_data);
        p_value = *(float*)_data;
    }
    if(_cmd == ENE_CMD){
//        printf("Energy value W: %f3.2\n",*(float*)_data);
        energy = *(float*)_data;
        count++;
        if(count >= 2){
            count = 0;
//            printf("send 1 msg to mqtt broker\n");
            char payload[256] ;
//            printf("Send mqtt: VOL: %f3.2 \nCUR: %f3.2 \nCOS: %f3.2 \nFRQ: %f3.2 \nP  : %f3.2 \nQ  : %f3.2 \nENE: %f3.2 \n \n",voltage,current,cos_phi,frequency,p_value,q_value,energy);
            sprintf(payload,"VOL: %f3.2 \nCUR: %f3.2 \nCOS: %f3.2 \nFRQ: %f3.2 \nP  : %f3.2 \nQ  : %f3.2 \nENE: %f3.2 \n \n",voltage,current,cos_phi,frequency,p_value,q_value,energy);
            int8_t rc = mosquitto_publish(m_mosq, NULL, "Project2", strlen(payload), payload, 2, false);
            if(rc != MOSQ_ERR_SUCCESS){
                printf("Error publishing.\n");
            }
        }
    }
    return 0;
}

void recv_host_thread(void){
    while(1){
        uint8_t packet[2048] = {0,};
        int ret = serial_recv_bytes(g_fd,packet,2048);
        if(ret > 0){
            sm_host_asyn_feed(packet, ret, host_master);
//            printf("Feed %d byte host data\n",ret);
        }
        sm_host_process(host_master);
    }
}

int main() {
    int rc;
    if (system("echo 0528 | sudo -S chmod 777 /dev/ttyUSB0") == -1) {
        perror("error sudo\n");
    }
    g_fd = serial_init(M_HOST_PORT,115200,false);
    if(g_fd < 0){
        printf("Could NOT open serial device file: %s\n", M_HOST_PORT);
        return -1;
    }else{
        printf("Just open success serial device file: %s with fd = %d\n", M_HOST_PORT,g_fd);
    }
    mosquitto_lib_init();
    m_mosq = mosquitto_new("vuong.lk", true, NULL);
    mosquitto_username_pw_set(m_mosq,M_MQTT_HOST_USER,M_MQTT_HOST_PASS);
    if(m_mosq == NULL){
        printf("Error: Cant create mosq module, out of memory.\n");
        return 1;
    }
    mosquitto_connect_callback_set(m_mosq, on_connect);
    mosquitto_publish_callback_set(m_mosq, on_publish);
    rc = mosquitto_connect(m_mosq, M_MQTT_HOST_ADDR, M_MQTT_PORT, 600);
    if(rc != MOSQ_ERR_SUCCESS){
        mosquitto_destroy(m_mosq);
        printf("Error: %s\n", mosquitto_strerror(rc));
        return 1;
    }
    printf("Connect success to broker!!!\n");
    rc = mosquitto_loop_start(m_mosq);
    if(rc != MOSQ_ERR_SUCCESS){
        mosquitto_destroy(m_mosq);
        printf("Start mqtt loop fail\n");
        return 1;
    }
    host_master = sm_host_create(SM_HOST_ADDR_DEFAULT,host_send_data_interface);
    sm_host_reg_handle(host_master,host_receive_cmd_callback,NULL);
    int ret;
    do{
        uint8_t packet[2048] = {0,};
        ret = serial_recv_bytes(g_fd,packet,2048);
    }while(ret < 5);
    std::thread Receive_data_thread(recv_host_thread);
    sleep(2);

    while(1){
        printf("\n/*************** Power management *****************/\n");
        printf(" Please choose the command:\n ");
        printf("Choose 1 to get voltage\n");
        printf(" Choose 2 to get current\n");
        printf(" Choose 3 to get cos alpha\n");
        printf(" Choose 4 to get frequency\n");
        printf(" Choose 5 to get real power\n");
        printf(" Choose 6 to get reactive power \n");
        printf(" Choose 7 to get total energy\n");

        int c = getchar();
        while (getc(stdin) != '\n');
        switch (c) {
            case '1':
                printf("Voltage U: %3.2f",voltage);
                break;
            case '2':
                printf("Current I: %3.2f",current);
                break;
            case '3':
                printf("Cos alpha: %3.2f",cos_phi);
                break;
            case '4':
                printf("Frequency f: %3.2f",frequency);
                break;
            case '5':
                printf("Real power P: %3.2f",p_value);
                break;
            case '6':
                printf("Reactive power P: %3.2f",q_value);
                break;
            case '7':
                printf("Total energy E: %3.2f",energy);
                break;
            default:
                printf("Please enter right character!!!\n");
                break;
        }
    }
}
