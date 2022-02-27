/*
 * Copyright (c) 2022, smartmx <smartmx@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-02-27     smartmx      the first version
 *
 */
 
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "mfbd.h"

#define BTN_CHECK_PERIOD  10 //10ms

void bsp_btn_value_report(mfbd_btn_code_t btn_value);

unsigned char bsp_btn_check(mfbd_btn_index_t btn_index);


/* tbtn test */
/* MFBD_TBTN_DEFINE(NAME, BTN_INDEX, FILTER_TIME, BTN_CODEDOWN, BTN_CODEUP, NEXT) */
MFBD_TBTN_DEFINE(test_tbtn, 1, 3, 0x1201, 0x1200, NULL);

const mfbd_group_t test_tbtn_group = {
   bsp_btn_check,
   bsp_btn_value_report,
   &test_tbtn,
};

/* nbtn test */
/*MFBD_NBTN_DEFINE(NAME, BTN_INDEX, FILTER_TIME, REPEAT_TIME, LONG_TIME, BTN_CODEDOWN, BTN_CODEUP, BTN_CODELONG, NEXT)*/
MFBD_NBTN_DEFINE(test_nbtn1, 3, 3, 0, 150, 0x1401, 0x1400, 0x1402, NULL);

MFBD_NBTN_DEFINE(test_nbtn, 2, 3, 30, 150, 0x1301, 0x1300, 0, &test_nbtn1);

const mfbd_group_t test_nbtn_group = {
    bsp_btn_check,
    bsp_btn_value_report,
    &test_nbtn,
};

/* mbtn test */
/* MFBD_MBTN_DEFINE(NAME, BTN_INDEX, FILTER_TIME, REPEAT_TIME, LONG_TIME, MULTICLICK_TIME, BTN_CODEUP, BTN_CODELONG, NEXT, BTN_CODEDOWN, ...) */
MFBD_MBTN_DEFINE(test_mbtn, 4, 3, 30, 150, 75, 0x1500, 0, NULL, 0x1501, 0x1511, 0x1521, 0x1531);

const mfbd_group_t test_mbtn_group = {
    bsp_btn_check,
    bsp_btn_value_report,
    &test_mbtn,
};

unsigned char bsp_btn_check(mfbd_btn_index_t btn_index)
{
    switch (btn_index)
    {
    case 1:
        if (KEY0 == 0)
        {
            return 1;
        }
        break;
    case 2:
        if (KEY1 == 0)
        {
            return 1;
        }
        break;
    case 3:
        if (KEY2 == 0)
        {
            return 1;
        }
        break;
    case 4:
        if (WK_UP == 1)
        {
            return 1;
        }
        break;
    default:
        break;
    }
    return 0;
}

void bsp_btn_value_report(mfbd_btn_code_t btn_value)
{
  printf("%04x\n",btn_value);
}

int main(void)
{
    HAL_Init();                     //初始化HAL库   
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
    delay_init(180);                //初始化延时函数
    uart_init(115200);              //初始化USART
    LED_Init();                     //初始化LED 
    KEY_Init();                     //初始化按键
	
    while(1)
    {
			  mfbd_tbtn_scan(&test_tbtn_group);
				mfbd_nbtn_scan(&test_nbtn_group);
				mfbd_mbtn_scan(&test_mbtn_group);
        delay_ms(BTN_CHECK_PERIOD);
    }
}

