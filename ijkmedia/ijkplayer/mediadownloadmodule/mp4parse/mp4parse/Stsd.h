#pragma once
#include "Atom.h"
/*Sample Description Atoms - STSD
����sample description atom�洢����Ϣ������ȷ�Ľ���media sample����ͬ��ý�����ʹ洢��ͬ��sample description�����磬��Ƶý�壬sample description����ͼ��Ľṹ�������½����˲�ͬý�����Ͷ�Ӧ��sample description��Ϣ��


sample description atom��������'stsd'��������һ��sample description�����ݲ�ͬ�ı��뷽���ʹ洢���ݵ��ļ���Ŀ��ÿ��media������һ�������sample description��sample-to-chunk atomͨ������������ҵ�����medai��ÿ��sample��description��
�ֶ�		����(�ֽ�)		����
�ߴ�		4				���atom���ֽ���
����		4				stsd
�汾		1				���atom�İ汾
��־		3				����Ϊ0
��Ŀ��Ŀ	4				sample descriptions����Ŀ
Sample description			��ͬ��ý�������в�ͬ��sample description������ÿ��sample description��ǰ�ĸ��ֶ�����ͬ�ģ��������µ����ݳ�Ա
�ߴ�		4				���sample description���ֽ���
���ݸ�ʽ	4				�洢���ݵĸ�ʽ��
����		6
������������2				��������������Լ����뵱ǰsample description���������ݡ��������ô洢��data reference atoms��



Audio track��ֵ

���Կ������sampleֻ��һ��description����Ӧ�õ����ݸ�ʽ��'mp4a'��14496-12���������ֽṹ��mp4��������ʶ���description��XXX
Video track��ֵ

���Կ������sampleֻ��һ��description����Ӧ�õ����ݸ�ʽ��'mp4v'��14496-12���������ֽṹ��mp4��������ʶ���description��XXX
*/
class CStsd :CAtom
{
public:
	CStsd();
	~CStsd();
};

