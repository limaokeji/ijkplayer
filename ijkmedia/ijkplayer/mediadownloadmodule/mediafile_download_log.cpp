#include "mediafile_download_log.h"
/*
#define   LOG_ALL		127   //yes all
#define   LOG_INFO		2   // yes
#define   LOG_WARN		4	// yes
#define   LOG_ERROR     8	// yes
#define   LOG_FILE		16
*/
int printf_log(int flags ,const char * location,const char * format,FILE * log_file){
	if(flags & LOG_INFO )
	{
		 __android_log_print(ANDROID_LOG_INFO,"lmk media file download", "lmk %s   |   %s ",location,format);
		if((flags & LOG_FILE) && (log_file != 0))
		{
			fprintf(log_file,"------> INFO: %s , %s\n",location,format);
		}
	}
	if(flags & LOG_WARN)
	{
		__android_log_print(ANDROID_LOG_WARN,"lmk media file download", "lmk %s   |   %s ",location,format);
		if((flags & LOG_FILE) && (log_file != 0))
		{
			fprintf(log_file,"????????  WARN: %s , %s\n",location,format);
		}
	}
	if(flags & LOG_ERROR)
	{
		__android_log_print(ANDROID_LOG_ERROR,"lmk media file download", "lmk %s   |   %s ",location,format);
		if((flags & LOG_FILE) && (log_file != 0))
		{
			fprintf(log_file,"!!!!!!!!!  ERROR :%s , %s\n",location,format);
		}
	}


	return 0;
}
