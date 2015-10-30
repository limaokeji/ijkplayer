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
#include "../mediadownloadmodule/mediafile_downld_module.h"
#include "../mediadownloadmodule/mediafile_download_log.h"

static pthread_key_t pthread_key_1;

static void start_routine_new(ThreadLocalData_t *pData)
{
	pthread_t threadID = pthread_self();
	ALOGD("LimaoApi: start_routine_new():threadID = %ld", threadID);
	ALOGD("LimaoApi: start_routine_new(): %s %s", pData->fileHash, pData->filenameExtension);

	pthread_setname_np(threadID, "limao_thread_n");

	//pthread_key_create(&pthread_key_1, NULL);
	pthread_setspecific (pthread_key_1, pData);

	// thread entry function
	media_file_download_module_thread((void *)pData);

	//

#if 0 // test code
	for (int i = 0; i < 600; i++)
	{
		//LimaoApi_download("fileHash_005", i, i*10, 100);

		int xRet = LimaoApi_downloadExt("fileHash_001", i, i*10, 10);
		ALOGD("LimaoApi: start_routine_new(): xRet = %d", xRet);
		
		LimaoApi_isDownload("fileHash_002", 300, 50);
		
		char filePath[100];
		LimaoApi_getFilePath("fileHash_003", filePath);
		ALOGD("LimaoApi: start_routine_new(): filePath = %s", filePath);
		
		int64_t fsize = LimaoApi_getFileSize("fileHash_004");
		ALOGD("LimaoApi: start_routine_new(): fsize = %lld", fsize);

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
	int block_count;
	int block_index = 0;
	char * mediafile_hash ;
	char * suffix_name ;
	limao_api_param_4_prepareToPlay_t *param;
	DOWNLOADBLOCKINFO * pdownload_blockinfo_list = NULL;


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
        case FFP_MSG_FLUSH: // first msg to queue
            ALOGD("LimaoApi: message_loop_x(): FFP_MSG_FLUSH");
            break;

        case LM_MSG_PREPARE_TO_PLAY:  //user want to paly . begin to downlaod
        	ALOGD("LimaoApi: message_loop_x(): LM_MSG_PREPARE_TO_PLAY");

        	param = msg.data;
        	mediafile_hash = param->fileHash;
        	suffix_name = param->filenameExtension;

        	printf_log(LOG_ERROR,
        			mediafile_hash,
        			suffix_name,
                			   	NULL);
        	if(!mediafile_downld_module_init(mediafile_hash, suffix_name, NULL, 0,
        			&g_pdownload_blockinfo_list, NULL))
        	{
        		printf_log(LOG_ERROR,
        			   "ijkplayer media file download medule thread",
        			   "mediafile download module init failed.\n",
        			   	NULL);
        		break;

        	}
        	printf_log(LOG_INFO,
        				   "ijkplayer media file download medule thread ",
        				   "mediafile_downld_module_init ok",
        				   	NULL);
        	if(!mediafile_downld_module_getrootbox_offset())
        	{
        		printf_log(LOG_ERROR,
        			   "ijkplayer media file download medule thread",
        			   "mediafile download module get root box offset failed.\n",
        			   	NULL);
        		break;
        	}
        	printf_log(LOG_INFO,
        				   "ijkplayer media file download medule thread",
        				   "mediafile_downld_module_getrootbox_offset ok",
        				   	NULL);
        	if(!mediafile_downld_module_download_playerinfobox())
        	{
        		printf_log(LOG_ERROR,
        			   "ijkplayer media file download medule thread",
        			   "mediafile download module get play info box failed.\n",
        			   	NULL);
        		break;
        	}

        	block_count = mediafile_downld_module_getmediadatalock_count();

        	if(!mediafile_downld_module_download_mediadatablock(0))
        	{
				printf_log(LOG_ERROR,
					   "ijkplayer media file download medule thread",
					   "mediafile download block failed --- 0.\n",
					   	NULL);
        	}
           	if(!mediafile_downld_module_download_mediadatablock(1))
			{
				printf_log(LOG_ERROR,
					   "ijkplayer media file download medule thread",
					   "mediafile download block failed --- 1.\n",
						NULL);
			}
           	if(!mediafile_downld_module_download_mediadatablock(2))
			{
				printf_log(LOG_ERROR,
					   "ijkplayer media file download medule thread",
					   "mediafile download block failed --- 2.\n",
						NULL);
			}
           	LimaoApi_prepareOK(mediafile_hash);

			LimaoApi_download(mediafile_hash, 0, LimaoApi_getFileSize(mediafile_hash)); // _test

           	msg_queue_put_simple2(LimaoApi_get_msg_queue(), LM_MSG_P2P_DOWNLOAD_BLOCK, 3);
            break;

        case LM_MSG_P2P_DOWNLOAD_BLOCK:

        	block_index  = msg.arg1;
        	ALOGI("LM_MSG_P2P_WONDLOAD_BLOCK in. %d.",block_index);
        	if((block_index<0)||(block_index >= block_count))
        	{
				printf_log(LOG_ERROR,
					   "ijkplayer media file download medule thread",
					   "the download index is invalid.\n",
					   	NULL);
				break;
        	}
        	if(!mediafile_downld_module_download_mediadatablock(block_index))
        	{
				printf_log(LOG_ERROR,
					   "ijkplayer media file download medule thread",
					   "mediafile download block failed.\n",
					   	NULL);
        	}

        	if(block_index<block_count)
        	{
        		msg_queue_put_simple2(LimaoApi_get_msg_queue(), LM_MSG_P2P_DOWNLOAD_BLOCK, block_index+1);

        	}else if(block_index == block_count)
        	{
				printf_log(LOG_INFO,
					   "ijkplayer media file download medule thread",
					   "mediafile download complete.\n",
					   	NULL);
        	}
        	break;

        case LM_MSG_PLAYER_SEEK:
        	block_index = msg.arg1;
        	ALOGI("LM_MSG_PLAYER_SEEK in. %d.",block_index);
			if((block_index<0)||(block_index >= block_count))
			{
				printf_log(LOG_ERROR,
					   "ijkplayer media file download medule thread",
					   "the download index is invalid.\n",
						NULL);
				break;
			}
			if(!mediafile_downld_module_download_mediadatablock(block_index))
			{
				printf_log(LOG_ERROR,
					   "ijkplayer media file download medule thread",
					   "mediafile download block failed.\n",
						NULL);
			}
			msg_queue_remove(LimaoApi_get_msg_queue(), LM_MSG_P2P_DOWNLOAD_BLOCK); //delete the download msg

			if(block_index<block_count)
			{
				msg_queue_put_simple2(LimaoApi_get_msg_queue(), LM_MSG_P2P_DOWNLOAD_BLOCK, block_index+1);
			}else if(block_index == block_count)
			{
				printf_log(LOG_INFO,
					   "ijkplayer media file download medule thread",
					   "mediafile download complete.\n",
						NULL);
			}

        	break;
        default:
            ALOGD("LimaoApi: unknown msg: %d", msg.what);
            break;

        }
        
        //FIXME: free msg mem
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
