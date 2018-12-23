/*! \file
*               $Revision: 16923 $
*
*               $Author: mstewart $
*
*               $Date: 2014-01-16 15:40:40 +0000 (Thu, 16 Jan 2014) $
*
*               $HeadURL:
*
*   \brief      RF module
*/
/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#ifndef UNIT_TEST
#endif

#ifdef GCC_TEST
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif // GCC_TEST

#include "C_defs.h"
#include "STDC.h"
#include "COMPILER_defs.h"
#include "HAL_BRD.h"
#include "HAL_SPI.h"
#include "NVM.h"
#include "main.h"
#include "NRF24.h"
#include "NRF24_Registers.h"



/* Module Identification for STDC_assert functionality */
#define STDC_MODULE_ID   STDC_MOD_RF

/* ^ different data pipes that can be used :) */
STATIC const u8_t NRF24_data_pipe_default_s [5] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
STATIC const u8_t NRF24_data_pipe_custom_s_1[5] = {0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
STATIC const u8_t NRF24_data_pipe_custom_s_2[5] = {0xBB, 0xCC, 0xDD, 0xEE, 0xAA};
STATIC const u8_t NRF24_data_pipe_custom_s_3[5] = {0xBB, 0xCC, 0xDD, 0xEE, 0xBB};
STATIC const u8_t NRF24_data_pipe_custom_s_4[5] = {0xBB, 0xCC, 0xDD, 0xEE, 0xCC};
STATIC const u8_t NRF24_data_pipe_custom_s_5[5] = {0xBB, 0xCC, 0xDD, 0xEE, 0xDD};
STATIC const u8_t NRF24_data_pipe_test_s[5];

STATIC NRF24_state_et NRF24_state_s = NRF24_POWERING_UP;

STATIC u8_t  NRF24_rx_rf_payload_s[NRF_MAX_PAYLOAD_SIZE];
STATIC u8_t  NRF24_status_register_s;
STATIC u8_t  NRF24_register_readback_s[DEFAULT_CONFIGURATION_SIZE];
STATIC u8_t  NRF24_fifo_status_s;
STATIC u16_t NRF24_cycle_counter_s;
STATIC u8_t  NRF24_tx_rf_payload_s[NRF_MAX_PAYLOAD_SIZE];



/***************************************************************************************************
**                              Data declarations and definitions                                 **
***************************************************************************************************/
/* None */



/***************************************************************************************************
**                              Public Functions                                                  **
***************************************************************************************************/
/*!
****************************************************************************************************
*
*   \brief         Module (re-)initialisation function
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void NRF24_init( void )
{
	/* Set up the state to initialise the module in the 1 sec tick */
	NRF24_state_s = NRF24_POWERING_UP;
	NRF24_cycle_counter_s = 0u;
	NRF24_status_register_s = 0u;
	NRF24_fifo_status_s = 0u;
	u8_t i = 0u;

    STDC_memset( NRF24_tx_rf_payload_s, 0xFF, 32 );
    STDC_memset( NRF24_rx_rf_payload_s, 0xFF, sizeof(NRF24_rx_rf_payload_s) );
}




/*!
****************************************************************************************************
*
*   \brief         Initialise all the RF registers
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
pass_fail_et NRF24_set_configuration( NRF24_static_configuration_et config )
{
    pass_fail_et returnType = PASS;

    if( config >= NRF24_CFG_MAX )
    {
        // invalid configuration
        STDC_basic_assert();
    }
    else
    {
        /* Config is OK */
    	u8_t len;
    	len = NRF24_config_c[ config ].length;

        u8_t i;

        for( i = 0u; i < len; i++ )
        {
            // write to all registers defined in lookup table for configuration to RFM69 chip
            if( NRF24_write_registers( W_REGISTER, NRF24_config_c[config].buffer_p[i].NRF24_register, &NRF24_config_c[config].buffer_p[i].register_data, 1 ) == FAIL )
            {
                /* Configuration failed :( */
                STDC_basic_assert();

                returnType = FAIL;
            }
        }
    }

    return ( returnType );
}






