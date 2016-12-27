/******************************************************************************
 * File Name: mtkern1.cpp
 * Description: 
 *
 * Created:
 * Author:
 *****************************************************************************/
 
/******************************************************************************
 * INCLUDEs
 *****************************************************************************/

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include "mt.h"
//#include "t3000def.h"
#include "mtkernel.h"
//#include "aio.h"
//#include "baseclas.h"
//#include "rs485.h"
//#include "ptp.h"

/******************************************************************************
 * FUNCTION DECLARATION
 *****************************************************************************/
#ifdef BAS_TEMP
extern void set_vid_mem( void );
#endif //BAS_TEMP
void kill_task( int id );
void mono_task( void );
void resume_tasking( void );
void semblock( int id, unsigned *sem );
int  restart( unsigned *sem );
void check_sleepers( void );
void stop_tasking( void );
void msleep( int ticks );
void msleep( int id, int ticks );
void suspend( int id );
void resume( int id );
void resume_suspend( int, int );
void task_status( void );
//void free_all( void );
int all_dead( void );
int check_sleepers_all_dead(void);
void HookHandlers(void);
void UnhookHandlers(void);

/******************************************************************************
 * GLOBALs
 *****************************************************************************/
 
#ifdef SERIAL_COMM
extern int int_occured;
#endif
#ifdef BAS_TEMP
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
#ifdef BAS_TEMP
extern unsigned  read_mon_flag; // i/o semaphore
extern unsigned  dos_flag; // i/o semaphore
extern unsigned  dos_host;
extern unsigned  screen; // i/o semaphore
//extern unsigned  memory; // i/o semaphore
extern unsigned  t3000_flag; // i/o semaphore
#endif //BAS_TEMP
extern int tswitch;      		// task index
#ifdef BAS_TEMP
extern int setvectint8;
//extern int communication;
extern char timecount;
extern char run_board_flag;
extern int def_macro;
#endif //BAS_TEMP

#ifdef TEST
 char *num, *a = "0";
#endif
int setvectint8;

static char tasking = 1;           // tasking system enabled
static char single_task = 0;       // single task flag off
static unsigned pri,i,j;


/******************************************************************************
 * FUNCTION DEFINATIONs
 *****************************************************************************/

//PORT: User OS timer to invoke int8_task_switch(). Modify OS TCB to accomodate
//project's mt kernel tcb

