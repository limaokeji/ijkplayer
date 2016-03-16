#ifndef _ANDROID
#include "stdafx.h"
#endif
#include "Mp4DownldMediaFile.h"
#include "./sources/mp4.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../mediafile_download_log.h"
#include "../../android/limao_api_jni.h"
#define stricmp strcasecmp
Mp4DownldMediaFile::Mp4DownldMediaFile()
{
	_freeBlockCount = 0;
	memset((void *)_freeBlockOffset, 0, sizeof(long)* 10);
	memset((void *)_freeBlockSize, 0, sizeof(int64_t)* 10);
	_moovBlockOffset = 0;
	_moovBlockSize = 0;
	_mdatBlockOffset = 0;
	_mdatBlockSize = 0;
	_current = 0;
	_end = 0;
	_syncSampleCount = 0;
	pMediaFileDownldLog = NULL;
}

Mp4DownldMediaFile::~Mp4DownldMediaFile()
{
	//_freeBlockCount = 0;
	//memset((void *)_freeBlockOffset, 0, sizeof(long)* 10);
	//memset((void *)_freeBlockSize, 0, sizeof(int64_t)* 10);
	//_moovBlockOffset = 0;
	//_moovBlockSize = 0;
	//_mdatBlockOffset = 0;
	//_mdatBlockSize = 0;
	//_current = 0;
	//_end = 0;
	if(_downloadBlockInfoList != NULL)
	{
		delete[] _downloadBlockInfoList;
		_downloadBlockInfoList = NULL;
		printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
						   "mp4 finish",
						   "delete the list memory\n",
						   	pMediaFileDownldLog);
	}
}
int Mp4DownldMediaFile:: Finish()
{
	if(_downloadBlockInfoList != NULL)
	{
		delete[] _downloadBlockInfoList;
		_downloadBlockInfoList = NULL;
		printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
						   "mp4 ~Mp4DownldMediaFile",
						   "delete the list memory\n",
						   	pMediaFileDownldLog);
	}
	pMediaFileDownldLog = NULL;
	return 1;
}
int Mp4DownldMediaFile::Mp4CheckBoxType( char* buf)
{
	buf[4] = 0;
	if (stricmp(buf, "ftyp") == 0)
	{
		return FTYP;
	}
	else if (stricmp(buf, "free") == 0)
	{
		return FREE;
	}
	else if (stricmp(buf, "mdat") == 0)
	{
		return MDAT;
	}
	else if (stricmp(buf, "moov") == 0)
	{
		return MOOV;
	}else if(stricmp(buf, "skip") == 0)
	{
		return SKIP;
	}
	else if(stricmp(buf, "udta") == 0)
	{
		return UDTA;
	}else if(stricmp(buf, "moof") == 0)
	{
		return MOOF;
	}
	else{
		return FREE;
	}

}
bool Mp4DownldMediaFile::PraseRootBox()
{
	printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
			   "mp4 download parse root box type and size",
			   "prase the mp4 file root box in.\n",
			   	pMediaFileDownldLog);
	int ret = -1;
	unsigned char readBuf[100] = { 0 };
	_current = 0;
	if(0 != P2pDownloadMediaData(0,MEDIAFILEPARSELEN))
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mp4 download parse root box type and size",
				   "P2pDownloadMediaData MEDIAFILEPARSELEN failed.\n",
				   	pMediaFileDownldLog);
		return false;
	}


	int64_t blockSize;

	LimaoApi_getFilePath(_mediaFileHash, _playMediaFilePath);
	if(strlen(_playMediaFilePath) == 0)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mp4 download parse root box type and size",
				   "get p2p download media file path failed.\n",
				   	pMediaFileDownldLog);
		return false;
	}
	_pPlayerMediaFile = fopen(_playMediaFilePath,"rb");
	if(_pPlayerMediaFile == NULL)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mp4 download parse root box type and size",
				   "open the p2p download media file failed.\n",
				   	pMediaFileDownldLog);
		return false;
	}
	char logbuf[100] ={ 0 };
	_end = LimaoApi_getFileSize(_mediaFileHash);
	sprintf(logbuf,"media file size %llu",_end);
	if(_pPlayerMediaFile == NULL)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
				   "mp4 download parse root box type and size",
				   "open the media file failed.",
				   	pMediaFileDownldLog);
		return false;
	}


	printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
				   "mp4 download parse root box type and size",
				   logbuf,
				   	pMediaFileDownldLog);
	if(_end <= 0)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
				   "mp4 download parse root box type and size",
				   "_end < = 0.",
				   	pMediaFileDownldLog);
		return false;
	}

	if(0 !=lfseek(_pPlayerMediaFile,0,SEEK_SET))
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mp4 download parse root box type and size",
				   "seek the play media file failed.\n",
				   	pMediaFileDownldLog);
		return false;
	}
	int len = fread(readBuf, 1, MEDIAFILEPARSELEN, _pPlayerMediaFile);
	if (len != MEDIAFILEPARSELEN)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mp4 download parse root box type and size",
				   "p2p download 100 Byte failed.\n",
				   	pMediaFileDownldLog);
		return false;
	}
	blockSize = _dataPrase.DataToBigEndianInt32(readBuf);
	if (blockSize <= 0)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mp4 download parse root box type and size",
				   "parse the box size failed.\n",
				   	pMediaFileDownldLog);
		return false;
	}
	if(0 != P2pDownloadMediaData(blockSize ,MEDIAFILEPARSELEN))
	{
		return false;
	}

	if(0 != lfseek(_pPlayerMediaFile, blockSize, SEEK_SET))
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mp4 download parse root box type and size",
				   "seek the play media file failed.\n",
				   	pMediaFileDownldLog);
		return false;
	}
	_current = lftell(_pPlayerMediaFile);
	_ftypBlockSize = blockSize;
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
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mp4 download parse root box type and size",
					   "p2p download 100 Byte failed..\n",
					   	pMediaFileDownldLog);
			return false;
		}
		blockSize = _dataPrase.DataToBigEndianInt32(readBuf);
		if (blockSize <= 0)
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mp4 download parse root box type and size",
					   "parse the box size failed.\n",
					   	pMediaFileDownldLog);
			return false;
		}
		int type = Mp4CheckBoxType((char *)(readBuf + 4));
		if (type == -1)
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mp4 download parse root box type and size",
					   "check box type is failed.\n",
					   	pMediaFileDownldLog);
			return false;
		}
		switch (type)
		{
		case FREE:
			_freeBlockOffset[_freeBlockCount] = _current;
			_freeBlockSize[_freeBlockCount++] = blockSize;
			break;
			
		case MOOV:
			_moovBlockOffset = _current;
			_moovBlockSize = blockSize;
			break;
		case MDAT:
			_mdatBlockOffset = _current;
			_mdatBlockSize = blockSize;
			break;
		default:
			_freeBlockOffset[_freeBlockCount] = _current;
			_freeBlockSize[_freeBlockCount++] = blockSize;
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
		if(0 != P2pDownloadMediaData(_current ,read_len))
		{
			return false;
		}

		if(0 != lfseek(_pPlayerMediaFile, _current, SEEK_SET))
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mp4 download parse root box type and size",
					   "seek the play media file failed.\n",
					   	pMediaFileDownldLog);
		}



	}
	
	printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
			   "mp4 download parse root box type and size",
			   "prase the mp4 file root box success.\n",
			   	pMediaFileDownldLog);
	return true;
}

