#pragma once
#include "Atom.h"
/*
Sync Sample Atoms - STSS
sync sample atomȷ��media�еĹؼ�֡������ѹ����ý�壬�ؼ�֡��һϵ��ѹ�����еĿ�ʼ֡�����Ľ�ѹ���ǲ���������ǰ��֡������֡�Ľ�ѹ������������ؼ�֡��
sync sample atom���Էǳ����յı��ý���ڵ������ȡ�㡣������һ��sample��ű����ڵ�ÿһ���ϸ���sample��������У�˵����ý���е���һ��sample�ǹؼ�֡��
����˱����ڣ�˵��ÿһ��sample����һ���ؼ�֡����һ�������ȡ�㡣
Sync sample atoms ��������'stss'��

�ֶ�		����(�ֽ�)			����
�ߴ�		4					���atom���ֽ���
����		4					stss
�汾		1					���atom�İ汾
��־		3					����Ϊ0
��Ŀ��Ŀ	4					sync sample����Ŀ
sync sample						sync sample��Ľṹ
Sample���	4					�ǹؼ�֡��sample���


Video track��ֵ
���Կ������videoƬ�Ϲ���35���ؼ�֡��
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
	uint32_t *_keySampleIdList;  //�ؼ�֡���
};

