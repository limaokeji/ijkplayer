#include "Mp4Parse.h"
#include <string.h>
#include "../../DataType.h"
#include <android/log.h>
CMp4Parse::CMp4Parse()
{
	_pFile = NULL;
}


CMp4Parse::~CMp4Parse()
{
}
unsigned int CMp4Parse::GetAtomType(int64_t pos, uint32_t *pSize)
{
	if ((_pFile == NULL) || (pSize == NULL))
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return 0;
	}
	if (lfseek(_pFile, pos, SEEK_SET) < 0)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return 0;
	}
	unsigned char readBuf[100] = { 0 };
	int len = fread(readBuf, 1, 8, _pFile);
	if (len != 8)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return 0;
	}
	uint32_t blockSize = _dataParse.DataToBigEndianInt32(readBuf);
	if (blockSize <= 0)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return 0;
	}
	*pSize = blockSize;
	uint32_t ret = _dataParse.DataToBigEndianInt32(readBuf+4);

	return ret;
}
int CMp4Parse::Init(char * filePath, FILE * pFile)
{
	if ((filePath == NULL) && (pFile == NULL))
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	if (filePath != NULL)
	{
		_pFile = fopen(filePath, "rb");
		if (_pFile == NULL)
		{
			 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
			return -1;
		}
	}
	else
	{
		_pFile = pFile;
	}

	uint32_t pos = FindMoovAtom();
	if (pos <= 0)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	uint32_t trakpos = FindVideoTrakAtom(pos);
	if (trakpos <= 0)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	if (0 > ParseVideoTrakAtom(trakpos))
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}

	return 0;
}

unsigned int CMp4Parse::FindMoovAtom()
{
	if (_pFile == NULL)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return 0;
	}
	uint32_t size = 0;
	int64_t pos = 0;
	uint32_t atomType = 0;
	while (atomType != MOOV)
	{
		pos = size + pos;
		atomType = GetAtomType(pos, &size);
		if (atomType == 0)
		{
			break;
		}

	}
	if (atomType == MOOV)
	{
		return pos;
	}
	 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
	return 0;
}
int CMp4Parse::CheckVideoTrakAtom(int64_t pos) // trak pos
{
	if (_pFile == NULL)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return 0;
	}
	if (pos <= 0)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return 0;
	}
	pos += 8;  // ȥ��8�ֽڣ���ѯ�ӿ�
	if (lfseek(_pFile, pos, SEEK_SET) < 0)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return 0;
	}
	uint32_t atomType = 0;
	uint32_t size = 0;

	while (atomType != MDIA)
	{
		pos = size + pos;
		atomType = GetAtomType(pos, &size);
		if (atomType == 0)
		{
			break;
		}
	}
	if (atomType != MDIA)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return 0;
	}
	pos = pos + 8;
	if (lfseek(_pFile, pos, SEEK_SET) < 0)  // seek to Mdia child atom
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return 0;
	}
	size = 0;
	while (atomType != HDLR)
	{
		pos = size + pos;
		atomType = GetAtomType(pos, &size);
		if (atomType == 0)
		{
			break;
		}
	}
	if (atomType != HDLR)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return 0;
	}
	if (size < 32)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return 0;
	}
	if (lfseek(_pFile, pos + 16, SEEK_SET) < 0)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return 0;
	}
	char buf[10] = { 0 };
	if (4 != fread(buf, 1, 4, _pFile))
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return 0;
	}
	if(strcmp(buf,"vide")==0)	//��media box�У���ֵΪ4���ַ���
	{							//��vide���� video track
		return 	1;				//��soun���� audio track
	}							//��hint���� hint track*/
	return 0;
}
unsigned int CMp4Parse::FindVideoTrakAtom(int64_t MoovPos)
{
	if (_pFile == NULL)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return 0;
	}
	uint32_t videoTrakPos = 0;
	uint32_t atomType = 0;
	uint32_t size = 0;
	uint32_t  pos = MoovPos + 8;  //ȥ��moov 8�ֽڣ���ѯmoov ���ӿ�
	while (1)
	{
		pos = size + pos;
		atomType = GetAtomType(pos, &size);
		if (atomType == 0)
		{

			break;
		}
		if (atomType == TRAK)
		{
			if (CheckVideoTrakAtom(pos) == 1)
			{
				break;
			}
		}
	}
	if (atomType == TRAK)
	{
		return pos;
	}
	 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
	return 0;

}
int CMp4Parse::ParseVideoTrakAtom(int64_t trakPos)
{
	if ((_pFile == NULL) || (trakPos <= 0))
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	int64_t pos = trakPos+8;  // ȥ��8�ֽڣ���ѯ�ӿ�
	if (lfseek(_pFile, pos, SEEK_SET) < 0)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	uint32_t atomType = 0;
	uint32_t size = 0;
	while (atomType != MDIA)
	{
		pos = size + pos;
		atomType = GetAtomType(pos, &size);
		if (atomType == 0)
		{
			break;
		}
	}
	if (atomType != MDIA)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	pos = pos + 8;
	if (lfseek(_pFile, pos, SEEK_SET) < 0)  // seek to Mdia child atom
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	size = 0;
	while (atomType != MINF)
	{
		pos = size + pos;
		atomType = GetAtomType(pos, &size);
		if (atomType == 0)
		{
			break;
		}
	}
	if (atomType != MINF)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}

	pos = pos + 8;  //find minf child  
	if (lfseek(_pFile, pos, SEEK_SET) < 0)  // seek to minf child atom
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	size = 0;
	while (atomType != STBL)
	{
		pos = size + pos;
		atomType = GetAtomType(pos, &size);
		if (atomType == 0)
		{
			break;
		}
	}
	uint32_t stblEndPos = size + pos;
	if (atomType == 0)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	pos = pos + 8;  //find STBL child  
	size = 0;
	if (lfseek(_pFile, pos, SEEK_SET) < 0)  // seek to STBL child atom
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	while (atomType != 0)
	{
		pos = size + pos;
		if (pos >= stblEndPos)
		{
			 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
			break;
		}
		atomType = GetAtomType(pos, &size);
		switch (atomType)
		{
		case STTS:
			if (0 > _stts.Init(_pFile, pos))
			{
				__android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
				break;
			}
			_stts.Parse();
			break;
		case STSS:
			if (0 > _stss.Init(_pFile, pos))
			{
				 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
				break;
			}
			_stss.Parse();
			break;
		case STSC:
			if (0 > _stsc.Init(_pFile, pos))
			{
				 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
				break;
			}
			_stsc.Parse();
			break;
		case STSZ:
			if (0 > _stsz.Init(_pFile, pos))
			{
				 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
				break;
			}
			_stsz.Parse();
			break;
		case STCO:
			if (0 > _stco.Init(_pFile, pos))
			{
				 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
				break;
			}
			_stco.Parse();
			break;
		default:
			break;
		}
	}

	return 0;
}

