#pragma once


#ifdef __GNUC__
#pragma scalar_storage_order little-endian

#endif



#pragma pack(1) 

#define NAME_SIZE  17
#define MAX_VIEWS              3
#define MAX_TBL_BANK          15
#define MAX_PASSW             10
#define ALARM_MESSAGE_SIZE    58
#define MAX_UNITS              8
#define MAX_OUTS_MINI  64
#define MAX_INS_MINI   64

typedef struct {
	char   panel_type;
	uint32_t  active_panels;
	uint16_t  des_length;
	int    version;
	char   panel_number;
	char   panel_name[NAME_SIZE];
	int16_t    network;
	char   network_name[NAME_SIZE];
} Panel_info1;

typedef struct
{
	char description[21]; 	        // (21 bytes; string)*/
	char label[9];			           // (9 bytes; string)*/

	uint16_t bytes;		        // (2 bytes; size in bytes of program)*/
	int16_t     on_off : 1;    // (1 bit; 0=off; 1=on)*/
	int16_t     auto_manual : 1;    // (1 bit; 0=auto; 1=manual)*/
	int16_t     com_prg : 1;	  // (1 bit; 0=normal , 1=com program)*/
	int16_t     errcode : 5;	  // (1 bit; 0=normal end, 1=too long in program)*/
	char         unused;                // because of mini's

}	Str_program_point;	  /* 21+9+2+1+1 = 34 bytes*/

typedef struct
{
	char description[21];	      /*  (21 bytes; string)*/
	char label[9];		      /*  (9 bytes; string)*/

	uint32_t value;		      /*  (4 bytes; float)*/

	int16_t auto_manual : 1;  /*  (1 bit; 0=auto, 1=manual)*/
	int16_t digital_analog : 1;  /*  (1 bit; 0=digital, 1=analog)*/
	int16_t control : 1;
	int16_t unused : 5;
	int16_t range : 8; /*  (1 Byte ; variable_range_equate)*/

}	Str_variable_point; /* 21+9+4+2 = 36*/


typedef struct {
	char *address;
	int16_t str_size;
	int16_t max_points;
	char *name;
} Info_Table;


class Point_Net {
public:
	uint8_t number;
	uint8_t point_type;
	uint8_t panel;
	int16_t  network;
public:
	//void setzero(void);
	//int zero(void);
	//int cmp(byte num, byte p_type, byte p, int  net);
	//void putpoint(byte num, byte p_type, byte p, int  net);
	//void getpoint(byte *num, byte *p_type, byte *p, int *net);
	//int operator==(Point_Net compare);
	//void operator=(const Point_Net &copy);
};

class Point_T3000 {
public:
	uint8_t number;
	uint8_t point_type;
	uint8_t panel;
public:
	//void setzero(void);
	//int zero(void);
	//int cmp(byte num, byte p_type, byte p);
	//void putpoint(byte num, byte p_type, byte p);
	//void getpoint(byte *num, byte *p_type, byte *p);
	//int operator==(Point_T3000 a);
	//void operator=(const Point_T3000 &copy);
};

#define MAX_POINTS_IN_MONITOR 14


typedef struct {
	Point_Net point;
	uint32_t point_value;
	uint16_t auto_manual : 1;  // 0=auto, 1=manual
	uint16_t digital_analog : 1;  // 0=digital, 1=analog
	uint16_t description_label : 3;  // 0=display description, 1=display label
	uint16_t security : 2;  // 0-3 correspond to 2-5 access level
	uint16_t decomisioned : 1;  // 0=normal, 1=point decommissioned
	uint16_t units : 8;
} Point_info;

typedef struct {
	char view_name[11];
	char onoff;
	int32_t timerange;
} Views;

typedef struct
{
	char name[16]; 		       // (10 byte s; string)
	char password[9]; 		       // (9 byte s; string)
	uint8_t access_level;    // (1 byte ; 0-99)
	uint32_t rights_access;
	uint8_t default_panel;
	uint8_t default_group;
	char screen_right[8];     // 2 biti per grp
	char program_right[8];    // 2 biti per prg
} Password_point;

