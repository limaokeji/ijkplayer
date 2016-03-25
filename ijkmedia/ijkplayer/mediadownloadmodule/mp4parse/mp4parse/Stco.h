#pragma once
#include "Atom.h"
/*
Chunk offset atoms ������ÿ��trunk��ý�����е�λ�ã�����������'stco'��
λ�������ֿ��ܣ�32λ�ĺ�64λ�ģ����߶Էǳ���ĵ�Ӱ�����á���һ������ֻ����һ�ֿ��ܣ�
���λ�����������ļ��еģ����������κ�atom�еģ��������Ϳ���ֱ�����ļ����ҵ�ý�����ݣ�
�����ý���atom����Ҫע�����һ��ǰ���atom�����κθı䣬���ű�Ҫ���½�������Ϊλ����Ϣ�Ѿ��ı��ˡ�

�ֶ�		����(�ֽ�)			����
�ߴ�		4					���atom���ֽ���
����		4					stco
�汾		1					���atom�İ汾
��־		3					����Ϊ0
��Ŀ��Ŀ	4					chunk offset����Ŀ
chunk offset					�ֽ�ƫ�������ļ���ʼ����ǰchunk����������chunk number��������һ����ǵ�һ��trunk���ڶ�����ǵڶ���trunk
��С		4					ÿ��sample�Ĵ�С

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

