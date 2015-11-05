/*
 * limao_api_jni.h
 */

#ifndef IJKPLAYER_ANDROID__LIMAO_API_JNI_H
#define IJKPLAYER_ANDROID__LIMAO_API_JNI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <jni.h>
#include "../ff_ffmsg_queue.h"

/**
 * ref to PlayManager.java
 */

#define LM_MSG_ERROR                       100     /* arg1 = error */

//11000
#define LM_MSG_PREPARE_TO_PLAY    11000

#define LM_MSG_DOWNLOAD_RSP    11001

#define LM_MSG_P2P_DOWNLOAD_BLOCK   11002

#define LM_MSG_PLAYER_SEEK   11003

#define LM_MSG_QUIT_THREAD   11004



//22000
#define LM_MSG_PREPARE_OK    22000

#define LM_MSG_DOWNLOAD_REQ    22001

typedef struct {
	char fileHash[32 + 4];
	char filenameExtension[8 + 4];
	int64_t fileSize;
	int64_t playRequestTime;
} limao_api_param_4_prepareToPlay_t;

typedef struct {
	char fileHash[32 + 4];
	int index;	
} limao_api_param_4_downloadFinish_t;


int LimaoApi_global_init(JavaVM *jvm, JNIEnv *env);

JavaVM * LimaoApi_get_JVM();

MessageQueue * LimaoApi_get_msg_queue();

int64_t LimaoApi_get_start_time();

int64_t LimaoApi_get_playRequestTime();

void LimaoApi_set_playRequestTime(int64_t time);

void LimaoApi_prepareOK(char *fileHash);

void LimaoApi_bufferingUpdate(char *fileHash, int percent);

int LimaoApi_download(char *fileHash, int64_t offset, int64_t size);

int LimaoApi_downloadExt(char *fileHash, int64_t offset, int64_t size, int timeout);

/*
 * return: 1-yes, 0-no
 */
int LimaoApi_isDownload(char *fileHash, int64_t offset, int64_t size);

void LimaoApi_getFilePath(/*IN*/char *fileHash, /*OUT*/char *filePath);

int64_t LimaoApi_getFileSize(char *fileHash);

void LimaoApi_MQ_map_add(int64_t time, void *ptr);

void LimaoApi_MQ_map_remove(int64_t time);

void * LimaoApi_MQ_map_get(int64_t time);

#ifdef __cplusplus
}
#endif

#endif
