/*
*********************************************************************************************************
*                                                MTK
*                                          The Real-Time Kernel
*                                     Freescale  MPC55xx Specific code
*
*                                 (c) Copyright 2007; Micrium; Weston, FL
*                                           All Rights Reserved
*
* File    : APP_CFG.H
* By      : Fabiano Kovalski
*********************************************************************************************************
*/

#ifndef _APP_CFG_H_
#define _APP_CFG_H_

/*
****************************************************************************************************
*                                          TASK PRIORITIES
****************************************************************************************************
*/
#define APP_TASK_START_PRIO     0

#define APP_TASK_1_PRIO         11
#define APP_TASK_2_PRIO         12
#define APP_TASK_3_PRIO         13
#define APP_TASK_4_PRIO         14
#define APP_TASK_5_PRIO         15
#define APP_TASK_6_PRIO         16

#define OS_TASK_TMR_PRIO        8


/*
****************************************************************************************************
*                                         TASK STACK SIZES
****************************************************************************************************
*/

#define APP_TASK_START_STK_SIZE      256

#define APP_TASK_1_STK_SIZE          OSMinStkSize()
#define APP_TASK_2_STK_SIZE          OSMinStkSize()
#define APP_TASK_3_STK_SIZE          OSMinStkSize()
#define APP_TASK_4_STK_SIZE          OSMinStkSize()
#define APP_TASK_5_STK_SIZE          OSMinStkSize()
#define APP_TASK_6_STK_SIZE          OSMinStkSize()


#endif  /* #ifndef _APP_CFG_H_ */