/*!
****************************************************************************************************
*
*   \brief         Initialise all the RF registers
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void NRF24_setup_default_registers( void )
{
    /* Setup all the default register values, these can all be modified later */
    u8_t register_val;

    register_val = 0x0E;
    NRF24_write_registers( W_REGISTER, CONFIG, &register_val, 1 );

    register_val = 0x00;
    NRF24_write_registers( W_REGISTER, EN_AUTO_ACK, &register_val, 1 );

    register_val = 0x03;
    NRF24_write_registers( W_REGISTER, EN_RXADDR, &register_val, 1 );

    register_val = 0x00;
    NRF24_write_registers( W_REGISTER, SETUP_RETR,  &register_val, 1 );

    register_val = 0x6C;
    NRF24_write_registers( W_REGISTER, RF_CH, &register_val, 1 );

    register_val = 0x26;
    NRF24_write_registers( W_REGISTER, RF_SETUP, &register_val, 1 );

    NRF24_write_registers( W_REGISTER, RX_ADDR_P0, (u8_t*)&NRF24_data_pipe_default_s, sizeof(NRF24_data_pipe_default_s) );

    NRF24_write_registers( W_REGISTER, TX_ADDR, (u8_t*)&NRF24_data_pipe_default_s, sizeof(NRF24_data_pipe_default_s) );

    register_val = 0x20;
    NRF24_write_registers( W_REGISTER, RX_PW_P0, &register_val, 1 );

    /* Flush out the tx and rx buffers */
    NRF24_flush_rx();
    NRF24_flush_tx();

    NRF24_set_rf_data_rate(RF24_250KBPS);
    //RF_set_rf_data_rate( RF24_1MBPS );
}






/*!
****************************************************************************************************
*
*   \brief         Reads several bytes from the RF chip
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
pass_fail_et NRF24_read_registers( NRF24_instruction_et instruction, NRF24_registers_et address, u8_t read_data[], u8_t num_bytes )
{
    /* assume pass until prove otherwise */
    pass_fail_et result = PASS;
    u8_t i = 0;

    /* Instruction and address looks good so carry out the read */

    /* pull NCS / Slave Select line low */
    NRF24_spi_slave_select( LOW );

    if( ( instruction == R_RX_PAYLOAD ) || ( instruction == R_RX_PL_WID ) )
    {
        /* Send the instruction as the address */
        HAL_SPI_write_and_read_data( instruction );
    }
    else
    {
        /* Send the address */
        HAL_SPI_write_and_read_data( address );
    }

    /* Writes data to chip */
    for( i = 0; i < num_bytes; i++ )
    {
        /* send dummy byte to force readback */
        read_data[ i ] = (u8_t)HAL_SPI_write_and_read_data( 0xFF );
    }

    /* pull NCS / Slave Select line high */
    NRF24_spi_slave_select( HIGH );

    return( result );
}




/*!
*******************************************************************************
*
*   \brief          Writes several bytes of consecutive data starting at a particular register
*
*   \author         MS
*
*   \param          config - configuration in question
*   \param          instruction - Instruction to be executed
*   \param          address - address to be written to
*   \param          write_data - data to be written
*   \param          num_bytes - Number of bytes to be written
*
*
*   \return         result - pass or fail
*
*******************************************************************************
*/
pass_fail_et NRF24_write_registers( NRF24_instruction_et instruction, NRF24_registers_et address, u8_t write_data[], u8_t num_bytes )
{
    // assume pass until prove otherwise
    pass_fail_et result = PASS;
    u8_t read_back_register[5] = {0u,};

    u8_t i;
    u8_t modified_address;

    /* pull NCS / Slave Select line low */
    NRF24_spi_slave_select( LOW );

    /* Check if this is a register write */
    if( instruction == W_REGISTER )
    {
        /* Now we need to modify the address with the "write" bit set */
        modified_address = address | NRF24_WRITE_BIT;

        /* Send register address */
        HAL_SPI_write_and_read_data( modified_address );
    }
    else if( instruction == R_REGISTER )
    {
        /* Send register address */
        HAL_SPI_write_and_read_data( address );
    }
    else
    {
        /* These registers are the instructions themselves,
        so instead of using the address, we write down the instruction */
        HAL_SPI_write_and_read_data( instruction );
    }

    /* Write all data to chip */
    for( i = 0; i < num_bytes; i++ )
    {
        HAL_SPI_write_and_read_data( write_data[ i ] );
    }

     /* release NCS / Slave Select line high */
    NRF24_spi_slave_select( HIGH );

    if( ( instruction == W_REGISTER ) && ( address != STATUS ) )
    {
        /* Now read back the register to ensure it was written succesfully,
        NOTE!! We can only read back the registers that we can write data to */
        NRF24_read_registers( R_REGISTER, address, read_back_register, num_bytes );

        /* compare actual values with expected values */
        if( STDC_memcompare( read_back_register, write_data, num_bytes ) == FALSE )
        {
          result = FAIL;
          STDC_basic_assert();
        }
    }

    return( result );
}