bool Mp4DownldMediaFile::GetDownloadOffset()
{
	u_int32_t verbosity = MP4_DETAILS_ERROR;

	// open the mp4 file, and read meta-info
	char * outFileName = GetPlayFileNamePath();


	fseek(_pPlayerMediaFile, 0, SEEK_SET);

	MP4FileHandle mp4File = MP4Read(outFileName, _pPlayerMediaFile, verbosity);

	if (mp4File == NULL)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "get download offset info",
				   "mp4 parse class create failed.\n",
				   	pMediaFileDownldLog);
		return false;
	}
	u_int8_t profileLevel = MP4GetVideoProfileLevel(mp4File);

	// get a handle on the first video track
	MP4TrackId trackId = MP4FindTrackId(mp4File, 0, "video");

	// gather the crucial track information 
	u_int32_t timeScale = MP4GetTrackTimeScale(mp4File, trackId);

	// note all times and durations 
	// are in units of the track time scale
	MP4Duration trackDuration = MP4GetTrackDuration(mp4File, trackId);

	u_int8_t* pConfig;
	u_int32_t configSize = 0;

	MP4GetTrackESConfiguration(mp4File, trackId, &pConfig, &configSize);

	// initialize decoder with Elementary Stream (ES) configuration

	// done with our copy of ES configuration
	free(pConfig);

	// now consecutively read and display the track samples

	MP4SampleId sampleId = 1;
	u_int64_t sampleFileOffset = 0;
	MP4Timestamp sampleTimestamp;

	u_int32_t syncSampleCount = MP4GetSyncSampleCount(mp4File,
		trackId);

	if (syncSampleCount <= 0)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "get download offset info",
				   "mp4 parse class create failed.\n",
				   	pMediaFileDownldLog);
		return false;
	}
	_syncSampleCount = syncSampleCount - 1;
	_downloadBlockInfoList = new DOWNLOADBLOCKINFO[syncSampleCount];
	if(_downloadBlockInfoList == NULL)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "get download offset info",
				   "_downloadBlockInfoList alloc failed.\n",
				   	pMediaFileDownldLog);
	}
	int index = 0;
	memset(_downloadBlockInfoList, 0, sizeof(DOWNLOADBLOCKINFO)*syncSampleCount);

	do
	{
		sampleId = MP4GetNextSyncSampleId(
			mp4File,
			trackId,
			sampleId);
		if (sampleId != MP4_INVALID_SAMPLE_ID)
		{
			sampleFileOffset = MP4GetSampleFileOffset(mp4File, trackId, sampleId);
			sampleTimestamp = MP4GetSampleTime(mp4File, trackId, sampleId);
			_downloadBlockInfoList[index].smapleId = sampleId;
			_downloadBlockInfoList[index].offset = sampleFileOffset;
			_downloadBlockInfoList[index].timeStamp = sampleTimestamp;
			//_downloadBlockInfoList[index].timeToSecond = sampleTimestamp / timeScale;
			_downloadBlockInfoList[index].isDownload = false;
			//_downloadBlockInfoList[index].isRequest = false;

			index++;

		}


	} while (sampleId != MP4_INVALID_SAMPLE_ID);

	printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
			   "get download offset info",
			   "create the offset info list success.\n",
			   	pMediaFileDownldLog);

	//MP4Close(mp4File);
	return true;
}

