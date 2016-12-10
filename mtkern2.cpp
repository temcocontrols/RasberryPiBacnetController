#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <syscall.h>
#include <stdio.h>
#ifdef BAS_TEMP
#pragma inline
#include <mem.h>
#include <alloc.h>
#endif //BAS_TEMP
#include <time.h>
#ifdef BAS_TEMP
#include <graphics.h>
#include "mouse.h"
#include <windows.h>
#endif //BAS_TEMP
#include "t3000def.h"
#include "mtkernel.h"

extern void UnhookHandlers(void);
#ifdef BAS_TEMP
extern void interrupt int8_task_switch( __CPPARGS );
extern void interrupt (*old_int8 )( __CPPARGS );
#endif //BAS_TEMP
extern void set_semaphore_dos(void);
extern void clear_semaphore_dos(void);
extern void set_semaphore( unsigned *sem );
extern void clear_semaphore( unsigned *sem );
extern int decode( unsigned char *line, int size, signed char *buf )   ;
extern int encode( unsigned char *line, int size , signed char *buf);
extern int bioskey_new(int cmd);
#ifdef BAS_TEMP
extern void interrupt ( *oldhandler)(__CPPARGS);
#endif //BAS_TEMP
extern int mode_text;
extern int xscreen, yscreen;
extern int dxscreen, dyscreen;
extern unsigned  screen; // i/o semaphore
extern char *vid_mem;	// pointer to video memory
extern int setvectint8;
extern int line_style;
extern int i;
extern unsigned oldss, oldsp;

void mputs( char *str, int bkgnd, int frgnd );
void mfarrealloc(char **far_point, unsigned long nbytes);


/* 	Make_task() returns false if the task cannot be added to the task queue,
	otherwise, it returns true.	*/

int make_task( taskptr task, char *stck, unsigned stck_size, unsigned id, void *ptr, int port )
{
	int_regs *r;
	if( ( id >= NUM_TASKS ) ) return 0;
	disable();
#ifdef BAS_TEMP
	// allocate space for the task
	i=0;
	if(id!=PROJ)
	{
//	 tasks[id].stck =(unsigned char *) stck;
	 if(ptr)
	 {                 // simulate a call function where ptr is the argument
							 // ptr - pointer to a object
	  r = ( int_regs *  ) (stck + stck_size -	sizeof( int_regs ));
	  r->eax   = 0;
	  r->ip    = 0;
	  r->cs    = FP_OFF(ptr);
	  r->flags = FP_SEG(ptr);
	  i = 8;
//	  tasks[id].port_number = port;
	  tasks[id].ps = &Routing_table[port].port_status_vars;
	 }
	 r = ( int_regs *  ) (stck + stck_size -	sizeof( int_regs ) - i );
// initialize task stack
	  tasks[id].sp = FP_OFF( ( int_regs far * ) r );
		tasks[id].ss = FP_SEG( ( int_regs far * ) r );

	  r->cs = FP_SEG(task);
	  r->ip = FP_OFF(task);
	// set up DS and ES
	  r->ds = _DS;
	  r->es = _ES;
	// enable interrupts
	  r->flags = 0x200;
	}
	else
	{
//	tasks[PROJ].stck =(char *) MK_FP(_SS,0);
//	r = ( int_regs *  ) (tasks[PROJ].stck + (stck_size-60) -
//										sizeof( int_regs ));
	// initialize task stack
	_BX=_SP;
	_SP=_SP-200;
	_AX=0x200;
	asm 	push ax;
	_AX=FP_SEG(task);
	asm 	push ax;
	_AX=FP_OFF(task);
	asm push ax;

	asm	push eax;
	asm	push ebx;
	asm	push ecx;
	asm	push edx;
	asm	push es;
	asm	push ds;
	asm	push esi;
	asm	push edi;
	asm	push bp;

	_AX=_SP;
	_SP=_BX;
	tasks[PROJ].sp = _AX;
	tasks[PROJ].ss = _SS;
//	ss_proj = tasks[PROJ].ss;
//	sp_proj = tasks[PROJ].sp;
	}

	// set up new task's  CS and IP registers
	tasks[id].status = SUSPENDED;
	if( id==PROJ )
		tasks[PROJ].status = READY;
#endif //BAS_TEMP
    enable();
	return 1;
}

