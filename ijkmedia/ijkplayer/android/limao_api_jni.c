/*
 * limao_api_jni.c
 */

#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <jni.h>

#include "limao_api_jni.h"
#include "limao_msg_loop.h"

#include "../ff_ffinc.h"
#include "../ff_ffmsg_queue.h"

#include "ijksdl/ijksdl_thread.h"
#include "ijksdl/ijksdl_log.h"
#include "ijksdl/android/ijksdl_android_jni.h"
#include "../mediadownloadmodule/mediafile_download_log.h"

#include "lmp2p.h"
#define JNI_CLASS_PLAY_MANAGER "com/limaoso/phonevideo/playmanager/PlayManager"

typedef struct limao_api_fields_t {
    jclass clazz;

	jmethodID jmid_c2j_postMsgToUI;
	jmethodID jmid_c2j_prepareOK;
	jmethodID jmid_c2j_bufferingUpdate;
	jmethodID jmid_c2j_download;
	jmethodID jmid_c2j_downloadExt;
	jmethodID jmid_c2j_isDownload;
	jmethodID jmid_c2j_getFilePath;
	jmethodID jmid_c2j_getFileSize;
	jmethodID jmid_c2j_hasEnoughMemory;
	jmethodID jmid_c2j_notifyNotEnoughMemory;
	jmethodID jmid_c2j_p2pDownloadFailed;

	jmethodID jmid_c2j_MQ_map_add;
	jmethodID jmid_c2j_MQ_map_remove;
	jmethodID jmid_c2j_MQ_map_get;

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

static int g_env_flag = 0; // is use g_env

static int64_t g_startTime = 0; // ms

static int64_t g_playRequestTime = 0; // ms

static pthread_mutex_t g_mutex_4_time = PTHREAD_MUTEX_INITIALIZER;

static limao_api_fields_t g_clazz;

static LimaoJniStruct s_limaoJniStruct;

static LimaoJniStruct *p_limaoJniStruct = &s_limaoJniStruct;

int g_isTest= 0;


int g_offset = 0;
int g_blocksize = 0;
static int g_quit = 0; //p2p  quit to waitfinsh function
FILE * log_File = NULL;
#if 0
static void LimaoApi_postMsgToUI(int msgID, int arg1, int arg2, char *str)
{
	//	LimaoApi_postMsgToUI(LM_MSG_DOWNLOAD_REQ, NULL, 0, 0);
	//(*env)->CallStaticVoidMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_postMsgToUI, msgID, arg1, arg2, NULL );
}
#endif

static void LimaoApi_prepareToPlay(JNIEnv *env, jclass clazz, jstring fileHash, jstring filenameExtension, jlong fileSize, jlong startTime, jlong curTime)
{
	limao_api_param_4_prepareToPlay_t *param = malloc(sizeof(limao_api_param_4_prepareToPlay_t));
	// FIXME -- malloc --null

	LimaoApi_set_playRequestTime(curTime);

	const char *c_fileHash = (*env)->GetStringUTFChars(env, fileHash, NULL);
	const char *c_filenameExtension = (*env)->GetStringUTFChars(env, filenameExtension, NULL);

	ALOGD("LimaoApi_prepareToPlay: fileHash | filenameExtension | fileSize | startTime = %s %s %lld %lld", c_fileHash, c_filenameExtension, fileSize, startTime);
	strcpy(param->fileHash, c_fileHash);
	strcpy(param->filenameExtension, c_filenameExtension);
	param->logFile = log_File;
	g_quit = 0;
	param->p2pQuit = &g_quit;

	if(strcmp(c_filenameExtension,"test") == 0)
	{
		g_isTest = 1;
	}else
	{
		g_isTest = 0;
	}

	param->fileSize = fileSize;
	param->playRequestTime = curTime;

	g_startTime = startTime;

	(*env)->ReleaseStringUTFChars(env, fileHash, c_fileHash);
	(*env)->ReleaseStringUTFChars(env, filenameExtension, c_filenameExtension);

	msg_queue_put_simple5(p_limaoJniStruct->msg_queue, LM_MSG_PREPARE_TO_PLAY, param);

	g_offset = 0;
	g_blocksize = 0;
}
void LimaoApi_stopP2pDownload(JNIEnv *env, jclass clazz,jstring strfileHash)
{
	printf_log(LOG_WARN|LOG_FILE,"ijk","lmk WLimaoApi_stopP2pDownload",log_File);
	g_quit = 1;
	msg_queue_put_simple1(p_limaoJniStruct->msg_queue, LM_MSG_QUIT_THREAD);

	const char * c_fileHash = (*env)->GetStringUTFChars(env, strfileHash, NULL);

	if(0 != StopDownload(c_fileHash))
	{
		printf_log(LOG_ERROR|LOG_FILE,c_fileHash,"lmk StopDownload ERROR",log_File);
	}else
	{
		printf_log(LOG_ERROR|LOG_FILE,c_fileHash,"lmk StopDownload SUCCESS",log_File);
	}
	(*env)->ReleaseStringUTFChars(env, strfileHash, c_fileHash);
	g_offset = 0;
	g_blocksize = 0;

}
void LimaoApi_prepareOK(char *fileHash, char * filePath)
{
	JNIEnv *env = NULL;

	if (g_env_flag == 1)
	{
		env = g_env;
	} else {
		pthread_key_t key = LimaoApi_get_pthread_key();
		ThreadLocalData_t *tld = pthread_getspecific(key);
		env = tld->env;
	}

	jstring strHash = (*env)->NewStringUTF(env, fileHash);

	jstring strPath = (*env)->NewStringUTF(env, filePath);

	(*env)->CallStaticVoidMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_prepareOK, strHash, strPath);

	(*env)->DeleteLocalRef(env, strHash);

	(*env)->DeleteLocalRef(env, strPath);


}

