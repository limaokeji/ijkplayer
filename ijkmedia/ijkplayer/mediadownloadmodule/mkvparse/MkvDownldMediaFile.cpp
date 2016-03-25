#include "MkvDownldMediaFile.h"
#include <stdlib.h>
#include "../mediafile_download_log.h"
#include "../../android/limao_api_jni.h"
MkvDownldMediaFile::MkvDownldMediaFile()
{
	_ebmlHeaderBlockSize = 0; //root block

	_segmentBlockOffset = 0;	//root block
	_segmentBlockSize = 0;

	_metaSeekInfoBlockOffset = 0; //child block
	_metaSeekInfoBlockSize = 0;

	_segmentInfoBlockOffset = 0;//child block
	_segmentInfoBlockSize = 0;

	_trackBlockOffset = 0;//child block
	_trackBlockSize = 0;

	_chaptersBlockOffset = 0;//child block
	_chaptersBlockSize = 0;

	_clustersBlockOffset = 0;//child block the audio video data
	_clustersBlockSize = 0;
	_firstClustersBlockOffset = 0;
	_allClustersBlockSize = 0;

	_cueingDataBlockOffset = 0;//child block  Contain the key frame infomation
	_cueingDataBlockSize = 0;

	_attachmentBlockOffset = 0;//child block
	_attachmentBlockSize = 0;

	_taggingBlockOffset = 0;//child block
	_taggingBlockSize = 0;

	_otherBlockCount = 0;
	memset(_otherBlcokOffset,0,sizeof(int64_t));
	memset(_otherBlockSize,0,sizeof(int64_t));
	printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
			_mediaFileHash,
			"MKVDownldMediaFile()",
						   	pMediaFileDownldLog);
	printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mkv new",
					   "malloc memory\n",
					   	pMediaFileDownldLog);
	_syncSampleCount = 0;
	_downloadBlockInfoList = NULL;
	_downloadBlockInfoList = (DOWNLOADBLOCKINFO *)malloc(sizeof(DOWNLOADBLOCKINFO)* 101);
	if(_downloadBlockInfoList != NULL)
	{
		memset(_downloadBlockInfoList, 0, sizeof(DOWNLOADBLOCKINFO)* 101);
	}



#ifdef FEYFRAMELOG
	pkeyframeLog = fopen("/sdcard/limao/MKV_KeyFrame.txt","w+");
