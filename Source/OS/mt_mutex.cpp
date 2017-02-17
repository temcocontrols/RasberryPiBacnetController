

#define  MT_SOURCE

#ifndef  MT_MASTER_FILE
#include "mt.h"
#endif


#if MT_MUTEX_EN > 0u
/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/

#define  MT_MUTEX_KEEP_LOWER_8   ((INT16U)0x00FFu)
#define  MT_MUTEX_KEEP_UPPER_8   ((INT16U)0xFF00u)

#define  MT_MUTEX_AVAILABLE      ((INT16U)0x00FFu)

/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/

static  void  OSMutex_RdyAtPrio(MT_TCB *ptcb, INT8U prio);

/*$PAGE*/
/*
*********************************************************************************************************
*                                  ACCEPT MUTUAL EXCLUSION SEMAPHORE
*
* Description: This  function checks the mutual exclusion semaphore to see if a resource is available.
*              Unlike OSMutexPend(), OSMutexAccept() does not suspend the calling task if the resource is
*              not available or the event did not occur.
*
* Arguments  : pevent     is a pointer to the event control block
*
*              perr       is a pointer to an error code which will be returned to your application:
*                            MT_ERR_NONE         if the call was successful.
*                            MT_ERR_EVENT_TYPE   if 'pevent' is not a pointer to a mutex
*                            MT_ERR_PEVENT_NULL  'pevent' is a NULL pointer
*                            MT_ERR_PEND_ISR     if you called this function from an ISR
*                            MT_ERR_PCP_LOWER    If the priority of the task that owns the Mutex is
*                                                HIGHER (i.e. a lower number) than the PCP.  This error
*                                                indicates that you did not set the PCP higher (lower
*                                                number) than ALL the tasks that compete for the Mutex.
*                                                Unfortunately, this is something that could not be
*                                                detected when the Mutex is created because we don't know
*                                                what tasks will be using the Mutex.
*
* Returns    : == MT_TRUE    if the resource is available, the mutual exclusion semaphore is acquired
*              == MT_FALSE   a) if the resource is not available
*                            b) you didn't pass a pointer to a mutual exclusion semaphore
*                            c) you called this function from an ISR
*
* Warning(s) : This function CANNOT be called from an ISR because mutual exclusion semaphores are
*              intended to be used by tasks only.
*********************************************************************************************************
*/

#if MT_MUTEX_ACCEPT_EN > 0u
BOOLEAN  OSMutexAccept (MT_EVENT  *pevent,
                        INT8U     *perr)
{
    INT8U      pcp;                                    /* Priority Ceiling Priority (PCP)              */
#if MT_CRITICAL_METHOD == 3u                           /* Allocate storage for CPU status register     */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#ifdef MT_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return (MT_FALSE);
    }
#endif

#if MT_ARG_CHK_EN > 0u
    if (pevent == (MT_EVENT *)0) {                     /* Validate 'pevent'                            */
        *perr = MT_ERR_PEVENT_NULL;
        return (MT_FALSE);
    }
