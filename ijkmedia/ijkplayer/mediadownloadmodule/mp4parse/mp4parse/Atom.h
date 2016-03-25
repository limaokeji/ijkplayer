#pragma once
#include "../../DataPrase.h"
#include "../../largefile.h"
#include <string.h>
#include <android/log.h>
#define		READLEN		100
class CAtom
{
public:
	CAtom();
	~CAtom();
	int Init(FILE * pFile, int64_t pos);
	unsigned int  _size;
	unsigned int  _type; //atom type    moov/mdat/....
	int _version;
	FILE * _pFile;
	int64_t _pos;
	DataPrase _dataParse;
	unsigned char _readBuf[100];
};

