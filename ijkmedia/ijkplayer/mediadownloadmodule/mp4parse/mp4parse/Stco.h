#pragma once
#include "Atom.h"
/*
Chunk offset atoms 定义了每个trunk在媒体流中的位置，它的类型是'stco'。
位置有两种可能，32位的和64位的，后者对非常大的电影很有用。在一个表中只会有一种可能，
这个位置是在整个文件中的，而不是在任何atom中的，这样做就可以直接在文件中找到媒体数据，
而不用解释atom。需要注意的是一旦前面的atom有了任何改变，这张表都要重新建立，因为位置信息已经改变了。

字段		长度(字节)			描述
尺寸		4					这个atom的字节数
类型		4					stco
版本		1					这个atom的版本
标志		3					这里为0
条目数目	4					chunk offset的数目
chunk offset					字节偏移量从文件开始到当前chunk。这个表根据chunk number索引，第一项就是第一个trunk，第二项就是第二个trunk
大小		4					每个sample的大小

*/
class CStco : public CAtom
{
public:
	CStco();
	~CStco();
	int Parse();
	uint32_t GetChunkOffset(uint32_t chunkId);
private :
	int _count;
	unsigned int * _chunkOffsetList;

};

