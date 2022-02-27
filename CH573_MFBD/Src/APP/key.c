/*
 * Copyright (c) 2022, smartmx <smartmx@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-02-22     smartmx      the first version
 *
 */

#include "mfbd.h"
#include "CH57x_common.h"
#include "config.h"

#define BTN_CHECK_PERIOD  10 //10ms

void bsp_btn_value_report(mfbd_btn_code_t btn_value);

unsigned char bsp_btn_check(mfbd_btn_index_t btn_index);

uint16 key_ProcessEvent( uint8 task_id, uint16 events );

/* tbtn test */
/* MFBD_TBTN_DEFINE(NAME, BTN_INDEX, FILTER_TIME, BTN_CODEDOWN, BTN_CODEUP, NEXT) */
MFBD_TBTN_DEFINE(test_tbtn, GPIO_Pin_12, 3, 0x1201, 0x1200, NULL);

const mfbd_group_t test_tbtn_group = {
   bsp_btn_check,
   bsp_btn_value_report,
   &test_tbtn,
};

/* nbtn test */
/*MFBD_NBTN_DEFINE(NAME, BTN_INDEX, FILTER_TIME, REPEAT_TIME, LONG_TIME, BTN_CODEDOWN, BTN_CODEUP, BTN_CODELONG, NEXT)*/
MFBD_NBTN_DEFINE(test_nbtn1, GPIO_Pin_14, 3, 0, 150, 0x1401, 0x1400, 0x1402, NULL);

MFBD_NBTN_DEFINE(test_nbtn, GPIO_Pin_13, 3, 30, 150, 0x1301, 0x1300, 0, &test_nbtn1);

const mfbd_group_t test_nbtn_group = {
    bsp_btn_check,
    bsp_btn_value_report,
    &test_nbtn,
};

/* mbtn test */
/* MFBD_MBTN_DEFINE(NAME, BTN_INDEX, FILTER_TIME, REPEAT_TIME, LONG_TIME, MULTICLICK_TIME, BTN_CODEUP, BTN_CODELONG, NEXT, BTN_CODEDOWN, ...) */
MFBD_MBTN_DEFINE(test_mbtn, GPIO_Pin_15, 3, 30, 150, 75, 0x1500, 0, NULL, 0x1501, 0x1511, 0x1521, 0x1531);

const mfbd_group_t test_mbtn_group = {
    bsp_btn_check,
    bsp_btn_value_report,
    &test_mbtn,
};

unsigned char bsp_btn_check(mfbd_btn_index_t btn_index)
{
  if(GPIOA_ReadPortPin(btn_index))
  {
    return 0;
  }
  return 1;
}

void bsp_btn_value_report(mfbd_btn_code_t btn_value)
{
  PRINT("%04x\n",btn_value);
}

static uint8_t key_task_id;
#define TMOS_KEY_CHECK_EVT 1

void key_init()
{
  GPIOA_SetBits(GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
  GPIOA_ModeCfg( GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15, GPIO_ModeIN_PU);
  key_task_id = TMOS_ProcessEventRegister(key_ProcessEvent);
  tmos_start_task(key_task_id, TMOS_KEY_CHECK_EVT, MS1_TO_SYSTEM_TIME(BTN_CHECK_PERIOD));
  PRINT("key init ok %d\n", key_task_id);
}

uint16 key_ProcessEvent( uint8 task_id, uint16 events )
{
  if( events & TMOS_KEY_CHECK_EVT ){
    mfbd_tbtn_scan(&test_tbtn_group);
    mfbd_nbtn_scan(&test_nbtn_group);
    mfbd_mbtn_scan(&test_mbtn_group);
    tmos_start_task(key_task_id, TMOS_KEY_CHECK_EVT, MS1_TO_SYSTEM_TIME(BTN_CHECK_PERIOD));
    return (events ^ TMOS_KEY_CHECK_EVT);
  }
  if ( events & SYS_EVENT_MSG ){
    uint8 *pMsg;

    if ( (pMsg = tmos_msg_receive( key_task_id )) != NULL ){
      // Release the TMOS message
      tmos_msg_deallocate( pMsg );
    }
    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // Discard unknown events
  return 0;
}
