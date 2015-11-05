#include "mediafile_downld_module.h"
#include "DataType.h"
#include "mediafile_download_log.h"
#include "DownldMediaFile.h"
#include "./mp4parse/Mp4DownldMediaFile.h"
#include "mkvparse/MkvDownldMediaFile.h"
#include "rmvbparse/RmvbDownldMediaFile.h"
#include "../android/limao_api_jni.h"
#include <string.h>
#define stricmp strcasecmp
DownldMediaFile * g_downld_mediafile = NULL;
FILE * g_mediafile_parse_log = NULL;
int check_media_type(char * suffix_name)
{
	char * suffixName = suffix_name;
	if(suffix_name == NULL)
	{
		return -1;
	}
	if (stricmp(suffixName, "mp4") == 0)
	{
		return MP4;
	}
	else if (stricmp(suffixName, "mov") == 0)
	{
		return MOV;
	}
	else if (stricmp(suffixName, "m4v") == 0)
	{
		return M4V;
	}
	else if (stricmp(suffixName, "3gp") == 0)
	{
		return GP3;
	}
	else if (stricmp(suffixName, "avi") == 0)
	{
		return AVI;
	}
	else if (stricmp(suffixName, "rmvb") == 0)
	{
		return RMVB;
	}
	else if (stricmp(suffixName, "rm") == 0)
	{
		return RM;
	}
	else if (stricmp(suffixName, "wmv") == 0)
	{
		return WMV;
	}
	else if (stricmp(suffixName, "FLV") == 0)
	{
		return FLV;
	}
	else if (stricmp(suffixName, "MKV") == 0)
	{
		return MKV;
	}
	else if (stricmp(suffixName, "MPG") == 0)
	{
		return MPG;
	}
	else if (stricmp(suffixName, "MPE") == 0)
	{
		return MPE;
	}
	else if (stricmp(suffixName, "MPEG") == 0)
	{
		return MPEG;
	}
	else if (stricmp(suffixName, "ASF") == 0)
	{
		return ASF;
	}
	else if (stricmp(suffixName, "ASX") == 0)
	{
		return ASX;
	}
	else if (stricmp(suffixName, "DAT") == 0)
	{
		return DAT;
	}
	else if (stricmp(suffixName, "VOB") == 0)
	{
		return VOB;
	}

	return -1;
}

DownldMediaFile * get_downld_mediafile(int file_type)
{
	switch (file_type)
		{
			case MP4:
			case MOV:
			case GP3:
			case M4V:
				g_downld_mediafile = new Mp4DownldMediaFile();
				break;
			case FLV:
				g_downld_mediafile = NULL;
				break;
			case AVI:
				g_downld_mediafile = NULL;
				break;
			case WMV:
				g_downld_mediafile = NULL;
				break;
			case MKV:
				g_downld_mediafile = new MkvDownldMediaFile();
				break;
			case MPG:
				g_downld_mediafile = NULL;
				break;
			case RMVB:
			case RM:
				g_downld_mediafile = new RmvbDownldMediaFile();
				break;
			default:
				g_downld_mediafile = NULL;
		}

	return g_downld_mediafile;
}

int check_media_type_for_file_data(char * hash_name);

/**
 * init
 * @param mediafile_hash  the media file hash for p2p downlaod
 * @param suffix_name     the media file suffix name for player
 * @param filesize   the media file size
 */
bool mediafile_downld_module_init(char * mediafile_hash,char * suffix_name, char * play_mediefile_path, uint64_t filesize,
									DOWNLOADBLOCKINFO ** pdownload_blockinfo_list, FILE* plog_file)
{
	if(mediafile_hash == NULL)
	{
		printf_log(plog_file == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mediafile downld module init",
				   "the arg mediafile hash string is invalid.\n",
				   plog_file);
	}
	if(suffix_name == NULL)
	{
		printf_log(plog_file == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mediafile downld module init",
				   "the arg suffix name is invalid.\n",
				   plog_file);
	}

	int tmp_media_type = check_media_type_for_file_data(mediafile_hash);
	int media_type = -1;
	if(tmp_media_type != -1)
	{
		media_type = tmp_media_type;
	}else
	{
		media_type = check_media_type(suffix_name);
	}

	if(media_type == -1)
	{
		printf_log(plog_file == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mediafile downld module init",
				   "the suffix_name type is Not identified.\n",
				   plog_file);
	}
	if(NULL == get_downld_mediafile(media_type))
	{
		printf_log(plog_file == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mediafile downld module init",
				   "the media file  type is Not Support.\n",
				   plog_file);
	}

	g_mediafile_parse_log = fopen("/sdcard/limao/mediafileparse.log","w+");
	if(!g_downld_mediafile->Init(mediafile_hash, suffix_name, play_mediefile_path, filesize,g_mediafile_parse_log))
	{
		printf_log(plog_file == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mediafile downld module init",
				   "g_downld_mediafile init failed.\n",
				   plog_file);
		return false;
	}

	return true;
}

