#pragma once
#include "Atom.h"
/*
Sample Table Atoms - STBL

sample table atom包含转化媒体时间到实际的sample的信息，他也说明了解释sample的信息，例如，视频数据是否需要解压缩，解压缩算法是什么？
它的类型是'stbl'，是一个容器atom，包含sample description atom, time-to-sample atom, sync sample atom, sample-to-chunk atom, sample size atom, chunk offset atom和shadow sync atom.
sample table atom 包含track中media sample的所有时间和数据索引，利用这个表，就可以定位sample到媒体时间，决定其类型，大小，以及如何在其他容器中找到紧邻的sample。
如果sample table atom所在的track没有引用任何数据，那么它就不是一个有用的media track，不需要包含任何子atom。
如果sample table atom所在的track引用了数据，那么必须包含以下的子atom：sample description, sample size, sample to chunk和chunk offset。
所有的子表有相同的sample数目。
sample description atom 是必不可少的一个atom，而且必须包含至少一个条目，因为它包含了数据引用atom检索media sample的目录信息。
没有sample description，就不可能计算出media sample存储的位置。sync sample atom 是可选的，如果没有，表明所有的samples都是sync samples。
*/
class CStbl :CAtom
{
public:
	CStbl();
	~CStbl();
};

