#pragma once
#include "Atom.h" 
/*   Sample-to-Chunk Atoms - STSC

当添加samples到media时，用chunks组织这些sample，这样可以方便优化数据获取。一个trunk包含一个或多个sample，chunk的长度可以不同，
chunk内的sample的长度也可以不同。sample-to-chunk atom存储sample与chunk的映射关系。
Sample-to-chunk atoms的类型是'stsc'。它也有一个表来映射sample和trunk之间的关系，查看这张表，就可以找到包含指定sample的trunk，从而找到这个sample。
字段		长度(字节)		描述
尺寸		4				这个atom的字节数
类型		4				stsc
版本		1				这个atom的版本
标志		3				这里为0
条目数目	4				sample-to-chunk的数目
sample-to-chunk				sample-to-chunk表的结构
First chunk	4				这个table使用的第一个chunk序号
Samples per chunk	4		当前trunk内的sample数目
Sample description ID	4	与这些sample关联的sample description的序号




可以建立Video track的sample-to-chunk表，共有*项。
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
…				…					…
277				4					1
280				3					1
这个表类似于行程编码，第一个first chunk减去第二个first chunk就是一共有多少个trunk包含相同的sample数目，
这样通过不断的叠加，就可以得到一共有280个trunk，每个trunk包含多少个sample，以及每个trunk对应的description。
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

