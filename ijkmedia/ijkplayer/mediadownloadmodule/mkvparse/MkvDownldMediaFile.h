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

	long _segmentBlockOffset;	//root block
	int64_t _segmentBlockSize;

	long _metaSeekInfoBlockOffset; //child block
	int64_t _metaSeekInfoBlockSize;

	long _segmentInfoBlockOffset;//child block
	int64_t _segmentInfoBlockSize;

	long _trackBlockOffset;//child block
	int64_t _trackBlockSize;

	long _chaptersBlockOffset;//child block
	int64_t _chaptersBlockSize;

	long _clustersBlockOffset;//child block the audio video data
	int64_t _clustersBlockSize;
	long _firstClustersBlockOffset;
	int64_t _allClustersBlockSize;

	long _cueingDataBlockOffset;//child block  Contain the key frame infomation
	int64_t _cueingDataBlockSize; 

	long _attachmentBlockOffset;//child block
	int64_t _attachmentBlockSize;

	long _taggingBlockOffset;//child block
	int64_t _taggingBlockSize;

	int _otherBlockCount;
	long _otherBlcokOffset[10];
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
};
#endif