void LimaoApi_bufferingUpdate(char *fileHash, int percent)
{
//	LimaoApi_postMsgToUI(LM_MSG_BUFFERING_UPDATE, NULL, 0, 0);

	JNIEnv *env = NULL;

	if (g_env_flag == 1)
	{
		env = g_env;
	} else {
		pthread_key_t key = LimaoApi_get_pthread_key();
		ThreadLocalData_t *tld = pthread_getspecific(key);
		env = tld->env;
	}

	jstring str = (*env)->NewStringUTF(env, fileHash);

	(*env)->CallStaticVoidMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_bufferingUpdate, str, percent);

	(*env)->DeleteLocalRef(env, str);
}

int LimaoApi_download(char *fileHash, int64_t offset, int64_t size)
{
//	LimaoApi_postMsgToUI(LM_MSG_DOWNLOAD_REQ, NULL, 0, 0);

	int ret = 0;

	JNIEnv *env = NULL;

	if (g_env_flag == 1)
	{
		env = g_env;
	} else {
		pthread_key_t key = LimaoApi_get_pthread_key();
		ThreadLocalData_t *tld = pthread_getspecific(key);
		env = tld->env;
	}
    if(g_isTest)
    {
    	jstring str = (*env)->NewStringUTF(env, fileHash);

    	jlong tmp_offset = offset;
    	jlong tmp_size = size;
    	ret = (*env)->CallStaticIntMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_download, str, tmp_offset, tmp_size);

    	(*env)->DeleteLocalRef(env, str);

    }else
    {
    	char logbuf[300] = {0};
    	int64_t tmpSize = 0;
    	ret = Download(fileHash, offset, /*size*/tmpSize);// 0 mean to download to file end
		sprintf(logbuf,"Download in offset : %llu, size  : %llu",offset, size);
		printf_log(LOG_WARN|LOG_FILE,"ijk",logbuf,log_File);

    	g_offset = offset;
    	g_blocksize = size;
    	if(ret != 0)
    	{
    		jstring str = (*env)->NewStringUTF(env, fileHash);
    		jlong fileSize = GetFileSize(fileHash);
    		(*env)->CallStaticVoidMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_p2pDownloadFailed,str, 0, fileSize);
    		(*env)->DeleteLocalRef(env, str);
    	}
    }


	return ret;
}

