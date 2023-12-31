//
// Created by vnbk on 24/03/2023.
//

#ifndef SELEX_BOOTLOADER_SM_ERR_H
#define SELEX_BOOTLOADER_SM_ERR_H

#ifdef __cplusplus
extern "C" {
#endif

#define SM_ERR_NONE             (0)
#define SM_ERR_COMMON           (-1)
#define SM_ERR_UNKNOWN          (-2)
#define SM_ERR_TIMEOUT          (-3)
#define SM_ERR_BUSY             (-4)
#define SM_ERR_MEM_ALLOCATED    (-5)

#ifdef __cplusplus
};
#endif


#endif //SELEX_BOOTLOADER_SM_ERR_H
