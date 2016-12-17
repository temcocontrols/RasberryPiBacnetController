/******************************************************************************
 * File Name: date.cpp
 * 
 * Description: 
 *
 * Created:
 * Author:
 *****************************************************************************/

/******************************************************************************
 * INCLUDES
 *****************************************************************************/

#include "fxengine.h"
#include "t3000def.h"
#include "baseclas.h"
//#include "graph.h"
//#include "gedit.h"

/******************************************************************************
 * PREPROCESSORs
 *****************************************************************************/
 
//  max_HEAP_BUF + max_HEAP_GRP <= 64K
//  MAX_HEAP_AMON+(long)MAX_HEAP_DIGM*sizeof(Heap_dmon)+(long)MAX_HEAP_ARRAY*4L <= 64k
#define max_HEAP_BUF   26 * 1024
#define max_HEAP_GRP   26 * 1024
#define max_HEAP_AMON  26 * 1024
#ifdef BAS_TEMP
#define max_HEAP_DIGM  (MAX_MEM_DIG_BUF/sizeof(Heap_dmon)-1) + 100
#endif //BAS_TEMP
#define max_HEAP_ARRAY 10 * 1024 / 4
unsigned int MAX_HEAP_BUF = max_HEAP_BUF;   //15000;
unsigned int MAX_HEAP_GRP = max_HEAP_GRP;   //15000;
unsigned int MAX_HEAP_AMON = max_HEAP_AMON;   //15000;
#ifdef BAS_TEMP
unsigned int MAX_HEAP_DIGM = max_HEAP_DIGM;   //15000;
#endif //BAS_TEMP
unsigned int MAX_HEAP_ARRAY = max_HEAP_ARRAY;   //15000;
//#endif

/******************************************************************************
 * GLOBALs
 *****************************************************************************/
 
#ifdef WORKSTATION
char heap_buf[1];
unsigned long ind_heap_buf=0;
#else
//char *heap_buf;
char *heap_buf;
unsigned long ind_heap_buf=0;
#endif

//TBD: HANDLE is not known. Its just a fix. Use appropriate one.
typedef int HANDLE;

HANDLE heap_buf_handle;
//char *heap_grp;
char *heap_grp;
unsigned long ind_heap_grp=0;
HANDLE heap_grp_handle;

//char *heap_amon;
char *heap_amon;
HANDLE heap_amon_handle;

//Heap_dmon *heap_dmon;  // 5000
Heap_dmon *heap_dmon;  // 5000
HANDLE heap_dmon_handle;
unsigned first_free;

//long *heap_array;
long *heap_array;
HANDLE heap_array_handle;


char point_cod[MAX_TBL_BANK][5]={{"OUT"},{"IN"},{"VAR"},{"CON"},{"WR"},{"AR"},{"PRG"},{"TBL"},{"DMON"},
													 {"AMON"},{"GRP"},{"AY"},{"ALAR"},{"UNIT"},{"PASS"}};

//unsigned char str_max_points[AY+1]={128,128,128,128,32,8,128,5,128,96,64,48};
//unsigned char  mini_max_points[AY+1]={8,8,32,8,4,2,5,3,8,8,4,0};

//char  mons[12]={31,29,31,30,31,30,31,31,30,31,30,31};

unsigned char filter[8]={1,2,4,8,16,32,64,128};

//int analog_limits[6]={10,10,20,10,10,20};

