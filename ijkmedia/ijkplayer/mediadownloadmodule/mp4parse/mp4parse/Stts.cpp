#include "Stts.h"


CStts::CStts()
{
	_count = 0;
	_sampleCount = 0;
	_sampleDuration = 0;
}


CStts::~CStts()
{
}

int CStts::Parse()
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
	_sampleCount = _dataParse.DataToBigEndianInt32(_readBuf + 16);
	if (_sampleCount <= 0)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	_sampleDuration = _dataParse.DataToBigEndianInt32(_readBuf + 20);
	if (_sampleDuration <= 0)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}

	return 0;
}
