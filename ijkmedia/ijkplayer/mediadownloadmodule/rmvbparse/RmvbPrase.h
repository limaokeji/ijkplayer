#ifndef RMVBPRASE_H_
#define RMVBPRASE_H_
#include <stdio.h>
#include "../DataType.h"
class RmvbPrase
{
private:
	void HandlePropChunk(unsigned int size);
	void HandleMdprChunk(unsigned int size);
	void HandleContChunk(unsigned int size);
	int HandleDataChunk(unsigned int size);
	int HandleIndxChunk(unsigned int size, short * pstream_number);
	void HandleSubDataChunk();
	void HandleIndxRecord(INDEXRECORD* pIndexRecord);
	short readshort(char* name);
	int readint(char* name);
	char readchar(unsigned int size, char* name, bool bchar = true);
	void HandleChunk(char* tagID, unsigned int size);
	FILE * _pFile;
	char * _rmvbFileName;
	INDEXRECORD* _pIndexRecord;
public:
	RmvbPrase();
	~RmvbPrase();
	RmvbPrase(char * filename);
	RmvbPrase(FILE * pFIle);
	int _indexRecordCount;
	int _duration;
	bool GetKeyFrameInfo(INDEXRECORD** pIndexRecord);
	short _VideoStreamNumber;
	short _StreamNumber;
};
#endif
