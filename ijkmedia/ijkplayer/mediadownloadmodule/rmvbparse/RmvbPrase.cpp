#include "RmvbPrase.h"
#include "../DataType.h"
#include <string.h>
#include <netinet/in.h>
#include "../mediafile_download_log.h"
RmvbPrase::RmvbPrase(char * filename)
{
	_pIndexRecord = NULL;
	_duration = 0;
	_indexRecordCount = 0;
	_VideoStreamNumber = -1;
	_StreamNumber = -1;
	CHECK_POINTER(filename);
	_pFile = fopen(filename, "rb");
	if (_pFile == NULL)
	{
		printf("class RmvbPrase open the file failed\n");
		return;
	}
}
RmvbPrase::RmvbPrase(FILE * pFile)
{
	_pIndexRecord = NULL;
	_duration = 0;
	_indexRecordCount = 0;
	_VideoStreamNumber = -1;
	_StreamNumber = -1;
	CHECK_POINTER(pFile);
	_pFile = pFile;
}
RmvbPrase::~RmvbPrase()
{
	if (_pIndexRecord != NULL)
	{
		delete[] _pIndexRecord;
		_pIndexRecord = NULL;
	}
}
void RmvbPrase::HandlePropChunk(unsigned int size)
{
	unsigned short ver = 0;
	fread(&ver, 2, 1, _pFile);
	ver = ntohs(ver);
	printf("\tversion is %d\n", ver);

	if (ver == 0) {
		readint("max_bit_rate");
		readint("avg_bit_rate");
		readint("max_packet_size");
		readint("avg_packet_size");
		readint("num_packets");
		_duration = readint("duration");
		readint("preroll");
		readint("index_offset");
		readint("data_offset");
		readshort("num_streams");
		readshort("flags");
	}
}
void RmvbPrase::HandleMdprChunk(unsigned int size)
{

	if (readshort("version") == 0) {
		_StreamNumber = readshort("stream_number");
		readint("max_bit_rate");
		readint("avg_bit_rate");
		readint("max_packet_size");
		readint("avg_packet_size");
		readint("start_time");
		readint("preroll");
		readint("duration");
		readchar(readchar(1, "stream_name_size", false), "stream_name");
		readchar(readchar(1, "mime_type_size", false), "mime_type");
		readchar(readint("type_specific_len"), "type_specific_data", false);
	}

}
void RmvbPrase::HandleContChunk(unsigned int size)
{
	if (readshort("version") == 0) {
		readchar(readshort("title_len"), "title");
		readchar(readshort("author_len"), "author");
		readchar(readshort("copyright_len"), "copyright");
		readchar(readshort("comment_len"), "comment");
	}
}
int RmvbPrase::HandleDataChunk(unsigned int size)
{
	int n = 0;
	if (readshort("version") == 0) {
		n = readint("num_packets");
		readint("next_data_header");
	}
	return n;
}
int RmvbPrase::HandleIndxChunk(unsigned int size, short * pstream_number)
{
	int n = 0;
	if (readshort("version") == 0) {
		n = readint("num_indices");
		*pstream_number = readshort("stream_number");
		readint("next_index_header");
	}
	return n;
}
void RmvbPrase::HandleSubDataChunk()
{
	if (readshort("version") == 0) {
		int size = readshort("length");
		short s_n = readshort("stream_number");
		readint("timestamp");
		readchar(1, "reserved", false);
		readchar(1, "flags", false);
		readchar(size - 12, "data", false);	// 12: data packet header size
	}
}
void RmvbPrase::HandleIndxRecord(INDEXRECORD* pIndexRecord)
{
	if (readshort("version") == 0) {
		pIndexRecord->timeStamp = readint("timestamp");
		pIndexRecord ->offset =  readint("offset");
		pIndexRecord->smapleId =  readint("packet_count_for_this_packet");
		pIndexRecord->isDownload = false;
	}
	/*char logBuf[200] = {0};
	sprintf(logBuf,"rmvb parse offset %ld",pIndexRecord ->offset);
	printf_log(LOG_INFO,
						   "rmvb download file play info",
						   logBuf,
						   	NULL);*/
}
#define strcmp strcasecmp
void RmvbPrase::HandleChunk(char* tagID, unsigned int size)
{
	if (strcmp(tagID, "PROP") == 0) {
		HandlePropChunk(size); 
	}else if (strcmp(tagID, "MDPR") == 0) {
		HandleMdprChunk(size); 
	}else if (strcmp(tagID, "CONT") == 0) {
		HandleContChunk(size); 
	}else if (strcmp(tagID, "DATA") == 0) {
		int num = HandleDataChunk(size);
		printf("index data chunk is %d\n", num);
		/*for (int i = 0; i < (num < 500 ? num : 500); i++)
			HandleSubDataChunk();
		return;*/
	}else if (strcmp(tagID, "INDX") == 0) {
		short stream_number;
		int num = HandleIndxChunk(size, &stream_number);
		if (stream_number == _VideoStreamNumber)
		{
			char logBuf[200] = {0};
			sprintf(logBuf,"download list info index is %d.",num);
			_indexRecordCount = num;
			printf_log(LOG_INFO ,
					"rmvb parse ",
					logBuf,
					NULL);
			_pIndexRecord = NULL;
			_pIndexRecord = new INDEXRECORD[num];
			if(_pIndexRecord == NULL)
			{
				printf_log(LOG_ERROR ,
									"rmvb parse",
									"index record new failed.",
									NULL);
				return;
			}
			memset(_pIndexRecord, 0 , num *sizeof(INDEXRECORD));
			printf_log(LOG_INFO ,
							"rmvb parsea",
							"index record new success.",
							NULL);

			for (int i = 0; i < num; i++)
			{
				HandleIndxRecord(_pIndexRecord + i);
			}
			printf_log(LOG_INFO ,
							"rmvb parsea",
							"index record memory write success.",
							NULL);

			return;
		}

	}
}

