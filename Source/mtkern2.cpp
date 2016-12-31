/******************************************************************************
 * File Name: mtkern2.cpp
 * Description: 
 *
 * Created:
 * Author:
 *****************************************************************************/
 
/******************************************************************************
 * INCLUDEs
 *****************************************************************************/

#include <pthread.h>
//#include <time.h>
#include <graphics.h>
//#include "mouse.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <termio.h>
#include <unistd.h>

#include "t3000def.h"
#include "mt.h"
#include "mtkernel.h"
//#include "key.h"

extern	task_struct tasks[NUM_TASKS];
extern  pthread_t   threadTask[ MT_LOWEST_PRIO ];

extern void UnhookHandlers(void);
extern void int8_task_switch( __CPPARGS );
extern void set_semaphore_dos(void);
extern void clear_semaphore_dos(void);
extern void set_semaphore( MT_EVENT *pevent );
extern void clear_semaphore(MT_EVENT *pevent);
#ifdef BAS_TEMP
extern int decode( unsigned char *line, int size, signed char *buf )   ;
extern int encode( unsigned char *line, int size , signed char *buf);
extern int bioskey_new(int cmd);

extern int mode_text;
extern int xscreen, yscreen;
extern int dxscreen, dyscreen;
#endif //BAS_TEMP
extern MT_EVENT *  sem_screen; // i/o semaphore
#ifdef BAS_TEMP
extern char far *vid_mem;	// pointer to video memory
extern int setvectint8;
#endif //BAS_TEMP
extern int line_style;
#ifdef BAS_TEMP
extern int i;

void mputs( char *str, int bkgnd, int frgnd );
void mfarrealloc(char far **far_point, unsigned long nbytes);
#endif //BAS_TEMP

static int stdin_fd = -1;
static struct termios original;

bool keyPressed(int *character)
{
    // If this is the first time the function is called, change the stdin
    // stream so that we get each character when the keys are pressed and
    // and so that character aren't echoed to the screen when the keys are
    // pressed.
    if (stdin_fd == -1)
    {
        // Get the file descriptor associated with stdin stream.
        stdin_fd = fileno(stdin);

        // Get the terminal (termios) attritubets for stdin so we can
        // modify them and reset them before exiting the program.
        tcgetattr(stdin_fd, &original);

        // Copy the termios attributes so we can modify them.
        struct termios term;
        memcpy(&term, &original, sizeof(term));

        // Unset ICANON and ECHO for stdin. When ICANON is not set the
        // input is in noncanonical mode. In noncanonical mode input is
        // available as each key is pressed. In canonical mode input is
        // only available after the enter key is pressed. Unset ECHO so that
        // the characters aren't echoed to the screen when keys are pressed.
        // See the termios(3) man page for more information.
        term.c_lflag &= ~(ICANON|ECHO);
        tcsetattr(stdin_fd, TCSANOW, &term);

        // Turn off buffering for stdin. We want to get the characters
        // immediately. We don't want the characters to be buffered.
        setbuf(stdin, NULL);
    }

    // Get the number of characters that are waiting to be read.
    int characters_buffered = 0;
    ioctl(stdin_fd, FIONREAD, &characters_buffered);

    // Set the return value to true if there are any characters to be read.
    bool pressed = (characters_buffered != 0);

    if (characters_buffered == 1)
    {
        // There is only one character to be read. Read it in.
        int c = fgetc(stdin);

        // Check if the caller wants the value of character read.
        if (character != NULL)
        {
            *character = c;
        }
    }
    else if (characters_buffered > 1)
    {
        // There is more than one character to be read. This can be key such
        // as the arrow keys or function keys. This code just reads them in
        // and ignores them. The caller will be informed that a key was
        // pressed, but won't get a value for the key.
        while (characters_buffered)
        {
            fgetc(stdin);
            --characters_buffered;
        }
    }

    return pressed;
}

void keyboardReset(void)
{
    if (stdin_fd != -1)
    {
        // If keyPressed() has been called the terminal input has been
        // changed for the stdin stream. Put the attributes back the way
        // we found them.
        tcsetattr(stdin_fd, TCSANOW, &original);
    }
}

/* 	Make_task() returns false if the task cannot be added to the task queue,
	otherwise, it returns true.	*/

int make_task( task pTask, char *stck, unsigned stck_size, unsigned id, void *ptr, int port )
{
	MT_CPU_SR cpu_sr = 0;
	
	if( ( id >= NUM_TASKS ) ) 
		return 0;
	
	disable();
	
	if(id != PROJ)
	{
		if(ptr)
		{
#ifdef BAS_TEMP //TODO: Uncomment once Routing_table[] is enabled in Router.cpp
			tasks[id].ps = &Routing_table[port].port_status_vars;
#endif //BAS_TEMP
		}
	}

	// set up new task's  CS and IP registers
	tasks[id].status = SUSPENDED;
	if(id == PROJ)
	{
		tasks[PROJ].status = READY;
	}
	enable();
	
	OSTaskCreate (pTask, NULL, (MT_STK*)stck, id);
					 
	return 1;
}

