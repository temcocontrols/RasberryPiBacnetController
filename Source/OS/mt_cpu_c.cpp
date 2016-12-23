

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include "mt.h"


/* Check MT_VERSION for compatibility with this port */
#if MT_VERSION < 204
#error MT_VERSION must be >= 204. This port uses OSTCBInitHook.
#endif

/*#if MT_CPU_HOOKS_EN > 0
*********************************************************************************************************
*                                       GLOBAL VARIABLES
*********************************************************************************************************
*/
static int nNumThreadsCreated=0;
static int nNumThreadsStarted=0;

/* Condition variable to indicate whether all threads are waiting for their condition variables. i.e.
   the system is ready to run. */
static pthread_cond_t	cvThreadWrapper;

/* Condition variables for all threads. Index for thread is stored in its stack. */
static pthread_cond_t	grcvThread[ MT_LOWEST_PRIO ];

/* Array of threads */
static pthread_t	threadTask[ MT_LOWEST_PRIO ];

/* Context switching control mutex */
static pthread_mutex_t 	mutThread;


/*
*********************************************************************************************************
*                                       Linux Port Function Prototypes
*********************************************************************************************************
*/
void (*pF)(char*);

typedef struct FuncInfoType
{
	void*	pFunc;
	void*	pArgs;
	int	nThreadIdx;
} FuncInfo;

int OSMinStkSize(){ return( sizeof( FuncInfo ) ); }

#if MT_SYSTEM_RESET_EN > 0
extern void OSSystemResetHook(void);
#endif

static void InitLinuxPort();
static void InitTick();
static void AlarmSigHandler( int signum );

/*
*********************************************************************************************************
*                                       MT Function Prototypes
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          TASK CREATION HOOK
*
* Description: This function is called when a task is created.
*
* Arguments  : ptcb   is a pointer to the task control block of the task being created.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/
void OSTaskCreateHook(MT_TCB *ptcb)
{
#if MT_CPU_HOOKS_EN > 0
#endif
}


/*
*********************************************************************************************************
*                                           TASK DELETION HOOK
*
* Description: This function is called when a task is deleted.
*
* Arguments  : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/
void OSTaskDelHook (MT_TCB *ptcb)
{
#if MT_CPU_HOOKS_EN > 0
#endif
}


/*
*********************************************************************************************************
*                                           TASK SWITCH HOOK
*
* Description: This function is called when a task switch is performed.  This allows you to perform other
*              operations during a context switch.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts are disabled during this call.
*              2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                 will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the
*                 task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/
void OSTaskSwHook (void)
{
#if (MT_CPU_HOOKS_EN > 0)
#endif
}

/*
*********************************************************************************************************
*                                           STATISTIC TASK HOOK
*
* Description: This function is called every second by MTK's statistics task.  This allows your
*              application to add functionality to the statistics task.
*
* Arguments  : none
*********************************************************************************************************
*/
void OSTaskStatHook (void)
{
#if (MT_TASK_STAT_HOOK_EN > 0)
#endif
}

/*
*********************************************************************************************************
*                                               TICK HOOK
*
* Description: This function is called every tick.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/
#if MT_TMR_EN > 0 
static INT16U OSTmrTickCtr = 0; 
#endif
void int8_task_switch();

void OSTimeTickHook (void)
{
#if MT_TMR_EN > 0 
	OSTmrTickCtr++; 
	int8_task_switch();
	if (OSTmrTickCtr >= (MT_TICKS_PER_SEC / MT_TMR_CFG_TICKS_PER_SEC)) 
	{
		OSTmrTickCtr = 0; 
		OSTmrSignal(); 
	} 
#endif
}

/*
*********************************************************************************************************
*                                           Init HOOK begin & end
*
* Description: Called in OSInit.
*
* Arguments  : pointer to TCB
*********************************************************************************************************
*/
void OSInitHookBegin (void)
{
#if MT_CPU_HOOKS_EN > 0 && MT_VERSION >= 204
    InitLinuxPort();
#endif
}

void OSInitHookEnd (void)
{
#if MT_CPU_HOOKS_EN > 0 && MT_VERSION >= 204
#endif
}

/*
*********************************************************************************************************
*                                               IDLE HOOK
*
* Description: This function is called by MT IDLE Task.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/
void OSTaskIdleHook (void)
{
#if MT_CPU_HOOKS_EN > 0 && MT_VERSION >= 251
    select(0, NULL, NULL, NULL, NULL);
#endif
}

/*
*********************************************************************************************************
*                                            TASK RETURN HOOK
*
* Description: This function is called if a task accidentally returns.  In other words, a task should
*              either be an infinite loop or delete itself when done.
*
* Arguments  : p_tcb        Pointer to the task control block of the task that is returning.
*
* Note(s)    : None.
*********************************************************************************************************
*/
void OSTaskReturnHook(MT_TCB *ptcb)
{
#if MT_CFG_APP_HOOKS_EN > 0u
	if (MT_AppTaskReturnHookPtr != (MT_APP_HOOK_TCB)0) {
		(*MT_AppTaskReturnHookPtr)(ptcb);
	}
#else
	(void)ptcb;	/* Prevent compiler warning */
#endif
}

