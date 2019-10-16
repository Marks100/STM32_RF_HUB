#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x.h"
#include "misc.h"
#include "assert.h"
#include "core_cm3.h"

#include "HAL_config.h"
#include "HAL_BRD.h"
#include "STDC.h"
#include "HAL_TIM.h"
#include "MAIN.h"
#include "NEOPIXEL.h"
#include "ROTARY.h"

EXTI_InitTypeDef EXTI_InitStruct;



/*!
****************************************************************************************************
*
*   \brief         Initialise the Pins,
*   			   lets just do gpio pins here and let other modules handle themselves
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_init( void )
{
	/* Disable the JTAG as this saves us some pins :) */
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* Configure the GPIOs */
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure the GPIO_LED pin */
    GPIO_InitStructure.GPIO_Pin = ONBOARD_LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(ONBOARD_LED_PORT, &GPIO_InitStructure);

	/* Configure the NRF24 NCS pin */
	GPIO_InitStructure.GPIO_Pin = NRF24_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(NRF24_CS_PORT, &GPIO_InitStructure);

	/* Configure the NRF24 CE pin */
	GPIO_InitStructure.GPIO_Pin = NRF24_CE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(NRF24_CE_PORT, &GPIO_InitStructure);

	/* Configure the onboard switch */
	GPIO_InitStructure.GPIO_Pin = ONBOARD_BTN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ONBOARD_BTN_PORT, &GPIO_InitStructure);

	/* Configure the NEO PIXEL pin */
	// GPIO_InitStructure.GPIO_Pin = ( GPIO_Pin_12 );
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	// GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure the rotary clock and data pins */
	GPIO_InitStructure.GPIO_Pin = ( ROTARY_CLK_PIN | ROTARY_DATA_PIN );
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ROTARY_PORT, &GPIO_InitStructure);

	/* Configure the 74HC164 clk pin */
	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	// GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure the 74HC164 data pin */
	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	// GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure the LCD Enable pin */
	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	// GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure the LCD RS pin */
	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	// GPIO_Init(GPIOB, &GPIO_InitStructure);


	NVIC_InitTypeDef NVIC_InitStruct;

	GPIO_EXTILineConfig(ROTARY_PORT_SOURCE, ROTARY_PIN_SOURCE);

	EXTI_InitStruct.EXTI_Line = ROTARY_EXT_LINE ;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt ;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);

	/* Add IRQ vector to NVIC */
	NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
	/* Set priority */
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	/* Set sub priority */
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	/* Enable interrupt */
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	/* Add to NVIC */
	NVIC_Init(&NVIC_InitStruct);

    /* Turn the led off straight away to save power */
    HAL_BRD_set_onboard_LED( OFF );
}



/*!
****************************************************************************************************
*
*   \brief         Resets the board
*
*   \author        MS
*
*   \return        low_high_et
*
***************************************************************************************************/
void HAL_BRD_reset( void )
{
	NVIC_SystemReset();
}
	    




/*!
****************************************************************************************************
*
*   \brief         Reads Any PORT and any PIN
*
*   \author        MS
*
*   \return        low_high_et
*
***************************************************************************************************/
low_high_et HAL_BRD_read_pin_state( GPIO_TypeDef * port, u16_t pin )
{
	low_high_et returnType;

	if( ( port->IDR & pin ) == pin )
	{
		returnType = HIGH;
	}
	else
	{
		returnType = LOW;
	}

	return ( returnType );
}






/*!
****************************************************************************************************
*
*   \brief         SETS Any PIN on any PORT
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_set_pin_state(  GPIO_TypeDef * port, u16_t pin, low_high_et state )
{
	if( state == HIGH)
	{
		port->ODR |= pin;
	}
	else
	{
		port->ODR &= ~pin;
	}
}



/*!
****************************************************************************************************
*
*   \brief         Toggles Any PIN on any PORT
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_toggle_pin_state(  GPIO_TypeDef * port, u16_t pin )
{
    /* Firstly read the PIN state */
    if( ( port->ODR & pin ) == pin )
    {
        HAL_BRD_set_pin_state( port, pin, LOW );
    }
    else
    {
        HAL_BRD_set_pin_state( port, pin, HIGH );
    }
}