#endif
    if (pevent->OSEventType != MT_EVENT_TYPE_MUTEX) {  /* Validate event block type                    */
        *perr = MT_ERR_EVENT_TYPE;
        return (MT_FALSE);
    }
    if (OSIntNesting > 0u) {                           /* Make sure it's not called from an ISR        */
        *perr = MT_ERR_PEND_ISR;
        return (MT_FALSE);
    }
    MT_ENTER_CRITICAL();                               /* Get value (0 or 1) of Mutex                  */
    pcp = (INT8U)(pevent->OSEventCnt >> 8u);           /* Get PCP from mutex                           */
    if ((pevent->OSEventCnt & MT_MUTEX_KEEP_LOWER_8) == MT_MUTEX_AVAILABLE) {
        pevent->OSEventCnt &= MT_MUTEX_KEEP_UPPER_8;   /*      Mask off LSByte (Acquire Mutex)         */
        pevent->OSEventCnt |= OSTCBCur->OSTCBPrio;     /*      Save current task priority in LSByte    */
        pevent->OSEventPtr  = (void *)OSTCBCur;        /*      Link TCB of task owning Mutex           */
        if ((pcp != MT_PRIO_MUTEX_CEIL_DIS) &&
            (OSTCBCur->OSTCBPrio <= pcp)) {            /*      PCP 'must' have a SMALLER prio ...      */
             MT_EXIT_CRITICAL();                       /*      ... than current task!                  */
            *perr = MT_ERR_PCP_LOWER;
        } else {
             MT_EXIT_CRITICAL();
            *perr = MT_ERR_NONE;
        }
        return (MT_TRUE);
    }
    MT_EXIT_CRITICAL();
    *perr = MT_ERR_NONE;
    return (MT_FALSE);
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                 CREATE A MUTUAL EXCLUSION SEMAPHORE
*
* Description: This function creates a mutual exclusion semaphore.
*
* Arguments  : prio          is the priority to use when accessing the mutual exclusion semaphore.  In
*                            other words, when the semaphore is acquired and a higher priority task
*                            attempts to obtain the semaphore then the priority of the task owning the
*                            semaphore is raised to this priority.  It is assumed that you will specify
*                            a priority that is LOWER in value than ANY of the tasks competing for the
*                            mutex. If the priority is specified as MT_PRIO_MUTEX_CEIL_DIS, then the
*                            priority ceiling promotion is disabled. This way, the tasks accessing the
*                            semaphore do not have their priority promoted.
*
*              perr          is a pointer to an error code which will be returned to your application:
*                               MT_ERR_NONE         if the call was successful.
*                               MT_ERR_CREATE_ISR   if you attempted to create a MUTEX from an ISR
*                               MT_ERR_PRIO_EXIST   if a task at the priority ceiling priority
*                                                   already exist.
*                               MT_ERR_PEVENT_NULL  No more event control blocks available.
*                               MT_ERR_PRIO_INVALID if the priority you specify is higher that the
*                                                   maximum allowed (i.e. > MT_LOWEST_PRIO)
*
* Returns    : != (void *)0  is a pointer to the event control clock (MT_EVENT) associated with the
*                            created mutex.
*              == (void *)0  if an error is detected.
*
* Note(s)    : 1) The LEAST significant 8 bits of '.OSEventCnt' hold the priority number of the task
*                 owning the mutex or 0xFF if no task owns the mutex.
*
*              2) The MOST  significant 8 bits of '.OSEventCnt' hold the priority number used to
*                 reduce priority inversion or 0xFF (MT_PRIO_MUTEX_CEIL_DIS) if priority ceiling
*                 promotion is disabled.
*********************************************************************************************************
*/

MT_EVENT  *OSMutexCreate (INT8U   prio,
                          INT8U  *perr)
{
    MT_EVENT  *pevent;
#if MT_CRITICAL_METHOD == 3u                               /* Allocate storage for CPU status register */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#ifdef MT_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return ((MT_EVENT *)0);
    }
#endif

#ifdef MT_SAFETY_CRITICAL_IEC61508
    if (OSSafetyCriticalStartFlag == MT_TRUE) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return ((MT_EVENT *)0);
    }
#endif

#if MT_ARG_CHK_EN > 0u
    if (prio != MT_PRIO_MUTEX_CEIL_DIS) {
        if (prio >= MT_LOWEST_PRIO) {                      /* Validate PCP                             */
           *perr = MT_ERR_PRIO_INVALID;
            return ((MT_EVENT *)0);
        }
    }
#endif
    if (OSIntNesting > 0u) {                               /* See if called from ISR ...               */
        *perr = MT_ERR_CREATE_ISR;                         /* ... can't CREATE mutex from an ISR       */
        return ((MT_EVENT *)0);
    }
    MT_ENTER_CRITICAL();
    if (prio != MT_PRIO_MUTEX_CEIL_DIS) {
        if (OSTCBPrioTbl[prio] != (MT_TCB *)0) {           /* Mutex priority must not already exist    */
            MT_EXIT_CRITICAL();                            /* Task already exist at priority ...       */
           *perr = MT_ERR_PRIO_EXIST;                      /* ... ceiling priority                     */
            return ((MT_EVENT *)0);
        }
        OSTCBPrioTbl[prio] = MT_TCB_RESERVED;              /* Reserve the table entry                  */
    }

    pevent = OSEventFreeList;                              /* Get next free event control block        */
    if (pevent == (MT_EVENT *)0) {                         /* See if an ECB was available              */
        if (prio != MT_PRIO_MUTEX_CEIL_DIS) {
            OSTCBPrioTbl[prio] = (MT_TCB *)0;              /* No, Release the table entry              */
        }
        MT_EXIT_CRITICAL();
       *perr = MT_ERR_PEVENT_NULL;                         /* No more event control blocks             */
        return (pevent);
    }
    OSEventFreeList     = (MT_EVENT *)OSEventFreeList->OSEventPtr; /* Adjust the free list             */
    MT_EXIT_CRITICAL();
    pevent->OSEventType = MT_EVENT_TYPE_MUTEX;
    pevent->OSEventCnt  = (INT16U)((INT16U)prio << 8u) | MT_MUTEX_AVAILABLE; /* Resource is avail.     */
    pevent->OSEventPtr  = (void *)0;                       /* No task owning the mutex                 */
