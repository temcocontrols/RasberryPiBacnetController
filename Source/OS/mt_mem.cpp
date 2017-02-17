#ifdef BAS_TEMP

#define  MT_SOURCE

#ifndef  MT_MASTER_FILE
#include "mt.h"
#endif

#if (MT_MEM_EN > 0u) && (MT_MAX_MEM_PART > 0u)
/*
*********************************************************************************************************
*                                      CREATE A MEMORY PARTITION
*
* Description : Create a fixed-sized memory partition that will be managed by MTK.
*
* Arguments   : addr     is the starting address of the memory partition
*
*               nblks    is the number of memory blocks to create from the partition.
*
*               blksize  is the size (in bytes) of each block in the memory partition.
*
*               perr     is a pointer to a variable containing an error message which will be set by
*                        this function to either:
*
*                        MT_ERR_NONE              if the memory partition has been created correctly.
*                        MT_ERR_MEM_INVALID_ADDR  if you are specifying an invalid address for the memory
*                                                 storage of the partition or, the block does not align
*                                                 on a pointer boundary
*                        MT_ERR_MEM_INVALID_PART  no free partitions available
*                        MT_ERR_MEM_INVALID_BLKS  user specified an invalid number of blocks (must be >= 2)
*                        MT_ERR_MEM_INVALID_SIZE  user specified an invalid block size
*                                                   - must be greater than the size of a pointer
*                                                   - must be able to hold an integral number of pointers
* Returns    : != (MT_MEM *)0  is the partition was created
*              == (MT_MEM *)0  if the partition was not created because of invalid arguments or, no
*                              free partition is available.
*********************************************************************************************************
*/

MT_MEM  *OSMemCreate (void   *addr,
                      INT32U  nblks,
                      INT32U  blksize,
                      INT8U  *perr)
{
    MT_MEM    *pmem;
    INT8U     *pblk;
    void     **plink;
    INT32U     loops;
    INT32U     i;
#if MT_CRITICAL_METHOD == 3u                          /* Allocate storage for CPU status register      */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#ifdef MT_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return ((MT_MEM *)0);
    }
#endif

#ifdef MT_SAFETY_CRITICAL_IEC61508
    if (OSSafetyCriticalStartFlag == MT_TRUE) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return ((MT_MEM *)0);
    }
#endif

#if MT_ARG_CHK_EN > 0u
    if (addr == (void *)0) {                          /* Must pass a valid address for the memory part.*/
        *perr = MT_ERR_MEM_INVALID_ADDR;
        return ((MT_MEM *)0);
    }
    if (((INT32U)addr & (sizeof(void *) - 1u)) != 0u){  /* Must be pointer size aligned                */
        *perr = MT_ERR_MEM_INVALID_ADDR;
        return ((MT_MEM *)0);
    }
    if (nblks < 2u) {                                 /* Must have at least 2 blocks per partition     */
        *perr = MT_ERR_MEM_INVALID_BLKS;
        return ((MT_MEM *)0);
    }
    if (blksize < sizeof(void *)) {                   /* Must contain space for at least a pointer     */
        *perr = MT_ERR_MEM_INVALID_SIZE;
        return ((MT_MEM *)0);
    }