/*!
*******************************************************************************
*
*   \brief          Powers up or down the RF chip by using the SPI command ( Register 0 ( CONFIG ) )
*
*   \author         MS
*
*   \return         result - pass or fail
*
*******************************************************************************
*/
pass_fail_et NRF24_set_low_level_mode( NRF24_low_level_mode_et mode )
{
 	u8_t register_val;

 	/* Read back the current register status, modify it and then rewrite it back down */
	NRF24_read_registers( R_REGISTER, CONFIG, &register_val, 1 );

 	/* Power down mode is the only mode that sets the PWR_UP bit low, so by default set it high */
 	register_val  |= ( 1 << PWR_UP );

 	/* RX mode is the only mode thats sets the PRIM RX bit high so lets set it low by default */
 	register_val &= ~( 1 << PRIM_RX );

 	switch( mode )
 	{
        case NRF_POWER_DOWN_MODE:
            register_val &= ~( 1 << PWR_UP );
            break;

        case NRF_STANDBY_1_MODE:
            break;

        case NRF_STANDBY_2_MODE:
            break;

        case NRF_RX_MODE:
            register_val |= ( 1 << PRIM_RX );
            break;

        case NRF_TX_MODE:
            break;

        default:
            break;
 	}

	NRF24_write_registers( W_REGISTER, CONFIG, &register_val, 1 );

    return( PASS );
}




/*!
*******************************************************************************
*
*   \brief          Sets the radio chanel by using the SPI command ( Register 0 ( CONFIG ) )
*
*   \author         MS
*
*   \return         result - pass or fail
*
*******************************************************************************
*/
pass_fail_et NRF24_set_channel( u8_t channel )
{
 	u8_t register_val;

 	/* The channel does not need to be masked but bit 7 always needs to be 0 */
	if( channel > NRF_MAX_CHANNEL_SELECTION )
	{
	  	channel = NRF_MAX_CHANNEL_SELECTION;
	}

	NRF24_write_registers( W_REGISTER, RF_CH, &channel, 1 );

    return( PASS );
}




/*!
*******************************************************************************
*
*   \brief          Flush the RF TX buffers using the SPI command
*
*   \author         MS
*
*   \return         result - pass or fail
*
*******************************************************************************
*/
pass_fail_et NRF24_flush_tx( void )
{
    u8_t data = NOP;
	NRF24_write_registers( FLUSH_TX, ADDRESS_NOP, &data, 1 );

    return ( PASS );
}



/*!
*******************************************************************************
*
*   \brief          Flush the RF RX buffers using the SPI command
*
*   \author         MS
*
*   \return         result - pass or fail
*
*******************************************************************************
*/
pass_fail_et NRF24_flush_rx( void )
{
    u8_t data = NOP;
	NRF24_write_registers( FLUSH_RX, ADDRESS_NOP, &data, 1 );

    return ( PASS );
}




/*!
*******************************************************************************
*
*   \brief          Grab the curent Dynamic Payload Length ( DPL )
*
*   \author         MS
*
*   \return         Payload size in bytes
*
*******************************************************************************
*/
u8_t NRF24_get_DPL_size(void)
{
    u8_t packet_size = 0u;

    /* Read back the current register status, modify it and then rewrite it back down */
    NRF24_read_registers( R_RX_PL_WID, ADDRESS_NOP, &packet_size, 1 );

    return ( packet_size );
}




/*!
*******************************************************************************
*
*   \brief          Set the output TX power
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_set_PA_TX_power( NRF24_power_level_et value)
{
    u8_t register_val;

    /* Read back the current register status, modify it and then rewrite it back down */
    NRF24_read_registers( R_REGISTER, RF_SETUP, &register_val, 1 );

    register_val &= ~( RF_PWR << 1 ) ;
    register_val |= ( value << 1 ) ;

    NRF24_write_registers( W_REGISTER, RF_SETUP, &register_val, 1 );

    return ( PASS );
}



/*!
*******************************************************************************
*
*   \brief          Grab the current output TX power
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
NRF24_power_level_et NRF24_get_PA_TX_power(void)
{
    u8_t pa_val;

    /* Read back the current register status, modify it and then rewrite it back down */
    NRF24_read_registers( R_REGISTER, RF_SETUP, &pa_val, 1 );

    /* Pull out the power level */
    pa_val = ( pa_val & 0x06 ) >> 1;

    return ( ( NRF24_power_level_et)pa_val );
}




/*!
*******************************************************************************
*
*   \brief          Grab the current status of the RF chip
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
u8_t NRF24_get_status(void)
{
    u8_t status = 0;

    /* Read back the current register status, modify it and then rewrite it back down */
    NRF24_read_registers( R_REGISTER, STATUS, &status, 1 );

    return ( status );
}



