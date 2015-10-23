/*
 * limao_api_jni.c
 */

#include <assert.h>
#include <string.h>
#include <jni.h>

#include "limao_api_jni.h"
#include "limao_msg_loop.h"

#include "../ff_ffinc.h"
#include "../ff_ffmsg_queue.h"

#include "ijksdl/ijksdl_thread.h"
#include "ijksdl/ijksdl_log.h"
#include "ijksdl/android/ijksdl_android_jni.h"

//

#define JNI_CLASS_PLAY_MANAGER "com/limaoso/phonevideo/playmanager/PlayManager"

typedef struct limao_api_fields_t {
    jclass clazz;

	jmethodID jmid_c2j_postMsgToUI;
	jmethodID jmid_c2j_prepareOK;
	jmethodID jmid_c2j_download;
	jmethodID jmid_c2j_downloadExt;
	jmethodID jmid_c2j_isDownload;
	jmethodID jmid_c2j_getFilePath;
	jmethodID jmid_c2j_getFileSize;

} limao_api_fields_t;

typedef struct LimaoJniStruct {

    //volatile int ref_count;
    //pthread_mutex_t mutex;
    //FFPlayer *ffplayer;

    int (*msg_loop)(void*);
    SDL_Thread *msg_thread;
    SDL_Thread _msg_thread;

    MessageQueue *msg_queue;
    MessageQueue _msg_queue;

    //int mp_state;
    //char *data_source;
    //void *weak_thiz;

    //int restart_from_beginning;
    //int seek_req;
    //long seek_msec;
} LimaoJniStruct;

static JavaVM *g_jvm;

static JNIEnv *g_env;

static limao_api_fields_t g_clazz;

static LimaoJniStruct s_limaoJniStruct;

static LimaoJniStruct *p_limaoJniStruct = &s_limaoJniStruct;

#if 0
static void LimaoApi_postMsgToUI(int msgID, int arg1, int arg2, char *str)
{
	//	LimaoApi_postMsgToUI(LM_MSG_DOWNLOAD_REQ, NULL, 0, 0);

	//(*env)->CallStaticVoidMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_postMsgToUI, msgID, arg1, arg2, NULL );
}
#endif

static void LimaoApi_prepareToPlay(JNIEnv *env, jclass clazz, jstring fileHash, jstring filenameExtension, jlong fileSize)
{
	limao_api_param_4_prepareToPlay_t *param = malloc(sizeof(limao_api_param_4_prepareToPlay_t));
	// FIXME -- malloc --null

	const char *c_fileHash = (*env)->GetStringUTFChars(env, fileHash, NULL);
	const char *c_filenameExtension = (*env)->GetStringUTFChars(env, filenameExtension, NULL);

	ALOGD("LimaoApi_prepareToPlay: fileHash | filenameExtension | fileSize = %s %s %lld", c_fileHash, c_filenameExtension, fileSize);
	strcpy(param->fileHash, c_fileHash);
	strcpy(param->filenameExtension, c_filenameExtension);
	param->fileSize = fileSize;
	
	(*env)->ReleaseStringUTFChars(env, fileHash, c_fileHash);
	(*env)->ReleaseStringUTFChars(env, filenameExtension, c_filenameExtension);	

	msg_queue_put_simple5(p_limaoJniStruct->msg_queue, LM_MSG_PREPARE_TO_PLAY, param);
}

void LimaoApi_prepareOK(char *fileHash)
{
//	LimaoApi_postMsgToUI(LM_MSG_PREPARE_OK, NULL, 0, 0);

	pthread_key_t key = LimaoApi_get_pthread_key();
	ThreadLocalData_t *tld = pthread_getspecific(key);
	JNIEnv *env = tld->env;

	jstring str = (*env)->NewStringUTF(env, fileHash);

	(*env)->CallStaticVoidMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_prepareOK, str);

	(*env)->DeleteLocalRef(env, str);
}

void LimaoApi_download(char *fileHash, int index, int64_t offset, int64_t size)
{
//	LimaoApi_postMsgToUI(LM_MSG_DOWNLOAD_REQ, NULL, 0, 0);

	pthread_key_t key = LimaoApi_get_pthread_key();
	ThreadLocalData_t *tld = pthread_getspecific(key);
	JNIEnv *env = tld->env;

	jstring str = (*env)->NewStringUTF(env, fileHash);

	(*env)->CallStaticVoidMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_download, str, index, offset, size);
	
	(*env)->DeleteLocalRef(env, str);
}

