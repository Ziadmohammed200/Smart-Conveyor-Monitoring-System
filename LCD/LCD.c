
#include "lcd.h"
#include <stdint.h>
#include "Gpio.h"
#include "Gpio_Private.h"

// LCD pin configuration with explicit port/pin definitions
static const GpioPin lcdPins[LCD_PINS_NUM]={
        {LCD_RS_PIN},
        {LCD_EN_PIN},
        {LCD_D4_PIN},
        {LCD_D5_PIN},
        {LCD_D6_PIN},
        {LCD_D7_PIN}
    };

static void DelayUs(uint32 us) {
    for (volatile uint32 i = 0; i < us * 16; i++);
}


void DelayMs(uint32 ms) {
    while (ms--) {
        DelayUs(1000);
    }
}

static void Lcd_EnablePulse(void) {
    Gpio_WritePin(lcdPins[1].port, lcdPins[1].pinNumber, HIGH);
    DelayUs(1);  // 450ns minimum per HD44780 datasheet
    Gpio_WritePin(lcdPins[1].port, lcdPins[1].pinNumber, LOW);
    DelayUs(100); // Command execution time (37μs minimum)
}

static void Lcd_Send4Bits(uint8_t data) {
    // Clear all data pins first (more efficient single write possible)
    for (uint8_t i = 2; i < LCD_PINS_NUM; i++) {
        Gpio_WritePin(lcdPins[i].port, lcdPins[i].pinNumber, LOW);
    }

    // Set only the bits that are high in the data nibble
    for (uint8_t i = 0; i < 4; i++) {
        if (data & (1 << i)) {
            Gpio_WritePin(lcdPins[i+2].port, lcdPins[i+2].pinNumber, HIGH);
        }
    }

    Lcd_EnablePulse();
}

void Lcd_SendCommand(uint8 cmd) {
    Gpio_WritePin(lcdPins[0].port, lcdPins[0].pinNumber, LOW); // Command mode
    Lcd_Send4Bits(cmd >> 4);    // High nibble first
    Lcd_Send4Bits(cmd & 0x0F);  // Low nibble

    // Special handling for clear and home commands
    if (cmd == LCD_CLEAR_DISPLAY || cmd == LCD_RETURN_HOME)
        DelayMs(2);
    else
        DelayUs(50);
}

void Lcd_SendData(uint8 data) {
    Gpio_WritePin(lcdPins[0].port, lcdPins[0].pinNumber, HIGH); // Data mode
    Lcd_Send4Bits(data >> 4);
    Lcd_Send4Bits(data & 0x0F);
    DelayUs(50); // Data write needs 43μs typically
}

void Lcd_SendString(const char *str) {
    while (*str) {
        // Handle newline characters if needed
        if (*str == '\n') {
            Lcd_SetCursor(0, 1); // Move to second line
        } else {
            Lcd_SendData(*str);
        }
        str++;
    }
}

void Lcd_SetCursor(uint8_t col, uint8_t row) {
    uint8_t row_offsets[] = {LCD_LINE1_ADDR, LCD_LINE2_ADDR};
    if (row > 1) row = 1;
    Lcd_SendCommand(LCD_SET_DDRAM_ADDR | (col + row_offsets[row]));
}

void Lcd_Init(void) {
    // Initialize all LCD pins as outputs with medium speed
    for (uint8_t i = 0; i < LCD_PINS_NUM; i++) {
        Gpio_Init(lcdPins[i].port, lcdPins[i].pinNumber,
                 GPIO_OUTPUT, GPIO_PUSH_PULL);
    }

    DelayMs(20);  // Wait for LCD to power up

    // Initialization sequence
    Lcd_Send4Bits(0x03); DelayMs(5);
    Lcd_Send4Bits(0x03); DelayUs(150);
    Lcd_Send4Bits(0x02); // Switch to 4-bit mode

    // 4-bit, 2 line, 5x8 font
    Lcd_SendCommand(LCD_FUNCTION_SET | LCD_4BIT_MODE | LCD_2LINE | LCD_5x8DOTS);
    // Display ON, cursor OFF, blink OFF
    Lcd_SendCommand(LCD_DISPLAY_CONTROL | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF);
    //  Increment cursor, no shift
    Lcd_SendCommand(LCD_ENTRY_MODE | LCD_ENTRY_LEFT | LCD_ENTRY_SHIFT_DEC);
    // Clear display
    Lcd_SendCommand(LCD_CLEAR_DISPLAY);
    DelayMs(2);
}

// Additional useful functions
void Lcd_Clear(void) {
    Lcd_SendCommand(LCD_CLEAR_DISPLAY);
    DelayMs(10);
}



