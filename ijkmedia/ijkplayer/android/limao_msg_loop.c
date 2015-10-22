/*
 * limao_msg_loop.c
 */

#include <assert.h>
#include <string.h>
#include <jni.h>

#include "limao_api_jni.h"

#include "../ff_ffinc.h"
#include "../ff_ffmsg_queue.h"

#include "ijksdl/ijksdl_thread.h"
#include "ijksdl/ijksdl_log.h"
#include "ijksdl/android/ijksdl_android_jni.h"

static void message_loop_x(JNIEnv *env)
{
    //jobject weak_thiz = (jobject) ijkmp_get_weak_thiz(mp);
    //JNI_CHECK_GOTO(weak_thiz, env, NULL, "mpjni: message_loop_x: null weak_thiz", LABEL_RETURN);

	  ALOGE("LimaoApi: message_loop_x()");

    while (1) {
        AVMessage msg;

        //int retval = ijkmp_get_msg(mp, &msg, 1);
        int retval = msg_queue_get(LimaoApi_get_msg_queue(), &msg, 1);
        //int retval = msg_queue_get(p_limaoJniStruct->msg_queue, &msg, 1);
        //ALOGE("LimaoApi: message_loop_x(): retval = %d", retval);
        if (retval < 0)
            break;

        // block-get should never return 0
        assert(retval > 0);

        switch (msg.what) {
        #if 0
        case FFP_MSG_ERROR:
            MPTRACE("FFP_MSG_ERROR: %d", msg.arg1);
            post_event(env, weak_thiz, MEDIA_ERROR, MEDIA_ERROR_IJK_PLAYER, msg.arg1);
            break;
        case FFP_MSG_BUFFERING_START:
            MPTRACE("FFP_MSG_BUFFERING_START:");
            post_event(env, weak_thiz, MEDIA_INFO, MEDIA_INFO_BUFFERING_START, 0);
            break;
        case FFP_MSG_BUFFERING_END:
            MPTRACE("FFP_MSG_BUFFERING_END:");
            post_event(env, weak_thiz, MEDIA_INFO, MEDIA_INFO_BUFFERING_END, 0);
            break;
        case FFP_MSG_BUFFERING_UPDATE:
            // MPTRACE("FFP_MSG_BUFFERING_UPDATE: %d, %d", msg.arg1, msg.arg2);
            post_event(env, weak_thiz, MEDIA_BUFFERING_UPDATE, msg.arg1, msg.arg2);
            break;
        case FFP_MSG_BUFFERING_BYTES_UPDATE:
            break;
        case FFP_MSG_BUFFERING_TIME_UPDATE:
            break;
        case FFP_MSG_SEEK_COMPLETE:
            MPTRACE("FFP_MSG_SEEK_COMPLETE:");
            post_event(env, weak_thiz, MEDIA_SEEK_COMPLETE, 0, 0);
            break;
				#endif

        case FFP_MSG_FLUSH: // 这个是消息队列接收的第一个消息
        	ALOGE("LimaoApi: message_loop_x(): FFP_MSG_FLUSH");
			//
            break;

        case LM_MSG_PREPARE_TO_PLAY:
        		ALOGE("LimaoApi: message_loop_x(): LM_MSG_PREPARE_TO_PLAY");

				    //LimaoApi_download("fileHash_001", 0, 100, 30);
				    int xRet = LimaoApi_downloadExt("fileHash_002", 200, 50, 10);
				    ALOGE("LimaoApi: message_loop_x(): xRet = %d", xRet);
				    
				    LimaoApi_isDownload("fileHash_003", 300, 50);
				    
				    char filePath[100];
				    LimaoApi_getFilePath("fileHash_004", filePath);
				    ALOGE("LimaoApi: message_loop_x(): filePath = %s", filePath);
					
					int64_t fsize = LimaoApi_getFileSize("fileHash_005");
					ALOGE("LimaoApi: message_loop_x(): fsize = %lld", fsize);

            break;

        case LM_MSG_DOWNLOAD_RSP:
        	ALOGE("LimaoApi: message_loop_x(): LM_MSG_DOWNLOAD_RSP");
        	LimaoApi_prepareOK("fileHash_001");
			//
            break;

        default:
            ALOGE("LimaoApi: unknown msg: %d", msg.what);
            break;

        }
        
        //FIXME: 释放内存
    }

}

void * LimaoApi_get_msg_loop()
{
	ALOGE("LimaoApi_get_msg_loop()");
	void (*f)(JNIEnv *env) = message_loop_x;
	return f;
}