/*!
*******************************************************************************
*
*   \brief          Grab the current status of the RF chip
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
low_high_et NRF24_check_status_mask( MRF24_status_masks_et mask, u8_t* data_p )
{
    u8_t status;
    low_high_et returnval = 0u;

    /* firstly grab the status byte */
    status = NRF24_get_status();
    *data_p =  status;

    switch( mask )
    {
        case RF24_RX_DATA_READY:
            returnval |= ( ( status & ( 1 << MASK_RX_DR ) ) >> MASK_RX_DR ) ;
            break;

        case RF24_TX_DATA_SENT:
            returnval |= ( ( status & ( 1 << MASK_TX_DS ) ) >> MASK_TX_DS );
            break;

        case RF24_MAX_RETR_REACHED:
            returnval |= ( ( status & ( 1 << MASK_MAX_RT ) ) >> MASK_MAX_RT );
            break;

        case RF24_RX_PIPE_DATA_NUM:
            returnval |= ( ( status & ( 7 << MASK_RX_P_NO ) ) >> MASK_RX_P_NO );
            returnval = LOW;
            break;

        case RF24_TX_FIFO_FULL:
            returnval |= ( ( status & ( 1 << MASK_TX_FULL ) ) >> MASK_TX_FULL);
            break;

        default:
            break;
    }

    return ( returnval );
}




/*!
*******************************************************************************
*
*   \brief          Grab the current status of the RF chip
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
u8_t NRF24_get_nRF_FIFO_status(void)
{
    u8_t status = 0;

    /* Read back the current register status, modify it and then rewrite it back down */
    NRF24_read_registers( R_REGISTER, FIFO_STATUS, &status, 1 );

    return ( status );
}



/*!
*******************************************************************************
*
*   \brief          Sets the RF air data rate
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_set_rf_data_rate( NRF24_air_data_rate_et value )
{
    u8_t register_val;

    /* Read back the current register status, modify it and then rewrite it back down */
    NRF24_read_registers( R_REGISTER, RF_SETUP, &register_val, 1 );

    /* Mask of the data rate */
    register_val &= ~( 1 << RF_DR_LOW );
    register_val &= ~( 1 << RF_DR_HIGH );

    switch( value )
    {
        case RF24_250KBPS:
            register_val |= ( 1 << RF_DR_LOW );
            break;

        case RF24_1MBPS:
            break;

        case RF24_2MBPS:
            register_val |= ( 1 << RF_DR_HIGH );
            break;

        default:
            break;
    }

    NRF24_write_registers( W_REGISTER, RF_SETUP, &register_val, 1 );

    return ( PASS );
}



/*!
*******************************************************************************
*
*   \brief          Sets the auto acknowledgement on a specific data pipe
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_set_AA_data_pipe( disable_enable_et state, u8_t pipe_num )
{
    u8_t register_val;

    if( pipe_num > NRF_MAX_NUM_PIPES )
    {
        pipe_num = NRF_MAX_NUM_PIPES;
    }

    /* Read back the current register status, modify it and then rewrite it back down */
    NRF24_read_registers( R_REGISTER, EN_AUTO_ACK, &register_val, 1 );

    /* Mask of the bit */
    register_val &= ~( 1 << pipe_num );
    register_val |= ( state << pipe_num );

    NRF24_write_registers( W_REGISTER, EN_AUTO_ACK, &register_val, 1 );

    return ( PASS );
}



/*!
*******************************************************************************
*
*   \brief          Enables or disables a specific data pipe
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_enable_data_pipe( disable_enable_et state, u8_t pipe_num )
{
    u8_t register_val;

    if( pipe_num > NRF_MAX_NUM_PIPES )
    {
        pipe_num = NRF_MAX_NUM_PIPES;
    }

    /* Read back the current register status, modify it and then rewrite it back down */
    NRF24_read_registers( R_REGISTER, EN_RXADDR, &register_val, 1 );

    /* Mask of the bit */
    register_val &= ~( 1 << pipe_num );
    register_val |= ( state << pipe_num );

    NRF24_write_registers( W_REGISTER, EN_RXADDR, &register_val, 1 );

    return ( PASS );
}


