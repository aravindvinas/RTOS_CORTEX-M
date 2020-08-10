#include "osKernel.h"

#define SYSPRI3 (*((volatile uint32_t *)0xE000ED20))

#define THREAD_COUNT    3
#define STACK_SIZE  100

#define BUS_FREQ    80000000
uint32_t millis_pre;

struct thd_ctrl_blk{
    int32_t *stkPtr;    //top of stack pointer
    struct tcb *nxtPtr; //pointer to next thread
};

typedef struct thd_crtl_blk tcbType;    //Typdef for Thread Control Block
tcbType tcb_arr[THREAD_COUNT];  //thread  instances
tcbType *curPtr;    //pointer to currently executing thread

int32_t TCB_STACK[THREAD_COUNT] [STACK_SIZE];   //thread stack memory

//Function takes thread number
static osKernelStackInit(int i)
{
    tcb_arr[i].stkPtr = &TCB_STACK[i] [STACK_SIZE - 16];    //Top of stack
    TCB_STACK[i] [STACK_SIZE - 1] = 0x01000000; //xPSR reg indicating THUMB mode
}

uint8_t osKernelAddThread(void(*task0) (void), 
                          void(*task1) (void), 
                          void(*task2) (void))
{
    __disable_irq();    //disable all interrupts

    //creating a circular  linked-list of threads
    tcb_arr[0].nxtPtr = &tcb_arr[1];
    tcb_arr[1].nxtPtr = &tcb_arr[2];
    tcb_arr[2].nxtPtr = &tcb_arr[0];
    
    //pointing PC reg of all threads to the corresponding tasks.
    osKernelStackInit(0);
    TCB_STACK[0] [STACK_SIZE - 2] = (int32_t) (task0);
    osKernelStackInit(1);
    TCB_STACK[1] [STACK_SIZE - 2] = (int32_t) (task1);
    osKernelStackInit(2);
    TCB_STACK[2] [STACK_SIZE - 2] = (int32_t) (task2);

    __enable_irq(); //enable all interrupts

    return 1;
}

void osKernelInit(void)
{
    __disable_irq();
    millis_pre = BUS_FREQ/1000; //set SysTick prescaler
}

void osKernelStart(uint32_t quanta)
{
    //disable SysTick Timer
    SysTick->CTRL = 0;
    SysTick->VAL = 0;

    //Set SysTick priority lowest
    SYSPRI3 = (SYSPRI3 & 0x00FFFFFF) | (0XE0000000);

    //load countdown value and start start systick
    SysTick->LOAD = (quanta*millis_pre);
    SysTick->CTRL = 0x00000007;

    //launch thread scheduler
    //osKernelScheduler();
}
