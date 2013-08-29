/************************************************
 *  GPRS demo, use ppp to connect internet
 *  use ttyS1 to ctrol GPRS
 *  by Liao ding shi <ah_zou@163.com>   
 *  2010-08-01
 *
*************************************************/


#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
//#include <sys/signal.h>
#include <pthread.h>
#include "tty.h"
#include "gprs.h"
#include "s3c2410-adc.h"
#include "ds1820.h"
/*--------------------------------------------------------*/
#define ENDMINITERM 27 /* ESC to quit miniterm */
#define FALSE		0
#define TRUE  		1
#define ADC_DEV   "/dev/adc/0raw"
#define DCM_DEV  "/dev/dcm/0raw"
#define GPIO_DEV  "/dev/demo"
#define DCM_IOCTRL_SETPWM      (0x10)
#define DCM_TCNTBO 		(16384)
/*--------------------------------------------------------*/
//升级版本后使用文件来记录以下常量
#define HOST_NUMBER  "13998182600"
#define ON_AIR "开空调"
#define OFF_AIR "关空调"
#define ON_RADIATOR "开暖气"
#define OFF_RADIATOR "关暖气"
#define ON_WATER_HEATER "开热水器"
#define OFF_WATER_HEATER "关热水器"
#define TEMPERATRUE   "温度"
#define CONDITION "状态"
#define DOOR_INSTALL "开盗警"
#define DOOR_UNINSTALL "关盗警"
#define AD_INSTALL "开火警"
#define AD_UNINSTALL "关火警"

/*---------------------------------------------------------*/
volatile int STOP=FALSE;
volatile int AD_STOP=FALSE;
volatile int DOOR_STOP=FALSE;
volatile int WIRING_STOP=FALSE;
int GET_GPRS_OK=FALSE;
int baud=B9600;
static int fd_gpio,fd_door,fd_dcm,fd_adc;
int get_baudrate(int argc,char** argv);
char my_buf[1024];
volatile int AIR_CTL=0;
volatile int RADIATOR=0;
volatile int WATER_HEATER=0;


char * cmd[20]={
		"at",
		"ate1",
		"at+CMGF=0",
		"at+CNMI=2,2,,1",
		"at+chfa=1", 			//设置通话通道为1,AT+CHFA 命令切换主副音频通道
		"at+clvl=100",			//设置受话器音量最大, AT+CLVL 命令可以调节输出音频信号增益
		"at+cmic=1,10"			//设置通道1的话筒增益
};
/*---------------------------------------------------------*/




