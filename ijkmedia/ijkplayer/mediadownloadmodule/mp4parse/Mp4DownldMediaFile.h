#ifndef MP4WOWNLDMEDIAFILE_H_
#define MP4WOWNLDMEDIAFILE_H_
#include "../DownldMediaFile.h"
#include "../DataPrase.h"
#include "../DataType.h"
#include <stdio.h>
#define FTYP    1
#define FREE    2
#define MDAT    3
#define MOOV    4
#define SKIP 	5
#define UDTA    6
#define MOOF    7


class Mp4DownldMediaFile :public DownldMediaFile
{
private:
	int64_t _ftypBlockSize;
	long _freeBlockOffset[10];
	int64_t _freeBlockSize[10];
	int _freeBlockCount;
	long _moovBlockOffset;
	int64_t _moovBlockSize;
	long _mdatBlockOffset;
	int64_t _mdatBlockSize;
	int64_t _current;
	int64_t _end;
	DataPrase _dataPrase;
	

public:
	Mp4DownldMediaFile();
	~Mp4DownldMediaFile();

	int Mp4CheckBoxType(char* buf);
	bool PraseRootBox();
	bool GetDownloadOffset();
	bool DownloadFileFirst();
	bool StartDownloadMediaData();
	bool DownloadMdatBlock(int index);
	int GetMdataBlockCount();
	bool CheckMdataBlcokDownload(int index);
	int Finish();
};
#endif
