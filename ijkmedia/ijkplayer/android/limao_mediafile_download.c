/*
 * limao_mediafile_download.c
 */

#include "limao_api_jni.h"
#include "limao_msg_loop.h"
#include "limao_mediafile_download.h"

#include "../mediadownloadmodule/mediafile_downld_module.h"
#include "../mediadownloadmodule/mediafile_download_log.h"

#define CHECK(ret)  if(ret!= 0){	printf_log(LOG_ERROR, \
				   "ijkplayer media file download medule thread ", \
				   "down the block.", \
				   	NULL); \
					return; \
					}



int timestamp_2_blockIndex(uint64_t timestamp)
{
	int download_block_count = mediafile_downld_module_getmediadatalock_count(NULL);
	DOWNLOADBLOCKINFO * download_blockinfo_list = mediafile_downld_module_getblocklistinfo(NULL);
	char logBuf[200] = { 0 };
	for (int i = 0; i < download_block_count - 1; i++)
	{
		if (( timestamp >= download_blockinfo_list[i].timeStamp) && (timestamp < download_blockinfo_list[i+1].timeStamp ))
		{
			sprintf(logBuf,"lmk timestamp_2_blockIndex index %d",i);
			printf_log(LOG_WARN,
						   "ijkplayer seek ",
						   logBuf,
						   	NULL);
			return i;
		}
	}
	printf_log(LOG_ERROR,
				   "ijkplayer seek ",
				   "lmk timestamp_2_blockIndex index -1",
				   	NULL);
	return -1;
}

int isBlockDownload(uint64_t timestamp)
{
	int download_block_count = mediafile_downld_module_getmediadatalock_count(NULL);
	DOWNLOADBLOCKINFO * download_blockinfo_list = mediafile_downld_module_getblocklistinfo(NULL);
	if(timestamp < download_blockinfo_list[2].timeStamp)
	{
		return  1;
	}

	for (int i = 0; i < download_block_count - 1; i++)
	{
		if(i >= download_block_count -5)
		{
			return 1;
		}
		if (( timestamp >= download_blockinfo_list[i].timeStamp) && (timestamp < download_blockinfo_list[i+1].timeStamp ))
		{
			char buf[200] = {0};
			sprintf(buf,"isBlockDownload timestamp %llu, index is %d isDownload %d",timestamp, i, download_blockinfo_list[i].isDownload ? 1 : 0);
			if(!download_blockinfo_list[i].isDownload)
				printf_log(LOG_WARN,
						   "ijkplayer isBlockDownload ",
						   buf,
						   	NULL);

			return download_blockinfo_list[i].isDownload && download_blockinfo_list[i+1].isDownload && download_blockinfo_list[i+2].isDownload;
		}
	}
	return 0;
}