typedef struct 		// (size = 16 byte s)
{
	union {
		struct {
			char time_on_minutes1;		// (1 byte ; 0-59)
			char time_on_hours1;		// (1 byte ; 0-23)
			char time_off_minutes1;	// (1 byte ; 0-59)
			char time_off_hours1;		// (1 byte ; 0-23)
			char time_on_minutes2;		// (1 byte ; 0-59)
			char time_on_hours2;		// (1 byte ; 0-23)
			char time_off_minutes2;	// (1 byte ; 0-59)
			char time_off_hours2;		// (1 byte ; 0-23)
			char time_on_minutes3;		// (1 byte ; 0-59)
			char time_on_hours3;		// (1 byte ; 0-23)
			char time_off_minutes3;	// (1 byte ; 0-59)
			char time_off_hours3;		// (1 byte ; 0-23)
			char time_on_minutes4;		// (1 byte ; 0-59)
			char time_on_hours4;		// (1 byte ; 0-23)
			char time_off_minutes4;	// (1 byte ; 0-59)
			char time_off_hours4;		// (1 byte ; 0-23)
		} time1;
		char time2[16];
	};
} Wr_one_day;


typedef struct
{
	Point_info 	point_info; 				/* 11 bytes*/

	uint16_t 	show_point : 1;
	uint16_t 	icon_name_index : 7;
	uint16_t 	nr_element : 8;

	uint32_t        high_limit;
	uint32_t        low_limit;

	uint16_t 	graphic_y_coordinate : 10;
	uint16_t 	off_low_color : 4;
	uint16_t 	type_icon : 2;

	uint16_t 	graphic_x_coordinate : 10;
	uint16_t 	on_high_color : 4;
	uint16_t 	display_point_name : 1;
	uint16_t 	default_icon : 1;

	uint16_t 	text_x_coordinate : 7;
	uint16_t 	modify : 1;
	uint16_t 	point_absent : 1;    // 0 - point present, 1 - point absent
	uint16_t 	where_point : 2;
	uint16_t 	text_y_coordinate : 5;

	signed char bkgnd_icon;

	uint16_t		xicon : 10;
	uint16_t 	text_place : 4;
	uint16_t 	text_present : 1;
	uint16_t 	icon_present : 1;

	uint16_t		yicon : 10;
	uint16_t 	text_size : 2;
	uint16_t 	normal_color : 4;

}	Str_grp_element; /*8+2+4+4+2+2+1+1+2+2 = 32*/

typedef struct
{
	char description[21]; 	       /* (21 bytes; string)*/
	char label[9];		       /* (9 bytes; string)*/

	uint32_t value;		       /* (4 bytes; long) */

	uint16_t auto_manual : 1;  /* (1 bit; 0=auto, 1=manual)*/
	uint16_t digital_analog : 1;  /* (1 bit; 0=digital, 1=analog)*/
	uint16_t access_level : 3;  /* (3 bits; 0-5)*/
	uint16_t control : 1;  /* (1 bit; 0=off, 1=on)*/
	uint16_t digital_control : 1;  /* (1 bit)*/
	uint16_t decom : 1;  /* (1 bit; 0=ok, 1=point decommissioned)*/
	uint16_t range : 8;	/* (1 Byte ; output_range_equate)*/

	uint8_t m_del_low;  /* (1 Byte ; if analog then low)*/
	uint8_t s_del_high; /* (1 Byte ; if analog then high)*/
	uint16_t delay_timer;      /* (2 bytes;  seconds,minutes)*/

}  Str_out_point;  /* 21+4+2+2+9 = 40 */

typedef struct
{
	char description[21]; 	      /* (21 bytes; string)*/
	char label[9];		      	/* (9 bytes; string)*/

	uint32_t value;		     						/* (4 bytes; long)*/

	uint8_t filter : 3;  /* (3 bits; 0=1,1=2,2=4,3=8,4=16,5=32,6=64,7=128)*/
	uint8_t decom : 1;  /*	(1 bit; 0=ok, 1=point decommissioned)*/
	uint8_t sen_on : 1;  /* (1 bit)*/
	uint8_t sen_off : 1;  /* (1 bit)*/
	uint8_t control : 1; /*  (1 bit; 0=OFF, 1=ON)*/
	uint8_t auto_manual : 1;  /* (1 bit; 0=auto, 1=manual)*/
	uint8_t digital_analog : 1;  /* (1 bit; 0=digital, 1=analog)*/
	uint8_t calibration_sign : 1;  /* (0 positive; 1 negativ)*/
	uint8_t calibration_increment : 1;  /* (1 bit;  0=0.1, 1=1.0)*/
	uint8_t unused : 5;


	uint8_t  calibration;  /* (8 bits; -256.0 to 256.0 / -25.6 to 25.6 (msb is sign))*/

	uint8_t        range;	      			/* (1 Byte ; input_range_equate)*/

} Str_in_point; /* 21+1+4+1+1+9+1 = 38 */