/*!
****************************************************************************************************
*
*   \brief         Reads the state of the onboard Button
*
*   \author        MS
*
*   \return        low_high_et
*
***************************************************************************************************/
low_high_et HAL_BRD_read_Onboard_btn_state( void )
{
	low_high_et returnType;

	returnType = HAL_BRD_read_pin_state( ONBOARD_BTN_PORT, ONBOARD_BTN_PIN );

	return ( returnType );
}





/*!
****************************************************************************************************
*
*   \brief         SETS the battery voltage enable pin
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_set_batt_monitor_state( disable_enable_et state )
{
	if( state == ENABLE_ )
	{
		//HAL_BRD_Set_Pin_state();
	}
	else
	{
		//HAL_BRD_Set_Pin_state();
	}
}



/*!
****************************************************************************************************
*
*   \brief         SETS the 74HC164 clk pin
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
#pragma GCC push_options
#pragma GCC optimize ("O1")

void HAL_BRD_74HC164_set_clk_pin_state( low_high_et state )
{
	if( state == HIGH )
	{
		HAL_BRD_set_pin_state( SHIFT_REG_PORT, SHIFT_REG_CLK_PIN, HIGH );
	}
	else
	{
		HAL_BRD_set_pin_state( SHIFT_REG_PORT, SHIFT_REG_CLK_PIN, LOW );
	}
}


void HAL_BRD_74HC164_pulse_clk_pin_state( void )
{

	/* This is soo time critical that instead of abstracting with function calls ill go direct to the hardware */
	SHIFT_REG_PORT->ODR |= SHIFT_REG_CLK_PIN;
	asm("nop");asm("nop");asm("nop");asm("nop");
	SHIFT_REG_PORT->ODR &= ~SHIFT_REG_CLK_PIN;
}


void HAL_BRD_74HC164_set_data_pin_high( void )
{
	SHIFT_REG_PORT->ODR |= SHIFT_REG_DATA_PIN;
}

void HAL_BRD_74HC164_set_data_pin_low( void )
{
	SHIFT_REG_PORT->ODR &= ~SHIFT_REG_DATA_PIN;
}

void HAL_BRD_LCD_set_enable_pin_high( void )
{
	LCD_PORT->ODR |= LCD_EN_PIN;
}

void HAL_BRD_LCD_set_enable_pin_low( void )
{
	LCD_PORT->ODR &= ~LCD_EN_PIN;
}


void HAL_BRD_LCD_pulse_enable_pin_state( void )
{
	LCD_PORT->ODR |= LCD_EN_PIN;

	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");

	LCD_PORT->ODR &= ~LCD_EN_PIN;
}

void HAL_BRD_LCD_set_RS_pin_high( void )
{
	LCD_PORT->ODR |= LCD_RS_PIN;
}

void HAL_BRD_LCD_set_RS_pin_low( void )
{
	LCD_PORT->ODR &= ~LCD_RS_PIN;
}
#pragma GCC pop_options



/*!
****************************************************************************************************
*
*   \brief         SETS the rf enable pin
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_NRF24_set_ce_pin_state( low_high_et state )
{
	if( state == HIGH )
	{
		HAL_BRD_set_pin_state( NRF24_CE_PORT, NRF24_CE_PIN, HIGH );
	}
	else
	{
		HAL_BRD_set_pin_state( NRF24_CE_PORT, NRF24_CE_PIN, LOW );
	}
}







/*!
****************************************************************************************************
*
*   \brief         SETS the SPI chip select pin for the RF module
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_NRF24_spi_slave_select( low_high_et state )
{
	if( state == HIGH )
	{
		HAL_BRD_set_pin_state( NRF24_CS_PORT, NRF24_CS_PIN, HIGH );
	}
	else
	{
		HAL_BRD_set_pin_state( NRF24_CS_PORT, NRF24_CS_PIN, LOW );
	}
}




/*!
****************************************************************************************************
*
*   \brief         Toggles the led
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_toggle_onboard_led( void )
{
    HAL_BRD_toggle_pin_state( ONBOARD_LED_PORT, ONBOARD_LED_PIN );
}


void HAL_BRD_set_onboard_LED( off_on_et state )
{
	low_high_et val;

	/* LED is inverse logic so flip the states */
	if( state == OFF )
	{
		val = HIGH;
	}
	else
	{
		val = LOW;
	}
	HAL_BRD_set_pin_state( ONBOARD_LED_PORT, ONBOARD_LED_PIN, val);
}



