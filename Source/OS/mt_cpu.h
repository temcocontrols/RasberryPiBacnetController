

#ifndef OS_CPU_LINUX_PORT
#ifdef OS_CPU_GLOBALS
#define OS_CPU_EXT
#else
#define OS_CPU_EXT extern
#endif

#include <signal.h>
#include <stddef.h>

/*
*********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)
*********************************************************************************************************
*/
typedef unsigned char		BOOLEAN;
typedef unsigned char		INT8U;
typedef signed   char		INT8S;
typedef unsigned short int	INT16U;
typedef signed   short int	INT16S;
typedef unsigned int		INT32U;
typedef signed   int		INT32S;
typedef float			FP32;
typedef INT32U			OS_STK;
//typedef sigset_t		OS_CPU_SR;
#ifndef OS_CPU_SR
#define OS_CPU_SR		sigset_t new_cpu_sr; int
#endif

/*
*********************************************************************************************************
*                                           Critical Method MACROS
*********************************************************************************************************
*/

#define	OS_CRITICAL_METHOD 3

/* Block interrupts by blocking the alarm signal */
#define	OS_ENTER_CRITICAL() { cpu_sr = cpu_sr; sigset_t mask; sigemptyset( &mask ); sigaddset( &mask, SIGALRM ); sigprocmask( SIG_SETMASK, &mask, &new_cpu_sr ); }

/* Unblock the alarm signal */
#define	OS_EXIT_CRITICAL() { sigprocmask(SIG_SETMASK, &new_cpu_sr, NULL); }

/*
**********************************************************************************************************
*                                          Miscellaneous
**********************************************************************************************************
*/

#define	OS_STK_GROWTH		1u					/* Stack grows from HIGH to LOW memory on Linux x86             */

#define  OS_TASK_SW()         { OSCtxSw(); }                            /* Perform task switch                                      */


/*
**********************************************************************************************************
*                                         Function Prototypes
**********************************************************************************************************
*/

/*
 * Extra functionality of this port. Minimum stack size. Pthreads creates its own stack for each thread(task)
 * but a minimum size is needed internally for the ports use.
 */
int		OSMinStkSize(void);

void		OSCtxSw   (void);
void		OSIntCtxSw(void);

void		OSStartHighRdy(void);

#if 0
CPU_BOOLEAN	OSIntCurTaskSuspend(void);
CPU_BOOLEAN	OSIntCurTaskResume (void);
#endif

void		OSDebuggerBreak(void);

/*
*********************************************************************************************************
*                                             MODULE END
*
* Note(s) : (1) See 'lib_def.h  MODULE'.
*********************************************************************************************************
*/

#endif                                                          /* End of os cpu module include.                        */