typedef struct
{
	Point_T3000 input;	      /* (2 bytes; point)*/
	int32_t input_value; 	      /* (4 bytes; float)*/
	int32_t value;		      /* (4 bytes; float)*/
	Point_T3000 setpoint;	      /* (2 bytes; point)*/
	int32_t setpoint_value;	      /* (4 bytes; float)*/
	uint8_t units;    /* (1 Byte ; Analog_units_equate)*/

	uint8_t auto_manual : 1; /* (1 bit; 0=auto, 1=manual)*/
	uint8_t action : 1; /* (1 bit; 0=direct, 1=reverse)*/
	uint8_t repeats_per_min : 1; /* (1 bit; 0=repeats/hour,1=repeats/min)*/
	uint8_t unused : 1; /* (1 bit)*/
	uint8_t prop_high : 4; /* (4 bits; high 4 bits of proportional bad)*/

	uint8_t proportional : 8;   /* (1 Byte ; 0-2000 with prop_high)*/
	uint8_t reset;	      /* (1 Byte ; 0-255)*/
	uint8_t bias;	      /* (1 Byte ; 0-100)*/
	uint8_t rate;	      /* (1 Byte ; 0-2.00)*/
}	Str_controller_point; /* 2+4+4+2+4+1+1+4 = 24*/

typedef struct
{
	char label[9];		      	  					/* 9 bytes; string */

	Point_Net    inputs[MAX_POINTS_IN_MONITOR];	/* 28 bytes; array of Point_T3000 */
	uint8_t			 range[MAX_POINTS_IN_MONITOR]; /* 14 bytes */

	uint8_t second_interval_time; 				/* 1 byte ; 0-59 */
	uint8_t minute_interval_time; 				/* 1 byte ; 0-59 */
	uint8_t hour_interval_time;   				/* 1 byte ; 0-255 */

	uint8_t max_time_length;      /* the length of the monitor in time units */

	Views views[MAX_VIEWS];			/* 16 x MAX_VIEWS bytes */

	unsigned num_inputs : 4; 	/* total number of points */
	unsigned anum_inputs : 4; 	/* number of analog points */
	unsigned unit : 2; 	/* 2 bits - minutes=0, hours=1, days=2	*/
	unsigned ind_views : 2; 	/* number of views */
	unsigned wrap_flag : 1;		/* (1 bit ; 0=no wrap, 1=data wrapped)*/
	unsigned status : 1;		/* monitor status 0=OFF / 1=ON */
	unsigned reset_flag : 1; 	/* 1 bit; 0=no reset, 1=reset	*/
	unsigned double_flag : 1; 	/* 1 bit; 0= 4 bytes data, 1= 2 bytes data */

}	Str_monitor_point; 		/* 9+28+14+3+1+48+2 = 147 bytes */

typedef struct
{
	char *data_segment;
	unsigned start : 1;    	          // (1 bit)
	unsigned saved : 1;    	          // (1 bit)
	unsigned unused : 6;    	          // (6 bits)
	unsigned long next_sample_time;
	unsigned int  head_index;	    // (2 byte s; index to next data entry)
	unsigned long last_sample_time;	    // (4 byte s; time)
	unsigned long last_sample_saved_time;	    // (4 byte s; time)
	unsigned int  start_index_dig[MAX_POINTS_IN_MONITOR];
	unsigned int  end_index_dig[MAX_POINTS_IN_MONITOR];

}	Str_monitor_work_data;