/*
*********************************************************************************************************
*                                               TASK STACK INIT
*
* Description:	This function stores the task information on the task stack for retrieval at a later time.
*
* Arguments  :	pd			pointer to task function.
*				pdata		pointer to task data.
*				ptos		pointer to top of task stack. Pthreads uses its own stack for tasks but this
*							has to be > sizeof( FuncInfo ).
*				opt			not used
*
*********************************************************************************************************
*/
MT_STK* OSTaskStkInit (void (*task)(void* pd), void* pdata, MT_STK* ptos, INT16U opt)
{
	FuncInfo* pFuncInfo = (FuncInfo*)((char *)ptos - OSMinStkSize() + 1);
	pFuncInfo->pFunc = task;
	pFuncInfo->pArgs = pdata;

	return ((MT_STK*)pFuncInfo);
}


/*
*********************************************************************************************************
*                                           System Reset
*
* Description: This function can be called by any task and will call the system reset hook function. This
* hook function should be defined by the application if the flag MT_SYSTEM_RESET_EN is set.
*
* Arguments  : none
*********************************************************************************************************
*/
void OSSystemReset(void)
{
#if MT_SYSTEM_RESET_EN > 0
	/* Stop alarm for system reset */
	alarm(0);

	/* Call application defined system reset hook */
	OSSystemResetHook();

	/* Start alarms */
	InitTick();
#endif
}

/*
*********************************************************************************************************
*                                          START MULTITASKING
*
* Description: This function is used to start the multitasking process which lets MTK manages the
*              task that you have created.  Before you can call OSStart(), you MUST have called OSInit()
*              and you MUST have created at least one task.
*
* Arguments  : none
*
* Returns    : none
*
* Note       : OSStartHighRdy() MUST:
*                 a) Call OSTaskSwHook() then,
*                 b) Set OSRunning to TRUE.
*                 c) Load the context of the task pointed to by OSTCBHighRdy.
*                 d_ Execute the task.
*********************************************************************************************************
*/
void OSStartHighRdy(void)
{
	OSTaskSwHook();
	OSRunning = MT_TRUE;

	/* Wait until all task wrappers have started */
	pthread_mutex_lock (&mutThread);
	if( nNumThreadsCreated != nNumThreadsStarted )
		pthread_cond_wait (&cvThreadWrapper, &mutThread);
	pthread_mutex_unlock (&mutThread);

	/* All tasks are ready so start interrupts */
	InitTick();

	/* Get pointer highest priority thread  */
	FuncInfo* pFuncInfo = (FuncInfo*)OSTCBCur->OSTCBStkPtr;

	/* Let highest prio thread go  */
	pthread_mutex_lock( &mutThread );
	pthread_cond_signal( &grcvThread[ pFuncInfo->nThreadIdx ] );
	pthread_mutex_unlock( &mutThread );

	/* Main thread must be kept going otherwise process turns into zombie */
	while(1);

}

/*
*********************************************************************************************************
*                                           Interrupt Context Switch
*
* Description: Context switch from an interrupt.
*
* Arguments  : none
*********************************************************************************************************
*/
void OSIntCtxSw(void)
{
	/* Grab task switching mutex */
	pthread_mutex_lock( &mutThread );

	/* Get ptrs to new and old context */
	FuncInfo* pFuncInfoNew = (FuncInfo*)OSTCBHighRdy->OSTCBStkPtr;
	FuncInfo* pFuncInfoOld = (FuncInfo*)OSTCBCur->OSTCBStkPtr;

	/* Set current context to highest priority */
	OSTCBCur = OSTCBHighRdy;
	OSPrioCur = OSPrioHighRdy;

	/* Signal highest priority thread to start by setting its condition variable */
	pthread_cond_signal( &grcvThread[ pFuncInfoNew->nThreadIdx ] );

	/* Wait for this threads condition variable to change ( i.e. the next ctx switch ). */
	pthread_cond_wait( &grcvThread[ pFuncInfoOld->nThreadIdx ], &mutThread );

	/* Variable change automatically locks mutex, so unlock */
	pthread_mutex_unlock( &mutThread );
}

/*
*********************************************************************************************************
*                                           Task Context Switch
*
* Description: Context switch from a task. This just calls the context switch for an interrupt, they are
* identical.
*
* Arguments  : none
*********************************************************************************************************
*/
void OSCtxSw()
{
	OSIntCtxSw();
}