/*!
*******************************************************************************
*
*   \brief          Setup the retransmit time and count
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_setup_retriese( NRF24_retransmitt_time_et time, u8_t counts )
{
    u8_t register_val;

    if( counts > NRF_MAX_NUM_RETRIES )
    {
        counts = NRF_MAX_NUM_RETRIES;
    }

    /* Read back the current register status, modify it and then rewrite it back down */
    NRF24_read_registers( R_REGISTER, SETUP_RETR, &register_val, 1 );

    /* Mask of the bit */
    register_val &= ~( 15 << ARD );
    register_val &= ~( 15 << ARC );

    register_val |= ( time << ARD );
    register_val |= ( counts << ARC );

    NRF24_write_registers( W_REGISTER, SETUP_RETR, &register_val, 1 );

    return ( PASS );
}







/*!
*******************************************************************************
*
*   \brief          Opens a pipe to communicate over RF link
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_open_write_data_pipe( u8_t pipe_num, const u8_t* data_pipe_address )
{
    u8_t regiter_val;
    u8_t payload_size = 32u;

    /* 6 pipes can be opened, let the user select from 0 - 5 and auto correct their
    selection if we need to */
    if( pipe_num > NRF_MAX_NUM_PIPES )
    {
        pipe_num = NRF_MAX_NUM_PIPES;
    }

    /* we will add an offset of "0x0A" onto the pipe number as this is the starting address of
    the pipe registers and always make the address of the pipe 5 bytes long*/
    NRF24_write_registers( W_REGISTER, ( NRF24_registers_et)( pipe_num + NRF_DATA_PIPE_OFFSET ), (u8_t*)data_pipe_address, 5 );

    NRF24_write_registers( W_REGISTER, TX_ADDR, (u8_t*)data_pipe_address, 5 );
    NRF24_write_registers( W_REGISTER, ( RX_PW_P0 + pipe_num ) , &payload_size, 1 );

    return ( PASS );
}



/*!
*******************************************************************************
*
*   \brief          Opens a pipe to communicate over RF link
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_read_data_pipe( u8_t pipe_num, const u8_t* data_p )
{
    u8_t regiter_val;

    /* 6 pipes can be opened, let the user select from 0 - 5 and auto correct their
    selection if we need to */
    if( pipe_num > 5 )
    {
        pipe_num = 5;
    }

    /* we will add an offset of "0x0A" onto the pipe number as this is the starting address of
    the pipe registers and always make the address of the pipe 5 bytes long*/
    NRF24_read_registers( R_REGISTER, ( NRF24_registers_et)( pipe_num + 0x0A ), (u8_t*)data_p, 5 );

    return ( PASS );
}




/*!
*******************************************************************************
*
*   \brief          Opens a pipe to communicate over RF link
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_status_register_clr_bit( u8_t bit_mask )
{
    u8_t regiter_val;

    /* Read back the current register status, modify it and then rewrite it back down */
    NRF24_read_registers( R_REGISTER, STATUS, &regiter_val, 1 );

    regiter_val = ( regiter_val | ( 1 << bit_mask ) );

    NRF24_write_registers( W_REGISTER, STATUS, &regiter_val, 1 );

    return ( PASS );
}



/*!
*******************************************************************************
*
*   \brief          Send the payload over the RF link
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_send_payload( u8_t* buffer, u8_t len )
{
    u8_t stuff_buffer_size;
    u8_t dpl_enabled_check;
    disable_enable_et dpl_enabled = DISABLE_;

    /* Is the payload dynamic or static?, Find out by reading the FEATURE register to check
    if DPL is activated */
    NRF24_read_registers( R_REGISTER, FEATURE, &dpl_enabled_check, 1 ) ;

    if( ( dpl_enabled_check & 0x04 ) == ( 1 << EN_DPL ) )
    {
        dpl_enabled = ENABLE_;
    }
    else
    {
        /* Find out the size of the payload setting for the specific pipe */
        NRF24_read_registers( R_REGISTER, RX_PW_P0, &stuff_buffer_size, 1 );

        /* Mask off the size of the payload setting for the specific pipe */
        stuff_buffer_size = ( stuff_buffer_size & 0x3F );

        /* subtract the amount of "actual" data that you have from the max payload setting */
        stuff_buffer_size -= len;
    }

      /* pull NCS / Slave Select line low */
      NRF24_spi_slave_select( LOW );

      /* Send register address */
      HAL_SPI_write_and_read_data( W_TX_PAYLOAD );

      /* Write all the valid data to the TX_buffer */
      while( len -- )
      {
          HAL_SPI_write_and_read_data( *buffer++ );
      }

      if( dpl_enabled != ENABLE_ )
      {
          /* Dynamic packet length is not set so we need to "pad" the rest of the
          transmission to make it up to 32 bytes */
          while( stuff_buffer_size -- )
          {
              HAL_SPI_write_and_read_data( 0x00 );
          }
      }

    /* release NCS / Slave Select line high */
    NRF24_spi_slave_select( HIGH );

    #if(UNIT_TEST!=1)
    delay_us(100);
    #endif

    /* toggle the CE pin to complete the RF transfer */
    NRF24_spi_slave_select(HIGH);

    /* hack a little delay in here */
    #if(UNIT_TEST!=1)
    delay_us(100);
    #endif

    NRF24_spi_slave_select(LOW);

    return ( PASS );
}