#endif
}
MkvDownldMediaFile::~MkvDownldMediaFile()
{
	if (_downloadBlockInfoList != NULL)
	{
		free(_downloadBlockInfoList);
		_downloadBlockInfoList = NULL;
	}
#ifdef FEYFRAMELOG
	if(pkeyframeLog != NULL)
		fclose(pkeyframeLog);
#endif
}
int MkvDownldMediaFile::Finish()
{
	if (_downloadBlockInfoList != NULL)
	{
		free(_downloadBlockInfoList);
		_downloadBlockInfoList = NULL;
	}
#ifdef FEYFRAMELOG
	if(pkeyframeLog != NULL)
		fclose(pkeyframeLog);
#endif
	printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
					   "mkv finish",
					   "delete the list memory\n",
					   	pMediaFileDownldLog);
	pMediaFileDownldLog = NULL;
	return 1;
}
bool MkvDownldMediaFile::PraseSeekInfoBlock(long offset, int64_t size, unsigned int& ID, int64_t& blockSize)
{
	unsigned char blockId[10] = { 0 };
	unsigned char blockSizeBuf[10] = { 0 };
	int idLen = 0;
	int blockSizeBufLen = 0;
	/*
	����������SeekHead ��һ��Ԫ��
	Seek
	ID = 4D BB
	size = 12
	data = 53 AB 84 16 54 AE 6B 53 AC 82 10 03
	data�������Ϣ����seekID �� SeekPosition
	53 AB ��ʾseekIDԪ��
	size = 4
	seekID = 15 49 A9 66
	53 AC ��ʾSeekPositionԪ��
	size = 2
	SeekPosition = 10 03  (4099)*/

	if(0 != P2pDownloadMediaData(offset,size + 10))
	{
		return false;
	}
	if (0 != lfseek(_pPlayerMediaFile, offset, SEEK_SET))
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "fseek the downlaod file failed.",
				   	pMediaFileDownldLog);
		return false;
	}
	unsigned char * buffer = new unsigned char[size + 10];
	memset((void *)buffer, 0, size + 10);
	if (size +10 != fread(buffer, 1, size +10, _pPlayerMediaFile))
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "fread the downlaod file failed.",
				   	pMediaFileDownldLog);
		delete[] buffer;
		return false;
	}
	if ((buffer[0] != 0x4d) || (buffer[1] != 0xbb))
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "downld media file Seek Info Block invalid.",
				   	pMediaFileDownldLog);
		return false;
	}
	if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, buffer))  //Seek 
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "download media file parse block id and size failed.",
				   	pMediaFileDownldLog);
		return false;
	}
	unsigned char *pBlockSeekInfo = buffer + idLen + blockSizeBufLen;
	memset(blockId, 0, 10);
	memset(blockSizeBuf, 0, 10);
	idLen = 0;
	blockSizeBufLen = 0;
	while ((pBlockSeekInfo[0] == 0x53) && (pBlockSeekInfo[1] == 0xAB))
	{
		if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, pBlockSeekInfo)) //seek id   53 AB
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mkv download parse root box type and size",
					   "download media file parse block id and size failed.",
					   	pMediaFileDownldLog);
			return false;
		}
		if ((blockId[0] != 0x53) || (blockId[1] != 0xAB))
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mkv download parse root box type and size",
					   "download media file parse block id and size failed.",
					   	pMediaFileDownldLog);
			return false;
		}
		memset(blockId, 0, 10);
		memcpy(blockId, pBlockSeekInfo + idLen + blockSizeBufLen, 4);
		ID = MkvCheckBoxType(blockId);


		pBlockSeekInfo = pBlockSeekInfo + idLen + blockSizeBufLen + 4;
		memset(blockId, 0, 10);
		memset(blockSizeBuf, 0, 10);
		idLen = 0;
		blockSizeBufLen = 0;
		if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, pBlockSeekInfo)) // seek postion 53 AC
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mkv download parse root box type and size",
					   "download media file parse block id and size failed.",
					   	pMediaFileDownldLog);
			return false;
		}
		int dataSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);
		if (dataSize <= 0)
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mkv download parse root box type and size",
					   "download media file parse block id size failed.",
					   	pMediaFileDownldLog);
			return false;
		}
		unsigned char data[10] = { 0 };

		memcpy(data, pBlockSeekInfo + idLen + blockSizeBufLen, dataSize);

		blockSize = GetBlockSize(data, dataSize);

		switch (ID)
		{
		case  SEGMENT_INFO:
			_segmentBlockOffset = blockSize + _metaSeekInfoBlockOffset;
			break;
		case TRACK:
			_trackBlockOffset = blockSize + _metaSeekInfoBlockOffset;
			break;
		case  CLUSTER:	
			_clustersBlockOffset = blockSize + _metaSeekInfoBlockOffset;
			break;
		case  CUES:
			_cueingDataBlockOffset = blockSize + _metaSeekInfoBlockOffset;
			break;
		default:
			break;
		}



		pBlockSeekInfo = pBlockSeekInfo + idLen + blockSizeBufLen + dataSize;

		if ((pBlockSeekInfo[0] != 0x4D) || (pBlockSeekInfo[1] != 0xBB))
		{
			break; //end
		}

		if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, pBlockSeekInfo))  //Seek 
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mkv download parse root box type and size",
					   "download media file parse block id size failed.",
					   	pMediaFileDownldLog);
			return false;
		}
		pBlockSeekInfo = pBlockSeekInfo + idLen + blockSizeBufLen;


	}
	/* 11 4D 9B 74 | 40 3D  | 4D BB | 8B | 53 AB | 84 | 15 49 A9 66
	   53 AC | 81 | DF      | 4D BB | 8C | 53 AB | 84 | 16 54 AE 6B 
	   53 AC | 82 | 01 2C   | 4D BB | 8C | 53 AB | 84 | 12 54 C3 67 
	   53 AC | 82 | 02 0B   | 4D BB | 8E | 53 AB | 84 | 1C 53 BB 6B 
	   53 AC | 84 | 28 73 14 24 | EC 01 */

	return true;

}
bool MkvDownldMediaFile::PraseCueTrackPosition(unsigned char * buffer, int len)
{
	CHECK_POINTER_RET(buffer, false);
	CHECK_INT_RET(len, false);
	//CueTrack				4 [F7] * -not 0 - u	 *	 *	 *	 *	 The track for which a position is given.
	//CueClusterPosition	4 [F1] * -- - u	 *	 *	 *	 *	 The position of the Cluster containing the required Block.
	//CueRelativePosition	4 [F0] - -- - u	  	  	  	  	 The relative position of the referenced block inside the cluster with 0 being the first possible position for an element inside that cluster.
	//CueDuration			4 [B2] - -- - u	  	  	  	  	 The duration of the block according to the segment time base.If missing the track's DefaultDuration does not apply and no duration information is available in terms of the cues.
	//CueBlockNumber		4 [53][78] - -not 0	 1	 u	 *	 *	 *	 *	 Number of the Block in the specified Cluster.
	//CueCodecState			4 [EA] - --0	 u	  	 *	 *	  	 The position of the Codec State corresponding to this Cue element. 0 means that the data is taken from the initial Track Entry.
	//CueReference			4 [DB] - *--m	  	 *	 *	  	 The Clusters containing the required referenced Blocks.
	//CueRefTime			5 [96]
	int index = 0;
	unsigned char blockId[10] = { 0 };
	unsigned char blockSizeBuf[10] = { 0 };
	int idLen = 0;
	int blockSizeBufLen = 0;
	uint32_t blockSize = 0;
	while (index < len)
	{
		memset(blockId, 0, 10);
		memset(blockSizeBuf, 0, 10);
		idLen = 0;
		blockSizeBufLen = 0;
		blockSize = 0;
		if (buffer[index] == 0xF7)//CueTrack	
		{
			if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, buffer + index))  
			{
				printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
						   "mkv download parse root box type and size",
						   "download media file prase block id and size failed.\n",
						   	pMediaFileDownldLog);
				return false;
			}

			blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);

			int64_t curTrack = GetBlockSize(buffer + index + idLen + blockSizeBufLen, blockSize);

		}
		else if (buffer[index] == 0xF1)//CueClusterPosition
		{
			if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, buffer + index))
			{
				printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
						   "mkv download parse root box type and size",
						   "download media file prase block id and size failed.\n",
						   	pMediaFileDownldLog);
				return false;
			}

			blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);

			int64_t CueClusterPosition = GetBlockSize(buffer + index + idLen + blockSizeBufLen, blockSize);

			#ifdef FEYFRAMELOG
			if(pkeyframeLog != NULL)
			{
				fprintf(pkeyframeLog,", offset %llu .\n",CueClusterPosition);
			}
			#endif
			_downloadBlockInfoList[_syncSampleCount].offset = CueClusterPosition;

		}
		else if (buffer[index] == 0xF0)//CueRelativePosition
		{
			if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, buffer + index))
			{
				printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
						   "mkv download parse root box type and size",
						   "download media file prase block id and size failed.\n",
						   	pMediaFileDownldLog);
				return false;
			}

			blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);

			int64_t CueRelativePosition = GetBlockSize(buffer + index + idLen + blockSizeBufLen, blockSize);
		}
		else  if (buffer[index] == 0xB2)//CueDuration	
		{
			if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, buffer + index))
			{
				printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
						   "mkv download parse root box type and size",
						   "download media file prase block id and size failed.\n",
						   	pMediaFileDownldLog);
				return false;
			}

			blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);

			int64_t CueDuration = GetBlockSize(buffer + index + idLen + blockSizeBufLen, blockSize);

		}
		else if ((buffer[index] == 0x53) && (buffer[index + 1] == 0x78))//CueBlockNumber
		{
			if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, buffer + index))
			{
				printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
						   "mkv download parse root box type and size",
						   "download media file prase block id and size failed.\n",
						   	pMediaFileDownldLog);
				return false;
			}

			blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);

			int64_t CueBlockNumber = GetBlockSize(buffer + index + idLen + blockSizeBufLen, blockSize);

		}
		else if (buffer[index] == 0xEA)//CueCodecState	
		{
			if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, buffer + index))
			{
				printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
						   "mkv download parse root box type and size",
						   "download media file prase block id and size failed.\n",
						   	pMediaFileDownldLog);
				return false;
			}

			blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);

			int64_t CueCodecState = GetBlockSize(buffer + index + idLen + blockSizeBufLen, blockSize);

		}
		else if (buffer[index] == 0xDB)//CueReference	
		{

		}
		index = index + idLen + blockSizeBufLen + blockSize;
	}
	return true;
}
bool MkvDownldMediaFile::PraseCuePoint(unsigned char * buffer,int len)
{
	CHECK_POINTER_RET(buffer, false);
	CHECK_INT_RET(len, false);
	unsigned char blockId[10] = { 0 };
	unsigned char blockSizeBuf[10] = { 0 };
	int idLen = 0;
	int blockSizeBufLen = 0;
	unsigned char * pBuf;
	//CueTime

	if (buffer[0] != 0xBB)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   " download media file prase CuePoint failed.\n",
				   	pMediaFileDownldLog);
		return false;
	}
	if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, buffer))  //BB CuePoint
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "download media file prase block id and size failed.\n",
				   	pMediaFileDownldLog);
		return false;
	}

	uint32_t blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);
	buffer = buffer + idLen + blockSizeBufLen;
	if (buffer[0] == 0xB3)
	{
		memset(blockId, 0, 10);
		memset(blockSizeBuf, 0, 10);
		idLen = 0;
		blockSizeBufLen = 0;
		if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, buffer))  // B3 CueTime
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mkv download parse root box type and size",
					   "download media file prase block id and size failed.\n",
					   	pMediaFileDownldLog);
			return false;
		}

		blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);

		int64_t timestamp = GetBlockSize(buffer + idLen + blockSizeBufLen, blockSize);

		if(_downloadBlockInfoList == NULL)
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
									   "mkv download get offset info",
									    "_downloadBlockInfoList == NULL",
									   	pMediaFileDownldLog);
			return false;
		}
		_downloadBlockInfoList[_syncSampleCount].timeStamp = timestamp;

		_downloadBlockInfoList[_syncSampleCount].isDownload = false;

		_downloadBlockInfoList[_syncSampleCount].smapleId = _syncSampleCount;


