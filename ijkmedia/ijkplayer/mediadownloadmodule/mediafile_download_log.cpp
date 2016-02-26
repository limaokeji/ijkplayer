#include "mediafile_download_log.h"
#define PRINTFLOG    1
/*
#define   LOG_ALL		127   //yes all
#define   LOG_INFO		2   // yes
#define   LOG_WARN		4	// yes
#define   LOG_ERROR     8	// yes
#define   LOG_FILE		16
*/
int printf_log(int flags ,const char * location,const char * format,FILE * log_file){
	if((location == NULL) ||(format== NULL))
	{
#ifdef PRINTFLOG
		 __android_log_print(ANDROID_LOG_ERROR,"lmk media file download", "printf_log arg is invalid");
#endif
		 return -1;
	}
	if(flags & LOG_INFO )
	{
#ifdef PRINTFLOG
		 __android_log_print(ANDROID_LOG_INFO,"lmk media file download", "lmk %s   |   %s ",location,format);
#endif
		 if((flags & LOG_FILE) && (log_file != 0))
		{
			fprintf(log_file,"------> INFO: %s , %s\n",location,format);
		}
	}
	if(flags & LOG_WARN)
	{
#ifdef PRINTFLOG
		__android_log_print(ANDROID_LOG_WARN,"lmk media file download", "lmk %s   |   %s ",location,format);
#endif
		if((flags & LOG_FILE) && (log_file != 0))
		{
			fprintf(log_file,"????????  WARN: %s , %s\n",location,format);
		}
	}
	if(flags & LOG_ERROR)
	{
#ifdef PRINTFLOG
		__android_log_print(ANDROID_LOG_ERROR,"lmk media file download", "lmk %s   |   %s ",location,format);
#endif
		if((flags & LOG_FILE) && (log_file != 0))
		{
			fprintf(log_file,"!!!!!!!!!  ERROR :%s , %s\n",location,format);
		}
	}

	fflush(log_file);
	return 0;
}
