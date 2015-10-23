/*
 * limao_msg_loop.h
 */

#ifndef IJKPLAYER_ANDROID__LIMAO_MSG_LOOP_H
#define IJKPLAYER_ANDROID__LIMAO_MSG_LOOP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <jni.h>
#include <pthread.h>

typedef struct {
	JavaVM *jvm;
	JNIEnv *env;

	char fileHash[32 + 4];
	char filenameExtension[8 + 4];
} ThreadLocalData_t;

void * LimaoApi_get_msg_loop();

pthread_key_t LimaoApi_get_pthread_key();

#ifdef __cplusplus
}
#endif

#endif
