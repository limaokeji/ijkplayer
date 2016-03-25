#pragma once
/*
sample size atoms������ÿ��sample�Ĵ�С������������'stsz'��������ý����ȫ��sample����Ŀ��һ�Ÿ���ÿ��sample��С�ı�������ý����������Ϳ���û�б߿�����ơ�


�ֶ�		����(�ֽ�)		����
�ߴ�		4				���atom���ֽ���
����		4				stsz
�汾		1				���atom�İ汾
��־		3				����Ϊ0
Sample size	4				ȫ��sample����Ŀ��������е�sample����ͬ�ĳ��ȣ�����ֶξ������ֵ����������ֶε�ֵ����0����Щ���ȴ���sample size����
��Ŀ��Ŀ	4				sample size����Ŀ
sample size					sample size��Ľṹ����������sample number��������һ����ǵ�һ��sample���ڶ�����ǵڶ���sample
��С		4				ÿ��sample�Ĵ�С


Audio track��ֵ
�����������һ�������Կ������audio track��sample�ĳ��ȶ���һ����һ����547�

Video track��ֵ
����������ÿ��sample�ĳ��ȣ��ҵ�sample����ţ��Ϳ����ҵ���Ӧsample�ĳ����ˡ����Կ���video track����1050��sample
*/
#include "Atom.h"
class CStsz : public CAtom
{
public:
	CStsz();
	~CStsz();
	int Parse();
	uint32_t GetSampleSize(uint32_t sampleId);
private:


	uint32_t _sampleSize;  
	uint32_t 	_count;
	uint32_t *_sampleSizeList;
};

