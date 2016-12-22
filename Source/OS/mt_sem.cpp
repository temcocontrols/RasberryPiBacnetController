

#define  MT_SOURCE

#ifndef  MT_MASTER_FILE
#include "mt.h"
#endif

#if MT_SEM_EN > 0u
/*$PAGE*/
/*
*********************************************************************************************************
*                                          ACCEPT SEMAPHORE
*
* Description: This function checks the semaphore to see if a resource is available or, if an event
*              occurred.  Unlike OSSemPend(), OSSemAccept() does not suspend the calling task if the
*              resource is not available or the event did not occur.
*
* Arguments  : pevent     is a pointer to the event control block
*
* Returns    : >  0       if the resource is available or the event did not occur the semaphore is
*                         decremented to obtain the resource.
*              == 0       if the resource is not available or the event did not occur or,
*                         if 'pevent' is a NULL pointer or,
*                         if you didn't pass a pointer to a semaphore
*********************************************************************************************************
*/

#if MT_SEM_ACCEPT_EN > 0u
INT16U  OSSemAccept (MT_EVENT *pevent)
{
    INT16U     cnt;
#if MT_CRITICAL_METHOD == 3u                          /* Allocate storage for CPU status register      */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#if MT_ARG_CHK_EN > 0u
    if (pevent == (MT_EVENT *)0) {                    /* Validate 'pevent'                             */
        return (0u);
    }
#endif
    if (pevent->OSEventType != MT_EVENT_TYPE_SEM) {   /* Validate event block type                     */
        return (0u);
    }
    MT_ENTER_CRITICAL();
    cnt = pevent->OSEventCnt;
    if (cnt > 0u) {                                   /* See if resource is available                  */
        pevent->OSEventCnt--;                         /* Yes, decrement semaphore and notify caller    */
    }
    MT_EXIT_CRITICAL();
    return (cnt);                                     /* Return semaphore count                        */
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                         CREATE A SEMAPHORE
*
* Description: This function creates a semaphore.
*
* Arguments  : cnt           is the initial value for the semaphore.  If the value is 0, no resource is
*                            available (or no event has occurred).  You initialize the semaphore to a
*                            non-zero value to specify how many resources are available (e.g. if you have
*                            10 resources, you would initialize the semaphore to 10).
*
* Returns    : != (void *)0  is a pointer to the event control block (MT_EVENT) associated with the
*                            created semaphore
*              == (void *)0  if no event control blocks were available
*********************************************************************************************************
*/

MT_EVENT  *OSSemCreate (INT16U cnt)
{
    MT_EVENT  *pevent;
#if MT_CRITICAL_METHOD == 3u                               /* Allocate storage for CPU status register */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#ifdef MT_SAFETY_CRITICAL_IEC61508
    if (OSSafetyCriticalStartFlag == MT_TRUE) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return ((MT_EVENT *)0);
    }
#endif

    if (OSIntNesting > 0u) {                               /* See if called from ISR ...               */
        return ((MT_EVENT *)0);                            /* ... can't CREATE from an ISR             */
    }
    MT_ENTER_CRITICAL();
    pevent = OSEventFreeList;                              /* Get next free event control block        */
    if (OSEventFreeList != (MT_EVENT *)0) {                /* See if pool of free ECB pool was empty   */
        OSEventFreeList = (MT_EVENT *)OSEventFreeList->OSEventPtr;
    }
    MT_EXIT_CRITICAL();
    if (pevent != (MT_EVENT *)0) {                         /* Get an event control block               */
        pevent->OSEventType    = MT_EVENT_TYPE_SEM;
        pevent->OSEventCnt     = cnt;                      /* Set semaphore value                      */
        pevent->OSEventPtr     = (void *)0;                /* Unlink from ECB free list                */
#if MT_EVENT_NAME_EN > 0u
        pevent->OSEventName    = (INT8U *)(void *)"?";
#endif
        MT_EventWaitListInit(pevent);                      /* Initialize to 'nobody waiting' on sem.   */
    }
    return (pevent);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                         DELETE A SEMAPHORE
*
* Description: This function deletes a semaphore and readies all tasks pending on the semaphore.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            semaphore.
*
*              opt           determines delete options as follows:
*                            opt == MT_DEL_NO_PEND   Delete semaphore ONLY if no task pending
*                            opt == MT_DEL_ALWAYS    Deletes the semaphore even if tasks are waiting.
*                                                    In this case, all the tasks pending will be readied.
*
*              perr          is a pointer to an error code that can contain one of the following values:
*                            MT_ERR_NONE             The call was successful and the semaphore was deleted
*                            MT_ERR_DEL_ISR          If you attempted to delete the semaphore from an ISR
*                            MT_ERR_INVALID_OPT      An invalid option was specified
*                            MT_ERR_TASK_WAITING     One or more tasks were waiting on the semaphore
*                            MT_ERR_EVENT_TYPE       If you didn't pass a pointer to a semaphore
*                            MT_ERR_PEVENT_NULL      If 'pevent' is a NULL pointer.
*
* Returns    : pevent        upon error
*              (MT_EVENT *)0 if the semaphore was successfully deleted.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of
*                 the semaphore MUST check the return code of OSSemPend().
*              2) OSSemAccept() callers will not know that the intended semaphore has been deleted unless
*                 they check 'pevent' to see that it's a NULL pointer.
*              3) This call can potentially disable interrupts for a long time.  The interrupt disable
*                 time is directly proportional to the number of tasks waiting on the semaphore.
*              4) Because ALL tasks pending on the semaphore will be readied, you MUST be careful in
*                 applications where the semaphore is used for mutual exclusion because the resource(s)
*                 will no longer be guarded by the semaphore.
*              5) All tasks that were waiting for the semaphore will be readied and returned an 
*                 MT_ERR_PEND_ABORT if OSSemDel() was called with MT_DEL_ALWAYS
*********************************************************************************************************
*/

