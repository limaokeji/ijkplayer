#include "Atom.h"


CAtom::CAtom()
{
}


CAtom::~CAtom()
{
}

int CAtom::Init(FILE * pFile, int64_t pos)
{
	if (pFile == NULL)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	if (pos <= 0)
	{
		 __android_log_print(ANDROID_LOG_ERROR,"lmk mp4 parse", "lmk %d  | %s ",__LINE__,__FILE__);
		return -1;
	}
	_pFile = pFile;
	_pos = pos;
	return 0;

}
