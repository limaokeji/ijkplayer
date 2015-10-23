#ifndef RMVBWOWNLDMEDIAFILE_H_
#define RMVBWOWNLDMEDIAFILE_H_
#include "../DownldMediaFile.h"
#include "../DataPrase.h"
#include "RmvbPrase.h"
#define stricmp strcasecmp
#define PROP    1
#define CONT    2
#define MDPR    3
#define DATA	4
#define INDX	5

class RmvbDownldMediaFile :public DownldMediaFile
{
private:
	int64_t _propBlockSize;

	long    _contBlockOffset;
	int64_t _contBlockSize;

	long	_mdprBlockOffset[10];
	int64_t _mdprBlockSize[10];
	int _mdprBlockCount;

	long _dataBlockOffset[10];
	int64_t _dataBlockSize[10];
	int _dataBlockCount;


	long _indxBlockOffset[10];
	int64_t _indxBlockSize[10];
	int _indxBlockCount;

	int64_t _current;
	int64_t _end;
	DataPrase _dataPrase;
public:
	RmvbDownldMediaFile();
	~RmvbDownldMediaFile();
	bool PraseRootBox();
	int RmvbCheckBoxType(char* buf);
	bool DownloadFileFirst();
	int GetMdataBlockCount();
	bool DownloadMdatBlock(int index);
	RmvbPrase * _pRmvbPrase;
};
#endif
