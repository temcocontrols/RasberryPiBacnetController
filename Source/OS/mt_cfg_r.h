

#ifndef MT_CFG_H
#define MT_CFG_H


                                       /* ---------------------- MISCELLANEOUS ----------------------- */
#define MT_APP_HOOKS_EN           1u   /* Application-defined hooks are called from the MTK hooks */
#define MT_ARG_CHK_EN             1u   /* Enable (1) or Disable (0) argument checking                  */
#define MT_CPU_HOOKS_EN           1u   /* MTK hooks are found in the processor port files         */

#define MT_DEBUG_EN               1u   /* Enable(1) debug variables                                    */

#define MT_EVENT_MULTI_EN         1u   /* Include code for OSEventPendMulti()                          */
#define MT_EVENT_NAME_EN          1u   /* Enable names for Sem, Mutex, Mbox and Q                      */

#define MT_LOWEST_PRIO           63u   /* Defines the lowest priority that can be assigned ...         */
                                       /* ... MUST NEVER be higher than 254!                           */

#define MT_MAX_EVENTS            10u   /* Max. number of event control blocks in your application      */
#define MT_MAX_FLAGS              5u   /* Max. number of Event Flag Groups    in your application      */
#define MT_MAX_MEM_PART           5u   /* Max. number of memory partitions                             */
#define MT_MAX_QS                 4u   /* Max. number of queue control blocks in your application      */
#define MT_MAX_TASKS             20u   /* Max. number of tasks in your application, MUST be >= 2       */

#define MT_SCHED_LOCK_EN          1u   /* Include code for OSSchedLock() and OSSchedUnlock()           */

#define MT_TICK_STEP_EN           1u   /* Enable tick stepping feature for MT-View                  */
#define MT_TICKS_PER_SEC        100u   /* Set the number of ticks in one second                        */

#define MT_TLS_TBL_SIZE           5u   /* Size of Thread-Local Storage Table                           */


                                       /* --------------------- TASK STACK SIZE ---------------------- */
#define MT_TASK_TMR_STK_SIZE    128u   /* Timer      task stack size (# of MT_STK wide entries)        */
#define MT_TASK_STAT_STK_SIZE   128u   /* Statistics task stack size (# of MT_STK wide entries)        */
#define MT_TASK_IDLE_STK_SIZE   128u   /* Idle       task stack size (# of MT_STK wide entries)        */


                                       /* --------------------- TASK MANAGEMENT ---------------------- */
#define MT_TASK_CHANGE_PRIO_EN    1u   /*     Include code for OSTaskChangePrio()                      */
#define MT_TASK_CREATE_EN         1u   /*     Include code for OSTaskCreate()                          */
#define MT_TASK_CREATE_EXT_EN     1u   /*     Include code for OSTaskCreateExt()                       */
#define MT_TASK_DEL_EN            1u   /*     Include code for OSTaskDel()                             */
#define MT_TASK_NAME_EN           1u   /*     Enable task names                                        */
#define MT_TASK_PROFILE_EN        1u   /*     Include variables in MT_TCB for profiling                */
#define MT_TASK_QUERY_EN          1u   /*     Include code for OSTaskQuery()                           */
#define MT_TASK_REG_TBL_SIZE      1u   /*     Size of task variables array (#of INT32U entries)        */
#define MT_TASK_STAT_EN           1u   /*     Enable (1) or Disable(0) the statistics task             */
#define MT_TASK_STAT_STK_CHK_EN   1u   /*     Check task stacks from statistic task                    */
#define MT_TASK_SUSPEND_EN        1u   /*     Include code for OSTaskSuspend() and OSTaskResume()      */
#define MT_TASK_SW_HOOK_EN        1u   /*     Include code for OSTaskSwHook()                          */


                                       /* ----------------------- EVENT FLAGS ------------------------ */
#define MT_FLAG_EN                1u   /* Enable (1) or Disable (0) code generation for EVENT FLAGS    */
#define MT_FLAG_ACCEPT_EN         1u   /*     Include code for OSFlagAccept()                          */
#define MT_FLAG_DEL_EN            1u   /*     Include code for OSFlagDel()                             */
#define MT_FLAG_NAME_EN           1u   /*     Enable names for event flag group                        */
#define MT_FLAG_QUERY_EN          1u   /*     Include code for OSFlagQuery()                           */
#define MT_FLAG_WAIT_CLR_EN       1u   /* Include code for Wait on Clear EVENT FLAGS                   */
#define MT_FLAGS_NBITS           16u   /* Size in #bits of MT_FLAGS data type (8, 16 or 32)            */


                                       /* -------------------- MESSAGE MAILBOXES --------------------- */
