#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
 
#define MAX_COUNT 20
 
char usms[20][30] =
{
 {"\r\nFirst process"},
 {"\r\nSecond process"},
 {"\r\nThird process"},
 {"\r\nFourth process"},
 {"\r\nFifth process"} ,
 {"\r\nSixth process"},
 {"\r\nSeventh process"},
 {"\r\nEighth process"},
 {"\r\nNinth process"},
 {"\r\nTenth process"},
 {"\r\nEleventh process"},
 {"\r\nTwelfth process"},
 {"\r\nThirteenth process"},
 {"\r\nFourteenth process"},
 {"\r\nFifteenth process"} ,
 {"\r\nSixteenth process"},
 {"\r\nSeventeenth process"},
 {"\r\nEighteenth process"},
 {"\r\nNineteenth process"},
 {"\r\nTwentieth process"},
 
 };
pid_t child_pid[MAX_COUNT];
 
int fend = 1;
int cur = 1;
int size = 0;
int fprint = 0;
int fquit = 0;
struct sigaction printSignal, endSignal;
 
void checkEnd()
{
 if(fend)
 {
 kill(getppid(),SIGUSR2); 
 exit(0);
 }
}
 
int kbhit()
{
 fd_set rfds;
 struct timeval tv;
 
 FD_ZERO(&rfds);
 FD_SET(0,&rfds);
 tv.tv_sec=0;
 tv.tv_usec=100;
 
 return select(1,&rfds,NULL,NULL,&tv);
}
 
void canPrint(int signo)
{
 fprint = 1;
}
void setEndFlag(int signo)
{
 fend = 1;
}
 
int main(void)
{
 initscr();
 clear();
 noecho();
 refresh();
 
 printSignal.sa_handler = canPrint;
 sigaction(SIGUSR1,&printSignal,NULL);
 
 endSignal.sa_handler = setEndFlag;
 sigaction(SIGUSR2,&endSignal,NULL);
 
 char c = 0;
 int i = 0;
 
 child_pid[0]=getpid();
 
 while(c!='q')
 {
 if(kbhit>0)
 {
 switch(c=getchar())
 {
 case '+':
 {
 if(size < MAX_COUNT)
 {
 size++;
 child_pid[size] = fork();
 }
 
 switch(child_pid[size])
 {
 case 0: 
 {
 fend = 0;
 while(!fend)
 {
 usleep(10000);
 if(fprint)
 {
 for(i=0; i<strlen(usms[size-1]); i++)
 {
 checkEnd();
 printf("%c",usms[size-1][i]);
 refresh();
 usleep(40000);
 }
 checkEnd();
 refresh();
 fprint = 0;
 kill(getppid(),SIGUSR2);
 }
 checkEnd();
 }
 }
 break;
 
 case -1:
 {
 printf("Child process[%d] failed!\n",size);
 }
 break;
 }
 }
 break;
 
 case '-':
 {
 if(size==0)
 break;
 kill(child_pid[size],SIGUSR2); 
 waitpid(child_pid[size],NULL,0);
 if(cur == size) cur = 1;
 size--;
 }
 break;
 }
 }
 if(fend && size>0) 
 {
 fend = 0;
 kill(child_pid[cur++],SIGUSR1); //signal to child process about printing of string; at the begining cur=1
 if(cur > size) // if current number > amount of processes,
 cur = 1; //then begin from the start
 }
 refresh();
 }
 
 //finishing all child processes
 if(child_pid[size]!=0)
 {
 for(;size>0;size--)
 {
 kill(child_pid[size],SIGUSR2);
 waitpid(child_pid[size],NULL,0);
 }
 }
 clear();
 endwin();
 
 return 0;
}