#if MT_EVENT_NAME_EN > 0u
    pevent->OSEventName = (INT8U *)(void *)"?";
#endif
    MT_EventWaitListInit(pevent);
   *perr = MT_ERR_NONE;
    return (pevent);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           DELETE A MUTEX
*
* Description: This function deletes a mutual exclusion semaphore and readies all tasks pending on the it.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired mutex.
*
*              opt           determines delete options as follows:
*                            opt == MT_DEL_NO_PEND   Delete mutex ONLY if no task pending
*                            opt == MT_DEL_ALWAYS    Deletes the mutex even if tasks are waiting.
*                                                    In this case, all the tasks pending will be readied.
*
*              perr          is a pointer to an error code that can contain one of the following values:
*                            MT_ERR_NONE             The call was successful and the mutex was deleted
*                            MT_ERR_DEL_ISR          If you attempted to delete the MUTEX from an ISR
*                            MT_ERR_INVALID_OPT      An invalid option was specified
*                            MT_ERR_TASK_WAITING     One or more tasks were waiting on the mutex
*                            MT_ERR_EVENT_TYPE       If you didn't pass a pointer to a mutex
*                            MT_ERR_PEVENT_NULL      If 'pevent' is a NULL pointer.
*
* Returns    : pevent        upon error
*              (MT_EVENT *)0 if the mutex was successfully deleted.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of
*                 the mutex MUST check the return code of OSMutexPend().
*
*              2) This call can potentially disable interrupts for a long time.  The interrupt disable
*                 time is directly proportional to the number of tasks waiting on the mutex.
*
*              3) Because ALL tasks pending on the mutex will be readied, you MUST be careful because the
*                 resource(s) will no longer be guarded by the mutex.
*
*              4) IMPORTANT: In the 'MT_DEL_ALWAYS' case, we assume that the owner of the Mutex (if there
*                            is one) is ready-to-run and is thus NOT pending on another kernel object or
*                            has delayed itself.  In other words, if a task owns the mutex being deleted,
*                            that task will be made ready-to-run at its original priority.
*********************************************************************************************************
*/

