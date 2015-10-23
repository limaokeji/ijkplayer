#ifndef DATATYPE_H_
#define DATATYPE_H_
#include <stdint.h>
#define NULL 0
#ifndef __cplusplus
#define bool unsigned char
#endif
typedef struct DownloadBlockInfo
{
	unsigned int smapleId;
	unsigned long offset;
	uint64_t timeStamp;
	bool isDownload;
}DOWNLOADBLOCKINFO;

#define INDEXRECORD DOWNLOADBLOCKINFO
#define FOURCC int
#define DWORD unsigned long
#define LONG long
#define CHECK_POINTER_RET(arg,ret) if(arg == NULL ){ printf("Parameter is invalid!\n"); return ret; }
#define CHECK_POINTER(arg) if(arg == NULL ) { printf("Parameter is invalid!\n"); return; }
#define CHECK_INT_RET(arg,ret) if(arg <= 0) { printf("Parameter is invalid!\n"); return ret; }
#define CHECK_INT(arg) if(arg <= 0) { printf("Parameter is invalid!\n"); return; }
#endif