int CMp4Parse::GetVideoKeyFrameCount()
{
	int syncCount = _stss.GetSyncCount();
	if (syncCount <= 0)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	return syncCount;
}
int CMp4Parse::GetVideoKeyFrame(DOWNLOADBLOCKINFO * list)
{
	/*���ҹ��������sample�Ĺ��̷ǳ����ƣ�ֻ����Ҫ����sync sample atom��ȷ��key frame��sample���
	ȷ������ʱ���sample���
	���sync sample atom���������sample���֮���key frame
	���sample - to - chunk atom�����ֶ�Ӧ��sample��chunk
	��chunk offset atom����ȡ��trunk��ƫ����
	����sample size atom�ҵ�sample��trunk�ڵ�ƫ������sample�Ĵ�С*/

	//����sampleId  ��chunk

	if (list == NULL)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	int syncCount = _stss.GetSyncCount();
	if (syncCount <= 0)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	uint32_t * syncSampleList = _stss.GetSyncSampleIdList();
	int64_t timestamp = 0;
	if (syncSampleList == NULL)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	uint32_t syncSampleId = 0;
	uint32_t chunkId = 0;
	uint32_t chunkPos = 0;
	uint32_t offset = 0;
	for (int i = 0; i < syncCount; i++)
	{ 
		syncSampleId = *(syncSampleList + i);

		_stsc.GetChunkIdBySampleId(syncSampleId, &chunkId, &chunkPos);

		offset = _stco.GetChunkOffset(chunkId -1);

		while (chunkPos > 1)
		{
			offset = offset + _stsz.GetSampleSize(syncSampleId - chunkPos);
			chunkPos--;
		}
		timestamp = (syncSampleId-1) * _stts.GetSampleDuration();
		(list + i)->smapleId = syncSampleId;
		(list + i)->offset = offset;
		(list + i)->timeStamp = timestamp;
		(list + i)->isDownload = false;
		syncSampleId = 0;
		offset = 0;
		chunkPos = 0;
	}
	return 0;
}
