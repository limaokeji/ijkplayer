#pragma once
#include "Atom.h"
/*
Sync Sample Atoms - STSS
sync sample atom确定media中的关键帧。对于压缩的媒体，关键帧是一系列压缩序列的开始帧，它的解压缩是不依赖于以前的帧。后续帧的解压缩依赖于这个关键帧。
sync sample atom可以非常紧凑的标记媒体内的随机存取点。它包含一个sample序号表，表内的每一项严格按照sample的序号排列，说明了媒体中的哪一个sample是关键帧。
如果此表不存在，说明每一个sample都是一个关键帧，是一个随机存取点。
Sync sample atoms 的类型是'stss'。

字段		长度(字节)			描述
尺寸		4					这个atom的字节数
类型		4					stss
版本		1					这个atom的版本
标志		3					这里为0
条目数目	4					sync sample的数目
sync sample						sync sample表的结构
Sample序号	4					是关键帧的sample序号


Video track的值
可以看出这个video片断共有35个关键帧。
*/
class CStss :public CAtom
{
public:
	CStss();
	~CStss();
	int Parse();
	int GetSyncCount(){ return _count; }
	uint32_t* GetSyncSampleIdList(){ return _keySampleIdList; }
private:
	int _count;
	uint32_t *_keySampleIdList;  //关键帧序号
};