typedef struct
{
	char description[21];		     /* (21 bytes; string)*/
	char label[9];		      	     /*	(9 bytes; string)*/

	unsigned value : 1;  /* (1 bit; 0=off, 1=on)*/
	unsigned auto_manual : 1;  /* (1 bit; 0=auto, 1=manual)*/
	unsigned override_1_value : 1;  /* (1 bit; 0=off, 1=on)*/
	unsigned override_2_value : 1;  /* (1 bit; 0=off, 1=on)*/
	unsigned off : 1;
	unsigned unused : 11; /* (11 bits)*/

	Point_T3000 override_1;	     /* (2 bytes; point)*/
	Point_T3000 override_2;	     /* (2 bytes; point)*/

}	Str_weekly_routine_point; /* 21+2+2+2+10 = 38*/

typedef struct
{
	char description[21]; 	    /* (21 bytes; string)*/
	char label[9];		      		/* (9 bytes; string)*/
	unsigned value : 1;  /* (1 bit; 0=off, 1=on)*/
	unsigned auto_manual : 1;  /* (1 bit; 0=auto, 1=manual)*/
	unsigned unused : 14; 	/* ( 12 bits)*/
}	Str_annual_routine_point;   /* 21+9+2=32 bytes*/

typedef struct
{
	char description[21];				/* (21 bytes; string)	*/
	char label[9];							/* (9 bytes; string)	*/
	char picture_file[11];			/* (11 bytes; string)	*/

	uint8_t                 update_time;       /* refresh time */
	unsigned mode : 1;				/* (1 bit ; 0=text, 1=graphic)	*/
	unsigned state : 1;        // 1 group displayed on screen 
	unsigned xcur_grp : 14;
	int    	             ycur_grp;
} Control_group_point;				/* (size = 46 bytes)	*/

typedef struct
{
	Str_grp_element	  *ptrgrp;
	int 			nr_elements;
} Control_group_elements;

typedef struct
{
	char hard_name[NAME_SIZE];         // (17 bytes; string)
	char name[NAME_SIZE]; 		       // (17 bytes; string)
	uint8_t number;
	unsigned des_length;
	unsigned descksum;
	uint8_t state;
	uint8_t panel_type;
	int  version;
	uint8_t tbl_bank[MAX_TBL_BANK];
} Station_point;

typedef struct {
	int ind_passwords;
	Password_point	passwords[MAX_PASSW];
} Password_struct;

typedef struct {

	Point_Net point;

	unsigned 	modem : 1;
	unsigned 	printer : 1;
	unsigned 	alarm : 1;
	unsigned 	restored : 1;
	unsigned 	acknowledged : 1;
	unsigned	ddelete : 1;
	unsigned  type : 2; /* DDDD */
	unsigned  cond_type : 4;
	unsigned  level : 4; /* DDDD */

	unsigned long 		alarm_time;
	char 			        alarm_count;
	char 			        alarm_message[ALARM_MESSAGE_SIZE + 1];
	char              none[5];
	unsigned char panel_type : 4;
	unsigned char dest_panel_type : 4;
	unsigned int      alarm_id;
	uint8_t              prg;

	uint8_t alarm_panel;   /* (1 byte ; 1-32, panel alarm originated from)	*/
	uint8_t where1;	      /* (1 byte ; panel# to send alarm to, 255 = all)	*/
	uint8_t where2;	      /* (1 byte ; panel# to send alarm to, 0 = none)	*/
	uint8_t where3;	      /* (1 byte ; panel# to send alarm to, 0 = none)	*/
	uint8_t where4;	      /* (1 byte ; panel# to send alarm to, 0 = none)	*/
	uint8_t where5;	      /* (1 byte ; panel# to send alarm to, 0 = none)	*/
	unsigned where_state1 : 1;	      /* (1 byte ; panel# to send alarm to, 255 = all)	*/
	unsigned where_state2 : 1;	      /* (1 byte ; panel# to send alarm to, 255 = all)	*/
	unsigned where_state3 : 1;	      /* (1 byte ; panel# to send alarm to, 255 = all)	*/
	unsigned where_state4 : 1;	      /* (1 byte ; panel# to send alarm to, 255 = all)	*/
	unsigned where_state5 : 1;	      /* (1 byte ; panel# to send alarm to, 255 = all)	*/
	unsigned change_flag : 2;
	unsigned original : 1;
	unsigned no : 8;
	//	unsigned line          :11;
	//	byte     no;
} Alarm_point;


