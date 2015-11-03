#ifndef WOWNLDMEDIAFILE_H_
#define WOWNLDMEDIAFILE_H_
#define FILENAMELEN 256
#define BUFFERSIZE 1024
#define P2PDOWNLOADTIMEOUT  1000
#define MEDIAFILEPARSELEN  100
#include <stdio.h>
#include "DataType.h"

class DownldMediaFile
{


public:
	//bool CopyFileData(long offset,  int64_t blockSize);
	int P2pDownloadMediaData(long offset, int64_t blockSize);
	DownldMediaFile();
	~DownldMediaFile();
	bool Init(char * inFileName,char * suffix_name, char * outFileName,uint64_t filesize,
			FILE* plog_file);
	virtual bool PraseRootBox(){return false;}
	virtual bool DownloadFileFirst(){return false;}
	virtual bool DownloadMdatBlock(int index){return false;}
	virtual int GetMdataBlockCount(){return 0;}
	bool IsDownloadMdatBlock(int index);
	DOWNLOADBLOCKINFO * GetDownloadBlockInfoList();
	char * _playMediaFilePath;
	char * _mediaFileHash;
	FILE* _pPlayerMediaFile;
	char * _buffer;
	char * GetP2pDownloadFileNameHash();
	char * GetPlayFileNamePath();
	FILE * pMediaFileDownldLog;
	int64_t _medieFileSize;
	DOWNLOADBLOCKINFO * _downloadBlockInfoList;
	int _syncSampleCount;
	uint64_t _duration;

};
#endif
