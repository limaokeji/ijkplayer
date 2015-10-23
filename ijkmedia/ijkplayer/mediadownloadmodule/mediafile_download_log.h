#include <android/log.h>
#include <stdio.h>
#define   LOG_ALL		127   //yes all
#define   LOG_INFO		2   // yes
#define   LOG_WARN		4	// yes
#define   LOG_ERROR     8	// yes

#define   LOG_FILE		16
#ifdef __cplusplus
extern "C" {
#endif
int printf_log(int flags ,const char * location,const char * format, FILE * log_file);  //
#ifdef __cplusplus
}
#endif
