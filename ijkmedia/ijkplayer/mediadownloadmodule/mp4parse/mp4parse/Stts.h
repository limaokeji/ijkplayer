#pragma once
#include "Atom.h"
/*   Time-to-Sample Atoms - STTS

Time-to-sample atoms�洢��media sample��duration ��Ϣ���ṩ��ʱ��Ծ���data sample��ӳ�䷽����ͨ�����atom��������ҵ��κ�ʱ���sample��������'stts'��
���atom���԰���һ��ѹ���ı���ӳ��ʱ���sample��ţ��������ı����ṩÿ��sample�ĳ��Ⱥ�ָ�롣����ÿ����Ŀ�ṩ����ͬһ��ʱ��ƫ��������������sample��ţ� �Լ�samples��ƫ������������Щƫ�������Ϳ��Խ���һ��������time-to-sample�����㹫ʽ����
DT(n+1) = DT(n) + STTS(n)
����STTS(n)��û��ѹ����STTS��n����Ϣ��DT�ǵ�n��sample����ʾʱ�䡣Sample�������ǰ���ʱ�����˳������ƫ������Զ�ǷǸ��ġ�DTһ����0��ʼ�������Ϊ0��edit list atom �趨��ʼ��DTֵ��DT���㹫ʽ����
DT(i) = SUM (for j=0 to i-1 of delta(j))
����ƫ�����ĺ;���track��media�ĳ��ȣ�������Ȳ�����media��time scale��Ҳ�������κ�edit list��


�ֶ�		����(�ֽ�)		����
�ߴ�		4				���atom���ֽ���
����		4				stts
�汾		1				���atom�İ汾
��־		3				����Ϊ0
��Ŀ��Ŀ	4				time-to-sample����Ŀ
time-to-sample				Media��ÿ��sample��duration���������½ṹ
Sample count	4			����ͬduration������sample����Ŀ
Sample duration	4			ÿ��sample��duration


������sample����ͬ��duration������ֻ��һ������������Щsamples�������ֶ�˵��sample�ĸ��������磬���һ����Ƶý���֡�ʱ��ֲ��䣬���������ֻ��һ���������ȫ����֡����
Audio track��ֵ


���Կ������mp4a���͵���Ƶtrackֻ��һ��time-to-sample��һ����547��sample��ÿ��sample��duration��1024��
Video track��ֵ

���Կ������mp4v���͵�video trackֻ��һ��time-to-sample��һ����1050��sample��ÿ��sample��duration��40��
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

