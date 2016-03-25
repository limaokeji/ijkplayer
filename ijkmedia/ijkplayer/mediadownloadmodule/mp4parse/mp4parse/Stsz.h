#pragma once
/*
sample size atoms定义了每个sample的大小，它的类型是'stsz'，包含了媒体中全部sample的数目和一张给出每个sample大小的表。这样，媒体数据自身就可以没有边框的限制。


字段		长度(字节)		描述
尺寸		4				这个atom的字节数
类型		4				stsz
版本		1				这个atom的版本
标志		3				这里为0
Sample size	4				全部sample的数目。如果所有的sample有相同的长度，这个字段就是这个值。否则，这个字段的值就是0。那些长度存在sample size表中
条目数目	4				sample size的数目
sample size					sample size表的结构。这个表根据sample number索引，第一项就是第一个sample，第二项就是第二个sample
大小		4				每个sample的大小


Audio track的值
这个表是最大的一个表，可以看到这个audio track的sample的长度都不一样，一共有547项。

Video track的值
这个表包含了每个sample的长度，找到sample的序号，就可以找到对应sample的长度了。可以看到video track共有1050个sample
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