void exit_proj(void)
{
	MT_CPU_SR cpu_sr = 0;
	
	int *oldstate;
	int *oldtype;
	
	disable();
	UnhookHandlers();
	pthread_setcancelstate (PTHREAD_CANCEL_ENABLE,   oldstate);
	pthread_setcanceltype  (PTHREAD_CANCEL_DEFERRED, oldtype);
	pthread_cancel (threadTask[OSTCBPrioThreadIdx[PROJ]]);
	enable();
}

//*************************************************************************
// 	Serialized and Reentrant I/O functions for the Multitasker.
//*************************************************************************

void pline(int x1,int y1,int x2,int y2,int color)
{
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
}

void prectangle(int ltopx,int ltopy,int rbottomx,int rbottomy,int color)
{
	pline(ltopx,ltopy,rbottomx,ltopy,color);
	pline(ltopx,ltopy,ltopx,rbottomy,color);
	pline(ltopx,rbottomy,rbottomx,rbottomy,color);
	pline(rbottomx,ltopy,rbottomx,rbottomy,color);
}

unsigned imagesize (int left, int top, int right, int bottom)
{
	// Returns the size in bytes of the memory area required to store
	// a bit image.

	return 2 * sizeof(Uint32) + // witdth, height
			(right - left + 1) * (bottom - top + 1) * sizeof (Uint32);
} // imagesize ()

unsigned mimagesize(int ltopx, int ltopy, int rbottomx, int rbottomy)
{
	unsigned l;

	set_semaphore(sem_screen);
	l=imagesize(ltopx, ltopy, rbottomx, rbottomy);
	clear_semaphore(sem_screen);

	return l;
}

void mgetimage(int ltopx, int ltopy, int rbottomx, int rbottomy, void *under1)
{
	set_semaphore(sem_screen);
#ifdef BAS_TEMP //TODO: Uncomment
	getimage(ltopx, ltopy, rbottomx, rbottomy, under1);
#endif //BAS_TEMP
	clear_semaphore(sem_screen);
}

void mputimage(int ltopx, int ltopy, void *under1, int Copy_put)
{
	set_semaphore(sem_screen);
#ifdef BAS_TEMP //TODO: Uncomment
	if(under1)
		putimage(ltopx, ltopy, under1, Copy_put);
#endif //BAS_TEMP
	clear_semaphore(sem_screen);
}

