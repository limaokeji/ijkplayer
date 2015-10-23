#ifndef DATAPRASE_H_
#define DATAPRASE_H_
#include <stdio.h>
class DataPrase{
public:
	int DataToInt32(unsigned char buf[]);
	int DataToBigEndianInt32(unsigned char buf[]);
};
#endif