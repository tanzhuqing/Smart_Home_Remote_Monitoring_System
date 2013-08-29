#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include "../keyboard/get_key.h"

char shell_s[]="\nkeyshell> ";

/*int argc, char *argv[*/
#define CMD_TPYE			(0)
#define PHONE_NUMBER		(1)
#define MAX_CMD_DEPTH  	(2)
extern volatile int STOP ;

extern char PHONE[];
void * keyshell()
{

	char cmd[256]={0,};
     //   char read_num[12];
	int i;
	int fd_num,lenth;
	kbd_init();		//键盘初始化	
	
	gprs_init();		//GPRS 初始化	
	
	printf("\n<gprs control shell>");
	printf("\n [0] register a number ");
	printf("\n [1]  change baudrate");
	printf("\n [2]  exit");
	printf("\n [**] help menu");

	while(1){

		printf(shell_s);
		fflush(stdout);

		get_line(cmd);
		printf("\r\n");				//显示必要的输出
               

              if(strncmp("0",cmd,1)==0)
			  {
                // fflush(stdout);
				 fd_num = open("phone_number",O_CREAT|O_RDWR|O_APPEND,0644);
           	 	 printf("\nthe number you have already registed:\n");
                 usleep(10000);
                 while(lenth=read(fd_num,cmd,11))
		   		{
					printf("\n");
		     	    write(1,cmd,lenth);    
		  		}
                  
		 		printf("\nInput a phone number to register into the number :\n");
			 	get_line(cmd);
                 printf("\n");
                // lenth = strlen(cmd);
             // printf("com[0]= %c,size of cmd = %d",cmd[0],lenth); 
                if(cmd[0]=='1' && strlen(cmd)==11)
               {
                // cmd[11]='\n';
			 	 write(fd_num,cmd,11);
	    	   }else
			   {
               	 printf("\nthe phone number you input is wrong!\n");
               }
               //memset(cmd,'\0',256);	
               	 cmd[0]=0;
             }
			else if(strncmp("1",cmd,1)==0)
			{
				printf("\nyou select to change baudrate, please input baudrate:");
				fflush(stdout);
				get_line(cmd);
				gprs_baud(cmd, strlen(cmd));
				printf("please exit and run as another baudrate.");
			}else if(strncmp("2",cmd,1)==0)
			{
				printf("\nexit this program!\n");
				STOP=1;
				break;
			}else if(strncmp("**",cmd,2)==0)
			{
				printf("\n<gprs control shell>");
				printf("\n [0]  register a number");
				printf("\n [1]  change baudrate");		
		    	printf("\n [2]  exit");
				printf("\n [**] help menu");
			}else if(cmd[0] != 0){
				system(cmd);
			}
	}		
}
