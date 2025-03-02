#include <stm32f4xx_conf.h>
#include "includes.h"
#include "leds.h"
#include "task1.h"

OS_STK   task1_stack[TASK1_STACK_SIZE]; 
OS_EVENT * task1_mbox;

void Task1_Task(void * ptr_args)
{
    INT8U err;
    TASK1_MBOX_TypeDef * mbox;
    
    //
    // The first task to run must call this function
    //
    OS_CPU_SysTickInit(1000);
	
    LEDS_Off(BLUE);
    LEDS_Off(ORANGE);
    
    for (;;){
	mbox = (TASK1_MBOX_TypeDef *) OSMboxPend(task1_mbox, OS_MBOX_TIMEOUT_MAX, &err);
	if (err == OS_ERR_NONE){
	    if (mbox->action == 0x01){
		LEDS_Toggle(ORANGE);		
	    }else{
		LEDS_Toggle(GREEN);		
	    }
	    
	}	
    }    
}
