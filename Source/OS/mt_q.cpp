#ifdef BAS_TEMP


#define  MT_SOURCE

#ifndef  MT_MASTER_FILE
#include "mt.h"
#endif

#if (MT_Q_EN > 0u) && (MT_MAX_QS > 0u)
/*
*********************************************************************************************************
*                                      ACCEPT MESSAGE FROM QUEUE
*
* Description: This function checks the queue to see if a message is available.  Unlike OSQPend(),
*              OSQAccept() does not suspend the calling task if a message is not available.
*
* Arguments  : pevent        is a pointer to the event control block
*
*              perr          is a pointer to where an error message will be deposited.  Possible error
*                            messages are:
*
*                            MT_ERR_NONE         The call was successful and your task received a
*                                                message.
*                            MT_ERR_EVENT_TYPE   You didn't pass a pointer to a queue
*                            MT_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer
*                            MT_ERR_Q_EMPTY      The queue did not contain any messages
*
* Returns    : != (void *)0  is the message in the queue if one is available.  The message is removed
*                            from the so the next time OSQAccept() is called, the queue will contain
*                            one less entry.
*              == (void *)0  if you received a NULL pointer message
*                            if the queue is empty or,
*                            if 'pevent' is a NULL pointer or,
*                            if you passed an invalid event type
*
* Note(s)    : As of V2.60, you can now pass NULL pointers through queues.  Because of this, the argument
*              'perr' has been added to the API to tell you about the outcome of the call.
*********************************************************************************************************
*/

#if MT_Q_ACCEPT_EN > 0u
void  *OSQAccept (MT_EVENT  *pevent,
                  INT8U     *perr)
{
    void      *pmsg;
    MT_Q      *pq;
#if MT_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#ifdef MT_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return ((void *)0);
    }
#endif

#if MT_ARG_CHK_EN > 0u
    if (pevent == (MT_EVENT *)0) {               /* Validate 'pevent'                                  */
        *perr = MT_ERR_PEVENT_NULL;
        return ((void *)0);
    }
#endif
    if (pevent->OSEventType != MT_EVENT_TYPE_Q) {/* Validate event block type                          */
        *perr = MT_ERR_EVENT_TYPE;
        return ((void *)0);
    }
    MT_ENTER_CRITICAL();
    pq = (MT_Q *)pevent->OSEventPtr;             /* Point at queue control block                       */
    if (pq->OSQEntries > 0u) {                   /* See if any messages in the queue                   */
        pmsg = *pq->OSQOut++;                    /* Yes, extract oldest message from the queue         */
        pq->OSQEntries--;                        /* Update the number of entries in the queue          */
        if (pq->OSQOut == pq->OSQEnd) {          /* Wrap OUT pointer if we are at the end of the queue */
            pq->OSQOut = pq->OSQStart;
        }
        *perr = MT_ERR_NONE;
    } else {
        *perr = MT_ERR_Q_EMPTY;
        pmsg  = (void *)0;                       /* Queue is empty                                     */
    }
    MT_EXIT_CRITICAL();
    return (pmsg);                               /* Return message received (or NULL)                  */
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                       CREATE A MESSAGE QUEUE
*
* Description: This function creates a message queue if free event control blocks are available.
*
* Arguments  : start         is a pointer to the base address of the message queue storage area.  The
*                            storage area MUST be declared as an array of pointers to 'void' as follows
*
*                            void *MessageStorage[size]
*
*              size          is the number of elements in the storage area
*
* Returns    : != (MT_EVENT *)0  is a pointer to the event control clock (MT_EVENT) associated with the
*                                created queue
*              == (MT_EVENT *)0  if no event control blocks were available or an error was detected
*********************************************************************************************************
*/

MT_EVENT  *OSQCreate (void    **start,
                      INT16U    size)
{
    MT_EVENT  *pevent;
    MT_Q      *pq;
#if MT_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#ifdef MT_SAFETY_CRITICAL_IEC61508
    if (OSSafetyCriticalStartFlag == MT_TRUE) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return ((MT_EVENT *)0);
    }