/*!
*******************************************************************************
*
*   \brief          grab the payload that has come over the RF link
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_get_payload( u8_t* buffer )
{
    u8_t buffer_size;
    u8_t dpl_enabled_check;

    /* Is the payload dynamic or static?, Find out by reading the FEATURE register to check
    if DPL is activated */
    NRF24_read_registers( R_REGISTER, FEATURE, &dpl_enabled_check, 1 ) ;

    if( ( dpl_enabled_check & 0x04 ) == ( 1 << EN_DPL ) )
    {
        /* Find out the size of the payload setting for the specific pipe */
        NRF24_read_registers( R_RX_PL_WID, ADDRESS_NOP, &buffer_size, 1 );
    }
    else
    {
       /* Find out the size of the payload setting for the specific pipe */
      NRF24_read_registers( R_REGISTER, RX_PW_P0, &buffer_size, 1 );

      /* Mask off the size of the payload setting for the specific pipe */
      buffer_size = ( buffer_size & 0x3F );
    }

    /* pull NCS / Slave Select line low */
    NRF24_spi_slave_select( LOW );

    /* Send register address */
    HAL_SPI_write_and_read_data( R_RX_PAYLOAD );

    /* Write all the valid data to the TX_buffer */

    while( buffer_size -- )
    {
         *buffer++ = HAL_SPI_write_and_read_data( 0xAA );
    }

     /* release NCS / Slave Select line high */
    NRF24_spi_slave_select( HIGH );

    return ( PASS );
}



/*!
*******************************************************************************
*
*   \brief          Enables dynamic payloads
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_set_dynamic_payloads( disable_enable_et state, u8_t pipe_num )
{
    u8_t register_val;
    pass_fail_et return_val;

    if( pipe_num > NRF_MAX_NUM_PIPES )
    {
        pipe_num = NRF_MAX_NUM_PIPES;
    }

    /* Firstly set the Dynamic payload bit in the DYNPD register */
    NRF24_read_registers( R_REGISTER, DYNPD, &register_val, 1 );

    /* This clears the specific DPLD bit */
    register_val &= ~( 1 << pipe_num );
    register_val |= ( state << pipe_num );

    NRF24_write_registers( W_REGISTER, DYNPD, (u8_t*)&register_val, 1 );

    /* That's the individual pipe dynamic payload bit set, next we need to enable the
    EN_DPL bit in the feature register */

    /* Read the feature register */
    NRF24_read_registers( R_REGISTER, FEATURE, &register_val, 1 );

    /* Always enable the dynamic payload bit in the feature register */
    register_val = ( 0 | ( 1 << EN_DPL ) );
    NRF24_write_registers( W_REGISTER, FEATURE, (u8_t*)&register_val, 1 );

    /* Read the feature register */
    NRF24_read_registers( R_REGISTER, FEATURE, &register_val, 1 );

    return ( return_val );
}



/*!
*******************************************************************************
*
*   \brief          Enables dynamic payloads
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_toggle_features_register( void )
{
    pass_fail_et return_val;
    u8_t register_val = 0x73;

    NRF24_write_registers( ACTIVATE, FEATURE, (u8_t*)&register_val, 1 );

    return ( PASS );
}



 /*!
*******************************************************************************
*
*   \brief          Reads back specifig registers for debug  purposes
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_read_all_registers( u8_t* data_p )
{
	pass_fail_et return_val;

    u8_t i;

    for( i = 0u; i < DEFAULT_CONFIGURATION_SIZE; i++ )
    {
        // Read all registers
        if( NRF24_read_registers( R_REGISTER, NRF24_config_c[NRF24_DEFAULT_CONFIG].buffer_p[i].NRF24_register, &data_p[i], 1 ) == FAIL )
        {
            /* Configuration failed :( */
            STDC_basic_assert();

            return_val = FAIL;
        }
    }

	return ( PASS );
}




 /*!
*******************************************************************************
*
*   \brief          sets up the low level payload to be transmitted
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
void NRF24_setup_payload( u8_t* data_p, u8_t len )
{
    pass_fail_et return_val;

    /* We have found a free array so lets fill it */
    STDC_memcpy( NRF24_tx_rf_payload_s, data_p , 32 );
}





