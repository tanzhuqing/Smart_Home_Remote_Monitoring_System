#include <iconv.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "tty.h"
#include"gprs.h"

void gprs_init()
{ 
	int i;
	for(i=0; i<7; i++) {
		tty_writecmd(cmd[i], strlen(cmd[i])); 
	}

}
void gprs_hold()
{
	tty_writecmd("at", strlen("at"));
	tty_writecmd("ath", strlen("ath"));
}

void gprs_ans()
{ 
	tty_writecmd("at", strlen("at"));
	tty_writecmd("ata", strlen("ata"));
}

void gprs_call(char *number, int num)			
{ 

	tty_write("atd", strlen("atd"));
	tty_write(number, num);
	tty_write(";\r", strlen(";\r"));
	usleep(200000);
}

//使用文本模式发送短信
void gprs_msg(char *number, int num)		
{ 
	char ctl[]={26,0};
	char text[]="Welcome to use up-tech embedded platform!";
	tty_writecmd("at", strlen("at"));
	tty_writecmd("at", strlen("at"));
	tty_writecmd("at+cmgf=1", strlen("at+cmgf=1"));	
	tty_write("at+cmgs=", strlen("at+cmgs="));

    tty_write("\"", strlen("\""));
    tty_write(number, strlen(number));
    tty_write("\"", strlen("\""));
    tty_write(";\r", strlen(";\r"));
	tty_write(text, strlen(text));
	tty_write(ctl, 1);
	usleep(300000);
}

void gprs_baud(char *baud,int num)
{
	tty_write("at+ipr=", strlen("at+ipr="));
	tty_writecmd(baud, strlen(baud) );
	usleep(200000);
}
//将字符串转换成相应的比特位
int gsmString2Bytes(const char *psrc,unsigned char* pdst,int nsrclength)
{
	int i;
	for(i=0;i<nsrclength;i+=2)
	{
		if(*psrc>='0'&&*psrc<='9')
			*pdst=(*psrc - '0')<<4;
		else
			*pdst=(*psrc - 'A'+10)<<4;

		psrc++;

		if((*psrc)>='0'&&(*psrc)<='9')
		{
			*pdst |=*psrc - '0';
		}
		else
		{
			*pdst |=*psrc - 'A'+10;	
		}
		psrc++;
		pdst++;
	}
	return nsrclength/2;
}

int gsmBytes2String(const unsigned char* pSrc, char* pDst, int nSrcLength)
{
   	const char tab[]="0123456789ABCDEF";    // 0x0-0xf的字符查找表
	int i;
    for(i=0; i<nSrcLength; i++)
   {
      // 输出低4位
	      *pDst++ = tab[*pSrc >> 4];
								    
     // 输出高4位
	      *pDst++ = tab[*pSrc & 0x0f];
													    
	       pSrc++;
    }
																    
	 // 输出字符串加个结束符
		   *pDst = '\0';
														    
    // 返回目标字符串长度
	return nSrcLength * 2;
	
}
	
int gsmInvertNumbers(const char* pSrc, char* pDst, int nSrcLength)
{
    int nDstLength;   // 目标字符串长度
	char ch;          // 用于保存一个字符
	int i;	    
      // 复制串长度
	   nDstLength = nSrcLength;
					    
	  // 两两颠倒
	  for(i=0; i<nSrcLength;i+=2)
	   {
		   ch = *pSrc++;  // 保存先出现的字符
	      *pDst++ = *pSrc++;   // 复制后出现的字符
	      *pDst++ = ch;        // 复制先出现的字符
	    }
																    
       // 源串长度是奇数吗？
	  if(nSrcLength & 1)
	    {
		 *(pDst-2) = 'F';     // 补'F'
	     nDstLength++;        // 目标串长度加1
	   }
																									    
	   // 输出字符串加个结束符
	   *pDst = '\0';
																												    
	   // 返回目标字符串长度
	 return nDstLength;
}
																															
