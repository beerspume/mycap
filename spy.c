#include "spy.h"
FILE *SPY_LOG_FILE;
void spy_printf(const char *fmt, ...){
	va_list ap;

	va_start(ap, fmt);

	if(SPY_LOG_FILE==NULL){
		SPY_LOG_FILE=fopen("log_spy.txt","a");
	}
	if(SPY_LOG_FILE!=NULL){
		vfprintf(SPY_LOG_FILE,fmt,ap);
		fflush(SPY_LOG_FILE);
	}
	va_end(ap);
}

static char spy_FormatedTimeResult[20];
char* spy_S2T(time_t second){
	struct tm* tm_time=localtime(&second);
	sprintf(spy_FormatedTimeResult,"%4d-%02d-%02d %02d:%02d:%02d",
		tm_time->tm_year+1900
		,tm_time->tm_mon+1
		,tm_time->tm_mday
		,tm_time->tm_hour
		,tm_time->tm_min
		,tm_time->tm_sec
	);
	return spy_FormatedTimeResult;
}
