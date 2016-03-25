#include "Stss.h"


CStss::CStss()
{
	_keySampleIdList = NULL;
}


CStss::~CStss()
{
	if (_keySampleIdList != NULL)
	{
		delete[] _keySampleIdList;
		_keySampleIdList = NULL;
	}
}

int CStss::Parse()
{
	if (_pFile == NULL)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	if (_pos <= 0)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}

	if (lfseek(_pFile, _pos, SEEK_SET) < 0)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	memset(_readBuf, 0, READLEN);
	if (fread(_readBuf, 1, READLEN, _pFile) != READLEN)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	_size = _dataParse.DataToBigEndianInt32(_readBuf);
	if (_size <= 8)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	_count = _dataParse.DataToBigEndianInt32(_readBuf + 12);
	if (_count <= 0)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	_keySampleIdList = new uint32_t[_count];
	memset(_keySampleIdList, 0, sizeof(uint32_t)*_count);

	_pos = _pos + 16;

	for (int i = 0; i < _count; i++)
	{
		memset(_readBuf, 0, 5);
		if (lfseek(_pFile, _pos + i*4, SEEK_SET) < 0)
		{
			 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
			return -1;
		}
		if (fread(_readBuf, 1, 5, _pFile) != 5)
		{
			 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
			return -1;
		}
		*(_keySampleIdList+i) = _dataParse.DataToBigEndianInt32(_readBuf);
		if (*(_keySampleIdList + i) <= 0)
		{
			 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
			return -1;
		}
	}

	return 0;
}

