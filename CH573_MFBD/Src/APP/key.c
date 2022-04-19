/*
 * Copyright (c) 2022, smartmx <smartmx@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-02-22     smartmx      the first version
 * 2022-03-15     smartmx      each mbtn has it's own max multi-click times
 * 2022-04-16     smartmx      drop list definitions, use arraylist, each group has all btn types.
 *
 */

#include "mfbd.h"
#include "CH57x_common.h"
#include "config.h"

#define BTN_CHECK_PERIOD  10 //10ms

unsigned char  bsp_btn_check(mfbd_btn_index_t btn_index);

void bsp_btn_value_report(mfbd_btn_code_t btn_value);

uint16 key_ProcessEvent(uint8 task_id, uint16 events);


#if MFBD_PARAMS_SAME_IN_GROUP

/* tbtn test */
/* MFBD_TBTN_DEFINE(NAME, BTN_INDEX, FILTER_TIME, BTN_DOWN_CODE, BTN_UP_CODE) */
MFBD_TBTN_DEFINE(test_tbtn, GPIO_Pin_12, 0x1201, 0x1200);

/* nbtn test */
/* MFBD_NBTN_DEFINE(NAME, BTN_INDEX, BTN_DOWN_CODE, BTN_UP_CODE, BTN_LONG_CODE) */
MFBD_NBTN_DEFINE(test_nbtn1, GPIO_Pin_14, 0x1401, 0x1400, 0x1402);

MFBD_NBTN_DEFINE(test_nbtn, GPIO_Pin_13, 0x1301, 0x1300, 0x1301);

/* mbtn test */
/* MFBD_MBTN_DEFINE(NAME, BTN_INDEX, MAX_MULTICLICK_STATE, BTN_DOWN_CODE, BTN_UP_CODE, BTN_LONG_CODE, ...) */
MFBD_MBTN_DEFINE(test_mbtn, GPIO_Pin_15, 3, 0x1501, 0x1500, 0, 0x1511, 0x1521, 0x1531);

#else

/* tbtn test */
/* MFBD_TBTN_DEFINE(NAME, BTN_INDEX, FILTER_TIME, BTN_DOWN_CODE, BTN_UP_CODE) */
MFBD_TBTN_DEFINE(test_tbtn, GPIO_Pin_12, 3, 0x1201, 0x1200);

/* nbtn test */
/* MFBD_NBTN_DEFINE(NAME, BTN_INDEX, FILTER_TIME, REPEAT_TIME, LONG_TIME, BTN_DOWN_CODE, BTN_UP_CODE, BTN_LONG_CODE) */
MFBD_NBTN_DEFINE(test_nbtn1, GPIO_Pin_14, 3, 0, 150, 0x1401, 0x1400, 0x1402);

MFBD_NBTN_DEFINE(test_nbtn, GPIO_Pin_13, 3, 30, 150, 0x1301, 0x1300, 0x1301);

/* mbtn test */
/* MFBD_MBTN_DEFINE(NAME, BTN_INDEX, FILTER_TIME, REPEAT_TIME, LONG_TIME, MULTICLICK_TIME, MAX_MULTICLICK_STATE, BTN_DOWN_CODE, BTN_UP_CODE, BTN_LONG_CODE, ...) */
MFBD_MBTN_DEFINE(test_mbtn, GPIO_Pin_15, 3, 30, 150, 75, 3, 0x1501, 0x1500, 0x1501, 0x1511, 0x1521, 0x1531);

#endif /*MFBD_PARAMS_SAME_IN_GROUP*/

MFBD_TBTN_ARRAYLIST(test_tbtn_list, &test_tbtn);

MFBD_NBTN_ARRAYLIST(test_nbtn_list, &test_nbtn1, &test_nbtn);

MFBD_MBTN_ARRAYLIST(test_mbtn_list, &test_mbtn);

const mfbd_group_t test_btn_group =
{
    bsp_btn_check,
    bsp_btn_value_report,
    test_tbtn_list,
    test_nbtn_list,
    test_mbtn_list,
};

unsigned char bsp_btn_check(mfbd_btn_index_t btn_index)
{
    if (GPIOA_ReadPortPin(btn_index))
    {
        return 0;
    }
    return 1;
}

void bsp_btn_value_report(mfbd_btn_code_t btn_value)
{
    PRINT("%04x\n", btn_value);
}

static uint8_t key_task_id;
#define TMOS_KEY_CHECK_EVT 1

void key_init()
{
    GPIOA_SetBits(GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
    GPIOA_ModeCfg(GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15, GPIO_ModeIN_PU);
    key_task_id = TMOS_ProcessEventRegister(key_ProcessEvent);
    tmos_start_task(key_task_id, TMOS_KEY_CHECK_EVT, MS1_TO_SYSTEM_TIME(BTN_CHECK_PERIOD));
    PRINT("key init ok %d\n", key_task_id);
}

uint16 key_ProcessEvent(uint8 task_id, uint16 events)
{
    if (events & TMOS_KEY_CHECK_EVT)
    {
        mfbd_group_scan(&test_btn_group);
        tmos_start_task(key_task_id, TMOS_KEY_CHECK_EVT, MS1_TO_SYSTEM_TIME(BTN_CHECK_PERIOD));
        return (events ^ TMOS_KEY_CHECK_EVT);
    }
    if (events & SYS_EVENT_MSG)
    {
        uint8 *pMsg;

        if ((pMsg = tmos_msg_receive(key_task_id)) != NULL)
        {
            // Release the TMOS message
            tmos_msg_deallocate(pMsg);
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }

    // Discard unknown events
    return 0;
}