/*!
*******************************************************************************
*
*   \brief          sets up the low level payload to be transmitted
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
void NRF24_setup_CRC_scheme( disable_enable_et state, NRF24_crclength_et crc_len )
{
    u8_t register_val;
    pass_fail_et return_val;

    if( crc_len > RF24_CRC_16 )
    {
        crc_len = RF24_CRC_16;
    }

    /* Find out the size of the payload setting for the specific pipe */
    NRF24_read_registers( R_REGISTER, CONFIG, &register_val, 1 );

    /* Clear the CRC enable/disable bit */
    register_val &= ~( 1 << EN_CRC );

    /* Clear the CRC byte num bit */
    register_val &= ~( 1 << CRCO );

    register_val |= ( state << EN_CRC );
    register_val |= ( crc_len << CRCO );

    NRF24_write_registers( W_REGISTER, CONFIG, (u8_t*)&register_val, 1 );

    return ( return_val );
}



/*!
*******************************************************************************
*
*   \brief          sets up the constant RF wave
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
void NRF24_setup_constant_wave( disable_enable_et state )
{
    u8_t register_val;
    pass_fail_et return_val;

    /* Find out the size of the payload setting for the specific pipe */
    NRF24_read_registers( R_REGISTER, RF_SETUP, &register_val, 1 );

    /* Clear the CRC enable/disable bit */
    register_val &= ~( 1 << CONT_WAVE );

    register_val |= ( state << CONT_WAVE );

    NRF24_write_registers( W_REGISTER, RF_SETUP, (u8_t*)&register_val, 1 );

    return ( return_val );
}



/*!
*******************************************************************************
*
*   \brief          sets up the pll
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
void NRF24_setup_pll( disable_enable_et state )
{
    u8_t register_val;
    pass_fail_et return_val;

    /* Find out the size of the payload setting for the specific pipe */
    NRF24_read_registers( R_REGISTER, RF_SETUP, &register_val, 1 );

    /* Clear the CRC enable/disable bit */
    register_val &= ~( 1 << PLL_LOCK );

    register_val |= ( state << PLL_LOCK );

    NRF24_write_registers( W_REGISTER, RF_SETUP, (u8_t*)&register_val, 1 );

    return ( return_val );
}


