#include "mediafile_downld_module.h"
#include "DataType.h"
#include "mediafile_download_log.h"
#include "DownldMediaFile.h"
#include "./mp4parse/Mp4DownldMediaFile.h"
#include "mkvparse/MkvDownldMediaFile.h"
#include "rmvbparse/RmvbDownldMediaFile.h"
#include <string.h>
#define stricmp strcasecmp
DownldMediaFile * g_downld_mediafile = NULL;
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
				//g_downld_mediafile = new MkvDownldMediaFile();
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

	int media_type = check_media_type(suffix_name);
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
	if(g_downld_mediafile->Init(mediafile_hash, suffix_name, play_mediefile_path, filesize,plog_file))
	{
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
