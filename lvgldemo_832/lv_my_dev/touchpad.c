/**
 * @file indev.c
 * 
 */

#if !defined(_MSC_VER)
/*********************
 *      INCLUDES
 *********************/
#include "touch.h"
#include "lcd.h"
#include "usart.h"
#include "touchpad.h"
#include "lvgl/lv_hal/lv_hal.h"

/*********************
 *      DEFINES
 *********************/
#define TOUCH_AD_TOP		160  	//按下触摸屏的顶部，写下 Y 轴模拟输入值。
#define TOUCH_AD_BOTTOM		3990 	//按下触摸屏的底部，写下 Y 轴模拟输入值。
#define TOUCH_AD_LEFT 		160		//按下触摸屏的左侧，写下 X 轴模拟输入值。
#define TOUCH_AD_RIGHT		3990	//按下触摸屏的右侧，写下 X 轴模拟输入值。
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool touchpad_read(lv_indev_data_t *data);

/**********************
 *  STATIC VARIABLES
 **********************/
//static TS_StateTypeDef  TS_State;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize your input devices here
 */
void touchpad_init(void)
{

  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.read = touchpad_read;
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  lv_indev_drv_register(&indev_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Read an input device
 * @param indev_id id of the input device to read
 * @param x put the x coordinate here
 * @param y put the y coordinate here
 * @return true: the device is pressed, false: released
 */
static bool touchpad_read(lv_indev_data_t *data)
{
	static int16_t last_x = 0;
	static int16_t last_y = 0;
	if(tp_dev.scan(0))//有按键按下时
	{
		if((lcddev.id == 0X5510) || (lcddev.id == 0X1963)) //电容屏的触摸值获取(使用NT5510和SSD1963芯片的4.3寸和7寸屏都是电容屏)
		{
			
			data->point.x=tp_dev.x[0];
			data->point.y = tp_dev.y[0];
			data->state = LV_INDEV_STATE_PR;
			last_x = data->point.x;
			last_y = data->point.y;

	//		return xvalue;
		}else				//电阻屏
		{
			data->point.x = tp_dev.x[0];
			data->point.y = tp_dev.y[0];
			data->state = LV_INDEV_STATE_PR;
			last_x = data->point.x;
			last_y = data->point.y;
		}
	}
	else{
		data->point.x = last_x;
		data->point.y = last_y;
		data->state = LV_INDEV_STATE_REL;
	}

	return false;
}
#else
/*********************
*      INCLUDES
*********************/
#include "touchpad.h"
#include "lvgl/src/hal/lv_hal.h"
#include "lvgl/src/hal/lv_hal_indev.h"

#include "../sdl_api.h"

/*********************
*      DEFINES
*********************/
#define TOUCH_AD_TOP		160  	//按下触摸屏的顶部，写下 Y 轴模拟输入值。
#define TOUCH_AD_BOTTOM		3990 	//按下触摸屏的底部，写下 Y 轴模拟输入值。
#define TOUCH_AD_LEFT 		160		//按下触摸屏的左侧，写下 X 轴模拟输入值。
#define TOUCH_AD_RIGHT		3990	//按下触摸屏的右侧，写下 X 轴模拟输入值。
/**********************
*      TYPEDEFS
**********************/

/**********************
*  STATIC PROTOTYPES
**********************/
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t *data);

/**********************
*  STATIC VARIABLES
**********************/
//static TS_StateTypeDef  TS_State;

/**********************
*      MACROS
**********************/

/**********************
*   GLOBAL FUNCTIONS
**********************/

/**
* Initialize your input devices here
*/
void touchpad_init(void)
{
	static lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);
	indev_drv.read_cb = touchpad_read;
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	lv_indev_drv_register(&indev_drv);
}

/**********************
*   STATIC FUNCTIONS
**********************/

/**
* Read an input device
* @param indev_id id of the input device to read
* @param x put the x coordinate here
* @param y put the y coordinate here
* @return true: the device is pressed, false: released
*/
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t *data)
{
	static int16_t last_x = 0;
	static int16_t last_y = 0;
	int status = MouseGetMouseStatus();
	int x = (status >> 12) & 0xfff;
	int y = (status >> 0) & 0xfff;
	int pen = (status >> 24) & 0xff;
	if (status != 0 && (pen == PEN_DOWN || pen == PEN_MOVE))
	{
		data->point.x = x;
		data->point.y = y;
		data->state = LV_INDEV_STATE_PRESSED;
		last_x = data->point.x;
		last_y = data->point.y;
	}
	else{
		data->point.x = last_x;
		data->point.y = last_y;
		data->state = LV_INDEV_STATE_RELEASED;
	}
}
#endif