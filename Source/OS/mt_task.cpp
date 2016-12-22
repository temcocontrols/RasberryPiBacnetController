

#define  MT_SOURCE

#ifndef  MT_MASTER_FILE
#include "mt.h"
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                      CHANGE PRIORITY OF A TASK
*
* Description: This function allows you to change the priority of a task dynamically.  Note that the new
*              priority MUST be available.
*
* Arguments  : oldp     is the old priority
*
*              newp     is the new priority
*
* Returns    : MT_ERR_NONE            is the call was successful
*              MT_ERR_PRIO_INVALID    if the priority you specify is higher that the maximum allowed
*                                     (i.e. >= MT_LOWEST_PRIO)
*              MT_ERR_PRIO_EXIST      if the new priority already exist.
*              MT_ERR_PRIO            there is no task with the specified OLD priority (i.e. the OLD task does
*                                     not exist.
*              MT_ERR_TASK_NOT_EXIST  if the task is assigned to a Mutex PIP.
*********************************************************************************************************
*/

#if MT_TASK_CHANGE_PRIO_EN > 0u
INT8U  OSTaskChangePrio (INT8U  oldprio,
                         INT8U  newprio)
{
#if (MT_EVENT_EN)
    MT_EVENT  *pevent;
#if (MT_EVENT_MULTI_EN > 0u)
    MT_EVENT **pevents;
#endif
#endif
    MT_TCB    *ptcb;
    INT8U      y_new;
    INT8U      x_new;
    INT8U      y_old;
    MT_PRIO    bity_new;
    MT_PRIO    bitx_new;
    MT_PRIO    bity_old;
    MT_PRIO    bitx_old;
#if MT_CRITICAL_METHOD == 3u
    MT_CPU_SR  cpu_sr = 0u;                                 /* Storage for CPU status register         */
#endif


/*$PAGE*/
#if MT_ARG_CHK_EN > 0u
    if (oldprio >= MT_LOWEST_PRIO) {
        if (oldprio != MT_PRIO_SELF) {
            return (MT_ERR_PRIO_INVALID);
        }
    }
    if (newprio >= MT_LOWEST_PRIO) {
        return (MT_ERR_PRIO_INVALID);
    }
#endif
    MT_ENTER_CRITICAL();
    if (OSTCBPrioTbl[newprio] != (MT_TCB *)0) {             /* New priority must not already exist     */
        MT_EXIT_CRITICAL();
        return (MT_ERR_PRIO_EXIST);
    }
    if (oldprio == MT_PRIO_SELF) {                          /* See if changing self                    */
        oldprio = OSTCBCur->OSTCBPrio;                      /* Yes, get priority                       */
    }
    ptcb = OSTCBPrioTbl[oldprio];
    if (ptcb == (MT_TCB *)0) {                              /* Does task to change exist?              */
        MT_EXIT_CRITICAL();                                 /* No, can't change its priority!          */
        return (MT_ERR_PRIO);
    }
    if (ptcb == MT_TCB_RESERVED) {                          /* Is task assigned to Mutex               */
        MT_EXIT_CRITICAL();                                 /* No, can't change its priority!          */
        return (MT_ERR_TASK_NOT_EXIST);
    }
#if MT_LOWEST_PRIO <= 63u
    y_new                 = (INT8U)(newprio >> 3u);         /* Yes, compute new TCB fields             */
    x_new                 = (INT8U)(newprio & 0x07u);
#else
    y_new                 = (INT8U)((INT8U)(newprio >> 4u) & 0x0Fu);
    x_new                 = (INT8U)(newprio & 0x0Fu);
#endif
    bity_new              = (MT_PRIO)(1uL << y_new);
    bitx_new              = (MT_PRIO)(1uL << x_new);

    OSTCBPrioTbl[oldprio] = (MT_TCB *)0;                    /* Remove TCB from old priority            */
    OSTCBPrioTbl[newprio] =  ptcb;                          /* Place pointer to TCB @ new priority     */
    y_old                 =  ptcb->OSTCBY;
    bity_old              =  ptcb->OSTCBBitY;
    bitx_old              =  ptcb->OSTCBBitX;
    if ((OSRdyTbl[y_old] &   bitx_old) != 0u) {             /* If task is ready make it not            */
         OSRdyTbl[y_old] &= (MT_PRIO)~bitx_old;
         if (OSRdyTbl[y_old] == 0u) {
             OSRdyGrp &= (MT_PRIO)~bity_old;
         }
         OSRdyGrp        |= bity_new;                       /* Make new priority ready to run          */
         OSRdyTbl[y_new] |= bitx_new;
    }

#if (MT_EVENT_EN)
    pevent = ptcb->OSTCBEventPtr;
    if (pevent != (MT_EVENT *)0) {
        pevent->OSEventTbl[y_old] &= (MT_PRIO)~bitx_old;    /* Remove old task prio from wait list     */
        if (pevent->OSEventTbl[y_old] == 0u) {
            pevent->OSEventGrp    &= (MT_PRIO)~bity_old;
        }
        pevent->OSEventGrp        |= bity_new;              /* Add    new task prio to   wait list     */
        pevent->OSEventTbl[y_new] |= bitx_new;
    }
#if (MT_EVENT_MULTI_EN > 0u)
    if (ptcb->OSTCBEventMultiPtr != (MT_EVENT **)0) {
        pevents =  ptcb->OSTCBEventMultiPtr;
        pevent  = *pevents;
        while (pevent != (MT_EVENT *)0) {
            pevent->OSEventTbl[y_old] &= (MT_PRIO)~bitx_old;   /* Remove old task prio from wait lists */
            if (pevent->OSEventTbl[y_old] == 0u) {
                pevent->OSEventGrp    &= (MT_PRIO)~bity_old;
            }
            pevent->OSEventGrp        |= bity_new;          /* Add    new task prio to   wait lists    */
            pevent->OSEventTbl[y_new] |= bitx_new;
            pevents++;
            pevent                     = *pevents;
        }
    }
#endif
#endif

    ptcb->OSTCBPrio = newprio;                              /* Set new task priority                   */
    ptcb->OSTCBY    = y_new;
    ptcb->OSTCBX    = x_new;
    ptcb->OSTCBBitY = bity_new;
    ptcb->OSTCBBitX = bitx_new;
    MT_EXIT_CRITICAL();
    if (OSRunning == MT_TRUE) {
        MT_Sched();                                         /* Find new highest priority task          */
    }
    return (MT_ERR_NONE);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                            CREATE A TASK
*
* Description: This function is used to have MTK manage the execution of a task.  Tasks can either
*              be created prior to the start of multitasking or by a running task.  A task cannot be
*              created by an ISR.
*
* Arguments  : task     is a pointer to the task's code
*
*              p_arg    is a pointer to an optional data area which can be used to pass parameters to
*                       the task when the task first executes.  Where the task is concerned it thinks
*                       it was invoked and passed the argument 'p_arg' as follows:
*
*                           void Task (void *p_arg)
*                           {
*                               for (;;) {
*                                   Task code;
*                               }
*                           }
*
*              ptos     is a pointer to the task's top of stack.  If the configuration constant
*                       MT_STK_GROWTH is set to 1, the stack is assumed to grow downward (i.e. from high
*                       memory to low memory).  'pstk' will thus point to the highest (valid) memory
*                       location of the stack.  If MT_STK_GROWTH is set to 0, 'pstk' will point to the
*                       lowest memory location of the stack and the stack will grow with increasing
*                       memory locations.
*
*              prio     is the task's priority.  A unique priority MUST be assigned to each task and the
*                       lower the number, the higher the priority.
*
* Returns    : MT_ERR_NONE                      if the function was successful.
*              MT_ERR_PRIO_EXIST                if the task priority already exist
*                                               (each task MUST have a unique priority).
*              MT_ERR_PRIO_INVALID              if the priority you specify is higher that the maximum
*                                               allowed (i.e. >= MT_LOWEST_PRIO)
*              MT_ERR_TASK_CREATE_ISR           if you tried to create a task from an ISR.
*              MT_ERR_ILLEGAL_CREATE_RUN_TIME   if you tried to create a task after safety critical
*                                               operation started.
*********************************************************************************************************
*/

#if MT_TASK_CREATE_EN > 0u
INT8U  OSTaskCreate (void   (*task)(void *p_arg),
                     void    *p_arg,
                     MT_STK  *ptos,
                     INT8U    prio)
{
    MT_STK     *psp;
    INT8U       err;
#if MT_CRITICAL_METHOD == 3u                 /* Allocate storage for CPU status register               */
    MT_CPU_SR   cpu_sr = 0u;
#endif



#ifdef MT_SAFETY_CRITICAL_IEC61508
    if (OSSafetyCriticalStartFlag == MT_TRUE) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return (MT_ERR_ILLEGAL_CREATE_RUN_TIME);
    }
#endif

#if MT_ARG_CHK_EN > 0u
    if (prio > MT_LOWEST_PRIO) {             /* Make sure priority is within allowable range           */
        return (MT_ERR_PRIO_INVALID);
    }
#endif
    MT_ENTER_CRITICAL();
    if (OSIntNesting > 0u) {                 /* Make sure we don't create the task from within an ISR  */
        MT_EXIT_CRITICAL();
        return (MT_ERR_TASK_CREATE_ISR);
    }
    if (OSTCBPrioTbl[prio] == (MT_TCB *)0) { /* Make sure task doesn't already exist at this priority  */
        OSTCBPrioTbl[prio] = MT_TCB_RESERVED;/* Reserve the priority to prevent others from doing ...  */
                                             /* ... the same thing until task is created.              */
        MT_EXIT_CRITICAL();
        psp = OSTaskStkInit(task, p_arg, ptos, 0u);             /* Initialize the task's stack         */
        err = MT_TCBInit(prio, psp, (MT_STK *)0, 0u, 0u, (void *)0, 0u);
        if (err == MT_ERR_NONE) {
            if (OSRunning == MT_TRUE) {      /* Find highest priority task if multitasking has started */
                MT_Sched();
            }
        } else {
            MT_ENTER_CRITICAL();
            OSTCBPrioTbl[prio] = (MT_TCB *)0;/* Make this priority available to others                 */
            MT_EXIT_CRITICAL();
        }
        return (err);
    }
    MT_EXIT_CRITICAL();
    return (MT_ERR_PRIO_EXIST);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                  CREATE A TASK (Extended Version)
*
* Description: This function is used to have MTK manage the execution of a task.  Tasks can either
*              be created prior to the start of multitasking or by a running task.  A task cannot be
*              created by an ISR.  This function is similar to OSTaskCreate() except that it allows
*              additional information about a task to be specified.
*
* Arguments  : task      is a pointer to the task's code
*
*              p_arg     is a pointer to an optional data area which can be used to pass parameters to
*                        the task when the task first executes.  Where the task is concerned it thinks
*                        it was invoked and passed the argument 'p_arg' as follows:
*
*                            void Task (void *p_arg)
*                            {
*                                for (;;) {
*                                    Task code;
*                                }
*                            }
*
*              ptos      is a pointer to the task's top of stack.  If the configuration constant
*                        MT_STK_GROWTH is set to 1, the stack is assumed to grow downward (i.e. from high
*                        memory to low memory).  'ptos' will thus point to the highest (valid) memory
*                        location of the stack.  If MT_STK_GROWTH is set to 0, 'ptos' will point to the
*                        lowest memory location of the stack and the stack will grow with increasing
*                        memory locations.  'ptos' MUST point to a valid 'free' data item.
*
*              prio      is the task's priority.  A unique priority MUST be assigned to each task and the
*                        lower the number, the higher the priority.
*
*              id        is the task's ID (0..65535)
*
*              pbos      is a pointer to the task's bottom of stack.  If the configuration constant
*                        MT_STK_GROWTH is set to 1, the stack is assumed to grow downward (i.e. from high
*                        memory to low memory).  'pbos' will thus point to the LOWEST (valid) memory
*                        location of the stack.  If MT_STK_GROWTH is set to 0, 'pbos' will point to the
*                        HIGHEST memory location of the stack and the stack will grow with increasing
*                        memory locations.  'pbos' MUST point to a valid 'free' data item.
*
*              stk_size  is the size of the stack in number of elements.  If MT_STK is set to INT8U,
*                        'stk_size' corresponds to the number of bytes available.  If MT_STK is set to
*                        INT16U, 'stk_size' contains the number of 16-bit entries available.  Finally, if
*                        MT_STK is set to INT32U, 'stk_size' contains the number of 32-bit entries
*                        available on the stack.
*
*              pext      is a pointer to a user supplied memory location which is used as a TCB extension.
*                        For example, this user memory can hold the contents of floating-point registers
*                        during a context switch, the time each task takes to execute, the number of times
*                        the task has been switched-in, etc.
*
*              opt       contains additional information (or options) about the behavior of the task.  The
*                        LOWER 8-bits are reserved by MTK while the upper 8 bits can be application
*                        specific.  See MT_TASK_OPT_??? in MT.H.  Current choices are:
*
*                        MT_TASK_OPT_STK_CHK      Stack checking to be allowed for the task
*                        MT_TASK_OPT_STK_CLR      Clear the stack when the task is created
*                        MT_TASK_OPT_SAVE_FP      If the CPU has floating-point registers, save them
*                                                 during a context switch.
*
* Returns    : MT_ERR_NONE                      if the function was successful.
*              MT_ERR_PRIO_EXIST                if the task priority already exist
*                                               (each task MUST have a unique priority).
*              MT_ERR_PRIO_INVALID              if the priority you specify is higher that the maximum
*                                               allowed (i.e. > MT_LOWEST_PRIO)
*              MT_ERR_TASK_CREATE_ISR           if you tried to create a task from an ISR.
*              MT_ERR_ILLEGAL_CREATE_RUN_TIME   if you tried to create a task after safety critical
*                                               operation started.
*********************************************************************************************************
*/
/*$PAGE*/
#if MT_TASK_CREATE_EXT_EN > 0u
INT8U  OSTaskCreateExt (void   (*task)(void *p_arg),
                        void    *p_arg,
                        MT_STK  *ptos,
                        INT8U    prio,
                        INT16U   id,
                        MT_STK  *pbos,
                        INT32U   stk_size,
                        void    *pext,
                        INT16U   opt)
{
    MT_STK     *psp;
    INT8U       err;
#if MT_CRITICAL_METHOD == 3u                 /* Allocate storage for CPU status register               */
    MT_CPU_SR   cpu_sr = 0u;
#endif



#ifdef MT_SAFETY_CRITICAL_IEC61508
    if (OSSafetyCriticalStartFlag == MT_TRUE) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return (MT_ERR_ILLEGAL_CREATE_RUN_TIME);
    }
#endif

#if MT_ARG_CHK_EN > 0u
    if (prio > MT_LOWEST_PRIO) {             /* Make sure priority is within allowable range           */
        return (MT_ERR_PRIO_INVALID);
    }
#endif
    MT_ENTER_CRITICAL();
    if (OSIntNesting > 0u) {                 /* Make sure we don't create the task from within an ISR  */
        MT_EXIT_CRITICAL();
        return (MT_ERR_TASK_CREATE_ISR);
    }
    if (OSTCBPrioTbl[prio] == (MT_TCB *)0) { /* Make sure task doesn't already exist at this priority  */
        OSTCBPrioTbl[prio] = MT_TCB_RESERVED;/* Reserve the priority to prevent others from doing ...  */
                                             /* ... the same thing until task is created.              */
        MT_EXIT_CRITICAL();

#if (MT_TASK_STAT_STK_CHK_EN > 0u)
        MT_TaskStkClr(pbos, stk_size, opt);                    /* Clear the task stack (if needed)     */
#endif

        psp = OSTaskStkInit(task, p_arg, ptos, opt);           /* Initialize the task's stack          */
        err = MT_TCBInit(prio, psp, pbos, id, stk_size, pext, opt);
        if (err == MT_ERR_NONE) {
            if (OSRunning == MT_TRUE) {                        /* Find HPT if multitasking has started */
                MT_Sched();
            }
        } else {
            MT_ENTER_CRITICAL();
            OSTCBPrioTbl[prio] = (MT_TCB *)0;                  /* Make this priority avail. to others  */
            MT_EXIT_CRITICAL();
        }
        return (err);
    }
    MT_EXIT_CRITICAL();
    return (MT_ERR_PRIO_EXIST);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                            DELETE A TASK
*
* Description: This function allows you to delete a task.  The calling task can delete itself by
*              its own priority number.  The deleted task is returned to the dormant state and can be
*              re-activated by creating the deleted task again.
*
* Arguments  : prio    is the priority of the task to delete.  Note that you can explicitly delete
*                      the current task without knowing its priority level by setting 'prio' to
*                      MT_PRIO_SELF.
*
* Returns    : MT_ERR_NONE             if the call is successful
*              MT_ERR_TASK_DEL_IDLE    if you attempted to delete MTK's idle task
*              MT_ERR_PRIO_INVALID     if the priority you specify is higher that the maximum allowed
*                                      (i.e. >= MT_LOWEST_PRIO) or, you have not specified MT_PRIO_SELF.
*              MT_ERR_TASK_DEL         if the task is assigned to a Mutex PIP.
*              MT_ERR_TASK_NOT_EXIST   if the task you want to delete does not exist.
*              MT_ERR_TASK_DEL_ISR     if you tried to delete a task from an ISR
*
* Notes      : 1) To reduce interrupt latency, OSTaskDel() 'disables' the task:
*                    a) by making it not ready
*                    b) by removing it from any wait lists
*                    c) by preventing OSTimeTick() from making the task ready to run.
*                 The task can then be 'unlinked' from the miscellaneous structures in MTK.
*              2) The function MT_Dummy() is called after MT_EXIT_CRITICAL() because, on most processors,
*                 the next instruction following the enable interrupt instruction is ignored.
*              3) An ISR cannot delete a task.
*              4) The lock nesting counter is incremented because, for a brief instant, if the current
*                 task is being deleted, the current task would not be able to be rescheduled because it
*                 is removed from the ready list.  Incrementing the nesting counter prevents another task
*                 from being schedule.  This means that an ISR would return to the current task which is
*                 being deleted.  The rest of the deletion would thus be able to be completed.
*********************************************************************************************************
*/

