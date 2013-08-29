#include <stdlib.h>
#include <stdio.h>

void num2CN(int,char *);

int main()
{
	char *CN=(char *)malloc(sizeof(char)*20);
	num2CN(26,CN);

return 0;
}

void num2CN(int number,char *cn)
{	
	int i;
	int a,b;
	char ** CN_BUF;
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
	/*strcpy(CN_BUF[0],"零");
	strcpy(CN_BUF[1],"一");
	strcpy(CN_BUF[2],"二");
	strcpy(CN_BUF[3],"三");*/
	
	//"四","五","六","七","八","九","十"};
	
	for(i=0;i<11;i++)
		printf("%s度\n",CN_BUF[i]);
	
	if(number>9)
	{
		a=number/10;//取十位数
		b=number%10;//取个位数
		
		strcpy(ones,CN_BUF[a]);
		strcpy(tens,CN_BUF[b]);
		memset(CN,'\0',20);
		strcat(CN,tens);
		strcat(CN,"十");
		strcat(CN,ones);
	}

printf("%s度\n",CN);

}