void int8_task_switch()
{
#ifdef BAS_TEMP
	if(++timecount >= 8)
	{
		oldhandler();
		disable();
		register int i;
		timecount = 0;
		tasking = 0;
		for( i=MSTP_MASTER; i<MSTP_MASTER+RS485TASKS; i++ )
		{
		 if( tasks[i].status != DEAD )
		 {
			tasks[i].ps->InactivityTimer++;
			if( ++tasks[i].ps->SilenceTimer == (Tno_token-1) )
			{
				 tasks[i].status = READY;
				 tasking = 1;
			}
		 }
		}
	}
	else
	{
		disable();
		asm mov al, 20h
		asm out 20h, al
		asm jmp $+2
		asm nop
		register int i;

		for( i=NETWORK1; i<MSTP_MASTER+RS485TASKS; i++ )
		{
		 if( tasks[i].status != DEAD )
		 {
			if( tasks[i].ps )
			{
			 if( ++tasks[i].ps->SilenceTimer == (Tno_token-1) )
				 tasks[i].status = READY;
			 tasks[i].ps->InactivityTimer++;
			}
			if( tasks[i].status == SLEEPING )
			{
			 tasks[i].sleep--;
			 if( !tasks[i].sleep ) tasks[i].status = READY;
			}
		 }
		}
//		if (run_board_flag || int_occured || int_dos)
		if (run_board_flag || int_occured )
		{
		 enable();return;
		}

		tasks[tswitch].sp = _SP;	// save current task's stack
		tasks[tswitch].ss = _SS;
		if(tasks[tswitch].status == RUNNING )
		 tasks[tswitch].status = READY;
		for( i=NETWORK1; i<MSTP_MASTER+RS485TASKS; i++ )
		{
		 if( tasks[i].status == READY ) tswitch = i;
		}
		_SP = tasks[tswitch].sp;
		_SS = tasks[tswitch].ss;
		tasks[tswitch].status = RUNNING; // state is running
		enable();
		return;
	}

	if (setvectint8)
	{
	 enable();
	 return;
	};
//	oldhandler();

//	disable();

	if(!timecount)
	{
	 timeout++;
	 timeout_asyn++;
	 microsec +=  54925;
	 miliseclast += 55;
	 if( microsec >= 1000000)
	 {
		microsec -= 1000000;
		onesec++;
		onesec_virtual++;

		if(tensec++ == 10) tensec = 0;
		timestart++;
		ora_current_sec++;
		++ora_current.ti_sec;
		if( ora_current.ti_sec == 60 )
		{
		 onemin++;
		 if(tenmin++ == 10) tenmin = 0;
		 ora_current.ti_sec = 0;
		 ++ora_current.ti_min;
		 if(ora_current.ti_min == 60)
		 {
			ora_current.ti_min = 0;
			++ora_current.ti_hour;
			if( ora_current.ti_hour == 24 )
			{
			 ora_current.ti_hour=0;
			 ora_current_sec = 0;
			 if(++ora_current.dayofweek==7)
				 ora_current.dayofweek=0;
			 if( ++ora_current.dayofmonth==month_days[ora_current.month]+1 )
			 {
				 ora_current.dayofmonth=1;
				 if( ++ora_current.month == 12 )
						ora_current.month = 0;
			 }
			 if(++ora_current.dayofyear==366)
			 {
					ora_current.dayofyear=0;
					++ora_current.year;
					month_days[1]=28;
			 }
			 else
				if(ora_current.dayofyear==365)
				{
				 if( ora_current.year&0x03 )
				 {
					ora_current.dayofyear=0;
					++ora_current.year;
					month_days[1]=28;
				 }
				 else
					month_days[1]=29;
				}
			 check_annual_routine_flag=1;
//			 readclock_flag=1;
			}
		 }
	 }

	}

	if (int_occured)
	{
	 enable();
	 return;
	}

	register int i;

	if(timesave) timesave--;
	if(timepoints) timepoints--;
	if(timepoints_ipx) timepoints_ipx--;
	if(refresh_time) refresh_time--;
	if(grp_time) grp_time--;
	if(display_grp_time) display_grp_time--;
	if(alarm_time) alarm_time--;
	if(refresh_graph_time) refresh_graph_time--;
	if(disconnect_time>0) disconnect_time--;
	if(worktime>0) worktime--;

	for( i=0; i< NUM_TASKS; i++ )
	{
		if( tasks[i].delay_time >= 0 ) tasks[i].delay_time -= 55;
		if( i>=PTP_CONNECTION && i<PTP_CONNECTION+RS232TASKS )
		{
		 if( tasks[i].status != DEAD )
		 {
			if( tasks[i].ps->time_modem >= 0 ) tasks[i].ps->time_modem -= 55;
			if( tasks[i].ps->connection == PTP_CONNECTED )
			{
			tasks[i].ps->SilenceTimer++;
			if( ++tasks[i].ps->InactivityTimer < T_Inactivity )    //T_INACTIVITY
			{
			 if( ++tasks[i].ps->HeartbeatTimer >= T_HEARTBEAT )
			 {
//				if (!run_board_flag && !int_dos )
				if (!run_board_flag )
				{
				 if(tasks[i+PTP_transmission].status != BLOCKED &&  tasks[i+PTP_transmission].status != RUNNING )
				 {
					tasks[i+PTP_transmission].status = READY;
					tasking = 1;
				 }
				}
			 }
			}
			else
			{
//				if (!run_board_flag && !int_dos )
			 if (!run_board_flag)
			 {
				if( tasks[i].status != RUNNING )
				{
				 tasks[i].status = READY;
				 tasking = 1;
				}
			 }
			}
			}
		 }
		}
		if( tasks[i].status == SLEEPING )
		{
			tasks[i].sleep--;
			if( !tasks[i].sleep )
			{
			 tasks[i].status = READY;
			 tasking = 1;
			}
		}
	}
	if(present_analog_monitor && onesec)
	{
	 for(i=0;i<MAX_ANALM;i++)
		if(ptr_panel->analog_mon[i].num_inputs)
		{
			if(ptr_panel->monitor_work_data[i].next_sample_time)
				ptr_panel->monitor_work_data[i].next_sample_time--;
		}
		onesec = 0;
	}


	if(time_key) time_key--;
	if(def_macro)
	{
		 time_last_key++;
	}

 }  // end if(!timecount)

//	if (run_board_flag || int_dos )
	if (run_board_flag)
	{
	 enable();
	 return;
	}

//	(*old_int8)(); // call to original int8 routine

	if( single_task ) // if single task is on, then return
	{                 //without a task switch
	 enable();
	 return;
	}

	tasks[tswitch].ss = _SS;	// save current task's stack
	tasks[tswitch].sp = _SP;
	// if current task was running, then change its state to READY
	if(tasks[tswitch].status == RUNNING )
	{
		tasks[tswitch].status = READY;
	  tasking = 1;
	}
	if(!timecount)
	{
	 if( disconnect_time==605)
	 {
		disconnect_modem=1;
		action=1;
		tasks[MISCELLANEOUS].status = READY;
		tasking = 1;
	 }
// ->
// see if any sleepers need to wakwe up
//	check_sleepers();
// see if all tasks are dead; if so, stop tasking
//	i=all_dead();
// <-

/*	 i = check_sleepers_all_dead();
	 if( i )
		tasking = 0;
*/
	 if( !tasking) { // stop tasking
			disable();
			_SS = oldss;
			_SP = oldsp;
//			setvect( 8, old_int8 );
			UnhookHandlers();
//			free_all();
			enable();
			return;
	 }
	// find new task
	}
	tswitch++;
	if( tswitch == NUM_TASKS ) tswitch = 0;
	while( tasks[tswitch].status != READY )
	{
		 tswitch++;
		 if( tswitch == NUM_TASKS ) tswitch = 0;
	}

	pri = tasks[tswitch].pri;
	i=tswitch;
	j=tswitch+1;
	if (j ==  NUM_TASKS ) j = 0;
	while( j != i )
	{
	 if( tasks[j].status == READY && tasks[j].pri > pri  )
	 {
		tswitch = j;
		pri = tasks[tswitch].pri;
	 }
	 j++;
	 if( j == NUM_TASKS ) j = 0;
	}

	// switch task to a new task

	_SP = tasks[tswitch].sp;
	_SS = tasks[tswitch].ss;
	tasks[tswitch].status = RUNNING; // state is running
	enable();
#endif //BAS_TEMP
};

