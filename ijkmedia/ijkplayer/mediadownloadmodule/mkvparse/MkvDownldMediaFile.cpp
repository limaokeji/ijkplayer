#ifdef _WIN32
#include "stdafx.h"
#endif
#include "MkvDownldMediaFile.h"
#include <stdlib.h>
#include "../DataType.h"
#include "../mediafile_download_log.h"
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
	memset(_otherBlcokOffset,0,sizeof(long));
	memset(_otherBlockSize,0,sizeof(int64_t));

	_syncSampleCount = 0;
	_downloadBlockInfoList = (DOWNLOADBLOCKINFO *)malloc(sizeof(DOWNLOADBLOCKINFO)* 101);
	memset(_downloadBlockInfoList, 0, sizeof(DOWNLOADBLOCKINFO)* 101);
}
MkvDownldMediaFile::~MkvDownldMediaFile()
{
	if (_downloadBlockInfoList != NULL)
	{
		free(_downloadBlockInfoList);
		_downloadBlockInfoList = NULL;
	}
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

	if (0 != fseek(_pInFile, offset, SEEK_SET))
	{
		printf("MKV downld media file fseek failed.\n");
		return false;
	}
	unsigned char * buffer = new unsigned char[size + 1];
	memset(buffer, 0, size + 1);
	if (size != fread(buffer, 1, size, _pInFile))
	{
		printf("MKV downld media file fread failed.\n");
		delete[] buffer;
		return false;
	}
	if ((buffer[0] != 0x4d) || (buffer[1] != 0xbb))
	{
		printf("MKV downld media file Seek Info Block invalid.\n");
		return false;
	}
	if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, buffer))  //Seek 
	{
		printf("MKV download media file prase block id size failed.\n");
		return false;
	}
	unsigned char *firstBlockSeekInfo = buffer + idLen + blockSizeBufLen;
	memset(blockId, 0, 10);
	memset(blockSizeBuf, 0, 10);
	idLen = 0;
	blockSizeBufLen = 0;
	if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, firstBlockSeekInfo)) //seek id   53 AB
	{
		printf("MKV download media file prase block id size failed.\n");
		return false;
	}
	if ((blockId[0] != 0x53) || (blockId[1] != 0xAB))
	{
		printf("MKV download media file prase block id size failed.\n");
		return false;
	}
	memset(blockId, 0, 10);
	memcpy(blockId, firstBlockSeekInfo + idLen + blockSizeBufLen, 4);
	ID = MkvCheckBoxType(blockId);


	firstBlockSeekInfo = firstBlockSeekInfo + idLen + blockSizeBufLen + 4;
	memset(blockId, 0, 10);
	memset(blockSizeBuf, 0, 10);
	idLen = 0;
	blockSizeBufLen = 0;
	if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, firstBlockSeekInfo)) // seek postion 53 AC
	{
		printf("MKV download media file prase block id size failed.\n");
		return false;
	}
	int dataSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);
	if (dataSize <= 0)
	{
		printf("MKV download media file prase block id size failed.\n");
		return false;
	}
	unsigned char data[10] = { 0 };

	memcpy(data, firstBlockSeekInfo + idLen + blockSizeBufLen, dataSize);

	blockSize = GetBlockSize(data, dataSize);

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
				printf("MKV download media file prase block id size failed.\n");
				return false;
			}

			blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);

			int64_t curTrack = GetBlockSize(buffer + index + idLen + blockSizeBufLen, blockSize);

			printf("curTrack is %llu\n", curTrack);
		}
		else if (buffer[index] == 0xF1)//CueClusterPosition
		{
			if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, buffer + index))
			{
				printf("MKV download media file prase block id size failed.\n");
				return false;
			}

			blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);

			int64_t CueClusterPosition = GetBlockSize(buffer + index + idLen + blockSizeBufLen, blockSize);

			_downloadBlockInfoList[_syncSampleCount].offset = CueClusterPosition;
			printf("CueClusterPosition is %llu\n", CueClusterPosition + _segmentBlockOffset);
		}
		else if (buffer[index] == 0xF0)//CueRelativePosition
		{
			if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, buffer + index))
			{
				printf("MKV download media file prase block id size failed.\n");
				return false;
			}

			blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);

			int64_t CueRelativePosition = GetBlockSize(buffer + index + idLen + blockSizeBufLen, blockSize);

			printf("CueRelativePosition is %llu\n", CueRelativePosition);
		}
		else  if (buffer[index] == 0xB2)//CueDuration	
		{
			if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, buffer + index))
			{
				printf("MKV download media file prase block id size failed.\n");
				return false;
			}

			blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);

			int64_t CueDuration = GetBlockSize(buffer + index + idLen + blockSizeBufLen, blockSize);

			printf("CueDuration is %llu\n", CueDuration);
		}
		else if ((buffer[index] == 0x53) && (buffer[index + 1] == 0x78))//CueBlockNumber
		{
			if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, buffer + index))
			{
				printf("MKV download media file prase block id size failed.\n");
				return false;
			}

			blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);

			int64_t CueBlockNumber = GetBlockSize(buffer + index + idLen + blockSizeBufLen, blockSize);

			printf("CueBlockNumber is %llu\n", CueBlockNumber);
		}
		else if (buffer[index] == 0xEA)//CueCodecState	
		{
			if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, buffer + index))
			{
				printf("MKV download media file prase block id size failed.\n");
				return false;
			}

			blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);

			int64_t CueCodecState = GetBlockSize(buffer + index + idLen + blockSizeBufLen, blockSize);

			printf("CueCodecState is %llu\n", CueCodecState);
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
		printf("MKV downld media file prase CuePoint failed.\n");
		return false;
	}
	if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, buffer))  //BB CuePoint
	{
		printf("MKV download media file prase block id size failed.\n");
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
			printf("MKV download media file prase block id size failed.\n");
			return false;
		}

		blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);

		int64_t timestamp = GetBlockSize(buffer + idLen + blockSizeBufLen, blockSize);
		printf(" %d CueTime size is %u, timestamp is %llu\n", _syncSampleCount, blockSize, timestamp);
		_downloadBlockInfoList[_syncSampleCount].timeStamp = timestamp;
		_downloadBlockInfoList[_syncSampleCount].isDownload = false;
		//_downloadBlockInfoList[_syncSampleCount].isRequest = false;
		_downloadBlockInfoList[_syncSampleCount].smapleId = _syncSampleCount;
		if ((_syncSampleCount % 100 == 0) && (_syncSampleCount >=100))
		{
			_downloadBlockInfoList = (DOWNLOADBLOCKINFO*)realloc(_downloadBlockInfoList, sizeof(DOWNLOADBLOCKINFO)* (_syncSampleCount + 102));
			if (_downloadBlockInfoList == NULL)
			{
				printf("MKV download media file realloc blockinfoList memory failed.\n");
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
				printf("MKV download media file prase block id size failed.\n");
				return false;
			}

			blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);
			pBuf = pBuf + idLen + blockSizeBufLen;
			if (!PraseCueTrackPosition(pBuf, blockSize))
			{
				printf("MKV media file down prace cue track position failed.\n");
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
			printf("MKV download media file prase block id size failed.\n");
			return false;
		}

		blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);
		buffer = buffer + idLen + blockSizeBufLen;
		if (!PraseCueTrackPosition(buffer, blockSize))
		{
			printf("MKV media file down prace cue track position failed.\n");
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
	if (0 != fseek(_pInFile, blockOffset, SEEK_SET))
	{
		printf("MKV downld media file fseek failed.\n");
		return false;
	}
	unsigned char * buffer = new unsigned char[blockSize + 1];
	memset(buffer, 0, blockSize + 1);
	if (blockSize != fread(buffer, 1, blockSize, _pInFile))
	{
		printf("MKV downld media file fread failed.\n");
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
			printf("waring prase block key frame info.\n");
			break;
		}
		if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, p))
		{
			printf("MKV download media file prase block id size failed.\n");
			return false;
		}

		blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);

		if (!PraseCuePoint(p, blockSize + idLen + blockSizeBufLen))
		{
			printf("MKV download media file prase block id size failed.\n");
			return false;
		}
		p = blockSize + idLen + blockSizeBufLen + p;
		CUES_pos += blockSize + idLen + blockSizeBufLen;
		_syncSampleCount++;
	}
	delete[] buffer;
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
	if (0 != fseek(_pInFile, 0, SEEK_END))
	{
		printf("MKV downld media file fseek failed.\n");
		return false;
	}
	_end = ftell(_pInFile);
	if (0 != fseek(_pInFile, 0, SEEK_SET))
	{
		printf("MKV downld media file fseek failed.\n");
		return false;
	}
	int len = fread(readBuf, 1, 100, _pInFile);  //EBML header
	if (len != 100)
	{
		printf("mp4 download media file prase root box falied\n");
		return false;
	}

	if(!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, readBuf))
	{
		printf("MKV download media file prase block id size failed.\n");
		return false;
	}

	int64_t blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);//TLV  ��ʽ  ����һ����ID  SIZE  DATA �������

	_current = blockSize + idLen + blockSizeBufLen + _current;

	_ebmlHeaderBlockSize = _current;

	if (0 != fseek(_pInFile, _current, SEEK_SET))
	{
		printf("MKV downld media file fseek failed.\n");
		return false;
	}

	len = fread(readBuf, 1, 100, _pInFile);  // segment 
	if (len != 100)
	{
		printf("mp4 download media file prase root box falied\n");
		return false;
	}
	if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, readBuf))
	{
		printf("MKV download media file prase block id size failed.\n");
		return false;
	}

	blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);
	_segmentBlockOffset = _current;
	_segmentBlockSize = blockSize + idLen + blockSizeBufLen;
	_current = blockSize + idLen + blockSizeBufLen +_current;  //TLV  ��ʽ  ����һ����ID  SIZE  DATA �������

	if (_current != _end)
	{
		printf("MKV downld media file is Special.\n");
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
		if (0 != fseek(_pInFile, _current, SEEK_SET))
		{
			printf("MKV downld media file fseek failed.\n");
			return false;
		}
		len = fread(readBuf, 1, 100, _pInFile);  //EBML header
		if (len != 100)
		{
			printf("mp4 download media file fread failed.\n");
			return false;
		}
		if (!PraseBlockIdAndSize(blockId, idLen, blockSizeBuf, blockSizeBufLen, readBuf))
		{
			printf("MKV download media file prase block id size failed.\n");
			return false;
		}
		blockSize = GetBlockSize(blockSizeBuf, blockSizeBufLen);
		if (blockSize == 0)
		{
			printf("MKV download media file block size is invaild.\n");
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
				printf("prase seek info block failed.\n");
				return false;
			}
			offset = offset + _metaSeekInfoBlockOffset;
			_current = offset;
			continue;
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
			if (!GetDownloadOffset(_cueingDataBlockOffset + idLen + blockSizeBufLen, blockSize))
			{
				printf("MKV download media file get download offset (key frame offset) failed.\n");
				return false;
			}
			break;
		default:
			if (_otherBlockCount >= 9)
			{
				printf("MKV download media file prase block failed.\n");
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
			printf("MKV media file download  don't support element IDs over class D\n");
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
			printf("MKV media file download  don't support element IDs over class D\n");
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

	unsigned int tmrp = META_SEEK_INFO;
	return boxType;
}
bool MkvDownldMediaFile::DownloadFileFirst()
{
	char writeBuf[1024] = { 0 };
	CHECK_POINTER_RET(_pOutFile, false);

	//int64_t _ebmlHeaderBlockSize; //root block

	if (_ebmlHeaderBlockSize == 0)
	{
		printf("MKV download media file hava no mkvfile header.\n");
		return false;
	}
	if (_segmentInfoBlockOffset != 0)
	{
		if (0 != P2pDownloadMediaData(0, _segmentInfoBlockOffset))
		{
			return false;
		}

	}
	else if (_segmentInfoBlockOffset != 0)
	{
		if (0 !=P2pDownloadMediaData(0, _segmentInfoBlockOffset))
		{
			return false;
		}
	}
	else
	{
		if (0 != P2pDownloadMediaData(0, 100*1024))
		{
			return false;
		}
	}


	if ((_segmentBlockOffset == 0) || (_segmentBlockSize == 0))
	{
		printf("MKV download media file hava no segment block.\n");
		return false;
	}


	//long _metaSeekInfoBlockOffset; //child block
	//int64_t _metaSeekInfoBlockSize;
	if ((_metaSeekInfoBlockOffset != 0) && (_metaSeekInfoBlockSize != 0))
	{
		if (0 != P2pDownloadMediaData(_metaSeekInfoBlockOffset, _metaSeekInfoBlockSize))
		{
			return false;
		}
	}


	//long _segmentInfoBlockOffset;//child block
	//int64_t _segmentInfoBlockSize;
	if ((_segmentInfoBlockOffset != 0) && (_segmentInfoBlockSize != 0))
	{
		if (0 != P2pDownloadMediaData(_segmentInfoBlockOffset, _segmentInfoBlockSize))
		{
			return false;
		}
	}

	//long _trackBlockOffset;//child block
	//int64_t _trackBlockSize;
	if ((_trackBlockOffset != 0) && (_trackBlockSize != 0))
	{
		if (0 != P2pDownloadMediaData(_trackBlockOffset, _trackBlockSize))
		{
			return false;
		}
	}

	//long _chaptersBlockOffset;//child block
	//int64_t _chaptersBlockSize;
	if ((_chaptersBlockOffset != 0) && (_chaptersBlockSize != 0))
	{
		if (0 != P2pDownloadMediaData(_chaptersBlockOffset, _chaptersBlockSize))
		{
			return false;
		}
	}


	//long _clustersBlockOffset;//child block the audio video data
	//int64_t _clustersBlockSize;
	//long _firstClustersBlockOffset;
	//int64_t _allClustersBlockSize;
	//if ((_firstClustersBlockOffset != 0) && (_allClustersBlockSize != 0))
	//{
	//	if (!CopyFileData(_firstClustersBlockOffset, _allClustersBlockSize))
	//	{
	//		return false;
	//	}
	//}


	//long _cueingDataBlockOffset;//child block  Contain the key frame infomation
	//int64_t _cueingDataBlockSize;
	if ((_cueingDataBlockOffset != 0) && (_cueingDataBlockSize != 0))
	{
		if (0 != P2pDownloadMediaData(_cueingDataBlockOffset, _cueingDataBlockSize))
		{
			return false;
		}
	}


	//long _attachmentBlockOffset;//child block
	//int64_t _attachmentBlockSize;
	if ((_attachmentBlockOffset != 0) && (_attachmentBlockSize != 0))
	{
		if (0 != P2pDownloadMediaData(_attachmentBlockOffset, _attachmentBlockSize))
		{
			return false;
		}
	}


	//long _taggingBlockOffset;//child block
	//int64_t _taggingBlockSize;
	if ((_taggingBlockOffset != 0) && (_taggingBlockSize != 0))
	{
		if (0 != P2pDownloadMediaData(_taggingBlockOffset, _taggingBlockSize))
		{
			return false;
		}
	}

	for (int i = 0; i < _otherBlockCount; i++)
	{
		if ((_otherBlcokOffset[i] == 0) || (_otherBlockSize[i] == 0))
			continue;
		if (0 != P2pDownloadMediaData(_otherBlcokOffset[i], _otherBlockSize[i]))
		{
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
		endOffset = _allClustersBlockSize + _clustersBlockOffset - startOffset;
	}
	else
	{
		endOffset = _downloadBlockInfoList[index + 1].offset;
	}

	


	if (endOffset <= startOffset)
	{
		printf("mp4 download media file offset if invalid\n");
		return false;
	}
	long size = endOffset - startOffset;
	if( 0 != P2pDownloadMediaData(startOffset, size))
	{
		printf_log(pMediaFileDownldLog == NULL ? LOG_ERROR : LOG_ERROR|LOG_FILE,
				   "download the mp4 file a block media data",
				   "the p2p module downlaod failed.\n",
				   	pMediaFileDownldLog);
	}
	_downloadBlockInfoList[index].isDownload = true;

	printf(" download the %d block complte size is %ld,the endtime is %u\n", index, size, _downloadBlockInfoList[index].timeStamp);
	return true;
}
