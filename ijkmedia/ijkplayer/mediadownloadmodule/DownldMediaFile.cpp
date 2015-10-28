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
}
DownldMediaFile::~DownldMediaFile()
{
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
	int ret =  LimaoApi_downloadExt(_mediaFileHash,offset,blockSize,1000);
	if(ret != 0)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
							   "p2p download file data",
							   "LimaoApi_downloadExt failed.",
							   	pMediaFileDownldLog);
	}
	return ret;
}
/*bool DownldMediaFile::CopyFileData(long offset, int64_t blockSize)
{
	CHECK_POINTER_RET(_pOutFile, false);
	CHECK_POINTER_RET(_pInFile, false);
	if (0 != fseek(_pInFile, offset, SEEK_SET))
	{
		printf("Download file seek input file failed\n");
		return false;
	}
	if (0 != fseek(_pOutFile, offset, SEEK_SET))
	{
		printf("Download file seek output file failed\n");
		return false;
	}


	int count = blockSize / BUFFERSIZE + 1;

	long lastCountSize = blockSize % BUFFERSIZE;

	for (int i = 0; i < count; i++)
	{
		memset(_buffer, 0, BUFFERSIZE);
		if (i == count - 1)
		{
			if (lastCountSize != fread(_buffer, 1, lastCountSize, _pInFile))
			{
				printf("Download  file read input file failed\n");
				return false;
			}
			if (lastCountSize != fwrite(_buffer, 1, lastCountSize, _pOutFile))
			{
				printf("Download  file write input file failed\n");
				return false;
			}
			break;
		}

		if (BUFFERSIZE != fread(_buffer, 1, BUFFERSIZE, _pInFile))
		{
			printf("Download  file read input file failed\n");
			return false;
		}
		if (BUFFERSIZE != fwrite(_buffer, 1, BUFFERSIZE, _pOutFile))
		{
			printf("Download  file write input file failed\n");
			return false;
		}
	}
	return true;


}*/

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