int LimaoApi_downloadExt(char *fileHash, int64_t offset, int64_t size, int timeout)
{
	int ret = 0;

	__android_log_print(ANDROID_LOG_DEBUG, "LimaoApi_downloadExt()", "offset = %lld, size = %lld", offset, size);

	pthread_key_t key = LimaoApi_get_pthread_key();
	ThreadLocalData_t *tld = pthread_getspecific(key);
	JNIEnv *env = tld->env;

	jstring str = (*env)->NewStringUTF(env, fileHash);

	ret = (*env)->CallStaticIntMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_downloadExt, str, offset, size, timeout);

	(*env)->DeleteLocalRef(env, str);

	return ret;
}

int LimaoApi_isDownload(char *fileHash, int64_t offset, int64_t size)
{
	int ret = 0;

	pthread_key_t key = LimaoApi_get_pthread_key();
	ThreadLocalData_t *tld = pthread_getspecific(key);
	JNIEnv *env = tld->env;

	jstring str = (*env)->NewStringUTF(env, fileHash);
	
	ret = (*env)->CallStaticIntMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_isDownload, str, offset, size);

	(*env)->DeleteLocalRef(env, str);

	return ret;
}

void LimaoApi_getFilePath(/*IN*/char *fileHash, /*OUT*/char *filePath)
{
	//int ret = 0;

	pthread_key_t key = LimaoApi_get_pthread_key();
	ThreadLocalData_t *tld = pthread_getspecific(key);
	JNIEnv *env = tld->env;

	jstring strHash = (*env)->NewStringUTF(env, fileHash);

	jstring strPath = (*env)->CallStaticObjectMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_getFilePath, strHash);

	const char * c_filePath = (*env)->GetStringUTFChars(env, strPath, NULL);

	memset(filePath, 0, 4);
	strcpy(filePath, c_filePath);
	ALOGD("LimaoApi_getFilePath: fileHash | filePath = %s %s", fileHash, filePath);
	
	(*env)->ReleaseStringUTFChars(env, strPath, c_filePath);
	(*env)->DeleteLocalRef(env, strHash);
	(*env)->DeleteLocalRef(env, strPath);

}

int64_t LimaoApi_getFileSize(char *fileHash)
{
	//int ret = 0;

	pthread_key_t key = LimaoApi_get_pthread_key();
	ThreadLocalData_t *tld = pthread_getspecific(key);
	JNIEnv *env = tld->env;

	jstring strHash = (*env)->NewStringUTF(env, fileHash);

	jlong fsize = (*env)->CallStaticLongMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_getFileSize, strHash);

	(*env)->DeleteLocalRef(env, strHash);

	ALOGD("LimaoApi_getFileSize: fileHash | fileSize = %s %lld", fileHash, fsize);

	return fsize;
}

static void LimaoApi_downloadFinish(JNIEnv *env, jclass clazz, jstring fileHash, jint index)
{
	limao_api_param_4_downloadFinish_t *param = malloc(sizeof(limao_api_param_4_downloadFinish_t));	
	// FIXME -- malloc --null

	const char * c_fileHash = (*env)->GetStringUTFChars(env, fileHash, NULL);

	ALOGD("LimaoApi_downloadFinish: fileHash | index = %s %d", c_fileHash, index);
	strcpy(param->fileHash, c_fileHash);
	
	(*env)->ReleaseStringUTFChars(env, fileHash, c_fileHash);

	msg_queue_put_simple5(p_limaoJniStruct->msg_queue, LM_MSG_DOWNLOAD_RSP, param);
}

#if 0
static void message_loop_n(JNIEnv *env)
{
    //jobject weak_thiz = (jobject) ijkmp_get_weak_thiz(mp);
    //JNI_CHECK_GOTO(weak_thiz, env, NULL, "mpjni: message_loop_n: null weak_thiz", LABEL_RETURN);

	ALOGD("LimaoApi: message_loop_n()");

    while (1) {
        AVMessage msg;

        //int retval = ijkmp_get_msg(mp, &msg, 1);
        int retval = msg_queue_get(p_limaoJniStruct->msg_queue, &msg, 1);
        //ALOGD("LimaoApi: message_loop_n(): retval = %d", retval);
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
        	ALOGD("LimaoApi: message_loop_n(): FFP_MSG_FLUSH");
			//
            break;

        case LM_MSG_PREPARE_TO_PLAY:
        	ALOGD("LimaoApi: message_loop_n(): LM_MSG_PREPARE_TO_PLAY");

		    //LimaoApi_download("fileHash_001", 0, 100, 30);
		    //int xRet = LimaoApi_downloadExt("fileHash_002", 200, 50, 10);
		    //LimaoApi_isDownload("fileHash_003", 300, 50);
					
            break;

        case LM_MSG_DOWNLOAD_RSP:
        	ALOGD("LimaoApi: message_loop_n(): LM_MSG_DOWNLOAD_RSP");
        	//LimaoApi_prepareOK("fileHash_001");
			//
            break;

        default:
            ALOGD("LimaoApi: unknown msg: %d", msg.what);
            break;

        }
        
        //FIXME: 释放内存
    }