#if MT_SEM_DEL_EN > 0u
MT_EVENT  *OSSemDel (MT_EVENT  *pevent,
                     INT8U      opt,
                     INT8U     *perr)
{
    BOOLEAN    tasks_waiting;
    MT_EVENT  *pevent_return;
#if MT_CRITICAL_METHOD == 3u                               /* Allocate storage for CPU status register */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#ifdef MT_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return ((MT_EVENT *)0);
    }
#endif

#if MT_ARG_CHK_EN > 0u
    if (pevent == (MT_EVENT *)0) {                         /* Validate 'pevent'                        */
        *perr = MT_ERR_PEVENT_NULL;
        return (pevent);
    }
#endif
    if (pevent->OSEventType != MT_EVENT_TYPE_SEM) {        /* Validate event block type                */
        *perr = MT_ERR_EVENT_TYPE;
        return (pevent);
    }
    if (OSIntNesting > 0u) {                               /* See if called from ISR ...               */
        *perr = MT_ERR_DEL_ISR;                            /* ... can't DELETE from an ISR             */
        return (pevent);
    }
    MT_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0u) {                        /* See if any tasks waiting on semaphore    */
        tasks_waiting = MT_TRUE;                           /* Yes                                      */
    } else {
        tasks_waiting = MT_FALSE;                          /* No                                       */
    }
    switch (opt) {
        case MT_DEL_NO_PEND:                               /* Delete semaphore only if no task waiting */
             if (tasks_waiting == MT_FALSE) {
#if MT_EVENT_NAME_EN > 0u
                 pevent->OSEventName    = (INT8U *)(void *)"?";
#endif
                 pevent->OSEventType    = MT_EVENT_TYPE_UNUSED;
                 pevent->OSEventPtr     = OSEventFreeList; /* Return Event Control Block to free list  */
                 pevent->OSEventCnt     = 0u;
                 OSEventFreeList        = pevent;          /* Get next free event control block        */
                 MT_EXIT_CRITICAL();
                 *perr                  = MT_ERR_NONE;
                 pevent_return          = (MT_EVENT *)0;   /* Semaphore has been deleted               */
             } else {
                 MT_EXIT_CRITICAL();
                 *perr                  = MT_ERR_TASK_WAITING;
                 pevent_return          = pevent;
             }
             break;

        case MT_DEL_ALWAYS:                                /* Always delete the semaphore              */
             while (pevent->OSEventGrp != 0u) {            /* Ready ALL tasks waiting for semaphore    */
                 (void)MT_EventTaskRdy(pevent, (void *)0, MT_STAT_SEM, MT_STAT_PEND_ABORT);
             }
#if MT_EVENT_NAME_EN > 0u
             pevent->OSEventName    = (INT8U *)(void *)"?";
#endif
             pevent->OSEventType    = MT_EVENT_TYPE_UNUSED;
             pevent->OSEventPtr     = OSEventFreeList;     /* Return Event Control Block to free list  */
             pevent->OSEventCnt     = 0u;
             OSEventFreeList        = pevent;              /* Get next free event control block        */
             MT_EXIT_CRITICAL();
             if (tasks_waiting == MT_TRUE) {               /* Reschedule only if task(s) were waiting  */
                 MT_Sched();                               /* Find highest priority task ready to run  */
             }
             *perr                  = MT_ERR_NONE;
             pevent_return          = (MT_EVENT *)0;       /* Semaphore has been deleted               */
             break;

        default:
             MT_EXIT_CRITICAL();
             *perr                  = MT_ERR_INVALID_OPT;
             pevent_return          = pevent;
             break;
    }
    return (pevent_return);
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                          PEND ON SEMAPHORE
*
* Description: This function waits for a semaphore.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            semaphore.
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for the resource up to the amount of time specified by this argument.
*                            If you specify 0, however, your task will wait forever at the specified
*                            semaphore or, until the resource becomes available (or the event occurs).
*
*              perr          is a pointer to where an error message will be deposited.  Possible error
*                            messages are:
*
*                            MT_ERR_NONE         The call was successful and your task owns the resource
*                                                or, the event you are waiting for occurred.
*                            MT_ERR_TIMEOUT      The semaphore was not received within the specified
*                                                'timeout'.
*                            MT_ERR_PEND_ABORT   The wait on the semaphore was aborted.
*                            MT_ERR_EVENT_TYPE   If you didn't pass a pointer to a semaphore.
*                            MT_ERR_PEND_ISR     If you called this function from an ISR and the result
*                                                would lead to a suspension.
*                            MT_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer.
*                            MT_ERR_PEND_LOCKED  If you called this function when the scheduler is locked
*
* Returns    : none
*********************************************************************************************************
*/
/*$PAGE*/
void  OSSemPend (MT_EVENT  *pevent,
                 INT32U     timeout,
                 INT8U     *perr)
{
#if MT_CRITICAL_METHOD == 3u                          /* Allocate storage for CPU status register      */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#ifdef MT_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if MT_ARG_CHK_EN > 0u
    if (pevent == (MT_EVENT *)0) {                    /* Validate 'pevent'                             */
        *perr = MT_ERR_PEVENT_NULL;
        return;
    }
#endif
    if (pevent->OSEventType != MT_EVENT_TYPE_SEM) {   /* Validate event block type                     */
        *perr = MT_ERR_EVENT_TYPE;
        return;
    }
    if (OSIntNesting > 0u) {                          /* See if called from ISR ...                    */
        *perr = MT_ERR_PEND_ISR;                      /* ... can't PEND from an ISR                    */
        return;
    }
    if (OSLockNesting > 0u) {                         /* See if called with scheduler locked ...       */
        *perr = MT_ERR_PEND_LOCKED;                   /* ... can't PEND when locked                    */
        return;
    }
    MT_ENTER_CRITICAL();
    if (pevent->OSEventCnt > 0u) {                    /* If sem. is positive, resource available ...   */
        pevent->OSEventCnt--;                         /* ... decrement semaphore only if positive.     */
        MT_EXIT_CRITICAL();
        *perr = MT_ERR_NONE;
        return;
    }
                                                      /* Otherwise, must wait until event occurs       */
    OSTCBCur->OSTCBStat     |= MT_STAT_SEM;           /* Resource not available, pend on semaphore     */
    OSTCBCur->OSTCBStatPend  = MT_STAT_PEND_OK;
    OSTCBCur->OSTCBDly       = timeout;               /* Store pend timeout in TCB                     */
    MT_EventTaskWait(pevent);                         /* Suspend task until event or timeout occurs    */
    MT_EXIT_CRITICAL();
    MT_Sched();                                       /* Find next highest priority task ready         */
    MT_ENTER_CRITICAL();
    switch (OSTCBCur->OSTCBStatPend) {                /* See if we timed-out or aborted                */
        case MT_STAT_PEND_OK:
             *perr = MT_ERR_NONE;
             break;

        case MT_STAT_PEND_ABORT:
             *perr = MT_ERR_PEND_ABORT;               /* Indicate that we aborted                      */
             break;

        case MT_STAT_PEND_TO:
        default:
             MT_EventTaskRemove(OSTCBCur, pevent);
             *perr = MT_ERR_TIMEOUT;                  /* Indicate that we didn't get event within TO   */
             break;
    }
    OSTCBCur->OSTCBStat          =  MT_STAT_RDY;      /* Set   task  status to ready                   */
    OSTCBCur->OSTCBStatPend      =  MT_STAT_PEND_OK;  /* Clear pend  status                            */
    OSTCBCur->OSTCBEventPtr      = (MT_EVENT  *)0;    /* Clear event pointers                          */
#if (MT_EVENT_MULTI_EN > 0u)
    OSTCBCur->OSTCBEventMultiPtr = (MT_EVENT **)0;
#endif
    MT_EXIT_CRITICAL();
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                    ABORT WAITING ON A SEMAPHORE
*
* Description: This function aborts & readies any tasks currently waiting on a semaphore.  This function
*              should be used to fault-abort the wait on the semaphore, rather than to normally signal
*              the semaphore via OSSemPost().
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            semaphore.
*
*              opt           determines the type of ABORT performed:
*                            MT_PEND_OPT_NONE         ABORT wait for a single task (HPT) waiting on the
*                                                     semaphore
*                            MT_PEND_OPT_BROADCAST    ABORT wait for ALL tasks that are  waiting on the
*                                                     semaphore
*
*              perr          is a pointer to where an error message will be deposited.  Possible error
*                            messages are:
*
*                            MT_ERR_NONE         No tasks were     waiting on the semaphore.
*                            MT_ERR_PEND_ABORT   At least one task waiting on the semaphore was readied
*                                                and informed of the aborted wait; check return value
*                                                for the number of tasks whose wait on the semaphore
*                                                was aborted.
*                            MT_ERR_EVENT_TYPE   If you didn't pass a pointer to a semaphore.
*                            MT_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer.
*
* Returns    : == 0          if no tasks were waiting on the semaphore, or upon error.
*              >  0          if one or more tasks waiting on the semaphore are now readied and informed.
*********************************************************************************************************
*/

#if MT_SEM_PEND_ABORT_EN > 0u
INT8U  OSSemPendAbort (MT_EVENT  *pevent,
                       INT8U      opt,
                       INT8U     *perr)
{
    INT8U      nbr_tasks;
#if MT_CRITICAL_METHOD == 3u                          /* Allocate storage for CPU status register      */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#ifdef MT_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return (0u);
    }
#endif

#if MT_ARG_CHK_EN > 0u
    if (pevent == (MT_EVENT *)0) {                    /* Validate 'pevent'                             */
        *perr = MT_ERR_PEVENT_NULL;
        return (0u);
    }
#endif
    if (pevent->OSEventType != MT_EVENT_TYPE_SEM) {   /* Validate event block type                     */
        *perr = MT_ERR_EVENT_TYPE;
        return (0u);
    }
    MT_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0u) {                   /* See if any task waiting on semaphore?         */
        nbr_tasks = 0u;
        switch (opt) {
            case MT_PEND_OPT_BROADCAST:               /* Do we need to abort ALL waiting tasks?        */
                 while (pevent->OSEventGrp != 0u) {   /* Yes, ready ALL tasks waiting on semaphore     */
                     (void)MT_EventTaskRdy(pevent, (void *)0, MT_STAT_SEM, MT_STAT_PEND_ABORT);
                     nbr_tasks++;
                 }
                 break;

            case MT_PEND_OPT_NONE:
            default:                                  /* No,  ready HPT       waiting on semaphore     */
                 (void)MT_EventTaskRdy(pevent, (void *)0, MT_STAT_SEM, MT_STAT_PEND_ABORT);
                 nbr_tasks++;
                 break;
        }
        MT_EXIT_CRITICAL();
        MT_Sched();                                   /* Find HPT ready to run                         */
        *perr = MT_ERR_PEND_ABORT;
        return (nbr_tasks);
    }
    MT_EXIT_CRITICAL();
    *perr = MT_ERR_NONE;
    return (0u);                                      /* No tasks waiting on semaphore                 */
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                         POST TO A SEMAPHORE
*
* Description: This function signals a semaphore
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            semaphore.
*
* Returns    : MT_ERR_NONE         The call was successful and the semaphore was signaled.
*              MT_ERR_SEM_OVF      If the semaphore count exceeded its limit. In other words, you have
*                                  signaled the semaphore more often than you waited on it with either
*                                  OSSemAccept() or OSSemPend().
*              MT_ERR_EVENT_TYPE   If you didn't pass a pointer to a semaphore
*              MT_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer.
*********************************************************************************************************
*/