void exit_proj(void)
{
    disable();
#ifdef BAS_TEMP
	_SS = oldss;
	_SP = oldsp;
	UnhookHandlers();
#endif //BAS_TEMP
enable();
#ifdef BAS_TEMP
	asm {
	pop bp
	pop edi
	pop esi
	pop ds
	pop es
	pop edx
	pop ecx
	pop ebx
	pop eax
	iret
	}
#endif //BAS_TEMP
}

//*************************************************************************
// 	Serialized and Reentrant I/O functions for the Multitasker.
//*************************************************************************

void pline(int x1,int y1,int x2,int y2,int color)
{
#ifdef BAS_TEMP
 int i=getcolor();
 setcolor(color);
 if(line_style==1)
	setlinestyle(SOLID_LINE, 1, 1);
 else
	setlinestyle(DOTTED_LINE, 1, 1);

 moveto(x1,y1);
 lineto(x2,y2);
 setcolor(i);
 setlinestyle(SOLID_LINE, 1, 1);
#endif //BAS_TEMP
}

void prectangle(int ltopx,int ltopy,int rbottomx,int rbottomy,int color)
{
 pline(ltopx,ltopy,rbottomx,ltopy,color);
 pline(ltopx,ltopy,ltopx,rbottomy,color);
 pline(ltopx,rbottomy,rbottomx,rbottomy,color);
 pline(rbottomx,ltopy,rbottomx,rbottomy,color);
}

unsigned mimagesize(int ltopx, int ltopy, int rbottomx, int rbottomy)
{
 unsigned l;
#ifdef BAS_TEMP
 asm push es;
 set_semaphore(&screen);
 l=imagesize(ltopx, ltopy, rbottomx, rbottomy);
 clear_semaphore(&screen);
 asm pop es;
#endif //BAS_TEMP
 return l;
}

void mgetimage(int ltopx, int ltopy, int rbottomx, int rbottomy, void *under1)
{
#ifdef BAS_TEMP
 asm push es;
 set_semaphore(&screen);
 getimage(ltopx, ltopy, rbottomx, rbottomy, under1);
 clear_semaphore(&screen);
 asm pop es;
#endif //BAS_TEMP
}

void mputimage(int ltopx, int ltopy, void *under1, int Copy_put)
{
#ifdef BAS_TEMP
 asm push es;
 set_semaphore(&screen);
 if(under1)
  putimage(ltopx, ltopy, under1, Copy_put);
 clear_semaphore(&screen);
 asm pop es;
#endif //BAS_TEMP
}

void mgetimage_10(int ltopx, int ltopy, int rbottomx, int rbottomy, char *p)
{
 set_semaphore(&screen);
 for(int i=ltopy; i<=rbottomy;i++)
	for(int j=ltopx; j<=rbottomx;j++)
		{
#ifdef BAS_TEMP
			asm {
				mov ah,0dh
				mov bh,0
				mov dx,i
				mov cx,j
				int 10h
				}
			*p++ = _AL;
#endif //BAS_TEMP
		}
 clear_semaphore(&screen);
}

void mputimage_10(int ltopx, int ltopy, int rbottomx, int rbottomy, char *p)
{
 set_semaphore(&screen);
 for(int i=ltopy; i<=rbottomy;i++)
	 for(int j=ltopx; j<=rbottomx;j++)
		 {
#ifdef BAS_TEMP
			_AL=*p++;
			asm {
				mov ah,0ch
				mov bh,0
				mov dx,i
				mov cx,j
				int 10h
				}
#endif //BAS_TEMP
		 }
 clear_semaphore(&screen);
}


void mputch( int c )
{
#ifdef BAS_TEMP
	 putch( c );
#endif //BAS_TEMP
}

