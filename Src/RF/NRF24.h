/*! \file
*               $Revision: 16923 $
*
*               $Author: mstewart $
*
*               $Date: 2014-01-16 15:40:40 +0000 (Thu, 16 Jan 2014) $
*
*               $HeadURL: https://selacvs01.schrader.local:8443/svn/ECU_Software/LF_TOOL_GEN2/trunk/Src/HAL/RF/RF.h $
*
*   \brief      Public header file for RF module.
*/
/* COPYRIGHT NOTICE
* ==================================================================================================
*
* The contents of this document are protected under copyright and contain commercially and/or
* technically confidential information. The content of this document must not be used other than
* for the purpose for which it was provided nor may it be disclosed or copied (by the authorised
* recipient or otherwise) without the prior written consent of an authorised officer of Schrader
* Electronics Ltd.
*
*         (C) $Date:: 2014#$ Schrader Electronics Ltd.
*
****************************************************************************************************
*/
#ifndef RF_H
#define RF_H

/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "C_defs.h"
#include "COMPILER_defs.h"



/***************************************************************************************************
**                              Defines                                                           **
***************************************************************************************************/

/* Bit Mnemonics */
#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRCO        2
#define PWR_UP      1
#define PRIM_RX     0
#define ENAA_P5     5
#define ENAA_P4     4
#define ENAA_P3     3
#define ENAA_P2     2
#define ENAA_P1     1
#define ENAA_P0     0
#define ERX_P5      5
#define ERX_P4      4
#define ERX_P3      3
#define ERX_P2      2
#define ERX_P1      1
#define ERX_P0      0
#define AW          0
#define ARD         4
#define ARC         0
#define PLL_LOCK    4
#define RF_DR       3
#define RF_PWR      6
#define RX_DR       6
#define TX_DS       5
#define MAX_RT      4
#define RX_P_NO     1
#define TX_FULL     0
#define PLOS_CNT    4
#define ARC_CNT     0
#define TX_REUSE    6
#define FIFO_FULL   5
#define TX_EMPTY    4
#define RX_FULL     1
#define RX_EMPTY    0
#define DPL_P5	    5
#define DPL_P4	    4
#define DPL_P3	    3
#define DPL_P2	    2
#define DPL_P1	    1
#define DPL_P0	    0
#define EN_DPL	    2
#define EN_ACK_PAY  1
#define EN_DYN_ACK  0


/* P model bit Mnemonics */
#define RF_DR_LOW   5
#define RF_DR_HIGH  3
#define RF_PWR_LOW  1
#define RF_PWR_HIGH 2



/***************************************************************************************************
**                              Constants                                                         **
***************************************************************************************************/
/* None */


/***************************************************************************************************
**                              Data Types and Enums                                              **
***************************************************************************************************/
/* None */

typedef enum
{
    NRF24_POWERING_UP  = 0,
    NRF24_INITIALISING,
    NRF24_RUNNING,
    NRF24_SETUP_TX_MODE,
    NRF24_TX_MODE,
    NRF24_SETUP_RX_MODE,
    NRF24_RX_MODE,
    NRF24_IDLE,
} NRF24_state_et;



typedef enum
{
	CONFIG 			= 0,
	EN_AUTO_ACK,
	EN_RXADDR,
	SETUP_AW,
	SETUP_RETR,
	RF_CH,
	RF_SETUP,
	STATUS,
	OBSERVE_TX,
	CD,
	RX_ADDR_P0,
	RX_ADDR_P1,
	RX_ADDR_P2,
	RX_ADDR_P3,
	RX_ADDR_P4,
	RX_ADDR_P5,
	TX_ADDR ,
	RX_PW_P0,
	RX_PW_P1,
	RX_PW_P2,
	RX_PW_P3,
	RX_PW_P4,
	RX_PW_P5,
	FIFO_STATUS,
	DYNPD			= 28,
	FEATURE,
    ADDRESS_NOP,
	REGISTER_MAX
} NRF24_registers_et;

typedef enum
{
    /* Instruction Mnemonics */
    R_REGISTER    = 0x00,
    W_REGISTER    = 0x20,
    REGISTER_MASK = 0x1F,
    ACTIVATE      = 0x50,
    R_RX_PL_WID   = 0x60,
    R_RX_PAYLOAD  = 0x61,
    W_TX_PAYLOAD  = 0xA0,
    W_ACK_PAYLOAD = 0xA8,
    FLUSH_TX      = 0xE1,
    FLUSH_RX      = 0xE2,
    REUSE_TX_PL   = 0xE3,
    NOP           = 0xFF
} NRF24_instruction_et;


typedef enum
{
    RF_MIN_TX_PWR = 0u,
    RF_MED_1_TX_PWR,
    RF_MED_2_TX_PWR,
    RF_MAX_TX_PWR
} NRF24_power_level_et;

typedef enum
{
  	TX_ENABLE = 0u,
	RX_ENABLE
} NRF24_tx_rx_mode_et;

typedef enum
{
    RF24_1MBPS = 0u,
    RF24_2MBPS,
    RF24_250KBPS
} NRF24_air_data_rate_et;

typedef enum
{
  RF24_CRC_DISABLED = 0,
  RF24_CRC_8,
  RF24_CRC_16
} NRF24_crclength_et;




/***************************************************************************************************
**                              Exported Globals                                                  **
***************************************************************************************************/
/* None */



/***************************************************************************************************
**                              Function Prototypes                                               **
***************************************************************************************************/
void                 NRF24_init( void );
void                 NRF24_20ms_tick( void );
pass_fail_et         NRF24_read_registers( NRF24_instruction_et instruction, NRF24_registers_et address, u8_t read_data[], u8_t num_bytes );
pass_fail_et         NRF24_write_registers( NRF24_instruction_et instruction, NRF24_registers_et address, u8_t write_data[], u8_t num_bytes );
pass_fail_et         NRF24_set_power_mode( disable_enable_et state );
pass_fail_et         NRF24_set_tx_rx_mode( NRF24_tx_rx_mode_et mode );
pass_fail_et         NRF24_set_channel( u8_t channel );
pass_fail_et         NRF24_flush_rx( void ) ;
pass_fail_et         NRF24_flush_tx( void );
NRF24_power_level_et NRF24_get_PA_TX_power(void);
pass_fail_et         NRF24_set_PA_TX_power( NRF24_power_level_et value);
u8_t                 NRF24_get_nRF_status(void);
pass_fail_et         NRF24_set_rf_data_rate( NRF24_air_data_rate_et value );
pass_fail_et         NRF24_open_write_data_pipe( u8_t pipe_num, const u8_t* data_pipe_address );
u8_t                 NRF24_get_nRF_FIFO_status(void);
pass_fail_et         NRF24_status_register_clr_bit( u8_t bit_mask );
pass_fail_et         NRF24_send_payload( u8_t* buffer, u8_t len );
pass_fail_et         NRF24_get_payload( u8_t* buffer );
pass_fail_et         NRF24_toggle_features_register( void );
pass_fail_et         NRF24_set_dynamic_payloads( disable_enable_et state );

pass_fail_et         NRF24_read_all_registers( void );
void                 NRF24_setup_payload( u8_t* data_p, u8_t len );
NRF24_state_et       NRF24_get_mode_status( void );

#endif /* RF_H multiple inclusion guard */

/****************************** END OF FILE *******************************************************/
