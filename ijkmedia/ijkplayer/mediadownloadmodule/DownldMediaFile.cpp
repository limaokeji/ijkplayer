#ifndef _ANDROID
#include "stdafx.h"
#endif
#include "DownldMediaFile.h"
#include "DataType.h"
#include <string.h>
#include "mediafile_download_log.h"
#include "../android/limao_api_jni.h"
DownldMediaFile::DownldMediaFile()
{
	_mediaFileHash = new char[FILENAMELEN];
	_playMediaFilePath = new char[FILENAMELEN];
	memset(_mediaFileHash, 0 , FILENAMELEN);
	memset(_playMediaFilePath, 0 , FILENAMELEN);
	_buffer = new char[BUFFERSIZE];
	_medieFileSize = 0;
	pMediaFileDownldLog = NULL;
	_downloadBlockInfoList = NULL;
	_quit =0;
}
DownldMediaFile::~DownldMediaFile()
{

	printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
			_mediaFileHash,
			"~DownldMediaFile()",
						   	pMediaFileDownldLog);
	if(_mediaFileHash != NULL)
	{
		delete[] _mediaFileHash;
		_mediaFileHash =NULL;
	}
	if(_playMediaFilePath != NULL)
	{
		delete[] _playMediaFilePath;
		_playMediaFilePath = NULL;
	}
	if(_pPlayerMediaFile != NULL)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
				"~DownldMediaFile()",
				"fclose open media file",
							   	pMediaFileDownldLog);
		fclose(_pPlayerMediaFile);
		_pPlayerMediaFile = NULL;
	}
	if (_buffer != NULL)
	{
		delete[] _buffer;
		_buffer = NULL;
	}
}
bool DownldMediaFile::Init(char * fileNamehash,char * suffix_name, char * playMediaFilePath,uint64_t filesize,
		 FILE* plog_file)
{
	CHECK_POINTER_RET(fileNamehash, false);
	strcpy(_mediaFileHash, fileNamehash);
	printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
			_mediaFileHash,
			"DownldMediaFile::Init",
						   	pMediaFileDownldLog);

	_medieFileSize = filesize;
	pMediaFileDownldLog = plog_file;
	return true;
}
int DownldMediaFile::P2pDownloadMediaData(long offset, int64_t blockSize)
{
	if(_mediaFileHash == NULL)
	{
		return -1;
	}
	char logbuf[200] ={0};
	sprintf(logbuf,"offset %ld,  block size %llu",offset,blockSize);
	printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
						   "DownldMediaFile::P2pDownloadMediaData ",
						   logbuf,
						   	pMediaFileDownldLog);
	int ret =  LimaoApi_downloadExt(_mediaFileHash,offset,blockSize,&_quit, 1000);
	if(ret != 0)
	{
		sprintf(logbuf,"offset %ld,  block size %llu  error %d",offset,blockSize,ret);
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
							   "p2p download file data",
							   logbuf,
							   	pMediaFileDownldLog);
		return ret;
	}
	printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
						   "DownldMediaFile::P2pDownloadMediaData ",
						   "P2pDownloadMediaData complete",
						   	pMediaFileDownldLog);
	return ret;
}


char * DownldMediaFile::GetP2pDownloadFileNameHash()
{
	return  _mediaFileHash;
}
char * DownldMediaFile::GetPlayFileNamePath()
{
	return _playMediaFilePath;
}
DOWNLOADBLOCKINFO * DownldMediaFile::GetDownloadBlockInfoList()
{
	return _downloadBlockInfoList;
}

bool DownldMediaFile::IsDownloadMdatBlock(int index)
{
	if(_downloadBlockInfoList == NULL)
	{
		return false;
	}
	if((index <0) ||(index >= _syncSampleCount))
	{
		return false;
	}
	return (_downloadBlockInfoList+index)->isDownload;
}
FILE * DownldMediaFile::GetLogFile()
{
	return pMediaFileDownldLog;
}