dig_range_form dig_range_array[]  = {
		 { 0 , "  Unused    " , ""				 , "" , 0} ,
		 { 1 , "  Off/ On   " , "Off"      , "On" ,0} ,
		 { 2 , "Close/Open  " , "Close"    , "Open" ,2} ,
		 { 3 , "Stop/Start  " , "Stop"     , "Start" ,4} ,
		 { 4 , "Dis/Enable  " , "Disable"  , "Enable" ,6} ,
		 { 5 , "Norm/Alarm  " , "Normal"   , "Alarm" ,8} ,
		 { 6 , " Norm/High  " , "Norm"     , "High" ,10} ,
		 { 7 , " Norm/Low   " , "Norm"     , "Low" ,12}   ,
		 { 8 , " No / Yes   " , "No"       , "Yes" ,14} ,
		 { 9 , " Cool/Heat  " , "Cool"     , "Heat" ,32} ,
		 { 10 , " Un/Occupy  " , "Unoccup"	, "Occupied" ,34} ,
		 { 11 , "  Low/High  " , "Low"		  , "High" ,36} ,

		 { 12 , "  On/Off    " , "On"       , "Off" ,1} ,
		 { 13 , "Open/Close  " , "Open"     , "Close" ,3} ,
		 { 14 , "Start/Stop  " , "Start"    , "Stop" ,5} ,
		 { 15 , "En/Disable  " , "Enable"   , "Disable" ,7} ,
		 { 16 , "Alarm/Norm  " , "Alarm"    , "Normal" ,9} ,
		 { 17 , " High/Norm  " , "High"     , "Normal" ,11} ,
		 { 18 , " Low/Norm   " , "Low"      , "Normal" ,13}   ,
		 { 19 , " Yes/No     " , "Yes"      , "No" ,15} ,
		 { 20 , "  Heat/Cool " , "Heat"     , "Cool" ,33} ,
		 { 21 , " Occupied/Un" , "Occupied" , "Unoccup" ,35} ,
		 { 22 , "  High/Low  " , "High"     , "Low" ,37},

		 { 23	, "Cust Dig 1  " , BLANKCHAR, BLANKCHAR ,16} ,
		 { 24 , "Cust Dig 2  " , BLANKCHAR, BLANKCHAR ,18} ,
		 { 25 , "Cust Dig 3  " , BLANKCHAR, BLANKCHAR ,20 } ,
		 { 26 , "Cust Dig 4  " , BLANKCHAR, BLANKCHAR ,22 } ,
		 { 27 , "Cust Dig 5  " , BLANKCHAR, BLANKCHAR ,24 } ,
		 { 28 , "Cust Dig 6  " , BLANKCHAR, BLANKCHAR ,26} ,
		 { 29 , "Cust Dig 7  " , BLANKCHAR, BLANKCHAR ,28} ,
		 { 30 , "Cust Dig 8  " , BLANKCHAR, BLANKCHAR ,30}
	};
an_range_form in_range_array[]  = {
		 { 0  , "    Unused  " , "     " 	,0} ,           /*& default 0 range */
		 { 1  , "Y3K -40->150" , "deg.C" 	,1} ,           /*& default 0 range */
		 { 2  , "Y3K -40->300" , "deg.F" 	,2} ,      /* range 11 */
		 { 3  , "10K -40->120" , "deg.C" 	,1} ,      /* range 11 */
		 { 4  , "10K -40->250" , "deg.F" 	,2} ,      /* range 12 */
		 { 5  , "G3K -40->150" , "deg.C" 	,1} ,           /*& default 0 range */
		 { 6  , "G3K -40->300" , "deg.F" 	,2} ,      /* range 11 */
		 { 7  , "K10K-40->120" , "deg.C" 	,1} ,    /* range 13 */
		 { 8  , "K10K-40->250" , "deg.F" 	,2} ,
		 { 9  , "10K -50->110" , "deg.C" 	,1} ,
		 {10  , "10K -60->200" , "deg.F" 	,2} ,
		 {11  , " 0.0 -> 5.0 " , "Volts" 	,11} ,        /* non existent */
		 {12  , " 0.0 -> 100 " , " Amps" 	,13} ,        /* non existent */
		 {13  , " 0.0 -> 20  " , "   ma" 	,14} ,    /* non existent */
		 {14  , " 0.0 -> 20  " , "  psi" 	,6} , /* non existent */
		 {15  , " 0 --> 2^32 " ,  "count" 	,25} ,
		 {16  , " 0.0 -> 3000" ,  "  FPM" 	,3} ,
		 {17  , " 0.0 -> 100 " ,  "%0-5V"   ,11} ,
		 {18  , " 0.0 -> 100 " ,  "%4-20m"  ,14} ,
		 {19  , " 0 -> 65535 " ,  "p/min" 	,24} ,
		 {20  , "  Table 1   " ,  "....."   ,0} ,
		 {21  , "  Table 2   " ,  "....." 	,0} ,
		 {22  , "  Table 3   " ,  "....." 	,0} ,
		 {23  , "  Table 4   " ,  "....." 	,0} ,
		 {24  , "  Table 5   " ,  "....." 	,0}
		};


