// PRGReader.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "PRGReader.h"


#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

char *itoa(int num, char *str, int base)
{
        if (str == NULL)
        {
                return NULL;
        }
        sprintf(str, "%d", num);
        return str;
}


unsigned char tbl_bank[MAX_TBL_BANK];



int	PRGReader::saveloadpoint(int type, int point_type, int n_points, int size, int panel, int panel_type, FILE *h, char *buf)
{
	int r;
	uint16_t length;
	int n = 2;
	length = n_points*size;
	// delay(500);
	while (n--)
	{
//		if ((r = net_call(type + point_type + 1, 0, buf, &length, panel, networkaddress, NETCALL_RETRY)) == SUCCESS) break;
	}
	// Delay(150);
	if (r == SUCCESS)
	{
		if (!type)
		{
			n = length / size;
			if (panel_type == MINI_T3000)
			{
				if (point_type == OUT)
					n = MAX_OUTS_MINI;
				if (point_type == IN)
					n = MAX_INS_MINI;
			}
			fwrite(&n, 2, 1, h);
			fwrite(&size, 2, 1, h);
			fwrite(buf, size, n, h);
		}
	}
	return r;
}

int PRGReader::savefile(char *buf1, Panel_info1 *panel_info)
{
	int i, r = SUCCESS;
	int n, nitem, st, net, ver, vermini;
	uint16_t l;
	Str_out_point   		 *poutputs;
	Str_in_point    		 *pinputs;
	char *pold_outs_ins, *ptr, btext[8];
	int local = 1;
	char tempbuf[65];
	FILE *h = NULL;

	st = panel_info->panel_number;
#if 0
	if (local)
	{
		net = NetworkAddress;
	}
	else
	{
		net = networkaddress;
	}
#endif

	char *buf = new char[7500];
	

	pold_outs_ins = NULL;
	if (!local && panel_info->panel_type == MINI_T3000)
	{
		
		pold_outs_ins = new char[MAX_OUTS_MINI * sizeof(Str_out_point) + MAX_INS_MINI * sizeof(Str_in_point)];
		
		memset(pold_outs_ins, 0, MAX_OUTS_MINI * sizeof(Str_out_point) + MAX_INS_MINI * sizeof(Str_in_point));
		poutputs = (Str_out_point *)pold_outs_ins;
		pinputs = (Str_in_point *)(pold_outs_ins + MAX_OUTS_MINI * sizeof(Str_out_point));
		for (i = 0; i < MAX_OUTS_MINI; i++)
			poutputs[i].digital_analog = 1;
		for (i = 0; i < MAX_INS_MINI; i++)
		{
			pinputs[i].filter = 6;
			pinputs[i].digital_analog = 1;
		}
		
		if ((h = fopen(buf1, "rb")) != NULL)
		{
			fread(tempbuf, 26, 1, h); //time and date
			fread(tempbuf, 4, 1, h);  //signature
			if (!memcmp(tempbuf, signature, 4))
			{
				fread(&n, 2, 1, h);   //panel number
				fread(&nitem, 2, 1, h);   //network number
				fread(&ver, 2, 1, h);       //ver T3000
				fread(&vermini, 2, 1, h);   //ver mini
				fread(tempbuf, 1, 32, h); // reserved bytes
				if (ver >= 210 && ver != 0x2020)
				{
					if (n == st)
					{
						//									  fseek(h,70,SEEK_SET);  //  information - length 44 bytes
						fread(&n, 2, 1, h);
						fread(&nitem, 2, 1, h);
						//								  fread(pold_outs_ins,1,32*sizeof(Str_out_point),h);
						if (vermini)
							fread((char *)poutputs, n, nitem, h);
						else
						{
							fread((char *)poutputs, MAX_OUTS_MINI, nitem, h);
							fread(buf, n - MAX_OUTS_MINI, nitem, h);
						}

						//								  fseek(h,4,SEEK_CUR);
						fread(&n, 2, 1, h);
						fread(&nitem, 2, 1, h);
						//								  fread(pold_outs_ins+32*sizeof(Str_out_point),1,32*sizeof(Str_in_point),h);
						if (vermini)
							fread((char *)pinputs, n, nitem, h);
						else
						{
							fread((char *)pinputs, MAX_INS_MINI, nitem, h);
							fread((char *)buf, n - MAX_INS_MINI, nitem, h);
						}
					}
				}
			}
			fclose(h);
		}
		
	}
	
	if ((h = fopen(buf1, "wb+")) == NULL)
	{
		//									 strcpy(buf, "Error !!!. File not saved                  ");
		if (pold_outs_ins)
			delete  pold_outs_ins;
		if (buf)
			delete buf;
		
		printf("Error create file!\n");
		//				int_disk1--;
		return ERROR_COMM;
	}

#if 0
	strcpy(buf, panel_text1);
	lin_text[10] = 0;
	gauge->GWPuts(5, 10, lin_text, Lightblue, Black);
	lin_text[10] = ' ';
	itoa(st, &buf[6], 10);
	gauge->GWPuts(5, 10, buf, Lightblue, White);
#endif 

	long length;
	if (local)
	{
		length = sizeof(outputs) + sizeof(inputs) + sizeof(vars) + sizeof(controllers) +
			sizeof(analog_mon) + sizeof(arrays) + sizeof(custom_tab) + sizeof(alarms_set) +
			sizeof(weekly_routines) + sizeof(annual_routines) + sizeof(control_groups) +
			MAX_WR * 9 * sizeof(Wr_one_day) + MAX_AR * 46 + sizeof(units);
		for (i = 0; i < MAX_GRPS; i++)
			if (local)
			{
				if (control_group_elements[i].ptrgrp)
					length += control_group_elements[i].nr_elements * sizeof(Str_grp_element);
			}
		for (i = 0; i < MAX_PRGS; i++)
			if (local)
			{
				length += programs[i].bytes;
			}
	}
	else
		length = 13 + tbl_bank[PRG] + tbl_bank[WR] + tbl_bank[AR] + tbl_bank[GRP];
	//									float coef = (float)length/20.;
	long coef = ((length * 1000L) / 20000L) * 1000L + (((length * 1000L) % 20000L) * 1000L) / 20000L;
	long ltot = 0;

	
	//  information - length 40 bytes
	const time_t t = time(NULL);
	ptr =  ctime((const time_t *)&t);
	fwrite(ptr, 26, 1, h);         //time and date
	fwrite(signature, 4, 1, h);    //signature
	fwrite(&st, 2, 1, h);          //panel number
	fwrite(&net, 2, 1, h);      //network number
	int16_t Version = 259;
	n = Version;
	fwrite(&n, 2, 1, h);            //version T3000
	if (local || panel_info->panel_type == T3000)
		n = 0;
	else
		n = panel_info->version;
	fwrite(&n, 2, 1, h);            //version mini
	memset(buf, 0, 1000);
	fwrite(buf, 32, 1, h);      //reserved 32 bytes
	

	for (i = 0; i <= UNIT; i++)
	{
		if (i == AMON)
		{
			if (!local && panel_info->panel_type == MINI_T3000)
				continue;
		}
		if (i != DMON && i != ALARMM)
		{
			nitem = info[i].str_size;
			n = info[i].max_points;
			if (local)
			{
				
				fwrite(&n, 2, 1, h);
				fwrite(&nitem, 2, 1, h);
				fwrite(info[i].address, nitem, n, h);
				
				ltot += n*nitem;
			}
			else
			{
				n = tbl_bank[i];
				ptr = buf;
				if (i == OUT)
				{
					if (pold_outs_ins) ptr = (char *)poutputs;
				}
				if (i == IN)
				{
					if (pold_outs_ins) ptr = (char *)pinputs;
				}
				if ((r = saveloadpoint(0, i, n, nitem, st, panel_info->panel_type, h, ptr)) != SUCCESS)
					break;
				ltot++;
			}
		}
	}

	if (r == SUCCESS)
	{
		//									for( i=0;i<MAX_PRGS;i++)
		btext[0] = 'P';
		btext[1] = 'R';
		btext[2] = 'G';
		btext[3] = ' ';
		btext[4] = ' ';
		btext[5] = 0;
		for (i = 0; i < tbl_bank[PRG]; i++)
		{
			itoa(i, &btext[3], 10);
			if (local)
			{
				n = programs[i].bytes;
				ltot += n;
				
				fwrite(&n, 2, 1, h);
				fwrite(program_codes[i], n, 1, h);
				
			}
			else
			{
#if 0
				l = 0;
				delay(30);
				if ((r = net_call(16, i, buf, &l, st, networkaddress, NETCALL_RETRY)) != SUCCESS) break;
#endif			
				fwrite(&l, 2, 1, h);
				fwrite(buf, l, 1, h);
				
				//										Delay(100);
				ltot++;
			}
		}
		//									(gauge, coef, ltot);
	}
	if (r == SUCCESS)
	{
		nitem = 9 * sizeof(Wr_one_day);
		//									n = MAX_WR * nitem;
		n = tbl_bank[WR] * nitem;
		
		fwrite(&n, 2, 1, h);
		if (local)
		{
			fwrite(wr_times, n, 1, h);
			
			ltot += n;
		}
		else
		{
			//									 char block[9 * sizeof(Wr_one_day)];
			//									 for(i=0;i< MAX_WR;i++)
			
			for (i = 0; i < tbl_bank[WR]; i++)
			{
#if 0
				delay(30);
				if ((r = net_call(WR_TIME + 1, i, buf, 0, st, networkaddress, NETCALL_RETRY)) != SUCCESS) break;
				//										Delay(100);
#endif			
				fwrite(buf, nitem, 1, h);
				
				ltot++;
			}
		}
	}
	if (r == SUCCESS)
	{
		//									n = MAX_AR * 46;
		n = tbl_bank[AR] * 46;
		
		fwrite(&n, 2, 1, h);
		if (local)
		{
			fwrite(ar_dates, n, 1, h);
			
			ltot += n;
		}
		else
		{
			//									 char block[46];
			//									 for(i=0;i< MAX_AR;i++)
			
			for (i = 0; i < tbl_bank[AR]; i++)
			{
#if 0
				delay(30);
				if ((r = net_call(AR_Y + 1, i, buf, 0, st, networkaddress, NETCALL_RETRY)) != SUCCESS) break;
				//										Delay(100);
#endif			
				fwrite(buf, 46, 1, h);
				
				ltot++;
			}
		}
	}
	if (r == SUCCESS)
	{
		Str_grp_element *buff = NULL;
		
		if (!local)
			buff = new Str_grp_element[MAX_ELEM];
		n = sizeof(Str_grp_element);
		fwrite(&n, 2, 1, h);
		
		btext[0] = 'G';
		btext[1] = 'R';
		btext[2] = 'P';
		btext[3] = ' ';
		btext[4] = ' ';
		btext[5] = 0;
		for (i = 0; i < tbl_bank[GRP]; i++)
		{
			itoa(i, &btext[3], 10);
			if (local)
			{
				if (control_group_elements[i].ptrgrp)
					n = control_group_elements[i].nr_elements * sizeof(Str_grp_element);
				else
					n = 0;
				ltot += n;
				
				fwrite(&n, 2, 1, h);
				fwrite(control_group_elements[i].ptrgrp, n, 1, h);
				
			}
			else
			{
				l = 0;
#if 0
				delay(30);
				if ((r = net_call(19, i, (char *)buff, &l, st, networkaddress, NETCALL_RETRY)) != SUCCESS)
				{
					l = 0;
					if ((r = net_call(19, i, (char *)buff, &l, st, networkaddress, NETCALL_RETRY)) != SUCCESS)
						break;
				}
#endif
				//										Delay(100);
				
				fwrite(&l, 2, 1, h);
				fwrite(buff, l, 1, h);
				
				ltot++;
			}
		}
		//								(gauge, coef, length);
		if (!local && buff)
		{
			
			delete[] buff;
			
		}
	}
	if (r == SUCCESS)
	{
		if (local)
		{
			n = sizeof(Icon_name_table);
			
			fwrite(&n, 2, 1, h);
			fwrite(Icon_name_table, n, 1, h);
			
		}
		else
		{
			n = 0;
#if 0
			r = net_call(COMMAND_50, ICON_NAME_TABLE_COMMAND, (char*)buf, (unsigned int *)&n, st, networkaddress, NETCALL_RETRY);
			if (r == SUCCESS)
#endif
			{
				n = sizeof(Icon_name_table);
				
				fwrite(&n, 2, 1, h);
				fwrite(buf, n, 1, h);
				
			}
		}
	}
	else
	{
		printf("Error saving file!\n");
	}

	
	fclose(h);
	delete[] buf;
	if (pold_outs_ins)
		delete  pold_outs_ins;
	
	return r;
}


