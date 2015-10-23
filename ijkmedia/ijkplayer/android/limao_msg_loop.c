/*
 * limao_msg_loop.c
 */

#include <assert.h>
#include <string.h>
#include <jni.h>

#include "limao_api_jni.h"
#include "limao_msg_loop.h"
#include "limao_mediafile_download.h"

#include "../ff_ffinc.h"
#include "../ff_ffmsg_queue.h"

#include "ijksdl/ijksdl_thread.h"
#include "ijksdl/ijksdl_log.h"
#include "ijksdl/android/ijksdl_android_jni.h"

static pthread_key_t pthread_key_1;

void LimaoApi_download(char *fileHash, int index, int64_t offset, int64_t size);

static void start_routine_new(ThreadLocalData_t *pData)
{
	pthread_t threadID = pthread_self();
	ALOGD("LimaoApi: start_routine_new():threadID = %ld", threadID);
	ALOGD("LimaoApi: start_routine_new(): %s %s", pData->fileHash, pData->filenameExtension);

	pthread_setname_np(threadID, "limao_thread_n");

	//pthread_key_create(&pthread_key_1, NULL);
	pthread_setspecific (pthread_key_1, pData);

	// 入口函数加在这里。
	media_file_download_module_thread((void *)pData);

	//

#if 0 // 测试代码
	for (int i = 0; i < 600; i++)
	{
#if 1
		LimaoApi_download("fileHash_005", i, 10, 100);

		int xRet = LimaoApi_downloadExt("fileHash_001", 200, 50, 10);
		ALOGD("LimaoApi: start_routine_new(): xRet = %d", xRet);
		
		LimaoApi_isDownload("fileHash_002", 300, 50);
		
		char filePath[100];
		LimaoApi_getFilePath("fileHash_003", filePath);
		ALOGD("LimaoApi: start_routine_new(): filePath = %s", filePath);
		
		int64_t fsize = LimaoApi_getFileSize("fileHash_004");
		ALOGD("LimaoApi: start_routine_new(): fsize = %lld", fsize);
#endif

		//LimaoApi_prepareOK("fileHash_005");

	}
#endif

	//FIXME: pthread_key_delete() ...

	ALOGD("LimaoApi: start_routine_new():threadID = %ld --- exit", threadID);
}

static void * thread_start_routine(void *pData)
{
	ALOGD("LimaoApi: %s\n", __func__);
	ThreadLocalData_t *threadLocalData = pData;

	JavaVM *jvm = LimaoApi_get_JVM();
		
	JNIEnv *env = NULL;
	(*jvm)->AttachCurrentThread(jvm, &env, NULL );
	
	threadLocalData->jvm = jvm;
	threadLocalData->env = env;
	
	start_routine_new(threadLocalData);
	
	(*jvm)->DetachCurrentThread(jvm);
	
	ALOGD("LimaoApi: thread_start_routine exit");
	return NULL;
}

static void message_loop_x(JNIEnv *env)
{
	ALOGD("LimaoApi: message_loop_x()");

	pthread_key_create(&pthread_key_1, NULL);

    while (1) {
        AVMessage msg;

        int retval = msg_queue_get(LimaoApi_get_msg_queue(), &msg, 1);
        //ALOGD("LimaoApi: message_loop_x(): retval = %d", retval);
        if (retval < 0)
            break;

        // block-get should never return 0
        assert(retval > 0);

        switch (msg.what) {
        case FFP_MSG_FLUSH: // 这个是消息队列接收的第一个消息
            ALOGD("LimaoApi: message_loop_x(): FFP_MSG_FLUSH");
            break;

        case LM_MSG_PREPARE_TO_PLAY:
        	ALOGD("LimaoApi: message_loop_x(): LM_MSG_PREPARE_TO_PLAY");

			limao_api_param_4_prepareToPlay_t *param = msg.data;

			ThreadLocalData_t *threadLocalData = (ThreadLocalData_t *) malloc(sizeof(ThreadLocalData_t));
			strcpy(threadLocalData->fileHash, param->fileHash);
			strcpy(threadLocalData->filenameExtension, param->filenameExtension);
			
			pthread_t newThread = 0;
			pthread_create(&newThread, NULL, thread_start_routine, threadLocalData);             
          
            break;

        default:
            ALOGD("LimaoApi: unknown msg: %d", msg.what);
            break;

        }
        
        //FIXME: 释放内存
    }

}

void * LimaoApi_get_msg_loop()
{
	ALOGD("LimaoApi_get_msg_loop()");
	void (*f)(JNIEnv *env) = message_loop_x;
	return f;
}

pthread_key_t LimaoApi_get_pthread_key()
{
	return pthread_key_1;
}
