/*
  nscltime.h
  by Chase Bolen
  2-25-00
  
  Functions to get current date and time, convert them to ascii,
  format them, and return them to nsclbin.
   
*/

#include <time.h>
#include <iostream.h>
#ifndef NSCLTIME_
#define NSCLTIME_
int exp(int, int);

void chartime(char t[9]) {
  void itochar(int,char[]);
  time_t tloc;
  tm *timestr;
  time(&tloc);
  timestr = localtime(&tloc);
  char hr[3], min[3],sec[3];
  itochar(timestr->tm_hour, hr);
  itochar(timestr->tm_min, min);
  itochar(timestr->tm_sec, sec);
  
  strcpy(t, hr);
  strcat(t,":");
  strcat(t,min);
  strcat(t,":");
  strcat(t,sec);
}
void chardate(char d[10]) {  
  void itochar(const int,char[]);
  void charmonth(const int,char[]);
  time_t tloc;
  int iyear;
  tm *timestr;
  time(&tloc);
  timestr = localtime(&tloc);
  char day[3], mon[6],year[3];
  itochar(timestr->tm_mday, day);
  iyear = timestr->tm_year-(timestr->tm_year/100)*100;
  itochar(iyear, year);
  strcpy(d,day);
  charmonth(timestr->tm_mon+1, mon);
  strcat(d,mon);
  strcat(d,year);
}
void charmonth(int imon, char mon[6]) {
  switch(imon) {
  case 1:
    strcpy(mon, "-JAN-");
    break;
  case 2:
    strcpy(mon, "-FEB-");
    break;
  case 3:
    strcpy(mon, "-MAR-");
    break;
  case 4:
    strcpy(mon, "-APR-");
    break;
  case 5:
    strcpy(mon, "-MAY-");
    break;
  case 6:
    strcpy(mon, "-JUN-");
    break;
  case 7:
    strcpy(mon, "-JUL-");
    break;
  case 8:
    strcpy(mon, "-AUG-");
    break;
  case 9:
    strcpy(mon, "-SEP-");
    break;
  case 10:
    strcpy(mon, "-OCT-");
    break;
  case 11:
    strcpy(mon, "-NOV-");
    break;
  case 12:
    strcpy(mon, "-DEC-");
    break;
  }  
}
void itochar(int i,char text[]) {
  char charin[5];
  memset(charin,0,5);
  int temp=i,s=-1;
  for(int n = 4; n>=0; n--) {
    if ((temp/(exp(10,n)))>0) {
      if (s==-1) s=n;
      charin[s-n]=48+(temp/exp(10,n));
      temp = temp%exp(10,n);
    }
    if ((charin[s-n]==0)&&(s!=-1)) charin[s-n]='0';	
  }
  if (s==-1) charin[0]=charin[1]='0';
  if ((charin[1]==0)&&(charin[0]!=0)) {
    charin[1]=charin[0];
    charin[0]='0';
  }
  strcpy(text,charin);
}
int exp(int num, int power) {
  if (power==0) return 1;
  int temp = num;
  for(int i = 1;i<power;i++) {
    temp*=num;
  }
  return temp;
}
#endif