static int init_ADdevice(void)
{
  if((fd_adc=open(ADC_DEV, O_RDWR))<0)
  {
  	printf("Error opening %s adc device\n", ADC_DEV);
 	return -1;
 }
 
}
//获取烟雾传感器电压			
static int GetADresult(int channel)
{   
	int PRESCALE=0XFF;
	int data=ADC_WRITE(channel, PRESCALE);
	write(fd_adc, &data, sizeof(data));
	read(fd_adc, &data, sizeof(data));
	
	return data;
}
//查看三机状态函数
void Look_condition(char *user_number)
{
	char *air=(char*)malloc(sizeof(char)*30);
	char *radiator=(char*)malloc(sizeof(char)*30);
	char *water_heater=(char*)malloc(sizeof(char)*30);
	char on[]="正在运行。";
	char off[]="已经关闭。";
	char *All=(char *)malloc(sizeof(char)*100);
	
	SM_PARAM  pDst;

	init_SM_PARAM(&pDst,user_number);

	printf("condition user_number: %s\n",user_number);

	if(AIR_CTL)
		sprintf(air,"空调%s",on);
	else
		sprintf(air,"空调%s",off);
		
	strcpy(All,air);
	
	if(RADIATOR)
		sprintf(radiator,"暖气%s",on);
	else
		sprintf(radiator,"暖气%s",off);
	
	strcat(All,radiator);
	
	if(WATER_HEATER)
		sprintf(water_heater,"热水器%s",on);
	else
		sprintf(water_heater,"热水器%s",off);
	
	strcat(All,water_heater);
	
	strcpy(pDst.TP_UD,All);

	gsmSendMessage(&pDst);
}
//控制直流机状态
void * air_condition_ctl()
{
	int stepwm=200;
	int factor=DCM_TCNTBO/1024;

		
	while(1)
	{
		if(AIR_CTL==1)
			ioctl(fd_dcm,DCM_IOCTRL_SETPWM,(stepwm*factor));
		else
			ioctl(fd_dcm,DCM_IOCTRL_SETPWM,0);
	}
	
}
void number2CN(int number,char *cn)
{
	int i;
	int a,b;
	char * CN_BUF[11];
	char *ones=(char *)malloc(sizeof(char)*4);
	char *tens=(char *)malloc(sizeof(char)*4);
	char CN[20];

	for(i=0;i<11;i++)
	{
		CN_BUF[i]=(char *)malloc(sizeof(char)*4);
		
		if(i== 0)
			strcpy(CN_BUF[0],"零");
		if(i==1)
			strcpy(CN_BUF[1],"一");
		if(i==2)
			strcpy(CN_BUF[2],"二");
		if(i==3)
			strcpy(CN_BUF[3],"三");
	    if(i==4)
			strcpy(CN_BUF[4],"四");
		if(i==5)
			strcpy(CN_BUF[5],"五");
		if(i==6)
			strcpy(CN_BUF[6],"六");
		if(i==7)
			strcpy(CN_BUF[7],"七");
		if(i==8)
			strcpy(CN_BUF[8],"八");
		if(i==9)
			strcpy(CN_BUF[9],"九");
		if(i==10)
			strcpy(CN_BUF[10],"十");
	}
	
		memset(CN,'\0',20);
	
	if(number>9)
	{
		a=number/10;//取十位数
		b=number%10;//取个位数
		
		if(b!=0)
			strcpy(ones,CN_BUF[b]);
		strcpy(tens,CN_BUF[a]);
		strcat(CN,tens);
		strcat(CN,"十");
		if(b!=0)
			strcat(CN,ones);
		strcat(CN,"度");
	}
	else if(number>-1)
	{
		b=number%10;
		strcpy(CN,CN_BUF[b]);
	}
	else if(number>-10)
	{
		number=number*(-1);
		b=number%10;
		strcpy(CN,"负");
		strcat(CN,CN_BUF[b]);
		
	}else
	{
		number=number*(-1);
		a=number/10;
		b=number%10;
		
		if(b!=0)
			strcpy(ones,CN_BUF[b]);
		strcpy(tens,CN_BUF[a]);
		strcpy(CN,"负");
		strcat(CN,tens);
		strcat(CN,"十");
		if(b!=0)
			strcat(CN,ones);
		strcat(CN,"度");
	}

	strcpy(cn,CN);
	
}
//查询已注册用户号码,成功则返回1,否则返回0
int find_user(char *number)
{
	int fd;
	char buf[12];
	int lenth;
	
	fd=open("phone_number",O_RDWR|O_APPEND,0644);

	if(fd<-1)
		return -1;
	
	memset(buf,'\0',12);
	//实际号码前有86,所以指针要向前移动两位
	number+=2;

	while(lenth=read(fd,buf,11))
	{
		if(strcmp(buf,number)==0)
			return 1;
		memset(buf,'\0',12);
	}

	return 0;
}

//根据短信相应用户请求
void doquest(SM_PARAM *	pSrc)
{
	int door_condition;
	SM_PARAM  pDst;
	int temp;
	char *temp_buf=(char *)malloc(sizeof(char)*30);



	if(strcmp(pSrc->TP_UD,ON_AIR)==0){
		printf("\n The air_condition is opening.......\n");
		AIR_CTL=1;
	}
	else if(strcmp(pSrc->TP_UD,OFF_AIR)==0){
		printf("\n The air_condition is off\n");
		AIR_CTL=0;
	}
	else if(strcmp(pSrc->TP_UD,ON_RADIATOR)==0){
		printf("\n The radiator is opening......\n");
		ioctl(fd_gpio,5,NULL);	
		RADIATOR=1;
	}
	else if(strcmp(pSrc->TP_UD,OFF_RADIATOR)==0){
		printf("\n The radiator is off\n");
		ioctl(fd_gpio,6,NULL);
		RADIATOR=0;
	}
	else if(strcmp(pSrc->TP_UD,ON_WATER_HEATER)==0){
		printf("\n The water heater is opening......\n");
		ioctl(fd_gpio,3,NULL);
		WATER_HEATER=1;
	}
	else if(strcmp(pSrc->TP_UD,OFF_WATER_HEATER)==0){
		printf("\n The water heater is off.......\n");
		ioctl(fd_gpio,4,NULL);
		WATER_HEATER=0;
	}
	else if(strcmp(pSrc->TP_UD,TEMPERATRUE)==0){
		printf("\n Look the temperatrue\n");
		//调用读取温度函数
		temp=listen_DSC();
		printf("%d度\n",temp);
		number2CN(temp,temp_buf);
		printf("%s\n",temp_buf);
		init_SM_PARAM(&pDst,pSrc->TPA);
		strcpy(pDst.TP_UD,temp_buf);
		gsmSendMessage(&pDst);
		
	}
	else if(strcmp(pSrc->TP_UD,CONDITION)==0){
		printf("\nLook all wiring\n");
		//调用读取空条状态,暖气状态,热水器状态函数并给发送请求用户发送短信提示
		Look_condition(pSrc->TPA);		
	}
	else if(strcmp(pSrc->TP_UD,AD_INSTALL)==0){
		printf("\nInstall AD defence\n");
		AD_STOP=FALSE;
			
	}
	else if(strcmp(pSrc->TP_UD,AD_UNINSTALL)==0){
		printf("\nUnstall AD defence\n");
		AD_STOP=TRUE;
	}
	else if(strcmp(pSrc->TP_UD,DOOR_INSTALL)==0){
		printf("\nInstall DOOR defence\n");
		DOOR_STOP=FALSE;
	}
	else if(strcmp(pSrc->TP_UD,DOOR_UNINSTALL)==0){
		printf("\nUninstall DOOR defence\n");
		DOOR_STOP=TRUE;
	}
		
}