#endif
    MT_ENTER_CRITICAL();
    pmem = OSMemFreeList;                             /* Get next free memory partition                */
    if (OSMemFreeList != (MT_MEM *)0) {               /* See if pool of free partitions was empty      */
        OSMemFreeList = (MT_MEM *)OSMemFreeList->OSMemFreeList;
    }
    MT_EXIT_CRITICAL();
    if (pmem == (MT_MEM *)0) {                        /* See if we have a memory partition             */
        *perr = MT_ERR_MEM_INVALID_PART;
        return ((MT_MEM *)0);
    }
    plink = (void **)addr;                            /* Create linked list of free memory blocks      */
    pblk  = (INT8U *)addr;
    loops  = nblks - 1u;
    for (i = 0u; i < loops; i++) {
        pblk +=  blksize;                             /* Point to the FOLLOWING block                  */
       *plink = (void  *)pblk;                        /* Save pointer to NEXT block in CURRENT block   */
        plink = (void **)pblk;                        /* Position to  NEXT      block                  */
    }
    *plink              = (void *)0;                  /* Last memory block points to NULL              */
    pmem->OSMemAddr     = addr;                       /* Store start address of memory partition       */
    pmem->OSMemFreeList = addr;                       /* Initialize pointer to pool of free blocks     */
    pmem->OSMemNFree    = nblks;                      /* Store number of free blocks in MCB            */
    pmem->OSMemNBlks    = nblks;
    pmem->OSMemBlkSize  = blksize;                    /* Store block size of each memory blocks        */
    *perr               = MT_ERR_NONE;
    return (pmem);
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                         GET A MEMORY BLOCK
*
* Description : Get a memory block from a partition
*
* Arguments   : pmem    is a pointer to the memory partition control block
*
*               perr    is a pointer to a variable containing an error message which will be set by this
*                       function to either:
*
*                       MT_ERR_NONE             if the memory partition has been created correctly.
*                       MT_ERR_MEM_NO_FREE_BLKS if there are no more free memory blocks to allocate to caller
*                       MT_ERR_MEM_INVALID_PMEM if you passed a NULL pointer for 'pmem'
*
* Returns     : A pointer to a memory block if no error is detected
*               A pointer to NULL if an error is detected
*********************************************************************************************************
*/

void  *OSMemGet (MT_MEM  *pmem,
                 INT8U   *perr)
{
    void      *pblk;
#if MT_CRITICAL_METHOD == 3u                          /* Allocate storage for CPU status register      */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#ifdef MT_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return ((void *)0);
    }
#endif

#if MT_ARG_CHK_EN > 0u
    if (pmem == (MT_MEM *)0) {                        /* Must point to a valid memory partition        */
        *perr = MT_ERR_MEM_INVALID_PMEM;
        return ((void *)0);
    }
#endif
    MT_ENTER_CRITICAL();
    if (pmem->OSMemNFree > 0u) {                      /* See if there are any free memory blocks       */
        pblk                = pmem->OSMemFreeList;    /* Yes, point to next free memory block          */
        pmem->OSMemFreeList = *(void **)pblk;         /*      Adjust pointer to new free list          */
        pmem->OSMemNFree--;                           /*      One less memory block in this partition  */
        MT_EXIT_CRITICAL();
        *perr = MT_ERR_NONE;                          /*      No error                                 */
        return (pblk);                                /*      Return memory block to caller            */
    }
    MT_EXIT_CRITICAL();
    *perr = MT_ERR_MEM_NO_FREE_BLKS;                  /* No,  Notify caller of empty memory partition  */
    return ((void *)0);                               /*      Return NULL pointer to caller            */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                 GET THE NAME OF A MEMORY PARTITION
*
* Description: This function is used to obtain the name assigned to a memory partition.
*
* Arguments  : pmem      is a pointer to the memory partition
*
*              pname     is a pointer to a pointer to an ASCII string that will receive the name of the memory partition.
*
*              perr      is a pointer to an error code that can contain one of the following values:
*
*                        MT_ERR_NONE                if the name was copied to 'pname'
*                        MT_ERR_MEM_INVALID_PMEM    if you passed a NULL pointer for 'pmem'
*                        MT_ERR_PNAME_NULL          You passed a NULL pointer for 'pname'
*                        MT_ERR_NAME_GET_ISR        You called this function from an ISR
*
* Returns    : The length of the string or 0 if 'pmem' is a NULL pointer.
*********************************************************************************************************
*/

#if MT_MEM_NAME_EN > 0u
INT8U  OSMemNameGet (MT_MEM   *pmem,
                     INT8U   **pname,
                     INT8U    *perr)
{
    INT8U      len;
#if MT_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#ifdef MT_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return (0u);
    }
#endif

#if MT_ARG_CHK_EN > 0u
    if (pmem == (MT_MEM *)0) {                   /* Is 'pmem' a NULL pointer?                          */
        *perr = MT_ERR_MEM_INVALID_PMEM;
        return (0u);
    }
    if (pname == (INT8U **)0) {                  /* Is 'pname' a NULL pointer?                         */
        *perr = MT_ERR_PNAME_NULL;
        return (0u);
    }