typedef struct
{
	Point_T3000 point;
	Point_T3000 point1;
	unsigned cond1;
	long waylow;
	long low;
	long normal;
	long hi;
	long wayhi;
	unsigned time;
	signed char nrmes;
	unsigned count;
} Alarm_set_point;

typedef struct 		// (size = 11 byte s)
{
	char label[9];		     // (9 byte s; string)
	int  length;  	 // (1 byte ; 0-255)
} Str_array_point;

typedef struct	// (size = 6 byte s)
{
	int16_t value; 		       // (10 byte s; string)
	int32_t unit; 		       // (9 byte s; string)
}  Tbl_point;

typedef struct
{
	char label[9];		      // (9 bytes; string)*/
	Tbl_point table[16];
} Str_tbl_point;      // size = 9+6*16 = 105

typedef struct {
	char direct;
	char digital_units_off[12];       //12 bytes; string)
	char digital_units_on[12];        //12 bytes; string)
} Units_element;             // 30 bytes;

#define MAX_CONS	16
#define MAX_HEAP_GRP   26 * 1024
#define MAX_ELEM	3
#define MAX_ICON_NAME_TABLE   16
#define MAX_TBL_BANK          15


#define MAX_OUTS	32
#define MAX_INS	32
#define MAX_VARS	128
#define MAX_CONS	16
#define MAX_WR		16
#define MAX_AR		8
#define MAX_PRGS	32
#define MAX_TABS	5
#define MAX_DIGM	1
#define MAX_ANALM	16
#define MAX_GCONS	32
#define MAX_GRPS 	32
#define MAX_ARRAYS	16
#define MAX_INFO_TYPE 50
#define MAX_ALARMS            48
#define MAX_ALARMS_SET        16
#define SIZE_TEXT_RANGE       13
#define SIZE_TEXT_UNITS        6          /* 5 characeters plus a null   */


typedef struct {
	int16_t range;
	char range_text[SIZE_TEXT_RANGE];
	char aunits[SIZE_TEXT_UNITS];
	char value;
} an_range_form;


static an_range_form  var_range_array[] = {
	{ 0 , "  Unused   " , "     "  ,0 } ,
	{ 1 , "   1       " , "deg.C"  ,0 } ,
	{ 2 , "   2       " , "deg.F" 	,0 } ,
	{ 3 , "   3       " , "  FPM" 	,0 } ,
	{ 4 , "   4       " , "   Pa"	,0 } ,
	{ 5 , "   5       " , "  KPa" 	,0 } ,
	{ 6 , "   6       " , "  psi" 	,0 } ,
	{ 7 , "   7       " , " in-W" 	,0 } ,
	{ 8 , "   8       " , "Watts" 	,0 } ,
	{ 9 , "   9       " , "   KW" 	,0 } ,
	{ 10 , "  10      " , "  KWH" 	,0 } ,
	{ 11 , "  11      " , "Volts" 	,0 } ,
	{ 12 , "  12      " , "   KV" 	,0 } ,
	{ 13 , "  13      " , " Amps" 	,0 } ,
	{ 14 , "  14      " , "   ma" 	,0 } ,
	{ 15 , "  15      " , "  CFM" 	,0 } ,
	{ 16 , "  16      " , " Sec." 	,0 } ,
	{ 17 , "  17      " , " Min." 	,0 } ,
	{ 18 , "  18      " , "Hours"  ,0 } ,
	{ 19 , "  19      " , " Days"  ,0 } ,
	{ 20 , "  20      " , " Time"  ,0 } ,
	{ 21 , "  21      " , " ohms"  ,0 } ,
	{ 22 , "  22      " , "    %"  ,0 } ,
	{ 23 , "  23      " , "  %RH"  ,0 } ,
	{ 24 , "  24      " , "p/min"  ,0 } ,
	{ 25 , "  25      " , "count"  ,0 } ,
	{ 26 , "  26      " , "%Open"  ,0 } ,
	{ 27 , "  27      " , " %Cls"  ,0 } ,
	{ 28 , "  28      " , " CFH" 	,0 } ,
	{ 29 , "  29      " , " GPM" 	,0 } ,
	{ 30 , "  30	     " , " GPH" 	,0 } ,
	{ 31 , "  31      " , " GAL" 	,0 } ,
	{ 32 , "  32      " , "  CF" 	,0 } ,
	{ 33 , "  33      " , " BTU" 	,0 } ,
	{ 34 , "  34      " , " CMH" 	,0 } ,
	{ 35 , "  35      " , "Cust1"	,0 } ,
	{ 36 , "  36      " , "Cust2"	,0 } ,
	{ 37 , "  37      " , "Cust3"	,0 } ,
	{ 38 , "  38      " , "Cust4"	,0 } ,
	{ 39 , "  39      " , "Cust5"	,0 } ,


	{ 40 , "  40      " , "Cust6"	,0 } ,
	{ 41 , "  41      " , "Cust7"	,0 } ,
	{ 42 , "  42      " , "Cust8"	,0 }
};



