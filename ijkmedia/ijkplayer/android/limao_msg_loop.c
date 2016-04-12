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

static void message_loop_x(ThreadLocalData_t *pData);

static pthread_key_t pthread_key_1;

static void start_routine_new(ThreadLocalData_t *pData)
{
	pthread_t threadID = pthread_self();
	ALOGD("LimaoApi: start_routine_new():threadID = %ld", threadID);
	ALOGD("LimaoApi: start_routine_new(): %s %s", pData->fileHash, pData->filenameExtension);

	pthread_setname_np(threadID, "limao_thread_n");

	//pthread_key_create(&pthread_key_1, NULL);
	pthread_setspecific (pthread_key_1, pData);

	LimaoApi_MQ_map_add(pData->playRequestTime, pData->msg_queue);

	// thread entry function
	//media_file_download_module_thread((void *)pData);
	message_loop_x(pData);

	LimaoApi_MQ_map_remove(pData->playRequestTime);

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

//static void message_loop_x(JNIEnv *env)
static void message_loop_x(ThreadLocalData_t *pData)
{
	int block_count;
	int block_index = 0;
	int64_t fileSize;
	char * mediafile_hash;
	char * suffix_name;
	char * filePath;
	limao_api_param_4_prepareToPlay_t *param;
	DOWNLOADBLOCKINFO * pdownload_blockinfo_list = NULL;
	void * g_downld_mediafile = NULL;

	MessageQueue *msg_queue = pData->msg_queue;
	//pthread_key_create(&pthread_key_1, NULL);
	char logbuf[200] = {0};
	FILE * g_mediafile_parse_log = NULL;
	int quit = 0;
	int* q2pQuit = NULL;
    while (1) {    	

    	if (quit == 1 || pData->playRequestTime != LimaoApi_get_playRequestTime())
    	{

        	mediafile_downld_module_finish(g_downld_mediafile);

    		printf_log(LOG_WARN | LOG_FILE,
    			   "ijkplayer media file download medule thread",
    			   "QUIT",
    			   g_mediafile_parse_log);
    	}


        AVMessage msg;

        int retval = msg_queue_get(msg_queue, &msg, 1);
        //ALOGD("LimaoApi: message_loop_x(): retval = %d", retval);
        if (retval < 0)
            break;

        // block-get should never return 0
        assert(retval > 0);

        switch (msg.what) {
        case FFP_MSG_FLUSH: // first msg to queue
            ALOGD("LimaoApi: message_loop_x(): FFP_MSG_FLUSH");
            break;

        case LM_MSG_PREPARE_TO_PLAY: // user want to play . begin to download
        	ALOGD("LimaoApi: message_loop_x(): LM_MSG_PREPARE_TO_PLAY");

			msg_queue_flush(msg_queue); // remove all msg

        	param = msg.data;
        	mediafile_hash = param->fileHash;
        	suffix_name = param->filenameExtension;
        	g_mediafile_parse_log = param->logFile;
        	q2pQuit = param->p2pQuit;
			LimaoApi_bufferingUpdate(mediafile_hash, 0);

        	printf_log(LOG_WARN|LOG_FILE,
        			mediafile_hash,
        			suffix_name,
        			g_mediafile_parse_log);
        	g_downld_mediafile = mediafile_downld_module_init(mediafile_hash, suffix_name, NULL, 0,
        			&g_pdownload_blockinfo_list, g_mediafile_parse_log);
        	if(g_downld_mediafile == NULL)
        	{
        		printf_log(LOG_ERROR | LOG_FILE,
        			   "ijkplayer media file download medule thread",
        			   "mediafile download module init failed.",
        			   g_mediafile_parse_log);
        		quit =1;
        		break;

        	}
        	printf_log(LOG_INFO | LOG_FILE,
        				   "ijkplayer media file download medule thread ",
        				   "mediafile_downld_module_init ok",
        				   g_mediafile_parse_log);
        				   	
            fileSize = LimaoApi_getFileSize(mediafile_hash);
			if (LimaoApi_hasEnoughMemory(fileSize) == 0)
			{
				printf_log(LOG_ERROR | LOG_FILE,
        			   "ijkplayer media file download medule thread",
        			   "Not has enough memory.\n",
        			   g_mediafile_parse_log);
        		LimaoApi_notifyNotEnoughMemory(mediafile_hash, fileSize);
        		break;
			}

  			LimaoApi_bufferingUpdate(mediafile_hash, 10);
  			
        	if(!mediafile_downld_module_getrootbox_offset(g_downld_mediafile))
        	{
        		printf_log(LOG_ERROR | LOG_FILE,
        			   "ijkplayer media file download medule thread",
        			   "mediafile download module get root box offset failed.\n",
        			   	g_mediafile_parse_log);
        		quit =1;


        		break;
        	}
        	printf_log(LOG_INFO | LOG_FILE,
        				   "ijkplayer media file download medule thread",
        				   "mediafile_downld_module_getrootbox_offset ok",
        				   g_mediafile_parse_log);
        	LimaoApi_bufferingUpdate(mediafile_hash, 20);

        	if(!mediafile_downld_module_download_playerinfobox(g_downld_mediafile))
        	{
        		printf_log(LOG_ERROR|LOG_FILE,
        			   "ijkplayer media file download medule thread",
        			   "mediafile download module get play info box failed.\n",
        			   g_mediafile_parse_log);
        		quit =1;


        		break;
        	}
        	LimaoApi_bufferingUpdate(mediafile_hash, 30);

        	block_count = mediafile_downld_module_getmediadatalock_count(g_downld_mediafile);

        	LimaoApi_download(mediafile_hash, 0, LimaoApi_getFileSize(mediafile_hash)); // _test

        	if(!mediafile_downld_module_download_mediadatablock(g_downld_mediafile,0))
        	{
				printf_log(LOG_ERROR|LOG_FILE,
					   "ijkplayer media file download medule thread",
					   "mediafile download block failed --- 0.",
					   g_mediafile_parse_log);
				quit =1;
				break;


        	}
        	LimaoApi_bufferingUpdate(mediafile_hash, 50);
           	if(!mediafile_downld_module_download_mediadatablock(g_downld_mediafile,1))
			{
				printf_log(LOG_ERROR|LOG_FILE,
					   "ijkplayer media file download medule thread",
					   "mediafile download block failed --- 1.\n",
					   g_mediafile_parse_log);
				quit =1;
				break;

			}
			LimaoApi_bufferingUpdate(mediafile_hash, 70);
           	if(!mediafile_downld_module_download_mediadatablock(g_downld_mediafile,2))
			{
				printf_log(LOG_ERROR|LOG_FILE,
					   "ijkplayer media file download medule thread",
					   "mediafile download block failed --- 2.\n",
					   g_mediafile_parse_log);
				quit =1;
				break;
			}
			LimaoApi_bufferingUpdate(mediafile_hash, 90);

			filePath =mediafile_downld_module_download_mediafilepath(g_downld_mediafile);
           	LimaoApi_prepareOK(mediafile_hash,filePath);



           	msg_queue_put_simple2(msg_queue, LM_MSG_P2P_DOWNLOAD_BLOCK, 3);
            break;

        case LM_MSG_P2P_DOWNLOAD_BLOCK:

        	block_index  = msg.arg1;
        	ALOGI("LM_MSG_P2P_WONDLOAD_BLOCK in. %d.",block_index);
        	if((block_index<0)||(block_index >= block_count))
        	{
				printf_log(LOG_ERROR|LOG_FILE,
					   "ijkplayer media file download medule thread",
					   "the download index is invalid.\n",
					   g_mediafile_parse_log);
				break;
        	}
        	if(block_index == block_count)
			{
				printf_log(LOG_WARN|LOG_FILE,
					   "ijkplayer media file download medule thread",
					   "mediafile download complete.",
					   g_mediafile_parse_log);
				break;
			}

        	if(!mediafile_downld_module_download_mediadatablock(g_downld_mediafile,block_index))
        	{
				printf_log(LOG_ERROR|LOG_FILE,
					   "ijkplayer media file download medule thread",
					   "mediafile download block failed.\n",
					   g_mediafile_parse_log);

				quit =1;

				//msg_queue_put_simple1(msg_queue, LM_MSG_QUIT_THREAD);
				break;

        	}

        	if(block_index<block_count)
        	{
        		msg_queue_put_simple2(msg_queue, LM_MSG_P2P_DOWNLOAD_BLOCK, block_index+1);

        	}
        	break;

        case LM_MSG_PLAYER_SEEK:
        	block_index = msg.arg1;
        	sprintf(logbuf,"the download index is %d",block_index);
			printf_log(LOG_INFO|LOG_FILE,
				   "ijkplayer media file download medule thread",
				   logbuf,
				   g_mediafile_parse_log);
        	ALOGI("LM_MSG_PLAYER_SEEK in. %d.",block_index);
        	LimaoApi_bufferingUpdate(mediafile_hash, 0);
			if((block_index<=0)||(block_index >= block_count))
			{
				printf_log(LOG_WARN|LOG_FILE,
					   "ijkplayer media file download medule thread",
					   "the download index is invalid.",
					   g_mediafile_parse_log);
				block_index = 3;
			}
			DOWNLOADBLOCKINFO * download_blockinfo_list = mediafile_downld_module_getblocklistinfo(g_downld_mediafile);
			if(download_blockinfo_list == NULL)
			{
				printf_log(LOG_ERROR|LOG_FILE,
					   "ijkplayer media file download medule thread",
					   "download_blockinfo_list is invalid",
					   g_mediafile_parse_log);

				quit =1;
				break;
			}
			block_index = block_index -1;
			int64_t offset = 0;
			if((download_blockinfo_list+block_index)->offset <1024)
			{
				offset =(download_blockinfo_list+block_index)->offset;
			}else
			{
				offset =(download_blockinfo_list+block_index)->offset  -1024;
			}

			LimaoApi_download(mediafile_hash, offset, LimaoApi_getFileSize(mediafile_hash)-offset);

			if(!mediafile_downld_module_download_mediadatablock(g_downld_mediafile,block_index))
			{
				printf_log(LOG_ERROR|LOG_FILE,
					   "ijkplayer media file download medule thread",
					   "mediafile download block failed",
					   g_mediafile_parse_log);
				quit =1;
				break;

			}
			msg_queue_remove(msg_queue, LM_MSG_P2P_DOWNLOAD_BLOCK); //delete the download msg

			if(block_index<block_count)
			{
				msg_queue_put_simple2(msg_queue, LM_MSG_P2P_DOWNLOAD_BLOCK, block_index+1);
			}else if(block_index == block_count)
			{
				printf_log(LOG_INFO|LOG_FILE,
					   "ijkplayer media file download medule thread",
					   "mediafile download complete",
					   g_mediafile_parse_log);
			}

        	break;

        case LM_MSG_QUIT_THREAD:
        	quit = 1;
        	break;

        default:
            ALOGD("LimaoApi: unknown msg: %d", msg.what);
            break;

        }
        
        //FIXME: free msg mem
    }

}

static void message_loop_M(JNIEnv *env)
{
	ALOGD("LimaoApi: message_loop_M()");

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
        case FFP_MSG_FLUSH: // �������Ϣ���н��յĵ�һ����Ϣ
            ALOGD("LimaoApi: message_loop_M(): FFP_MSG_FLUSH");
            break;

        case LM_MSG_PREPARE_TO_PLAY:
        	ALOGD("LimaoApi: message_loop_M(): LM_MSG_PREPARE_TO_PLAY");

			limao_api_param_4_prepareToPlay_t *param = msg.data;

			ThreadLocalData_t *threadLocalData = (ThreadLocalData_t *) malloc(sizeof(ThreadLocalData_t));
			strcpy(threadLocalData->fileHash, param->fileHash);
			strcpy(threadLocalData->filenameExtension, param->filenameExtension);
			threadLocalData->fileSize = param->fileSize;
			threadLocalData->playRequestTime = param->playRequestTime;

			ALOGD("param->playRequestTime: %lld", param->playRequestTime);

			threadLocalData->msg_queue = &(threadLocalData->_msg_queue);
			msg_queue_init(threadLocalData->msg_queue);
			msg_queue_start(threadLocalData->msg_queue);
			
			pthread_t newThread = 0;
			pthread_create(&newThread, NULL, thread_start_routine, threadLocalData);             
          
            // transmit LM_MSG_PREPARE_TO_PLAY msg
          	limao_api_param_4_prepareToPlay_t *newParam = malloc(sizeof(limao_api_param_4_prepareToPlay_t));
            
            strcpy(newParam->fileHash, param->fileHash);
			strcpy(newParam->filenameExtension, param->filenameExtension);
			newParam->logFile = param->logFile;
			newParam->p2pQuit = param->p2pQuit;
			msg_queue_put_simple5(threadLocalData->msg_queue, LM_MSG_PREPARE_TO_PLAY, newParam);
            
            break;

        default:
            ALOGD("LimaoApi: message_loop_M(): unknown msg: %d", msg.what);
            break;

        }
        
        //FIXME: �ͷ��ڴ�
    }

}

void * LimaoApi_get_msg_loop()
{
	ALOGD("LimaoApi_get_msg_loop()");
	//void (*f)(JNIEnv *env) = message_loop_x;
	void (*f)(JNIEnv *env) = message_loop_M;
	return f;
}

pthread_key_t LimaoApi_get_pthread_key()
{
	return pthread_key_1;
}
