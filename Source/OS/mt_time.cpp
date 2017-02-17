

#define  MT_SOURCE

#ifndef  MT_MASTER_FILE
#include "mt.h"
#endif

/*
*********************************************************************************************************
*                                        DELAY TASK 'n' TICKS
*
* Description: This function is called to delay execution of the currently running task until the
*              specified number of system ticks expires.  This, of course, directly equates to delaying
*              the current task for some time to expire.  No delay will result If the specified delay is
*              0.  If the specified delay is greater than 0 then, a context switch will result.
*
* Arguments  : ticks     is the time delay that the task will be suspended in number of clock 'ticks'.
*                        Note that by specifying 0, the task will not be delayed.
*
* Returns    : none
*********************************************************************************************************
*/

void  OSTimeDly (INT32U ticks)
{
    INT8U      y;
#if MT_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    MT_CPU_SR  cpu_sr = 0u;
#endif



    if (OSIntNesting > 0u) {                     /* See if trying to call from an ISR                  */
        return;
    }
    if (OSLockNesting > 0u) {                    /* See if called with scheduler locked                */
        return;
    }
    if (ticks > 0u) {                            /* 0 means no delay!                                  */
        MT_ENTER_CRITICAL();
        y            =  OSTCBCur->OSTCBY;        /* Delay current task                                 */
        OSRdyTbl[y] &= (MT_PRIO)~OSTCBCur->OSTCBBitX;
        if (OSRdyTbl[y] == 0u) {
            OSRdyGrp &= (MT_PRIO)~OSTCBCur->OSTCBBitY;
        }
        OSTCBCur->OSTCBDly = ticks;              /* Load ticks in TCB                                  */
        MT_EXIT_CRITICAL();
        MT_Sched();                              /* Find next task to run!                             */
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        DELAY SPECIFIC TASK 'n' TICKS
*
* Description: This function is called to delay execution of the specified task until the
*              specified number of system ticks expires.  This, of course, directly equates to delaying
*              the specified task for some time to expire.  No delay will result If the specified delay is
*              0.  If the specified delay is greater than 0 then, a context switch will result.
*
* Arguments  : ticks     is the time delay that the task will be suspended in number of clock 'ticks'.
*                        Note that by specifying 0, the task will not be delayed.
*
* Returns    : none
*********************************************************************************************************
*/

void  OSTimeDlyTask (INT8U prio, INT32U ticks)
{
    INT8U      y;
#if MT_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    MT_CPU_SR  cpu_sr = 0u;
#endif



    if (OSIntNesting > 0u) {                     /* See if trying to call from an ISR                  */
        return;
    }
    if (OSLockNesting > 0u) {                    /* See if called with scheduler locked                */
        return;
    }
    if (ticks > 0u) {                            /* 0 means no delay!                                  */
        MT_ENTER_CRITICAL();
        y            =  OSTCBPrioTbl[OSPrioHighRdy]->OSTCBY;        /* Delay current task                                 */
        OSRdyTbl[y] &= (MT_PRIO)~OSTCBPrioTbl[OSPrioHighRdy]->OSTCBBitX;
        if (OSRdyTbl[y] == 0u) {
            OSRdyGrp &= (MT_PRIO)~OSTCBPrioTbl[OSPrioHighRdy]->OSTCBBitY;
        }
        OSTCBPrioTbl[OSPrioHighRdy]->OSTCBDly = ticks;              /* Load ticks in TCB                                  */
        MT_EXIT_CRITICAL();
        MT_Sched();                              /* Find next task to run!                             */
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                    DELAY TASK FOR SPECIFIED TIME
*
* Description: This function is called to delay execution of the currently running task until some time
*              expires.  This call allows you to specify the delay time in HOURS, MINUTES, SECONDS and
*              MILLISECONDS instead of ticks.
*
* Arguments  : hours     specifies the number of hours that the task will be delayed (max. is 255)
*              minutes   specifies the number of minutes (max. 59)
*              seconds   specifies the number of seconds (max. 59)
*              ms        specifies the number of milliseconds (max. 999)
*
* Returns    : MT_ERR_NONE
*              MT_ERR_TIME_INVALID_MINUTES
*              MT_ERR_TIME_INVALID_SECONDS
*              MT_ERR_TIME_INVALID_MS
*              MT_ERR_TIME_ZERO_DLY
*              MT_ERR_TIME_DLY_ISR
*
* Note(s)    : The resolution on the milliseconds depends on the tick rate.  For example, you can't do
*              a 10 mS delay if the ticker interrupts every 100 mS.  In this case, the delay would be
*              set to 0.  The actual delay is rounded to the nearest tick.
*********************************************************************************************************
*/

#if MT_TIME_DLY_HMSM_EN > 0u
INT8U  OSTimeDlyHMSM (INT8U   hours,
                      INT8U   minutes,
                      INT8U   seconds,
                      INT16U  ms)
{
    INT32U ticks;


    if (OSIntNesting > 0u) {                     /* See if trying to call from an ISR                  */
        return (MT_ERR_TIME_DLY_ISR);
    }
    if (OSLockNesting > 0u) {                    /* See if called with scheduler locked                */
        return (MT_ERR_SCHED_LOCKED);
    }
#if MT_ARG_CHK_EN > 0u
    if (hours == 0u) {
        if (minutes == 0u) {
            if (seconds == 0u) {
                if (ms == 0u) {
                    return (MT_ERR_TIME_ZERO_DLY);
                }
            }
        }
    }
    if (minutes > 59u) {
        return (MT_ERR_TIME_INVALID_MINUTES);    /* Validate arguments to be within range              */
    }
    if (seconds > 59u) {
        return (MT_ERR_TIME_INVALID_SECONDS);
    }
    if (ms > 999u) {
        return (MT_ERR_TIME_INVALID_MS);
    }
#endif
                                                 /* Compute the total number of clock ticks required.. */
                                                 /* .. (rounded to the nearest tick)                   */
    ticks = ((INT32U)hours * 3600uL + (INT32U)minutes * 60uL + (INT32U)seconds) * MT_TICKS_PER_SEC
          + MT_TICKS_PER_SEC * ((INT32U)ms + 500uL / MT_TICKS_PER_SEC) / 1000uL;
    OSTimeDly(ticks);
    return (MT_ERR_NONE);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                        RESUME A DELAYED TASK
*
* Description: This function is used resume a task that has been delayed through a call to either
*              OSTimeDly() or OSTimeDlyHMSM().  Note that you can call this function to resume a
*              task that is waiting for an event with timeout.  This would make the task look
*              like a timeout occurred.
*
* Arguments  : prio                      specifies the priority of the task to resume
*
* Returns    : MT_ERR_NONE               Task has been resumed
*              MT_ERR_PRIO_INVALID       if the priority you specify is higher that the maximum allowed
*                                        (i.e. >= MT_LOWEST_PRIO)
*              MT_ERR_TIME_NOT_DLY       Task is not waiting for time to expire
*              MT_ERR_TASK_NOT_EXIST     The desired task has not been created or has been assigned to a Mutex.
*********************************************************************************************************
*/

#if MT_TIME_DLY_RESUME_EN > 0u
INT8U  OSTimeDlyResume (INT8U prio)
{
    MT_TCB    *ptcb;
#if MT_CRITICAL_METHOD == 3u                                   /* Storage for CPU status register      */
    MT_CPU_SR  cpu_sr = 0u;
#endif



    if (prio >= MT_LOWEST_PRIO) {
        return (MT_ERR_PRIO_INVALID);
    }
    MT_ENTER_CRITICAL();
    ptcb = OSTCBPrioTbl[prio];                                 /* Make sure that task exist            */
    if (ptcb == (MT_TCB *)0) {
        MT_EXIT_CRITICAL();
        return (MT_ERR_TASK_NOT_EXIST);                        /* The task does not exist              */
    }
    if (ptcb == MT_TCB_RESERVED) {
        MT_EXIT_CRITICAL();
        return (MT_ERR_TASK_NOT_EXIST);                        /* The task does not exist              */
    }
    if (ptcb->OSTCBDly == 0u) {                                /* See if task is delayed               */
        MT_EXIT_CRITICAL();
        return (MT_ERR_TIME_NOT_DLY);                          /* Indicate that task was not delayed   */
    }

    ptcb->OSTCBDly = 0u;                                       /* Clear the time delay                 */
    if ((ptcb->OSTCBStat & MT_STAT_PEND_ANY) != MT_STAT_RDY) {
        ptcb->OSTCBStat     &= ~MT_STAT_PEND_ANY;              /* Yes, Clear status flag               */
        ptcb->OSTCBStatPend  =  MT_STAT_PEND_TO;               /* Indicate PEND timeout                */
    } else {
        ptcb->OSTCBStatPend  =  MT_STAT_PEND_OK;
    }
    if ((ptcb->OSTCBStat & MT_STAT_SUSPEND) == MT_STAT_RDY) {  /* Is task suspended?                   */
        OSRdyGrp               |= ptcb->OSTCBBitY;             /* No,  Make ready                      */
        OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
        MT_EXIT_CRITICAL();
        MT_Sched();                                            /* See if this is new highest priority  */
    } else {
        MT_EXIT_CRITICAL();                                    /* Task may be suspended                */
    }
    return (MT_ERR_NONE);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                       GET CURRENT SYSTEM TIME
*
* Description: This function is used by your application to obtain the current value of the 32-bit
*              counter which keeps track of the number of clock ticks.
*
* Arguments  : none
*
* Returns    : The current value of OSTime
*********************************************************************************************************
*/

#if MT_TIME_GET_SET_EN > 0u
INT32U  OSTimeGet (void)
{
    INT32U     ticks;
#if MT_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    MT_CPU_SR  cpu_sr = 0u;
#endif



    MT_ENTER_CRITICAL();
    ticks = OSTime;
    MT_EXIT_CRITICAL();
    return (ticks);
}
#endif

/*
*********************************************************************************************************
*                                          SET SYSTEM CLOCK
*
* Description: This function sets the 32-bit counter which keeps track of the number of clock ticks.
*
* Arguments  : ticks      specifies the new value that OSTime needs to take.
*
* Returns    : none
*********************************************************************************************************
*/

#if MT_TIME_GET_SET_EN > 0u
void  OSTimeSet (INT32U ticks)
{
#if MT_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    MT_CPU_SR  cpu_sr = 0u;
#endif



    MT_ENTER_CRITICAL();
    OSTime = ticks;
    MT_EXIT_CRITICAL();
}
#endif