int LimaoApi_downloadExt(char *fileHash, int64_t offset, int64_t size,volatile const int *quit, int timeout)
{
	int ret = 0;

	JNIEnv *env = NULL;

	char logbuf[300] = {0};
	if (g_env_flag == 1)
	{
		env = g_env;
	} else {
		pthread_key_t key = LimaoApi_get_pthread_key();
		ThreadLocalData_t *tld = pthread_getspecific(key);
		env = tld->env;
	}
    if(g_isTest)
    {
    	jstring str = (*env)->NewStringUTF(env, fileHash);

    	jlong tmp_offset = offset;
    	jlong tmp_size = size;
    	//ret = (*env)->CallStaticIntMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_downloadExt, str, offset, size, timeout);
    	ret = (*env)->CallStaticIntMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_downloadExt, str, tmp_offset, tmp_size, timeout);

    	(*env)->DeleteLocalRef(env, str);
    }else
    {

    	if ((g_offset > offset) || (g_offset + g_blocksize < offset+ size))  // 查询过界，发送下载命令
    	{
    		ret = Download(fileHash, offset, size);
    		sprintf(logbuf,"LimaoApi_downloadExt no seed download order , download : %llu, size  : %llu",offset, size);
    		printf_log(LOG_WARN|LOG_FILE,"ijk",logbuf,log_File);
    		if(ret != 0)
    		{
        		sprintf(logbuf,"Download in offset : %llu, size  : %llu failed",offset, size);
        		printf_log(LOG_ERROR|LOG_FILE,"ijk",logbuf,log_File);
        		jstring str = (*env)->NewStringUTF(env, fileHash);
        		jlong fileSize = GetFileSize(fileHash);
        		(*env)->CallStaticVoidMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_p2pDownloadFailed,str, 0, fileSize);
        		(*env)->DeleteLocalRef(env, str);
    			return -1;
    		}
    	}

    	ret = WaitFinish(fileHash, offset, size, &g_quit,  timeout);
    	//sprintf(logbuf,"WaitFinish in offset :quit = %d,  %llu, size  : %llu, ret = %d",g_quit,offset, size, ret);
    	//printf_log(LOG_ERROR|LOG_FILE,"ijk",logbuf,log_File);
    	if(ret != 0)
    	{
    		jstring str = (*env)->NewStringUTF(env, fileHash);
    		jlong fileSize = GetFileSize(fileHash);
    		(*env)->CallStaticVoidMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_p2pDownloadFailed,str, g_quit==1 ? 1 : 0, fileSize);
    		(*env)->DeleteLocalRef(env, str);
    	}
    }


	return ret;
}

int LimaoApi_isDownload(char *fileHash, int64_t offset, int64_t size)
{
	int ret = 0;

	JNIEnv *env = NULL;

	if (g_env_flag == 1)
	{
		env = g_env;
	} else {
		pthread_key_t key = LimaoApi_get_pthread_key();
		ThreadLocalData_t *tld = pthread_getspecific(key);
		env = tld->env;
	}

	jstring str = (*env)->NewStringUTF(env, fileHash);

	ret = (*env)->CallStaticIntMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_isDownload, str, offset, size);

	(*env)->DeleteLocalRef(env, str);

	return ret;
}

void LimaoApi_getFilePath(/*IN*/char *fileHash, /*OUT*/char *filePath)
{
	//int ret = 0;

	JNIEnv *env = NULL;

	if (g_env_flag == 1)
	{
		env = g_env;
	} else {
		pthread_key_t key = LimaoApi_get_pthread_key();
		ThreadLocalData_t *tld = pthread_getspecific(key);
		env = tld->env;
	}
	if(g_isTest)
	{
		jstring strHash = (*env)->NewStringUTF(env, fileHash);

		jstring strPath = (*env)->CallStaticObjectMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_getFilePath, strHash);

		const char * c_filePath = (*env)->GetStringUTFChars(env, strPath, NULL); // _xxx

		memset(filePath, 0, 4);
		strcpy(filePath, c_filePath);
		(*env)->ReleaseStringUTFChars(env, strPath, c_filePath);
		(*env)->DeleteLocalRef(env, strHash);
		(*env)->DeleteLocalRef(env, strPath);
	}else
	{
		GetFilePath(fileHash, filePath);
	}


}