LABEL_RETURN:
    ;
}
#endif

static int message_loop(void *arg)
{
    ALOGD("LimaoApi: %s\n", __func__);

    JNIEnv *env = NULL;
    (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL );

    //IjkMediaPlayer *mp = (IjkMediaPlayer*) arg;
    //JNI_CHECK_GOTO(mp, env, NULL, "mpjni: native_message_loop: null mp", LABEL_RETURN);

		g_env = env;
		
    //message_loop_n(env);
    void (*f)(JNIEnv *env) = LimaoApi_get_msg_loop();
    f(env);
    
    //LABEL_RETURN:
    //ijkmp_dec_ref_p(&mp);

    (*g_jvm)->DetachCurrentThread(g_jvm);

    ALOGD("LimaoApi: message_loop exit");
    return 0;
}

static int createThread()
{
	int ret = 0;

	p_limaoJniStruct->msg_loop = message_loop;

	p_limaoJniStruct->msg_queue = &(p_limaoJniStruct->_msg_queue);

	msg_queue_init(p_limaoJniStruct->msg_queue);

	msg_queue_start(p_limaoJniStruct->msg_queue);

	p_limaoJniStruct->msg_thread = SDL_CreateThreadEx(&p_limaoJniStruct->_msg_thread, p_limaoJniStruct->msg_loop, NULL, "limao_msg_loop");

	return ret;
}

static void LimaoApi_native_init(JNIEnv *env)
{
    //int ret = 0;

	ALOGD("LimaoApi_native_init()");

	createThread();
	
}

static void LimaoApi_native_deinit(JNIEnv *env)
{
	ALOGD("LimaoApi_native_deinit()");
}

static JNINativeMethod g_methods[] = {
    //{"av_base64_encode", "([B)Ljava/lang/String;", (void *) FFmpegApi_av_base64_encode},

    { "_native_init",            "()V",        (void *) LimaoApi_native_init },
    { "_native_deinit",         "()V",        (void *) LimaoApi_native_deinit },

    { "_prepareToPlay",       "(Ljava/lang/String;Ljava/lang/String;J)V",     (void *) LimaoApi_prepareToPlay },
    { "_downloadFinish",      "(Ljava/lang/String;I)V",                               (void *) LimaoApi_downloadFinish }

};

int LimaoApi_global_init(JavaVM *jvm, JNIEnv *env)
{
    int ret = 0;

	g_jvm = jvm;

    IJK_FIND_JAVA_CLASS(env, g_clazz.clazz, JNI_CLASS_PLAY_MANAGER);
    (*env)->RegisterNatives(env, g_clazz.clazz, g_methods, NELEM(g_methods));

//    IJK_FIND_JAVA_STATIC_METHOD(env, g_clazz.jmid_postEventFromNative, g_clazz.clazz,
//        "postEventFromNative", "(Ljava/lang/Object;IIILjava/lang/Object;)V");

    IJK_FIND_JAVA_STATIC_METHOD(env, g_clazz.jmid_c2j_prepareOK, g_clazz.clazz,
        "c2j_prepareOK", "(Ljava/lang/String;)V");

    IJK_FIND_JAVA_STATIC_METHOD(env, g_clazz.jmid_c2j_download, g_clazz.clazz,
        "c2j_download", "(Ljava/lang/String;IJJ)V");

    IJK_FIND_JAVA_STATIC_METHOD(env, g_clazz.jmid_c2j_downloadExt, g_clazz.clazz,
        "c2j_downloadExt", "(Ljava/lang/String;JJI)I");

    IJK_FIND_JAVA_STATIC_METHOD(env, g_clazz.jmid_c2j_isDownload, g_clazz.clazz,
        "c2j_isDownload", "(Ljava/lang/String;JJ)I");

    IJK_FIND_JAVA_STATIC_METHOD(env, g_clazz.jmid_c2j_getFilePath, g_clazz.clazz,
        "c2j_getFilePath", "(Ljava/lang/String;)Ljava/lang/String;");

    IJK_FIND_JAVA_STATIC_METHOD(env, g_clazz.jmid_c2j_getFileSize, g_clazz.clazz,
        "c2j_getFileSize", "(Ljava/lang/String;)J");

    return ret;
}

JavaVM * LimaoApi_get_JVM()
{
	return g_jvm;
}

MessageQueue * LimaoApi_get_msg_queue()
{
	return p_limaoJniStruct->msg_queue;
}

// JNI_OnLoad()

// JNI_OnUnload()