int gsmSerializeNumbers(const char* pSrc, char* pDst, int nSrcLength)
{
	int nDstLength;   // 目标字符串长度
	char ch;          // 用于保存一个字符
	int i;    
 // 复制串长度
    nDstLength = nSrcLength;
					    
  // 两两颠倒
    for(i=0; i<nSrcLength;i+=2)
	 {
	   ch = *pSrc++;        // 保存先出现的字符
	  *pDst++ = *pSrc++;   // 复制后出现的字符
	  *pDst++ = ch;        // 复制先出现的字符
	 }
																    
	// 最后的字符是'F'吗？
	if(*(pDst-1) == 'F')
	 {
	    pDst--;
        nDstLength--;        // 目标字符串长度减1
	   }
																									     
	// 输出字符串加个结束符
	   *pDst = '\0';
																												    
   // 返回目标字符串长度
    return nDstLength;
}

		
// UCS2编码
// pSrc: 源字符串指针
// pDst: 目标编码串指针
// nSrcLength: 源字符串长度
// 返回: 目标编码串长度
int gsmEncodeUcs2(const char* pSrc, unsigned char* pDst, int nSrcLength)
{
    int nDstLength;        // UNICODE宽字符数目
    char  unicode_buf[200];      // UNICODE串缓冲区
    int i;
	char *p=pDst;
	printf("gb2312: %s\n",pSrc);
	
    // 字符串-->UNICODE串
	g2u(pSrc,nSrcLength,unicode_buf,200);
	printf("nSrcLength: %d\n",nSrcLength);
	
	printf("unicode: %s\n",unicode_buf);	
	nDstLength=strlen(unicode_buf);
	    
	printf("dstlength: %d\n",nDstLength);
    // 高低字节对调，输出
    for(i=0; i<nDstLength-1; i+=2)
    {
	//	printf("$$$$$: %c\n",unicode_buf[i]);
		*pDst=unicode_buf[i+1];
		 pDst++;
		 *pDst=unicode_buf[i];
		 pDst++;
         unicode_buf[i] & 0xff;	 
    }
 //  printf("UD: %s\n",p); 
    // 返回目标编码串长度 i*/
    return nDstLength;
}

// UCS2解码
// pSrc: 源编码串指针
// pDst: 目标字符串指针
// nSrcLength: 源编码串长度
// 返回: 目标字符串长度
int gsmDecodeUcs2(const unsigned char* pSrc, char* pDst, int nSrcLength)
{
    int nDstLength;        // UNICODE宽字符数目
    char unicode_buf[200];      // UNICODE串缓冲区
    int i;
	
	printf("srclength:%d\n",nSrcLength);
	
    // 高低字节对调，拼成UNICODE
    for(i=0; i<nSrcLength/2+1;i+=2)
    {
        // 先高位字节
        unicode_buf[i+1] =*pSrc;
		pSrc++;
        // 后低位字节
        unicode_buf[i] =*pSrc;
		pSrc++;
    }
	
    //printf("Decode: %s\n",unicode_buf);
    // UNICODE串-->字符串
    u2g(unicode_buf,nSrcLength,pDst,nSrcLength);

	printf("decode gb2312: %s\n",pDst);
    // 输出字符串加个结束符    
    pDst[nDstLength] ='\0';    

    // 返回目标字符串长度
    return nDstLength;
}

/*代码转换:从一种编码转为另一种编码*/
  int code_convert(char *from_charset,char *to_charset,const char *inbuf,int inlen,char *outbuf,int outlen)
 {
     iconv_t cd;
     int rc;
     char **pin = (char **)&inbuf;
     char **pout = (char **)&outbuf;
     cd = iconv_open(to_charset,from_charset);
     if (cd==0) return -1;
     memset(outbuf,0,outlen);
     if (iconv(cd,pin,&inlen,pout,&outlen)==-1) return -1;
     iconv_close(cd);
     return 0;
 }
  /*UNICODE码转为GB2312码*/
 int u2g(const char *inbuf,int inlen,char *outbuf,int outlen)
 {
    return code_convert("UTF-16Le","gb2312",inbuf,inlen,outbuf,outlen);
 }
    /*GB2312码转为UNICODE码*/
 int g2u(const char *inbuf,size_t inlen,char *outbuf,size_t outlen)
 {
   return code_convert("gb2312","UTF-16Le",inbuf,inlen,outbuf,outlen);
 }
 
		
