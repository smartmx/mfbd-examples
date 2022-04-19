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
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "sdram.h"
#include "usmart.h"
#include "stmflash.h"
#include "mfbd.h"

unsigned char bsp_btn_check(mfbd_btn_index_t btn_index)
{
	switch(btn_index)
	{
		case 0:
			if(KEY0 == 0){
				return 1;
			}
			break;
		case 1:
			if(KEY1 == 0){
				return 1;
			}
			break;
		case 2:
			if(KEY2 == 0){
				return 1;
			}
			break;
		case 3:
			if(WK_UP == 1){
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

#if MFBD_PARAMS_SAME_IN_GROUP

/* tbtn test */
/* MFBD_TBTN_DEFINE(NAME, BTN_INDEX, FILTER_TIME, BTN_DOWN_CODE, BTN_UP_CODE) */
MFBD_TBTN_DEFINE(test_tbtn, 0, 0x1201, 0x1200);

/* nbtn test */
/* MFBD_NBTN_DEFINE(NAME, BTN_INDEX, BTN_DOWN_CODE, BTN_UP_CODE, BTN_LONG_CODE) */
MFBD_NBTN_DEFINE(test_nbtn1, 2, 0x1401, 0x1400, 0x1402);

MFBD_NBTN_DEFINE(test_nbtn, 1, 0x1301, 0x1300, 0x1301);

/* mbtn test */
/* MFBD_MBTN_DEFINE(NAME, BTN_INDEX, MAX_MULTICLICK_STATE, BTN_DOWN_CODE, BTN_UP_CODE, BTN_LONG_CODE, ...) */
MFBD_MBTN_DEFINE(test_mbtn, 3, 3, 0x1501, 0x1500, 0, 0x1511, 0x1521, 0x1531);

#else

/* tbtn test */
/* MFBD_TBTN_DEFINE(NAME, BTN_INDEX, FILTER_TIME, BTN_DOWN_CODE, BTN_UP_CODE) */
MFBD_TBTN_DEFINE(test_tbtn, 0, 3, 0x1201, 0x1200);

/* nbtn test */
/* MFBD_NBTN_DEFINE(NAME, BTN_INDEX, FILTER_TIME, REPEAT_TIME, LONG_TIME, BTN_DOWN_CODE, BTN_UP_CODE, BTN_LONG_CODE) */
MFBD_NBTN_DEFINE(test_nbtn1, 2, 3, 0, 150, 0x1401, 0x1400, 0x1402);

MFBD_NBTN_DEFINE(test_nbtn, 1, 3, 30, 150, 0x1301, 0x1300, 0x1301);

/* mbtn test */
/* MFBD_MBTN_DEFINE(NAME, BTN_INDEX, FILTER_TIME, REPEAT_TIME, LONG_TIME, MULTICLICK_TIME, MAX_MULTICLICK_STATE, BTN_DOWN_CODE, BTN_UP_CODE, BTN_LONG_CODE, ...) */
MFBD_MBTN_DEFINE(test_mbtn, 3, 3, 30, 150, 75, 3, 0x1501, 0x1500, 0x1501, 0x1511, 0x1521, 0x1531);

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

int main(void)
{
	u16 i=0;
	HAL_Init();                     //初始化HAL库   
	Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
	delay_init(180);                //初始化延时函数
	uart_init(115200);              //初始化USART
	usmart_dev.init(90); 		    //初始化USMART	
	LED_Init();                     //初始化LED 
	KEY_Init();                     //初始化按键
	SDRAM_Init();                   //初始化SDRAM
	LCD_Init();                     //初始化LCD
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"MFBD TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2022/3/15");	 		
	while(1)
	{
		mfbd_group_scan(&test_btn_group);
		i++;
		delay_ms(10);  
		if(i==20)
		{
			LED0=!LED0;//提示系统正在运行	
			i=0;
		}		   
	}    
}