#endif

    if (OSIntNesting > 0u) {                     /* See if called from ISR ...                         */
        return ((MT_EVENT *)0);                  /* ... can't CREATE from an ISR                       */
    }
    MT_ENTER_CRITICAL();
    pevent = OSEventFreeList;                    /* Get next free event control block                  */
    if (OSEventFreeList != (MT_EVENT *)0) {      /* See if pool of free ECB pool was empty             */
        OSEventFreeList = (MT_EVENT *)OSEventFreeList->OSEventPtr;
    }
    MT_EXIT_CRITICAL();
    if (pevent != (MT_EVENT *)0) {               /* See if we have an event control block              */
        MT_ENTER_CRITICAL();
        pq = OSQFreeList;                        /* Get a free queue control block                     */
        if (pq != (MT_Q *)0) {                   /* Were we able to get a queue control block ?        */
            OSQFreeList            = OSQFreeList->OSQPtr; /* Yes, Adjust free list pointer to next free*/
            MT_EXIT_CRITICAL();
            pq->OSQStart           = start;               /*      Initialize the queue                 */
            pq->OSQEnd             = &start[size];
            pq->OSQIn              = start;
            pq->OSQOut             = start;
            pq->OSQSize            = size;
            pq->OSQEntries         = 0u;
            pevent->OSEventType    = MT_EVENT_TYPE_Q;
            pevent->OSEventCnt     = 0u;
            pevent->OSEventPtr     = pq;
#if MT_EVENT_NAME_EN > 0u
            pevent->OSEventName    = (INT8U *)(void *)"?";
#endif
            MT_EventWaitListInit(pevent);                 /*      Initialize the wait list             */
        } else {
            pevent->OSEventPtr = (void *)OSEventFreeList; /* No,  Return event control block on error  */
            OSEventFreeList    = pevent;
            MT_EXIT_CRITICAL();
            pevent = (MT_EVENT *)0;
        }
    }
    return (pevent);
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                       DELETE A MESSAGE QUEUE
*
* Description: This function deletes a message queue and readies all tasks pending on the queue.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            queue.
*
*              opt           determines delete options as follows:
*                            opt == MT_DEL_NO_PEND   Delete the queue ONLY if no task pending
*                            opt == MT_DEL_ALWAYS    Deletes the queue even if tasks are waiting.
*                                                    In this case, all the tasks pending will be readied.
*
*              perr          is a pointer to an error code that can contain one of the following values:
*                            MT_ERR_NONE             The call was successful and the queue was deleted
*                            MT_ERR_DEL_ISR          If you tried to delete the queue from an ISR
*                            MT_ERR_INVALID_OPT      An invalid option was specified
*                            MT_ERR_TASK_WAITING     One or more tasks were waiting on the queue
*                            MT_ERR_EVENT_TYPE       If you didn't pass a pointer to a queue
*                            MT_ERR_PEVENT_NULL      If 'pevent' is a NULL pointer.
*
* Returns    : pevent        upon error
*              (MT_EVENT *)0 if the queue was successfully deleted.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of
*                 the queue MUST check the return code of OSQPend().
*              2) OSQAccept() callers will not know that the intended queue has been deleted unless
*                 they check 'pevent' to see that it's a NULL pointer.
*              3) This call can potentially disable interrupts for a long time.  The interrupt disable
*                 time is directly proportional to the number of tasks waiting on the queue.
*              4) Because ALL tasks pending on the queue will be readied, you MUST be careful in
*                 applications where the queue is used for mutual exclusion because the resource(s)
*                 will no longer be guarded by the queue.
*              5) If the storage for the message queue was allocated dynamically (i.e. using a malloc()
*                 type call) then your application MUST release the memory storage by call the counterpart
*                 call of the dynamic allocation scheme used.  If the queue storage was created statically
*                 then, the storage can be reused.
*              6) All tasks that were waiting for the queue will be readied and returned an 
*                 MT_ERR_PEND_ABORT if OSQDel() was called with MT_DEL_ALWAYS
*********************************************************************************************************
*/

