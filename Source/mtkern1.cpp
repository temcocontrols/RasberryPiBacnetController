/******************************************************************************
 * File Name: mtkern1.cpp
 * 
 * Description: 
 *
 * Created:
 * Author:
 *****************************************************************************/

/******************************************************************************
 * INCLUDES
 *****************************************************************************/
 
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <syscall.h>
#include <stdio.h>
#include "t3000def.h"
#include "mtkernel.h"
#include "aio.h"
#include "baseclas.h"
//#include "vga12.h"
//#include "netbios.h"
#include "rs485.h"
#include "rs232.h"
#include "ptp.h"

/******************************************************************************
 * GLOBALs
 *****************************************************************************/

extern pthread_t appThread[NUM_TASKS];

void kill_task(int id);
void msleep( int id, int ticks );
void suspend( int id );
void resume( int id );
void resume_suspend( int, int );

//void HookHandlers(void);
//void UnhookHandlers(void);

/*
#ifdef NETWORK_COMM
extern unsigned int number_of_remote_points;
extern unsigned long program_interval;
extern int task_control(void);

char huge stack_NETWORK[stack_NETWORK_SIZE];
#endif
*/

#ifdef BAS_TEMP
#ifdef SERIAL_COMM
extern int int_occured;
#endif
extern int 	T_Inactivity;
extern int timepoints, timepoints_ipx;
extern long timesave;
//extern char virtual_not_run;
extern char	check_annual_routine_flag;
extern char	readclock_flag;

//extern PENDING_STATUS pr_call_state;

extern Panel *ptr_panel;
extern char disconnect_modem, action;
extern int ring_reset_time;
extern char ring_counts;
extern char control;
extern int refresh_time;
extern unsigned long grp_time;
extern unsigned long display_grp_time;
extern int alarm_time;
extern int worktime;
extern long refresh_graph_time;
extern char present_analog_monitor;
extern char *ontext;

extern char timeout, timeout_asyn;
extern int disconnect_time;

/* Timer interrupt task sheduller */
//extern unsigned char *ptr_dos_flag;

//extern int_regs *r;

extern int time_key;
extern char simulate;
extern int time_last_key;
//extern unsigned long timesec1970;  // sec la inceputul programului
extern unsigned long timestart;    // sec de la inceputul programului

extern char month_days[12];
extern unsigned long  ora_current_sec; // ora curenta exprimata in secunde
//struct  time  ora_current;      // ora curenta
extern Time_block ora_current;
extern int milisec;
extern long microsec;
//char dayofweek;
extern char onesec,onemin,tenmin,tensec;
extern int onesec_virtual;
//extern int pixvar;
extern int miliseclast;
//extern byte console_mode;

extern int indarray;
extern int int_dos;

extern unsigned real_ss;
#endif //BAS_TEMP

extern task_struct tasks[NUM_TASKS];
extern sem_t read_mon_flag;
extern sem_t dos_flag;
extern sem_t screen; 
//extern sem_t memory;
extern sem_t t3000_flag;
extern sem_t print_sem;
#ifdef BAS_TEMP
extern unsigned dos_host;
extern unsigned oldss, oldsp;
extern int tswitch;      		// task index
extern int setvectint8;
//extern int communication;
extern char timecount;
extern char run_board_flag;
extern int def_macro;
static char tasking = 1;           // tasking system enabled
static char single_task = 0;       // single task flag off
static unsigned pri,i,j;

#ifdef TEST
 char *num, *a = "0";
#endif
#endif //BAS_TEMP

pthread_mutex_t taskLock[NUM_TASKS]; 
pthread_cond_t  taskSuspendSig[NUM_TASKS]; 

/******************************************************************************
 * FUNCTION DEFINATIONs
 *****************************************************************************/

void InitTick()
{
   ualarm(1000000/MT_TICKS_PER_SEC, 1000000/MT_TICKS_PER_SEC);
}