#endif
    if (OSIntNesting > 0u) {                     /* See if trying to call from an ISR                  */
        *perr = MT_ERR_NAME_GET_ISR;
        return (0u);
    }
    MT_ENTER_CRITICAL();
    *pname = pmem->OSMemName;
    len    = MT_StrLen(*pname);
    MT_EXIT_CRITICAL();
    *perr  = MT_ERR_NONE;
    return (len);
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                 ASSIGN A NAME TO A MEMORY PARTITION
*
* Description: This function assigns a name to a memory partition.
*
* Arguments  : pmem      is a pointer to the memory partition
*
*              pname     is a pointer to an ASCII string that contains the name of the memory partition.
*
*              perr      is a pointer to an error code that can contain one of the following values:
*
*                        MT_ERR_NONE                if the name was copied to 'pname'
*                        MT_ERR_MEM_INVALID_PMEM    if you passed a NULL pointer for 'pmem'
*                        MT_ERR_PNAME_NULL          You passed a NULL pointer for 'pname'
*                        MT_ERR_MEM_NAME_TOO_LONG   if the name doesn't fit in the storage area
*                        MT_ERR_NAME_SET_ISR        if you called this function from an ISR
*
* Returns    : None
*********************************************************************************************************
*/

#if MT_MEM_NAME_EN > 0u
void  OSMemNameSet (MT_MEM  *pmem,
                    INT8U   *pname,
                    INT8U   *perr)
{
#if MT_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#ifdef MT_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        MT_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if MT_ARG_CHK_EN > 0u
    if (pmem == (MT_MEM *)0) {                   /* Is 'pmem' a NULL pointer?                          */
        *perr = MT_ERR_MEM_INVALID_PMEM;
        return;
    }
    if (pname == (INT8U *)0) {                   /* Is 'pname' a NULL pointer?                         */
        *perr = MT_ERR_PNAME_NULL;
        return;
    }
#endif
    if (OSIntNesting > 0u) {                     /* See if trying to call from an ISR                  */
        *perr = MT_ERR_NAME_SET_ISR;
        return;
    }
    MT_ENTER_CRITICAL();
    pmem->OSMemName = pname;
    MT_EXIT_CRITICAL();
    *perr           = MT_ERR_NONE;
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                       RELEASE A MEMORY BLOCK
*
* Description : Returns a memory block to a partition
*
* Arguments   : pmem    is a pointer to the memory partition control block
*
*               pblk    is a pointer to the memory block being released.
*
* Returns     : MT_ERR_NONE              if the memory block was inserted into the partition
*               MT_ERR_MEM_FULL          if you are returning a memory block to an already FULL memory
*                                        partition (You freed more blocks than you allocated!)
*               MT_ERR_MEM_INVALID_PMEM  if you passed a NULL pointer for 'pmem'
*               MT_ERR_MEM_INVALID_PBLK  if you passed a NULL pointer for the block to release.
*********************************************************************************************************
*/

INT8U  OSMemPut (MT_MEM  *pmem,
                 void    *pblk)
{
#if MT_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#if MT_ARG_CHK_EN > 0u
    if (pmem == (MT_MEM *)0) {                   /* Must point to a valid memory partition             */
        return (MT_ERR_MEM_INVALID_PMEM);
    }
    if (pblk == (void *)0) {                     /* Must release a valid block                         */
        return (MT_ERR_MEM_INVALID_PBLK);
    }
#endif
    MT_ENTER_CRITICAL();
    if (pmem->OSMemNFree >= pmem->OSMemNBlks) {  /* Make sure all blocks not already returned          */
        MT_EXIT_CRITICAL();
        return (MT_ERR_MEM_FULL);
    }
    *(void **)pblk      = pmem->OSMemFreeList;   /* Insert released block into free block list         */
    pmem->OSMemFreeList = pblk;
    pmem->OSMemNFree++;                          /* One more memory block in this partition            */
    MT_EXIT_CRITICAL();
    return (MT_ERR_NONE);                        /* Notify caller that memory block was released       */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                       QUERY MEMORY PARTITION
*
* Description : This function is used to determine the number of free memory blocks and the number of
*               used memory blocks from a memory partition.
*
* Arguments   : pmem        is a pointer to the memory partition control block
*
*               p_mem_data  is a pointer to a structure that will contain information about the memory
*                           partition.
*
* Returns     : MT_ERR_NONE               if no errors were found.
*               MT_ERR_MEM_INVALID_PMEM   if you passed a NULL pointer for 'pmem'
*               MT_ERR_MEM_INVALID_PDATA  if you passed a NULL pointer to the data recipient.
*********************************************************************************************************
*/

#if MT_MEM_QUERY_EN > 0u
INT8U  OSMemQuery (MT_MEM       *pmem,
                   MT_MEM_DATA  *p_mem_data)
{
#if MT_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    MT_CPU_SR  cpu_sr = 0u;
#endif



#if MT_ARG_CHK_EN > 0u
    if (pmem == (MT_MEM *)0) {                   /* Must point to a valid memory partition             */
        return (MT_ERR_MEM_INVALID_PMEM);
    }
    if (p_mem_data == (MT_MEM_DATA *)0) {        /* Must release a valid storage area for the data     */
        return (MT_ERR_MEM_INVALID_PDATA);
    }
#endif
    MT_ENTER_CRITICAL();
    p_mem_data->OSAddr     = pmem->OSMemAddr;
    p_mem_data->OSFreeList = pmem->OSMemFreeList;
    p_mem_data->OSBlkSize  = pmem->OSMemBlkSize;
    p_mem_data->OSNBlks    = pmem->OSMemNBlks;
    p_mem_data->OSNFree    = pmem->OSMemNFree;
    MT_EXIT_CRITICAL();
    p_mem_data->OSNUsed    = p_mem_data->OSNBlks - p_mem_data->OSNFree;
    return (MT_ERR_NONE);
}
#endif                                           /* MT_MEM_QUERY_EN                                    */
/*$PAGE*/
/*
*********************************************************************************************************
*                                 INITIALIZE MEMORY PARTITION MANAGER
*
* Description : This function is called by MTK to initialize the memory partition manager.  Your
*               application MUST NOT call this function.
*
* Arguments   : none
*
* Returns     : none
*
* Note(s)    : This function is INTERNAL to MTK and your application should not call it.
*********************************************************************************************************
*/

void  MT_MemInit (void)
{
#if MT_MAX_MEM_PART == 1u
    MT_MemClr((INT8U *)&OSMemTbl[0], sizeof(OSMemTbl));   /* Clear the memory partition table          */
    OSMemFreeList               = (MT_MEM *)&OSMemTbl[0]; /* Point to beginning of free list           */
#if MT_MEM_NAME_EN > 0u
    OSMemFreeList->OSMemName    = (INT8U *)"?";           /* Unknown name                              */
#endif
#endif

#if MT_MAX_MEM_PART >= 2u
    MT_MEM  *pmem;
    INT16U   i;


    MT_MemClr((INT8U *)&OSMemTbl[0], sizeof(OSMemTbl));   /* Clear the memory partition table          */
    for (i = 0u; i < (MT_MAX_MEM_PART - 1u); i++) {       /* Init. list of free memory partitions      */
        pmem                = &OSMemTbl[i];               /* Point to memory control block (MCB)       */
        pmem->OSMemFreeList = (void *)&OSMemTbl[i + 1u];  /* Chain list of free partitions             */
#if MT_MEM_NAME_EN > 0u
        pmem->OSMemName  = (INT8U *)(void *)"?";
#endif
    }
    pmem                = &OSMemTbl[i];
    pmem->OSMemFreeList = (void *)0;                      /* Initialize last node                      */
#if MT_MEM_NAME_EN > 0u
    pmem->OSMemName = (INT8U *)(void *)"?";
#endif

    OSMemFreeList   = &OSMemTbl[0];                       /* Point to beginning of free list           */
#endif
}
#endif                                                    /* MT_MEM_EN                                 */
#endif //BAS_TEMP