int64_t LimaoApi_getFileSize(char *fileHash)
{
	//int ret = 0;

	JNIEnv *env = NULL;
	int64_t fsize = 1;
	if (g_env_flag == 1)
	{
		env = g_env;
	} else {
		pthread_key_t key = LimaoApi_get_pthread_key();
		ThreadLocalData_t *tld = pthread_getspecific(key);
		env = tld->env;
	}
	if(g_isTest)
	{
		jstring strHash = (*env)->NewStringUTF(env, fileHash);

		fsize = (*env)->CallStaticLongMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_getFileSize, strHash);

		(*env)->DeleteLocalRef(env, strHash);
	}else
	{
		fsize = GetFileSize(fileHash);
		char logbuf[300] ={0};
		sprintf(logbuf,"GetFileSize: fsize = %lld",fsize);
		printf_log(LOG_INFO|LOG_FILE,
				   "ijk",
				   logbuf,
				   log_File);
	}




	return fsize;
}
static jint LimaoApi_getP2pDownloadSpeed(JNIEnv *env, jclass clazz, jstring fileHash)
{
	jint ret = 100;
	char logbuf[300] ={0};
	const char * c_fileHash = (*env)->GetStringUTFChars(env, fileHash, NULL);
	ret = GetDownloadSpeed((const char*)c_fileHash);
	/*sprintf(logbuf,"LimaoApi_getP2pDownloadSpeed: fileHash | speed = %s %d", c_fileHash, ret);
	printf_log(LOG_INFO|LOG_FILE,
			   "ijk",
			   logbuf,
			   log_File);*/
	(*env)->ReleaseStringUTFChars(env, fileHash, c_fileHash);
	return ret;
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

int LimaoApi_hasEnoughMemory(int64_t fileSize)
{
	int ret = 0;

	JNIEnv *env = NULL;

	if (g_env_flag == 1)
	{
		env = g_env;
	} else {
		pthread_key_t key = LimaoApi_get_pthread_key();
		ThreadLocalData_t *tld = pthread_getspecific(key);
		env = tld->env;
	}

	ret = (*env)->CallStaticIntMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_hasEnoughMemory, fileSize);

	return ret;
}

void LimaoApi_notifyNotEnoughMemory(char *fileHash, int64_t fileSize)
{
	//int ret = 0;

	JNIEnv *env = NULL;

	if (g_env_flag == 1)
	{
		env = g_env;
	} else {
		pthread_key_t key = LimaoApi_get_pthread_key();
		ThreadLocalData_t *tld = pthread_getspecific(key);
		env = tld->env;
	}

	jstring str = (*env)->NewStringUTF(env, fileHash);

	(*env)->CallStaticVoidMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_notifyNotEnoughMemory, str, fileSize);

	(*env)->DeleteLocalRef(env, str);
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
        case FFP_MSG_FLUSH: // first msg to queue
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

        //FIXME: free msg mem
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

	printf_log(LOG_INFO|LOG_FILE,"ijk","LimaoApi_native_init open log file /sdcard/limao/player.log",NULL);
	log_File  = fopen("/sdcard/limao/player.log","w+");
	createThread();

}

static void LimaoApi_native_deinit(JNIEnv *env)
{
	printf_log(LOG_INFO|LOG_FILE,"ijk","LimaoApi_native_deinit close log file /sdcard/limao/player.log",log_File);
	if(log_File)
	{
		fclose(log_File);
		log_File = NULL;
	}
}