bool Mp4DownldMediaFile::DownloadFileFirst()
{
	char writeBuf[1024] = { 0 };
	// write file fype box
	if (_ftypBlockSize == 0)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "download the mp4 file header moov box",
				   "the header size is invalid.\n",
				   	pMediaFileDownldLog);
		return false;
	}
	if (0 != P2pDownloadMediaData(0, _ftypBlockSize))
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "download the mp4 file header moov box",
				   "downlaod mp4 file header box failed.\n",
				   	pMediaFileDownldLog);
		return false;
	}

	//write file free boxs

	for (int i = 0; i < _freeBlockCount; i++)
	{
		if ((_freeBlockOffset[i] <= 0) || (_freeBlockSize[i] <= 0))
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "download the mp4 file header moov box",
					   "the free size or offset is invalid.\n",
					   	pMediaFileDownldLog);
			return false;
		}
		if (0 != P2pDownloadMediaData(_freeBlockOffset[i], _freeBlockSize[i]))
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "download the mp4 file header moov box",
					   "downlaod mp4 file free box failed.\n",
					   	pMediaFileDownldLog);
			return false;
		}

	}

	//write file moov box
	if ((_moovBlockOffset == 0) || (_moovBlockSize == 0))
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "download the mp4 file header moov box",
				   "the moov size or offset is invalid.\n",
				   	pMediaFileDownldLog);
		return false;
	}
	if (0 != P2pDownloadMediaData(_moovBlockOffset, _moovBlockSize))
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "download the mp4 file header moov box",
				   "downlaod mp4 file moov box failed.\n",
				   	pMediaFileDownldLog);
		return false;
	}


	printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
			   "download the mp4 file play info",
			   " success.\n",
			   	pMediaFileDownldLog);

	if (!GetDownloadOffset())
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "download the mp4 file play info",
				   "get download offset failed.\n",
				   	pMediaFileDownldLog);
		return false;
	}
	
	return true;
}
bool Mp4DownldMediaFile::StartDownloadMediaData()
{
	long firstBlockSize = _downloadBlockInfoList[1].offset - _mdatBlockOffset;
	if(0 != P2pDownloadMediaData(_mdatBlockOffset, firstBlockSize))
	{
		return false;
	}
	_downloadBlockInfoList[0].isDownload = true;
	_downloadBlockInfoList[1].isDownload = true;
	for (int i = 1; i < _syncSampleCount; i++)
	{
		DownloadMdatBlock(i);
		getchar();
	}
	return true;
}
bool Mp4DownldMediaFile::DownloadMdatBlock(int index)
{
	if(_downloadBlockInfoList == NULL)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "download the mp4 file a block media data",
				   "_downloadBlockInfoList is null.\n",
					pMediaFileDownldLog);
		return false;
	}
	unsigned long startOffset;
	if (index == 0)
	{
		startOffset = _mdatBlockOffset;
	}
	else
	{
		startOffset = _downloadBlockInfoList[index].offset;
	}
	unsigned long endOffset;
	if (index + 1 == _syncSampleCount)
	{
		endOffset = _mdatBlockOffset + _mdatBlockSize;
	}
	else
	{
		endOffset  = _downloadBlockInfoList[index + 1].offset;
	}
	if (endOffset <= startOffset)
	{
		printf("mp4 download media file offset if invalid\n");
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "download the mp4 file a block media data",
				   "the block offset or size is invalid.\n",
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
			   "download the mp4 file a block media data",
				log,
			   	pMediaFileDownldLog);
	return true;
}

int Mp4DownldMediaFile::GetMdataBlockCount()
{
	return _syncSampleCount;
}
bool Mp4DownldMediaFile::CheckMdataBlcokDownload(int index)
{
	if (index >= _syncSampleCount)
	{
		printf("download mp4 file check mdata block is download the index Overflow.\n");
		return false;
	}
	return  _downloadBlockInfoList[index + 1].isDownload;
}
