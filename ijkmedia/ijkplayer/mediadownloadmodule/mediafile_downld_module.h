#ifndef MEDIAFILE_DOWNLD_MODULE_H_
#define MEDIAFILE_DOWNLD_MODULE_H_

#include "DataType.h"
#include <stdio.h>

//static  DOWNLOADBLOCKINFO * g_downloadBlockInfoList = NULL;
//static  FILE * g_downloadModuleLogFile = NULL;

#ifndef __cplusplus
#define bool unsigned char
#endif

#define MP4		1001
#define MOV		1002
#define M4V		1003
#define GP3		1004
#define AVI		2001
#define RMVB	3001
#define RM		3002
#define WMV     4001
#define FLV		5001
#define MKV		6001
#define MPG		7001
#define MPE		7002
#define MPEG	7003
#define ASF		8001
#define ASX		9001
#define DAT		10001
#define VOB		20001


#ifdef __cplusplus
extern "C" {
#endif

/**
 * init
 * @param mediafile_hash  the media file hash for p2p downlaod
 * @param suffix_name     the media file suffix name for player
 * @param filesize   the media file size
 */
void * mediafile_downld_module_init(char * mediafile_hash,char * suffix_name, char * play_mediefile_path, uint64_t filesize,
									DOWNLOADBLOCKINFO ** pdownload_blockinfo_list, FILE* plog_file);

/**
 * finish
 */
bool mediafile_downld_module_finish(void * g_downld_mediafile);

/**
 * get the mediafile root box offset and size for download
 */
bool mediafile_downld_module_getrootbox_offset(void * g_downld_mediafile);

/**
 * get the mediafile root box but media data box. then the player info is ok
 */
bool mediafile_downld_module_download_playerinfobox(void * g_downld_mediafile);


/**
 * download mediadata block by index
 */
bool mediafile_downld_module_download_mediadatablock(void * g_downld_mediafile,int index);

/**
 * get download mediadata block count
 */
int mediafile_downld_module_getmediadatalock_count(void * g_downld_mediafile);


DOWNLOADBLOCKINFO * mediafile_downld_module_getblocklistinfo(void * g_downld_mediafile);
#ifdef __cplusplus
}
#endif

#endif