int PRGReader::loadprg(char *file, Panel_info1 *panel_info)
{
	init_info_table();

	char *buf, btext[8];
	int length, coef;
	short i, k, m, l1, c1, nitem, ver, vermini, ret, st;
	unsigned short n, j, l;
	Str_program_point *pprg;
	buf = new char[7500];   //7000

	FILE *h = NULL;

	ret = 0; // SUCCESS; //ERROR_COMM;
	st = panel_info->panel_number;
	if ((h = fopen(file, "rb+")) == NULL)
	{
		printf("File not loaded\n");
	}
	else
	{
		m = 1;
		fread(buf, 26, 1, h); //time and date
		fread(buf, 4, 1, h);  //signature
		if (!memcmp(buf, signature, 4))
		{
			fread(&n, 2, 1, h);   //panel number
			fread(&m, 2, 1, h);   //network number
			fread(&ver, 2, 1, h);       //ver T3000
			fread(&vermini, 2, 1, h);   //ver mini
			fread(buf, 1, 32, h); // reserved bytes

			if (ver < 210 || ver == 0x2020)
			{
				printf("File not loaded. File verrsion less than 2.10");
				m = 0;
			}
		}
		else
		{

			m = 0;
			printf("File corrupt\n");
		}

		if (n != st && m)
		{
			m = 1;
		}
		if (m)
		{
			m = SUCCESS;
			l = sizeof(tbl_bank);

			if (m == SUCCESS)
			{

				coef = ftell(h);    // save file pointer
				fseek(h, 0, SEEK_END);
				length = ftell(h);
				fseek(h, coef, SEEK_SET);   // restore file pointer
				coef = ((length * 1000L) / 20000L) * 1000L + (((length * 1000L) % 20000L) * 1000L) / 20000L;
				//									float coef = (float)length/20.;
				long ltot = 0;
				int max_prg = 0, max_grp = 0;
				for (i = OUT; i <= UNIT; i++)
				{
					if (i == AMON)
					{
						//										 if( panel_info1.panel_type == MINI_T3000 && panel_info1.version>110
						//										 if( panel_info1.version>110 && vermini >= 230  )
						if (ver >= 230 && vermini)
							continue;
						if (ver < 230 && vermini >= 230)
						{
							delete buf;

							printf("Versions conflict!\n");
							return ERROR_COMM;
						}
					}

					if (i == ALARMM)
						if (ver < 216)
						{
							fread(&n, 2, 1, h);
							fread(&nitem, 2, 1, h);
							fread(buf, n, nitem, h);
							continue;
						}
					if (i != DMON && i != ALARMM)
					{
						fread(&n, 2, 1, h);
						fread(&nitem, 2, 1, h);
						//l = min(n, info[i].max_points); FIXME
						if (i == GRP)
							max_grp = n;
						if (i == PRG)
						{
							max_prg = n;
						}

						{
							if (nitem == info[i].str_size)
							{
								fread(info[i].address, nitem, l, h);
							}
							else
							{
								m = min(nitem, info[i].str_size);
								for (j = 0; j < l; j++)
								{
									fread(info[i].address + j*info[i].str_size, m, 1, h);
									if (nitem > info[i].str_size)
										fseek(h, nitem - info[i].str_size, SEEK_CUR);
								}
							}
							if (n > l)
								fseek(h, (n - l)*nitem, SEEK_CUR);
						}
						ltot += n*nitem + 2;
					}
				}

				char *p = NULL, *q = NULL;
				int n1;

				// load programs code
				//									l=min(MAX_PRGS,max_prg);
				btext[0] = 'P';
				btext[1] = 'R';
				btext[2] = 'G';
				btext[3] = ' ';
				btext[4] = ' ';
				btext[5] = 0;
				l = min(max_prg, tbl_bank[PRG]);
				for (i = 0; i < l; i++)
				{
					itoa(i, &btext[3], 10);
					{

						fread(&n, 2, 1, h);
						fread(p, n, 1, h);

						if (n)
						{
							q = p;
							memcpy(&n1, q, 2);
							q += n1 + 2 + 3;
							memcpy(&n1, q, 2);
							q += 2;              //local variables
							for (j = 0; j < n1; )
							{
								switch (q[j]) {
								case FLOAT_TYPE:
								case LONG_TYPE:
									k = 4;
									break;
								case INTEGER_TYPE:
									k = 2;
									break;
								case BYTE_TYPE:
									k = 1;
									break;
								default:
								{
									switch (q[j]) {
									case FLOAT_TYPE_ARRAY:
									case LONG_TYPE_ARRAY:
										k = 4;
										break;
									case INTEGER_TYPE_ARRAY:
										k = 2;
										break;
									case BYTE_TYPE_ARRAY:
									case STRING_TYPE:
										//														case STRING_TYPE_ARRAY:
										k = 1;
										break;
									}
									memcpy(&l1, &q[j + 1], 2);
									memcpy(&c1, &q[j + 3], 2);
									if (l1)
										k *= l1*c1;
									else
										k *= c1;
									j += 4;
								}
								break;
								}
								j++;
								memset(&q[j], 0, k);
								j += k;
								j += 1 + strlen(&q[j]);
							}
							q += n1;
							memcpy(&n1, q, 2);   //time
							q += 2 + n1;
							memcpy(&n1, q, 2);    //ind_remote_local_list
							q += 2;         //remote_local_list
// FIXME							updateremotepoints(n1, (struct remote_local_list *)q);
						}
						/*
						if ((program_codes[i] = Heap_alloc(n)) != NULL)
						{
							movedata(FP_SEG(p), FP_OFF(p), FP_SEG(program_codes[i]), FP_OFF(program_codes[i]), n);
						}*/
#if 0
						programs[i].bytes = n;
						programs_size += n;
						ltot += n + 2;
						(gauge, coef, ltot);
#endif
					}
				}

				for (i = l; i < max_prg; i++)
				{
					fread(&n, 2, 1, h);
					fread(buf, n, 1, h);
					ltot += n + 2;
				}

				/*
				if(local)
				if(max_prg > MAX_PRGS)
				for(i=0;i< max_prg-MAX_PRGS;i++)
				{

				fread(	&n, 2, 1, h);
				fread( p, n, 1, h);
				ltot += n+2;

				}
				*/
				/*				if (local)
									mfarfree(handle);*/
									//				(gauge, coef, ltot);
									//				(gauge, "WR-T");


				m = MAX_WR * 9 * sizeof(Wr_one_day);
				fread(&n, 2, 1, h);
				l = min(n, m);

				{
					fread(wr_times, l, 1, h);
					if (n > m)
						fseek(h, n - m, SEEK_CUR);

				}

				ltot += n + 2;
				//				(gauge, coef, ltot);
				//				(gauge, "AR-D");


				m = MAX_AR * 46;
				fread(&n, 2, 1, h);
				l = min(n, m);

				{
					fread(ar_dates, l, 1, h);
					if (n > m)
						fseek(h, n - m, SEEK_CUR);

				}
				ltot += n + 2;


				Str_grp_element *pgrp;
				nitem = sizeof(Str_grp_element);

				fread(&nitem, 2, 1, h);


				//								  l=min(MAX_GRPS,max_grp);
				btext[0] = 'G';
				btext[1] = 'R';
				btext[2] = 'P';
				btext[3] = ' ';
				btext[4] = ' ';
				btext[5] = 0;
				l = min(tbl_bank[GRP], max_grp);
				for (i = 0; i < l; i++)
				{
					itoa(i, &btext[3], 10);
					{

						fread(&n, 2, 1, h);
						fread(p, n, 1, h);

						pgrp = (Str_grp_element *)p;
						for (j = 0; j < n / sizeof(Str_grp_element); j++, pgrp++)
						{
							pgrp->point_absent = 0;
							if (pgrp->point_info.point.point_type == OUT + 1)
								if (pgrp->point_info.point.number + 1 > tbl_bank[OUT])
									pgrp->point_absent = 1;
							if (pgrp->point_info.point.point_type == IN + 1)
								if (pgrp->point_info.point.number + 1 > tbl_bank[IN])
									pgrp->point_absent = 1;
						}
						ltot += n + 2;
#if 0
						if ((control_group_elements[i].ptrgrp = (Str_grp_element *)Heap_grp_alloc(n)) != NULL)
							movedata(FP_SEG(p), FP_OFF(p), FP_SEG(control_group_elements[i].ptrgrp), FP_OFF(control_group_elements[i].ptrgrp), n);
						control_group_elements[i].nr_elements = n / sizeof(Str_grp_element);
						grp_updateremotepoint(control_group_elements[i]);
#endif
					}
				}

				for (i = l; i < max_grp; i++)
				{
					fread(&n, 2, 1, h);
					fread(buf, n, 1, h);
					ltot += n + 2;
				}

				/*
				if(local)
				if(max_grp > MAX_GRPS)
				for(int i=0;i< max_grp-MAX_GRPS;i++)
				{
				fread(	&n, 2, 1, h);
				fread( p, n, 1, h);
				ltot += n+2;
				}
				*/
				//				(gauge, coef, length);


								//                  read icon_name_table
				n = 0;
				fread(&n, 2, 1, h);
				{
					fread(Icon_name_table, n, 1, h);

				}

				fclose(h);

				//				deletegauge(&gauge);
								//countdeslength();
			}
			else
			{

				fclose(h);

				ret = ERROR_COMM;
			}
		}
		else
		{
			fclose(h);

			ret = ERROR_COMM;
		}
	}

	delete buf;


	return ret;
}