void movetoxy(int x, int y)
{
#ifdef BAS_TEMP
	 asm {
		push ax
		push bx
		push dx
		mov ah,2
		mov dh, BYTE PTR y
		dec dh
		mov dl, BYTE PTR x
		dec dl
		mov bh,0
		int 10h
		pop dx
		pop bx
		pop ax
	 }
#endif //BAS_TEMP
}

void hidecur(void)
{
#ifdef BAS_TEMP
	if(mode_text)
	{
	 set_semaphore(&screen);
	 asm {
		push ax
		push bx
		push dx
		mov ah,2
		mov dh, 25
		mov dl, 0
		mov bh,0
		int 10h
		pop dx
		pop bx
		pop ax
	}
	clear_semaphore(&screen);
 }
#endif //BAS_TEMP
}

void getxy(int *x, int *y)
{
#ifdef BAS_TEMP
	 union REGS regs;
	 asm {
	 mov ah, 3;  /* read cursor position */
	 mov bh, 0;  /* video page 0 */
	 int 10h
	 inc dh
	 inc dl
	 }
	 *y=(int)_DH;
	 *x=(int)_DL;
#endif //BAS_TEMP
}

int mgets(char *str,int viz, int bkgnd, int frgnd)
{
	int y, x;
	x = y = 0;
	union {
		int i;
		char ch[2];
		} key;
#ifdef BAS_TEMP
	asm {
	 mov ah,03h
	 mov bh,0
	 int 10h
	 inc dh
	 mov BYTE PTR y,dh
	 inc dl
	 mov BYTE PTR x,dl
	}
#endif //BAS_TEMP
 char  *p;
 char ret=0;
 p=str;
 *str = '\0';
 if(viz==-1) ret = 1;
 for(;;) {
#ifdef BAS_TEMP
 while(bioskey_new(1)==0)
		 if(ret) return 0;
#endif //BAS_TEMP
 ret = 0;
#ifdef BAS_TEMP
 key.i = bioskey_new(0);
#endif //BAS_TEMP
 *str=key.ch[0];
// *str=mgetch();
 if(*str=='\r') {
	*str = '\0';
	return 13;
 }
 if(*str==27)
	 {
		*str = '\0';
		return 27;
	 }
 if(*str=='\b') {
	if(str>p) {
	 str--;
	 movetoxy(--x, y);
	 moutchar(x,y,' ',bkgnd,frgnd);
	}
 }
 else {
	 if(viz)
		 moutchar(x,y,*str,bkgnd,frgnd);
	 else
		 moutchar(x,y,'*',bkgnd,frgnd);
	 movetoxy(++x, y);
	 str++;
	}
 }
}

/* Output a character at specified X, Y coordinates. This function IS
	 reentrant and may be called by any task at any time. */
void moutchar( int x, int y, char ch, int bkgnd, int frgnd )
{
	 char  *v;
	 x--;y--;
	 v = vid_mem;
	 v += ( y * 160 ) + x * 2;  // compute char location
	 *v++ = ch; 				// write the character
	 *v = bkgnd*16+frgnd; 				// normal character
}

void moutchar( int x, int y, char ch )
{
	 char *v;
	 v = vid_mem;
	 v += ( y * 160 ) + x * 2;  // compute char location
	 *v++ = ch; 				// write the character
	 *v = 7; 				// normal character
}

void mputchar( char ch, int bkgnd, int frgnd )
{
	 char *v;
	 if( ch == '\r' ) { xscreen = 0; return; }
	 if( ch == '\n' ) { yscreen++; xscreen = 0; return; }
	 if(bkgnd==-1)
	 {
		 v = vid_mem;
		 v += ( yscreen * 160 ) + xscreen * 2;  // compute char location
		 bkgnd = *v >> 4;
	 }
	 bkgnd = bkgnd*16 + frgnd;
	 v = vid_mem;
	 v += ( yscreen * 160 ) + xscreen++ * 2;  // compute char location
	 *v++ = ch; 				// write the character
	 *v = bkgnd; 				// normal character
	 if (xscreen==80) xscreen = 0;
}