INT8U  OSSemPost (MT_EVENT *pevent)
{
#if MT_CRITICAL_METHOD == 3u                          /* Allocate storage for CPU status register      */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#if MT_ARG_CHK_EN > 0u
    if (pevent == (MT_EVENT *)0) {                    /* Validate 'pevent'                             */
        return (MT_ERR_PEVENT_NULL);
    }
#endif
    if (pevent->OSEventType != MT_EVENT_TYPE_SEM) {   /* Validate event block type                     */
        return (MT_ERR_EVENT_TYPE);
    }
    MT_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0u) {                   /* See if any task waiting for semaphore         */
                                                      /* Ready HPT waiting on event                    */
        (void)MT_EventTaskRdy(pevent, (void *)0, MT_STAT_SEM, MT_STAT_PEND_OK);
        MT_EXIT_CRITICAL();
        MT_Sched();                                   /* Find HPT ready to run                         */
        return (MT_ERR_NONE);
    }
    if (pevent->OSEventCnt < 65535u) {                /* Make sure semaphore will not overflow         */
        pevent->OSEventCnt++;                         /* Increment semaphore count to register event   */
        MT_EXIT_CRITICAL();
        return (MT_ERR_NONE);
    }
    MT_EXIT_CRITICAL();                               /* Semaphore value has reached its maximum       */
    return (MT_ERR_SEM_OVF);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                          QUERY A SEMAPHORE