static JNINativeMethod g_methods[] = {
    //{"av_base64_encode", "([B)Ljava/lang/String;", (void *) FFmpegApi_av_base64_encode},

    { "_native_init",            "()V",        (void *) LimaoApi_native_init },
    { "_native_deinit",         "()V",        (void *) LimaoApi_native_deinit },

    { "_prepareToPlay",       "(Ljava/lang/String;Ljava/lang/String;JJJ)V",     (void *) LimaoApi_prepareToPlay },
    { "_stopP2pDownload",     "(Ljava/lang/String;)V",							(void *) LimaoApi_stopP2pDownload },
    { "_downloadFinish",      "(Ljava/lang/String;I)V",                               (void *) LimaoApi_downloadFinish },
    { "_getP2pDownloadSpeed", "(Ljava/lang/String;)I",                               (void *) LimaoApi_getP2pDownloadSpeed }

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
        "c2j_prepareOK", "(Ljava/lang/String;Ljava/lang/String;)V");

    IJK_FIND_JAVA_STATIC_METHOD(env, g_clazz.jmid_c2j_bufferingUpdate, g_clazz.clazz,
        "c2j_bufferingUpdate", "(Ljava/lang/String;I)V");

    IJK_FIND_JAVA_STATIC_METHOD(env, g_clazz.jmid_c2j_download, g_clazz.clazz,
        "c2j_download", "(Ljava/lang/String;JJ)I");

    IJK_FIND_JAVA_STATIC_METHOD(env, g_clazz.jmid_c2j_downloadExt, g_clazz.clazz,
        "c2j_downloadExt", "(Ljava/lang/String;JJI)I");

    IJK_FIND_JAVA_STATIC_METHOD(env, g_clazz.jmid_c2j_isDownload, g_clazz.clazz,
        "c2j_isDownload", "(Ljava/lang/String;JJ)I");

    IJK_FIND_JAVA_STATIC_METHOD(env, g_clazz.jmid_c2j_getFilePath, g_clazz.clazz,
        "c2j_getFilePath", "(Ljava/lang/String;)Ljava/lang/String;");

    IJK_FIND_JAVA_STATIC_METHOD(env, g_clazz.jmid_c2j_getFileSize, g_clazz.clazz,
        "c2j_getFileSize", "(Ljava/lang/String;)J");

    IJK_FIND_JAVA_STATIC_METHOD(env, g_clazz.jmid_c2j_hasEnoughMemory, g_clazz.clazz,
        "c2j_hasEnoughMemory", "(J)I");

    IJK_FIND_JAVA_STATIC_METHOD(env, g_clazz.jmid_c2j_notifyNotEnoughMemory, g_clazz.clazz,
        "c2j_notifyNotEnoughMemory", "(Ljava/lang/String;J)V");

    IJK_FIND_JAVA_STATIC_METHOD(env, g_clazz.jmid_c2j_p2pDownloadFailed, g_clazz.clazz,
        "c2j_p2pDownloadFailed", "(Ljava/lang/String;IJ)V");


    IJK_FIND_JAVA_STATIC_METHOD(env, g_clazz.jmid_c2j_MQ_map_add, g_clazz.clazz,
        "c2j_MQ_map_add", "(JJ)V");

    IJK_FIND_JAVA_STATIC_METHOD(env, g_clazz.jmid_c2j_MQ_map_remove, g_clazz.clazz,
        "c2j_MQ_map_remove", "(J)V");

    IJK_FIND_JAVA_STATIC_METHOD(env, g_clazz.jmid_c2j_MQ_map_get, g_clazz.clazz,
        "c2j_MQ_map_get", "(J)J");

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

int64_t LimaoApi_get_start_time()
{
	ALOGD("LimaoApi_get_start_time(): %lld", g_startTime);
	return g_startTime;
}

int64_t LimaoApi_get_playRequestTime()
{
	ALOGD("LimaoApi_get_playRequestTime(): %lld", g_playRequestTime);

	pthread_mutex_lock(&g_mutex_4_time);
	int64_t ret = g_playRequestTime;
	pthread_mutex_unlock(&g_mutex_4_time);

	return ret;
}

void LimaoApi_set_playRequestTime(int64_t time)
{
	pthread_mutex_lock(&g_mutex_4_time);
	g_playRequestTime = time;
	pthread_mutex_unlock(&g_mutex_4_time);

	ALOGD("LimaoApi_set_playRequestTime(): %lld", g_playRequestTime);
}

void LimaoApi_MQ_map_add(int64_t time, void *ptr)
{
	JNIEnv *env = NULL;

	if (g_env_flag == 1)
	{
		env = g_env;
	} else {
		pthread_key_t key = LimaoApi_get_pthread_key();
		ThreadLocalData_t *tld = pthread_getspecific(key);
		env = tld->env;
	}

	ALOGD("LimaoApi_MQ_map_add(): time | ptr = %lld %p", time, ptr);
	jlong tmp_ptr = (jlong) ptr;
	(*env)->CallStaticVoidMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_MQ_map_add, time, tmp_ptr);
}

void LimaoApi_MQ_map_remove(int64_t time)
{
	JNIEnv *env = NULL;

	if (g_env_flag == 1)
	{
		env = g_env;
	} else {
		pthread_key_t key = LimaoApi_get_pthread_key();
		ThreadLocalData_t *tld = pthread_getspecific(key);
		env = tld->env;
	}

	ALOGD("LimaoApi_MQ_map_remove(): time = %lld", time);
	(*env)->CallStaticVoidMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_MQ_map_remove, time);
}

void * LimaoApi_MQ_map_get(int64_t time)
{
	JNIEnv *env = NULL;

	void *ptr;

    (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

	jlong ret = (*env)->CallStaticLongMethod(env, g_clazz.clazz, g_clazz.jmid_c2j_MQ_map_get, time);
	if (ret == 0)
		ptr = NULL;
	else
		ptr = (void *)ret;

	(*g_jvm)->DetachCurrentThread(g_jvm);

	ALOGD("LimaoApi_MQ_map_get(): time | ptr = %lld %p", time, ptr);

	return ptr;
}

// JNI_OnLoad()

// JNI_OnUnload()
