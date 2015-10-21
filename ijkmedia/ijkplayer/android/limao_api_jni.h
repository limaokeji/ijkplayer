/*
 * limao_api_jni.h
 */

#ifndef IJKPLAYER_ANDROID__LIMAO_API_JNI_H

#include <stdint.h>
#include <jni.h>
#include "../ff_ffmsg_queue.h"

/**
 * 以下消息ID必须与 PlayManager.java 中定义的数值相同。
 */

#define LM_MSG_ERROR                       100     /* arg1 = error */

//11000
#define LM_MSG_PREPARE_TO_PLAY    11000

#define LM_MSG_DOWNLOAD_RSP    11001


//22000
#define LM_MSG_PREPARE_OK    22000

#define LM_MSG_DOWNLOAD_REQ    22001

typedef struct {
	char fileHash[32 + 4];
	char filenameExtension[8 + 4];
	int64_t fileSize;
} limao_api_param_4_prepareToPlay_t;

typedef struct {
	char fileHash[32 + 4];
	int index;	
} limao_api_param_4_downloadFinish_t;


int LimaoApi_global_init(JavaVM *jvm, JNIEnv *env);

MessageQueue * LimaoApi_get_msg_queue();

void LimaoApi_prepareOK(char *fileHash);

int LimaoApi_downloadExt(char *fileHash, int64_t offset, int64_t size, int timeout);

/*
 * return: 1-已下载，0-未下载
 */
int LimaoApi_isDownload(char *fileHash, int64_t offset, int64_t size);

void LimaoApi_getFilePath(/*IN*/char *fileHash, /*OUT*/char *filePath);

#endif
