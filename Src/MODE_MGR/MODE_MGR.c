/* STM32 specific includes */
#include "C_defs.h"
#include "COMPILER_defs.h"
#include "MODE_MGR.h"
#include "STDC.h"
#include "NVM.h"
#include "HAL_ADC.h"
#include "HAL_BRD.h"
#include "HAL_TIM.h"
#include "MAIN.h"
#include "AIRCON.h"
#include "CLI.h"
#include "RFM69.h"
#include "HAL_UART.h"
#include "NEOPIXEL.h"


STATIC false_true_et 		   MODE_MGR_system_init_s;
STATIC u32_t 			       MODE_MGR_tick_timer_msecs_s;
STATIC MODE_MGR_mode_et        MODE_MGR_mode_s;
STATIC u8_t                    MODE_MGR_debounce_ctr_s;

STATIC MODE_MGR_user_input_st  MODE_MGR_user_input_s[MODE_MGR_NUM_SLIDER_INPUTS];
extern NVM_info_st NVM_info_s;



void MODE_MGR_init( void )
{
	u8_t i = 0u;

	MODE_MGR_tick_timer_msecs_s = MODE_MGR_TICK_RATE_MSECS;
	MODE_MGR_mode_s = MODE_MGR_MODE_NORMAL;
	MODE_MGR_system_init_s = FALSE;
	MODE_MGR_debounce_ctr_s = 0xFF;

	STDC_memset( &MODE_MGR_user_input_s, 0x00, sizeof( MODE_MGR_user_input_s ) );

	for( i = 0u; i < MODE_MGR_NUM_SLIDER_INPUTS; i++ )
	{
		/* Need to set the high level state initial on reset, so just read the current state of the selector switches */
		MODE_MGR_user_input_s[i].slider_state_hl = HAL_BRD_read_selector_switch_pin( (HAL_BRD_switch_slider_et)i );
	}
}



void MODE_MGR_20ms_tick( void )
{
	switch( MODE_MGR_mode_s )
	{
		case MODE_MGR_MODE_NORMAL:
			MODE_MGR_action_schedule_normal();
		break;

		case MODE_MGR_MODE_TEST:
		break;

		case MODE_MGR_SETTINGS_MODE:
			MODE_MGR_action_schedule_normal();
			break;

		default:
			break;
	}
}


/*!
****************************************************************************************************
*
*   \brief          < Brief description of function >
*
*   \author
*
*   \return         none
*
***************************************************************************************************/
void MODE_MGR_action_schedule_normal( void )
{
    NRF24_tick();
    CLI_message_handler();
    MODE_MGR_check_user_input();
    NEOPIXEL_tick();

    switch( MODE_MGR_tick_timer_msecs_s )
    {
        case 20u:
        	ROTARY_tick();
        	break;

        case 40u:
            AIRCON_tick();
        	break;

        case 60u:
        	ROTARY_tick();
        	break;

        case 80u:
        	NVM_tick();
        	break;

        case 100u:
        	MAIN_WATCHDOG_kick();
            RF_MGR_tick();
        	ROTARY_tick();
        	break;

        case 120u:
        	break;

        case 140u:
            AIRCON_tick();
        	ROTARY_tick();
        	break;

        case 160u:
        	break;

        case 180u:
        	NVM_tick();
        	ROTARY_tick();
        	break;

        case 200u:
        	MAIN_WATCHDOG_kick();
            RF_MGR_tick();
        	break;

        case 220u:
        	ROTARY_tick();
        	break;

        case 240u:
            AIRCON_tick();
        	break;

        case 260u:
        	ROTARY_tick();
        	break;

        case 280u:
        	NVM_tick();
        	break;

        case 300u:
        	MAIN_WATCHDOG_kick();
            RF_MGR_tick();
        	ROTARY_tick();
        	break;

        case 320u:
        	break;

        case 340u:
            AIRCON_tick();
        	ROTARY_tick();
        	break;

        case 360u:
        	break;

        case 380u:
        	NVM_tick();
        	ROTARY_tick();
        	break;

        case 400u:
        	MAIN_WATCHDOG_kick();
            RF_MGR_tick();
        	break;

        case 420u:
        	ROTARY_tick();
        	break;

        case 440u:
            AIRCON_tick();
        	break;

        case 460u:
        	ROTARY_tick();
        	break;

        case 480u:
        	NVM_tick();
        	break;

        case 500u:
        	MAIN_WATCHDOG_kick();
            RF_MGR_tick();
        	ROTARY_tick();
        	break;

        case 520u:
        	break;

        case 540u:
            AIRCON_tick();
        	ROTARY_tick();
        	break;

        case 560u:
        	break;

        case 580u:
        	NVM_tick();
        	ROTARY_tick();
        	break;

        case 600u:
        	MAIN_WATCHDOG_kick();
            RF_MGR_tick();
        	break;

        case 620u:
        	ROTARY_tick();
        	break;

        case 640u:
            AIRCON_tick();
        	break;

        case 660u:
        	ROTARY_tick();
        	break;

        case 680u:
        	NVM_tick();
        	break;

        case 700u:
        	MAIN_WATCHDOG_kick();
            RF_MGR_tick();
        	ROTARY_tick();
        	break;

        case 720u:
        	break;

        case 740u:
            AIRCON_tick();
        	ROTARY_tick();
        	break;

        case 760u:
        	break;

        case 780u:
        	NVM_tick();
        	ROTARY_tick();
        	break;

        case 800u:
        	MAIN_WATCHDOG_kick();
            RF_MGR_tick();
        	break;

        case 820u:
        	ROTARY_tick();
        	break;

        case 840u:
            AIRCON_tick();
        	break;

        case 860u:
        	ROTARY_tick();
        	break;

        case 880u:
        	NVM_tick();
        	break;

        case 900u:
        	MAIN_WATCHDOG_kick();
            RF_MGR_tick();
        	ROTARY_tick();
        	break;

        case 920u:
        	break;

        case 940u:
            AIRCON_tick();
        	ROTARY_tick();
        	break;

        case 960u:
        	RF_MGR_analyse_fault_conditions();
        	break;

        case 980u:
        	NVM_tick();
        	ROTARY_tick();
        	break;

        case 1000u:
        	MAIN_WATCHDOG_kick();
            RF_MGR_tick();

            /* keep track of time in secs */
            HAL_TIM_increment_secs();
        	break;

			default:
			break;
    }

	if( MODE_MGR_tick_timer_msecs_s >= MODE_MGR_MAX_TICK_CYCLE_VALUE )
	{
		MODE_MGR_tick_timer_msecs_s = MODE_MGR_TICK_RATE_MSECS;
	}
	else
	{
		MODE_MGR_tick_timer_msecs_s += MODE_MGR_TICK_RATE_MSECS;
	}
}




