/* STM32 specific includes */
#include "C_defs.h"
#include "COMPILER_defs.h"
#include "STDC.h"
#include "NVM.h"
#include "HAL_BRD.h"
#include "MAIN.h"
#include "NEOPIXEL.h"




#pragma GCC push_options
#pragma GCC optimize ("O0")

STATIC void NEOPIXEL_one_pulse_direct( void )
{
	/* This is soo time critical that instead of abstracting with function calls ill go direct to the hardware */
	GPIOA->ODR |= GPIO_Pin_12;
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");

	GPIOA->ODR &= ~GPIO_Pin_12;
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");
}


STATIC void NEOPIXEL_zero_pulse_direct( void )
{
	/* This is soo time critical that instead of abstracting with function calls ill go direct to the hardware */
	GPIOA->ODR |= GPIO_Pin_12;
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");

	GPIOA->ODR &= ~GPIO_Pin_12;
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");
}

STATIC void NEOPIXEL_drive_colour( u32_t colour )
{
	u8_t bit = 0u;

	for ( bit = 0; bit < NEOPIXEL_CMD_BITS; bit++ )
	{
		if( ( colour & (BV(NEOPIXEL_CMD_BITS-1)>>bit) ) == (BV(NEOPIXEL_CMD_BITS-1)>>bit) )
		{
			NEOPIXEL_one_pulse_direct();
		}
		else
		{
			NEOPIXEL_zero_pulse_direct();
		}
	}
}


void NEOPIXEL_set_led( u32_t led_pos, u32_t colour )
{
	u8_t led_index = 0u;

	for( led_index = 0; led_index < NEOPIXEL_LED_MAX; led_index++ )
	{
		if( ( led_pos & (BV(NEOPIXEL_LED_MAX - ( led_index + 1) ) ) ) != 0 )
		{
			NEOPIXEL_drive_colour( colour );
		}
		else
		{
			NEOPIXEL_drive_colour( NEOPIXEL_COLOUR_NONE );
		}
	}

	/* Drive one more pulse just to get us back round to the start pos */
	NEOPIXEL_drive_colour( NEOPIXEL_COLOUR_NONE );
}


STATIC void NEOPIXEL_latch( void )
{
	u16_t bit = 0u;

	GPIOA->ODR &= ~GPIO_Pin_12;
	for ( bit = 0; bit < 500; bit++ )
	{
		asm("nop");
	}
}


void NEOPIXEL_clear_all_leds( void )
{
	u8_t led_pos = 0;

	for ( led_pos = 0; led_pos < NEOPIXEL_LED_MAX; led_pos ++ )
	{
		NEOPIXEL_drive_colour( NEOPIXEL_COLOUR_NONE );
	}
	NEOPIXEL_latch();
}


void NEOPIXEL_light_all_leds( u32_t colour )
{
	u8_t led_pos = 0;

	for ( led_pos = 0; led_pos < NEOPIXEL_LED_MAX; led_pos ++ )
	{
		NEOPIXEL_drive_colour( colour );
	}
	NEOPIXEL_latch();
}

#pragma GCC pop_options



