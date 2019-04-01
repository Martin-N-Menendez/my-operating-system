/**
 * @file    main.c
 * @author  Ericson Joseph
 * @date    March 2019
 *
 * @brief   Main program body
 */

/* Include ----------------------------------------- */

#include <stdint.h>
#include <strings.h>

/* Private Include --------------------------------- */

#include "main.h"
#include "board.h"
#include "myOS.h"
#include "sapi.h"

/* Private typedef --------------------------------- */

typedef enum { UP, DOWN } ButtonState;

/* Private define ---------------------------------- */

#define EJ0
#define EJ1
#define STACK_SIZE_B 512

/* Private variables ------------------------------- */

uint32_t ledOnTimeTick;
uint32_t launchLed = 0;

/* Private function prototypes --------------------- */

void *task1(void *args) {
    while (1) {
        gpioToggle(LED1);
        taskDelay(500);
    }
    return NULL;
}

void *task2(void *args) {
    while (1) {
        gpioToggle(LED2);
        taskDelay(500);
    }
    return NULL;
}

void *task3(void *args) {
    while (1) {
        gpioToggle(LED3);
        taskDelay(100);
    }
    return NULL;
}

void *buttonTask(void *taskParm) {

    ButtonState state = UP;
    uint32_t onTime;

    while (TRUE) {

        taskDelay(10);

        if (!gpioRead(TEC1) && state == UP) {
            taskDelay(20);
            if (!gpioRead(TEC1)) {
                state  = DOWN;
                onTime = getTickCount();
            }
        }

        if (gpioRead(TEC1) && state == DOWN) {
            taskDelay(20);
            if (gpioRead(TEC1)) {
                state         = UP;
                onTime        = getTickCount() - onTime;
                ledOnTimeTick = onTime;
                launchLed     = 1; // give
            }
        }
    }
}

void *ledTask2(void *taskParmPtr) {
    while (TRUE) {
        while (launchLed == 0) { // take
            taskDelay(100);
        }
        launchLed = 0;
        gpioWrite(LEDB, ON);
        taskDelay(ledOnTimeTick);
        gpioWrite(LEDB, OFF);
        //        xSemaphoreGive(controlLed);
    }
}

/* Code -------------------------------------------- */

int main() {

    MyOSInit();

#ifdef EJ0
    taskCreate(STACK_SIZE_B, task1, 1, (void *)0x11223344);
    taskCreate(STACK_SIZE_B, task2, 1, (void *)0x55667788);
    taskCreate(STACK_SIZE_B, task3, 1, (void *)0x55667788);
#endif

#ifdef EJ1
    taskCreate(STACK_SIZE_B, buttonTask, 2, (void *)0x11223344);
    taskCreate(STACK_SIZE_B, ledTask2, 2, (void *)0x55667788);
#endif

    Board_Init();
    SystemCoreClockUpdate();
    NVIC_SetPriority(PendSV_IRQn, PRIORITY_PENDSV);
    SysTick_Config(SystemCoreClock / 1000);

    while (1) {
    }

    return 0;
}
