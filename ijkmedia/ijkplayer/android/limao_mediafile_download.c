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

void media_file_download_module_thread(void *argv)
{
	if(argv == NULL)
	{
		printf_log(LOG_ERROR,
				   "ijkplayer media file download medule thread",
				   "thread argv is invalid.\n",
				   	NULL);
		return;
	}
	ThreadLocalData_t * arg = argv;
	char * mediafile_hash = arg->fileHash;
	char * suffix_name = arg->filenameExtension;
	printf_log(LOG_INFO,
					   "ijkplayer media file download medule thread ",
					   mediafile_hash,
					   	NULL);
	/*int64_t offset = 0;
	int64_t size = 16;
	int ret=  LimaoApi_downloadExt(mediafile_hash, 1, 16, 1000);CHECK(ret);
	offset = 18;	size = 16;
	ret=LimaoApi_downloadExt(mediafile_hash, 0, size, 1000);CHECK(ret);
	offset = 132;	size = 16;
	ret=LimaoApi_downloadExt(mediafile_hash, offset, size, 1000);CHECK(ret);
	offset = 244;	size = 16;
	ret=LimaoApi_downloadExt(mediafile_hash, offset, size, 1000);CHECK(ret);
	offset = 406;	size = 16;
	ret=LimaoApi_downloadExt(mediafile_hash, offset, size, 1000);CHECK(ret);
	offset = 839;	size = 16;
	ret=LimaoApi_downloadExt(mediafile_hash, offset, size, 1000);CHECK(ret);
	offset = 208896090;	size = 16;
	ret=LimaoApi_downloadExt(mediafile_hash, offset, size, 1000);CHECK(ret);
	offset = 208899904;	size = 16;
	ret=LimaoApi_downloadExt(mediafile_hash, offset, size, 1000);CHECK(ret);
	offset = 209116546;	size = 16;
	ret=LimaoApi_downloadExt(mediafile_hash, offset, size, 1000);CHECK(ret);
	offset = 0;	size = 18;
	ret=LimaoApi_downloadExt(mediafile_hash, offset, size, 1000);CHECK(ret);
	offset = 68;	size = 64;
	ret=LimaoApi_downloadExt(mediafile_hash, offset, size, 1000);CHECK(ret);
	offset = 132;	size = 112;
	ret=LimaoApi_downloadExt(mediafile_hash, offset, size, 1000);CHECK(ret);
	offset = 244;	size = 162;
	ret=LimaoApi_downloadExt(mediafile_hash, offset, size, 1000);CHECK(ret);
	offset = 406;	size = 433;
	ret=LimaoApi_downloadExt(mediafile_hash, offset, size, 1000);CHECK(ret);
	offset = 893;	size = 160;
	ret=LimaoApi_downloadExt(mediafile_hash, offset, size, 1000);CHECK(ret);
	offset = 208896090;	size = 3814;
	ret=LimaoApi_downloadExt(mediafile_hash, offset, size, 1000);CHECK(ret);
	offset = 208899904;	size = 216642;
	ret=LimaoApi_downloadExt(mediafile_hash, offset, size, 1000);CHECK(ret);
	offset = 209116546;	size = 20;
	ret=LimaoApi_downloadExt(mediafile_hash, offset, size, 1000);CHECK(ret);

	long len = LimaoApi_getFileSize(mediafile_hash);

	for(long i = 1000 ; i<len ; i=i+20000)
	{
		offset = i;	size = 20000;
		ret = LimaoApi_downloadExt(mediafile_hash, offset, size, 1000);
		CHECK(ret);
		printf_log(LOG_INFO,
					   "ijkplayer media file download medule thread ",
					   "down the block.",
					   	NULL);
	}

	printf_log(LOG_INFO,
				   "ijkplayer media file download medule thread ",
				   "thread is close.",
				   	NULL);
	return;*/
	if((mediafile_hash == NULL)||(suffix_name == NULL))
	{
		printf_log(LOG_ERROR,
				   "ijkplayer media file download medule thread : hash name",
				   "thread argv is invalid.\n",
				   	NULL);
		return;
	}
	printf_log(LOG_INFO,
			   "ijkplayer media file download medule thread hash name:",
			   mediafile_hash,
			   	NULL);
	printf_log(LOG_INFO,
				   "ijkplayer media file download medule thread suffix name : ",
				   suffix_name,
				   	NULL);

	if(!mediafile_downld_module_init(mediafile_hash, suffix_name, NULL, 0,
			&g_pdownload_blockinfo_list, NULL))
	{
		printf_log(LOG_ERROR,
			   "ijkplayer media file download medule thread",
			   "mediafile download module init failed.\n",
			   	NULL);
		return;

	}
	printf_log(LOG_INFO,
				   "ijkplayer media file download medule thread ",
				   "mediafile_downld_module_init ok",
				   	NULL);
	if(!mediafile_downld_module_getrootbox_offset())
	{

		printf_log(LOG_ERROR,
			   "ijkplayer media file download medule thread",
			   "mediafile download module get root box offset failed.\n",
			   	NULL);
		goto thread_failed;
	}
	printf_log(LOG_INFO,
				   "ijkplayer media file download medule thread",
				   "mediafile_downld_module_getrootbox_offset ok",
				   	NULL);
	if(!mediafile_downld_module_download_playerinfobox())
	{
		printf_log(LOG_ERROR,
			   "ijkplayer media file download medule thread",
			   "mediafile download module get play info box failed.\n",
			   	NULL);
		goto thread_failed;
	}
	printf_log(LOG_INFO,
				   "ijkplayer media file download medule thread ",
				   "mediafile_downld_module_download_playerinfobox ok",
				   	NULL);
	g_media_download_blockCount =  mediafile_downld_module_getmediadatalock_count();

	char log_buf[200] = {0};

	sprintf(log_buf,"block count is %d.",g_media_download_blockCount);
	printf_log(LOG_INFO,
				   "ijkplayer media file download medule thread ",
				   log_buf,
				   	NULL);
	if(g_media_download_blockCount == 0)
	{
		printf_log(LOG_ERROR,
			   "ijkplayer media file download medule thread",
			   "mediafile download module get ker frame count failed.\n",
			   	NULL);
		goto thread_failed;
	}

	for(int i = 0; i< g_media_download_blockCount; i++)
	{
		if(!mediafile_downld_module_download_mediadatablock(i))
		{
			printf_log(LOG_ERROR,
				   "ijkplayer media file download medule thread",
				   "mediafile download block failed.\n",
				   	NULL);
			goto thread_failed;
		}
		sprintf(log_buf,"downlaod the %d block success.",i);
		printf_log(LOG_INFO,
					   "ijkplayer media file download medule thread ",
					   log_buf,
					   	NULL);
	}

thread_failed:
	mediafile_downld_module_finish();
	printf_log(LOG_INFO,
				   "ijkplayer media file download medule thread ",
				   "thread is close.",
				   	NULL);
	return;

}

int timestamp_2_blockIndex(uint64_t timestamp)
{
	int download_block_count = mediafile_downld_module_getmediadatalock_count();
	DOWNLOADBLOCKINFO * download_blockinfo_list = mediafile_downld_module_getblocklistinfo();
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
	int download_block_count = mediafile_downld_module_getmediadatalock_count();
	DOWNLOADBLOCKINFO * download_blockinfo_list = mediafile_downld_module_getblocklistinfo();
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
