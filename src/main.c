/*****************************************************************************\
 *  main.c - firmware for Bathroom Monkey
 *****************************************************************************
 *  Copyright (C) 2010 Shaun Garlick and Jim Garlick
 *  
 *  This file is part of bathroomtimer, firmware for a kid's shower/toothbrush
 *  timer.  For details, see <http://code.google.com/p/bathroomtimer>.
 *  
 *  bathroomtimer is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *  
 *  bathroomtimer is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *  
 *  You should have received a copy of the GNU General Public License along
 *  with bathroomtimer; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
\*****************************************************************************/

#include <htc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* Personalize your Bathroom Monkey
 */
#define TOP_TEETH_MIN	1
#define TOP_TEETH_SEC	0

#define BOT_TEETH_MIN	1
#define BOT_TEETH_SEC	0

#define SHOWER_PREP_MIN	0
#define SHOWER_PREP_SEC	30

#define SHOWER_MIN	10
#define SHOWER_SEC	0

#define GREETING	"Hello Shaun!"
#define BYEBYE		"Goodbye Shaun!"

#if defined(_16F84)
__CONFIG (XT & WDTDIS & PWRTEN & UNPROTECT);
#else
#error Config bits and other stuff may need attention for non-16F84 chip.
#endif

/**
 ** PIC parallel port/SPI definitions
 **/

#define LCD_PWR         RA0
#define LCD_EN          RA1
#define LCD_RW          RA2
#define LCD_RS          RA3
#define BUZZER          RA4	/* active low */
#define PORTA_INPUTS    0b00000000

#define LCD_DATA        PORTB
#define LCD_DATA4       RB0
#define LCD_DATA5       RB1
#define LCD_DATA6       RB2
#define LCD_DATA7       RB3
#define _UNUSED_1       RB4
#define SW_SHOWER       RB5	/* active low */
#define SW_BRUSH        RB6	/* active low */
#define _UNUSED_2       RB7
#define PORTB_INPUTS    0b01100000

#define LCD_STROBE()    ((LCD_EN = 1),(LCD_EN=0))

void
lcd_write(unsigned char c)
{
    __delay_us(40);
    LCD_DATA = (c >> 4) & 0x0f;
    LCD_STROBE();
    LCD_DATA = c & 0x0f;
    LCD_STROBE();
}

void
lcd_clear(void)
{
    LCD_RS = 0;
    lcd_write(0x1);
    __delay_ms(2);
}

void
lcd_puts (unsigned char pos, const char *s)
{
    LCD_RS = 0;
    lcd_write(0x80+pos);
    LCD_RS = 1;
    while(*s)
        lcd_write(*s++);
}

void
lcd_puttime_at(unsigned char pos, unsigned char m, unsigned char s)
{
    LCD_RS = 0;
    lcd_write(0x80+pos);
    LCD_RS = 1;
    lcd_write (m / 10 + '0');
    lcd_write (m % 10 + '0');
    lcd_write (':');
    lcd_write (s / 10 + '0');
    lcd_write (s % 10 + '0');
}

void
lcd_init()
{
    LCD_RS = 0;
    LCD_EN = 0;
    LCD_RW = 0;
    
    __delay_ms(15);
    LCD_DATA = 0x3;
    LCD_STROBE();
    __delay_ms(5);
    LCD_STROBE();
    __delay_us(200);
    LCD_STROBE();
    __delay_us(200);
    LCD_DATA = 2;        /* select 4 bit mode */
    LCD_STROBE();

    lcd_write(0x28);     /* set interface length */
    lcd_write(0xc);      /* display on, cursor off, cursor no blink */
    lcd_clear();         /* clear screen */
    lcd_write(0x6);      /* set entry mode */
}

void
delay_100ms (unsigned char tics)
{
    while (tics-- > 0)
        __delay_ms (100);
}