#if MT_MUTEX_DEL_EN > 0u
MT_EVENT  *OSMutexDel (MT_EVENT  *pevent,
                       INT8U      opt,
                       INT8U     *perr)
{
    BOOLEAN    tasks_waiting;
    MT_EVENT  *pevent_return;
    INT8U      pcp;                                        /* Priority ceiling priority                */
    INT8U      prio;
    MT_TCB    *ptcb;
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
    if (pevent->OSEventType != MT_EVENT_TYPE_MUTEX) {      /* Validate event block type                */
        *perr = MT_ERR_EVENT_TYPE;
        return (pevent);
    }
    if (OSIntNesting > 0u) {                               /* See if called from ISR ...               */
        *perr = MT_ERR_DEL_ISR;                             /* ... can't DELETE from an ISR             */
        return (pevent);
    }
    MT_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0u) {                        /* See if any tasks waiting on mutex        */
        tasks_waiting = MT_TRUE;                           /* Yes                                      */
    } else {
        tasks_waiting = MT_FALSE;                          /* No                                       */
    }
    switch (opt) {
        case MT_DEL_NO_PEND:                               /* DELETE MUTEX ONLY IF NO TASK WAITING --- */
             if (tasks_waiting == MT_FALSE) {
#if MT_EVENT_NAME_EN > 0u
                 pevent->OSEventName   = (INT8U *)(void *)"?";
#endif
                 pcp                   = (INT8U)(pevent->OSEventCnt >> 8u);
                 if (pcp != MT_PRIO_MUTEX_CEIL_DIS) {
                     OSTCBPrioTbl[pcp] = (MT_TCB *)0;      /* Free up the PCP                          */
                 }
                 pevent->OSEventType   = MT_EVENT_TYPE_UNUSED;
                 pevent->OSEventPtr    = OSEventFreeList;  /* Return Event Control Block to free list  */
                 pevent->OSEventCnt    = 0u;
                 OSEventFreeList       = pevent;
                 MT_EXIT_CRITICAL();
                 *perr                 = MT_ERR_NONE;
                 pevent_return         = (MT_EVENT *)0;    /* Mutex has been deleted                   */
             } else {
                 MT_EXIT_CRITICAL();
                 *perr                 = MT_ERR_TASK_WAITING;
                 pevent_return         = pevent;
             }
             break;

        case MT_DEL_ALWAYS:                                /* ALWAYS DELETE THE MUTEX ---------------- */
             pcp  = (INT8U)(pevent->OSEventCnt >> 8u);                       /* Get PCP of mutex       */
             if (pcp != MT_PRIO_MUTEX_CEIL_DIS) {
                 prio = (INT8U)(pevent->OSEventCnt & MT_MUTEX_KEEP_LOWER_8); /* Get owner's orig prio  */
                 ptcb = (MT_TCB *)pevent->OSEventPtr;
                 if (ptcb != (MT_TCB *)0) {                /* See if any task owns the mutex           */
                     if (ptcb->OSTCBPrio == pcp) {         /* See if original prio was changed         */
                         OSMutex_RdyAtPrio(ptcb, prio);    /* Yes, Restore the task's original prio    */
                     }
                 }
             }
             while (pevent->OSEventGrp != 0u) {            /* Ready ALL tasks waiting for mutex        */
                 (void)MT_EventTaskRdy(pevent, (void *)0, MT_STAT_MUTEX, MT_STAT_PEND_ABORT);
             }
#if MT_EVENT_NAME_EN > 0u
             pevent->OSEventName   = (INT8U *)(void *)"?";
#endif
             pcp                   = (INT8U)(pevent->OSEventCnt >> 8u);
             if (pcp != MT_PRIO_MUTEX_CEIL_DIS) {
                 OSTCBPrioTbl[pcp] = (MT_TCB *)0;          /* Free up the PCP                          */
             }
             pevent->OSEventType   = MT_EVENT_TYPE_UNUSED;
             pevent->OSEventPtr    = OSEventFreeList;      /* Return Event Control Block to free list  */
             pevent->OSEventCnt    = 0u;
             OSEventFreeList       = pevent;               /* Get next free event control block        */
             MT_EXIT_CRITICAL();
             if (tasks_waiting == MT_TRUE) {               /* Reschedule only if task(s) were waiting  */
                 MT_Sched();                               /* Find highest priority task ready to run  */
             }
             *perr         = MT_ERR_NONE;
             pevent_return = (MT_EVENT *)0;                /* Mutex has been deleted                   */
             break;

        default:
             MT_EXIT_CRITICAL();
             *perr         = MT_ERR_INVALID_OPT;
             pevent_return = pevent;
             break;
    }
    return (pevent_return);
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                 PEND ON MUTUAL EXCLUSION SEMAPHORE
*
* Description: This function waits for a mutual exclusion semaphore.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            mutex.
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for the resource up to the amount of time specified by this argument.
*                            If you specify 0, however, your task will wait forever at the specified
*                            mutex or, until the resource becomes available.
*
*              perr          is a pointer to where an error message will be deposited.  Possible error
*                            messages are:
*                               MT_ERR_NONE        The call was successful and your task owns the mutex
*                               MT_ERR_TIMEOUT     The mutex was not available within the specified 'timeout'.
*                               MT_ERR_PEND_ABORT  The wait on the mutex was aborted.
*                               MT_ERR_EVENT_TYPE  If you didn't pass a pointer to a mutex
*                               MT_ERR_PEVENT_NULL 'pevent' is a NULL pointer
*                               MT_ERR_PEND_ISR    If you called this function from an ISR and the result
*                                                  would lead to a suspension.
*                               MT_ERR_PCP_LOWER   If the priority of the task that owns the Mutex is
*                                                  HIGHER (i.e. a lower number) than the PCP.  This error
*                                                  indicates that you did not set the PCP higher (lower
*                                                  number) than ALL the tasks that compete for the Mutex.
*                                                  Unfortunately, this is something that could not be
*                                                  detected when the Mutex is created because we don't know
*                                                  what tasks will be using the Mutex.
*                               MT_ERR_PEND_LOCKED If you called this function when the scheduler is locked
*
* Returns    : none
*
* Note(s)    : 1) The task that owns the Mutex MUST NOT pend on any other event while it owns the mutex.
*
*              2) You MUST NOT change the priority of the task that owns the mutex
*********************************************************************************************************
*/

