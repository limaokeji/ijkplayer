#ifdef _WIN32
#include "stdafx.h"
#endif
#include "RmvbDownldMediaFile.h"
#include <stdlib.h>
#include "../DataType.h"
#include "../mediafile_download_log.h"
#include "../../android/limao_api_jni.h"
RmvbDownldMediaFile::RmvbDownldMediaFile()
{
	_propBlockSize = 0;

	_contBlockOffset = 0;
	_contBlockSize = 0;

	memset(_mdprBlockOffset, 0, sizeof(int64_t)*10);
	memset(_mdprBlockSize, 0, sizeof(_mdprBlockSize));
	_mdprBlockCount = 0;

	memset(_dataBlockOffset, 0, sizeof(_dataBlockOffset));
	memset(_dataBlockSize, 0, sizeof(_dataBlockSize));
	_dataBlockCount = 0;


	memset(_indxBlockOffset, 0, sizeof(_indxBlockOffset));
	memset(_indxBlockSize, 0, sizeof(_indxBlockSize));
	_indxBlockCount = 0;

	_current = 0;
	_end = 0;


}
RmvbDownldMediaFile::~RmvbDownldMediaFile()
{
	if (_pRmvbPrase == NULL)
	{
		delete _pRmvbPrase;
		_pRmvbPrase = NULL;
	}
	if(_downloadBlockInfoList != NULL)
	{
		delete[] _downloadBlockInfoList;
		_downloadBlockInfoList =NULL;
	}
}
int RmvbDownldMediaFile::Finish()
{
	if (_pRmvbPrase == NULL)
	{
		delete _pRmvbPrase;
		_pRmvbPrase = NULL;
	}
	if(_downloadBlockInfoList != NULL)
	{
		delete[] _downloadBlockInfoList;
		_downloadBlockInfoList = NULL;
		printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
						   "rmvb finish",
						   "delete the list memory\n",
						   	pMediaFileDownldLog);
	}
	pMediaFileDownldLog = NULL;
	return 1;
}
int RmvbDownldMediaFile::RmvbCheckBoxType(char* buf)
{
	buf[4] = 0;
	if (stricmp(buf, "PROP") == 0)
	{
		return PROP;
	}
	else if (stricmp(buf, "MDPR") == 0)
	{
		return MDPR;
	}
	else if (stricmp(buf, "CONT") == 0)
	{
		return CONT;
	}
	else if (stricmp(buf, "DATA") == 0)
	{
		return DATA;
	}
	else if (stricmp(buf, "INDX") == 0)
	{
		return INDX;
	}
	else{
		return -1;
	}
}
bool RmvbDownldMediaFile::PraseRootBox()
{
	unsigned char readBuf[100] = { 0 };
	int64_t blockSize;
	int len = 0;
	_current = 0;
	if(0 != P2pDownloadMediaData(0,MEDIAFILEPARSELEN))
	{
		return false;
	}
	LimaoApi_getFilePath(_mediaFileHash, _playMediaFilePath);
	if(strlen(_playMediaFilePath) == 0)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "rmvb download parse root box type and size",
				   "get p2p download media file path failed.\n",
				   	pMediaFileDownldLog);
		return false;
	}
	_pPlayerMediaFile = fopen(_playMediaFilePath,"rb");
	if(_pPlayerMediaFile == NULL)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "rmvb download parse root box type and size",
				   "open the p2p download media file failed.\n",
				   	pMediaFileDownldLog);
		return false;
	}
	_end =  LimaoApi_getFileSize(_mediaFileHash);
	if(_end <= 0)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "rmvb download parse root box type and size",
				   "get p2p download media file size failed.\n",
				   	pMediaFileDownldLog);
		return false;
	}


	len = fread(readBuf, 1, MEDIAFILEPARSELEN, _pPlayerMediaFile);
	if (len != MEDIAFILEPARSELEN)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "rmvb download parse root box type and size",
				   "p2p download 100 Byte failed.\n",
				   	pMediaFileDownldLog);
		return false;

	}
	blockSize = _dataPrase.DataToBigEndianInt32(readBuf + 4);
	if (blockSize <= 0)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "rmvb download parse root box type and size",
				   "parse the box size failed.\n",
				   	pMediaFileDownldLog);
		return false;
	}

	if(0 != P2pDownloadMediaData(blockSize,MEDIAFILEPARSELEN))
	{
		return false;
	}

	if(0 != lfseek(_pPlayerMediaFile, blockSize, SEEK_SET))
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "rmvb download parse root box type and size",
				   "seek the play media file failed.\n",
				   	pMediaFileDownldLog);
		return false;
	}
	_current = blockSize;
	_propBlockSize = blockSize;
	int64_t read_len = 0;
	while (_current < _end)
	{
		if(_current + MEDIAFILEPARSELEN > _end)
		{
			read_len = _end - _current;
		}else
		{
			read_len = MEDIAFILEPARSELEN;
		}
		memset(readBuf, 0, 100);
		len = fread(readBuf, 1, read_len, _pPlayerMediaFile);
		if (len != read_len)
		{
			printf("rmvb download media file prase root box falied\n");
			return false;
		}
		blockSize = _dataPrase.DataToBigEndianInt32(readBuf+4);
		if (blockSize <= 0)
		{
			printf("rmvb download read box len failed\n");
			return false;
		}
		int type = RmvbCheckBoxType((char *)(readBuf));
		if (type == -1)
		{
			printf("rmvb download check box type failed\n");
			return false;
		}
		switch (type)
		{
		case CONT:
			_contBlockOffset = _current;
			_contBlockSize = blockSize;
			break;
		case MDPR:
			_mdprBlockOffset[_mdprBlockCount] = _current;
			_mdprBlockSize[_mdprBlockCount++] = blockSize;
			break;
		case DATA:
			_dataBlockOffset[_dataBlockCount] = _current;
			_dataBlockSize[_dataBlockCount++] = blockSize;
			break;
		case INDX:
			_indxBlockOffset[_indxBlockCount] = _current;
			_indxBlockSize[_indxBlockCount++] = blockSize;
			break;
		default:
			break;
		}

		if (_current + blockSize >= _end)
		{
			return true;
		}
		_current = blockSize + _current;
		if(_current + MEDIAFILEPARSELEN > _end)
		{
			read_len = _end - _current;
		}else
		{
			read_len = MEDIAFILEPARSELEN;
		}
		if(0 != P2pDownloadMediaData(_current,read_len))
		{
			return false;
		}
		if(0 != lfseek(_pPlayerMediaFile, _current, SEEK_SET))
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "rmvb download parse root box type and size",
					   "seek the play media file failed.\n",
					   	pMediaFileDownldLog);
			return false;
		}
	}
	return true;
}

