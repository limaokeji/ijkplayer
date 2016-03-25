#include "Stsc.h"


CStsc::CStsc()
{
	_sampleToTunck = NULL;
	_count = 0;
}


CStsc::~CStsc()
{
	if (_sampleToTunck != NULL)
	{
		delete[] _sampleToTunck;
		_sampleToTunck = NULL;
	}
}

int CStsc::Parse()
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

	_sampleToTunck = new SAMPLETUNCK[_count];
	memset(_sampleToTunck, 0, sizeof(SAMPLETUNCK)*_count);

	_pos = _pos + 16;
	for (int i = 0; i < _count; i++)
	{
		memset(_readBuf, 0, 13);
		if (lfseek(_pFile, _pos + i * 12, SEEK_SET) < 0)
		{
			 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
			return -1;
		}
		if (fread(_readBuf, 1, 13, _pFile) != 13)
		{
			 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
			return -1;
		}
		(_sampleToTunck + i)->firstTunck = _dataParse.DataToBigEndianInt32(_readBuf);
		(_sampleToTunck + i)->samplePerChunk = _dataParse.DataToBigEndianInt32(_readBuf+4);
		(_sampleToTunck + i)->sampleDescriptionId = _dataParse.DataToBigEndianInt32(_readBuf + 8);
		if ((_sampleToTunck + i)->samplePerChunk <= 0)
		{
			 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
			return -1;
		}
	}

	return 0;
}

int CStsc::GetChunkIdBySampleId(uint32_t sampleId, /*out*/ uint32_t* chunkId, /*out*/ uint32_t * chunkPos)
{
	if (sampleId <= 0)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	if ((chunkId == NULL) || (chunkPos == NULL))
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	uint32_t tmpId = 0;
	uint32_t tmpchunkId = 0;
	int tmpChunkCount = 0;

	for (int i = 0; i < _count; i++)
	{

		if (i == _count - 1)
		{
			tmpId = tmpId + (_sampleToTunck + i)->samplePerChunk;
			*chunkId = i;
			*chunkPos = tmpId - sampleId;
			break;
		}
		tmpChunkCount = (_sampleToTunck + i + 1)->firstTunck - (_sampleToTunck + i)->firstTunck;
		tmpId = tmpId + tmpChunkCount * (_sampleToTunck + i)->samplePerChunk;
		if (tmpId >= sampleId)
		{
			*chunkId = tmpChunkCount - (tmpId - sampleId) / (_sampleToTunck + i)->samplePerChunk - 1 + (_sampleToTunck + i)->firstTunck;
			*chunkPos = (_sampleToTunck + i)->samplePerChunk - (tmpId - sampleId) % (_sampleToTunck + i)->samplePerChunk;
			break;
		}
		//else if (tmpId == sampleId)
		//{
		//	*chunkId = tmpChunkCount - (tmpId - sampleId) / (_sampleToTunck + i)->samplePerChunk - 1 + (_sampleToTunck + i)->firstTunck;
		//	*chunkPos = (_sampleToTunck + i)->samplePerChunk - (tmpId - sampleId) % (_sampleToTunck + i)->samplePerChunk;
		//	break;
		//}

	}

	return 0;
}
