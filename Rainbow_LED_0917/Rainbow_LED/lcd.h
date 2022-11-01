/*
 * lcd.h
 *
 * Created: 2016-09-17 오전 8:26:25
 *  Author: Administrator
 */ 


#ifndef __LCD_H__
#define __LCD_H__

#include <avr/io.h>
#include <util/delay.h>

#define LCD_PORT_CONTROL	PORTG
#define LCD_DDR_CONTROL		DDRG			//LDC CD 컨트롤

#define LCD_PORT_DATA		PORTC
#define LCD_DDR_DATA		DDRC

void write_Command ( unsigned char command );
void write_Data ( unsigned char data );
void print_String (unsigned char *string );
void print_String_a ( unsigned char *tring );
void LCD_Init (void);

#endif	/* __LCD_H__ */