int gsmEncodePdu(const SM_PARAM* pSrc, char* pDst)
{
    int nLength;             // 内部用的串长度
	int nDstLength;          // 目标PDU串长度
    unsigned char buf[256];  // 内部用的缓冲区
	unsigned char UD[100];  
	int len;
	char my_buf[50];
	// SMSC地址信息段
    nLength = strlen(pSrc->SCA);    // SMSC地址字符串的长度   
//	nLength=strlen("开空调");
	//printf("nLength: %d\n",nLength);
//	len=gsmEncodeUcs2("开空调",buf,nLength);
//	gsmBytes2String(buf,UD,len);
//	printf("kongtiao ud: %s\n",UD);
    buf[0] = (char)((nLength & 1) == 0 ? nLength : nLength + 1) / 2 + 1;    // SMSC地址信息长度
    buf[1] = 0x91;        // 固定: 用国际格式号码
    nDstLength = gsmBytes2String(buf, pDst, 2);        // 转换2个字节到目标PDU串
	nDstLength += gsmInvertNumbers(pSrc->SCA, &pDst[nDstLength], nLength);    // 转换SMSC到目标PDU串
										    
   // TPDU段基本参数、目标地址等
    nLength = strlen(pSrc->TPA); 
	// TP-DA地址字符串的长度
    buf[0] = 0x11;            // 是发送短信(TP-MTI=01)，TP-VP用相对格式(TP-VPF=10)
    buf[1] = 0;               // TP-MR=0
	buf[2] = (char)nLength;   // 目标地址数字个数(TP-DA地址字符串真实长度)
    buf[3] = 0x91;            // 固定: 用国际格式号码
	nDstLength += gsmBytes2String(buf, &pDst[nDstLength], 4);  // 转换4个字节到目标PDU串
	nDstLength += gsmInvertNumbers(pSrc->TPA, &pDst[nDstLength], nLength); // 转换TP-DA到目标PDU串
																			    
	// TPDU段协议标识、编码方式、用户信息等
	   nLength = strlen(pSrc->TP_UD);    // 用户信息字符串的长度
	   buf[0] = pSrc->TP_PID;        // 协议标识(TP-PID)
	   buf[1] = pSrc->TP_DCS;        // 用户信息编码方式(TP-DCS)
	   buf[2] = 0;            // 有效期(TP-VP)为5分钟
	 if(pSrc->TP_DCS == GSM_UCS2)
	  {
	      // UCS2编码方式
		  buf[3] = gsmEncodeUcs2(pSrc->TP_UD, &buf[4], nLength);
		 //gsmEncodeUcs2(pSrc->TP_UD,&UD[0],nLength);
          //gsmBytes2String(&buf[4],UD,buf[3]);

		  //printf("UD:%s\n",UD);
		  // 转换TP-DA到目标PDU串
	      nLength = buf[3] + 4;        // nLength等于该段数据长度
	   }

    nDstLength += gsmBytes2String(buf, &pDst[nDstLength], nLength);        // 转换该段数据到目标PDU串
	
	// 返回目标字符串长度
	return nDstLength;
	
	}
	
	
