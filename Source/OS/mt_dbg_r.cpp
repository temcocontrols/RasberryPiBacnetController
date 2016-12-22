

#define  MT_SOURCE

#include "mt.h"

/*
*********************************************************************************************************
*                                             DEBUG DATA
*********************************************************************************************************
*/

INT16U  const  OSDebugEn             = MT_DEBUG_EN;             /* Debug constants are defined below   */

#if MT_DEBUG_EN > 0u

INT32U  const  OSEndiannessTest      = 0x12345678uL;            /* Variable to test CPU endianness     */

INT16U  const  OSEventEn             = MT_EVENT_EN;
INT16U  const  OSEventMax            = MT_MAX_EVENTS;           /* Number of event control blocks      */
INT16U  const  OSEventNameEn         = MT_EVENT_NAME_EN;
#if (MT_EVENT_EN) && (MT_MAX_EVENTS > 0u)
INT16U  const  OSEventSize           = sizeof(MT_EVENT);        /* Size in Bytes of MT_EVENT           */
INT16U  const  OSEventTblSize        = sizeof(OSEventTbl);      /* Size of OSEventTbl[] in bytes       */
#else
INT16U  const  OSEventSize           = 0u;
INT16U  const  OSEventTblSize        = 0u;
#endif
INT16U  const  OSEventMultiEn        = MT_EVENT_MULTI_EN;


INT16U  const  OSFlagEn              = MT_FLAG_EN;
#if (MT_FLAG_EN > 0u) && (MT_MAX_FLAGS > 0u)
INT16U  const  OSFlagGrpSize         = sizeof(MT_FLAG_GRP);     /* Size in Bytes of MT_FLAG_GRP        */
INT16U  const  OSFlagNodeSize        = sizeof(MT_FLAG_NODE);    /* Size in Bytes of MT_FLAG_NODE       */
INT16U  const  OSFlagWidth           = sizeof(MT_FLAGS);        /* Width (in bytes) of MT_FLAGS        */
#else
INT16U  const  OSFlagGrpSize         = 0u;
INT16U  const  OSFlagNodeSize        = 0u;
INT16U  const  OSFlagWidth           = 0u;
#endif
INT16U  const  OSFlagMax             = MT_MAX_FLAGS;
INT16U  const  OSFlagNameEn          = MT_FLAG_NAME_EN;

INT16U  const  OSLowestPrio          = MT_LOWEST_PRIO;

INT16U  const  OSMboxEn              = MT_MBOX_EN;

INT16U  const  OSMemEn               = MT_MEM_EN;
INT16U  const  OSMemMax              = MT_MAX_MEM_PART;         /* Number of memory partitions         */
INT16U  const  OSMemNameEn           = MT_MEM_NAME_EN;
#if (MT_MEM_EN > 0u) && (MT_MAX_MEM_PART > 0u)
INT16U  const  OSMemSize             = sizeof(MT_MEM);          /* Mem. Partition header sine (bytes)  */
INT16U  const  OSMemTblSize          = sizeof(OSMemTbl);
#else
INT16U  const  OSMemSize             = 0u;
INT16U  const  OSMemTblSize          = 0u;
#endif
INT16U  const  OSMutexEn             = MT_MUTEX_EN;

INT16U  const  OSPtrSize             = sizeof(void *);          /* Size in Bytes of a pointer          */

INT16U  const  OSQEn                 = MT_Q_EN;
INT16U  const  OSQMax                = MT_MAX_QS;               /* Number of queues                    */
#if (MT_Q_EN > 0u) && (MT_MAX_QS > 0u)
INT16U  const  OSQSize               = sizeof(MT_Q);            /* Size in bytes of MT_Q structure     */
#else
INT16U  const  OSQSize               = 0u;
#endif

INT16U  const  OSRdyTblSize          = MT_RDY_TBL_SIZE;         /* Number of bytes in the ready table  */

INT16U  const  OSSemEn               = MT_SEM_EN;

INT16U  const  OSStkWidth            = sizeof(MT_STK);          /* Size in Bytes of a stack entry      */

INT16U  const  OSTaskCreateEn        = MT_TASK_CREATE_EN;
INT16U  const  OSTaskCreateExtEn     = MT_TASK_CREATE_EXT_EN;
INT16U  const  OSTaskDelEn           = MT_TASK_DEL_EN;
INT16U  const  OSTaskIdleStkSize     = MT_TASK_IDLE_STK_SIZE;
INT16U  const  OSTaskProfileEn       = MT_TASK_PROFILE_EN;
INT16U  const  OSTaskMax             = MT_MAX_TASKS + MT_N_SYS_TASKS; /* Total max. number of tasks    */
INT16U  const  OSTaskNameEn          = MT_TASK_NAME_EN;
INT16U  const  OSTaskStatEn          = MT_TASK_STAT_EN;
INT16U  const  OSTaskStatStkSize     = MT_TASK_STAT_STK_SIZE;
INT16U  const  OSTaskStatStkChkEn    = MT_TASK_STAT_STK_CHK_EN;
INT16U  const  OSTaskSwHookEn        = MT_TASK_SW_HOOK_EN;
INT16U  const  OSTaskRegTblSize      = MT_TASK_REG_TBL_SIZE;

