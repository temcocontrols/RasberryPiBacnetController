/*********************** START OF MSDOS.CPP ***********************************
 * File: msdos.cpp
 *
 * DESCRIPTION:                                                            
 * This module contains OS specific routines.  These routines are  
 * all defined for MS-DOS.  When the target OS is OS/2, Windows, or
 * UNIX, different versions of these routines must be linked in.   
 *                                                                 
 * TIMER INPUT FREQ = 1,193,180 Hz
 * CLOCK FREQ = 18.2064819335938
 * TICK PERIOD = 54,925.4932197992 micro sec
 ******************************************************************************

 
/******************************************************************************
 * INCLUDEs
 *****************************************************************************/

#include <stdlib.h>
#include <math.h>
#include "t3000def.h"
#include "rs232.h"
#include "_msdos.h"
#include "t3000def.h"
#include "mtkernel.h"


/******************************************************************************
 * GLOBALs
 *****************************************************************************/

extern int tswitch;
long time_delay;


/******************************************************************************
 * FUNCTION DEFINATIONs
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION: IdleFunction
 * 
 * DESCRIPTION:
 * The default idle function for MS-DOS does nothing.
 *****************************************************************************/

int RS232::IdleFunction( void )
{
	return RS232_SUCCESS;
}

/*****************************************************************************
 * FUNCTION: ReadTime
 * 
 * DESCRIPTION:
 * ReadTime() returns the current time of day in milliseconds.
 *****************************************************************************/

unsigned long ReadTime( void )
{	
	unsigned long ms;
    struct timespec spec;

    //clock_gettime(CLOCK_REALTIME, &spec);
	clock_gettime(CLOCK_MONOTONIC, &spec);
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
	
	return ms;
}

// This function waits a specified number of miliseconds based on ReadTime
// function - it doesn't use interrupts

void Delay( unsigned long time )
{
#ifdef BAS_TEMP
//	time_delay = time;
//	while( time_delay >= 0 );
	tasks[tswitch].delay_time = (long)time;
	while( tasks[tswitch].delay_time >= 0 );
#endif //BAS_TEMP
}

// This function determines whether we are on a Microchannel
// or ISA machine.

/*BusType Bus( void )
{
		union REGS r;
		struct SREGS s = { 0, 0, 0, 0 };
	 char FAR *system_data;

	 r.h.ah = 0xc0;
	 int86x( 0x15, &r, &r, &s );
	 if ( r.x.cflag )
		return ISA_BUS;
	 system_data = ( char FAR *) ( ( (long) s.es << 16 ) + r.x.bx );
	 if ( system_data[ 5 ] & 0x02 )
		return MCA_BUS;
	 else
		return ISA_BUS;
}
  */
// *********************** END OF MSDOS.CPP ***********************