*
* Description: This function obtains information about a semaphore
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            semaphore
*
*              p_sem_data    is a pointer to a structure that will contain information about the
*                            semaphore.
*
* Returns    : MT_ERR_NONE         The call was successful and the message was sent
*              MT_ERR_EVENT_TYPE   If you are attempting to obtain data from a non semaphore.
*              MT_ERR_PEVENT_NULL  If 'pevent'     is a NULL pointer.
*              MT_ERR_PDATA_NULL   If 'p_sem_data' is a NULL pointer
*********************************************************************************************************
*/

#if MT_SEM_QUERY_EN > 0u
INT8U  OSSemQuery (MT_EVENT     *pevent,
                   MT_SEM_DATA  *p_sem_data)
{
    INT8U       i;
    MT_PRIO    *psrc;
    MT_PRIO    *pdest;
#if MT_CRITICAL_METHOD == 3u                               /* Allocate storage for CPU status register */
    MT_CPU_SR   cpu_sr = 0u;
#endif



#if MT_ARG_CHK_EN > 0u
    if (pevent == (MT_EVENT *)0) {                         /* Validate 'pevent'                        */
        return (MT_ERR_PEVENT_NULL);
    }
    if (p_sem_data == (MT_SEM_DATA *)0) {                  /* Validate 'p_sem_data'                    */
        return (MT_ERR_PDATA_NULL);
    }
#endif
    if (pevent->OSEventType != MT_EVENT_TYPE_SEM) {        /* Validate event block type                */
        return (MT_ERR_EVENT_TYPE);
    }
    MT_ENTER_CRITICAL();
    p_sem_data->OSEventGrp = pevent->OSEventGrp;           /* Copy message mailbox wait list           */
    psrc                   = &pevent->OSEventTbl[0];
    pdest                  = &p_sem_data->OSEventTbl[0];
    for (i = 0u; i < MT_EVENT_TBL_SIZE; i++) {
        *pdest++ = *psrc++;
    }
    p_sem_data->OSCnt = pevent->OSEventCnt;                /* Get semaphore count                      */
    MT_EXIT_CRITICAL();
    return (MT_ERR_NONE);
}
#endif                                                     /* MT_SEM_QUERY_EN                          */