/* This is the manual task switcher which a program can call to force a task
switch. It does not decrement any sleeper's sleep counter because a clock tick
has not occured */
//PORT: Use OS_Sched()
void task_switch( void )
//void task_switch( void )
{
	MT_CPU_SR cpu_sr = 0;
	disable();

	if( tasks[tswitch].status == RUNNING )
		tasks[tswitch].status = READY;

	tasking=all_dead();

	if( !tasking )
	{
		disable();
		//			setvect( 8, old_int8 );
		UnhookHandlers();
		//			free_all();
		enable();
		return;
	}
	
	// find new tasks
	tswitch++;
	if( tswitch==NUM_TASKS ) 
		tswitch = 0;

	while( tasks[tswitch].status!=READY)
	{
		tswitch++;
		if( tswitch==NUM_TASKS ) 
			tswitch = 0;
	}
	pri = tasks[tswitch].pri;

	i=tswitch;
	j=tswitch+1;
	
	if (j ==  NUM_TASKS ) 
		j = 0;
	
	while( j != i )
	{
		if( tasks[j].status == READY && tasks[j].pri > pri  )
		{
			tswitch = j;
			pri = tasks[tswitch].pri;
		}
		j++;
		if( j == NUM_TASKS ) 
			j = 0;
	}

	tasks[tswitch].status = RUNNING;

	enable();

	MT_Sched();
}