bool RmvbDownldMediaFile::DownloadFileFirst()
{

	// write file fype box
	if (_propBlockSize == 0)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "rmvb download file play info",
				   "prop block size is invalied.\n",
				   	pMediaFileDownldLog);
		return false;
	}
	if (0 != P2pDownloadMediaData(0, _propBlockSize))
	{
		return false;
	}

	//write file cont boxs

	if ((_contBlockOffset == 0) || (_contBlockSize == 0))
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "rmvb download file play info",
				   "cont block size is invalied.\n",
				   	pMediaFileDownldLog);
		return false;
	}
	if (0 != P2pDownloadMediaData(_contBlockOffset, _contBlockSize))
	{
		return false;
	}

	//write file mdpr box

	for (int i = 0; i < _mdprBlockCount; i++)
	{
		if ((_mdprBlockOffset[i] <= 0) || (_mdprBlockSize[i] <= 0))
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "rmvb download file play info",
					   "mdpr block size is invalied.\n",
					   	pMediaFileDownldLog);
			return false;
		}
		if (0 != P2pDownloadMediaData(_mdprBlockOffset[i], _mdprBlockSize[i]))
		{
			return false;
		}

	}

	//write file data box
	for (int i = 0; i < _dataBlockCount; i++)
	{
		if ((_dataBlockOffset[i] == 0) || (_dataBlockSize[i] == 0))
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
								   "rmvb download file play info",
								   "data block size is invalied.\n",
								   	pMediaFileDownldLog);
			return false;
		}
		if (0 != P2pDownloadMediaData(_dataBlockOffset[i], MEDIAFILEPARSELEN * 10))
		{
			return false;
		}
	}


	//write file indx box
	for (int i = 0; i < _indxBlockCount; i++)
	{
		if ((_indxBlockOffset[i] == 0) || (_indxBlockSize[i] == 0))
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
								   "rmvb download file play info",
								   "index block size is invalied.\n",
								   	pMediaFileDownldLog);
			return false;
		}
		if (0 != P2pDownloadMediaData(_indxBlockOffset[i], _indxBlockSize[i]))
		{
			return false;
		}
	}
	printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
						   "rmvb download file play info",
						   "_pRmvbPrase = new RmvbPrase(_pPlayerMediaFile);",
						   	pMediaFileDownldLog);
	_pRmvbPrase = new RmvbPrase(_pPlayerMediaFile);

	_pRmvbPrase->GetKeyFrameInfo(&_downloadBlockInfoList);

	_syncSampleCount = _pRmvbPrase->_indexRecordCount;

	_duration = _pRmvbPrase->_duration;


	if(_downloadBlockInfoList == NULL)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
							   "rmvb download file play info",
							   "alloc down load block info list failed.",
							   	pMediaFileDownldLog);
	}
	char log_buf[200] = {0};
	sprintf(log_buf,"index block size is success. boclk count is %d  duration is %llu", _syncSampleCount, _duration);
	printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
						   "rmvb download file play info",
						   log_buf,
						   	pMediaFileDownldLog);
	return true;
}
int RmvbDownldMediaFile::GetMdataBlockCount()
{
	return _syncSampleCount;
}
bool RmvbDownldMediaFile::DownloadMdatBlock(int index)
{
	if(_downloadBlockInfoList == NULL)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "download the rmvb file a block media data",
				   "_downloadBlockInfoList is null.\n",
					pMediaFileDownldLog);
		return false;
	}
	unsigned long startOffset;
	if (index == 0)
	{
		startOffset = _dataBlockOffset[0];
	}
	else
	{
		startOffset = _downloadBlockInfoList[index].offset;
	}
	unsigned long endOffset;
	if (index + 1 == _syncSampleCount)
	{
		endOffset = _dataBlockOffset[0] + _dataBlockSize[0];
	}
	else
	{
		endOffset  = _downloadBlockInfoList[index + 1].offset;
	}
	if (endOffset <= startOffset)
	{
		char logbuf[200] = {0};
		sprintf(logbuf,"the block offset is %llu  %llu",_downloadBlockInfoList[index].offset ,_downloadBlockInfoList[index + 1].offset);
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "download the rmvb file a block media data",
				   logbuf,
					pMediaFileDownldLog);
		return false;
	}
	long size = endOffset - startOffset;
	if(0 != P2pDownloadMediaData(startOffset, size))
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "download the mp4 file a block media data",
				   "the p2p module downlaod failed.\n",
					pMediaFileDownldLog);
		return false;
	}
	_downloadBlockInfoList[index].isDownload = true;

	char log[300] = {0};
	sprintf(log," download the %d block , size is %ld,the endtime is %u\n", index, size, _downloadBlockInfoList[index].timeStamp);
	printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
			   "download the rmvb file a block media data",
				log,
				pMediaFileDownldLog);
	return true;
}