an_range_form  var_range_array[]  = {
		 { 0 , "  Unused   " , "     "  ,0} ,
		 { 1 , "   1       " , "deg.C"  ,0} ,
		 { 2 , "   2       " , "deg.F" 	,0} ,
		 { 3 , "   3       " , "  FPM" 	,0} ,
		 { 4 , "   4       " , "   Pa"	,0} ,
		 { 5 , "   5       " , "  KPa" 	,0} ,
		 { 6 , "   6       " , "  psi" 	,0} ,
		 { 7 , "   7       " , " in-W" 	,0} ,
		 { 8 , "   8       " , "Watts" 	,0} ,
		 { 9 , "   9       " , "   KW" 	,0} ,
		 { 10 , "  10      " , "  KWH" 	,0} ,
		 { 11 , "  11      " , "Volts" 	,0} ,
		 { 12 , "  12      " , "   KV" 	,0} ,
		 { 13 , "  13      " , " Amps" 	,0} ,
		 { 14 , "  14      " , "   ma" 	,0} ,
		 { 15 , "  15      " , "  CFM" 	,0} ,
		 { 16 , "  16      " , " Sec." 	,0} ,
		 { 17 , "  17      " , " Min." 	,0} ,
		 { 18 , "  18      " , "Hours"  ,0} ,
		 { 19 , "  19      " , " Days"  ,0} ,
		 { 20 , "  20      " , " Time"  ,0} ,
		 { 21 , "  21      " , " ohms"  ,0} ,
		 { 22 , "  22      " , "    %"  ,0} ,
		 { 23 , "  23      " , "  %RH"  ,0} ,
		 { 24 , "  24      " , "p/min"  ,0} ,
		 { 25 , "  25      " , "count"  ,0} ,
		 { 26 , "  26      " , "%Open"  ,0} ,
		 { 27 , "  27      " , " %Cls"  ,0} ,

		 { 28 , "  28      " , " CFH" 	,0} ,
		 { 29 , "  29      " , " GPM" 	,0} ,
		 { 30 , "  30	     " , " GPH" 	,0} ,
		 { 31 , "  31      " , " GAL" 	,0} ,
		 { 32 , "  32      " , "  CF" 	,0} ,
		 { 33 , "  33      " , " BTU" 	,0} ,
		 { 34 , "  34      " , " CMH" 	,0} ,
		 { 35 , "  35      " , "Cust1"	,0} ,
		 { 36 , "  36      " , "Cust2"	,0} ,
		 { 37 , "  37      " , "Cust3"	,0} ,
		 { 38 , "  38      " , "Cust4"	,0} ,
		 { 39 , "  39      " , "Cust5"	,0} ,


		 { 40 , "  40      " , "Cust6"	,0} ,
		 { 41 , "  41      " , "Cust7"	,0} ,
		 { 42 , "  42      " , "Cust8"	,0}
	};

an_range_form out_range_array[]  = {
		 { 0 , "  Unused    " , "     ", 0} ,
		 { 1 , " 0.0 -> 10  " , "Volts", 11} ,
		 { 2 , " 0.0 -> 100 " , "%Open", 26} ,
		 { 3 , " 0.0 -> 20  " , "  psi", 6} , /* non existent */
		 { 4 , " 0.0 -> 100 " , "    %", 22} ,
		 { 5 , " 0.0 -> 100 " , " %Cls", 27} ,
		 { 6 , " 0.0 -> 20  " , "   ma", 14}     /* non existent */
	};