void mgetimage_10(int ltopx, int ltopy, int rbottomx, int rbottomy, char *p)
{
	set_semaphore(sem_screen);
	for(int i=ltopy; i<=rbottomy;i++)
	{
		for(int j=ltopx; j<=rbottomx;j++)
		{
#ifdef BAS_TEMP //TODO: Understand and replace
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
	}
	clear_semaphore(sem_screen);
}

void mputimage_10(int ltopx, int ltopy, int rbottomx, int rbottomy, char *p)
{
	set_semaphore(sem_screen);
	for(int i=ltopy; i<=rbottomy;i++)
	{
		for(int j=ltopx; j<=rbottomx;j++)
		{
#ifdef BAS_TEMP //TODO: Understand and replace
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
	}
	clear_semaphore(sem_screen);
}

void mputch( int c )
{
	putchar( c );
}

void movetoxy(int x, int y)
{
#ifdef BAS_TEMP //TODO: Understand and replace
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
#endif // BAS_TEMP
}

void hidecur(void)
{
#ifdef BAS_TEMP //TODO: Understand and replace
	if(mode_text)
	{
		set_semaphore(sem_screen);

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

		clear_semaphore(sem_screen);
	}
#endif //BAS_TEMP
}

void getxy(int *x, int *y)
{
#ifdef BAS_TEMP //TODO: Understand and replace
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
#ifdef BAS_TEMP //TODO: Understand and replace
	asm {
	 mov ah,03h
	 mov bh,0
	 int 10h
	 inc dh
	 mov BYTE PTR y,dh
	 inc dl
	 mov BYTE PTR x,dl
	}

 char *p;
 char ret=0;
 p=str;
 *str = '\0';
 if(viz==-1) ret = 1;
 for(;;) {
 while(bioskey_new(1)==0)
		 if(ret) return 0;
 ret = 0;
 key.i = bioskey_new(0);
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
#endif //BAS_TEMP
}

/* Output a character at specified X, Y coordinates. This function IS
	 reentrant and may be called by any task at any time. */
void moutchar( int x, int y, char ch, int bkgnd, int frgnd )
{
#ifdef BAS_TEMP //TODO: Understand and replace
	char far *v;
	x--;y--;
	v = vid_mem;
	v += ( y * 160 ) + x * 2;  // compute char location
	*v++ = ch; 				// write the character
	*v = bkgnd*16+frgnd; 				// normal character
#endif //BAS_TEMP
}

void moutchar( int x, int y, char ch )
{
#ifdef BAS_TEMP //TODO: Understand and replace
	char far *v;
	v = vid_mem;
	v += ( y * 160 ) + x * 2;  // compute char location
	*v++ = ch; 				// write the character
	*v = 7; 				// normal character
#endif //BAS_TEMP
}

void mputchar( char ch, int bkgnd, int frgnd )
{
#ifdef BAS_TEMP //TODO: Understand and replace
	char far *v;
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
#endif //BAS_TEMP
}

int mgetch(int x, int y)
{
	char ch,atr;
#ifdef BAS_TEMP //TODO: Understand and replace
	char far *v;
	v = vid_mem;
	v += ( (y-1) * 160 ) + (x-1) * 2;  // compute char location
	ch = *v++; 				// write the character
	atr = *v; 				// normal character
#endif //BAS_TEMP
	return ((atr<<8) + ch);
}

int mgetword(int x, int y, char *pword, int length = 80)
{
#ifdef BAS_TEMP //TODO: Understand and replace
	char c;
	int i;
	hide_pointer();
	*pword = 0;
	if((char)mgetch(x,y)==' ')
	{
		display_pointer();
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
	display_pointer();
#endif //BAS_TEMP
	return 1;
}

/* Output a string at specified X, Y coordinates. This function IS
	 reentrant and may be called by any task at any time. */
void mxyputs( int x, int y, char *str )
{
#ifdef BAS_TEMP //TODO: Understand and replace
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
#endif //BAS_TEMP
}

void mxyputs( int x, int y, char *str, int bkgnd, int frgnd )
{
#ifdef BAS_TEMP //TODO: Understand and replace
	int initx;
	char far *v;
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
#endif //BAS_TEMP
}

void mputs( char *str )
{
#ifdef BAS_TEMP //TODO: Understand and replace
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
#endif //BAS_TEMP
}

void mputs( char *str, int bkgnd, int frgnd )
{
#ifdef BAS_TEMP //TODO: Understand and replace
	 char far *v;
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
#endif //BAS_TEMP
}

void mfarrealloc(char **far_point, unsigned long nbytes)
{
	char *p = *far_point;

	set_semaphore_dos();
	*far_point = (char *)realloc(p, nbytes);
	clear_semaphore_dos();
}
#ifdef BAS_TEMP //TODO: Understand and replace
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
#ifdef BAS_TEMP //TODO: Understand and replace
	if(handle)
	{
		set_semaphore_dos();
		GlobalUnlock( handle);
		GlobalFree( handle );
		clear_semaphore_dos();
	}
#endif //BAS_TEMP
}
#endif //BAS_TEMP
void mfarmalloc(char **far_point, unsigned long nbytes)
{
	set_semaphore_dos();
	*far_point=(char *)malloc(nbytes);
	if(*far_point != NULL)
		memset(*far_point, 0, nbytes);
	clear_semaphore_dos();
}

void mfarfree(void *far_point)
{
	if(far_point)
	{
		set_semaphore_dos();
		free((void *)far_point);
		clear_semaphore_dos();
	}
}

void mgettext(int left, int top, int right, int bottom, void *dest)
{
	set_semaphore(sem_screen);
#if BAS_TEMP
	gettext(left,top,right,bottom,(void *)dest);
#endif
	clear_semaphore(sem_screen);
}

void mputtext(int left, int top, int right, int bottom, void *source)
{
	set_semaphore(sem_screen);
#ifdef BAS_TEMP
	puttext(left,top,right,bottom,(void *)source);
#endif
	clear_semaphore(sem_screen);
}

void mclrscr(char x1, char y1, char x2, char y2, char col)
{
#ifdef BAS_TEMP //TODO: Understand and replace
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
#ifdef BAS_TEMP
	movetoxy(x,y);
	// gotoxy(x, y);
	xscreen = x-1;
	yscreen = y-1;
#endif //BAS_TEMP
}

time_t mtime( time_t *timer )
{
	time_t n;
	set_semaphore_dos();
	n = time(timer);
	clear_semaphore_dos();
	return n;
}


char *mctime(const time_t *time)
{
	char *p;
	set_semaphore_dos();
	p = ctime(time);
	clear_semaphore_dos();
	return p;
}

//**********************************************************************
//	Initialize the video subsystem.
//   Returns the current video mode.
//**********************************************************************
video_mode( void )
{
#ifdef BAS_TEMP //TODO: Understand and replace
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
#ifdef BAS_TEMP //TODO: Understand and replace
	int vmode;
	vmode = video_mode();
	if( vmode == 7 ) vid_mem = ( char *) MK_FP(__SegB000, 0 );
	else vid_mem = ( unsigned char *) MK_FP( __SegB800, 0 );
#endif //BAS_TEMP
}