void
beep_timeleft (unsigned char m)
{
    while (m-- > 0) {
        BUZZER = 0;
        __delay_ms (50);
        BUZZER = 1;
        __delay_ms (100);
    }
}

void
beeps (void)
{
    BUZZER = 0;
    delay_100ms (5);
    BUZZER = 1;
    delay_100ms (2);
    BUZZER = 0;
    delay_100ms (5);
    BUZZER = 1;
    delay_100ms (2);
}

void
countdown_time (unsigned char pos, unsigned char m, unsigned char s,
                unsigned char mwarn)
{ 
    while (m > 0 || s > 0) {
        lcd_puttime_at (pos, m, s);
        delay_100ms (10);
        if (s == 0) {
            if (mwarn)
                beep_timeleft (m);
	    s = 59;
            m--;
        } else
            s--;
        if (!SW_SHOWER || !SW_BRUSH)
            s = 0; 
    }
}

void
countup_time (unsigned char pos)
{
    unsigned char m = 0;
    unsigned char s = 0;

    while (1) {
        if (m < 60) {
            lcd_puttime_at (pos, m, s);
            BUZZER = 0;
            delay_100ms (5);
            BUZZER = 1;
            delay_100ms (5);
            s++;
            if (s == 60) {
                m++;
                s = 0;
            }
        }
        if (!SW_SHOWER || !SW_BRUSH)
            break;
    }
}

void
brush_teeth (void)
{
    lcd_clear ();
    lcd_puts (0, GREETING);
    delay_100ms (20);
    lcd_puts (0x40, "Time to brush teeth!");
    delay_100ms (20);

    lcd_clear ();
    lcd_puts (0, "Top teeth:");
    countdown_time (0x0e, TOP_TEETH_MIN, TOP_TEETH_SEC, 0);
    lcd_puts (0x0e, "good! ");
    beeps ();
    delay_100ms (20);

    lcd_puts (0x40, "Bottom teeth:");
    countdown_time (0x4e, BOT_TEETH_MIN, BOT_TEETH_SEC, 0);
    lcd_puts (0x4e, "great!");
    beeps ();
    delay_100ms (20);

    lcd_clear ();
    lcd_puts (0, "Excellent!");
    delay_100ms (20);
    lcd_puts (0x40, BYEBYE);
    delay_100ms (20);
}

void
shower (void)
{
    lcd_clear ();
    lcd_puts (0, GREETING);
    delay_100ms (20);
    lcd_puts (0x40, "To the shower!");
    countdown_time (0x4f, SHOWER_PREP_MIN, SHOWER_PREP_SEC, 0);

    lcd_puts (0x40, "Showering:    ");
    countdown_time (0x4f, SHOWER_MIN, SHOWER_SEC, 1);

    lcd_puts (0x40, "Overtime:     ");
    countup_time (0x4f);

    lcd_clear ();
    lcd_puts (0, "Excellent!");
    delay_100ms (20);
    lcd_puts (0x40, BYEBYE);
    delay_100ms (20);
}

void interrupt
isr (void)
{
    if (RBIF) {
        (void)PORTB;
        RBIF = 0;
    }
}

void
main(void)
{
    TRISA = PORTA_INPUTS;
    TRISB = PORTB_INPUTS;

    LCD_PWR = 0; /* LCD OFF */
    BUZZER = 1;	 /* buzzer OFF */

    /* Interrupts */
    RBIF = 0;           /* clear PORTB state change interrupt flag */
    RBIE = 1;           /* enable interrupt on PORTB state change */
    GIE = 1;            /* enable global interrupts */

    for (;;) {
        while (SW_BRUSH && SW_SHOWER)
            SLEEP ();
        LCD_PWR = 1;    /* LCD ON */
        lcd_init ();
	if (!SW_BRUSH)
            brush_teeth ();
	else if (!SW_SHOWER)
	    shower ();
        LCD_PWR = 0;	/* LCD OFF */
        delay_100ms (50);
    }
}

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */
 
