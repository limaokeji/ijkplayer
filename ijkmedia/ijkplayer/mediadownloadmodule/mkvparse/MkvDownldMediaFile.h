#ifndef MKVWOWNLDMEDIAFILE_H_
#define MKVWOWNLDMEDIAFILE_H_
#include "../DataPrase.h"
#include "../DownldMediaFile.h"

#define  META_SEEK_INFO  0x114D9B74
#define  SEGMENT_INFO    0x1549A966
#define  TRACK			 0x1654AE6B
#define  CLUSTER		 0x1F43B675
#define  CUES            0x1C53BB6B 


class MkvDownldMediaFile :public DownldMediaFile
{
private:
	int64_t _ebmlHeaderBlockSize; //root block

	int64_t _segmentBlockOffset;	//root block
	int64_t _segmentBlockSize;

	int64_t _metaSeekInfoBlockOffset; //child block
	int64_t _metaSeekInfoBlockSize;

	int64_t _segmentInfoBlockOffset;//child block
	int64_t _segmentInfoBlockSize;

	int64_t _trackBlockOffset;//child block
	int64_t _trackBlockSize;

	int64_t _chaptersBlockOffset;//child block
	int64_t _chaptersBlockSize;

	int64_t _clustersBlockOffset;//child block the audio video data
	int64_t _clustersBlockSize;
	int64_t _firstClustersBlockOffset;
	int64_t _allClustersBlockSize;

	int64_t _cueingDataBlockOffset;//child block  Contain the key frame infomation
	int64_t _cueingDataBlockSize; 

	int64_t _attachmentBlockOffset;//child block
	int64_t _attachmentBlockSize;

	int64_t _taggingBlockOffset;//child block
	int64_t _taggingBlockSize;

	int _otherBlockCount;
	int64_t _otherBlcokOffset[10];
	int64_t _otherBlockSize[10];

	int64_t _current;
	int64_t _end;

	DataPrase _dataPrase;

	int _syncSampleCount;
	bool PraseCuePoint(unsigned char * buffer, int len);
	bool PraseCueTrackPosition(unsigned char * buffer, int len);
	bool PraseBlockIdAndSize(unsigned char * blockId, int &idLen, unsigned char * blockSize, int & blockSizeLen, unsigned char * buffer);
	int64_t GetBlockSize(unsigned char* blockSizeBuf, int blockSizeBufLen);
	unsigned int MkvCheckBoxType(unsigned char* blockId);
	bool PraseSeekInfoBlock(long offset,int64_t size, unsigned int& ID, int64_t& blockSize);

#ifdef FEYFRAMELOG
	FILE * pkeyframeLog;
#endif
public:
	MkvDownldMediaFile();
	~MkvDownldMediaFile();

	int MkvCheckBoxType(char* buf);
	bool PraseRootBox();
	bool GetDownloadOffset(long blockOffset, int64_t blockSize);
	bool DownloadFileFirst();
	bool StartDownloadMediaData();
	bool DownloadMdatBlock(int index);
	int GetMdataBlockCount();
	bool CheckMdataBlcokDownload(int index);
	int Finish();
};
#endif
