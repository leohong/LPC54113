#ifndef TASKCONFIG_H
#define TASKCONFIG_H
// ===============================================================================
// FILE NAME: TaskConfig.h
// DESCRIPTION:
//
//
// modification history
// --------------------
// 2019/10/29, Leo Create
// --------------------
// ===============================================================================

//This is the number of words the stack will hold, not the number of bytes.
#define APP_MAIN_TASK_STACK_SZ      (256)
#define EVENT_TASK_STACK_SZ         (512)
#define CNT_EVENT_STACK_SZ          (256)
#define ITE_TASK_STACK_SZ           (512)
#define HOST_TASK_STACK_SZ          (512)
#define USB_CDC_TASK_STACK_SZ       (128)

// Priority list
#define APP_MAIN_TASK_PRIORITY      (tskIDLE_PRIORITY)
#define EVENT_TASK_PRIORITY         (tskIDLE_PRIORITY + 1UL)
#define CNT_EVENT_TASK_PRIORITY     (tskIDLE_PRIORITY)
#define ITE_TASK_PRIORITY           (tskIDLE_PRIORITY)
#define HOST_TASK_PRIORITY          (tskIDLE_PRIORITY + 2UL)
#define SB_CDC_TASK_PRIORITY        (tskIDLE_PRIORITY)
#endif // TASKCONFIG_H