#define MT_MBOX_EN                1u   /* Enable (1) or Disable (0) code generation for MAILBOXES      */
#define MT_MBOX_ACCEPT_EN         1u   /*     Include code for OSMboxAccept()                          */
#define MT_MBOX_DEL_EN            1u   /*     Include code for OSMboxDel()                             */
#define MT_MBOX_PEND_ABORT_EN     1u   /*     Include code for OSMboxPendAbort()                       */
#define MT_MBOX_POST_EN           1u   /*     Include code for OSMboxPost()                            */
#define MT_MBOX_POST_OPT_EN       1u   /*     Include code for OSMboxPostOpt()                         */
#define MT_MBOX_QUERY_EN          1u   /*     Include code for OSMboxQuery()                           */


                                       /* --------------------- MEMORY MANAGEMENT -------------------- */
#define MT_MEM_EN                 1u   /* Enable (1) or Disable (0) code generation for MEMORY MANAGER */
#define MT_MEM_NAME_EN            1u   /*     Enable memory partition names                            */
#define MT_MEM_QUERY_EN           1u   /*     Include code for OSMemQuery()                            */


                                       /* ---------------- MUTUAL EXCLUSION SEMAPHORES --------------- */
#define MT_MUTEX_EN               1u   /* Enable (1) or Disable (0) code generation for MUTEX          */
#define MT_MUTEX_ACCEPT_EN        1u   /*     Include code for OSMutexAccept()                         */
#define MT_MUTEX_DEL_EN           1u   /*     Include code for OSMutexDel()                            */
#define MT_MUTEX_QUERY_EN         1u   /*     Include code for OSMutexQuery()                          */


                                       /* ---------------------- MESSAGE QUEUES ---------------------- */
#define MT_Q_EN                   1u   /* Enable (1) or Disable (0) code generation for QUEUES         */
#define MT_Q_ACCEPT_EN            1u   /*     Include code for OSQAccept()                             */
#define MT_Q_DEL_EN               1u   /*     Include code for OSQDel()                                */
#define MT_Q_FLUSH_EN             1u   /*     Include code for OSQFlush()                              */
#define MT_Q_PEND_ABORT_EN        1u   /*     Include code for OSQPendAbort()                          */
#define MT_Q_POST_EN              1u   /*     Include code for OSQPost()                               */
#define MT_Q_POST_FRONT_EN        1u   /*     Include code for OSQPostFront()                          */
#define MT_Q_POST_OPT_EN          1u   /*     Include code for OSQPostOpt()                            */
#define MT_Q_QUERY_EN             1u   /*     Include code for OSQQuery()                              */


                                       /* ------------------------ SEMAPHORES ------------------------ */
#define MT_SEM_EN                 1u   /* Enable (1) or Disable (0) code generation for SEMAPHORES     */
#define MT_SEM_ACCEPT_EN          1u   /*    Include code for OSSemAccept()                            */
#define MT_SEM_DEL_EN             1u   /*    Include code for OSSemDel()                               */
#define MT_SEM_PEND_ABORT_EN      1u   /*    Include code for OSSemPendAbort()                         */
#define MT_SEM_QUERY_EN           1u   /*    Include code for OSSemQuery()                             */
#define MT_SEM_SET_EN             1u   /*    Include code for OSSemSet()                               */


                                       /* --------------------- TIME MANAGEMENT ---------------------- */
#define MT_TIME_DLY_HMSM_EN       1u   /*     Include code for OSTimeDlyHMSM()                         */
#define MT_TIME_DLY_RESUME_EN     1u   /*     Include code for OSTimeDlyResume()                       */
#define MT_TIME_GET_SET_EN        1u   /*     Include code for OSTimeGet() and OSTimeSet()             */
#define MT_TIME_TICK_HOOK_EN      1u   /*     Include code for OSTimeTickHook()                        */


                                       /* --------------------- TIMER MANAGEMENT --------------------- */
#define MT_TMR_EN                 1u   /* Enable (1) or Disable (0) code generation for TIMERS         */
#define MT_TMR_CFG_MAX           16u   /*     Maximum number of timers                                 */
#define MT_TMR_CFG_NAME_EN        1u   /*     Determine timer names                                    */
#define MT_TMR_CFG_WHEEL_SIZE     7u   /*     Size of timer wheel (#Spokes)                            */
#define MT_TMR_CFG_TICKS_PER_SEC 10u   /*     Rate at which timer management task runs (Hz)            */

#endif
