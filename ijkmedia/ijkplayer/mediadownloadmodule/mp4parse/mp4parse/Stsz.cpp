#include "Stsz.h"


CStsz::CStsz()
{
	_sampleSize = 0 ;  //if != 0  all sample is this same Size
	_sampleSizeList = 0;
	_count = 0;
}


CStsz::~CStsz()
{
	if(_sampleSizeList != 0)
	{
		delete[] _sampleSizeList;
		_sampleSizeList =NULL;
	}
}


int CStsz::Parse()
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
	_sampleSize = _dataParse.DataToBigEndianInt32(_readBuf + 12);//ȫ��sample����Ŀ��������е�sample����ͬ�ĳ��ȣ�����ֶξ������ֵ����������ֶε�ֵ����0��
	//if (_count <= 0)
	//{
	//	return -1;
	//}

	_count = _dataParse.DataToBigEndianInt32(_readBuf + 16);
	if (_count <= 0)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}

	_sampleSizeList = new uint32_t[_count];
	memset(_sampleSizeList, 0, sizeof(uint32_t) * _count);
	_pos = _pos + 20;

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
		*(_sampleSizeList + i) = _dataParse.DataToBigEndianInt32(_readBuf);
		if (*(_sampleSizeList + i) <= 0)
		{
			 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
			return -1;
		}
	}
	return 0;

}

uint32_t CStsz::GetSampleSize(uint32_t sampleId)
{
	if (sampleId > _count)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return 0;
	}
	if (_sampleSize != 0)
	{
		return _sampleSize;
	}
	else
	{
		return *(_sampleSizeList + sampleId);
	}
	return 0;
}