#if MT_TASK_DEL_EN > 0u
INT8U  OSTaskDel (INT8U prio)
{
#if (MT_FLAG_EN > 0u) && (MT_MAX_FLAGS > 0u)
    MT_FLAG_NODE *pnode;
#endif
    MT_TCB       *ptcb;
#if MT_CRITICAL_METHOD == 3u                            /* Allocate storage for CPU status register    */
    MT_CPU_SR     cpu_sr = 0u;
#endif



    if (OSIntNesting > 0u) {                            /* See if trying to delete from ISR            */
        return (MT_ERR_TASK_DEL_ISR);
    }
    if (prio == MT_TASK_IDLE_PRIO) {                    /* Not allowed to delete idle task             */
        return (MT_ERR_TASK_DEL_IDLE);
    }
#if MT_ARG_CHK_EN > 0u
    if (prio >= MT_LOWEST_PRIO) {                       /* Task priority valid ?                       */
        if (prio != MT_PRIO_SELF) {
            return (MT_ERR_PRIO_INVALID);
        }
    }
#endif

/*$PAGE*/
    MT_ENTER_CRITICAL();
    if (prio == MT_PRIO_SELF) {                         /* See if requesting to delete self            */
        prio = OSTCBCur->OSTCBPrio;                     /* Set priority to delete to current           */
    }
    ptcb = OSTCBPrioTbl[prio];
    if (ptcb == (MT_TCB *)0) {                          /* Task to delete must exist                   */
        MT_EXIT_CRITICAL();
        return (MT_ERR_TASK_NOT_EXIST);
    }
    if (ptcb == MT_TCB_RESERVED) {                      /* Must not be assigned to Mutex               */
        MT_EXIT_CRITICAL();
        return (MT_ERR_TASK_DEL);
    }

    OSRdyTbl[ptcb->OSTCBY] &= (MT_PRIO)~ptcb->OSTCBBitX;
    if (OSRdyTbl[ptcb->OSTCBY] == 0u) {                 /* Make task not ready                         */
        OSRdyGrp           &= (MT_PRIO)~ptcb->OSTCBBitY;
    }

#if (MT_EVENT_EN)
    if (ptcb->OSTCBEventPtr != (MT_EVENT *)0) {
        MT_EventTaskRemove(ptcb, ptcb->OSTCBEventPtr);  /* Remove this task from any event   wait list */
    }
#if (MT_EVENT_MULTI_EN > 0u)
    if (ptcb->OSTCBEventMultiPtr != (MT_EVENT **)0) {   /* Remove this task from any events' wait lists*/
        MT_EventTaskRemoveMulti(ptcb, ptcb->OSTCBEventMultiPtr);
    }
#endif
#endif

#if (MT_FLAG_EN > 0u) && (MT_MAX_FLAGS > 0u)
    pnode = ptcb->OSTCBFlagNode;
    if (pnode != (MT_FLAG_NODE *)0) {                   /* If task is waiting on event flag            */
        MT_FlagUnlink(pnode);                           /* Remove from wait list                       */
    }
#endif

    ptcb->OSTCBDly      = 0u;                           /* Prevent OSTimeTick() from updating          */
    ptcb->OSTCBStat     = MT_STAT_RDY;                  /* Prevent task from being resumed             */
    ptcb->OSTCBStatPend = MT_STAT_PEND_OK;
    if (OSLockNesting < 255u) {                         /* Make sure we don't context switch           */
        OSLockNesting++;
    }
    MT_EXIT_CRITICAL();                                 /* Enabling INT. ignores next instruc.         */
    MT_Dummy();                                         /* ... Dummy ensures that INTs will be         */
    MT_ENTER_CRITICAL();                                /* ... disabled HERE!                          */
    if (OSLockNesting > 0u) {                           /* Remove context switch lock                  */
        OSLockNesting--;
    }
    OSTaskDelHook(ptcb);                                /* Call user defined hook                      */

#if MT_TASK_CREATE_EXT_EN > 0u
#if defined(MT_TLS_TBL_SIZE) && (MT_TLS_TBL_SIZE > 0u)
    MT_TLS_TaskDel(ptcb);                               /* Call TLS hook                               */
#endif
#endif

    OSTaskCtr--;                                        /* One less task being managed                 */
    OSTCBPrioTbl[prio] = (MT_TCB *)0;                   /* Clear old priority entry                    */
    if (ptcb->OSTCBPrev == (MT_TCB *)0) {               /* Remove from TCB chain                       */
        ptcb->OSTCBNext->OSTCBPrev = (MT_TCB *)0;
        OSTCBList                  = ptcb->OSTCBNext;
    } else {
        ptcb->OSTCBPrev->OSTCBNext = ptcb->OSTCBNext;
        ptcb->OSTCBNext->OSTCBPrev = ptcb->OSTCBPrev;
    }
    ptcb->OSTCBNext     = OSTCBFreeList;                /* Return TCB to free TCB list                 */
    OSTCBFreeList       = ptcb;
#if MT_TASK_NAME_EN > 0u
    ptcb->OSTCBTaskName = (INT8U *)(void *)"?";
#endif
    MT_EXIT_CRITICAL();
    if (OSRunning == MT_TRUE) {
        MT_Sched();                                     /* Find new highest priority task              */
    }
    return (MT_ERR_NONE);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                  REQUEST THAT A TASK DELETE ITSELF
*
* Description: This function is used to:
*                   a) notify a task to delete itself.
*                   b) to see if a task requested that the current task delete itself.
*              This function is a little tricky to understand.  Basically, you have a task that needs
*              to be deleted however, this task has resources that it has allocated (memory buffers,
*              semaphores, mailboxes, queues etc.).  The task cannot be deleted otherwise these
*              resources would not be freed.  The requesting task calls OSTaskDelReq() to indicate that
*              the task needs to be deleted.  Deleting of the task is however, deferred to the task to
*              be deleted.  For example, suppose that task #10 needs to be deleted.  The requesting task
*              example, task #5, would call OSTaskDelReq(10).  When task #10 gets to execute, it calls
*              this function by specifying MT_PRIO_SELF and monitors the returned value.  If the return
*              value is MT_ERR_TASK_DEL_REQ, another task requested a task delete.  Task #10 would look like
*              this:
*
*                   void Task(void *p_arg)
*                   {
*                       .
*                       .
*                       while (1) {
*                           OSTimeDly(1);
*                           if (OSTaskDelReq(MT_PRIO_SELF) == MT_ERR_TASK_DEL_REQ) {
*                               Release any owned resources;
*                               De-allocate any dynamic memory;
*                               OSTaskDel(MT_PRIO_SELF);
*                           }
*                       }
*                   }
*
* Arguments  : prio    is the priority of the task to request the delete from
*
* Returns    : MT_ERR_NONE            if the task exist and the request has been registered
*              MT_ERR_TASK_NOT_EXIST  if the task has been deleted.  This allows the caller to know whether
*                                     the request has been executed.
*              MT_ERR_TASK_DEL        if the task is assigned to a Mutex.
*              MT_ERR_TASK_DEL_IDLE   if you requested to delete MTK's idle task
*              MT_ERR_PRIO_INVALID    if the priority you specify is higher that the maximum allowed
*                                     (i.e. >= MT_LOWEST_PRIO) or, you have not specified MT_PRIO_SELF.
*              MT_ERR_TASK_DEL_REQ    if a task (possibly another task) requested that the running task be
*                                     deleted.
*********************************************************************************************************
*/
/*$PAGE*/
#if MT_TASK_DEL_EN > 0u
INT8U  OSTaskDelReq (INT8U prio)
{
    INT8U      stat;
    MT_TCB    *ptcb;
#if MT_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    MT_CPU_SR  cpu_sr = 0u;
#endif



    if (prio == MT_TASK_IDLE_PRIO) {                            /* Not allowed to delete idle task     */
        return (MT_ERR_TASK_DEL_IDLE);
    }
#if MT_ARG_CHK_EN > 0u
    if (prio >= MT_LOWEST_PRIO) {                               /* Task priority valid ?               */
        if (prio != MT_PRIO_SELF) {
            return (MT_ERR_PRIO_INVALID);
        }
    }
#endif
    if (prio == MT_PRIO_SELF) {                                 /* See if a task is requesting to ...  */
        MT_ENTER_CRITICAL();                                    /* ... this task to delete itself      */
        stat = OSTCBCur->OSTCBDelReq;                           /* Return request status to caller     */
        MT_EXIT_CRITICAL();
        return (stat);
    }
    MT_ENTER_CRITICAL();
    ptcb = OSTCBPrioTbl[prio];
    if (ptcb == (MT_TCB *)0) {                                  /* Task to delete must exist           */
        MT_EXIT_CRITICAL();
        return (MT_ERR_TASK_NOT_EXIST);                         /* Task must already be deleted        */
    }
    if (ptcb == MT_TCB_RESERVED) {                              /* Must NOT be assigned to a Mutex     */
        MT_EXIT_CRITICAL();
        return (MT_ERR_TASK_DEL);
    }
    ptcb->OSTCBDelReq = MT_ERR_TASK_DEL_REQ;                    /* Set flag indicating task to be DEL. */
    MT_EXIT_CRITICAL();
    return (MT_ERR_NONE);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                       GET THE NAME OF A TASK
*
* Description: This function is called to obtain the name of a task.
*
* Arguments  : prio      is the priority of the task that you want to obtain the name from.
*
*              pname     is a pointer to a pointer to an ASCII string that will receive the name of the task.
*
*              perr      is a pointer to an error code that can contain one of the following values:
*
*                        MT_ERR_NONE                if the requested task is resumed
*                        MT_ERR_TASK_NOT_EXIST      if the task has not been created or is assigned to a Mutex
*                        MT_ERR_PRIO_INVALID        if you specified an invalid priority:
*                                                   A higher value than the idle task or not MT_PRIO_SELF.
*                        MT_ERR_PNAME_NULL          You passed a NULL pointer for 'pname'
*                        MT_ERR_NAME_GET_ISR        You called this function from an ISR
*
*
* Returns    : The length of the string or 0 if the task does not exist.
*********************************************************************************************************
*/

#if MT_TASK_NAME_EN > 0u
INT8U  OSTaskNameGet (INT8U    prio,
                      INT8U  **pname,
                      INT8U   *perr)
{
    MT_TCB    *ptcb;
    INT8U      len;
#if MT_CRITICAL_METHOD == 3u                             /* Allocate storage for CPU status register   */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#ifdef MT_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return (0u);
    }
#endif

#if MT_ARG_CHK_EN > 0u
    if (prio > MT_LOWEST_PRIO) {                         /* Task priority valid ?                      */
        if (prio != MT_PRIO_SELF) {
            *perr = MT_ERR_PRIO_INVALID;                 /* No                                         */
            return (0u);
        }
    }
    if (pname == (INT8U **)0) {                          /* Is 'pname' a NULL pointer?                 */
        *perr = MT_ERR_PNAME_NULL;                       /* Yes                                        */
        return (0u);
    }
#endif
    if (OSIntNesting > 0u) {                              /* See if trying to call from an ISR          */
        *perr = MT_ERR_NAME_GET_ISR;
        return (0u);
    }
    MT_ENTER_CRITICAL();
    if (prio == MT_PRIO_SELF) {                          /* See if caller desires it's own name        */
        prio = OSTCBCur->OSTCBPrio;
    }
    ptcb = OSTCBPrioTbl[prio];
    if (ptcb == (MT_TCB *)0) {                           /* Does task exist?                           */
        MT_EXIT_CRITICAL();                              /* No                                         */
        *perr = MT_ERR_TASK_NOT_EXIST;
        return (0u);
    }
    if (ptcb == MT_TCB_RESERVED) {                       /* Task assigned to a Mutex?                  */
        MT_EXIT_CRITICAL();                              /* Yes                                        */
        *perr = MT_ERR_TASK_NOT_EXIST;
        return (0u);
    }
    *pname = ptcb->OSTCBTaskName;
    len    = MT_StrLen(*pname);
    MT_EXIT_CRITICAL();
    *perr  = MT_ERR_NONE;
    return (len);
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                       ASSIGN A NAME TO A TASK
*
* Description: This function is used to set the name of a task.
*
* Arguments  : prio      is the priority of the task that you want the assign a name to.
*
*              pname     is a pointer to an ASCII string that contains the name of the task.
*
*              perr       is a pointer to an error code that can contain one of the following values:
*
*                        MT_ERR_NONE                if the requested task is resumed
*                        MT_ERR_TASK_NOT_EXIST      if the task has not been created or is assigned to a Mutex
*                        MT_ERR_PNAME_NULL          You passed a NULL pointer for 'pname'
*                        MT_ERR_PRIO_INVALID        if you specified an invalid priority:
*                                                   A higher value than the idle task or not MT_PRIO_SELF.
*                        MT_ERR_NAME_SET_ISR        if you called this function from an ISR
*
* Returns    : None
*********************************************************************************************************
*/
#if MT_TASK_NAME_EN > 0u
void  OSTaskNameSet (INT8U   prio,
                     INT8U  *pname,
                     INT8U  *perr)
{
    MT_TCB    *ptcb;
#if MT_CRITICAL_METHOD == 3u                         /* Allocate storage for CPU status register       */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#ifdef MT_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if MT_ARG_CHK_EN > 0u
    if (prio > MT_LOWEST_PRIO) {                     /* Task priority valid ?                          */
        if (prio != MT_PRIO_SELF) {
            *perr = MT_ERR_PRIO_INVALID;             /* No                                             */
            return;
        }
    }
    if (pname == (INT8U *)0) {                       /* Is 'pname' a NULL pointer?                     */
        *perr = MT_ERR_PNAME_NULL;                   /* Yes                                            */
        return;
    }
#endif
    if (OSIntNesting > 0u) {                         /* See if trying to call from an ISR              */
        *perr = MT_ERR_NAME_SET_ISR;
        return;
    }
    MT_ENTER_CRITICAL();
    if (prio == MT_PRIO_SELF) {                      /* See if caller desires to set it's own name     */
        prio = OSTCBCur->OSTCBPrio;
    }
    ptcb = OSTCBPrioTbl[prio];
    if (ptcb == (MT_TCB *)0) {                       /* Does task exist?                               */
        MT_EXIT_CRITICAL();                          /* No                                             */
        *perr = MT_ERR_TASK_NOT_EXIST;
        return;
    }
    if (ptcb == MT_TCB_RESERVED) {                   /* Task assigned to a Mutex?                      */
        MT_EXIT_CRITICAL();                          /* Yes                                            */
        *perr = MT_ERR_TASK_NOT_EXIST;
        return;
    }
    ptcb->OSTCBTaskName = pname;
    MT_EXIT_CRITICAL();
    *perr               = MT_ERR_NONE;
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                       RESUME A SUSPENDED TASK
*
* Description: This function is called to resume a previously suspended task.  This is the only call that
*              will remove an explicit task suspension.
*
* Arguments  : prio     is the priority of the task to resume.
*
* Returns    : MT_ERR_NONE                if the requested task is resumed
*              MT_ERR_PRIO_INVALID        if the priority you specify is higher that the maximum allowed
*                                         (i.e. >= MT_LOWEST_PRIO)
*              MT_ERR_TASK_RESUME_PRIO    if the task to resume does not exist
*              MT_ERR_TASK_NOT_EXIST      if the task is assigned to a Mutex PIP
*              MT_ERR_TASK_NOT_SUSPENDED  if the task to resume has not been suspended
*********************************************************************************************************
*/

#if MT_TASK_SUSPEND_EN > 0u
INT8U  OSTaskResume (INT8U prio)
{
    MT_TCB    *ptcb;
#if MT_CRITICAL_METHOD == 3u                                  /* Storage for CPU status register       */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#if MT_ARG_CHK_EN > 0u
    if (prio >= MT_LOWEST_PRIO) {                             /* Make sure task priority is valid      */
        return (MT_ERR_PRIO_INVALID);
    }
#endif
    MT_ENTER_CRITICAL();
    ptcb = OSTCBPrioTbl[prio];
    if (ptcb == (MT_TCB *)0) {                                /* Task to suspend must exist            */
        MT_EXIT_CRITICAL();
        return (MT_ERR_TASK_RESUME_PRIO);
    }
    if (ptcb == MT_TCB_RESERVED) {                            /* See if assigned to Mutex              */
        MT_EXIT_CRITICAL();
        return (MT_ERR_TASK_NOT_EXIST);
    }
    if ((ptcb->OSTCBStat & MT_STAT_SUSPEND) != MT_STAT_RDY) { /* Task must be suspended                */
        ptcb->OSTCBStat &= (INT8U)~(INT8U)MT_STAT_SUSPEND;    /* Remove suspension                     */
        if ((ptcb->OSTCBStat & MT_STAT_PEND_ANY) == MT_STAT_RDY) { /* See if task is now ready         */
            if (ptcb->OSTCBDly == 0u) {
                OSRdyGrp               |= ptcb->OSTCBBitY;    /* Yes, Make task ready to run           */
                OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
                MT_EXIT_CRITICAL();
                if (OSRunning == MT_TRUE) {
                    MT_Sched();                               /* Find new highest priority task        */
                }
            } else {
                MT_EXIT_CRITICAL();
            }
        } else {                                              /* Must be pending on event              */
            MT_EXIT_CRITICAL();
        }
        return (MT_ERR_NONE);
    }
    MT_EXIT_CRITICAL();
    return (MT_ERR_TASK_NOT_SUSPENDED);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                           STACK CHECKING
*
* Description: This function is called to check the amount of free memory left on the specified task's
*              stack.
*
* Arguments  : prio          is the task priority
*
*              p_stk_data    is a pointer to a data structure of type MT_STK_DATA.
*
* Returns    : MT_ERR_NONE            upon success
*              MT_ERR_PRIO_INVALID    if the priority you specify is higher that the maximum allowed
*                                     (i.e. > MT_LOWEST_PRIO) or, you have not specified MT_PRIO_SELF.
*              MT_ERR_TASK_NOT_EXIST  if the desired task has not been created or is assigned to a Mutex PIP
*              MT_ERR_TASK_OPT        if you did NOT specified MT_TASK_OPT_STK_CHK when the task was created
*              MT_ERR_PDATA_NULL      if 'p_stk_data' is a NULL pointer
*********************************************************************************************************
*/
#if (MT_TASK_STAT_STK_CHK_EN > 0u) && (MT_TASK_CREATE_EXT_EN > 0u)
INT8U  OSTaskStkChk (INT8U         prio,
                     MT_STK_DATA  *p_stk_data)
{
    MT_TCB    *ptcb;
    MT_STK    *pchk;
    INT32U     nfree;
    INT32U     size;
#if MT_CRITICAL_METHOD == 3u                           /* Allocate storage for CPU status register     */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#if MT_ARG_CHK_EN > 0u
    if (prio > MT_LOWEST_PRIO) {                       /* Make sure task priority is valid             */
        if (prio != MT_PRIO_SELF) {
            return (MT_ERR_PRIO_INVALID);
        }
    }
    if (p_stk_data == (MT_STK_DATA *)0) {              /* Validate 'p_stk_data'                        */
        return (MT_ERR_PDATA_NULL);
    }
#endif
    p_stk_data->OSFree = 0u;                           /* Assume failure, set to 0 size                */
    p_stk_data->OSUsed = 0u;
    MT_ENTER_CRITICAL();
    if (prio == MT_PRIO_SELF) {                        /* See if check for SELF                        */
        prio = OSTCBCur->OSTCBPrio;
    }
    ptcb = OSTCBPrioTbl[prio];
    if (ptcb == (MT_TCB *)0) {                         /* Make sure task exist                         */
        MT_EXIT_CRITICAL();
        return (MT_ERR_TASK_NOT_EXIST);
    }
    if (ptcb == MT_TCB_RESERVED) {
        MT_EXIT_CRITICAL();
        return (MT_ERR_TASK_NOT_EXIST);
    }
    if ((ptcb->OSTCBOpt & MT_TASK_OPT_STK_CHK) == 0u) { /* Make sure stack checking option is set      */
        MT_EXIT_CRITICAL();
        return (MT_ERR_TASK_OPT);
    }
    nfree = 0u;
    size  = ptcb->OSTCBStkSize;
    pchk  = ptcb->OSTCBStkBottom;
    MT_EXIT_CRITICAL();
#if MT_STK_GROWTH == 1u
    while (*pchk++ == (MT_STK)0) {                    /* Compute the number of zero entries on the stk */
        nfree++;
    }
#else
    while (*pchk-- == (MT_STK)0) {
        nfree++;
    }
#endif
    p_stk_data->OSFree = nfree;                       /* Store   number of free entries on the stk     */
    p_stk_data->OSUsed = size - nfree;                /* Compute number of entries used on the stk     */
    return (MT_ERR_NONE);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                           SUSPEND A TASK
*
* Description: This function is called to suspend a task.  The task can be the calling task if the
*              priority passed to OSTaskSuspend() is the priority of the calling task or MT_PRIO_SELF.
*
* Arguments  : prio     is the priority of the task to suspend.  If you specify MT_PRIO_SELF, the
*                       calling task will suspend itself and rescheduling will occur.
*
* Returns    : MT_ERR_NONE               if the requested task is suspended
*              MT_ERR_TASK_SUSPEND_IDLE  if you attempted to suspend the idle task which is not allowed.
*              MT_ERR_PRIO_INVALID       if the priority you specify is higher that the maximum allowed
*                                        (i.e. >= MT_LOWEST_PRIO) or, you have not specified MT_PRIO_SELF.
*              MT_ERR_TASK_SUSPEND_PRIO  if the task to suspend does not exist
*              MT_ERR_TASK_NOT_EXITS     if the task is assigned to a Mutex PIP
*
* Note       : You should use this function with great care.  If you suspend a task that is waiting for
*              an event (i.e. a message, a semaphore, a queue ...) you will prevent this task from
*              running when the event arrives.
*********************************************************************************************************
*/

#if MT_TASK_SUSPEND_EN > 0u
INT8U  OSTaskSuspend (INT8U prio)
{
    BOOLEAN    self;
    MT_TCB    *ptcb;
    INT8U      y;
#if MT_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#if MT_ARG_CHK_EN > 0u
    if (prio == MT_TASK_IDLE_PRIO) {                            /* Not allowed to suspend idle task    */
        return (MT_ERR_TASK_SUSPEND_IDLE);
    }
    if (prio >= MT_LOWEST_PRIO) {                               /* Task priority valid ?               */
        if (prio != MT_PRIO_SELF) {
            return (MT_ERR_PRIO_INVALID);
        }
    }
#endif
    MT_ENTER_CRITICAL();
    if (prio == MT_PRIO_SELF) {                                 /* See if suspend SELF                 */
        prio = OSTCBCur->OSTCBPrio;
        self = MT_TRUE;
    } else if (prio == OSTCBCur->OSTCBPrio) {                   /* See if suspending self              */
        self = MT_TRUE;
    } else {
        self = MT_FALSE;                                        /* No suspending another task          */
    }
    ptcb = OSTCBPrioTbl[prio];
    if (ptcb == (MT_TCB *)0) {                                  /* Task to suspend must exist          */
        MT_EXIT_CRITICAL();
        return (MT_ERR_TASK_SUSPEND_PRIO);
    }
    if (ptcb == MT_TCB_RESERVED) {                              /* See if assigned to Mutex            */
        MT_EXIT_CRITICAL();
        return (MT_ERR_TASK_NOT_EXIST);
    }
    y            = ptcb->OSTCBY;
    OSRdyTbl[y] &= (MT_PRIO)~ptcb->OSTCBBitX;                   /* Make task not ready                 */
    if (OSRdyTbl[y] == 0u) {
        OSRdyGrp &= (MT_PRIO)~ptcb->OSTCBBitY;
    }
    ptcb->OSTCBStat |= MT_STAT_SUSPEND;                         /* Status of task is 'SUSPENDED'       */
    MT_EXIT_CRITICAL();
    if (self == MT_TRUE) {                                      /* Context switch only if SELF         */
        MT_Sched();                                             /* Find new highest priority task      */
    }
    return (MT_ERR_NONE);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                            QUERY A TASK
*
* Description: This function is called to obtain a copy of the desired task's TCB.
*
* Arguments  : prio         is the priority of the task to obtain information from.
*
*              p_task_data  is a pointer to where the desired task's MT_TCB will be stored.
*
* Returns    : MT_ERR_NONE            if the requested task is suspended
*              MT_ERR_PRIO_INVALID    if the priority you specify is higher that the maximum allowed
*                                     (i.e. > MT_LOWEST_PRIO) or, you have not specified MT_PRIO_SELF.
*              MT_ERR_PRIO            if the desired task has not been created
*              MT_ERR_TASK_NOT_EXIST  if the task is assigned to a Mutex PIP
*              MT_ERR_PDATA_NULL      if 'p_task_data' is a NULL pointer
*********************************************************************************************************
*/

#if MT_TASK_QUERY_EN > 0u
INT8U  OSTaskQuery (INT8U    prio,
                    MT_TCB  *p_task_data)
{
    MT_TCB    *ptcb;
#if MT_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#if MT_ARG_CHK_EN > 0u
    if (prio > MT_LOWEST_PRIO) {                 /* Task priority valid ?                              */
        if (prio != MT_PRIO_SELF) {
            return (MT_ERR_PRIO_INVALID);
        }
    }
    if (p_task_data == (MT_TCB *)0) {            /* Validate 'p_task_data'                             */
        return (MT_ERR_PDATA_NULL);
    }
#endif
    MT_ENTER_CRITICAL();
    if (prio == MT_PRIO_SELF) {                  /* See if suspend SELF                                */
        prio = OSTCBCur->OSTCBPrio;
    }
    ptcb = OSTCBPrioTbl[prio];
    if (ptcb == (MT_TCB *)0) {                   /* Task to query must exist                           */
        MT_EXIT_CRITICAL();
        return (MT_ERR_PRIO);
    }
    if (ptcb == MT_TCB_RESERVED) {               /* Task to query must not be assigned to a Mutex      */
        MT_EXIT_CRITICAL();
        return (MT_ERR_TASK_NOT_EXIST);
    }
                                                 /* Copy TCB into user storage area                    */
    MT_MemCopy((INT8U *)p_task_data, (INT8U *)ptcb, sizeof(MT_TCB));
    MT_EXIT_CRITICAL();
    return (MT_ERR_NONE);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                              GET THE CURRENT VALUE OF A TASK REGISTER
*
* Description: This function is called to obtain the current value of a task register.  Task registers
*              are application specific and can be used to store task specific values such as 'error
*              numbers' (i.e. errno), statistics, etc.  Each task register can hold a 32-bit value.
*
* Arguments  : prio      is the priority of the task you want to get the task register from.  If you
*                        specify MT_PRIO_SELF then the task register of the current task will be obtained.
*
*              id        is the 'id' of the desired task register.  Note that the 'id' must be less
*                        than MT_TASK_REG_TBL_SIZE
*
*              perr      is a pointer to a variable that will hold an error code related to this call.
*
*                        MT_ERR_NONE            if the call was successful
*                        MT_ERR_PRIO_INVALID    if you specified an invalid priority
*                        MT_ERR_ID_INVALID      if the 'id' is not between 0 and MT_TASK_REG_TBL_SIZE-1
*
* Returns    : The current value of the task's register or 0 if an error is detected.
*
* Note(s)    : The maximum number of task variables is 254
*********************************************************************************************************
*/

#if MT_TASK_REG_TBL_SIZE > 0u
INT32U  OSTaskRegGet (INT8U   prio,
                      INT8U   id,
                      INT8U  *perr)
{
#if MT_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    MT_CPU_SR  cpu_sr = 0u;
#endif
    INT32U     value;
    MT_TCB    *ptcb;



#ifdef MT_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return (0u);
    }
#endif

#if MT_ARG_CHK_EN > 0u
    if (prio >= MT_LOWEST_PRIO) {
        if (prio != MT_PRIO_SELF) {
            *perr = MT_ERR_PRIO_INVALID;
            return (0u);
        }
    }
    if (id >= MT_TASK_REG_TBL_SIZE) {
        *perr = MT_ERR_ID_INVALID;
        return (0u);
    }
#endif
    MT_ENTER_CRITICAL();
    if (prio == MT_PRIO_SELF) {                  /* See if need to get register from current task      */
        ptcb = OSTCBCur;
    } else {
        ptcb = OSTCBPrioTbl[prio];
    }
    value = ptcb->OSTCBRegTbl[id];
    MT_EXIT_CRITICAL();
    *perr = MT_ERR_NONE;
    return (value);
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                    ALLOCATE THE NEXT AVAILABLE TASK REGISTER ID
*
* Description: This function is called to obtain a task register ID.  This function thus allows task registers IDs to be
*              allocated dynamically instead of statically.
*
* Arguments  : p_err       is a pointer to a variable that will hold an error code related to this call.
*
*                            MT_ERR_NONE               if the call was successful
*                            MT_ERR_NO_MORE_ID_AVAIL   if you are attempting to assign more task register IDs than you 
*                                                           have available through MT_TASK_REG_TBL_SIZE.
*
* Returns    : The next available task register 'id' or MT_TASK_REG_TBL_SIZE if an error is detected.
************************************************************************************************************************
*/

#if MT_TASK_REG_TBL_SIZE > 0u
INT8U  OSTaskRegGetID (INT8U  *perr)
{
#if MT_CRITICAL_METHOD == 3u                                    /* Allocate storage for CPU status register           */
    MT_CPU_SR  cpu_sr = 0u;
#endif
    INT8U      id;


#ifdef MT_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return ((INT8U)MT_TASK_REG_TBL_SIZE);
    }
#endif

    MT_ENTER_CRITICAL();
    if (OSTaskRegNextAvailID >= MT_TASK_REG_TBL_SIZE) {         /* See if we exceeded the number of IDs available     */
       *perr = MT_ERR_NO_MORE_ID_AVAIL;                         /* Yes, cannot allocate more task register IDs        */
        MT_EXIT_CRITICAL();
        return ((INT8U)MT_TASK_REG_TBL_SIZE);
    }
     
    id   = OSTaskRegNextAvailID;                                /* Assign the next available ID                       */
    OSTaskRegNextAvailID++;                                     /* Increment available ID for next request            */
    MT_EXIT_CRITICAL();
   *perr = MT_ERR_NONE;
    return (id);
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                              SET THE CURRENT VALUE OF A TASK VARIABLE
*
* Description: This function is called to change the current value of a task register.  Task registers
*              are application specific and can be used to store task specific values such as 'error
*              numbers' (i.e. errno), statistics, etc.  Each task register can hold a 32-bit value.
*
* Arguments  : prio      is the priority of the task you want to set the task register for.  If you
*                        specify MT_PRIO_SELF then the task register of the current task will be obtained.
*
*              id        is the 'id' of the desired task register.  Note that the 'id' must be less
*                        than MT_TASK_REG_TBL_SIZE
*
*              value     is the desired value for the task register.
*
*              perr      is a pointer to a variable that will hold an error code related to this call.
*
*                        MT_ERR_NONE            if the call was successful
*                        MT_ERR_PRIO_INVALID    if you specified an invalid priority
*                        MT_ERR_ID_INVALID      if the 'id' is not between 0 and MT_TASK_REG_TBL_SIZE-1
*
* Returns    : The current value of the task's variable or 0 if an error is detected.
*
* Note(s)    : The maximum number of task variables is 254
*********************************************************************************************************
*/

#if MT_TASK_REG_TBL_SIZE > 0u
void  OSTaskRegSet (INT8U    prio,
                    INT8U    id,
                    INT32U   value,
                    INT8U   *perr)
{
#if MT_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    MT_CPU_SR  cpu_sr = 0u;
#endif
    MT_TCB    *ptcb;


#ifdef MT_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if MT_ARG_CHK_EN > 0u
    if (prio >= MT_LOWEST_PRIO) {
        if (prio != MT_PRIO_SELF) {
            *perr = MT_ERR_PRIO_INVALID;
            return;
        }
    }
    if (id >= MT_TASK_REG_TBL_SIZE) {
        *perr = MT_ERR_ID_INVALID;
        return;
    }
#endif
    MT_ENTER_CRITICAL();
    if (prio == MT_PRIO_SELF) {                  /* See if need to get register from current task      */
        ptcb = OSTCBCur;
    } else {
        ptcb = OSTCBPrioTbl[prio];
    }
    ptcb->OSTCBRegTbl[id] = value;
    MT_EXIT_CRITICAL();
    *perr                 = MT_ERR_NONE;
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                    CATCH ACCIDENTAL TASK RETURN
*
* Description: This function is called if a task accidentally returns without deleting itself.  In other
*              words, a task should either be an infinite loop or delete itself if it's done.
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to MTK and your application should not call it.
*********************************************************************************************************
*/

void  MT_TaskReturn (void)
{
    OSTaskReturnHook(OSTCBCur);                   /* Call hook to let user decide on what to do        */

#if MT_TASK_DEL_EN > 0u
    (void)OSTaskDel(MT_PRIO_SELF);                /* Delete task if it accidentally returns!           */
#else
    for (;;) {
        OSTimeDly(MT_TICKS_PER_SEC);
    }
#endif
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                          CLEAR TASK STACK
*
* Description: This function is used to clear the stack of a task (i.e. write all zeros)
*
* Arguments  : pbos     is a pointer to the task's bottom of stack.  If the configuration constant
*                       MT_STK_GROWTH is set to 1, the stack is assumed to grow downward (i.e. from high
*                       memory to low memory).  'pbos' will thus point to the lowest (valid) memory
*                       location of the stack.  If MT_STK_GROWTH is set to 0, 'pbos' will point to the
*                       highest memory location of the stack and the stack will grow with increasing
*                       memory locations.  'pbos' MUST point to a valid 'free' data item.
*
*              size     is the number of 'stack elements' to clear.
*
*              opt      contains additional information (or options) about the behavior of the task.  The
*                       LOWER 8-bits are reserved by MTK while the upper 8 bits can be application
*                       specific.  See MT_TASK_OPT_??? in MT.H.
*
* Returns    : none
*********************************************************************************************************
*/
#if (MT_TASK_STAT_STK_CHK_EN > 0u) && (MT_TASK_CREATE_EXT_EN > 0u)
void  MT_TaskStkClr (MT_STK  *pbos,
                     INT32U   size,
                     INT16U   opt)
{
    if ((opt & MT_TASK_OPT_STK_CHK) != 0x0000u) {      /* See if stack checking has been enabled       */
        if ((opt & MT_TASK_OPT_STK_CLR) != 0x0000u) {  /* See if stack needs to be cleared             */
#if MT_STK_GROWTH == 1u
            while (size > 0u) {                        /* Stack grows from HIGH to LOW memory          */
                size--;
                *pbos++ = (MT_STK)0;                   /* Clear from bottom of stack and up!           */
            }
#else
            while (size > 0u) {                        /* Stack grows from LOW to HIGH memory          */
                size--;
                *pbos-- = (MT_STK)0;                   /* Clear from bottom of stack and down          */
            }
#endif
        }
    }
}

#endif