char portconfl1[]   = " Port      not installed. Panel number      conflict. Change the panel number!";
char invalpanelnr[] = " Invalid panel number!. The valid panel numbers are between 1 and 32 ";

char sourcefile[]  = "Source File ";
char destfile[]    = "Destination File ";
char local_text[]  = "Local ";
char remote_text[] = "Remote ";
char more_text[]   = "More...";
char autoac_text[] = "Auto activate";
char userac_text[] = "User activate";
char async_text[]  = "Asyncron mode";
char estab_text[]  = "Trying to establish connection on COM1 ...";
char array_miniconf[][16]={{" Panel setting "},{" Ports setting "}};
char (*files)[13];
char directories[50][13];
char pressanykey[]="Press any key to continue...";
char grp[]="GRP  ";
char Main[]="MAIN";
char Mini[]="MINI";
char Suba[]="SUBA";
char Subb[]="SUBB";
char man[]="MAN ";
char autom[]="AUTO";
char nulbuf[]="          ";
char yes[]="Y";
char Yes[]="YES";
char No[]="NO ";
char lin[]="-";
char on[]="ON ";
char off[]="OFF";
char colon[]=":";
char zero[]="0";
char sun[]="SU";
char mon[]="MO";
char tue[]="TU";
char wed[]="WE";
char thu[]="TH";
char fri[]="FR";
char sat[]="SA";
char enabl[]="Enable ";
char disabl[]= "Disable";
char snothing[]="        ";
char analog_text[]="Analog";
char digital_text[]="Digital";
char slash[]="XXXXXXXX";
char hours[]="hours";
char days[]="days ";
char minut[]="min  ";
char tout[]     ="TIMEOUT";
char tindexout[]  ="INDEXAR";
char tioutrar[]   ="IRDAR";
char tioutwar[]   ="IWRAR";
char tioutray[]   ="IRDAY";
char tioutway[]   ="IWRAY";
char normal[]="Normal";
char na[]="N/A  ";
char com_prg_text[]="COM";
char hhelpline[] = "^L-seL ^U-Unsel ^D-Del block ^Y-Del line ^O-undO ^C-Copy ^F-Find ^N-Next ^R-Repl";
char loadtext[] = "File name [Load]: ";
char savetext[] =	"File name [Save]: ";
char searchstr[] = "Search string  :";
char replstr[]   = "Replace with   :";
char bas_text[]="*.BAS";
char array_ranges[][11]={{"Analog    "},{"Digital   "},{"Custom dig"}};
char array_access[][13]={{"Full access "},{"View only   "},{"Custom      "}};

char filetransfername[65];
char disconnect_mes[]="   If no activity connection will be          interrupted in     seconds!";

char current_path[65],filename_tmp[65];
char ptr_mouse[512];
char search_str[41];
char replace_str[41];

int worktime;

NETWORK_POINTS	network_points_list[MAXNETWORKPOINTS];
//WANT_POINTS		   want_points_list[MAXREMOTEPOINTS];
REMOTE_POINTS remote_points_list[MAXREMOTEPOINTS82];
char remote_points_OUT;
int remote_list_last_index_mstp, remote_list_last_index_ipx;

char int_disk, int_disk1;

int networkaddress;
int NetworkAddress=1;
char NetworkName[NAME_SIZE]={'N','E','T','W','O','R','K','1',0};

Str_tbl_point custom_tab[MAX_TABS];

