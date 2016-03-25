#include "Stco.h"


CStco::CStco()
{
	_count = 0;
	_chunkOffsetList = NULL;
}


CStco::~CStco()
{
	if (_chunkOffsetList != NULL)
	{
		delete[] _chunkOffsetList;
		_chunkOffsetList = NULL;
	}
}

int CStco::Parse()
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

	_chunkOffsetList = new uint32_t[_count];
	memset(_chunkOffsetList, 0, sizeof(uint32_t)*_count);

	_pos = _pos + 16;

	for (int i = 0; i < _count; i++)
	{
		memset(_readBuf, 0, 5);
		if (lfseek(_pFile, _pos + i * 4, SEEK_SET) < 0)
		{
			 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
			return -1;
		}
		if (fread(_readBuf, 1, 5, _pFile) != 5)
		{
			 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
			return -1;
		}
		*(_chunkOffsetList + i) = _dataParse.DataToBigEndianInt32(_readBuf);
		if (*(_chunkOffsetList + i) <= 0)
		{
			 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
			return -1;
		}
	}
	return 0;
}
uint32_t CStco::GetChunkOffset(uint32_t chunkId)
{
	if (chunkId > _count)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return 0;
	}

	return *(_chunkOffsetList + chunkId);
}
