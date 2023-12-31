//
// Created by vnbk on 23/03/2023.
//

#ifndef SELEX_BOOTLOADER_SM_TIMER_H
#define SELEX_BOOTLOADER_SM_TIMER_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern int32_t get_tick_count();

typedef struct sm_elapsed_timer{
    int32_t m_duration;
    int32_t m_start_time;
}sm_elapsed_timer_t;

static inline void sm_elapsed_timer_reset(sm_elapsed_timer_t* _self){
    _self->m_start_time = get_tick_count();
}

static inline void sm_elapsed_timer_resetz(sm_elapsed_timer_t* _self, int32_t _duration){
    _self->m_duration = _duration;
    _self->m_start_time = get_tick_count();
}

static inline int32_t sm_elapsed_timer_get_remain(sm_elapsed_timer_t* _self){
    int32_t remain_time = get_tick_count() - _self->m_start_time;
    if(remain_time >= _self->m_duration){
        return 0;
    }else{
        return _self->m_duration - remain_time;
    }
}

#ifdef __cplusplus
};
#endif

#endif //SELEX_BOOTLOADER_SM_TIMER_H