INT16U  const  OSTCBPrioTblMax       = MT_LOWEST_PRIO + 1u;     /* Number of entries in OSTCBPrioTbl[] */
INT16U  const  OSTCBSize             = sizeof(MT_TCB);          /* Size in Bytes of MT_TCB             */
INT16U  const  OSTicksPerSec         = MT_TICKS_PER_SEC;
INT16U  const  OSTimeTickHookEn      = MT_TIME_TICK_HOOK_EN;
INT16U  const  OSVersionNbr          = MT_VERSION;

#if MT_TASK_CREATE_EXT_EN > 0u
#if defined(MT_TLS_TBL_SIZE) && (MT_TLS_TBL_SIZE > 0u)
INT16U  const  MT_TLS_TblSize        = MT_TLS_TBL_SIZE * sizeof(MT_TLS);
#else
INT16U  const  MT_TLS_TblSize        = 0u;
#endif
#endif

INT16U  const  OSTmrEn               = MT_TMR_EN;
INT16U  const  OSTmrCfgMax           = MT_TMR_CFG_MAX;
INT16U  const  OSTmrCfgNameEn        = MT_TMR_CFG_NAME_EN;
INT16U  const  OSTmrCfgWheelSize     = MT_TMR_CFG_WHEEL_SIZE;
INT16U  const  OSTmrCfgTicksPerSec   = MT_TMR_CFG_TICKS_PER_SEC;

#if (MT_TMR_EN > 0u) && (MT_TMR_CFG_MAX > 0u)
INT16U  const  OSTmrSize             = sizeof(MT_TMR);
INT16U  const  OSTmrTblSize          = sizeof(OSTmrTbl);
INT16U  const  OSTmrWheelSize        = sizeof(MT_TMR_WHEEL);
INT16U  const  OSTmrWheelTblSize     = sizeof(OSTmrWheelTbl);
#else
INT16U  const  OSTmrSize             = 0u;
INT16U  const  OSTmrTblSize          = 0u;
INT16U  const  OSTmrWheelSize        = 0u;
INT16U  const  OSTmrWheelTblSize     = 0u;
#endif

#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                             DEBUG DATA
*                            TOTAL DATA SPACE (i.e. RAM) USED BY MTK
*********************************************************************************************************
*/
#if MT_DEBUG_EN > 0u

INT16U  const  OSDataSize = sizeof(OSCtxSwCtr)
#if (MT_EVENT_EN) && (MT_MAX_EVENTS > 0u)
                          + sizeof(OSEventFreeList)
                          + sizeof(OSEventTbl)
#endif

#if (MT_FLAG_EN > 0u) && (MT_MAX_FLAGS > 0u)
                          + sizeof(OSFlagTbl)
                          + sizeof(OSFlagFreeList)
#endif

#if MT_TASK_STAT_EN > 0u
                          + sizeof(OSCPUUsage)
                          + sizeof(OSIdleCtrMax)
                          + sizeof(OSIdleCtrRun)
                          + sizeof(OSStatRdy)
                          + sizeof(OSTaskStatStk)
#endif

#if MT_TICK_STEP_EN > 0u
                          + sizeof(OSTickStepState)
#endif

#if (MT_MEM_EN > 0u) && (MT_MAX_MEM_PART > 0u)
                          + sizeof(OSMemFreeList)
                          + sizeof(OSMemTbl)
#endif

#ifdef MT_SAFETY_CRITICAL_IEC61508
                          + sizeof(OSSafetyCriticalStartFlag)
#endif

#if (MT_Q_EN > 0u) && (MT_MAX_QS > 0u)
                          + sizeof(OSQFreeList)
                          + sizeof(OSQTbl)
#endif

#if MT_TASK_REG_TBL_SIZE > 0u
                          + sizeof(OSTaskRegNextAvailID)
#endif

#if MT_TIME_GET_SET_EN > 0u
                          + sizeof(OSTime)
#endif

#if (MT_TMR_EN > 0u) && (MT_TMR_CFG_MAX > 0u)
                          + sizeof(OSTmrFree)
                          + sizeof(OSTmrUsed)
                          + sizeof(OSTmrTime)
                          + sizeof(OSTmrSem)
                          + sizeof(OSTmrSemSignal)
                          + sizeof(OSTmrTbl)
                          + sizeof(OSTmrFreeList)
                          + sizeof(OSTmrTaskStk)
                          + sizeof(OSTmrWheelTbl)