/*!
*******************************************************************************
*
*   \brief          sets up the data pipe address widths
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
void NRF24_setup_address_widths( NRF24_address_width_et value )
{
    u8_t register_val;
    pass_fail_et return_val;

    NRF24_read_registers( R_REGISTER, SETUP_AW, &register_val, 1 );

    register_val &= ~( 3 << AW );

    register_val |= ( value << AW );

    NRF24_write_registers( W_REGISTER, SETUP_AW, (u8_t*)&register_val, 1 );

    return ( return_val );
}





/*!
****************************************************************************************************
*
*   \brief         RF Module tick
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void NRF24_tick( void )
{
    /* Need to make sure that the SPI interface is initialised */
    if( HAL_SPI_get_init_status() == FALSE )
    {
        /* it is not initialised so do that now, Need to check that the UART isnt currently using the
        hardware pins before we do any initialisation */
        HAL_SPI_init();
    }

    /* The SPI module is initialised so we can carry out the RF task */
    switch( NRF24_state_s )
    {
        case NRF24_POWERING_UP:
        {
            /* This gives the RF chip time to power up and stabilize before we begin
               writing any of the registers */
            NRF24_ce_select( LOW );
            NRF24_spi_slave_select( HIGH );

            NRF24_state_s = NRF24_INITIALISING;
        }
        break;

        case NRF24_INITIALISING:
        {
            NRF24_set_low_level_mode( NRF_STANDBY_1_MODE );

            /* Setup initial register values */
            NRF24_set_configuration( NRF24_DEFAULT_CONFIG );

            NRF24_set_PA_TX_power( RF_MAX_TX_PWR );

            NRF24_read_all_registers( NRF24_register_readback_s );

            /* open up the data pipe to communicate with the receiver */
            NRF24_open_write_data_pipe( 0, NRF24_data_pipe_default_s );
            NRF24_read_data_pipe( 0, NRF24_data_pipe_test_s );
            NRF24_set_dynamic_payloads( ENABLE, 0 );

            /* Setup has completed so now move onto the next state */
            NRF24_state_s = NVM_info_s.NVM_generic_data_blk_s.nrf_startup_tx_rx_mode;
        }
        break;

        case NRF24_SETUP_TX:
        {
            /* carry out the necessary steps to transition to TX_MODE */
            NRF24_set_low_level_mode( NRF_TX_MODE );

            NRF24_ce_select( HIGH );

            /* Flush out the tx and rx buffers */
            NRF24_flush_rx();
            NRF24_flush_tx();

            NRF24_status_register_clr_bit( TX_DS );
            NRF24_status_register_clr_bit( TX_DS );
            NRF24_status_register_clr_bit( TX_DS );

            /* open up the data pipe to communicate with the receiver */
            NRF24_open_write_data_pipe( 1, NRF24_data_pipe_default_s );

            /* Move onto the TX_MODE state */
            NRF24_state_s = NRF24_TX;
        }

        case NRF24_TX:
        {
			if( NRF24_check_status_mask( RF24_TX_DATA_SENT, &NRF24_status_register_s ) == HIGH )
            {
                NRF24_status_register_clr_bit( TX_DS );

                HAL_BRD_toggle_led();

                 //NRF24_state_s = NRF24_POWER_DOWN;
            }

            if( NRF24_cycle_counter_s > 10 )
            {
                NRF24_cycle_counter_s = 0u;

                NRF24_setup_payload( "Open Door", 10 );

                NRF24_send_payload( NRF24_tx_rf_payload_s, 10 );
            }
            else
            {
                NRF24_cycle_counter_s ++;
            }

			NRF24_read_all_registers( NRF24_register_readback_s );
        }
        break;

        case NRF24_SETUP_RX:
        {
        	/* carry out the necessary steps to transition to TX_MODE */
			NRF24_set_low_level_mode( NRF_RX_MODE );

			/* Flush out the tx and rx buffers */
			NRF24_flush_rx();
			NRF24_flush_tx();

			NRF24_status_register_clr_bit( TX_DS );
			NRF24_status_register_clr_bit( TX_DS );
			NRF24_status_register_clr_bit( TX_DS );

			/* open up the data pipe to communicate with the receiver */
			NRF24_open_write_data_pipe( 1, NRF24_data_pipe_default_s );

			/* The CE pin to has to be HIGH to Receive */
			NRF24_ce_select(HIGH);

			/* Move onto the RX_MODE state */
			NRF24_state_s = NRF24_RX;
        }

        case NRF24_RX:
        {
            /* We are now in Receive mode so lets just wait for a packet to come in */
            /* Eventually might be interrupt driven but for now lets poll */

            /* Grab the status of the RF chip */
            NRF24_status_register_s = NRF24_get_status();
            NRF24_fifo_status_s = NRF24_get_nRF_FIFO_status();

            if( NRF24_check_status_mask( RF24_RX_DATA_READY, &NRF24_status_register_s ) == HIGH )
            {
                /* we may have received a packet !!!!!!*/
                NRF24_status_register_clr_bit( RX_DR );

                NRF24_get_payload( NRF24_rx_rf_payload_s );

                HAL_BRD_toggle_led();
            }
        }
        break;

        case NRF24_STANDBY:
            /* Power down the RF chip by setting the low level mode of operation to power down */
            NRF24_set_low_level_mode( NRF_STANDBY_1_MODE );
            break;


        case NRF24_POWER_DOWN:
            /* Power down the RF chip by setting the low level mode of operation to power down */
            NRF24_set_low_level_mode( NRF_POWER_DOWN_MODE );
            break;

        case NRF24_SETUP_CONST_WAVE:
            /* Set to TX mode */
            NRF24_set_low_level_mode( NRF_TX_MODE );

            /* Move onto the NRF24_CONST_WAVE state */
			NRF24_state_s = NRF24_CONST_WAVE;
            break;

        case NRF24_CONST_WAVE:
            NRF24_setup_constant_wave( ENABLE );
            NRF24_setup_pll( ENABLE );
            NRF24_ce_select(HIGH);
            break;

        default:
        break;
    }
}




/*!
****************************************************************************************************
*
*   \brief         Return RF Module status
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
NRF24_state_et NRF24_get_state( void )
{
    return ( NRF24_state_s );
}



/*!
****************************************************************************************************
*
*   \brief         Sets the NRF state machine state RF Module status
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void NRF24_set_state( NRF24_state_et state )
{
    NRF24_state_s = state;
}




void NRF24_spi_slave_select( low_high_et state )
{
    HAL_BRD_NRF24_spi_slave_select( state );
}


void NRF24_ce_select( low_high_et state )
{
    HAL_BRD_NRF24_set_ce_pin_state( state );
}





/***************************************************************************************************
**                              Private Functions                                                 **
***************************************************************************************************/


/****************************** END OF FILE *******************************************************/
