#include <string.h>
#include <fcntl.h>
int find_user(char *number)
{

	int fd;
	char buf[12];
	int lenth;
	char cmd;
	fd=open("phone_number",O_RDWR|O_APPEND,0644);

	if(fd<-1)
		return -1;
	memset(buf,'\0',12);
	
	number+=2;
	while(lenth=read(fd,buf,11))
	{
		//write(1,buf,11);
		printf("buf: %s\n",buf);
		if(strcmp(buf,number)==0)
			return 1;
		memset(buf,'\0',12);
	}

	return 0;
}

int main()
{
	char number[]="8613704030599";
	int flag=0;
	
	flag=find_user(number);
	printf("find result : %d",flag);

}