class PRGReader
{

public:

Info_Table info[18];
Str_out_point   				   outputs[MAX_OUTS];
Str_in_point    				   inputs[MAX_INS];
Str_variable_point			   vars[MAX_VARS];
Str_controller_point 		   controllers[MAX_CONS];
Str_monitor_point				   analog_mon[MAX_ANALM];
Str_monitor_work_data      monitor_work_data[MAX_ANALM];
Str_weekly_routine_point   weekly_routines[MAX_WR];
Wr_one_day                 wr_times[MAX_WR][9];
Str_annual_routine_point	 annual_routines[MAX_AR];
//byte                       ar_dates[MAX_AR][46];
Str_program_point	 			   programs[MAX_PRGS];
uint8_t	     				     *program_codes[MAX_PRGS];   //  pointer to code
Control_group_point	 		   control_groups[MAX_GRPS];
Control_group_elements     control_group_elements[MAX_GRPS];
Station_point					     local_stations[32];
Password_struct            passwords;
Alarm_point 				       alarms[MAX_ALARMS];
Alarm_set_point 		       alarms_set[MAX_ALARMS_SET];
Str_array_point 				   arrays[MAX_ARRAYS];
Str_tbl_point custom_tab[MAX_TABS];
Units_element					     units[MAX_UNITS];


unsigned long ind_heap_grp = 0;
char *heap_grp;
char Icon_name_table[MAX_ICON_NAME_TABLE][14];
char(*icon_name_table)[14];
char just_load = 1;
int pixvar;

typedef enum { T3000, MINI_T3000, STANDARD, MINI_MAIN, SUB_MINI } PanelType;

char                       ar_dates[MAX_AR][46];

char *signature = (char *) "!@#$";

#define ERROR_COMM 1
#define SUCCESS 1
#define OUT 0
#define MAX_AR		8


#define OUT 		 0
#define IN  		 1
#define VAR 		 2
#define CON 		 3
#define WR 			 4
#define AR 			 5
#define PRG 		 6
#define TBL 	 	 7
#define DMON 		 8
#define AMON	    9
#define GRP 	   10
#define AY        11                  /* table to hold file names */
#define ALARMM    12
#define UNIT      13
#define USER_NAME 14
#define ALARMS    15
#define WR_TIME   16
#define AR_Y      17


#define LOCAL_VARIABLE 0x82
#define FLOAT_TYPE     0x83
#define LONG_TYPE      0x84
#define INTEGER_TYPE   0x85
#define BYTE_TYPE      0x86
#define STRING_TYPE    0x87
#define FLOAT_TYPE_ARRAY     0x88
#define LONG_TYPE_ARRAY      0x89
#define INTEGER_TYPE_ARRAY   0x8A
#define BYTE_TYPE_ARRAY      0x8B
#define STRING_TYPE_ARRAY    0x8C


int loadprg(char *file, Panel_info1 *panel_info);
int savefile(char *buf1, Panel_info1 *panel_info);
void init_info_table(void);
int saveloadpoint(int type, int point_type, int n_points, int size, int panel, int panel_type, FILE *h, char *buf);

};

#pragma pack()