// PDU解码，用于接收、阅读短消息
// pSrc: 源PDU串指针
// pDst: 目标PDU参数指针
// 返回: 用户信息串长度
int gsmDecodePdu(const char* pSrc, SM_PARAM* pDst)
{
	char *ptr;
	int i;
	char *convert_num=(char *)malloc(sizeof(char)*20);
	char *s_number=(char *)malloc(sizeof(char)*20);
	char buf[200];
	char b_buf[100];
	int nDstLength=0;
	for(i=0;i<14;i++)convert_num[i]='\0';
	strcpy(buf,pSrc);
	
	ptr=buf;
	ptr+=24;
	printf("token: %s\n",ptr);
	if(ptr!=NULL)
	{
		
		for(i=0;i<14;i++)
		{
			convert_num[i]=*ptr;
			ptr++;
		}
		printf("covert_num: %s\n",convert_num);
		
		gsmSerializeNumbers(convert_num,s_number,14);
		printf("s_number: %s\n",s_number);	
		
		strcpy(pDst->TPA,s_number);
		
		ptr+=20;

		printf("PDU unicode: %s\n",ptr);

		nDstLength=gsmString2Bytes(ptr,buf,strlen(ptr)*2);
		nDstLength = gsmDecodeUcs2(buf, pDst->TP_UD, nDstLength);    // 转换到TP-DU
	
	return 1;
	}




}
/*
int gsmDecodePdu(const char* pSrc, SM_PARAM* pDst)
{
   int nDstLength;          // 目标PDU串长度
   unsigned char tmp;       // 内部用的临时字节变量
   unsigned char buf[256];  // 内部用的缓冲区
   int len;
		  // SMSC地址信息段
   gsmString2Bytes(pSrc, &tmp, 2);    // 取长度
   tmp = (tmp - 1) * 2;    // SMSC号码串长度
   pSrc += 4;              // 指针后移
   gsmSerializeNumbers(pSrc, pDst->SCA, tmp);    // 转换SMSC号码到目标PDU串
   printf("SMSC number: %s\n",pDst->SCA);
   pSrc += tmp;        // 指针后移
										    
 // TPDU段基本参数、回复地址等
   gsmString2Bytes(pSrc, &tmp, 2);    // 取基本参数
   pSrc += 2;        // 指针后移
  if(tmp & 0x80)
  {
	 // 包含回复地址，取回复地址信息
     gsmString2Bytes(pSrc, &tmp, 2);    // 取长度
	  if(tmp & 1) tmp += 1;    // 调整奇偶性
		pSrc += 4;          // 指针后移
      gsmSerializeNumbers(pSrc, pDst->TPA, tmp);    // 取TP-RA号码
	  printf("TP_PA: %s\n",pDst->TPA);
	  pSrc += tmp;        // 指针后移
    }
																													    
   // TPDU段协议标识、编码方式、用户信息等
	gsmString2Bytes(pSrc, (unsigned char*)&pDst->TP_PID, 2);    // 取协议标识(TP-PID)
	printf("TP_PID: %s\n",pDst->TP_PID);
    pSrc += 2;        // 指针后移
    gsmString2Bytes(pSrc, (unsigned char*)&pDst->TP_DCS, 2);    // 取编码方式(TP-DCS)
	printf("TP_DCS: %s\n",pDst->TP_DCS);
    pSrc += 2;        // 指针后移
    gsmSerializeNumbers(pSrc, pDst->TP_SCTS, 14);        // 服务时间戳字符串(TP_SCTS)
	printf("TP_SCTS: %s\n",pDst->TP_SCTS);
    pSrc += 14;       // 指针后移
    gsmString2Bytes(pSrc, &tmp, 2);    // 用户信息长度(TP-UDL)
    pSrc += 2;        // 指针后移
	if(pDst->TP_DCS == GSM_UCS2)
	  {
	
		// UCS2解码
		printf("before to bytes: %s\n",pSrc);
		printf("tmp: %d\n",tmp);
		len=strlen(pSrc);
		printf("len:%d\n",len);
		nDstLength = gsmString2Bytes(pSrc, buf,tmp*2);        // 格式转换
		nDstLength = gsmDecodeUcs2(buf, pDst->TP_UD, nDstLength);    // 转换到TP-DU
	
		}
		// 返回目标字符串长度
	
	return nDstLength;
	
}
token
*/
void init_SM_PARAM(SM_PARAM *psrc,char *number)
{
	strcpy(psrc->SCA,"8613800411500");
	strcpy(psrc->TPA,number);
	psrc->TP_PID=0;
	psrc->TP_DCS=GSM_UCS2;
	strcpy(psrc->TP_UD,"家中可能起火");
}	
	
int gsmSendMessage(const SM_PARAM* pSrc)
{
    int nPduLength;        // PDU串长度
	unsigned char nSmscLength;    // SMSC串长度
	int nLength;           // 串口收到的数据长度
	char cmd[16];          // 命令串
    char pdu[512];         // PDU串
	char ans[128];         // 应答串
				
	printf("SM_PARM  before encode: %s\n",pSrc->TP_UD);	
	    
    nPduLength = gsmEncodePdu(pSrc, pdu);    // 根据PDU参数，编码PDU串
	strcat(pdu, "\x01a");        // 以Ctrl-Z结束
	
	printf("SM_PARAM after encod:%s\n",pdu);
			
						    
    gsmString2Bytes(pdu, &nSmscLength, 2);    // 取PDU串中的SMSC信息长度
    nSmscLength++;        // 加上长度字节本身
												    
   // 命令中的长度，不包括SMSC信息长度，以数据字节计
   tty_writecmd("AT+CMGF=0",strlen("AT+CMGF=0"));
 	sprintf(cmd, "AT+CMGS=%d\r", nPduLength / 2 - nSmscLength);    // 生成命令
	

	tty_write(cmd, strlen(cmd));    // 先输出命令串	
	
	tty_write(pdu,strlen(pdu));

	  return 1;
  }
									
