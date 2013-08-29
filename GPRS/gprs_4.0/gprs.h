
#ifndef __GPRS_H__
#define __GPRS_H__
#define GSM_7BIT 0
#define GSM_8BIT 4
#define GSM_UCS2 8
typedef struct{
	char SCA[16];
	char TPA[16];
	char TP_PID;
	char TP_DCS;
	char TP_SCTS[16];
	char TP_UD[161];
	char index;
}SM_PARAM;


extern char * cmd[];

void init_SM_PARAM(SM_PARAM *psrc,char *number);
void gprs_init();
void gprs_msg(char *number, int num);
void gprs_call(char *number, int num);
void gprs_hold();
void gprs_ans();
void gprs_call(char *number, int num);
void gprs_baud(char *baud,int num);
int gsmString2Bytes(const char *psrc,unsigned char *pdst,int);
int gsmBytes2String(const unsigned char*psrc,char *pdst,int nsrclength);
int gsmInvertNumbers(const char *psrc,char *pDst,int nsrclength);
int gsmSerializeNumbers(const char *psrc,char *pdst,int nsrclength);
int gsmEncodeUcs2(const char* pSrc, unsigned char* pDst, int nSrcLength);
int gsmDecodeUcs2(const unsigned char* pSrc, char* pDst, int nSrcLength);
int gsmEncodePdu(const SM_PARAM *psrc,char * pdst);
int gsmDecodePdu(const char *psrc, SM_PARAM * pdst);
int gsmSendMessage(const SM_PARAM * psrc);
int code_convert(char *from_charset,char *to_charset,const char *inbuf,int inlen,char *outbuf,int outlen);
int u2g(const char *inbuf,int inlen,char *outbuf,int outlen);
int g2u(const char *inbuf,size_t inlen,char *outbuf,size_t outlen);


#endif

