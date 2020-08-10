#ifndef __OS_KERNEL_H
#define __OS_KERNEL_H

#include <stdint.h>
#include "stm32f303xe.h"

//Functions that main application can use
void osKernelStart(uint32_t quanta);
void osKernelInit(void);
uint8_t osKernelAddThread(void(*task0) (void),
                          void(*task1) (void),
                          void(*task2) (void));


#endif