bool  RmvbPrase::GetKeyFrameInfo(INDEXRECORD** pIndexRecord)
{
	char tagID[5] = { 0, 0, 0, 0, 0 };
	unsigned long size = 0;
	unsigned long fpos = 0;
	fread(tagID, 4, 1, _pFile);
	fread(&size, 4, 1, _pFile);
	size = ntohl(size);
	if (readshort("version") == 0) {
		readint("file_version");
		readint("num_headers");
	}
	for (;;) {
		fpos += size;
		if (fseek(_pFile, fpos, SEEK_SET)) break;

		fread(tagID, 4, 1, _pFile);
		fread(&size, 4, 1, _pFile);
		if (feof(_pFile)) {
			fpos_t pos;
			fgetpos(_pFile, &pos);
			break;
		}

		size = ntohl(size);
		if (size <= 0) {  break; }


		HandleChunk(tagID, size);
	}




	*pIndexRecord = _pIndexRecord;

	printf_log(LOG_WARN ,
							"rmvb parse",
							"GetKeyFrameInfo success.",
							NULL);



	return true;
}
short RmvbPrase::readshort(char* name)
{
	short t;
	fread(&t, 2, 1, _pFile);
	t = ntohs(t);
	printf("\t%s is %d\n", name, t);
	return t;
}
int RmvbPrase::readint(char* name)
{
	int t;
	int ret = fread(&t, 4, 1, _pFile);
	if (ret == 0) {
		printf("\t!!! %s read error, feof(%d), ferror(%d)\n", name, feof(_pFile), ferror(_pFile));
		return -1;
	}

	t = ntohl(t);
	return t;
}
char RmvbPrase::readchar(unsigned int size, char* name, bool bchar)
{
	char *p = new char[size + 1];
	memset(p, 0, size + 1);

	unsigned int ret = fread(p, 1, size, _pFile);
	if (ret < size) printf("!!!!!!!!!! data read error, %d/%d", ret, size);
	if ((strcmp("Video Stream", p)==0) && (strcmp(name,"stream_name")==0))
	{
		_VideoStreamNumber = _StreamNumber;
	}
	if (bchar)
		printf("\t%s is %s\n", name, p);
	else {
		printf("\t%s are:", name);
		for (unsigned int i = 0; i < (size < 10 ? size : 10); i++)
			printf(" %d", p[i]);
		if (size > 10) {
			printf(" ...");
		}
		printf("\n");
	}

	char ch = p[0];
	delete[] p;

	return ch;
}