void InitLinuxPort()
{
#ifdef BAS_TEMP
	//setup signal handlers
	struct sigaction act;
	sigset_t mask;
	sigemptyset(&mask);

	act.sa_sigaction = (void (*)(int, siginfo_t*, void*))int8_task_switch;
	act.sa_flags = 0;
	act.sa_mask = mask;
	if ( sigaction(SIGALRM, &act, NULL) )
			printf("Sigaction failed for SIGALRM\n" );

	//Block all signals in this the main thread. It should not call any signal handler
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGALRM);
	//InitTick();
	sigprocmask(SIG_UNBLOCK, &set, 0);
	//sigprocmask(SIG_BLOCK, &set,  0);
#endif //BAS_TEMP	
	// Init all semaphores
	sem_init(&read_mon_flag, 0, 1);
	sem_init(&dos_flag, 0, 1);
	sem_init(&screen, 0, 1);
	sem_init(&t3000_flag, 0, 1);
	sem_init(&print_sem, 0, 1);
}

// Kill a task. ( i.e., make it's state DEAD. )
void kill_task( int id )
{
	pthread_cancel(appThread[id]);
}

// Initialize the task control structures
void init_tasks( void )
{
	//TBD: Not Required. Remove it.
#ifdef BAS_TEMP
	for( i=0; i<NUM_TASKS; i++ )
	{
		//tasks[i].status = SUSPENDED;
		tasks[i].status = DEAD;
		tasks[i].pending = NULL;
		tasks[i].sleep = 0;
		tasks[i].pri = i;
	}
	tasks[PROJ].status = DEAD;
	set_vid_mem();
#endif //BAS_TEMP
}

void msleep( int id, int ticks )
{
	//TBB: This function needs to be made Linux compatible
	if(tasks[id].status==SUSPENDED || tasks[id].status==SLEEPING)
	{
	 disable();
	 tasks[id].sleep = ticks;
	 tasks[id].status = SLEEPING;
	 enable();
	}
}

// Suspend a task until resumed by another task
void suspend( int id )
{
	//TBD: Not As per requirement
	if ((id < 0) || (id > NUM_TASKS))
	{
		return;
	}
	
	pthread_mutex_lock(&taskLock[id]); 
	//while (count == 0) 
		pthread_cond_wait(&taskSuspendSig[id], &taskLock[id]); 
	//count = count - 1;
	pthread_mutex_unlock(&taskLock[id]); 
}

// Restart a previously suspended task.
void resume( int id )
{
	//TBD: Not As per requirement
	
	if((id < 0 || id > NUM_TASKS))
	{
		return;
	}

	pthread_mutex_lock(&taskLock[id]); 
	//if (count == 0)
		pthread_cond_signal(&taskSuspendSig[id]); 
	//count = count + 1;
	pthread_mutex_unlock(&taskLock[id]);
}

void resume_suspend( int id_res, int id_susp )
{
	resume( id_res );
	suspend( id_susp );
}

void blocked_resume( int id )
{
	resume(id );
}

// Wait for a semaphore
void set_semaphore( sem_t *sem )
{
	sem_wait (sem);
}
#ifdef BAS_TEMP
void set_semaphore_dos(void)
{
	 disable();
/*
	asm {
				push es
				mov ah,34h
				int 21h
				mov ax,es:[bx]
			 }
				ptr_dos_flag=(char *)MK_FP(_ES,_BX);
	asm		pop es
*/
//	 while(*ptr_dos_flag)
	 while(dos_flag && dos_host != tswitch)
	 {
//			semblock( tswitch, &io_out );
			tasks[tswitch].status  = BLOCKED;
			tasks[tswitch].pending = &dos_flag;
			task_switch();
			disable(); // task switch will enable interrupts, so they need
								 //   to be turned off again
	 }
	 dos_flag = 1;
	 dos_host = tswitch;
	 enable();
}

// Release a semaphore
void clear_semaphore_dos(void)
{
	disable();
	tasks[tswitch].pending = NULL;
	dos_flag = 0;
	if( restart( &dos_flag ) )
		task_switch();
	enable();
}
#endif //BAS_TEMP
// Release a semaphore
void clear_semaphore( sem_t *sem )
{
	sem_post (sem);
}

void UnhookHandlers(void)
{
	//  set new vectors
#ifdef BAS_TEMP
	setvect(8, (void interrupt (*)( ... ))oldhandler);
#endif //BAS_TEMP
}
