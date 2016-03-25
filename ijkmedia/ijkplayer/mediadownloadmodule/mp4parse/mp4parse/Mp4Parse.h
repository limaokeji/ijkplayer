#pragma once
#include "Atom.h"
#include "Stts.h"
#include "Stss.h"
#include "Stsc.h"
#include "Stsz.h"
#include "Stco.h"
#include "../../DataType.h"
#define  MOOV  0x6D6F6F76
#define  TRAK  0x7472616B
#define  MDIA  0x6D646961
#define	 HDLR  0x68646C72
#define  MINF  0x6D696E66
#define  STBL  0x7374626C

#define  STTS  0x73747473
#define  STSS  0x73747373
#define  STSC  0x73747363
#define  STSZ  0x7374737A
#define  STCO  0x7374636F
class CMp4Parse : CAtom
{
public:
	CMp4Parse();
	~CMp4Parse();
	int Init(char * filePath , FILE * pfile);
	int GetVideoKeyFrameCount();
	int GetVideoKeyFrame(DOWNLOADBLOCKINFO * p);
private:
	unsigned int GetAtomType(int64_t pos, uint32_t * pSize);
	unsigned int FindMoovAtom();
	unsigned int FindVideoTrakAtom(int64_t MoovPos);
	int CheckVideoTrakAtom(int64_t pos);
	int ParseVideoTrakAtom(int64_t trakPos);

	FILE * _pFile;
	DataPrase _dataParse;
	CStts _stts;
	CStss _stss;
	CStsc _stsc;
	CStsz _stsz;
	CStco _stco;
};