/**
 * finish
 */
bool mediafile_downld_module_finish()
{
	if(g_downld_mediafile != NULL)
	{
		delete g_downld_mediafile;
		g_downld_mediafile = NULL;
	}
	if(g_mediafile_parse_log!= NULL)
	{
		fclose(g_mediafile_parse_log);
		g_mediafile_parse_log = NULL;
	}
}

/**
 * get the mediafile root box offset and size for download
 */
bool mediafile_downld_module_getrootbox_offset()
{
	if(g_downld_mediafile == NULL)
		return false;
	return g_downld_mediafile->PraseRootBox();
}

/**
 * get the mediafile root box but media data box. then the player info is ok
 */
bool mediafile_downld_module_download_playerinfobox()
{
	if(g_downld_mediafile == NULL)
		return false;
	return g_downld_mediafile->DownloadFileFirst();
}


/**
 * download mediadata block by index
 */
bool mediafile_downld_module_download_mediadatablock(int index)
{
	if(g_downld_mediafile == NULL)
		return false;
	return g_downld_mediafile->DownloadMdatBlock(index);
}

/**
 * get download mediadata block count
 */
int mediafile_downld_module_getmediadatalock_count()
{
	if(g_downld_mediafile == NULL)
		return 0;
	return g_downld_mediafile->GetMdataBlockCount();
}

DOWNLOADBLOCKINFO * mediafile_downld_module_getblocklistinfo()
{
	return g_downld_mediafile->GetDownloadBlockInfoList();
}

int check_media_type_for_file_data(char * hash_name)
{
	if(hash_name == NULL)
	{
		printf_log(LOG_ERROR,
				   "mediafile downld module init",
				   "hash name is invalid.",
				   NULL);
		return -1;
	}

	printf_log(LOG_INFO,
			"mediafile downld module init",
			"check_media_type_for_file_data p2p download  100*1024 data\n",
			NULL);

	int ret =  LimaoApi_downloadExt(hash_name,0,100*1024,1000);
	if(ret != 0)
	{
		printf_log(LOG_ERROR,
				   "mediafile downld module init",
				   "check_media_type_for_file_data p2p download  100*1024 data failed .\n",
				   NULL);
		return -1;
	}
	printf_log(LOG_INFO,
			"mediafile downld module init",
			"check_media_type_for_file_data p2p download  100*1024 data complete\n",
			NULL);

	char  local_file_name[256] = {0};
	LimaoApi_getFilePath(hash_name, local_file_name);
	if(local_file_name[0] == 0)
	{
		printf_log(LOG_ERROR,
				   "mediafile downld module init",
				   "check_media_type_for_file_data get file path failed .\n",
				   NULL);
		return -1;
	}

	FILE * pFile = fopen(local_file_name,"rb");
	if(pFile == NULL)
	{
		printf_log(LOG_ERROR,
				   "mediafile downld module init",
				   "check_media_type_for_file_data open file failed .\n",
				   NULL);
		return -1;
	}
	unsigned char readBuf[100] = {0};

	ret = fread(readBuf,1,100,pFile);
	if(ret != 100)
	{
		printf_log(LOG_ERROR,
				   "mediafile downld module init",
				   "check_media_type_for_file_data fread file failed .\n",
				   NULL);
		fclose(pFile);
		pFile = NULL;
		return -1;
	}

	if((readBuf[4] == 0x66) && (readBuf[5] == 0x74) && (readBuf[6] == 0x79) && (readBuf[7] == 0x70))
	{

		printf_log(LOG_INFO,
				   "mediafile downld module init",
				   "check_media_type_for_file_data  check MP4\n",
				   NULL);
		fclose(pFile);
		pFile = NULL;
		return MP4;
	}else if((readBuf[0] == 0x2E) && (readBuf[1] == 0x52) && (readBuf[2] == 0x4D) && (readBuf[3] == 0x46)) // 2E 52 4D 46
	{
		printf_log(LOG_INFO,
				   "mediafile downld module init",
				   "check_media_type_for_file_data  check RMVB\n",
				   NULL);
		fclose(pFile);
		pFile = NULL;
		return RMVB;
	}else if((readBuf[0] == 0x1A) && (readBuf[1] == 0x45) && (readBuf[2] == 0xDF) && (readBuf[3] == 0xA3)){  //00000000h: 1A 45 DF A3                                     ; .E撸
		printf_log(LOG_INFO,
				   "mediafile downld module init",
				   "check_media_type_for_file_data  check MKV\n",
				   NULL);
		fclose(pFile);
		pFile = NULL;
		return MKV;
	}
	printf_log(LOG_INFO,
			   "mediafile downld module init",
			   "check_media_type_for_file_data  not check file type\n",
			   NULL);
	return -1;
}
