#pragma once
#include "Atom.h"
/*Sample Description Atoms - STSD
利用sample description atom存储的信息可以正确的解码media sample。不同的媒体类型存储不同的sample description，例如，视频媒体，sample description就是图像的结构。第四章解释了不同媒体类型对应的sample description信息。


sample description atom的类型是'stsd'，包含了一个sample description表。根据不同的编码方案和存储数据的文件数目，每个media可以有一个到多个sample description。sample-to-chunk atom通过这个索引表，找到合适medai中每个sample的description。
字段		长度(字节)		描述
尺寸		4				这个atom的字节数
类型		4				stsd
版本		1				这个atom的版本
标志		3				这里为0
条目数目	4				sample descriptions的数目
Sample description			不同的媒体类型有不同的sample description，但是每个sample description的前四个字段是相同的，包含以下的数据成员
尺寸		4				这个sample description的字节数
数据格式	4				存储数据的格式。
保留		6
数据引用索引2				利用这个索引可以检索与当前sample description关联的数据。数据引用存储在data reference atoms。



Audio track的值

可以看出这个sample只有一个description，对应得的数据格式是'mp4a'，14496-12定义了这种结构，mp4解码器会识别此description。XXX
Video track的值

可以看出这个sample只有一个description，对应得的数据格式是'mp4v'，14496-12定义了这种结构，mp4解码器会识别此description。XXX
*/
class CStsd :CAtom
{
public:
	CStsd();
	~CStsd();
};