#endif
                          + sizeof(OSIntNesting)
                          + sizeof(OSLockNesting)
                          + sizeof(OSPrioCur)
                          + sizeof(OSPrioHighRdy)
                          + sizeof(OSRdyGrp)
                          + sizeof(OSRdyTbl)
                          + sizeof(OSRunning)
                          + sizeof(OSTaskCtr)
                          + sizeof(OSIdleCtr)
                          + sizeof(OSTaskIdleStk)
                          + sizeof(OSTCBCur)
                          + sizeof(OSTCBFreeList)
                          + sizeof(OSTCBHighRdy)
                          + sizeof(OSTCBList)
                          + sizeof(OSTCBPrioTbl)
                          + sizeof(OSTCBTbl);

#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                       OS DEBUG INITIALIZATION
*
* Description: This function is used to make sure that debug variables that are unused in the application
*              are not optimized away.  This function might not be necessary for all compilers.  In this
*              case, you should simply DELETE the code in this function while still leaving the declaration
*              of the function itself.
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : (1) This code doesn't do anything, it simply prevents the compiler from optimizing out
*                  the 'const' variables which are declared in this file.
*              (2) You may decide to 'compile out' the code (by using #if 0/#endif) INSIDE the function 
*                  if your compiler DOES NOT optimize out the 'const' variables above.
*********************************************************************************************************
*/

#if MT_DEBUG_EN > 0u
void  OSDebugInit (void)
{
    void  const  *ptemp;


    ptemp = (void const *)&OSDebugEn;

    ptemp = (void const *)&OSEndiannessTest;

    ptemp = (void const *)&OSEventMax;
    ptemp = (void const *)&OSEventNameEn;
    ptemp = (void const *)&OSEventEn;
    ptemp = (void const *)&OSEventSize;
    ptemp = (void const *)&OSEventTblSize;
    ptemp = (void const *)&OSEventMultiEn;

    ptemp = (void const *)&OSFlagEn;
    ptemp = (void const *)&OSFlagGrpSize;
    ptemp = (void const *)&OSFlagNodeSize;
    ptemp = (void const *)&OSFlagWidth;
    ptemp = (void const *)&OSFlagMax;
    ptemp = (void const *)&OSFlagNameEn;

    ptemp = (void const *)&OSLowestPrio;

    ptemp = (void const *)&OSMboxEn;

    ptemp = (void const *)&OSMemEn;
    ptemp = (void const *)&OSMemMax;
    ptemp = (void const *)&OSMemNameEn;
    ptemp = (void const *)&OSMemSize;
    ptemp = (void const *)&OSMemTblSize;

    ptemp = (void const *)&OSMutexEn;

    ptemp = (void const *)&OSPtrSize;

    ptemp = (void const *)&OSQEn;
    ptemp = (void const *)&OSQMax;
    ptemp = (void const *)&OSQSize;

    ptemp = (void const *)&OSRdyTblSize;

    ptemp = (void const *)&OSSemEn;

    ptemp = (void const *)&OSStkWidth;

    ptemp = (void const *)&OSTaskCreateEn;
    ptemp = (void const *)&OSTaskCreateExtEn;
    ptemp = (void const *)&OSTaskDelEn;
    ptemp = (void const *)&OSTaskIdleStkSize;
    ptemp = (void const *)&OSTaskProfileEn;
    ptemp = (void const *)&OSTaskMax;
    ptemp = (void const *)&OSTaskNameEn;
    ptemp = (void const *)&OSTaskStatEn;
    ptemp = (void const *)&OSTaskStatStkSize;
    ptemp = (void const *)&OSTaskStatStkChkEn;
    ptemp = (void const *)&OSTaskSwHookEn;

    ptemp = (void const *)&OSTCBPrioTblMax;
    ptemp = (void const *)&OSTCBSize;

    ptemp = (void const *)&OSTicksPerSec;
    ptemp = (void const *)&OSTimeTickHookEn;

#if MT_TMR_EN > 0u
    ptemp = (void const *)&OSTmrTbl[0];
    ptemp = (void const *)&OSTmrWheelTbl[0];

    ptemp = (void const *)&OSTmrEn;
    ptemp = (void const *)&OSTmrCfgMax;
    ptemp = (void const *)&OSTmrCfgNameEn;
    ptemp = (void const *)&OSTmrCfgWheelSize;
    ptemp = (void const *)&OSTmrCfgTicksPerSec;
    ptemp = (void const *)&OSTmrSize;
    ptemp = (void const *)&OSTmrTblSize;

    ptemp = (void const *)&OSTmrWheelSize;
    ptemp = (void const *)&OSTmrWheelTblSize;
#endif

    ptemp = (void const *)&OSVersionNbr;

    ptemp = (void const *)&OSDataSize;

    ptemp = ptemp;                             /* Prevent compiler warning for 'ptemp' not being used! */
}
#endif
