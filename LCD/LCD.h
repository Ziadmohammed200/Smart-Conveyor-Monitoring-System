#ifndef LCD_H
#define LCD_H

#include "Gpio.h"
#define LCD_PINS_NUM 6
#define LCD_RS_PIN GPIO_A,1    //Register select(command/data)
#define LCD_EN_PIN GPIO_A,2     //enable
// #define LCD_D0_PIN GPIO_C,8
// #define LCD_D1_PIN GPIO_C,9
// #define LCD_D2_PIN GPIO_C,10
// #define LCD_D3_PIN GPIO_C,11
#define LCD_D4_PIN GPIO_A,3
#define LCD_D5_PIN GPIO_A,4
#define LCD_D6_PIN GPIO_A,5
#define LCD_D7_PIN GPIO_A,6

// Command macros
#define LCD_CLEAR_DISPLAY     0x01
#define LCD_RETURN_HOME       0x02

// Entry mode set
#define LCD_ENTRY_MODE        0x04
#define LCD_ENTRY_LEFT        0x02
#define LCD_ENTRY_SHIFT_INC   0x01
#define LCD_ENTRY_SHIFT_DEC   0x00

// Display control
#define LCD_DISPLAY_CONTROL   0x08
#define LCD_DISPLAY_ON        0x04
#define LCD_CURSOR_ON         0x02
#define LCD_BLINK_ON          0x01
#define LCD_DISPLAY_OFF       0x00
#define LCD_CURSOR_OFF        0x00
#define LCD_BLINK_OFF         0x00

// Function set
#define LCD_FUNCTION_SET      0x20
#define LCD_8BIT_MODE         0x10
#define LCD_4BIT_MODE         0x00
#define LCD_2LINE             0x08
#define LCD_1LINE             0x00
#define LCD_5x10DOTS          0x04
#define LCD_5x8DOTS           0x00

// DDRAM addressing
#define LCD_SET_DDRAM_ADDR    0x80
#define LCD_LINE1_ADDR        0x00
#define LCD_LINE2_ADDR        0x40



void Lcd_Init(void);
void Lcd_SendCommand(uint8 cmd);
void Lcd_SendData(uint8 data);
void Lcd_SendString(const char *str);
void Lcd_SetCursor(uint8 col, uint8 row);
void Lcd_Clear(void) ;


#endif