MODE_MGR_mode_et MODE_MGR_get_mode( void )
{
	return MODE_MGR_mode_s;
}




void MODE_MGR_change_mode( void )
{
	false_true_et reset_value = FALSE;
	(void)reset_value;

	switch( MODE_MGR_mode_s )
	{
		case MODE_MGR_MODE_NORMAL:
			break;

		case MODE_MGR_SETTINGS_MODE:
		    break;

        default:
            break;
	}
}




void MODE_MGR_action_selector_switch_changes( HAL_BRD_switch_slider_et slider, low_high_et state )
{
	switch ( slider )
	{
		case SLIDER_1:
			AIRCON_toggle_state();
			break;


		case SLIDER_2:
			/* This switch has changed state, we dont care what the actual state is ( high or low ) 0 as long as we detect a change */
			AIRCON_toggle_mode();
			break;

		default:
			break;
	}
}




void MODE_MGR_check_user_input( void )
{
	u8_t i = 0u;

	/* First read the low level state of the pins */
	for( i = 0u; i < MODE_MGR_NUM_SLIDER_INPUTS; i++ )
	{
		MODE_MGR_user_input_s[i].slider_state_ll = HAL_BRD_read_selector_switch_pin( (HAL_BRD_switch_slider_et)i );

		/* If the low level state is != the high level state then start the debounce */
		if( MODE_MGR_user_input_s[i].slider_state_ll != MODE_MGR_user_input_s[i].slider_state_hl )
		{
			if( MODE_MGR_user_input_s[i].slider_debounce_time < U8_T_MAX )
			{
				MODE_MGR_user_input_s[i].slider_debounce_time++;
			}
		}
		else
		{
			MODE_MGR_user_input_s[i].slider_debounce_time = 0u;
		}
	}

	/* Now check if the debounce times have been elapsed */
	for( i = 0u; i < MODE_MGR_NUM_SLIDER_INPUTS; i++ )
	{
		if( MODE_MGR_user_input_s[i].slider_debounce_time >= MODE_MGR_SLIDER_DB_TICKS )
		{
			/* Toggle the high level state */
			MODE_MGR_user_input_s[i].slider_state_hl ^= 1u;
			MODE_MGR_user_input_s[i].slider_debounce_time = 0u;

			MODE_MGR_action_selector_switch_changes( (HAL_BRD_switch_slider_et)i, MODE_MGR_user_input_s[i].slider_state_hl );
		}
	}
}




/*!
****************************************************************************************************
*
*   \brief          Requests a SPOT weld so that on the next MODE_MGR tick it gets carried out
*
*   \author
*
*   \return         none
*
***************************************************************************************************/
void MODE_MGR_set_system_init_status( false_true_et state )
{
	MODE_MGR_system_init_s = state;
}