#ifdef FEYFRAMELOG
	if(pkeyframeLog != NULL)
	{
		fprintf(pkeyframeLog,"key frame info timestamp  %llu , sample id %d",timestamp , _syncSampleCount );
	}
#endif
		if ((_syncSampleCount % 100 == 0) && (_syncSampleCount >=100))
		{
			_downloadBlockInfoList = (DOWNLOADBLOCKINFO*)realloc(_downloadBlockInfoList, sizeof(DOWNLOADBLOCKINFO)* (_syncSampleCount + 102));
			if (_downloadBlockInfoList == NULL)
			{
				printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
						   "mkv download parse root box type and size",
						   "download media file realloc blockinfoList memory failed.\n",
						   	pMediaFileDownldLog);
				return false;
			}
		}
		pBuf = buffer + idLen + blockSizeBufLen + blockSize;

		if (pBuf[0] == 0xB7)//cueTrackPosition
		{
			memset(blockId, 0, 10);
			memset(blockSizeBuf, 0, 10);
			idLen = 0;
			blockSizeBufLen = 0;

			if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, pBuf))  // B7 cueTrackPosition
			{
				printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
						   "mkv download parse root box type and size",
						   "download media file prase block id and size failed.\n",
						   	pMediaFileDownldLog);
				return false;
			}

			blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);
			pBuf = pBuf + idLen + blockSizeBufLen;
			if (!PraseCueTrackPosition(pBuf, blockSize))
			{
				printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
						   "mkv download parse root box type and size",
						   "download media file prase PraseCueTrackPosition failed.\n",
						   	pMediaFileDownldLog);
				return false;
			}
		}
	}
	else if (buffer[0] == 0xB7){  //cueTrackPosition
		memset(blockId, 0, 10);
		memset(blockSizeBuf, 0, 10);
		idLen = 0;
		blockSizeBufLen = 0;

		if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, buffer))  // B7 cueTrackPosition
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mkv download parse root box type and size",
					   "download media file prase block id and size failed.\n",
					   	pMediaFileDownldLog);
			return false;
		}

		blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);
		buffer = buffer + idLen + blockSizeBufLen;
		if (!PraseCueTrackPosition(buffer, blockSize))
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mkv download parse root box type and size",
					   "download media file prase PraseCueTrackPosition failed.\n",
					   	pMediaFileDownldLog);
			return false;
		}

	}


	return true;
}
bool MkvDownldMediaFile::GetDownloadOffset(long blockOffset, int64_t blockSize)
{
	_syncSampleCount = 0;
	unsigned char blockId[10] = { 0 };
	unsigned char blockSizeBuf[10] = { 0 };
	int idLen = 0;
	int blockSizeBufLen = 0;
	if (0 != lfseek(_pPlayerMediaFile, blockOffset, SEEK_SET))
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "fseek download file failed.\n",
				   	pMediaFileDownldLog);
		return false;
	}
	unsigned char * buffer = new unsigned char[blockSize + 1];
	memset(buffer, 0, blockSize + 1);
	if (blockSize != fread(buffer, 1, blockSize, _pPlayerMediaFile))
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "fread download file failed.\n",
				   	pMediaFileDownldLog);
		delete[] buffer;
		return false;
	}
	//����keyframe list
	long CUES_pos = 0;
	CUES_pos = blockOffset;
	long CUES_end = blockOffset + blockSize;
	unsigned char *p = buffer;
	while (CUES_pos != CUES_end)  
	{
		//Cues					1 [1C][53][BB][6B] - -- - m	 *	 *	 *	 *	 A top - level element to speed seeking access.All entries are local to the segment.Should be mandatory for non "live" streams.
		//CuePoint				2 [BB] * *	 --m	 *	 *	 *	 *	 Contains all information relative to a seek point in the segment.
		//CueTime				3 [B3] * -- - u	 *	 *	 *	 *	 Absolute timecode according to the segment time base.
		//CueTrackPositions	    3 [B7] * *	 --m	 *	 *	 *	 *	 Contain positions for different tracks corresponding to the timecode.
		//CueTrack				4 [F7] * -not 0 - u	 *	 *	 *	 *	 The track for which a position is given.
		//CueClusterPosition	4 [F1] * -- - u	 *	 *	 *	 *	 The position of the Cluster containing the required Block.
		//CueRelativePosition	4 [F0] - -- - u	  	  	  	  	 The relative position of the referenced block inside the cluster with 0 being the first possible position for an element inside that cluster.
		//CueDuration			4 [B2] - -- - u	  	  	  	  	 The duration of the block according to the segment time base.If missing the track's DefaultDuration does not apply and no duration information is available in terms of the cues.
		//CueBlockNumber		4 [53][78] - -not 0	 1	 u	 *	 *	 *	 *	 Number of the Block in the specified Cluster.
		//CueCodecState			4 [EA] - --0	 u	  	 *	 *	  	 The position of the Codec State corresponding to this Cue element. 0 means that the data is taken from the initial Track Entry.
		//CueReference			4 [DB] - *--m	  	 *	 *	  	 The Clusters containing the required referenced Blocks.
		//CueRefTime			5 [96]

		if (p[0] != 0xBB)
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mkv download parse root box type and size",
					   "parse block key frame info failed.\n",
					   	pMediaFileDownldLog);
			break;
		}
		if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, p))
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mkv download parse root box type and size",
					   " download media file prase block id and size failed.\n",
					   	pMediaFileDownldLog);
			return false;
		}

		blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);

		if (!PraseCuePoint(p, blockSize + idLen + blockSizeBufLen))
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mkv download parse root box type and size",
					   " download media file prase CuePoint block id and size failed.\n",
					   	pMediaFileDownldLog);
			return false;
		}
		p = blockSize + idLen + blockSizeBufLen + p;
		CUES_pos += blockSize + idLen + blockSizeBufLen;
		_syncSampleCount++;
	}
	delete[] buffer;
	_syncSampleCount--;

	#ifdef FEYFRAMELOG
	if(pkeyframeLog != NULL)
	{
		fclose(pkeyframeLog);
		pkeyframeLog = NULL;
	}
	#endif
	return true;

}
bool MkvDownldMediaFile::PraseRootBox()
{
	unsigned char blockId[10] = { 0 };
	unsigned char blockSizeBuf[10] = { 0 };
	int idLen = 0;
	int blockSizeBufLen = 0;
	unsigned char readBuf[100] = { 0 };
	_current = 0;

	_current = 0;
	if(0 != P2pDownloadMediaData(0,MEDIAFILEPARSELEN))
	{
		return false;
	}

	LimaoApi_getFilePath(_mediaFileHash, _playMediaFilePath);
	if(strlen(_playMediaFilePath) == 0)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "get p2p download media file path failed.\n",
				   	pMediaFileDownldLog);
		return false;
	}
	_pPlayerMediaFile = fopen(_playMediaFilePath,"rb");
	if(_pPlayerMediaFile == NULL)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "open the p2p download media file failed.\n",
				   	pMediaFileDownldLog);
		return false;
	}
	char logbuf[100] ={ 0 };
	_end = LimaoApi_getFileSize(_mediaFileHash);


	int len = fread(readBuf, 1, MEDIAFILEPARSELEN, _pPlayerMediaFile);  //EBML header
	if (MEDIAFILEPARSELEN != len)
	{
		printf("mp4 download media file prase root box falied\n");
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "fread the download file failed.",
				   	pMediaFileDownldLog);
		return false;
	}

	if(!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, readBuf))
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "parse block id and size failed.",
				   	pMediaFileDownldLog);
		return false;
	}

	int64_t blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);//TLV  ��ʽ  ����һ����ID  SIZE  DATA �������

	_current = blockSize + idLen + blockSizeBufLen + _current;

	_ebmlHeaderBlockSize = _current;


	if(0 != P2pDownloadMediaData(_current,MEDIAFILEPARSELEN))
	{
		return false;
	}

	if (0 != lfseek(_pPlayerMediaFile,(long) _current, SEEK_SET))
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "fseek the downlaod file failed.",
				   	pMediaFileDownldLog);
		return false;
	}

	len = fread(readBuf, 1, MEDIAFILEPARSELEN, _pPlayerMediaFile);  // segment
	if (len != MEDIAFILEPARSELEN)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "fread the download file failed.",
				   	pMediaFileDownldLog);
		return false;
	}
	if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, readBuf))
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "parse block id and size failed.",
				   	pMediaFileDownldLog);
		return false;
	}

	blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);
	_segmentBlockOffset = _current;
	_segmentBlockSize = blockSize + idLen + blockSizeBufLen;
	_current = blockSize + idLen + blockSizeBufLen +_current;  //TLV  ��ʽ  ����һ����ID  SIZE  DATA �������

	if (_current != _end)
	{
		printf("MKV downld media file is Special.\n");
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "MKV downld media file is Special",
				   	pMediaFileDownldLog);
		return false;
	}

	//��ʼ����segment�е��ӿ�
	long childBeginSeek= _segmentBlockOffset + idLen + blockSizeBufLen;
	_current = childBeginSeek;
	while (_current != _end)
	{
		memset(readBuf, 0, 100);
		memset(blockId, 0, 10);
		memset(blockSizeBuf, 0, 10);
		idLen = 0;
		blockSizeBufLen = 0;

		if(0 != P2pDownloadMediaData(_current,MEDIAFILEPARSELEN))
		{
			return false;
		}
		if (0 != lfseek(_pPlayerMediaFile, _current, SEEK_SET))
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mkv download parse root box type and size",
					   "fseek the downlaod file failed.<line 597>",
					   	pMediaFileDownldLog);
			return false;
		}
		len = fread(readBuf, 1, MEDIAFILEPARSELEN, _pPlayerMediaFile);  //EBML header
		if (len != MEDIAFILEPARSELEN)
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mkv download parse root box type and size",
					   "fread the download file failed.",
					   	pMediaFileDownldLog);
			return false;
		}
		if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, readBuf))
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mkv download parse root box type and size",
					   "parse block id and size failed.",
					   	pMediaFileDownldLog);
			return false;
		}
		blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);
		if (blockSize == 0)
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mkv download parse root box type and size",
					   "parse block size failed.",
					   	pMediaFileDownldLog);
			return false;
		}
		unsigned int boxType = MkvCheckBoxType(blockId);
		if (boxType == META_SEEK_INFO)  /*Meta Seek Information ��ʵ�Ǹ�������������Ϣ��
										�����ܰ���Track information, Chapters, Tags, Cues, Attachments, ��Щ���ֵ�λ����Ϣ��
										��Щλ������Segment�е����λ�á�Meta Seek�ⲿ�����ݱȲ�һ�������У�
										������������������ٵĶ�λ������ĵ�һЩ�ؼ���Ϣ��λ�ã������ð����ļ�˳��Ľ�����*/	
		{
			_metaSeekInfoBlockOffset = _current;
			_metaSeekInfoBlockSize = blockSize + idLen + blockSizeBufLen;
			//��ת��meta seek infomation ָ���ĵ�һ���λ�ã���������������ļ�   �߼��ø��Ӱ�
			unsigned int ID;
			int64_t offset;
			if (!PraseSeekInfoBlock(_metaSeekInfoBlockOffset + idLen + blockSizeBufLen, _metaSeekInfoBlockSize - idLen - blockSizeBufLen, ID, offset))
			{
				printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
						   "mkv download parse root box type and size",
						   "parse seek info block failed.",
						   	pMediaFileDownldLog);
				return false;
			}
			offset = offset + _metaSeekInfoBlockOffset;
			_current = _cueingDataBlockOffset;
			break;
		}
		
		switch (boxType)
		{
		case META_SEEK_INFO:  
			_metaSeekInfoBlockOffset = _current;
			_segmentInfoBlockSize = blockSize + idLen + blockSizeBufLen;
			break;
		case SEGMENT_INFO:
			_segmentInfoBlockOffset = _current;
			_segmentInfoBlockSize = blockSize + idLen + blockSizeBufLen;
			break;
		case TRACK:
			_trackBlockOffset = _current;//child block
			_trackBlockSize = blockSize + idLen + blockSizeBufLen;
			break;
		case CLUSTER:
			if (_firstClustersBlockOffset == 0)
			{
				_firstClustersBlockOffset = _current;

			}
			_clustersBlockOffset = _current;//child block the audio video data  //�кܶ�� 
			_clustersBlockSize = blockSize + idLen + blockSizeBufLen;
			_allClustersBlockSize += _clustersBlockSize;
			break;
		case CUES:


			_cueingDataBlockOffset = _current;//child block  Contain the key frame infomation
			_cueingDataBlockSize = blockSize + idLen + blockSizeBufLen;
			//if (!GetDownloadOffset(_cueingDataBlockOffset + idLen + blockSizeBufLen, blockSize))
			//{
			//	printf("MKV download media file get download offset (key frame offset) failed.\n");
			//	return false;
			//}
			//break;
		default:
			if (_otherBlockCount >= 9)
			{
				printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
						   "mkv download parse root box type and size",
						   "download media file prase block failed.",
						   	pMediaFileDownldLog);
				return false;
			}
			
			_otherBlcokOffset[_otherBlockCount] = _current;
			_otherBlockSize[_otherBlockCount] = blockSize + idLen + blockSizeBufLen;
			_otherBlockCount++;
		}

		_current = _current + blockSize + idLen + blockSizeBufLen;
	}
	return true;



}
bool MkvDownldMediaFile::PraseBlockIdAndSize(unsigned char * blockId, int &idLen, unsigned char * blockSize, int & blockSizeLen, unsigned char * buffer)
{
	CHECK_POINTER_RET(blockId,false);
	CHECK_POINTER_RET(blockSize,false);
	CHECK_POINTER_RET(buffer,false);
	idLen = 0;
	blockSizeLen = 0;
	int index = 0;
	uint8_t BitMask;
	BitMask = 1 << 7;
	while(1){
		blockId[idLen] = buffer[index++];
		if (++idLen > 4) {
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mkv download parse root box type and size",
					   "media file download  don't support element IDs over class D.",
					   	pMediaFileDownldLog);
			return false; // we don't support element IDs over class D

		}
		if (blockId[0] & BitMask) {
			break;
		}
		BitMask >>= 1;

	} 

	BitMask = 1 << 7;
	while (1){
		blockSize[blockSizeLen] = buffer[index++];
		if (++blockSizeLen > 8) {
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mkv download parse root box type and size",
					   "media file download  don't support element IDs over class D.",
					   	pMediaFileDownldLog);
			return false; // we don't support element IDs over class D

		}
		if (blockSize[0] & BitMask) {
			blockSize[0] = blockSize[0] & (~BitMask);
			break;
		}
		BitMask >>= 1;

	}
	return true;

}
int64_t MkvDownldMediaFile::GetBlockSize(unsigned char* blockSizeBuf, int blockSizeBufLen)
{
	int64_t size = 0;
	int index = 0;
	for (int i = 0; i < blockSizeBufLen; i++)
	{
		if (blockSizeBuf[i] != 0)
		{
			index = i;
			break;
		}
	}
	unsigned char * validbuf = blockSizeBuf + index;
	int len = blockSizeBufLen - index;
	unsigned char tmp;
	for (int i = 0; i < len; i++)
	{

		tmp = validbuf[i];
		validbuf[i] = validbuf[len - i -1];
		validbuf[len - i -1] = tmp;
		if (i >= len / 2 -1)
		{
			break;
		}

	}
	memcpy((char *)(&size), blockSizeBuf+index, blockSizeBufLen-index);
	return size;
}
unsigned int MkvDownldMediaFile::MkvCheckBoxType(unsigned char* blockId)
{
	CHECK_POINTER_RET(blockId, 0);
	unsigned int boxType = 0;
	unsigned char tmp;
	int len = sizeof(int);
	for (int i = 0; i < len; i++)
	{
		tmp = blockId[i];
		blockId[i] = blockId[len - i - 1];
		blockId[len - i - 1] = tmp;
		if (i >= len / 2 - 1)
		{
			break;
		}
	}
	memcpy(&boxType, blockId, sizeof(int));
	return boxType;
}
bool MkvDownldMediaFile::DownloadFileFirst()
{

	//int64_t _ebmlHeaderBlockSize; //root block
	int64_t fileSize = _end;
	if (_ebmlHeaderBlockSize == 0)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "download media file hava no mkvfile header.",
				   	pMediaFileDownldLog);
		return false;
	}
	if (_segmentBlockOffset != 0)
	{
		if (0 != P2pDownloadMediaData(0, 1*1024*1024))
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mkv download parse root box type and size",
					   "download media file header failed.",
					   	pMediaFileDownldLog);
			return false;
		}

	}
	


	if (_cueingDataBlockOffset != 0)
	{
		if (0 != P2pDownloadMediaData(_cueingDataBlockOffset, _end - _cueingDataBlockOffset))
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mkv download parse root box type and size",
					   "download media file key frame info data failed.",
					   	pMediaFileDownldLog);
			return false;
		}
	}


	unsigned char blockId[10] = { 0 };
	unsigned char blockSizeBuf[10] = { 0 };
	int idLen = 0;
	int blockSizeBufLen = 0;
	unsigned char readBuf[100] = { 0 };
	int64_t blockSize;
	memset(readBuf, 0, 100);
	memset(blockId, 0, 10);
	memset(blockSizeBuf, 0, 10);
	idLen = 0;
	blockSizeBufLen = 0;
	if (0 != lfseek(_pPlayerMediaFile, _cueingDataBlockOffset, SEEK_SET))
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "fseek downlaod file failed.",
				   	pMediaFileDownldLog);
		return false;
	}
	int len = fread(readBuf, 1, MEDIAFILEPARSELEN, _pPlayerMediaFile);  //EBML header
	if (len != MEDIAFILEPARSELEN)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "fread downlaod file failed.",
				   	pMediaFileDownldLog);
		return false;
	}
	if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, readBuf))
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "parse block id and size failed.",
				   	pMediaFileDownldLog);
		return false;
	}
	blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);
	if (blockSize == 0)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "parse block size failed.",
				   	pMediaFileDownldLog);
		return false;
	}
	unsigned int boxType = MkvCheckBoxType(blockId);
	if (boxType != CUES)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "DownloadFileFirst check box type is not CUSE",
				   	pMediaFileDownldLog);
		return false;
	}
	if (!GetDownloadOffset(_cueingDataBlockOffset + idLen + blockSizeBufLen, blockSize))
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "DownloadFileFirst get download offset failed.",
				   	pMediaFileDownldLog);
		return false;
	}

	if (_downloadBlockInfoList == NULL)
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv download parse root box type and size",
				   "DownloadFileFirst download block info invalid.",
				   	pMediaFileDownldLog);
		return false;
	}

	if (_downloadBlockInfoList->offset > 1 * 1024 * 1024)
	{
		if (0 != P2pDownloadMediaData(0, _downloadBlockInfoList->offset))
		{
			printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
					   "mkv download parse root box type and size",
					   "DownloadFileFirst download mvk file header data failed.",
					   	pMediaFileDownldLog);
			return false;
		}
	}
	return true;
}