#if MT_Q_DEL_EN > 0u
MT_EVENT  *OSQDel (MT_EVENT  *pevent,
                   INT8U      opt,
                   INT8U     *perr)
{
    BOOLEAN    tasks_waiting;
    MT_EVENT  *pevent_return;
    MT_Q      *pq;
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
    if (pevent->OSEventType != MT_EVENT_TYPE_Q) {          /* Validate event block type                */
        *perr = MT_ERR_EVENT_TYPE;
        return (pevent);
    }
    if (OSIntNesting > 0u) {                               /* See if called from ISR ...               */
        *perr = MT_ERR_DEL_ISR;                            /* ... can't DELETE from an ISR             */
        return (pevent);
    }
    MT_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0u) {                        /* See if any tasks waiting on queue        */
        tasks_waiting = MT_TRUE;                           /* Yes                                      */
    } else {
        tasks_waiting = MT_FALSE;                          /* No                                       */
    }
    switch (opt) {
        case MT_DEL_NO_PEND:                               /* Delete queue only if no task waiting     */
             if (tasks_waiting == MT_FALSE) {
#if MT_EVENT_NAME_EN > 0u
                 pevent->OSEventName    = (INT8U *)(void *)"?";
#endif
                 pq                     = (MT_Q *)pevent->OSEventPtr;  /* Return MT_Q to free list     */
                 pq->OSQPtr             = OSQFreeList;
                 OSQFreeList            = pq;
                 pevent->OSEventType    = MT_EVENT_TYPE_UNUSED;
                 pevent->OSEventPtr     = OSEventFreeList; /* Return Event Control Block to free list  */
                 pevent->OSEventCnt     = 0u;
                 OSEventFreeList        = pevent;          /* Get next free event control block        */
                 MT_EXIT_CRITICAL();
                 *perr                  = MT_ERR_NONE;
                 pevent_return          = (MT_EVENT *)0;   /* Queue has been deleted                   */
             } else {
                 MT_EXIT_CRITICAL();
                 *perr                  = MT_ERR_TASK_WAITING;
                 pevent_return          = pevent;
             }
             break;

        case MT_DEL_ALWAYS:                                /* Always delete the queue                  */
             while (pevent->OSEventGrp != 0u) {            /* Ready ALL tasks waiting for queue        */
                 (void)MT_EventTaskRdy(pevent, (void *)0, MT_STAT_Q, MT_STAT_PEND_ABORT);
             }
#if MT_EVENT_NAME_EN > 0u
             pevent->OSEventName    = (INT8U *)(void *)"?";
#endif
             pq                     = (MT_Q *)pevent->OSEventPtr;   /* Return MT_Q to free list        */
             pq->OSQPtr             = OSQFreeList;
             OSQFreeList            = pq;
             pevent->OSEventType    = MT_EVENT_TYPE_UNUSED;
             pevent->OSEventPtr     = OSEventFreeList;     /* Return Event Control Block to free list  */
             pevent->OSEventCnt     = 0u;
             OSEventFreeList        = pevent;              /* Get next free event control block        */
             MT_EXIT_CRITICAL();
             if (tasks_waiting == MT_TRUE) {               /* Reschedule only if task(s) were waiting  */
                 MT_Sched();                               /* Find highest priority task ready to run  */
             }
             *perr                  = MT_ERR_NONE;
             pevent_return          = (MT_EVENT *)0;       /* Queue has been deleted                   */
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
*                                             FLUSH QUEUE
*
* Description : This function is used to flush the contents of the message queue.
*
* Arguments   : none
*
* Returns     : MT_ERR_NONE         upon success
*               MT_ERR_EVENT_TYPE   If you didn't pass a pointer to a queue
*               MT_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer
*
* WARNING     : You should use this function with great care because, when to flush the queue, you LOOSE
*               the references to what the queue entries are pointing to and thus, you could cause
*               'memory leaks'.  In other words, the data you are pointing to that's being referenced
*               by the queue entries should, most likely, need to be de-allocated (i.e. freed).
*********************************************************************************************************
*/

#if MT_Q_FLUSH_EN > 0u
INT8U  OSQFlush (MT_EVENT *pevent)
{
    MT_Q      *pq;
#if MT_CRITICAL_METHOD == 3u                          /* Allocate storage for CPU status register      */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#if MT_ARG_CHK_EN > 0u
    if (pevent == (MT_EVENT *)0) {                    /* Validate 'pevent'                             */
        return (MT_ERR_PEVENT_NULL);
    }
    if (pevent->OSEventType != MT_EVENT_TYPE_Q) {     /* Validate event block type                     */
        return (MT_ERR_EVENT_TYPE);
    }
#endif
    MT_ENTER_CRITICAL();
    pq             = (MT_Q *)pevent->OSEventPtr;      /* Point to queue storage structure              */
    pq->OSQIn      = pq->OSQStart;
    pq->OSQOut     = pq->OSQStart;
    pq->OSQEntries = 0u;
    MT_EXIT_CRITICAL();
    return (MT_ERR_NONE);
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                    PEND ON A QUEUE FOR A MESSAGE
*
* Description: This function waits for a message to be sent to a queue
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for a message to arrive at the queue up to the amount of time
*                            specified by this argument.  If you specify 0, however, your task will wait
*                            forever at the specified queue or, until a message arrives.
*
*              perr          is a pointer to where an error message will be deposited.  Possible error
*                            messages are:
*
*                            MT_ERR_NONE         The call was successful and your task received a
*                                                message.
*                            MT_ERR_TIMEOUT      A message was not received within the specified 'timeout'.
*                            MT_ERR_PEND_ABORT   The wait on the queue was aborted.
*                            MT_ERR_EVENT_TYPE   You didn't pass a pointer to a queue
*                            MT_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer
*                            MT_ERR_PEND_ISR     If you called this function from an ISR and the result
*                                                would lead to a suspension.
*                            MT_ERR_PEND_LOCKED  If you called this function with the scheduler is locked
*
* Returns    : != (void *)0  is a pointer to the message received
*              == (void *)0  if you received a NULL pointer message or,
*                            if no message was received or,
*                            if 'pevent' is a NULL pointer or,
*                            if you didn't pass a pointer to a queue.
*
* Note(s)    : As of V2.60, this function allows you to receive NULL pointer messages.
*********************************************************************************************************
*/

void  *OSQPend (MT_EVENT  *pevent,
                INT32U     timeout,
                INT8U     *perr)
{
    void      *pmsg;
    MT_Q      *pq;
#if MT_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#ifdef MT_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return ((void *)0);
    }
#endif

#if MT_ARG_CHK_EN > 0u
    if (pevent == (MT_EVENT *)0) {               /* Validate 'pevent'                                  */
        *perr = MT_ERR_PEVENT_NULL;
        return ((void *)0);
    }
#endif
    if (pevent->OSEventType != MT_EVENT_TYPE_Q) {/* Validate event block type                          */
        *perr = MT_ERR_EVENT_TYPE;
        return ((void *)0);
    }
    if (OSIntNesting > 0u) {                     /* See if called from ISR ...                         */
        *perr = MT_ERR_PEND_ISR;                 /* ... can't PEND from an ISR                         */
        return ((void *)0);
    }
    if (OSLockNesting > 0u) {                    /* See if called with scheduler locked ...            */
        *perr = MT_ERR_PEND_LOCKED;              /* ... can't PEND when locked                         */
        return ((void *)0);
    }
    MT_ENTER_CRITICAL();
    pq = (MT_Q *)pevent->OSEventPtr;             /* Point at queue control block                       */
    if (pq->OSQEntries > 0u) {                   /* See if any messages in the queue                   */
        pmsg = *pq->OSQOut++;                    /* Yes, extract oldest message from the queue         */
        pq->OSQEntries--;                        /* Update the number of entries in the queue          */
        if (pq->OSQOut == pq->OSQEnd) {          /* Wrap OUT pointer if we are at the end of the queue */
            pq->OSQOut = pq->OSQStart;
        }
        MT_EXIT_CRITICAL();
        *perr = MT_ERR_NONE;
        return (pmsg);                           /* Return message received                            */
    }
    OSTCBCur->OSTCBStat     |= MT_STAT_Q;        /* Task will have to pend for a message to be posted  */
    OSTCBCur->OSTCBStatPend  = MT_STAT_PEND_OK;
    OSTCBCur->OSTCBDly       = timeout;          /* Load timeout into TCB                              */
    MT_EventTaskWait(pevent);                    /* Suspend task until event or timeout occurs         */
    MT_EXIT_CRITICAL();
    MT_Sched();                                  /* Find next highest priority task ready to run       */
    MT_ENTER_CRITICAL();
    switch (OSTCBCur->OSTCBStatPend) {                /* See if we timed-out or aborted                */
        case MT_STAT_PEND_OK:                         /* Extract message from TCB (Put there by QPost) */
             pmsg =  OSTCBCur->OSTCBMsg;
            *perr =  MT_ERR_NONE;
             break;

        case MT_STAT_PEND_ABORT:
             pmsg = (void *)0;
            *perr =  MT_ERR_PEND_ABORT;               /* Indicate that we aborted                      */
             break;

        case MT_STAT_PEND_TO:
        default:
             MT_EventTaskRemove(OSTCBCur, pevent);
             pmsg = (void *)0;
            *perr =  MT_ERR_TIMEOUT;                  /* Indicate that we didn't get event within TO   */
             break;
    }
    OSTCBCur->OSTCBStat          =  MT_STAT_RDY;      /* Set   task  status to ready                   */
    OSTCBCur->OSTCBStatPend      =  MT_STAT_PEND_OK;  /* Clear pend  status                            */
    OSTCBCur->OSTCBEventPtr      = (MT_EVENT  *)0;    /* Clear event pointers                          */
#if (MT_EVENT_MULTI_EN > 0u)
    OSTCBCur->OSTCBEventMultiPtr = (MT_EVENT **)0;
#endif
    OSTCBCur->OSTCBMsg           = (void      *)0;    /* Clear  received message                       */
    MT_EXIT_CRITICAL();
    return (pmsg);                                    /* Return received message                       */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                  ABORT WAITING ON A MESSAGE QUEUE
*
* Description: This function aborts & readies any tasks currently waiting on a queue.  This function
*              should be used to fault-abort the wait on the queue, rather than to normally signal
*              the queue via OSQPost(), OSQPostFront() or OSQPostOpt().
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue.
*
*              opt           determines the type of ABORT performed:
*                            MT_PEND_OPT_NONE         ABORT wait for a single task (HPT) waiting on the
*                                                     queue
*                            MT_PEND_OPT_BROADCAST    ABORT wait for ALL tasks that are  waiting on the
*                                                     queue
*
*              perr          is a pointer to where an error message will be deposited.  Possible error
*                            messages are:
*
*                            MT_ERR_NONE         No tasks were     waiting on the queue.
*                            MT_ERR_PEND_ABORT   At least one task waiting on the queue was readied
*                                                and informed of the aborted wait; check return value
*                                                for the number of tasks whose wait on the queue
*                                                was aborted.
*                            MT_ERR_EVENT_TYPE   If you didn't pass a pointer to a queue.
*                            MT_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer.
*
* Returns    : == 0          if no tasks were waiting on the queue, or upon error.
*              >  0          if one or more tasks waiting on the queue are now readied and informed.
*********************************************************************************************************
*/

#if MT_Q_PEND_ABORT_EN > 0u
INT8U  OSQPendAbort (MT_EVENT  *pevent,
                     INT8U      opt,
                     INT8U     *perr)
{
    INT8U      nbr_tasks;
#if MT_CRITICAL_METHOD == 3u                               /* Allocate storage for CPU status register */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#ifdef MT_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return (0u);
    }
#endif

#if MT_ARG_CHK_EN > 0u
    if (pevent == (MT_EVENT *)0) {                         /* Validate 'pevent'                        */
        *perr = MT_ERR_PEVENT_NULL;
        return (0u);
    }
#endif
    if (pevent->OSEventType != MT_EVENT_TYPE_Q) {          /* Validate event block type                */
        *perr = MT_ERR_EVENT_TYPE;
        return (0u);
    }
    MT_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0u) {                        /* See if any task waiting on queue?        */
        nbr_tasks = 0u;
        switch (opt) {
            case MT_PEND_OPT_BROADCAST:                    /* Do we need to abort ALL waiting tasks?   */
                 while (pevent->OSEventGrp != 0u) {        /* Yes, ready ALL tasks waiting on queue    */
                     (void)MT_EventTaskRdy(pevent, (void *)0, MT_STAT_Q, MT_STAT_PEND_ABORT);
                     nbr_tasks++;
                 }
                 break;

            case MT_PEND_OPT_NONE:
            default:                                       /* No,  ready HPT       waiting on queue    */
                 (void)MT_EventTaskRdy(pevent, (void *)0, MT_STAT_Q, MT_STAT_PEND_ABORT);
                 nbr_tasks++;
                 break;
        }
        MT_EXIT_CRITICAL();
        MT_Sched();                                        /* Find HPT ready to run                    */
        *perr = MT_ERR_PEND_ABORT;
        return (nbr_tasks);
    }
    MT_EXIT_CRITICAL();
    *perr = MT_ERR_NONE;
    return (0u);                                           /* No tasks waiting on queue                */
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                       POST MESSAGE TO A QUEUE
*
* Description: This function sends a message to a queue
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              pmsg          is a pointer to the message to send.
*
* Returns    : MT_ERR_NONE           The call was successful and the message was sent
*              MT_ERR_Q_FULL         If the queue cannot accept any more messages because it is full.
*              MT_ERR_EVENT_TYPE     If you didn't pass a pointer to a queue.
*              MT_ERR_PEVENT_NULL    If 'pevent' is a NULL pointer
*
* Note(s)    : As of V2.60, this function allows you to send NULL pointer messages.
*********************************************************************************************************
*/

#if MT_Q_POST_EN > 0u
INT8U  OSQPost (MT_EVENT  *pevent,
                void      *pmsg)
{
    MT_Q      *pq;
#if MT_CRITICAL_METHOD == 3u                           /* Allocate storage for CPU status register     */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#if MT_ARG_CHK_EN > 0u
    if (pevent == (MT_EVENT *)0) {                     /* Validate 'pevent'                            */
        return (MT_ERR_PEVENT_NULL);
    }
#endif
    if (pevent->OSEventType != MT_EVENT_TYPE_Q) {      /* Validate event block type                    */
        return (MT_ERR_EVENT_TYPE);
    }
    MT_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0u) {                    /* See if any task pending on queue             */
                                                       /* Ready highest priority task waiting on event */
        (void)MT_EventTaskRdy(pevent, pmsg, MT_STAT_Q, MT_STAT_PEND_OK);
        MT_EXIT_CRITICAL();
        MT_Sched();                                    /* Find highest priority task ready to run      */
        return (MT_ERR_NONE);
    }
    pq = (MT_Q *)pevent->OSEventPtr;                   /* Point to queue control block                 */
    if (pq->OSQEntries >= pq->OSQSize) {               /* Make sure queue is not full                  */
        MT_EXIT_CRITICAL();
        return (MT_ERR_Q_FULL);
    }
    *pq->OSQIn++ = pmsg;                               /* Insert message into queue                    */
    pq->OSQEntries++;                                  /* Update the nbr of entries in the queue       */
    if (pq->OSQIn == pq->OSQEnd) {                     /* Wrap IN ptr if we are at end of queue        */
        pq->OSQIn = pq->OSQStart;
    }
    MT_EXIT_CRITICAL();
    return (MT_ERR_NONE);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                POST MESSAGE TO THE FRONT OF A QUEUE
*
* Description: This function sends a message to a queue but unlike OSQPost(), the message is posted at
*              the front instead of the end of the queue.  Using OSQPostFront() allows you to send
*              'priority' messages.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              pmsg          is a pointer to the message to send.
*
* Returns    : MT_ERR_NONE           The call was successful and the message was sent
*              MT_ERR_Q_FULL         If the queue cannot accept any more messages because it is full.
*              MT_ERR_EVENT_TYPE     If you didn't pass a pointer to a queue.
*              MT_ERR_PEVENT_NULL    If 'pevent' is a NULL pointer
*
* Note(s)    : As of V2.60, this function allows you to send NULL pointer messages.
*********************************************************************************************************
*/

#if MT_Q_POST_FRONT_EN > 0u
INT8U  OSQPostFront (MT_EVENT  *pevent,
                     void      *pmsg)
{
    MT_Q      *pq;
#if MT_CRITICAL_METHOD == 3u                          /* Allocate storage for CPU status register      */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#if MT_ARG_CHK_EN > 0u
    if (pevent == (MT_EVENT *)0) {                    /* Validate 'pevent'                             */
        return (MT_ERR_PEVENT_NULL);
    }
#endif
    if (pevent->OSEventType != MT_EVENT_TYPE_Q) {     /* Validate event block type                     */
        return (MT_ERR_EVENT_TYPE);
    }
    MT_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0u) {                   /* See if any task pending on queue              */
                                                      /* Ready highest priority task waiting on event  */
        (void)MT_EventTaskRdy(pevent, pmsg, MT_STAT_Q, MT_STAT_PEND_OK);
        MT_EXIT_CRITICAL();
        MT_Sched();                                   /* Find highest priority task ready to run       */
        return (MT_ERR_NONE);
    }
    pq = (MT_Q *)pevent->OSEventPtr;                  /* Point to queue control block                  */
    if (pq->OSQEntries >= pq->OSQSize) {              /* Make sure queue is not full                   */
        MT_EXIT_CRITICAL();
        return (MT_ERR_Q_FULL);
    }
    if (pq->OSQOut == pq->OSQStart) {                 /* Wrap OUT ptr if we are at the 1st queue entry */
        pq->OSQOut = pq->OSQEnd;
    }
    pq->OSQOut--;
    *pq->OSQOut = pmsg;                               /* Insert message into queue                     */
    pq->OSQEntries++;                                 /* Update the nbr of entries in the queue        */
    MT_EXIT_CRITICAL();
    return (MT_ERR_NONE);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                       POST MESSAGE TO A QUEUE
*
* Description: This function sends a message to a queue.  This call has been added to reduce code size
*              since it can replace both OSQPost() and OSQPostFront().  Also, this function adds the
*              capability to broadcast a message to ALL tasks waiting on the message queue.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              pmsg          is a pointer to the message to send.
*
*              opt           determines the type of POST performed:
*                            MT_POST_OPT_NONE         POST to a single waiting task
*                                                     (Identical to OSQPost())
*                            MT_POST_OPT_BROADCAST    POST to ALL tasks that are waiting on the queue
*                            MT_POST_OPT_FRONT        POST as LIFO (Simulates OSQPostFront())
*                            MT_POST_OPT_NO_SCHED     Indicates that the scheduler will NOT be invoked
*
* Returns    : MT_ERR_NONE           The call was successful and the message was sent
*              MT_ERR_Q_FULL         If the queue cannot accept any more messages because it is full.
*              MT_ERR_EVENT_TYPE     If you didn't pass a pointer to a queue.
*              MT_ERR_PEVENT_NULL    If 'pevent' is a NULL pointer
*
* Warning    : Interrupts can be disabled for a long time if you do a 'broadcast'.  In fact, the
*              interrupt disable time is proportional to the number of tasks waiting on the queue.
*********************************************************************************************************
*/

#if MT_Q_POST_OPT_EN > 0u
INT8U  OSQPostOpt (MT_EVENT  *pevent,
                   void      *pmsg,
                   INT8U      opt)
{
    MT_Q      *pq;
#if MT_CRITICAL_METHOD == 3u                          /* Allocate storage for CPU status register      */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#if MT_ARG_CHK_EN > 0u
    if (pevent == (MT_EVENT *)0) {                    /* Validate 'pevent'                             */
        return (MT_ERR_PEVENT_NULL);
    }
#endif
    if (pevent->OSEventType != MT_EVENT_TYPE_Q) {     /* Validate event block type                     */
        return (MT_ERR_EVENT_TYPE);
    }
    MT_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0x00u) {                /* See if any task pending on queue              */
        if ((opt & MT_POST_OPT_BROADCAST) != 0x00u) { /* Do we need to post msg to ALL waiting tasks ? */
            while (pevent->OSEventGrp != 0u) {        /* Yes, Post to ALL tasks waiting on queue       */
                (void)MT_EventTaskRdy(pevent, pmsg, MT_STAT_Q, MT_STAT_PEND_OK);
            }
        } else {                                      /* No,  Post to HPT waiting on queue             */
            (void)MT_EventTaskRdy(pevent, pmsg, MT_STAT_Q, MT_STAT_PEND_OK);
        }
        MT_EXIT_CRITICAL();
        if ((opt & MT_POST_OPT_NO_SCHED) == 0u) {     /* See if scheduler needs to be invoked          */
            MT_Sched();                               /* Find highest priority task ready to run       */
        }
        return (MT_ERR_NONE);
    }
    pq = (MT_Q *)pevent->OSEventPtr;                  /* Point to queue control block                  */
    if (pq->OSQEntries >= pq->OSQSize) {              /* Make sure queue is not full                   */
        MT_EXIT_CRITICAL();
        return (MT_ERR_Q_FULL);
    }
    if ((opt & MT_POST_OPT_FRONT) != 0x00u) {         /* Do we post to the FRONT of the queue?         */
        if (pq->OSQOut == pq->OSQStart) {             /* Yes, Post as LIFO, Wrap OUT pointer if we ... */
            pq->OSQOut = pq->OSQEnd;                  /*      ... are at the 1st queue entry           */
        }
        pq->OSQOut--;
        *pq->OSQOut = pmsg;                           /*      Insert message into queue                */
    } else {                                          /* No,  Post as FIFO                             */
        *pq->OSQIn++ = pmsg;                          /*      Insert message into queue                */
        if (pq->OSQIn == pq->OSQEnd) {                /*      Wrap IN ptr if we are at end of queue    */
            pq->OSQIn = pq->OSQStart;
        }
    }
    pq->OSQEntries++;                                 /* Update the nbr of entries in the queue        */
    MT_EXIT_CRITICAL();
    return (MT_ERR_NONE);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                        QUERY A MESSAGE QUEUE
*
* Description: This function obtains information about a message queue.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              p_q_data      is a pointer to a structure that will contain information about the message
*                            queue.
*
* Returns    : MT_ERR_NONE         The call was successful and the message was sent
*              MT_ERR_EVENT_TYPE   If you are attempting to obtain data from a non queue.
*              MT_ERR_PEVENT_NULL  If 'pevent'   is a NULL pointer
*              MT_ERR_PDATA_NULL   If 'p_q_data' is a NULL pointer
*********************************************************************************************************
*/

#if MT_Q_QUERY_EN > 0u
INT8U  OSQQuery (MT_EVENT  *pevent,
                 MT_Q_DATA *p_q_data)
{
    MT_Q       *pq;
    INT8U       i;
    MT_PRIO    *psrc;
    MT_PRIO    *pdest;
#if MT_CRITICAL_METHOD == 3u                           /* Allocate storage for CPU status register     */
    MT_CPU_SR   cpu_sr = 0u;
#endif



#if MT_ARG_CHK_EN > 0u
    if (pevent == (MT_EVENT *)0) {                     /* Validate 'pevent'                            */
        return (MT_ERR_PEVENT_NULL);
    }
    if (p_q_data == (MT_Q_DATA *)0) {                  /* Validate 'p_q_data'                          */
        return (MT_ERR_PDATA_NULL);
    }
#endif
    if (pevent->OSEventType != MT_EVENT_TYPE_Q) {      /* Validate event block type                    */
        return (MT_ERR_EVENT_TYPE);
    }
    MT_ENTER_CRITICAL();
    p_q_data->OSEventGrp = pevent->OSEventGrp;         /* Copy message queue wait list                 */
    psrc                 = &pevent->OSEventTbl[0];
    pdest                = &p_q_data->OSEventTbl[0];
    for (i = 0u; i < MT_EVENT_TBL_SIZE; i++) {
        *pdest++ = *psrc++;
    }
    pq = (MT_Q *)pevent->OSEventPtr;
    if (pq->OSQEntries > 0u) {
        p_q_data->OSMsg = *pq->OSQOut;                 /* Get next message to return if available      */
    } else {
        p_q_data->OSMsg = (void *)0;
    }
    p_q_data->OSNMsgs = pq->OSQEntries;
    p_q_data->OSQSize = pq->OSQSize;
    MT_EXIT_CRITICAL();
    return (MT_ERR_NONE);
}
#endif                                                 /* MT_Q_QUERY_EN                                */

/*$PAGE*/
/*
*********************************************************************************************************
*                                     QUEUE MODULE INITIALIZATION
*
* Description : This function is called by MTK to initialize the message queue module.  Your
*               application MUST NOT call this function.
*
* Arguments   :  none
*
* Returns     : none
*
* Note(s)    : This function is INTERNAL to MTK and your application should not call it.
*********************************************************************************************************
*/

void  MT_QInit (void)
{
#if MT_MAX_QS == 1u
    OSQFreeList         = &OSQTbl[0];                /* Only ONE queue!                                */
    OSQFreeList->OSQPtr = (MT_Q *)0;
#endif

#if MT_MAX_QS >= 2u
    INT16U   ix;
    INT16U   ix_next;
    MT_Q    *pq1;
    MT_Q    *pq2;



    MT_MemClr((INT8U *)&OSQTbl[0], sizeof(OSQTbl));  /* Clear the queue table                          */
    for (ix = 0u; ix < (MT_MAX_QS - 1u); ix++) {     /* Init. list of free QUEUE control blocks        */
        ix_next = ix + 1u;
        pq1 = &OSQTbl[ix];
        pq2 = &OSQTbl[ix_next];
        pq1->OSQPtr = pq2;
    }
    pq1         = &OSQTbl[ix];
    pq1->OSQPtr = (MT_Q *)0;
    OSQFreeList = &OSQTbl[0];
#endif
}
#endif                                               /* MT_Q_EN                                        */
#endif //BAS_TEMP
