#pragma once
#include "Atom.h"
/*   Time-to-Sample Atoms - STTS

Time-to-sample atoms存储了media sample的duration 信息，提供了时间对具体data sample的映射方法，通过这个atom，你可以找到任何时间的sample，类型是'stts'。
这个atom可以包含一个压缩的表来映射时间和sample序号，用其他的表来提供每个sample的长度和指针。表中每个条目提供了在同一个时间偏移量里面连续的sample序号， 以及samples的偏移量。递增这些偏移量，就可以建立一个完整的time-to-sample表，计算公式如下
DT(n+1) = DT(n) + STTS(n)
其中STTS(n)是没有压缩的STTS第n项信息，DT是第n个sample的显示时间。Sample的排列是按照时间戳的顺序，这样偏移量永远是非负的。DT一般以0开始，如果不为0，edit list atom 设定初始的DT值。DT计算公式如下
DT(i) = SUM (for j=0 to i-1 of delta(j))
所有偏移量的和就是track中media的长度，这个长度不包括media的time scale，也不包括任何edit list。


字段		长度(字节)		描述
尺寸		4				这个atom的字节数
类型		4				stts
版本		1				这个atom的版本
标志		3				这里为0
条目数目	4				time-to-sample的数目
time-to-sample				Media中每个sample的duration。包含如下结构
Sample count	4			有相同duration的连续sample的数目
Sample duration	4			每个sample的duration


如果多个sample有相同的duration，可以只用一项描述所有这些samples，数量字段说明sample的个数。例如，如果一个视频媒体的帧率保持不变，整个表可以只有一项，数量就是全部的帧数。
Audio track的值


可以看出这个mp4a类型的音频track只有一项time-to-sample，一共有547个sample，每个sample的duration是1024。
Video track的值

可以看出这个mp4v类型的video track只有一项time-to-sample，一共有1050个sample，每个sample的duration是40。
*/
class CStts : public CAtom
{
public:
	CStts();
	~CStts();
	int Parse();
	int64_t GetSampleDuration(){ return _sampleDuration; }
private:
	int _count;
	uint32_t _sampleCount;
	uint32_t _sampleDuration;
};