int MkvDownldMediaFile::GetMdataBlockCount()
{
	return _syncSampleCount;
}
bool MkvDownldMediaFile::DownloadMdatBlock(int index)
{
	unsigned long startOffset = _downloadBlockInfoList[index].offset;
	unsigned long endOffset;
	if (index + 1 == _syncSampleCount)
	{
		endOffset =_cueingDataBlockOffset;
	}
	else
	{
		endOffset = _downloadBlockInfoList[index + 1].offset;
	}

	


	if (endOffset <= startOffset)
	{
		char buf[200] = {0};
		sprintf(buf,"DownloadFileFirst download mvk file header data failed index %d startoffset: %lu, endoffset: %lu",index,startOffset, endOffset);
		printf_log(pMediaFileDownldLog == NULL ? LOG_WARN : LOG_WARN|LOG_FILE,
				   "mkv DownloadMdatBlock",
				   buf,
				   	pMediaFileDownldLog);
		_downloadBlockInfoList[index].isDownload = true;
		return true;
	}
	long size = endOffset - startOffset;
	if(0 != P2pDownloadMediaData(startOffset, size))
	{

		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "mkv DownloadMdatBlock",
				   "DownloadFileFirst download mvk file header data failed.",
				   	pMediaFileDownldLog);
		return false;
	}
	_downloadBlockInfoList[index].isDownload = true;
	char log[300] = {0};
	sprintf(log," download the %d block , size is %ld,the endtime is %u\n", index, size, _downloadBlockInfoList[index].timeStamp);
	printf_log(pMediaFileDownldLog == NULL ? LOG_INFO : LOG_INFO|LOG_FILE,
				   "download the mkv file a block media data",
					log,
				   	pMediaFileDownldLog);
	printf(" download the %d block complte size is %ld,the endtime is %u\n", index, size, _downloadBlockInfoList[index].timeStamp);
	return true;
}