void PRGReader::init_info_table(void)
{
	int i;
	memset(info, 0, 18 * sizeof(Info_Table));
	for (i = 0; i < 18; i++)
	{
		switch (i)
		{
		case OUT:
			info[i].address = (char *)outputs;
			info[i].str_size = sizeof(Str_out_point);
			info[i].name = (char *) "OUT";
			info[i].max_points = MAX_OUTS;
			break;
		case IN:
			info[i].address = (char *)inputs;
			info[i].str_size = sizeof(Str_in_point);
			info[i].name = (char *)"IN";
			info[i].max_points = MAX_INS;
			break;
		case VAR:
			info[i].address = (char *)vars;
			info[i].str_size = sizeof(Str_variable_point);
			info[i].name = (char *)"VAR";
			info[i].max_points = MAX_VARS;

			memset(vars, 0, info[i].str_size * info[i].max_points);
			break;
		case CON:
			info[i].address = (char *)controllers;
			info[i].str_size = sizeof(Str_controller_point);
			info[i].name = (char *)"CON";
			info[i].max_points = MAX_CONS;
			break;
		case WR:
			info[i].address = (char *)weekly_routines;
			info[i].str_size = sizeof(Str_weekly_routine_point);
			info[i].name = (char *)"WR";
			info[i].max_points = MAX_WR;
			break;
		case AR:
			info[i].address = (char *)annual_routines;
			info[i].str_size = sizeof(Str_annual_routine_point);
			info[i].name = (char *)"AR";
			info[i].max_points = MAX_AR;
			break;
		case PRG:
			info[i].address = (char *)programs;
			info[i].str_size = sizeof(Str_program_point);
			info[i].name = (char *)"PRG";
			info[i].max_points = MAX_PRGS;
			break;
		case TBL:
			info[i].address = (char *)custom_tab;
			info[i].str_size = sizeof(Str_tbl_point);
			info[i].name = (char *)"TBL";
			info[i].max_points = MAX_TABS;
			break;
		case DMON:
			info[i].address = NULL;
			info[i].name = (char *) "";
			break;
		case AMON:
			info[i].address = (char *)analog_mon;
			info[i].str_size = sizeof(Str_monitor_point);
			info[i].name = (char *)"AMON";
			info[i].max_points = MAX_ANALM;
			break;
		case GRP:
			info[i].address = (char *)control_groups;
			info[i].str_size = sizeof(Control_group_point);
			info[i].name = (char *)"GRP";
			info[i].max_points = MAX_GRPS;
			break;
		case AY:
			info[i].address = (char *)arrays;
			info[i].str_size = sizeof(Str_array_point);
			info[i].name = (char *)"AY";
			info[i].max_points = MAX_ARRAYS;
			break;
		case ALARMM:          //12
			info[i].address = (char *)alarms;
			info[i].str_size = sizeof(Alarm_point);
			info[i].name = (char *)"ALR";
			info[i].max_points = MAX_ALARMS;
			break;
		case UNIT:
			info[i].address = (char *)units;
			info[i].str_size = sizeof(Units_element);
			info[i].name = (char *)"UNITS";
			info[i].max_points = MAX_UNITS;
			break;
		case USER_NAME:
			info[i].address = (char *)&passwords;
			info[i].str_size = sizeof(Password_point);
			info[i].name = (char *)"PASS";
			info[i].max_points = MAX_PASSW;
			break;
		case ALARMS:          //12
			info[i].address = (char *)alarms_set;
			info[i].str_size = sizeof(Alarm_set_point);
			info[i].name = (char *) "ALRS";
			info[i].max_points = MAX_ALARMS_SET;
			break;
			/*	case 15: {
			//		  &system_info[i].name = data;
			System_Name(  ctype );
			info[i].name = "";
			break;
			}
			case 16: {
			program_code = ( byte * ) data;
			result = Program_Code( ctype, code_length, subscript );
			info[i].name = "";
			break;
			}************************/
		case WR_TIME:
			info[i].address = (char *)wr_times;
			info[i].str_size = 9 * sizeof(Wr_one_day);
			info[i].name = (char *)"WR_T";
			break;
		case AR_Y:                //17
			info[i].address = (char *)ar_dates;
			info[i].str_size = 46;
			info[i].name = (char *)"AR_D";
			break;
		}

	}
}


#if 0
int main()
{
	Panel_info1 panel_info;

	panel_info.panel_number = 1;

	PRGReader reader;

	reader.loadprg("C:\\Users\\jpmendoza\\Downloads\\PRGs\\PANEL1.PRG", &panel_info);

	for (int i = 0; i < MAX_VARS; i++)
	{
		Str_variable_point *current_var = &reader.vars[i];

		if (!current_var->description[0])
			continue;

		printf("---------------------------\n");
		printf("Description: %s\n", current_var->description);
		printf("Label: %s\n", current_var->label);
		printf("Value: %d\n", current_var->value);
		printf("AUTO/MANUAL: %s\n", current_var->auto_manual ? "MANUAL" : "AUTO");

	}

	reader.savefile("C:\\Users\\jpmendoza\\Downloads\\PRGs\\NEW.PRG", &panel_info);


	return 0;
}
#endif
