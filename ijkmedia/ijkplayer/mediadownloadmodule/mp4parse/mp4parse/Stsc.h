#pragma once
#include "Atom.h" 
/*   Sample-to-Chunk Atoms - STSC

�����samples��mediaʱ����chunks��֯��Щsample���������Է����Ż����ݻ�ȡ��һ��trunk����һ������sample��chunk�ĳ��ȿ��Բ�ͬ��
chunk�ڵ�sample�ĳ���Ҳ���Բ�ͬ��sample-to-chunk atom�洢sample��chunk��ӳ���ϵ��
Sample-to-chunk atoms��������'stsc'����Ҳ��һ������ӳ��sample��trunk֮��Ĺ�ϵ���鿴���ű��Ϳ����ҵ�����ָ��sample��trunk���Ӷ��ҵ����sample��
�ֶ�		����(�ֽ�)		����
�ߴ�		4				���atom���ֽ���
����		4				stsc
�汾		1				���atom�İ汾
��־		3				����Ϊ0
��Ŀ��Ŀ	4				sample-to-chunk����Ŀ
sample-to-chunk				sample-to-chunk��Ľṹ
First chunk	4				���tableʹ�õĵ�һ��chunk���
Samples per chunk	4		��ǰtrunk�ڵ�sample��Ŀ
Sample description ID	4	����Щsample������sample description�����




���Խ���Video track��sample-to-chunk������*�
First chunk		Samples per chunk	Sample description ID
1				4					1
4				3					1
5				4					1
8				3					1
9				4					1
12				3					1
13				4					1
16				3					1
17				4					1
20				3					1
21				4					1
��				��					��
277				4					1
280				3					1
������������г̱��룬��һ��first chunk��ȥ�ڶ���first chunk����һ���ж��ٸ�trunk������ͬ��sample��Ŀ��
����ͨ�����ϵĵ��ӣ��Ϳ��Եõ�һ����280��trunk��ÿ��trunk�������ٸ�sample���Լ�ÿ��trunk��Ӧ��description��
*/

typedef struct  sampleTunck
{
	uint32_t firstTunck;
	uint32_t samplePerChunk;
	uint32_t sampleDescriptionId;
}SAMPLETUNCK;
class CStsc : public CAtom
{
public:
	CStsc();
	~CStsc();
	int Parse();
	int GetChunkIdBySampleId(uint32_t sampleId, /*out*/ uint32_t* chunkId, /*out*/ uint32_t * chunkPos);
private:
	uint32_t _count;
	SAMPLETUNCK *_sampleToTunck;
};