/*
*********************************************************************************************************
*                                           OSTickISR
*
* Description: This function processes time ticks.
*
* Arguments  : none
*********************************************************************************************************
*/
static void OSTickISR(void)
{
	OSIntEnter();
	OSTimeTick();
	OSIntExit();
}

/*
*********************************************************************************************************
*                                           AlarmSigHandler
*
* Description: This function is called when ever a interrupt is delivered.
*
* Arguments  : none of these args are used.
*********************************************************************************************************
*/
//void AlarmSigHandler(int signo, siginfo_t* info, void* uc)
static void AlarmSigHandler( int signum )
{
	OSTickISR();
}


/*
*********************************************************************************************************
*                                           InitTick
*
* Description: This function starts the alarm interrupt at the appropriate frequency. Note: Linux can only
* deliver 10ms resolution on this alarm even though it is set in usecs.
*
* Arguments  : none
*********************************************************************************************************
*/
static void InitTick()
{
   ualarm(1000000/MT_TICKS_PER_SEC, 1000000/MT_TICKS_PER_SEC);
}

/*
*********************************************************************************************************
*                                           ThreadWrapper
*
* Description: 	Every Task is wrapped with this function. When the tasks thread is first created it executes
* 				this wrapper function which stops the thread from executing.
*
* Arguments  :	pTaskInfo	Pointer to a FuncInfo structure. This structure holds the information about
*				the task that should be exectued in this thread.
*********************************************************************************************************
*/
void ThreadWrapper(void* pTaskInfo)
{
	/* Grab thread index */
	int nThreadIdx = (( FuncInfo* )( pTaskInfo ))->nThreadIdx;

	/* Wait until task switching mutex is available */
	pthread_mutex_lock( &mutThread );

	/* Increment the number of task wrappers that have started */
	nNumThreadsStarted++;

	/* Signal if all wrappers have started */
	if( nNumThreadsCreated == nNumThreadsStarted )
		pthread_cond_signal( &cvThreadWrapper );

	/* Wait on this threads condition variable and unlock task switching mutex */
	pthread_cond_wait( &grcvThread[  nThreadIdx  ], &mutThread );
	pthread_mutex_unlock( &mutThread );

	/* Unblock alarm signals. */
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGALRM);
	sigprocmask(SIG_UNBLOCK, &set, 0);

	pF = (( FuncInfo* )( pTaskInfo ))->pFunc;
	( *pF )( (( FuncInfo* )( pTaskInfo ))->pArgs );
}

/*
*********************************************************************************************************
*                                           OSTCBInitHook
*
* Description: Initializes the task by creating a thread for that task. The thread information was stored
* on the stack during. This Linux port requires OSTCBInitHook and must have MT_VERSION >= 204.
*
* Arguments  :	ptcb	pointer to a task control block. Contains all the info for the task.
*********************************************************************************************************
*/
void OSTCBInitHook(MT_TCB *ptcb)
{
#if MT_VERSION >= 204
	/* Grab task info that was previously stored on task stack */
	FuncInfo* pFuncInfo = (FuncInfo*)ptcb->OSTCBStkPtr;

	/* Store index of thread and condition variable on stk for use in ctx switching */
	pFuncInfo->nThreadIdx = nNumThreadsCreated;
	nNumThreadsCreated++;

	/* Initialize thread condition variable and create thread */
	pthread_cond_init(&grcvThread[ pFuncInfo->nThreadIdx ], NULL);
	pthread_create(&threadTask[ pFuncInfo->nThreadIdx ], NULL, (void*)&ThreadWrapper, (void*)(pFuncInfo) );
#endif
}

/*
*********************************************************************************************************
*                                           InitLinuxPort
*
* Description: Creates the signal handler for the alarm interrupt. Blocks all alarm signals, threads will
* also inherit this blocking when they are created. Sets up context switching mutex.
*
* Arguments  :	none
*********************************************************************************************************
*/
static void InitLinuxPort()
{
	//setup signal handlers
	struct sigaction act;
	sigset_t mask;
	sigemptyset(&mask);

	act.sa_sigaction = (void*)AlarmSigHandler;
	act.sa_flags = 0;
	act.sa_mask = mask;
	if ( sigaction(SIGALRM, &act, NULL) )
		printf("Sigaction failed for SIGALRM\n" );

	//Block all signals in this the main thread. It should not call any signal handler
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGALRM);
	sigprocmask(SIG_BLOCK, &set,  0);

	// Setup context switching mutex
	pthread_mutex_init(&mutThread, NULL);
	pthread_cond_init (&cvThreadWrapper, NULL);
}
