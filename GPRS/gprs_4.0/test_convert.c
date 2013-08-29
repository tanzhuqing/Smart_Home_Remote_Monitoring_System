#include<string.h>
#include"gprs.h"
																													
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


int main(){
char str[]="683199182806F0";
char *pdst=(char *)malloc(sizeof(char)*20);

gsmSerializeNumbers(str,pdst,14);
printf("pdst: %s\n",pdst);




return 0;
}
