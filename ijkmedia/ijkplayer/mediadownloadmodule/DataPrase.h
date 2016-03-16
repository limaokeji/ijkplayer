#ifndef DATAPRASE_H_
#define DATAPRASE_H_
#include <stdio.h>
class DataPrase{
public:
	unsigned int DataToInt32(unsigned char buf[]);
	unsigned int DataToBigEndianInt32(unsigned char buf[]);
};
#endif
