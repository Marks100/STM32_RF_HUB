#ifndef WDG_H
#define WDG_H


/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "C_defs.h"
#include "COMPILER_defs.h"


/***************************************************************************************************
**                              Defines                                                           **
***************************************************************************************************/




/***************************************************************************************************
**                              Constants                                                         **
***************************************************************************************************/
/* None */


/***************************************************************************************************
**                              Data Types and Enums                                              **
***************************************************************************************************/
/* None */





/***************************************************************************************************
**                              Exported Globals                                                  **
***************************************************************************************************/
/* None */



/***************************************************************************************************
**                              Function Prototypes                                               **
***************************************************************************************************/
void WDG_init( void );
void WDG_deinit( void );
void WDG_kick( void );


#endif /* WDG_H multiple inclusion guard */

///****************************** END OF FILE *******************************************************/