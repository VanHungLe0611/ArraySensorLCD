/*
 * lcd_functions.h
 *
 *  Created on: 17.02.2020
 *      Author: ole
 */

#ifndef LCD_FUNCTIONS_H_
#define LCD_FUNCTIONS_H_

/*****************************  # Includes #   ****************************/
#include <tm4c1294ncpdt.h>
#include <string.h>
#include <Interrupt_handler.h>
#include <math.h>
#include <stdio.h>

/*****************************  # defines #   *****************************/
#define BIG_DOT_DIM ( 15 )
#define NUM_DOTS ( 96 )
#define LCD_DIR  0x32

#define DRAWRANGE_X 250
#define DRAWRANGE_Y 200

#define TBL_OFFSET_X (CENTER_X - DRAWRANGE_X/2)
#define TBL_OFFSET_Y (143 - DRAWRANGE_Y/2)
#define CENTER_X 240
#define CENTER_Y  265

#define CENTERX_TACHONADEL 240
#define CENTERY_TACHONADEL 220


#define FONT_WIDTH_BIG 12
#define FONT_HIGHT_BIG 16
#define NO_ARROW        0
#define WITH_ARROW      1

/**************************  # Prototypes #   ****************************/

// Typedefs:
// Typedefs:
typedef union color
{
    int all;
    struct
    {
        char red;
        char green;
        char blue;
    };
} COLOR;

// enum colors
enum
{
    BLACK = 0x000000,
    RED = 0x0000FF,
    GREEN = 0x00FF00,
    YELLOW = 0x00FFFF,
    BLUE = 0xFF0000,
    WHITE = 0xFFFFFF,
    VIOLET = 0xEE82EE,
    MARINE = 0x7FFFD4
};

// Structs:
struct TABLE
{
    char red;
    char green;
    char blue;
};

void screen_show_nr(short);
void screen_write_nr(short);
void write_char(int, COLOR, COLOR);
void print_string(char *, int, int, COLOR, COLOR);
void write_screen_color(COLOR color);
void write_command(unsigned char data);
void write_cmd_data(unsigned char);
void write_data(COLOR);
void write_position(short, short, short, short);
void write_line(short start_x, short start_y, short stop_x, short stop_y,
                COLOR color, uint16_t arrowOption);
void erase_line(short, short, short, short);
void write_line_normal(short start_x, short start_y, short stop_x, short stop_y,
                COLOR color);

void lcd_write_table(short y, short x);
void TFT_Set_Index_Ptr(unsigned char command);
void TFT_Write_Command_Ptr(unsigned char data);

void darstellung_display(void);
void darstellung_tacho(void);
void load_picture(void);

void draw_display(void);

void write_line_0_degree  (short, short, short, short, COLOR);
void write_line_90_degree (short, short, short, short, COLOR);
void write_line_180_degree(short, short, short, short, COLOR);
void write_line_270_degree(short, short, short, short, COLOR);

void write_line_quadrant_1_I (short, short, short, short, double, COLOR);      //   0° < degree < 90°
void write_line_quadrant_1_II(short, short, short, short, double, COLOR);      //   0° < degree < 90°
void write_line_quadrant_2_I (short, short, short, short, double, COLOR);      //  90° < degree < 180°
void write_line_quadrant_2_II(short, short, short, short, double, COLOR);      //  90° < degree < 180°
void write_line_quadrant_3_I (short, short, short, short, double, COLOR);      // 180° < degree < 270°
void write_line_quadrant_3_II(short, short, short, short, double, COLOR);      // 180° < degree < 270°
void write_line_quadrant_4_I (short, short, short, short, double, COLOR);      // 270° < degree < 360°
void write_line_quadrant_4_II(short, short, short, short, double, COLOR);      // 270° < degree < 360°

#endif /* LCD_FUNCTIONS_H_ */
