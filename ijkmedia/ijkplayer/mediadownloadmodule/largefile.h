#ifndef __LARGEFILE_H__
#define __LARGEFILE_H__

#ifndef WIN32
/************************************************************************/
// If you want to use this macro, it must be defined before all the header
// files. And you maybe not use it but need to add compiler option
// -D_FILE_OFFSET_BITS=64 in the Makefile.
/************************************************************************/
//#define _FILE_OFFSET_BITS 64  // <==> -D_FILE_OFFSET_BITS=64
//#define _LARGEFILE_SOURCE
//#define _LARGEFILE64_SOURCE   // <==> -D_LARGEFILE64_SOURCE
#endif

#include <stdio.h>
#ifdef WIN32

#else
#include <sys/types.h>
#include <unistd.h>
#endif

inline int lfseek(FILE* fp, int64_t offset, int mode)
{
#ifdef WIN32
    return _fseeki64(fp, offset, mode);
#else
    return fseeko(fp, offset, mode);
#endif
}

inline int64_t lftell(FILE* fp)
{
#ifdef WIN32
    return _ftelli64(fp);
#else
    return ftello(fp);
#endif
}


#endif //__LARGEFILE_H__

