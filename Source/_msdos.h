/******************************************************************************
 * File Name: _msdos.h
 * 
 * Description: 
 * This file contains header files used to call MS-DOS specific support 
 * routines.  The only one defined at this time is the Bus() function, which 
 * determines whether an ISA or Microchannel bus is in use on the target 
 * machine.
 *
 * Created:
 * Author:
 *****************************************************************************/
 
#ifndef __MSDOS_DOT_H
#define __MSDOS_DOT_H
/*
enum BusType { ISA_BUS, MCA_BUS };
BusType Bus( void );*/
void Delay( unsigned long );

long filelength(int fhandle);


#endif  // #ifndef __MSDOS_DOT_H
