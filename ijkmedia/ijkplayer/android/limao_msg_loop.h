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

    MessageQueue *msg_queue;
    MessageQueue _msg_queue;

	char fileHash[32 + 4];
	char filenameExtension[8 + 4];
	int64_t fileSize;
	int64_t playRequestTime;
} ThreadLocalData_t;

void * LimaoApi_get_msg_loop();

pthread_key_t LimaoApi_get_pthread_key();

#ifdef __cplusplus
}
#endif

#endif