int mgetch(int x, int y)
{
 char ch,atr;
 char *v;
 v = vid_mem;
 v += ( (y-1) * 160 ) + (x-1) * 2;  // compute char location
 ch = *v++; 				// write the character
 atr = *v; 				// normal character
 return ((atr<<8) + ch);
}

int mgetword(int x, int y, char *pword, int length = 80)
{
 char c;
 int i;
#ifdef BAS_TEMP
 hide_pointer();
#endif //BAS_TEMP
 *pword = 0;
 if((char)mgetch(x,y)==' ')
 {
#ifdef BAS_TEMP
	display_pointer();
#endif //BAS_TEMP
	return 0;
 }
 while(x>1)
 {
	c = (char)mgetch(--x,y);
	if( c==' ' || c==')' || c=='(' || c=='/' || c=='=') break;
 }
 i = x + length  - 1;
 while(x<i && x<80)
 {
	c = (char)mgetch(++x,y);
	if( c==' ' || c==')' || c=='(' || c=='/' || c=='=') break;
	*pword++ = c;
 }
 *pword = 0;
#ifdef BAS_TEMP
 display_pointer();
#endif //BAS_TEMP
 return 1;
}

/* Output a string at specified X, Y coordinates. This function IS
	 reentrant and may be called by any task at any time. */
void mxyputs( int x, int y, char *str )
{
 int initx;
	 initx = x;
	 while( *str )
		 {
			if (*str=='\n')
				{
				 y++;
				 str++;
				 continue;
				}
			if (*str=='\r')
				{
				 x=initx;
				 str++;
				 continue;
				}

			moutchar( x++, y, *str++ );
		}
}

void mxyputs( int x, int y, char *str, int bkgnd, int frgnd )
{
 int initx;
 char *v;
	 x--;y--;
	 initx = x;

	 if(bkgnd==-1)
		{
		 v = vid_mem;
		 v += ( y * 160 ) + x * 2;  // compute char location
		 bkgnd = *(v+1) >> 4;
		}
	 bkgnd = bkgnd*16 + frgnd;

	 while( *str )
		 {
			if (*str=='\n')
				{
				 y++;
				 str++;
				 continue;
				}
			if (*str=='\r')
				{
				 x=initx;
				 str++;
				 continue;
				}

			 v = vid_mem;
			 v += ( y * 160 ) + x * 2;  // compute char location
			 *v++ = *str++; 				// write the character
			 *v = bkgnd; 				// normal character

			 x += dxscreen;
			 y += dyscreen;
//			moutchar( x++, y, *str++, bkgnd, frgnd );
		}
}

void mputs( char *str )
{
	 while( *str )
		 {
			if (*str=='\r')
				{
				 xscreen=0;
				 str++;
				 continue;
				}
			if (*str=='\n')
				{
				 if (yscreen!=24)
					 yscreen++;
				 else
					 yscreen = 0;
				 str++;
				 continue;
				}
			moutchar( xscreen++, yscreen, *str++ );
			if (xscreen==80)
				{
				 xscreen = 0;
				 continue;
				}
		}
}

void mputs( char *str, int bkgnd, int frgnd )
{
	 char *v;
	 if(bkgnd==-1)
		{
		 v = vid_mem;
		 v += ( yscreen * 160 ) + xscreen * 2;  // compute char location
		 bkgnd = *(v+1) >> 4;
		}
	 bkgnd = bkgnd*16 + frgnd;
	 while( *str )
		 {
			if (*str=='\n')
				{
				 yscreen++;
				 str++;
				 continue;
				}
			if (*str=='\r')
				{
				 xscreen=0;
				 str++;
				 continue;
				}
			v = vid_mem;
			v += ( yscreen * 160 ) + xscreen++ * 2;  // compute char location
			*v++ = *str++; 				// write the character
			*v = bkgnd; 				// normal character
			if (xscreen==80)
				 xscreen = 0;
		}
}