void * keyshell() ;

/*--------------------------------------------------------*/
/* modem input handler */
void* gprs_read(void * data)
{
	char buf[1024];
	char *token;
	int length=0;
	int len=0;
	SM_PARAM pDst;
  	printf("\nread modem\n");
	
	while(1){
//	tty_read(&c,1);
//	printf("%c",c);

	
	memset(buf,'\0',1024);
	
	length=tty_read(buf,1024);
	
	printf("PDU BUF:%s\n",buf);
	printf("len pdu: %d\n",length);
	
	if(length>45)
	{
		gsmDecodePdu(buf,&pDst);
		if(find_user(pDst.TPA))
			doquest(&pDst);
	}
}
   
  	printf("exit from reading modem\n");
  	return NULL; 
}
//监听烟雾传感器电压
void* listen_ad()
{
	  
	 int i;
	 int d;
	
	 SM_PARAM pSrc;
	 init_SM_PARAM(&pSrc,HOST_NUMBER);			
	
	printf("&&&&&&&&&&&&&&&&&\n");
     while(1)
	 {
	      
		 if(AD_STOP==FALSE)
		 {	 
         	d=GetADresult(3);
		 // printf("result: %d\n",d);
	      	if(d>600)
		  	{
		    	gsmSendMessage(&pSrc);
				AIR_CTL=0;//关空调
				ioctl(fd_gpio,0,NULL);//关热水器,关暖气
				printf("ad: %d\n",d);
				sleep(30);
			}
	  	}
	 }
		  sleep(1);
		  printf("\r");
 }
void * listen_door()
{
	int result;
	SM_PARAM pSrc;

	//可以使用读取文件中的用户号码来初始化
	
	init_SM_PARAM(&pSrc,HOST_NUMBER);

	strcpy(pSrc.TP_UD,"家中可能有盗");
	while(1)
	{
		if(DOOR_STOP==FALSE)
		{
			result=ioctl(fd_gpio,7,NULL);
			if(result==1)
			{
				printf("The door is open !\n");
				gsmSendMessage(&pSrc);
				sleep(30);
			}
		}
	}


}
//查看温度
int listen_DSC()
{
	float temperature;
	char psn[8];
	int i;
	static signed short tmp;
	gpioinit();
	readrom(psn);
	//for(i=0;i<8;i++) 
	//	printf("%d\n",psn[i]);

	delay(4000);
	startconverttmpt(psn);
	delay(50);
	tmp = readtemperature(psn);
//	printf("temperature:%d\n",tmp);

	return tmp;
			
}
/*--------------------------------------------------------*/
int main(int argc,char** argv)
{
	int ok;
 	pthread_t th_a, th_b, th_show,th_ad,th_door,th_air;
 	void * retval;
//	SM_PARAM sm;
		
	fd_gpio=open(GPIO_DEV,O_RDWR);
	fd_dcm=open(DCM_DEV,O_WRONLY);
		
	if(fd_gpio<0||fd_dcm<0)
	{
		printf("open fail!\n");
		return -1;	
	}

	if(init_ADdevice()<0)
	{
		return -1;
	}
	
	
	ioctl(fd_gpio,0,NULL);//灭灯
	

	
//	init_SM_PARAM(&sm,HOST_NUMBER);
	
	if(argc < 2) {
    // 	printf ("Default baudrate is 9600 bps. If not, please enter the baudrate as a parameter\n");
	}
       else
		baud=get_baudrate(argc, argv);
		tty_init();

		
//		gsmSendMessage(&sm);
		
  	pthread_create(&th_a, NULL, keyshell, 0);//键盘操作监听
 	pthread_create(&th_b, NULL, gprs_read, 0);//GPRS 串口监控,用于接收短信
	pthread_create(&th_air,NULL,air_condition_ctl,0);
	pthread_create(&th_ad,NULL,listen_ad,0);//烟雾传感器监听
	pthread_create(&th_door,NULL,listen_door,0);//门盗监听
	
	while(1){
		usleep(100000);
	}
  	tty_end();
  	exit(0); 
}

int get_baudrate(int argc,char** argv)
{
	int v=atoi(argv[1]);
	 switch(v){
		case 4800:
			return B4800;
			
		case 9600:
			return B9600;
			
		case 19200:
			return B19200;
			
		case 38400:
			return B38400;
			
		case 57600:
			return B57600;
			
		case 115200:
			return B115200;
		default:
			return -1;
	 } 	 
}

