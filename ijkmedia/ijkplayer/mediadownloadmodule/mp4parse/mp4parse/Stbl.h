#pragma once
#include "Atom.h"
/*
Sample Table Atoms - STBL

sample table atom����ת��ý��ʱ�䵽ʵ�ʵ�sample����Ϣ����Ҳ˵���˽���sample����Ϣ�����磬��Ƶ�����Ƿ���Ҫ��ѹ������ѹ���㷨��ʲô��
����������'stbl'����һ������atom������sample description atom, time-to-sample atom, sync sample atom, sample-to-chunk atom, sample size atom, chunk offset atom��shadow sync atom.
sample table atom ����track��media sample������ʱ�����������������������Ϳ��Զ�λsample��ý��ʱ�䣬���������ͣ���С���Լ�����������������ҵ����ڵ�sample��
���sample table atom���ڵ�trackû�������κ����ݣ���ô���Ͳ���һ�����õ�media track������Ҫ�����κ���atom��
���sample table atom���ڵ�track���������ݣ���ô����������µ���atom��sample description, sample size, sample to chunk��chunk offset��
���е��ӱ�����ͬ��sample��Ŀ��
sample description atom �Ǳز����ٵ�һ��atom�����ұ����������һ����Ŀ����Ϊ����������������atom����media sample��Ŀ¼��Ϣ��
û��sample description���Ͳ����ܼ����media sample�洢��λ�á�sync sample atom �ǿ�ѡ�ģ����û�У��������е�samples����sync samples��
*/
class CStbl :CAtom
{
public:
	CStbl();
	~CStbl();
};