void mfarrealloc(char **far_point, unsigned long nbytes)
{
 char *p=*far_point;
 set_semaphore_dos();
// disable();     //***************************
#ifdef BAS_TEMP
 *far_point=(char *)farrealloc(p, nbytes);
#endif //BAS_TEMP
// enable();     //****************************
 clear_semaphore_dos();
}
#ifdef BAS_TEMP
void mfarmalloc(char **far_point, unsigned long nbytes, HANDLE& handle)
{
 set_semaphore_dos();
	if( (handle = GlobalAlloc(GPTR,nbytes)) != NULL )  //GHND - GMEM_MOVEABLE; GPTR - FIXED
		*far_point = GlobalLock( handle );
	else
		*far_point = NULL;

	if(handle != NULL)
		memset(*far_point, 0, nbytes);
 clear_semaphore_dos();
}

void mfarfree(HANDLE handle)
{
 if(handle)
 {
	set_semaphore_dos();
	GlobalUnlock( handle);
	GlobalFree( handle );
	clear_semaphore_dos();
 }
}
#endif //BAS_TEMP
void mfarmalloc(char **far_point, unsigned long nbytes)
{
 set_semaphore_dos();
#ifdef BAS_TEMP
 *far_point=(char *)farmalloc(nbytes);
 if(*far_point != NULL)
		memset(*far_point, 0, nbytes);
#endif //BAS_TEMP
 clear_semaphore_dos();
}

void mfarfree(void *far_point)
{
 if(far_point)
 {
	set_semaphore_dos();
#ifdef BAS_TEMP
	farfree((void *)far_point);
#endif //BAS_TEMP
	clear_semaphore_dos();
 }
}

void mgettext(int left, int top, int right, int bottom, void *dest)
{
 set_semaphore(&screen);
#ifdef BAS_TEMP
 gettext(left,top,right,bottom,(void *)dest);
#endif //BAS_TEMP
 clear_semaphore(&screen);
}

void mputtext(int left, int top, int right, int bottom, void *source)
{
 set_semaphore(&screen);
#ifdef BAS_TEMP
 puttext(left,top,right,bottom,(void *)source);
#endif //BAS_TEMP
 clear_semaphore(&screen);
}

void mclrscr(char x1, char y1, char x2, char y2, char col)
{
#ifdef BAS_TEMP
 asm {
	push ax
	push bx
	push cx
	push dx

	mov ah,6
	mov al,0
	mov ch, BYTE PTR y1
  dec ch
  mov cl, BYTE PTR x1
  dec cl
  mov dh, BYTE PTR y2
  dec dh
  mov dl, BYTE PTR x2
  dec dl
  mov bh, BYTE PTR col
	shl bh,1
  shl bh,1
  shl bh,1
  shl bh,1
  int 10h
	pop dx
	pop cx
	pop bx
	pop ax
 }
#endif //BAS_TEMP
}

void mgotoxy(int x, int y)
{
 movetoxy(x,y);
// gotoxy(x, y);
 xscreen=x-1;
 yscreen=y-1;
}

time_t mtime( time_t *timer )
{
 time_t n;
 set_semaphore_dos();
 n=time(timer);
 clear_semaphore_dos();
 return n;
}


char *mctime(const time_t *time)
{
 char *p;
 set_semaphore_dos();
 p=ctime(time);
 clear_semaphore_dos();
 return p;
}

//**********************************************************************
//	Initialize the video subsystem.
//   Returns the current video mode.
//**********************************************************************
int video_mode( void )
{
#ifdef BAS_TEMP
	 union REGS r;
	 asm {
	  mov ah,0fh
		int 10h
	 }
	 return (_AX & 255);
#endif //BAS_TEMP
}

// Set the vid_mem pointer to the start of video memory.
void set_vid_mem( void )
{
	 int vmode;
#ifdef BAS_TEMP
	 vmode = video_mode();
	 if( vmode == 7 ) vid_mem = ( char *) MK_FP(__SegB000, 0 );
	 else vid_mem = ( unsigned char *) MK_FP( __SegB800, 0 );
#endif //BAS_TEMP
}