/*$PAGE*/
/*
*********************************************************************************************************
*                                            SET SEMAPHORE
*
* Description: This function sets the semaphore count to the value specified as an argument.  Typically,
*              this value would be 0.
*
*              You would typically use this function when a semaphore is used as a signaling mechanism
*              and, you want to reset the count value.
*
* Arguments  : pevent     is a pointer to the event control block
*
*              cnt        is the new value for the semaphore count.  You would pass 0 to reset the
*                         semaphore count.
*
*              perr       is a pointer to an error code returned by the function as follows:
*
*                            MT_ERR_NONE          The call was successful and the semaphore value was set.
*                            MT_ERR_EVENT_TYPE    If you didn't pass a pointer to a semaphore.
*                            MT_ERR_PEVENT_NULL   If 'pevent' is a NULL pointer.
*                            MT_ERR_TASK_WAITING  If tasks are waiting on the semaphore.
*********************************************************************************************************
*/

#if MT_SEM_SET_EN > 0u
void  OSSemSet (MT_EVENT  *pevent,
                INT16U     cnt,
                INT8U     *perr)
{
#if MT_CRITICAL_METHOD == 3u                          /* Allocate storage for CPU status register      */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#ifdef MT_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if MT_ARG_CHK_EN > 0u
    if (pevent == (MT_EVENT *)0) {                    /* Validate 'pevent'                             */
        *perr = MT_ERR_PEVENT_NULL;
        return;
    }
#endif
    if (pevent->OSEventType != MT_EVENT_TYPE_SEM) {   /* Validate event block type                     */
        *perr = MT_ERR_EVENT_TYPE;
        return;
    }
    MT_ENTER_CRITICAL();
    *perr = MT_ERR_NONE;
    if (pevent->OSEventCnt > 0u) {                    /* See if semaphore already has a count          */
        pevent->OSEventCnt = cnt;                     /* Yes, set it to the new value specified.       */
    } else {                                          /* No                                            */
        if (pevent->OSEventGrp == 0u) {               /*      See if task(s) waiting?                  */
            pevent->OSEventCnt = cnt;                 /*      No, OK to set the value                  */
        } else {
            *perr              = MT_ERR_TASK_WAITING;
        }
    }
    MT_EXIT_CRITICAL();
}
#endif

#endif                                                /* MT_SEM_EN                                     */
