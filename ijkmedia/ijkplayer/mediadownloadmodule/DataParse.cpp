#ifndef _ANDROID
#include "stdafx.h"
#endif
#include "DataPrase.h"
int DataPrase::DataToInt32(unsigned char data[])
{
	if (data == NULL)
		return -1;
	//return ((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
	return ((data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0]);
}

int DataPrase::DataToBigEndianInt32(unsigned char data[])
{
	if (data == NULL)
		return -1;
	return ((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
	//return ((data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0]);
}