// Return 1 if no task are ready to run; 0 if at least one task is READY.

int all_dead( void )
{
//	register int i;
	for( j=0; j<NUM_TASKS; j++ )
		if( tasks[j].status == READY || tasks[j].status == RUNNING ) return 1;
	return 0;
}
#ifdef BAS_TEMP
// Start up the multitasking kernel.
void interrupt multitask( void )
//void multitask( void )
{
	disable();
// Switch in the timer based schedular
//	old_int8 = getvect( 8 );
//	setvect( 8, int8_task_switch );
	HookHandlers();

/* Save the program's stack pointer and segment so that when
  tasking ends, execution can continue where it left off in the
	program. */
	oldss = _SS;
	oldsp = _SP;
// set stack to first task's stack
	_SP = tasks[tswitch].sp;
	_SS = tasks[tswitch].ss;
	enable();
}
#endif //BAS_TEMP
// Kill a task. ( i.e., make it's state DEAD. )
void kill_task( int id )
{
	tasks[id].status = DEAD;
	OSTaskSuspend (id);
#ifdef BAS_TEMP
	//TODO: Clear and Unlink OS TCB from TCB list
	MT_CPU_SR cpu_sr = 0;
	disable();
	tasks[id].status = DEAD;
	enable();
	task_switch();
#endif //BAS_TEMP
}

// Initialize the task control structures
void init_tasks( void )
{

	for( i=0; i<NUM_TASKS; i++ )
	{
//	 tasks[i].status = SUSPENDED;
	 tasks[i].status = DEAD;
	 tasks[i].pending = NULL;
	 tasks[i].sleep = 0;
	 tasks[i].pri = i;
	}
	 tasks[PROJ].status = DEAD;
#ifdef BAS_TEMP
	 set_vid_mem();
#endif //BAS_TEMP
}

// Stop execution of a task for a specified number of clock cycles.
void msleep( int ticks )
{
	OSTimeDly(ticks);
}

void msleep( int id, int ticks )
{
	if(tasks[id].status==SUSPENDED || tasks[id].status==SLEEPING)
	{
		OSTimeDlyTask(id, ticks);
	}
}

// Suspend a task until resumed by another task
void suspend( int id )
{
	OSTaskSuspend(id);
}

// Restart a previously suspended task.
void resume( int id )
{
	OSTaskResume (id);
}

void resume_suspend( int id_res, int id_susp )
{
	resume( id_res );
	suspend( id_susp );
}

void blocked_resume( int id )
{
	 if( id < 0 || id > NUM_TASKS ) 
		 return;
	 tasks[id].status = READY;
	 OSTaskResume (id);
}

// Wait for a semaphore
void set_semaphore( MT_EVENT *pevent )
{
	INT8U     *perr;
	OSSemPend (pevent, 0, perr);
	tasks[tswitch].status  = BLOCKED;
}


void set_semaphore_dos(void)
{
#ifdef BAS_TEMP
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
#endif //BAS_TEMP
}

// Release a semaphore
void clear_semaphore_dos(void)
{
#ifdef BAS_TEMP
	disable();
	tasks[tswitch].pending = NULL;
	dos_flag = 0;
	if( restart( &dos_flag ) )
		task_switch();
	enable();
#endif //BAS_TEMP
}

// Release a semaphore
void clear_semaphore(MT_EVENT *pevent)
{
	OSSemPost (pevent);
	tasks[tswitch].status  = READY;
}

void HookHandlers(void)
{
#ifdef BAS_TEMP
	oldhandler = getvect(8);
	setvect(8, (void interrupt (*)( ... ))int8_task_switch);
#endif //BAS_TEMP
	setvectint8 = 1;	
}

void UnhookHandlers(void)
{
#ifdef BAS_TEMP
	//  set new vectors
	setvect(8, (void interrupt (*)( ... ))oldhandler);
#endif //BAS_TEMP
}