void  OSMutexPend (MT_EVENT  *pevent,
                   INT32U     timeout,
                   INT8U     *perr)
{
    INT8U      pcp;                                        /* Priority Ceiling Priority (PCP)          */
    INT8U      mprio;                                      /* Mutex owner priority                     */
    BOOLEAN    rdy;                                        /* Flag indicating task was ready           */
    MT_TCB    *ptcb;
    MT_EVENT  *pevent2;
    INT8U      y;
#if MT_CRITICAL_METHOD == 3u                               /* Allocate storage for CPU status register */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#ifdef MT_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if MT_ARG_CHK_EN > 0u
    if (pevent == (MT_EVENT *)0) {                         /* Validate 'pevent'                        */
        *perr = MT_ERR_PEVENT_NULL;
        return;
    }
#endif
    if (pevent->OSEventType != MT_EVENT_TYPE_MUTEX) {      /* Validate event block type                */
        *perr = MT_ERR_EVENT_TYPE;
        return;
    }
    if (OSIntNesting > 0u) {                               /* See if called from ISR ...               */
        *perr = MT_ERR_PEND_ISR;                           /* ... can't PEND from an ISR               */
        return;
    }
    if (OSLockNesting > 0u) {                              /* See if called with scheduler locked ...  */
        *perr = MT_ERR_PEND_LOCKED;                        /* ... can't PEND when locked               */
        return;
    }
/*$PAGE*/
    MT_ENTER_CRITICAL();
    pcp = (INT8U)(pevent->OSEventCnt >> 8u);               /* Get PCP from mutex                       */
                                                           /* Is Mutex available?                      */
    if ((INT8U)(pevent->OSEventCnt & MT_MUTEX_KEEP_LOWER_8) == MT_MUTEX_AVAILABLE) {
        pevent->OSEventCnt &= MT_MUTEX_KEEP_UPPER_8;       /* Yes, Acquire the resource                */
        pevent->OSEventCnt |= OSTCBCur->OSTCBPrio;         /*      Save priority of owning task        */
        pevent->OSEventPtr  = (void *)OSTCBCur;            /*      Point to owning task's MT_TCB       */
        if ((pcp != MT_PRIO_MUTEX_CEIL_DIS) &&
            (OSTCBCur->OSTCBPrio <= pcp)) {                /*      PCP 'must' have a SMALLER prio ...  */
             MT_EXIT_CRITICAL();                           /*      ... than current task!              */
            *perr = MT_ERR_PCP_LOWER;
        } else {
             MT_EXIT_CRITICAL();
            *perr = MT_ERR_NONE;
        }
        return;
    }
    if (pcp != MT_PRIO_MUTEX_CEIL_DIS) {
        mprio = (INT8U)(pevent->OSEventCnt & MT_MUTEX_KEEP_LOWER_8); /*  Get priority of mutex owner   */
        ptcb  = (MT_TCB *)(pevent->OSEventPtr);                   /*     Point to TCB of mutex owner   */
        if (ptcb->OSTCBPrio > pcp) {                              /*     Need to promote prio of owner?*/
            if (mprio > OSTCBCur->OSTCBPrio) {
                y = ptcb->OSTCBY;
                if ((OSRdyTbl[y] & ptcb->OSTCBBitX) != 0u) {      /*     See if mutex owner is ready   */
                    OSRdyTbl[y] &= (MT_PRIO)~ptcb->OSTCBBitX;     /*     Yes, Remove owner from Rdy ...*/
                    if (OSRdyTbl[y] == 0u) {                      /*          ... list at current prio */
                        OSRdyGrp &= (MT_PRIO)~ptcb->OSTCBBitY;
                    }
                    rdy = MT_TRUE;
                } else {
                    pevent2 = ptcb->OSTCBEventPtr;
                    if (pevent2 != (MT_EVENT *)0) {               /* Remove from event wait list       */
                        y = ptcb->OSTCBY;
                        pevent2->OSEventTbl[y] &= (MT_PRIO)~ptcb->OSTCBBitX;
                        if (pevent2->OSEventTbl[y] == 0u) {
                            pevent2->OSEventGrp &= (MT_PRIO)~ptcb->OSTCBBitY;
                        }
                    }
                    rdy = MT_FALSE;                        /* No                                       */
                }
                ptcb->OSTCBPrio = pcp;                     /* Change owner task prio to PCP            */
#if MT_LOWEST_PRIO <= 63u
                ptcb->OSTCBY    = (INT8U)( ptcb->OSTCBPrio >> 3u);
                ptcb->OSTCBX    = (INT8U)( ptcb->OSTCBPrio & 0x07u);
#else
                ptcb->OSTCBY    = (INT8U)((INT8U)(ptcb->OSTCBPrio >> 4u) & 0xFFu);
                ptcb->OSTCBX    = (INT8U)( ptcb->OSTCBPrio & 0x0Fu);
#endif
                ptcb->OSTCBBitY = (MT_PRIO)(1uL << ptcb->OSTCBY);
                ptcb->OSTCBBitX = (MT_PRIO)(1uL << ptcb->OSTCBX);

                if (rdy == MT_TRUE) {                      /* If task was ready at owner's priority ...*/
                    OSRdyGrp               |= ptcb->OSTCBBitY; /* ... make it ready at new priority.   */
                    OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
                } else {
                    pevent2 = ptcb->OSTCBEventPtr;
                    if (pevent2 != (MT_EVENT *)0) {        /* Add to event wait list                   */
                        pevent2->OSEventGrp               |= ptcb->OSTCBBitY;
                        pevent2->OSEventTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
                    }
                }
                OSTCBPrioTbl[pcp] = ptcb;
            }
        }
    }
    OSTCBCur->OSTCBStat     |= MT_STAT_MUTEX;         /* Mutex not available, pend current task        */
    OSTCBCur->OSTCBStatPend  = MT_STAT_PEND_OK;
    OSTCBCur->OSTCBDly       = timeout;               /* Store timeout in current task's TCB           */
    MT_EventTaskWait(pevent);                         /* Suspend task until event or timeout occurs    */
    MT_EXIT_CRITICAL();
    MT_Sched();                                       /* Find next highest priority task ready         */
    MT_ENTER_CRITICAL();
    switch (OSTCBCur->OSTCBStatPend) {                /* See if we timed-out or aborted                */
        case MT_STAT_PEND_OK:
             *perr = MT_ERR_NONE;
             break;

        case MT_STAT_PEND_ABORT:
             *perr = MT_ERR_PEND_ABORT;               /* Indicate that we aborted getting mutex        */
             break;

        case MT_STAT_PEND_TO:
        default:
             MT_EventTaskRemove(OSTCBCur, pevent);
             *perr = MT_ERR_TIMEOUT;                  /* Indicate that we didn't get mutex within TO   */
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
*                                POST TO A MUTUAL EXCLUSION SEMAPHORE
*
* Description: This function signals a mutual exclusion semaphore
*
* Arguments  : pevent              is a pointer to the event control block associated with the desired
*                                  mutex.
*
* Returns    : MT_ERR_NONE             The call was successful and the mutex was signaled.
*              MT_ERR_EVENT_TYPE       If you didn't pass a pointer to a mutex
*              MT_ERR_PEVENT_NULL      'pevent' is a NULL pointer
*              MT_ERR_POST_ISR         Attempted to post from an ISR (not valid for MUTEXes)
*              MT_ERR_NOT_MUTEX_OWNER  The task that did the post is NOT the owner of the MUTEX.
*              MT_ERR_PCP_LOWER        If the priority of the new task that owns the Mutex is
*                                      HIGHER (i.e. a lower number) than the PCP.  This error
*                                      indicates that you did not set the PCP higher (lower
*                                      number) than ALL the tasks that compete for the Mutex.
*                                      Unfortunately, this is something that could not be
*                                      detected when the Mutex is created because we don't know
*                                      what tasks will be using the Mutex.
*********************************************************************************************************
*/

INT8U  OSMutexPost (MT_EVENT *pevent)
{
    INT8U      pcp;                                   /* Priority ceiling priority                     */
    INT8U      prio;
#if MT_CRITICAL_METHOD == 3u                          /* Allocate storage for CPU status register      */
    MT_CPU_SR  cpu_sr = 0u;
#endif



    if (OSIntNesting > 0u) {                          /* See if called from ISR ...                    */
        return (MT_ERR_POST_ISR);                     /* ... can't POST mutex from an ISR              */
    }
#if MT_ARG_CHK_EN > 0u
    if (pevent == (MT_EVENT *)0) {                    /* Validate 'pevent'                             */
        return (MT_ERR_PEVENT_NULL);
    }
#endif
    if (pevent->OSEventType != MT_EVENT_TYPE_MUTEX) { /* Validate event block type                     */
        return (MT_ERR_EVENT_TYPE);
    }
    MT_ENTER_CRITICAL();
    pcp  = (INT8U)(pevent->OSEventCnt >> 8u);         /* Get priority ceiling priority of mutex        */
    prio = (INT8U)(pevent->OSEventCnt & MT_MUTEX_KEEP_LOWER_8);  /* Get owner's original priority      */
    if (OSTCBCur != (MT_TCB *)pevent->OSEventPtr) {   /* See if posting task owns the MUTEX            */
        MT_EXIT_CRITICAL();
        return (MT_ERR_NOT_MUTEX_OWNER);
    }
    if (pcp != MT_PRIO_MUTEX_CEIL_DIS) {
        if (OSTCBCur->OSTCBPrio == pcp) {             /* Did we have to raise current task's priority? */
            OSMutex_RdyAtPrio(OSTCBCur, prio);        /* Restore the task's original priority          */
        }
        OSTCBPrioTbl[pcp] = MT_TCB_RESERVED;          /* Reserve table entry                           */
    }
    if (pevent->OSEventGrp != 0u) {                   /* Any task waiting for the mutex?               */
                                                      /* Yes, Make HPT waiting for mutex ready         */
        prio                = MT_EventTaskRdy(pevent, (void *)0, MT_STAT_MUTEX, MT_STAT_PEND_OK);
        pevent->OSEventCnt &= MT_MUTEX_KEEP_UPPER_8;  /*      Save priority of mutex's new owner       */
        pevent->OSEventCnt |= prio;
        pevent->OSEventPtr  = OSTCBPrioTbl[prio];     /*      Link to new mutex owner's MT_TCB         */
        if ((pcp  != MT_PRIO_MUTEX_CEIL_DIS) &&
            (prio <= pcp)) {                          /*      PCP 'must' have a SMALLER prio ...       */
            MT_EXIT_CRITICAL();                       /*      ... than current task!                   */
            MT_Sched();                               /*      Find highest priority task ready to run  */
            return (MT_ERR_PCP_LOWER);
        } else {
            MT_EXIT_CRITICAL();
            MT_Sched();                               /*      Find highest priority task ready to run  */
            return (MT_ERR_NONE);
        }
    }
    pevent->OSEventCnt |= MT_MUTEX_AVAILABLE;         /* No,  Mutex is now available                   */
    pevent->OSEventPtr  = (void *)0;
    MT_EXIT_CRITICAL();
    return (MT_ERR_NONE);
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                 QUERY A MUTUAL EXCLUSION SEMAPHORE
*
* Description: This function obtains information about a mutex
*
* Arguments  : pevent          is a pointer to the event control block associated with the desired mutex
*
*              p_mutex_data    is a pointer to a structure that will contain information about the mutex
*
* Returns    : MT_ERR_NONE          The call was successful and the message was sent
*              MT_ERR_QUERY_ISR     If you called this function from an ISR
*              MT_ERR_PEVENT_NULL   If 'pevent'       is a NULL pointer
*              MT_ERR_PDATA_NULL    If 'p_mutex_data' is a NULL pointer
*              MT_ERR_EVENT_TYPE    If you are attempting to obtain data from a non mutex.
*********************************************************************************************************
*/

#if MT_MUTEX_QUERY_EN > 0u
INT8U  OSMutexQuery (MT_EVENT       *pevent,
                     MT_MUTEX_DATA  *p_mutex_data)
{
    INT8U       i;
    MT_PRIO    *psrc;
    MT_PRIO    *pdest;
#if MT_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    MT_CPU_SR   cpu_sr = 0u;
#endif



    if (OSIntNesting > 0u) {                               /* See if called from ISR ...               */
        return (MT_ERR_QUERY_ISR);                         /* ... can't QUERY mutex from an ISR        */
    }
#if MT_ARG_CHK_EN > 0u
    if (pevent == (MT_EVENT *)0) {                         /* Validate 'pevent'                        */
        return (MT_ERR_PEVENT_NULL);
    }
    if (p_mutex_data == (MT_MUTEX_DATA *)0) {              /* Validate 'p_mutex_data'                  */
        return (MT_ERR_PDATA_NULL);
    }
#endif
    if (pevent->OSEventType != MT_EVENT_TYPE_MUTEX) {      /* Validate event block type                */
        return (MT_ERR_EVENT_TYPE);
    }
    MT_ENTER_CRITICAL();
    p_mutex_data->OSMutexPCP  = (INT8U)(pevent->OSEventCnt >> 8u);
    p_mutex_data->OSOwnerPrio = (INT8U)(pevent->OSEventCnt & MT_MUTEX_KEEP_LOWER_8);
    if (p_mutex_data->OSOwnerPrio == 0xFFu) {
        p_mutex_data->OSValue = MT_TRUE;
    } else {
        p_mutex_data->OSValue = MT_FALSE;
    }
    p_mutex_data->OSEventGrp  = pevent->OSEventGrp;        /* Copy wait list                           */
    psrc                      = &pevent->OSEventTbl[0];
    pdest                     = &p_mutex_data->OSEventTbl[0];
    for (i = 0u; i < MT_EVENT_TBL_SIZE; i++) {
        *pdest++ = *psrc++;
    }
    MT_EXIT_CRITICAL();
    return (MT_ERR_NONE);
}
#endif                                                     /* MT_MUTEX_QUERY_EN                        */

/*$PAGE*/
/*
*********************************************************************************************************
*                            RESTORE A TASK BACK TO ITS ORIGINAL PRIORITY
*
* Description: This function makes a task ready at the specified priority
*
* Arguments  : ptcb            is a pointer to MT_TCB of the task to make ready
*
*              prio            is the desired priority
*
* Returns    : none
*********************************************************************************************************
*/

static  void  OSMutex_RdyAtPrio (MT_TCB  *ptcb,
                                 INT8U    prio)
{
    INT8U  y;


    y            =  ptcb->OSTCBY;                          /* Remove owner from ready list at 'pcp'    */
    OSRdyTbl[y] &= (MT_PRIO)~ptcb->OSTCBBitX;
    if (OSRdyTbl[y] == 0u) {
        OSRdyGrp &= (MT_PRIO)~ptcb->OSTCBBitY;
    }
    ptcb->OSTCBPrio         = prio;
    OSPrioCur               = prio;                        /* The current task is now at this priority */
#if MT_LOWEST_PRIO <= 63u
    ptcb->OSTCBY            = (INT8U)((INT8U)(prio >> 3u) & 0x07u);
    ptcb->OSTCBX            = (INT8U)(prio & 0x07u);
#else
    ptcb->OSTCBY            = (INT8U)((INT8U)(prio >> 4u) & 0x0Fu);
    ptcb->OSTCBX            = (INT8U) (prio & 0x0Fu);
#endif
    ptcb->OSTCBBitY         = (MT_PRIO)(1uL << ptcb->OSTCBY);
    ptcb->OSTCBBitX         = (MT_PRIO)(1uL << ptcb->OSTCBX);
    OSRdyGrp               |= ptcb->OSTCBBitY;             /* Make task ready at original priority     */
    OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
    OSTCBPrioTbl[prio]      = ptcb;
}


#endif                                                     /* MT_MUTEX_EN                              */
