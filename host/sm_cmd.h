//
// Created by vnbk on 21/03/2023.
//

#ifndef SELEX_HOST_SM_CMD_H
#define SELEX_HOST_SM_CMD_H

#ifdef __cplusplus
extern "C" {
#endif

/// Selex Motor define Command
#define SM_CMD_UPGRADE_FW               0x08
#define SM_SUB_CMD_REQUEST              0x00
#define SM_SUB_CMD_FW_INFO              0x01
#define SM_SUB_CMD_FW_CHECK_READY       0x02
#define SM_SUB_CMD_SEND_SEGMENT_INFO    0x03
#define SM_SUB_CMD_SEND_SEGMENT_DATA    0x04
#define SM_SUB_CMD_FINISH_SEGMENT       0x05
#define SM_SUB_CMD_FINISH_UPGRADE       0x06

#define SM_CMD_REBOOT           0x10
#define SM_CMD_RESET_FACTORY    0x11
#define SM_CMD_GET_VERSION      0x12

/// Selex Motor define command response

#define SM_RES_NONE                     0x00
#define SM_RES_ERR_COMMON               0x01
#define SM_RES_ERR_UNKNOWN              0x02
#define SM_RES_ERR_WRONG_CRC            0x03
#define SM_RES_ERR_DEVICE_NOT_MATCH     0x04

#ifdef __cplusplus
};
#endif

#endif //SELEX_HOST_SM_CMD_H