/*!
****************************************************************************************************
*
*   \brief         Reads the state of the rotary clock pin
*
*   \author        MS
*
*   \return        low_high_et state of the pin
*
***************************************************************************************************/
low_high_et HAL_BRD_read_rotary_clock_pin( void )
{
    low_high_et state = LOW;

    state = HAL_BRD_read_pin_state( ROTARY_PORT, ROTARY_CLK_PIN );

    return( state );
}

/*!
****************************************************************************************************
*
*   \brief         Reads the state of the rotary data pin
*
*   \author        MS
*
*   \return        low_high_et state of the pin
*
***************************************************************************************************/
low_high_et HAL_BRD_read_rotary_data_pin( void )
{
    low_high_et state = LOW;

    state = HAL_BRD_read_pin_state( ROTARY_PORT, ROTARY_DATA_PIN );

    return( state );
}


/****************************************************************************************************
*
*   \brief         Reads the state of the rotary SW pin
*
*   \author        MS
*
*   \return        low_high_et state of the pin
*
***************************************************************************************************/
low_high_et HAL_BRD_read_rotary_SW_pin( void )
{
    low_high_et state = LOW;

    state = HAL_BRD_read_pin_state( ROTARY_PORT, ROTARY_SW_PIN );

    return( state );
}


void HAL_BRD_set_heater_state( off_on_et state )
{
	HAL_BRD_set_LED_state( LED_3, state );
}


void HAL_BRD_set_cooler_state( off_on_et state )
{
	HAL_BRD_set_LED_state( LED_3, state );
}



void HAL_BRD_set_ROTARY_interrupt_state( disable_enable_et state )
{
	EXTI_InitStruct.EXTI_Line = ROTARY_EXT_LINE ;
	EXTI_InitStruct.EXTI_LineCmd = state;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt ;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);
}





void EXTI0_IRQHandler(void)
{
	/* Make sure that interrupt flag is set */
	if ( EXTI_GetFlagStatus(EXTI_Line0) != RESET )
	{
		/* Now we keep track of the interrupt edge */
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}


void EXTI1_IRQHandler(void)
{
	/* Make sure that interrupt flag is set */
	if ( EXTI_GetFlagStatus(EXTI_Line1) != RESET )
	{
		/* Now we keep track of the interrupt edge */
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}


void EXTI9_5_IRQHandler(void)
{
	/* Make sure that interrupt flag is set */
	if ( EXTI_GetFlagStatus(EXTI_Line5) != RESET )
	{
		/* Now we keep track of the interrupt edge */
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit(EXTI_Line5);
	}
}



/*!
****************************************************************************************************
*
*   \brief         Interrupt Handler
*
*   \author        MS
*
*
***************************************************************************************************/
/* Handle PB12 interrupt */
void EXTI15_10_IRQHandler(void)
{
	/* Make sure that interrupt flag is set */
	if ( EXTI_GetFlagStatus(EXTI_Line15) != RESET )
	{
	    /* Now we keep track of the interrupt edge */
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit(EXTI_Line15);
	}

	if ( EXTI_GetFlagStatus(ROTARY_EXT_LINE) != RESET )
	{
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit(ROTARY_EXT_LINE);

		/* I wont abstract this as it needs to be done ASAP */
		HAL_BRD_set_ROTARY_interrupt_state( DISABLE );
		//ROTARY_set_ROTARY_interrupt_state( DISABLE );

		ROTARY_set_prev_clk_pin_state( ROTARY_read_rotary_clock_pin() );

		/* Start a timer to generate a callback in xms to debounce the LOGIC */
		HAL_TIM2_start();
	}
}