/*
range_form huge range_array[MAX_RANGE]  = {
		 { 0  , "    Unused  " , "     " 	, ""		, "" ,0} ,
		 { 1  , "3K -40->150 " , "deg.C" 	, "", "" ,0} ,
		 { 2  , "10K -40->120" , "deg.C" 	, "", "" ,0} ,
		 { 3  , "10K -40->250" , "deg.F" 	, "", "" ,0} ,
		 { 4  , "K10K-40->120" , "deg.C" 	, "", "" ,0} ,
		 { 5  , "K10K-40->250" , "deg.F" 	, "", "" ,0} ,
		 { 6  , "10K -50->110" , "deg.C" 	, "", "" ,0} ,
		 { 7  , " 0.0 -> 5.0 " , "Volts" 	, "", "" ,0} ,
		 { 8  , " 0.0 -> 100 " , "Amps" 	, "", "" ,0} ,
		 { 9  , " 0.0 -> 20  " , "ma" 	, "", "" ,0} ,
		 { 10  , " 0.0 -> 20  " , "psi" 	, "", "" ,0} ,
		 { 11 , " 0 --> 2^32 " , "" 		, "", "" ,0} ,
		 { 12 , " 0.0 -> 3000" , "FPM" 	, "", "" ,0} ,
		 { 13 , " 0.0 -> 100 " , "%" 	, "", "" ,0} ,
		 { 14 , " 0.0 -> 100 " , "%" 	, "", "" ,0} ,
		 { 15 , " 0.0 -> 255 " , "p/min" 	, "", "" ,0} ,
		 { 16 , "  Table 1   " , "....."   	, "", "" ,0} ,
		 { 17 , "  Table 2   " , "....." 		, "", "" ,0} ,
		 { 18 , "  Table 3   " , "....." 		, "", "" ,0} ,
		 { 19 , "  Table 4   " , "....." 		, "", "" ,0} ,
		 { 20 , "  Table 5   " , "....." 		, "", "" ,0} ,
		 { 21 , "  Unused6   " , "....."  , ""     , "" ,0} ,
		 { 22 , "  Unused7   " , "....."  , ""    , "" ,0} ,
		 { 23 , "  Unused8   " , "....."  , ""    , "" ,0} ,

		 { 24 , "   24       " , "deg.C"  , "", "" ,0} ,
		 { 25 , "   25       " , "deg.F" 	, "", "" ,0} ,
		 { 26 , "   26       " , "  FPM" 	, "", "" ,0} ,
		 { 27 , "   27       " , "   Pa"	, "", "" ,0} ,
		 { 28 , "   28       " , "  KPa" 	, "", "" ,0} ,
		 { 29 , "   29       " , "  psi" 	, "", "" ,0} ,
		 { 30 , "   30       " , " in-W" 	, "", "" ,0} ,
		 { 31 , "   31       " , "Watts" 	, "", "" ,0} ,
		 { 32 , "32          " , "   KW" 	, "", "" ,0} ,
		 { 33 , "33          " , "  KWH" 	, "", "" ,0} ,
		 { 34 , "34          " , "Volts" 	, "", "" ,0} ,
		 { 35 , "35          " , "   KV" 	, "", "" ,0} ,
		 { 36 , "36          " , " Amps" 	, "", "" ,0} ,
		 { 37 , "37          " , "   ma" 	, ""    , "" ,0} ,
		 { 38 , "38          " , "  CFM" 	, ""    , "" ,0} ,
		 { 39 , "39          " , " Sec." 	, "" 	 , "" ,0} ,
		 { 40 , "40          " , " Min." 	, ""	 , "" ,0} ,
		 { 41 , "41          " , "Hours"  , "" , "" ,0} ,
		 { 42 , "42          " , " Days"  , ""     , "" ,0} ,
		 { 43 , "43          " , " Time"  , ""    , "" ,0} ,
		 { 44 , "44          " , " ohms"  , ""    , "" ,0} ,
		 { 45 , "45          " , "    %"  , ""    , "" ,0} ,
		 { 46 , "46          " , "  %RH"  , ""    , "" ,0} ,
		 { 47 , "47          " , "p/min"  , ""    , "" ,0} ,
		 { 48 , " 0.0 -> 10  " , "Volts"  , ""     , "" ,0} ,
		 { 49 , " 0.0 -> 100 " , "%Open"  , ""    , "" ,0} ,
		 { 50 , "50          " , ""  , "" , "" ,0} ,

		 { 51 , "  Off/ On   " , "" 		, "Off"      , "On" ,0} ,
		 { 52 , "Close/Open  " , "" 		, "Close"    , "Open" ,2} ,
		 { 53 , "Stop/Start  " , "" 		, "Stop"     , "Start" ,4} ,
		 { 54 , "Dis/Enable  " , "" 		, "Disable"  , "Enable" ,6} ,
		 { 55 , "Norm/Alarm  " , "" 		, "Normal"   , "Alarm" ,8} ,
		 { 56 , " Norm/High  " , "" 		, "Norm"     , "High" ,10} ,
		 { 57 , " Norm/Low   " , "" 		, "Norm"     , "Low" ,12}   ,
		 { 58 , " No / Yes   " , "" 		, "No"       , "Yes" ,14} ,

		 { 59 , "Cust Dig 1  " , "" 		, BLANKCHAR     , BLANKCHAR ,16} ,
		 { 60 , "Cust Dig 2  " , "" 		, BLANKCHAR     , BLANKCHAR ,18} ,
		 { 61 , "Cust Dig 3  " , "" 		, BLANKCHAR     , BLANKCHAR ,20 } ,
		 { 61 , "Cust Dig 4  " , "" 		, BLANKCHAR     , BLANKCHAR ,22 } ,
		 { 63 , "Cust Dig 5  " , "" 		, BLANKCHAR     , BLANKCHAR ,24 } ,
		 { 64 , "Cust Dig 6  " , "" 		, "off6"     , "on6" ,26} ,
		 { 65 , "Cust Dig 7  " , "" 		, "off7"     , "on7" ,28} ,
		 { 66 , "Cust Dig 8  " , "" 		, "off8"     , "on8" ,30} ,
		 { 67 , " Cool/Heat  " , ""		, "Cool"     , "Heat" ,32} ,
		 { 68 , " Un/Occupy  " , "" 		, "Unoccup", "Occupied" ,34} ,
		 { 69 , "  Low/High  " , "" 		, "Low"		  , "High" ,36} ,
		 { 70 , "    70      " , "" 		, ""    	 , "" ,0} ,
		 { 71 , "  On/Off    " , "" 		, "On"       , "Off" ,1} ,
		 { 72 , "Open/Close  " , "" 		, "Open"     , "Close" ,3} ,
		 { 73 , "Start/Stop  " , "" 		, "Start"    , "Stop" ,5} ,
		 { 74 , "En/Disable  " , "" 		, "Enable"   , "Disable" ,7} ,
		 { 75 , "Alarm/Norm  " , "" 		, "Alarm"    , "Normal" ,9} ,
		 { 76 , " High/Norm  " , "" 		, "High"     , "Normal" ,11} ,
		 { 77 , " Low/Norm   " , "" 		, "Low"      , "Normal" ,13}   ,
		 { 78 , " Yes/No     " , "" 		, "Yes"      , "No" ,15} ,

		 { 79 , "        79  " , " CFH" 	, ""    , "" ,0} ,
		 { 80 , "        80  " , " GPM" 	, ""    , "" ,0} ,
		 { 81 , "        81  " , " GPH" 	, ""    , "" ,0} ,
		 { 82 , "        82  " , " GAL" 	, ""    , "" ,0} ,
		 { 83 , "        83  " , "  CF" 	, ""    , "" ,0} ,
		 { 84 , "        84  " , " BTU" 	, ""    , "" ,0} ,
		 { 85 , "        85  " , " CMH" 	, ""    , "" ,0} ,
		 { 86 , "        86  " , "" 	, ""     , "" ,0} ,


		 { 87 , "  Heat/Cool " , "" 		, "Heat"      , "Cool" ,33} ,
		 { 88 , " Occupied/Un" , "     "  , "Occupied" , "Unoccup" ,35} ,
		 { 89 , "  High/Low  " , "     " 	, "High"     , "Low" ,37}

		 }   ;
*/
//
