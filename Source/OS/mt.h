

#ifndef   MT_MT_II_H
#define   MT_MT_II_H

#ifdef __cplusplus
extern "C" {
#endif

/*
*********************************************************************************************************
*                                       MTK VERSION NUMBER
*********************************************************************************************************
*/

#define  MT_VERSION                 29211u              /* Version of MTK (Vx.yy mult. by 10000)  */

/*
*********************************************************************************************************
*                                        INCLUDE HEADER FILES
*********************************************************************************************************
*/

#include "app_cfg.h"
#include "mt_cfg.h"
#include "mt_cpu.h"

/*
*********************************************************************************************************
*                                            MISCELLANEOUS
*********************************************************************************************************
*/

#ifdef   MT_GLOBALS
#define  MT_EXT
#else
#define  MT_EXT  extern
#endif

#ifndef  MT_FALSE
#define  MT_FALSE                       0u
#endif

#ifndef  MT_TRUE
#define  MT_TRUE                        1u
#endif

#define  MT_ASCII_NUL            (INT8U)0

#define  MT_PRIO_SELF                0xFFu              /* Indicate SELF priority                      */
#define  MT_PRIO_MUTEX_CEIL_DIS      0xFFu              /* Disable mutex priority ceiling promotion    */

#if MT_TASK_STAT_EN > 0u
#define  MT_N_SYS_TASKS                 2u              /* Number of system tasks                      */
#else
#define  MT_N_SYS_TASKS                 1u
#endif

#define  MT_TASK_STAT_PRIO  (MT_LOWEST_PRIO - 1u)       /* Statistic task priority                     */
#define  MT_TASK_IDLE_PRIO  (MT_LOWEST_PRIO)            /* IDLE      task priority                     */

#if MT_LOWEST_PRIO <= 63u
#define  MT_EVENT_TBL_SIZE ((MT_LOWEST_PRIO) / 8u + 1u) /* Size of event table                         */
#define  MT_RDY_TBL_SIZE   ((MT_LOWEST_PRIO) / 8u + 1u) /* Size of ready table                         */
#else
#define  MT_EVENT_TBL_SIZE ((MT_LOWEST_PRIO) / 16u + 1u)/* Size of event table                         */
#define  MT_RDY_TBL_SIZE   ((MT_LOWEST_PRIO) / 16u + 1u)/* Size of ready table                         */
#endif

#define  MT_TASK_IDLE_ID            65535u              /* ID numbers for Idle, Stat and Timer tasks   */
#define  MT_TASK_STAT_ID            65534u
#define  MT_TASK_TMR_ID             65533u

#define  MT_EVENT_EN           (((MT_Q_EN > 0u) && (MT_MAX_QS > 0u)) || (MT_MBOX_EN > 0u) || (MT_SEM_EN > 0u) || (MT_MUTEX_EN > 0u))

#define  MT_TCB_RESERVED        ((MT_TCB *)1)

/*$PAGE*/
/*
*********************************************************************************************************
*                             TASK STATUS (Bit definition for OSTCBStat)
*********************************************************************************************************
*/
#define  MT_STAT_RDY                 0x00u  /* Ready to run                                            */
#define  MT_STAT_SEM                 0x01u  /* Pending on semaphore                                    */
#define  MT_STAT_MBOX                0x02u  /* Pending on mailbox                                      */
#define  MT_STAT_Q                   0x04u  /* Pending on queue                                        */
#define  MT_STAT_SUSPEND             0x08u  /* Task is suspended                                       */
#define  MT_STAT_MUTEX               0x10u  /* Pending on mutual exclusion semaphore                   */
#define  MT_STAT_FLAG                0x20u  /* Pending on event flag group                             */
#define  MT_STAT_MULTI               0x80u  /* Pending on multiple events                              */

#define  MT_STAT_PEND_ANY         (MT_STAT_SEM | MT_STAT_MBOX | MT_STAT_Q | MT_STAT_MUTEX | MT_STAT_FLAG)

/*
*********************************************************************************************************
*                          TASK PEND STATUS (Status codes for OSTCBStatPend)
*********************************************************************************************************
*/
#define  MT_STAT_PEND_OK                0u  /* Pending status OK, not pending, or pending complete     */
#define  MT_STAT_PEND_TO                1u  /* Pending timed out                                       */
#define  MT_STAT_PEND_ABORT             2u  /* Pending aborted                                         */

/*
*********************************************************************************************************
*                                           MT_EVENT types
*********************************************************************************************************
*/
#define  MT_EVENT_TYPE_UNUSED           0u
#define  MT_EVENT_TYPE_MBOX             1u
#define  MT_EVENT_TYPE_Q                2u
#define  MT_EVENT_TYPE_SEM              3u
#define  MT_EVENT_TYPE_MUTEX            4u
#define  MT_EVENT_TYPE_FLAG             5u

#define  MT_TMR_TYPE                  100u  /* Used to identify Timers ...                             */
                                            /* ... (Must be different value than MT_EVENT_TYPE_xxx)    */

/*
*********************************************************************************************************
*                                             EVENT FLAGS
*********************************************************************************************************
*/
#define  MT_FLAG_WAIT_CLR_ALL           0u  /* Wait for ALL    the bits specified to be CLR (i.e. 0)   */
#define  MT_FLAG_WAIT_CLR_AND           0u

#define  MT_FLAG_WAIT_CLR_ANY           1u  /* Wait for ANY of the bits specified to be CLR (i.e. 0)   */
#define  MT_FLAG_WAIT_CLR_OR            1u

#define  MT_FLAG_WAIT_SET_ALL           2u  /* Wait for ALL    the bits specified to be SET (i.e. 1)   */
#define  MT_FLAG_WAIT_SET_AND           2u

#define  MT_FLAG_WAIT_SET_ANY           3u  /* Wait for ANY of the bits specified to be SET (i.e. 1)   */
#define  MT_FLAG_WAIT_SET_OR            3u


#define  MT_FLAG_CONSUME             0x80u  /* Consume the flags if condition(s) satisfied             */


#define  MT_FLAG_CLR                    0u
#define  MT_FLAG_SET                    1u

/*
*********************************************************************************************************
*                                     Values for OSTickStepState
*
* Note(s): 
*********************************************************************************************************
*/

#if MT_TICK_STEP_EN > 0u
#define  MT_TICK_STEP_DIS               0u  /* Stepping is disabled, tick runs as normal               */
#define  MT_TICK_STEP_WAIT              1u  /* Waiting for MT-View to set OSTickStepState to _ONCE  */
#define  MT_TICK_STEP_ONCE              2u  /* Process tick once and wait for next cmd from MT-View */
#endif

/*
*********************************************************************************************************
*      Possible values for 'opt' argument of OSSemDel(), OSMboxDel(), OSQDel() and OSMutexDel()
*********************************************************************************************************
*/
#define  MT_DEL_NO_PEND                 0u
#define  MT_DEL_ALWAYS                  1u

/*
*********************************************************************************************************
*                                        OS???Pend() OPTIONS
*
* These #defines are used to establish the options for OS???PendAbort().
*********************************************************************************************************
*/
#define  MT_PEND_OPT_NONE               0u  /* NO option selected                                      */
#define  MT_PEND_OPT_BROADCAST          1u  /* Broadcast action to ALL tasks waiting                   */

/*
*********************************************************************************************************
*                                     OS???PostOpt() OPTIONS
*
* These #defines are used to establish the options for OSMboxPostOpt() and OSQPostOpt().
*********************************************************************************************************
*/
#define  MT_POST_OPT_NONE            0x00u  /* NO option selected                                      */
#define  MT_POST_OPT_BROADCAST       0x01u  /* Broadcast message to ALL tasks waiting                  */
#define  MT_POST_OPT_FRONT           0x02u  /* Post to highest priority task waiting                   */
#define  MT_POST_OPT_NO_SCHED        0x04u  /* Do not call the scheduler if this option is selected    */

/*
*********************************************************************************************************
*                                TASK OPTIONS (see OSTaskCreateExt())
*********************************************************************************************************
*/
#define  MT_TASK_OPT_NONE          0x0000u  /* NO option selected                                      */
#define  MT_TASK_OPT_STK_CHK       0x0001u  /* Enable stack checking for the task                      */
#define  MT_TASK_OPT_STK_CLR       0x0002u  /* Clear the stack when the task is create                 */
#define  MT_TASK_OPT_SAVE_FP       0x0004u  /* Save the contents of any floating-point registers       */
#define  MT_TASK_OPT_NO_TLS        0x0008u  /* Specify that task doesn't needs TLS                     */

/*
*********************************************************************************************************
*                          TIMER OPTIONS (see OSTmrStart() and OSTmrStop())
*********************************************************************************************************
*/
#define  MT_TMR_OPT_NONE                0u  /* No option selected                                      */

#define  MT_TMR_OPT_ONE_SHOT            1u  /* Timer will not automatically restart when it expires    */
#define  MT_TMR_OPT_PERIODIC            2u  /* Timer will     automatically restart when it expires    */

#define  MT_TMR_OPT_CALLBACK            3u  /* OSTmrStop() option to call 'callback' w/ timer arg.     */
#define  MT_TMR_OPT_CALLBACK_ARG        4u  /* OSTmrStop() option to call 'callback' w/ new   arg.     */

/*
*********************************************************************************************************
*                                            TIMER STATES
*********************************************************************************************************
*/
#define  MT_TMR_STATE_UNUSED            0u
#define  MT_TMR_STATE_STOPPED           1u
#define  MT_TMR_STATE_COMPLETED         2u
#define  MT_TMR_STATE_RUNNING           3u

/*
*********************************************************************************************************
*                                             ERROR CODES
*********************************************************************************************************
*/
#define MT_ERR_NONE                     0u

#define MT_ERR_EVENT_TYPE               1u
#define MT_ERR_PEND_ISR                 2u
#define MT_ERR_POST_NULL_PTR            3u
#define MT_ERR_PEVENT_NULL              4u
#define MT_ERR_POST_ISR                 5u
#define MT_ERR_QUERY_ISR                6u
#define MT_ERR_INVALID_OPT              7u
#define MT_ERR_ID_INVALID               8u
#define MT_ERR_PDATA_NULL               9u

#define MT_ERR_TIMEOUT                 10u
#define MT_ERR_EVENT_NAME_TOO_LONG     11u
#define MT_ERR_PNAME_NULL              12u
#define MT_ERR_PEND_LOCKED             13u
#define MT_ERR_PEND_ABORT              14u
#define MT_ERR_DEL_ISR                 15u
#define MT_ERR_CREATE_ISR              16u
#define MT_ERR_NAME_GET_ISR            17u
#define MT_ERR_NAME_SET_ISR            18u
#define MT_ERR_ILLEGAL_CREATE_RUN_TIME 19u

#define MT_ERR_MBOX_FULL               20u

#define MT_ERR_Q_FULL                  30u
#define MT_ERR_Q_EMPTY                 31u

#define MT_ERR_PRIO_EXIST              40u
#define MT_ERR_PRIO                    41u
#define MT_ERR_PRIO_INVALID            42u

#define MT_ERR_SCHED_LOCKED            50u
#define MT_ERR_SEM_OVF                 51u

#define MT_ERR_TASK_CREATE_ISR         60u
#define MT_ERR_TASK_DEL                61u
#define MT_ERR_TASK_DEL_IDLE           62u
#define MT_ERR_TASK_DEL_REQ            63u
#define MT_ERR_TASK_DEL_ISR            64u
#define MT_ERR_TASK_NAME_TOO_LONG      65u
#define MT_ERR_TASK_NO_MORE_TCB        66u
#define MT_ERR_TASK_NOT_EXIST          67u
#define MT_ERR_TASK_NOT_SUSPENDED      68u
#define MT_ERR_TASK_OPT                69u
#define MT_ERR_TASK_RESUME_PRIO        70u
#define MT_ERR_TASK_SUSPEND_IDLE       71u
#define MT_ERR_TASK_SUSPEND_PRIO       72u
#define MT_ERR_TASK_WAITING            73u

#define MT_ERR_TIME_NOT_DLY            80u
#define MT_ERR_TIME_INVALID_MINUTES    81u
#define MT_ERR_TIME_INVALID_SECONDS    82u
#define MT_ERR_TIME_INVALID_MS         83u
#define MT_ERR_TIME_ZERO_DLY           84u
#define MT_ERR_TIME_DLY_ISR            85u

#define MT_ERR_MEM_INVALID_PART        90u
#define MT_ERR_MEM_INVALID_BLKS        91u
#define MT_ERR_MEM_INVALID_SIZE        92u
#define MT_ERR_MEM_NO_FREE_BLKS        93u
#define MT_ERR_MEM_FULL                94u
#define MT_ERR_MEM_INVALID_PBLK        95u
#define MT_ERR_MEM_INVALID_PMEM        96u
#define MT_ERR_MEM_INVALID_PDATA       97u
#define MT_ERR_MEM_INVALID_ADDR        98u
#define MT_ERR_MEM_NAME_TOO_LONG       99u

#define MT_ERR_NOT_MUTEX_OWNER        100u

#define MT_ERR_FLAG_INVALID_PGRP      110u
#define MT_ERR_FLAG_WAIT_TYPE         111u
#define MT_ERR_FLAG_NOT_RDY           112u
#define MT_ERR_FLAG_INVALID_OPT       113u
#define MT_ERR_FLAG_GRP_DEPLETED      114u
#define MT_ERR_FLAG_NAME_TOO_LONG     115u

#define MT_ERR_PCP_LOWER              120u

#define MT_ERR_TMR_INVALID_DLY        130u
#define MT_ERR_TMR_INVALID_PERIOD     131u
#define MT_ERR_TMR_INVALID_OPT        132u
#define MT_ERR_TMR_INVALID_NAME       133u
#define MT_ERR_TMR_NON_AVAIL          134u
#define MT_ERR_TMR_INACTIVE           135u
#define MT_ERR_TMR_INVALID_DEST       136u
#define MT_ERR_TMR_INVALID_TYPE       137u
#define MT_ERR_TMR_INVALID            138u
#define MT_ERR_TMR_ISR                139u
#define MT_ERR_TMR_NAME_TOO_LONG      140u
#define MT_ERR_TMR_INVALID_STATE      141u
#define MT_ERR_TMR_STOPPED            142u
#define MT_ERR_TMR_NO_CALLBACK        143u

#define MT_ERR_NO_MORE_ID_AVAIL       150u

#define MT_ERR_TLS_NO_MORE_AVAIL      160u
#define MT_ERR_TLS_ID_INVALID         161u
#define MT_ERR_TLS_NOT_EN             162u
#define MT_ERR_TLS_DESTRUCT_ASSIGNED  163u
#define MT_ERR_MT_NOT_RUNNING         164u

/*$PAGE*/
/*
*********************************************************************************************************
*                                       THREAD LOCAL STORAGE (TLS)
*********************************************************************************************************
*/

#if MT_TASK_CREATE_EXT_EN > 0u
#if defined(MT_TLS_TBL_SIZE) && (MT_TLS_TBL_SIZE > 0u)
typedef  void                       *MT_TLS;

typedef  INT8U                       MT_TLS_ID;
#endif
#endif

/*
*********************************************************************************************************
*                                         EVENT CONTROL BLOCK
*********************************************************************************************************
*/

#if MT_LOWEST_PRIO <= 63u
typedef  INT8U    MT_PRIO;
#else
typedef  INT16U   MT_PRIO;
#endif

#if (MT_EVENT_EN) && (MT_MAX_EVENTS > 0u)
typedef struct os_event {
    INT8U    OSEventType;                   /* Type of event control block (see MT_EVENT_TYPE_xxxx)    */
    void    *OSEventPtr;                    /* Pointer to message or queue structure                   */
    INT16U   OSEventCnt;                    /* Semaphore Count (not used if other EVENT type)          */
    MT_PRIO  OSEventGrp;                    /* Group corresponding to tasks waiting for event to occur */
    MT_PRIO  OSEventTbl[MT_EVENT_TBL_SIZE]; /* List of tasks waiting for event to occur                */

#if MT_EVENT_NAME_EN > 0u
    INT8U   *OSEventName;
#endif
} MT_EVENT;
#endif


/*
*********************************************************************************************************
*                                      EVENT FLAGS CONTROL BLOCK
*********************************************************************************************************
*/

#if (MT_FLAG_EN > 0u) && (MT_MAX_FLAGS > 0u)

#if MT_FLAGS_NBITS == 8u                    /* Determine the size of MT_FLAGS (8, 16 or 32 bits)       */
typedef  INT8U    MT_FLAGS;
#endif

#if MT_FLAGS_NBITS == 16u
typedef  INT16U   MT_FLAGS;
#endif

#if MT_FLAGS_NBITS == 32u
typedef  INT32U   MT_FLAGS;
#endif


typedef struct os_flag_grp {                /* Event Flag Group                                        */
    INT8U         OSFlagType;               /* Should be set to MT_EVENT_TYPE_FLAG                     */
    void         *OSFlagWaitList;           /* Pointer to first NODE of task waiting on event flag     */
    MT_FLAGS      OSFlagFlags;              /* 8, 16 or 32 bit flags                                   */
#if MT_FLAG_NAME_EN > 0u
    INT8U        *OSFlagName;
#endif
} MT_FLAG_GRP;



typedef struct os_flag_node {               /* Event Flag Wait List Node                               */
    void         *OSFlagNodeNext;           /* Pointer to next     NODE in wait list                   */
    void         *OSFlagNodePrev;           /* Pointer to previous NODE in wait list                   */
    void         *OSFlagNodeTCB;            /* Pointer to TCB of waiting task                          */
    void         *OSFlagNodeFlagGrp;        /* Pointer to Event Flag Group                             */
    MT_FLAGS      OSFlagNodeFlags;          /* Event flag to wait on                                   */
    INT8U         OSFlagNodeWaitType;       /* Type of wait:                                           */
                                            /*      MT_FLAG_WAIT_AND                                   */
                                            /*      MT_FLAG_WAIT_ALL                                   */
                                            /*      MT_FLAG_WAIT_OR                                    */
                                            /*      MT_FLAG_WAIT_ANY                                   */
} MT_FLAG_NODE;
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                        MESSAGE MAILBOX DATA
*********************************************************************************************************
*/

#if MT_MBOX_EN > 0u
typedef struct os_mbox_data {
    void   *OSMsg;                          /* Pointer to message in mailbox                           */
    MT_PRIO OSEventTbl[MT_EVENT_TBL_SIZE];  /* List of tasks waiting for event to occur                */
    MT_PRIO OSEventGrp;                     /* Group corresponding to tasks waiting for event to occur */
} MT_MBOX_DATA;
#endif

/*
*********************************************************************************************************
*                                  MEMORY PARTITION DATA STRUCTURES
*********************************************************************************************************
*/

#if (MT_MEM_EN > 0u) && (MT_MAX_MEM_PART > 0u)
typedef struct os_mem {                     /* MEMORY CONTROL BLOCK                                    */
    void   *OSMemAddr;                      /* Pointer to beginning of memory partition                */
    void   *OSMemFreeList;                  /* Pointer to list of free memory blocks                   */
    INT32U  OSMemBlkSize;                   /* Size (in bytes) of each block of memory                 */
    INT32U  OSMemNBlks;                     /* Total number of blocks in this partition                */
    INT32U  OSMemNFree;                     /* Number of memory blocks remaining in this partition     */
#if MT_MEM_NAME_EN > 0u
    INT8U  *OSMemName;                      /* Memory partition name                                   */
#endif
} MT_MEM;


typedef struct os_mem_data {
    void   *OSAddr;                         /* Ptr to the beginning address of the memory partition    */
    void   *OSFreeList;                     /* Ptr to the beginning of the free list of memory blocks  */
    INT32U  OSBlkSize;                      /* Size (in bytes) of each memory block                    */
    INT32U  OSNBlks;                        /* Total number of blocks in the partition                 */
    INT32U  OSNFree;                        /* Number of memory blocks free                            */
    INT32U  OSNUsed;                        /* Number of memory blocks used                            */
} MT_MEM_DATA;
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                   MUTUAL EXCLUSION SEMAPHORE DATA
*********************************************************************************************************
*/

#if MT_MUTEX_EN > 0u
typedef struct os_mutex_data {
    MT_PRIO OSEventTbl[MT_EVENT_TBL_SIZE];  /* List of tasks waiting for event to occur                */
    MT_PRIO OSEventGrp;                     /* Group corresponding to tasks waiting for event to occur */
    BOOLEAN OSValue;                        /* Mutex value (MT_FALSE = used, MT_TRUE = available)      */
    INT8U   OSOwnerPrio;                    /* Mutex owner's task priority or 0xFF if no owner         */
    INT8U   OSMutexPCP;                     /* Priority Ceiling Priority or 0xFF if PCP disabled       */
} MT_MUTEX_DATA;
#endif

/*
*********************************************************************************************************
*                                         MESSAGE QUEUE DATA
*********************************************************************************************************
*/

#if MT_Q_EN > 0u
typedef struct os_q {                       /* QUEUE CONTROL BLOCK                                     */
    struct os_q   *OSQPtr;                  /* Link to next queue control block in list of free blocks */
    void         **OSQStart;                /* Ptr to start of queue data                              */
    void         **OSQEnd;                  /* Ptr to end   of queue data                              */
    void         **OSQIn;                   /* Ptr to where next message will be inserted  in   the Q  */
    void         **OSQOut;                  /* Ptr to where next message will be extracted from the Q  */
    INT16U         OSQSize;                 /* Size of queue (maximum number of entries)               */
    INT16U         OSQEntries;              /* Current number of entries in the queue                  */
} MT_Q;


typedef struct os_q_data {
    void          *OSMsg;                   /* Pointer to next message to be extracted from queue      */
    INT16U         OSNMsgs;                 /* Number of messages in message queue                     */
    INT16U         OSQSize;                 /* Size of message queue                                   */
    MT_PRIO        OSEventTbl[MT_EVENT_TBL_SIZE];  /* List of tasks waiting for event to occur         */
    MT_PRIO        OSEventGrp;              /* Group corresponding to tasks waiting for event to occur */
} MT_Q_DATA;
#endif

/*
*********************************************************************************************************
*                                           SEMAPHORE DATA
*********************************************************************************************************
*/

#if MT_SEM_EN > 0u
typedef struct os_sem_data {
    INT16U  OSCnt;                          /* Semaphore count                                         */
    MT_PRIO OSEventTbl[MT_EVENT_TBL_SIZE];  /* List of tasks waiting for event to occur                */
    MT_PRIO OSEventGrp;                     /* Group corresponding to tasks waiting for event to occur */
} MT_SEM_DATA;
#endif

/*
*********************************************************************************************************
*                                           TASK STACK DATA
*********************************************************************************************************
*/

#if MT_TASK_CREATE_EXT_EN > 0u
typedef struct os_stk_data {
    INT32U  OSFree;                         /* Number of free entries on the stack                     */
    INT32U  OSUsed;                         /* Number of entries used on the stack                     */
} MT_STK_DATA;
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                         TASK CONTROL BLOCK
*********************************************************************************************************
*/

typedef struct os_tcb {
    MT_STK          *OSTCBStkPtr;           /* Pointer to current top of stack                         */

#if MT_TASK_CREATE_EXT_EN > 0u
    void            *OSTCBExtPtr;           /* Pointer to user definable data for TCB extension        */
    MT_STK          *OSTCBStkBottom;        /* Pointer to bottom of stack                              */
    INT32U           OSTCBStkSize;          /* Size of task stack (in number of stack elements)        */
    INT16U           OSTCBOpt;              /* Task options as passed by OSTaskCreateExt()             */
    INT16U           OSTCBId;               /* Task ID (0..65535)                                      */
#endif

    struct os_tcb   *OSTCBNext;             /* Pointer to next     TCB in the TCB list                 */
    struct os_tcb   *OSTCBPrev;             /* Pointer to previous TCB in the TCB list                 */

#if MT_TASK_CREATE_EXT_EN > 0u
#if defined(MT_TLS_TBL_SIZE) && (MT_TLS_TBL_SIZE > 0u)
    MT_TLS           OSTCBTLSTbl[MT_TLS_TBL_SIZE];
#endif
#endif

#if (MT_EVENT_EN)
    MT_EVENT        *OSTCBEventPtr;         /* Pointer to          event control block                 */
#endif

#if (MT_EVENT_EN) && (MT_EVENT_MULTI_EN > 0u)
    MT_EVENT       **OSTCBEventMultiPtr;    /* Pointer to multiple event control blocks                */
#endif

#if ((MT_Q_EN > 0u) && (MT_MAX_QS > 0u)) || (MT_MBOX_EN > 0u)
    void            *OSTCBMsg;              /* Message received from OSMboxPost() or OSQPost()         */
#endif

#if (MT_FLAG_EN > 0u) && (MT_MAX_FLAGS > 0u)
#if MT_TASK_DEL_EN > 0u
    MT_FLAG_NODE    *OSTCBFlagNode;         /* Pointer to event flag node                              */
#endif
    MT_FLAGS         OSTCBFlagsRdy;         /* Event flags that made task ready to run                 */
#endif

    INT32U           OSTCBDly;              /* Nbr ticks to delay task or, timeout waiting for event   */
    INT8U            OSTCBStat;             /* Task      status                                        */
    INT8U            OSTCBStatPend;         /* Task PEND status                                        */
    INT8U            OSTCBPrio;             /* Task priority (0 == highest)                            */

    INT8U            OSTCBX;                /* Bit position in group  corresponding to task priority   */
    INT8U            OSTCBY;                /* Index into ready table corresponding to task priority   */
    MT_PRIO          OSTCBBitX;             /* Bit mask to access bit position in ready table          */
    MT_PRIO          OSTCBBitY;             /* Bit mask to access bit position in ready group          */

#if MT_TASK_DEL_EN > 0u
    INT8U            OSTCBDelReq;           /* Indicates whether a task needs to delete itself         */
#endif

#if MT_TASK_PROFILE_EN > 0u
    INT32U           OSTCBCtxSwCtr;         /* Number of time the task was switched in                 */
    INT32U           OSTCBCyclesTot;        /* Total number of clock cycles the task has been running  */
    INT32U           OSTCBCyclesStart;      /* Snapshot of cycle counter at start of task resumption   */
    MT_STK          *OSTCBStkBase;          /* Pointer to the beginning of the task stack              */
    INT32U           OSTCBStkUsed;          /* Number of bytes used from the stack                     */
#endif

#if MT_TASK_NAME_EN > 0u
    INT8U           *OSTCBTaskName;
#endif

#if MT_TASK_REG_TBL_SIZE > 0u
    INT32U           OSTCBRegTbl[MT_TASK_REG_TBL_SIZE];
#endif
} MT_TCB;

/*$PAGE*/
/*
*********************************************************************************************************
*                                          TIMER DATA TYPES
*********************************************************************************************************
*/

#if MT_TMR_EN > 0u
typedef  void (*MT_TMR_CALLBACK)(void *ptmr, void *parg);



typedef  struct  os_tmr {
    INT8U            OSTmrType;             /* Should be set to MT_TMR_TYPE                            */
    MT_TMR_CALLBACK  OSTmrCallback;         /* Function to call when timer expires                     */
    void            *OSTmrCallbackArg;      /* Argument to pass to function when timer expires         */
    void            *OSTmrNext;             /* Double link list pointers                               */
    void            *OSTmrPrev;
    INT32U           OSTmrMatch;            /* Timer expires when OSTmrTime == OSTmrMatch              */
    INT32U           OSTmrDly;              /* Delay time before periodic update starts                */
    INT32U           OSTmrPeriod;           /* Period to repeat timer                                  */
#if MT_TMR_CFG_NAME_EN > 0u
    INT8U           *OSTmrName;             /* Name to give the timer                                  */
#endif
    INT8U            OSTmrOpt;              /* Options (see MT_TMR_OPT_xxx)                            */
    INT8U            OSTmrState;            /* Indicates the state of the timer:                       */
                                            /*     MT_TMR_STATE_UNUSED                                 */
                                            /*     MT_TMR_STATE_RUNNING                                */
                                            /*     MT_TMR_STATE_STOPPED                                */
} MT_TMR;



typedef  struct  os_tmr_wheel {
    MT_TMR          *OSTmrFirst;            /* Pointer to first timer in linked list                   */
    INT16U           OSTmrEntries;
} MT_TMR_WHEEL;
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                       THREAD LOCAL STORAGE (TLS)
*********************************************************************************************************
*/

#if MT_TASK_CREATE_EXT_EN > 0u
#if defined(MT_TLS_TBL_SIZE) && (MT_TLS_TBL_SIZE > 0u)
typedef  void                      (*MT_TLS_DESTRUCT_PTR)(MT_TCB    *ptcb,
                                                          MT_TLS_ID  id,
                                                          MT_TLS     value);
#endif
#endif

/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

MT_EXT  INT32U            OSCtxSwCtr;               /* Counter of number of context switches           */

#if (MT_EVENT_EN) && (MT_MAX_EVENTS > 0u)
MT_EXT  MT_EVENT         *OSEventFreeList;          /* Pointer to list of free EVENT control blocks    */
MT_EXT  MT_EVENT          OSEventTbl[MT_MAX_EVENTS];/* Table of EVENT control blocks                   */
#endif

#if (MT_FLAG_EN > 0u) && (MT_MAX_FLAGS > 0u)
MT_EXT  MT_FLAG_GRP       OSFlagTbl[MT_MAX_FLAGS];  /* Table containing event flag groups              */
MT_EXT  MT_FLAG_GRP      *OSFlagFreeList;           /* Pointer to free list of event flag groups       */
#endif

#if MT_TASK_STAT_EN > 0u
MT_EXT  INT8U             OSCPUUsage;               /* Percentage of CPU used                          */
MT_EXT  INT32U            OSIdleCtrMax;             /* Max. value that idle ctr can take in 1 sec.     */
MT_EXT  INT32U            OSIdleCtrRun;             /* Val. reached by idle ctr at run time in 1 sec.  */
MT_EXT  BOOLEAN           OSStatRdy;                /* Flag indicating that the statistic task is rdy  */
MT_EXT  MT_STK            OSTaskStatStk[MT_TASK_STAT_STK_SIZE];      /* Statistics task stack          */
#endif

MT_EXT  INT8U             OSIntNesting;             /* Interrupt nesting level                         */

MT_EXT  INT8U             OSLockNesting;            /* Multitasking lock nesting level                 */

MT_EXT  INT8U             OSPrioCur;                /* Priority of current task                        */
MT_EXT  INT8U             OSPrioHighRdy;            /* Priority of highest priority task               */

MT_EXT  MT_PRIO           OSRdyGrp;                        /* Ready list group                         */
MT_EXT  MT_PRIO           OSRdyTbl[MT_RDY_TBL_SIZE];       /* Table of tasks which are ready to run    */

MT_EXT  BOOLEAN           OSRunning;                       /* Flag indicating that kernel is running   */

MT_EXT  INT8U             OSTaskCtr;                       /* Number of tasks created                  */

MT_EXT  volatile  INT32U  OSIdleCtr;                                 /* Idle counter                   */

#ifdef MT_SAFETY_CRITICAL_IEC61508
MT_EXT  BOOLEAN           OSSafetyCriticalStartFlag;
#endif

MT_EXT  MT_STK            OSTaskIdleStk[MT_TASK_IDLE_STK_SIZE];      /* Idle task stack                */


MT_EXT  MT_TCB           *OSTCBCur;                        /* Pointer to currently running TCB         */
MT_EXT  MT_TCB           *OSTCBFreeList;                   /* Pointer to list of free TCBs             */
MT_EXT  MT_TCB           *OSTCBHighRdy;                    /* Pointer to highest priority TCB R-to-R   */
MT_EXT  MT_TCB           *OSTCBList;                       /* Pointer to doubly linked list of TCBs    */
MT_EXT  MT_TCB           *OSTCBPrioTbl[MT_LOWEST_PRIO + 1u];    /* Table of pointers to created TCBs   */
MT_EXT  MT_TCB            OSTCBTbl[MT_MAX_TASKS + MT_N_SYS_TASKS];   /* Table of TCBs                  */

#if MT_TICK_STEP_EN > 0u
MT_EXT  INT8U             OSTickStepState;          /* Indicates the state of the tick step feature    */
#endif

#if (MT_MEM_EN > 0u) && (MT_MAX_MEM_PART > 0u)
MT_EXT  MT_MEM           *OSMemFreeList;            /* Pointer to free list of memory partitions       */
MT_EXT  MT_MEM            OSMemTbl[MT_MAX_MEM_PART];/* Storage for memory partition manager            */
#endif

#if (MT_Q_EN > 0u) && (MT_MAX_QS > 0u)
MT_EXT  MT_Q             *OSQFreeList;              /* Pointer to list of free QUEUE control blocks    */
MT_EXT  MT_Q              OSQTbl[MT_MAX_QS];        /* Table of QUEUE control blocks                   */
#endif

#if MT_TASK_REG_TBL_SIZE > 0u
MT_EXT  INT8U             OSTaskRegNextAvailID;     /* Next available Task register ID                 */
#endif

#if MT_TIME_GET_SET_EN > 0u
MT_EXT  volatile  INT32U  OSTime;                   /* Current value of system time (in ticks)         */
#endif

#if MT_TMR_EN > 0u
MT_EXT  INT16U            OSTmrFree;                /* Number of free entries in the timer pool        */
MT_EXT  INT16U            OSTmrUsed;                /* Number of timers used                           */
MT_EXT  INT32U            OSTmrTime;                /* Current timer time                              */

MT_EXT  MT_EVENT         *OSTmrSem;                 /* Sem. used to gain exclusive access to timers    */
MT_EXT  MT_EVENT         *OSTmrSemSignal;           /* Sem. used to signal the update of timers        */

MT_EXT  MT_TMR            OSTmrTbl[MT_TMR_CFG_MAX]; /* Table containing pool of timers                 */
MT_EXT  MT_TMR           *OSTmrFreeList;            /* Pointer to free list of timers                  */
MT_EXT  MT_STK            OSTmrTaskStk[MT_TASK_TMR_STK_SIZE];

MT_EXT  MT_TMR_WHEEL      OSTmrWheelTbl[MT_TMR_CFG_WHEEL_SIZE];
#endif

extern  INT8U   const     OSUnMapTbl[256];          /* Priority->Index    lookup table                 */

/*$PAGE*/
/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*                                     (Target Independent Functions)
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            MISCELLANEOUS
*********************************************************************************************************
*/

#if (MT_EVENT_EN)

#if (MT_EVENT_NAME_EN > 0u)
INT8U         OSEventNameGet          (MT_EVENT        *pevent,
                                       INT8U          **pname,
                                       INT8U           *perr);

void          OSEventNameSet          (MT_EVENT        *pevent,
                                       INT8U           *pname,
                                       INT8U           *perr);
#endif

#if (MT_EVENT_MULTI_EN > 0u)
INT16U        OSEventPendMulti        (MT_EVENT       **pevents_pend,
                                       MT_EVENT       **pevents_rdy,
                                       void           **pmsgs_rdy,
                                       INT32U           timeout,
                                       INT8U           *perr);
#endif

#endif

/*
*********************************************************************************************************
*                                   TASK LOCAL STORAGE (TLS) SUPPORT                                          
*********************************************************************************************************
*/

#if MT_TASK_CREATE_EXT_EN > 0u
#if defined(MT_TLS_TBL_SIZE) && (MT_TLS_TBL_SIZE > 0u)

MT_TLS_ID     MT_TLS_GetID            (INT8U               *perr);

MT_TLS        MT_TLS_GetValue         (MT_TCB              *ptcb,
                                       MT_TLS_ID            id,
                                       INT8U               *perr);

void          MT_TLS_Init             (INT8U               *perr);

void          MT_TLS_SetValue         (MT_TCB              *ptcb,
                                       MT_TLS_ID            id,
                                       MT_TLS               value,
                                       INT8U               *perr);

void          MT_TLS_SetDestruct      (MT_TLS_ID            id,
                                       MT_TLS_DESTRUCT_PTR  pdestruct,
                                       INT8U               *perr);

void          MT_TLS_TaskCreate       (MT_TCB              *ptcb);

void          MT_TLS_TaskDel          (MT_TCB              *ptcb);

void          MT_TLS_TaskSw           (void);

#endif
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                       EVENT FLAGS MANAGEMENT
*********************************************************************************************************
*/

#if (MT_FLAG_EN > 0u) && (MT_MAX_FLAGS > 0u)

#if MT_FLAG_ACCEPT_EN > 0u
MT_FLAGS      OSFlagAccept            (MT_FLAG_GRP     *pgrp,
                                       MT_FLAGS         flags,
                                       INT8U            wait_type,
                                       INT8U           *perr);
#endif

MT_FLAG_GRP  *OSFlagCreate            (MT_FLAGS         flags,
                                       INT8U           *perr);

#if MT_FLAG_DEL_EN > 0u
MT_FLAG_GRP  *OSFlagDel               (MT_FLAG_GRP     *pgrp,
                                       INT8U            opt,
                                       INT8U           *perr);
#endif

#if (MT_FLAG_EN > 0u) && (MT_FLAG_NAME_EN > 0u)
INT8U         OSFlagNameGet           (MT_FLAG_GRP     *pgrp,
                                       INT8U          **pname,
                                       INT8U           *perr);

void          OSFlagNameSet           (MT_FLAG_GRP     *pgrp,
                                       INT8U           *pname,
                                       INT8U           *perr);
#endif

MT_FLAGS      OSFlagPend              (MT_FLAG_GRP     *pgrp,
                                       MT_FLAGS         flags,
                                       INT8U            wait_type,
                                       INT32U           timeout,
                                       INT8U           *perr);

MT_FLAGS      OSFlagPendGetFlagsRdy   (void);
MT_FLAGS      OSFlagPost              (MT_FLAG_GRP     *pgrp,
                                       MT_FLAGS         flags,
                                       INT8U            opt,
                                       INT8U           *perr);

#if MT_FLAG_QUERY_EN > 0u
MT_FLAGS      OSFlagQuery             (MT_FLAG_GRP     *pgrp,
                                       INT8U           *perr);
#endif
#endif

/*
*********************************************************************************************************
*                                     MESSAGE MAILBOX MANAGEMENT
*********************************************************************************************************
*/

#if MT_MBOX_EN > 0u

#if MT_MBOX_ACCEPT_EN > 0u
void         *OSMboxAccept            (MT_EVENT        *pevent);
#endif

MT_EVENT     *OSMboxCreate            (void            *pmsg);

#if MT_MBOX_DEL_EN > 0u
MT_EVENT     *OSMboxDel               (MT_EVENT        *pevent,
                                       INT8U            opt,
                                       INT8U           *perr);
#endif

void         *OSMboxPend              (MT_EVENT        *pevent,
                                       INT32U           timeout,
                                       INT8U           *perr);

#if MT_MBOX_PEND_ABORT_EN > 0u
INT8U         OSMboxPendAbort         (MT_EVENT        *pevent,
                                       INT8U            opt,
                                       INT8U           *perr);
#endif

#if MT_MBOX_POST_EN > 0u
INT8U         OSMboxPost              (MT_EVENT        *pevent,
                                       void            *pmsg);
#endif

#if MT_MBOX_POST_OPT_EN > 0u
INT8U         OSMboxPostOpt           (MT_EVENT        *pevent,
                                       void            *pmsg,
                                       INT8U            opt);
#endif

#if MT_MBOX_QUERY_EN > 0u
INT8U         OSMboxQuery             (MT_EVENT        *pevent,
                                       MT_MBOX_DATA    *p_mbox_data);
#endif
#endif

/*
*********************************************************************************************************
*                                          MEMORY MANAGEMENT
*********************************************************************************************************
*/

#if (MT_MEM_EN > 0u) && (MT_MAX_MEM_PART > 0u)

MT_MEM       *OSMemCreate             (void            *addr,
                                       INT32U           nblks,
                                       INT32U           blksize,
                                       INT8U           *perr);

void         *OSMemGet                (MT_MEM          *pmem,
                                       INT8U           *perr);
#if MT_MEM_NAME_EN > 0u
INT8U         OSMemNameGet            (MT_MEM          *pmem,
                                       INT8U          **pname,
                                       INT8U           *perr);

void          OSMemNameSet            (MT_MEM          *pmem,
                                       INT8U           *pname,
                                       INT8U           *perr);
#endif
INT8U         OSMemPut                (MT_MEM          *pmem,
                                       void            *pblk);

#if MT_MEM_QUERY_EN > 0u
INT8U         OSMemQuery              (MT_MEM          *pmem,
                                       MT_MEM_DATA     *p_mem_data);
#endif

#endif

/*
*********************************************************************************************************
*                                MUTUAL EXCLUSION SEMAPHORE MANAGEMENT
*********************************************************************************************************
*/

#if MT_MUTEX_EN > 0u

#if MT_MUTEX_ACCEPT_EN > 0u
BOOLEAN       OSMutexAccept           (MT_EVENT        *pevent,
                                       INT8U           *perr);
#endif

MT_EVENT     *OSMutexCreate           (INT8U            prio,
                                       INT8U           *perr);

#if MT_MUTEX_DEL_EN > 0u
MT_EVENT     *OSMutexDel              (MT_EVENT        *pevent,
                                       INT8U            opt,
                                       INT8U           *perr);
#endif

void          OSMutexPend             (MT_EVENT        *pevent,
                                       INT32U           timeout,
                                       INT8U           *perr);

INT8U         OSMutexPost             (MT_EVENT        *pevent);

#if MT_MUTEX_QUERY_EN > 0u
INT8U         OSMutexQuery            (MT_EVENT        *pevent,
                                       MT_MUTEX_DATA   *p_mutex_data);
#endif

#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                      MESSAGE QUEUE MANAGEMENT
*********************************************************************************************************
*/

#if (MT_Q_EN > 0u) && (MT_MAX_QS > 0u)

#if MT_Q_ACCEPT_EN > 0u
void         *OSQAccept               (MT_EVENT        *pevent,
                                       INT8U           *perr);
#endif

MT_EVENT     *OSQCreate               (void           **start,
                                       INT16U           size);

#if MT_Q_DEL_EN > 0u
MT_EVENT     *OSQDel                  (MT_EVENT        *pevent,
                                       INT8U            opt,
                                       INT8U           *perr);
#endif

#if MT_Q_FLUSH_EN > 0u
INT8U         OSQFlush                (MT_EVENT        *pevent);
#endif

void         *OSQPend                 (MT_EVENT        *pevent,
                                       INT32U           timeout,
                                       INT8U           *perr);

#if MT_Q_PEND_ABORT_EN > 0u
INT8U         OSQPendAbort            (MT_EVENT        *pevent,
                                       INT8U            opt,
                                       INT8U           *perr);
#endif

#if MT_Q_POST_EN > 0u
INT8U         OSQPost                 (MT_EVENT        *pevent,
                                       void            *pmsg);
#endif

#if MT_Q_POST_FRONT_EN > 0u
INT8U         OSQPostFront            (MT_EVENT        *pevent,
                                       void            *pmsg);
#endif

#if MT_Q_POST_OPT_EN > 0u
INT8U         OSQPostOpt              (MT_EVENT        *pevent,
                                       void            *pmsg,
                                       INT8U            opt);
#endif

#if MT_Q_QUERY_EN > 0u
INT8U         OSQQuery                (MT_EVENT        *pevent,
                                       MT_Q_DATA       *p_q_data);
#endif

#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                        SEMAPHORE MANAGEMENT
*********************************************************************************************************
*/
#if MT_SEM_EN > 0u

#if MT_SEM_ACCEPT_EN > 0u
INT16U        OSSemAccept             (MT_EVENT        *pevent);
#endif

MT_EVENT     *OSSemCreate             (INT16U           cnt);

#if MT_SEM_DEL_EN > 0u
MT_EVENT     *OSSemDel                (MT_EVENT        *pevent,
                                       INT8U            opt,
                                       INT8U           *perr);
#endif

void          OSSemPend               (MT_EVENT        *pevent,
                                       INT32U           timeout,
                                       INT8U           *perr);

#if MT_SEM_PEND_ABORT_EN > 0u
INT8U         OSSemPendAbort          (MT_EVENT        *pevent,
                                       INT8U            opt,
                                       INT8U           *perr);
#endif

INT8U         OSSemPost               (MT_EVENT        *pevent);

#if MT_SEM_QUERY_EN > 0u
INT8U         OSSemQuery              (MT_EVENT        *pevent,
                                       MT_SEM_DATA     *p_sem_data);
#endif

#if MT_SEM_SET_EN > 0u
void          OSSemSet                (MT_EVENT        *pevent,
                                       INT16U           cnt,
                                       INT8U           *perr);
#endif

#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                           TASK MANAGEMENT
*********************************************************************************************************
*/
#if MT_TASK_CHANGE_PRIO_EN > 0u
INT8U         OSTaskChangePrio        (INT8U            oldprio,
                                       INT8U            newprio);
#endif

#if MT_TASK_CREATE_EN > 0u
INT8U         OSTaskCreate            (void           (*task)(void *p_arg),
                                       void            *p_arg,
                                       MT_STK          *ptos,
                                       INT8U            prio);
#endif

#if MT_TASK_CREATE_EXT_EN > 0u
INT8U         OSTaskCreateExt         (void           (*task)(void *p_arg),
                                       void            *p_arg,
                                       MT_STK          *ptos,
                                       INT8U            prio,
                                       INT16U           id,
                                       MT_STK          *pbos,
                                       INT32U           stk_size,
                                       void            *pext,
                                       INT16U           opt);
#endif

#if MT_TASK_DEL_EN > 0u
INT8U         OSTaskDel               (INT8U            prio);
INT8U         OSTaskDelReq            (INT8U            prio);
#endif

#if MT_TASK_NAME_EN > 0u
INT8U         OSTaskNameGet           (INT8U            prio,
                                       INT8U          **pname,
                                       INT8U           *perr);

void          OSTaskNameSet           (INT8U            prio,
                                       INT8U           *pname,
                                       INT8U           *perr);
#endif

#if MT_TASK_SUSPEND_EN > 0u
INT8U         OSTaskResume            (INT8U            prio);
INT8U         OSTaskSuspend           (INT8U            prio);
#endif

#if (MT_TASK_STAT_STK_CHK_EN > 0u) && (MT_TASK_CREATE_EXT_EN > 0u)
INT8U         OSTaskStkChk            (INT8U            prio,
                                       MT_STK_DATA     *p_stk_data);
#endif

#if MT_TASK_QUERY_EN > 0u
INT8U         OSTaskQuery             (INT8U            prio,
                                       MT_TCB          *p_task_data);
#endif



#if MT_TASK_REG_TBL_SIZE > 0u
INT32U        OSTaskRegGet            (INT8U            prio,
                                       INT8U            id,
                                       INT8U           *perr);

INT8U         OSTaskRegGetID          (INT8U           *perr);

void          OSTaskRegSet            (INT8U            prio,
                                       INT8U            id,
                                       INT32U           value,
                                       INT8U           *perr);
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                           TIME MANAGEMENT
*********************************************************************************************************
*/

void          OSTimeDly               (INT32U           ticks);
void          OSTimeDlyTask           (INT8U prio, INT32U ticks);

#if MT_TIME_DLY_HMSM_EN > 0u
INT8U         OSTimeDlyHMSM           (INT8U            hours,
                                       INT8U            minutes,
                                       INT8U            seconds,
                                       INT16U           ms);
#endif

#if MT_TIME_DLY_RESUME_EN > 0u
INT8U         OSTimeDlyResume         (INT8U            prio);
#endif

#if MT_TIME_GET_SET_EN > 0u
INT32U        OSTimeGet               (void);
void          OSTimeSet               (INT32U           ticks);
#endif

void          OSTimeTick              (void);

/*
*********************************************************************************************************
*                                          TIMER MANAGEMENT
*********************************************************************************************************
*/

#if MT_TMR_EN > 0u
MT_TMR      *OSTmrCreate              (INT32U           dly,
                                       INT32U           period,
                                       INT8U            opt,
                                       MT_TMR_CALLBACK  callback,
                                       void            *callback_arg,
                                       INT8U           *pname,
                                       INT8U           *perr);

BOOLEAN      OSTmrDel                 (MT_TMR          *ptmr,
                                       INT8U           *perr);

#if MT_TMR_CFG_NAME_EN > 0u
INT8U        OSTmrNameGet             (MT_TMR          *ptmr,
                                       INT8U          **pdest,
                                       INT8U           *perr);
#endif
INT32U       OSTmrRemainGet           (MT_TMR          *ptmr,
                                       INT8U           *perr);

INT8U        OSTmrStateGet            (MT_TMR          *ptmr,
                                       INT8U           *perr);

BOOLEAN      OSTmrStart               (MT_TMR          *ptmr,
                                       INT8U           *perr);

BOOLEAN      OSTmrStop                (MT_TMR          *ptmr,
                                       INT8U            opt,
                                       void            *callback_arg,
                                       INT8U           *perr);

INT8U        OSTmrSignal              (void);
#endif

/*
*********************************************************************************************************
*                                            MISCELLANEOUS
*********************************************************************************************************
*/

void          OSInit                  (void);

void          OSIntEnter              (void);
void          OSIntExit               (void);

#ifdef MT_SAFETY_CRITICAL_IEC61508
void          OSSafetyCriticalStart   (void);
#endif

#if MT_SCHED_LOCK_EN > 0u
void          OSSchedLock             (void);
void          OSSchedUnlock           (void);
#endif

void          OSStart                 (void);

void          OSStatInit              (void);

INT16U        OSVersion               (void);

/*$PAGE*/
/*
*********************************************************************************************************
*                                      INTERNAL FUNCTION PROTOTYPES
*                            (Your application MUST NOT call these functions)
*********************************************************************************************************
*/

#if MT_TASK_DEL_EN > 0u
void          MT_Dummy                (void);
#endif

#if (MT_EVENT_EN)
INT8U         MT_EventTaskRdy         (MT_EVENT        *pevent,
                                       void            *pmsg,
                                       INT8U            msk,
                                       INT8U            pend_stat);

void          MT_EventTaskWait        (MT_EVENT        *pevent);

void          MT_EventTaskRemove      (MT_TCB          *ptcb,
                                       MT_EVENT        *pevent);

#if (MT_EVENT_MULTI_EN > 0u)
void          MT_EventTaskWaitMulti   (MT_EVENT       **pevents_wait);

void          MT_EventTaskRemoveMulti (MT_TCB          *ptcb,
                                       MT_EVENT       **pevents_multi);
#endif

void          MT_EventWaitListInit    (MT_EVENT        *pevent);
#endif

#if (MT_FLAG_EN > 0u) && (MT_MAX_FLAGS > 0u)
void          MT_FlagInit             (void);
void          MT_FlagUnlink           (MT_FLAG_NODE    *pnode);
#endif

void          MT_MemClr               (INT8U           *pdest,
                                       INT16U           size);

void          MT_MemCopy              (INT8U           *pdest,
                                       INT8U           *psrc,
                                       INT16U           size);

#if (MT_MEM_EN > 0u) && (MT_MAX_MEM_PART > 0u)
void          MT_MemInit              (void);
#endif

#if MT_Q_EN > 0u
void          MT_QInit                (void);
#endif

void          MT_Sched                (void);

#if (MT_EVENT_NAME_EN > 0u) || (MT_FLAG_NAME_EN > 0u) || (MT_MEM_NAME_EN > 0u) || (MT_TASK_NAME_EN > 0u)
INT8U         MT_StrLen               (INT8U           *psrc);
#endif

void          MT_TaskIdle             (void            *p_arg);

void          MT_TaskReturn           (void);

#if MT_TASK_STAT_EN > 0u
void          MT_TaskStat             (void            *p_arg);
#endif

#if (MT_TASK_STAT_STK_CHK_EN > 0u) && (MT_TASK_CREATE_EXT_EN > 0u)
void          MT_TaskStkClr           (MT_STK          *pbos,
                                       INT32U           size,
                                       INT16U           opt);
#endif

#if (MT_TASK_STAT_STK_CHK_EN > 0u) && (MT_TASK_CREATE_EXT_EN > 0u)
void          MT_TaskStatStkChk       (void);
#endif

INT8U         MT_TCBInit              (INT8U            prio,
                                       MT_STK          *ptos,
                                       MT_STK          *pbos,
                                       INT16U           id,
                                       INT32U           stk_size,
                                       void            *pext,
                                       INT16U           opt);

#if MT_TMR_EN > 0u
void          OSTmr_Init              (void);
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*                                      (Target Specific Functions)
*********************************************************************************************************
*/

#if MT_DEBUG_EN > 0u
void          OSDebugInit             (void);
#endif

void          OSInitHookBegin         (void);
void          OSInitHookEnd           (void);

void          OSTaskCreateHook        (MT_TCB          *ptcb);
void          OSTaskDelHook           (MT_TCB          *ptcb);

void          OSTaskIdleHook          (void);

void          OSTaskReturnHook        (MT_TCB          *ptcb);

void          OSTaskStatHook          (void);
MT_STK       *OSTaskStkInit           (void           (*task)(void *p_arg),
                                       void            *p_arg,
                                       MT_STK          *ptos,
                                       INT16U           opt);

#if MT_TASK_SW_HOOK_EN > 0u
void          OSTaskSwHook            (void);
#endif

void          OSTCBInitHook           (MT_TCB          *ptcb);

#if MT_TIME_TICK_HOOK_EN > 0u
void          OSTimeTickHook          (void);
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*                                   (Application Specific Functions)
*********************************************************************************************************
*/

#if MT_APP_HOOKS_EN > 0u
void          App_TaskCreateHook      (MT_TCB          *ptcb);
void          App_TaskDelHook         (MT_TCB          *ptcb);
void          App_TaskIdleHook        (void);

void          App_TaskReturnHook      (MT_TCB          *ptcb);

void          App_TaskStatHook        (void);

#if MT_TASK_SW_HOOK_EN > 0u
void          App_TaskSwHook          (void);
#endif

void          App_TCBInitHook         (MT_TCB          *ptcb);

#if MT_TIME_TICK_HOOK_EN > 0u
void          App_TimeTickHook        (void);
#endif
#endif

/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*
* IMPORTANT: These prototypes MUST be placed in MT_CPU.H
*********************************************************************************************************
*/

#if 0
void          OSStartHighRdy          (void);
void          OSIntCtxSw              (void);
void          OSCtxSw                 (void);
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                   LOOK FOR MISSING #define CONSTANTS
*
* This section is used to generate ERROR messages at compile time if certain #define constants are
* MISSING in MT_CFG.H.  This allows you to quickly determine the source of the error.
*
* You SHOULD NOT change this section UNLESS you would like to add more comments as to the source of the
* compile time error.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             EVENT FLAGS
*********************************************************************************************************
*/

#ifndef MT_FLAG_EN
#error  "MT_CFG.H, Missing MT_FLAG_EN: Enable (1) or Disable (0) code generation for Event Flags"
#else
    #ifndef MT_MAX_FLAGS
    #error  "MT_CFG.H, Missing MT_MAX_FLAGS: Max. number of Event Flag Groups in your application"
    #else
        #if     MT_MAX_FLAGS > 65500u
        #error  "MT_CFG.H, MT_MAX_FLAGS must be <= 65500"
        #endif
    #endif

    #ifndef MT_FLAGS_NBITS
    #error  "MT_CFG.H, Missing MT_FLAGS_NBITS: Determine #bits used for event flags, MUST be either 8, 16 or 32"
    #endif

    #ifndef MT_FLAG_WAIT_CLR_EN
    #error  "MT_CFG.H, Missing MT_FLAG_WAIT_CLR_EN: Include code for Wait on Clear EVENT FLAGS"
    #endif

    #ifndef MT_FLAG_ACCEPT_EN
    #error  "MT_CFG.H, Missing MT_FLAG_ACCEPT_EN: Include code for OSFlagAccept()"
    #endif

    #ifndef MT_FLAG_DEL_EN
    #error  "MT_CFG.H, Missing MT_FLAG_DEL_EN: Include code for OSFlagDel()"
    #endif

    #ifndef MT_FLAG_NAME_EN
    #error  "MT_CFG.H, Missing MT_FLAG_NAME_EN: Enable flag group names"
    #endif

    #ifndef MT_FLAG_QUERY_EN
    #error  "MT_CFG.H, Missing MT_FLAG_QUERY_EN: Include code for OSFlagQuery()"
    #endif
#endif

/*
*********************************************************************************************************
*                                          MESSAGE MAILBOXES
*********************************************************************************************************
*/

#ifndef MT_MBOX_EN
#error  "MT_CFG.H, Missing MT_MBOX_EN: Enable (1) or Disable (0) code generation for MAILBOXES"
#else
    #ifndef MT_MBOX_ACCEPT_EN
    #error  "MT_CFG.H, Missing MT_MBOX_ACCEPT_EN: Include code for OSMboxAccept()"
    #endif

    #ifndef MT_MBOX_DEL_EN
    #error  "MT_CFG.H, Missing MT_MBOX_DEL_EN: Include code for OSMboxDel()"
    #endif

    #ifndef MT_MBOX_PEND_ABORT_EN
    #error  "MT_CFG.H, Missing MT_MBOX_PEND_ABORT_EN: Include code for OSMboxPendAbort()"
    #endif

    #ifndef MT_MBOX_POST_EN
    #error  "MT_CFG.H, Missing MT_MBOX_POST_EN: Include code for OSMboxPost()"
    #endif

    #ifndef MT_MBOX_POST_OPT_EN
    #error  "MT_CFG.H, Missing MT_MBOX_POST_OPT_EN: Include code for OSMboxPostOpt()"
    #endif

    #ifndef MT_MBOX_QUERY_EN
    #error  "MT_CFG.H, Missing MT_MBOX_QUERY_EN: Include code for OSMboxQuery()"
    #endif
#endif

/*
*********************************************************************************************************
*                                          MEMORY MANAGEMENT
*********************************************************************************************************
*/

#ifndef MT_MEM_EN
#error  "MT_CFG.H, Missing MT_MEM_EN: Enable (1) or Disable (0) code generation for MEMORY MANAGER"
#else
    #ifndef MT_MAX_MEM_PART
    #error  "MT_CFG.H, Missing MT_MAX_MEM_PART: Max. number of memory partitions"
    #else
        #if     MT_MAX_MEM_PART > 65500u
        #error  "MT_CFG.H, MT_MAX_MEM_PART must be <= 65500"
        #endif
    #endif

    #ifndef MT_MEM_NAME_EN
    #error  "MT_CFG.H, Missing MT_MEM_NAME_EN: Enable memory partition names"
    #endif

    #ifndef MT_MEM_QUERY_EN
    #error  "MT_CFG.H, Missing MT_MEM_QUERY_EN: Include code for OSMemQuery()"
    #endif
#endif

/*
*********************************************************************************************************
*                                     MUTUAL EXCLUSION SEMAPHORES
*********************************************************************************************************
*/

#ifndef MT_MUTEX_EN
#error  "MT_CFG.H, Missing MT_MUTEX_EN: Enable (1) or Disable (0) code generation for MUTEX"
#else
    #ifndef MT_MUTEX_ACCEPT_EN
    #error  "MT_CFG.H, Missing MT_MUTEX_ACCEPT_EN: Include code for OSMutexAccept()"
    #endif

    #ifndef MT_MUTEX_DEL_EN
    #error  "MT_CFG.H, Missing MT_MUTEX_DEL_EN: Include code for OSMutexDel()"
    #endif

    #ifndef MT_MUTEX_QUERY_EN
    #error  "MT_CFG.H, Missing MT_MUTEX_QUERY_EN: Include code for OSMutexQuery()"
    #endif
#endif

/*
*********************************************************************************************************
*                                           MESSAGE QUEUES
*********************************************************************************************************
*/

#ifndef MT_Q_EN
#error  "MT_CFG.H, Missing MT_Q_EN: Enable (1) or Disable (0) code generation for QUEUES"
#else
    #ifndef MT_MAX_QS
    #error  "MT_CFG.H, Missing MT_MAX_QS: Max. number of queue control blocks"
    #else
        #if     MT_MAX_QS > 65500u
        #error  "MT_CFG.H, MT_MAX_QS must be <= 65500"
        #endif
    #endif

    #ifndef MT_Q_ACCEPT_EN
    #error  "MT_CFG.H, Missing MT_Q_ACCEPT_EN: Include code for OSQAccept()"
    #endif

    #ifndef MT_Q_DEL_EN
    #error  "MT_CFG.H, Missing MT_Q_DEL_EN: Include code for OSQDel()"
    #endif

    #ifndef MT_Q_FLUSH_EN
    #error  "MT_CFG.H, Missing MT_Q_FLUSH_EN: Include code for OSQFlush()"
    #endif

    #ifndef MT_Q_PEND_ABORT_EN
    #error  "MT_CFG.H, Missing MT_Q_PEND_ABORT_EN: Include code for OSQPendAbort()"
    #endif

    #ifndef MT_Q_POST_EN
    #error  "MT_CFG.H, Missing MT_Q_POST_EN: Include code for OSQPost()"
    #endif

    #ifndef MT_Q_POST_FRONT_EN
    #error  "MT_CFG.H, Missing MT_Q_POST_FRONT_EN: Include code for OSQPostFront()"
    #endif

    #ifndef MT_Q_POST_OPT_EN
    #error  "MT_CFG.H, Missing MT_Q_POST_OPT_EN: Include code for OSQPostOpt()"
    #endif

    #ifndef MT_Q_QUERY_EN
    #error  "MT_CFG.H, Missing MT_Q_QUERY_EN: Include code for OSQQuery()"
    #endif
#endif

/*
*********************************************************************************************************
*                                             SEMAPHORES
*********************************************************************************************************
*/

#ifndef MT_SEM_EN
#error  "MT_CFG.H, Missing MT_SEM_EN: Enable (1) or Disable (0) code generation for SEMAPHORES"
#else
    #ifndef MT_SEM_ACCEPT_EN
    #error  "MT_CFG.H, Missing MT_SEM_ACCEPT_EN: Include code for OSSemAccept()"
    #endif

    #ifndef MT_SEM_DEL_EN
    #error  "MT_CFG.H, Missing MT_SEM_DEL_EN: Include code for OSSemDel()"
    #endif

    #ifndef MT_SEM_PEND_ABORT_EN
    #error  "MT_CFG.H, Missing MT_SEM_PEND_ABORT_EN: Include code for OSSemPendAbort()"
    #endif

    #ifndef MT_SEM_QUERY_EN
    #error  "MT_CFG.H, Missing MT_SEM_QUERY_EN: Include code for OSSemQuery()"
    #endif

    #ifndef MT_SEM_SET_EN
    #error  "MT_CFG.H, Missing MT_SEM_SET_EN: Include code for OSSemSet()"
    #endif
#endif

/*
*********************************************************************************************************
*                                           TASK MANAGEMENT
*********************************************************************************************************
*/

#ifndef MT_MAX_TASKS
#error  "MT_CFG.H, Missing MT_MAX_TASKS: Max. number of tasks in your application"
#else
    #if     MT_MAX_TASKS < 2u
    #error  "MT_CFG.H,         MT_MAX_TASKS must be >= 2"
    #endif

    #if     MT_MAX_TASKS >  ((MT_LOWEST_PRIO - MT_N_SYS_TASKS) + 1u)
    #error  "MT_CFG.H,         MT_MAX_TASKS must be <= MT_LOWEST_PRIO - MT_N_SYS_TASKS + 1"
    #endif

#endif

#if     MT_LOWEST_PRIO >  254u
#error  "MT_CFG.H,         MT_LOWEST_PRIO must be <= 254 in V2.8x and higher"
#endif

#ifndef MT_TASK_IDLE_STK_SIZE
#error  "MT_CFG.H, Missing MT_TASK_IDLE_STK_SIZE: Idle task stack size"
#endif

#ifndef MT_TASK_STAT_EN
#error  "MT_CFG.H, Missing MT_TASK_STAT_EN: Enable (1) or Disable(0) the statistics task"
#endif

#ifndef MT_TASK_STAT_STK_SIZE
#error  "MT_CFG.H, Missing MT_TASK_STAT_STK_SIZE: Statistics task stack size"
#endif

#ifndef MT_TASK_STAT_STK_CHK_EN
#error  "MT_CFG.H, Missing MT_TASK_STAT_STK_CHK_EN: Check task stacks from statistics task"
#endif

#ifndef MT_TASK_CHANGE_PRIO_EN
#error  "MT_CFG.H, Missing MT_TASK_CHANGE_PRIO_EN: Include code for OSTaskChangePrio()"
#endif

#ifndef MT_TASK_CREATE_EN
#error  "MT_CFG.H, Missing MT_TASK_CREATE_EN: Include code for OSTaskCreate()"
#endif

#ifndef MT_TASK_CREATE_EXT_EN
#error  "MT_CFG.H, Missing MT_TASK_CREATE_EXT_EN: Include code for OSTaskCreateExt()"
#else
    #if (MT_TASK_CREATE_EXT_EN == 0u) && (MT_TASK_CREATE_EN == 0u)
    #error  "MT_CFG.H,         MT_TASK_CREATE_EXT_EN or MT_TASK_CREATE_EN must be Enable (1)"
    #endif
#endif

#ifndef MT_TASK_DEL_EN
#error  "MT_CFG.H, Missing MT_TASK_DEL_EN: Include code for OSTaskDel()"
#endif

#ifndef MT_TASK_NAME_EN
#error  "MT_CFG.H, Missing MT_TASK_NAME_EN: Enable task names"
#endif

#ifndef MT_TASK_SUSPEND_EN
#error  "MT_CFG.H, Missing MT_TASK_SUSPEND_EN: Include code for OSTaskSuspend() and OSTaskResume()"
#endif

#ifndef MT_TASK_QUERY_EN
#error  "MT_CFG.H, Missing MT_TASK_QUERY_EN: Include code for OSTaskQuery()"
#endif

#ifndef MT_TASK_REG_TBL_SIZE
#error  "MT_CFG.H, Missing MT_TASK_REG_TBL_SIZE: Include code for task specific registers"
#else
    #if     MT_TASK_REG_TBL_SIZE > 255u
    #error  "MT_CFG.H,         MT_TASK_REG_TBL_SIZE must be <= 255"
    #endif
#endif

/*
*********************************************************************************************************
*                                           TIME MANAGEMENT
*********************************************************************************************************
*/

#ifndef MT_TICKS_PER_SEC
#error  "MT_CFG.H, Missing MT_TICKS_PER_SEC: Sets the number of ticks in one second"
#endif

#ifndef MT_TIME_DLY_HMSM_EN
#error  "MT_CFG.H, Missing MT_TIME_DLY_HMSM_EN: Include code for OSTimeDlyHMSM()"
#endif

#ifndef MT_TIME_DLY_RESUME_EN
#error  "MT_CFG.H, Missing MT_TIME_DLY_RESUME_EN: Include code for OSTimeDlyResume()"
#endif

#ifndef MT_TIME_GET_SET_EN
#error  "MT_CFG.H, Missing MT_TIME_GET_SET_EN: Include code for OSTimeGet() and OSTimeSet()"
#endif

/*
*********************************************************************************************************
*                                          TIMER MANAGEMENT
*********************************************************************************************************
*/

#ifndef MT_TMR_EN
#error  "MT_CFG.H, Missing MT_TMR_EN: When (1) enables code generation for Timer Management"
#elif   MT_TMR_EN > 0u
    #if     MT_SEM_EN == 0u
    #error  "MT_CFG.H, Semaphore management is required (set MT_SEM_EN to 1) when enabling Timer Management."
    #error  "          Timer management require TWO semaphores."
    #endif

    #ifndef MT_TMR_CFG_MAX
    #error  "MT_CFG.H, Missing MT_TMR_CFG_MAX: Determines the total number of timers in an application (2 .. 65500)"
    #else
        #if MT_TMR_CFG_MAX < 2u
        #error  "MT_CFG.H, MT_TMR_CFG_MAX should be between 2 and 65500"
        #endif

        #if MT_TMR_CFG_MAX > 65500u
        #error  "MT_CFG.H, MT_TMR_CFG_MAX should be between 2 and 65500"
        #endif
    #endif

    #ifndef MT_TMR_CFG_WHEEL_SIZE
    #error  "MT_CFG.H, Missing MT_TMR_CFG_WHEEL_SIZE: Sets the size of the timer wheel (1 .. 1023)"
    #else
        #if MT_TMR_CFG_WHEEL_SIZE < 2u
        #error  "MT_CFG.H, MT_TMR_CFG_WHEEL_SIZE should be between 2 and 1024"
        #endif

        #if MT_TMR_CFG_WHEEL_SIZE > 1024u
        #error  "MT_CFG.H, MT_TMR_CFG_WHEEL_SIZE should be between 2 and 1024"
        #endif
    #endif

    #ifndef MT_TMR_CFG_NAME_EN
    #error  "MT_CFG.H, Missing MT_TMR_CFG_NAME_EN: Enable Timer names"
    #endif

    #ifndef MT_TMR_CFG_TICKS_PER_SEC
    #error  "MT_CFG.H, Missing MT_TMR_CFG_TICKS_PER_SEC: Determines the rate at which the timer management task will run (Hz)"
    #endif

    #ifndef MT_TASK_TMR_STK_SIZE
    #error  "MT_CFG.H, Missing MT_TASK_TMR_STK_SIZE: Determines the size of the Timer Task's stack"
    #endif
#endif


/*
*********************************************************************************************************
*                                            MISCELLANEOUS
*********************************************************************************************************
*/

#ifndef MT_ARG_CHK_EN
#error  "MT_CFG.H, Missing MT_ARG_CHK_EN: Enable (1) or Disable (0) argument checking"
#endif


#ifndef MT_CPU_HOOKS_EN
#error  "MT_CFG.H, Missing MT_CPU_HOOKS_EN: MTK hooks are found in the processor port files when 1"
#endif


#ifndef MT_APP_HOOKS_EN
#error  "MT_CFG.H, Missing MT_APP_HOOKS_EN: Application-defined hooks are called from the MTK hooks"
#endif


#ifndef MT_DEBUG_EN
#error  "MT_CFG.H, Missing MT_DEBUG_EN: Allows you to include variables for debugging or not"
#endif


#ifndef MT_LOWEST_PRIO
#error  "MT_CFG.H, Missing MT_LOWEST_PRIO: Defines the lowest priority that can be assigned"
#endif


#ifndef MT_MAX_EVENTS
#error  "MT_CFG.H, Missing MT_MAX_EVENTS: Max. number of event control blocks in your application"
#else
    #if     MT_MAX_EVENTS > 65500u
    #error  "MT_CFG.H, MT_MAX_EVENTS must be <= 65500"
    #endif
#endif


#ifndef MT_SCHED_LOCK_EN
#error  "MT_CFG.H, Missing MT_SCHED_LOCK_EN: Include code for OSSchedLock() and OSSchedUnlock()"
#endif


#ifndef MT_EVENT_MULTI_EN
#error  "MT_CFG.H, Missing MT_EVENT_MULTI_EN: Include code for OSEventPendMulti()"
#endif


#ifndef MT_TASK_PROFILE_EN
#error  "MT_CFG.H, Missing MT_TASK_PROFILE_EN: Include data structure for run-time task profiling"
#endif


#ifndef MT_TASK_SW_HOOK_EN
#error  "MT_CFG.H, Missing MT_TASK_SW_HOOK_EN: Allows you to include the code for OSTaskSwHook() or not"
#endif


#ifndef MT_TICK_STEP_EN
#error  "MT_CFG.H, Missing MT_TICK_STEP_EN: Allows to 'step' one tick at a time with MT-View"
#endif


#ifndef MT_TIME_TICK_HOOK_EN
#error  "MT_CFG.H, Missing MT_TIME_TICK_HOOK_EN: Allows you to include the code for OSTimeTickHook() or not"
#endif

/*
*********************************************************************************************************
*                                         SAFETY CRITICAL USE
*********************************************************************************************************
*/

#ifdef SAFETY_CRITICAL_RELEASE

#if    MT_ARG_CHK_EN < 1u
#error "MT_CFG.H, MT_ARG_CHK_EN must be enabled for safety-critical release code"
#endif

#if    MT_APP_HOOKS_EN > 0u
#error "MT_CFG.H, MT_APP_HOOKS_EN must be disabled for safety-critical release code"
#endif

#if    MT_DEBUG_EN > 0u
#error "MT_CFG.H, MT_DEBUG_EN must be disabled for safety-critical release code"
#endif

#ifdef CANTATA
#error "MT_CFG.H, CANTATA must be disabled for safety-critical release code"
#endif

#ifdef MT_SCHED_LOCK_EN
#error "MT_CFG.H, MT_SCHED_LOCK_EN must be disabled for safety-critical release code"
#endif

#ifdef VSC_VALIDATION_MODE
#error "MT_CFG.H, VSC_VALIDATION_MODE must be disabled for safety-critical release code"
#endif

#if    MT_TASK_STAT_EN > 0u
#error "MT_CFG.H, MT_TASK_STAT_EN must be disabled for safety-critical release code"
#endif

#if    MT_TICK_STEP_EN > 0u
#error "MT_CFG.H, MT_TICK_STEP_EN must be disabled for safety-critical release code"
#endif

#if    MT_FLAG_EN > 0u
    #if    MT_FLAG_DEL_EN > 0
    #error "MT_CFG.H, MT_FLAG_DEL_EN must be disabled for safety-critical release code"
    #endif
#endif

#if    MT_MBOX_EN > 0u
    #if    MT_MBOX_DEL_EN > 0u
    #error "MT_CFG.H, MT_MBOX_DEL_EN must be disabled for safety-critical release code"
    #endif
#endif

#if    MT_MUTEX_EN > 0u
    #if    MT_MUTEX_DEL_EN > 0u
    #error "MT_CFG.H, MT_MUTEX_DEL_EN must be disabled for safety-critical release code"
    #endif
#endif

#if    MT_Q_EN > 0u
    #if    MT_Q_DEL_EN > 0u
    #error "MT_CFG.H, MT_Q_DEL_EN must be disabled for safety-critical release code"
    #endif
#endif

#if    MT_SEM_EN > 0u
    #if    MT_SEM_DEL_EN > 0u
    #error "MT_CFG.H, MT_SEM_DEL_EN must be disabled for safety-critical release code"
    #endif
#endif

#if    MT_TASK_DEL_EN > 0u
#error "MT_CFG.H, MT_TASK_DEL_EN must be disabled for safety-critical release code"
#endif

#if    MT_CRITICAL_METHOD != 3u
#error "MT_CPU.H, MT_CRITICAL_METHOD must be type 3 for safety-critical release code"
#endif

#endif  /* ------------------------ SAFETY_CRITICAL_RELEASE ------------------------ */

#ifdef __cplusplus
}
#endif

#endif
