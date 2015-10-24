package com.limaoso.phonevideo.playmanager;

import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;
import java.util.Timer;
import java.util.TimerTask;

import tv.danmaku.ijk.media.player.IjkMediaPlayer;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import com.limaoso.phonevideo.download.DownloadManager;

/**
 * 播放管理类
 */
public class PlayManager {

	private static final String TAG = "PlayManager";
	
	// 测试数据
	private static boolean isTest = true;
	//private static boolean isTest = false;
	private static int testProgress = 0;
	
	//private Context mContext;
	
	/**
	 * 以下消息ID必须与 limao_api_jni.h 中定义的数值相同。
	 */
	private static final int LM_MSG_PREPARE_OK = 22000;

	private static final int LM_MSG_DOWNLOAD_REQ = 22001;
	
    private static Handler sHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
        	String fileHash;
        	
            switch (msg.what) {
                case LM_MSG_PREPARE_OK: // 准备OK
                	Log.d(TAG, "sHandler.handleMessage(): LM_MSG_PREPARE_OK");
                	//
                	fileHash = (String) msg.obj;
                	if (mOnPreparedListener.containsKey(fileHash)) {
                		String filePath = str_oldFilePath; // 调用DM
                		mOnPreparedListener.get(fileHash).onPrepared(fileHash, filePath, 0);
                		clearOnPreparedListener(fileHash);                		
                	}
                    break;

                case LM_MSG_DOWNLOAD_REQ: // 下载请求
                	Log.d(TAG, "sHandler.handleMessage(): LM_MSG_DOWNLOAD_REQ");
                	//
                    break;
                
//                case 100: // DownloadManager 
//                	_downloadFinish("fileHash", 0);
//                	break;
            }
        }
    };
    
    static IjkMediaPlayer sIjkMediaPlayer = null; // for load .so
    
    static {
    	sIjkMediaPlayer = new IjkMediaPlayer();
    	initNativeOnce();
    	sIjkMediaPlayer = null;
    }
    
	/**
	 * 初始化 PlayManager
	 */
	private static void init() {
		//initNativeOnce();
	}
	
	/**
	 * 解初始化 PlayManager
	 */
	private static void deinit() {
		//native_deinit();
	}
	
    private static volatile boolean mIsNativeInitialized = false;
    private static void initNativeOnce() {
        synchronized (PlayManager.class) {
            if (!mIsNativeInitialized) {
                _native_init();
                mIsNativeInitialized = true;
            }
        }
    }
	
	private static native void _native_init();
	
	private static native void _native_deinit();
	
    public static interface OnPreparedListener {
        void onPrepared(String fileHash, String path, int progress);
    }
	
	private static HashMap<String, OnPreparedListener> mOnPreparedListener = new HashMap<String, OnPreparedListener>();
    
	private static void setOnPreparedListener(String fileHash, OnPreparedListener listener) {
    	//mOnPreparedListener = listener;
    	
//    	if (mOnPreparedListener.containsKey(fileHash)) {
//    		mOnPreparedListener.put(fileHash, listener);
//    	} else {
//    		
//    	}
    	
    	mOnPreparedListener.put(fileHash, listener);    	        
    }
	
	private static void clearOnPreparedListener(String fileHash) {
    	mOnPreparedListener.remove(fileHash);
    }
	
	private static Handler handlerUI = new Handler() {
        @Override
        public void handleMessage(Message msg) {
        	if (msg.what == 1) {
        		testProgress++;
        		
        		String fileHash = "18b1f7281dc128c3";
        		
        		String pathName = "/sdcard/001_lmv/vdata.lmv";
        		
				if (mOnPreparedListener.containsKey(fileHash) && testProgress >= 3) { // 3%
					mOnPreparedListener.get(fileHash).onPrepared(fileHash, "vdata_concat_path", testProgress);
					clearOnPreparedListener(fileHash);
					
				}
        	}
        } 
    };
	
    /**
     * 准备播放某个视频
     */
	public static void prepareToPlay(Context context, String fileHash, String filenameExtension, OnPreparedListener listener) {
		
		setOnPreparedListener(fileHash, listener);
		
		_prepareToPlay(fileHash, filenameExtension, 0);
		
		//_prepareToPlay(fileHash, filenameExtension, 0);
		//new VdataDownloadThread("", "", 0).start();
		
		//_downloadFinish(fileHash, 10);
		
		//c2j_downloadExt("fileHash", 20610000, 8887, 0);

//		for (int i=0; i < 600; i++) {
//			_prepareToPlay(fileHash, filenameExtension, i);
//			_downloadFinish(fileHash, i);
//		}

		// 测试代码 --- begin
//		int ret = 0;
//		
//		c2j_downloadExt("fileHash", 0, 100, 0);
//		c2j_downloadExt("fileHash", 1, 200, 0);
//				
//		ret = c2j_isDownload("fileHash", 0, 100); // 1
//		ret = c2j_isDownload("fileHash", 5, 300); // 1
//		ret = c2j_isDownload("fileHash", 100*1024, 500); // 0
//		
//		c2j_downloadExt("fileHash", 200*1024, 100*1024, 0);
//		
//		ret = c2j_isDownload("fileHash", 200*1024, 1000); // 1
//		ret = c2j_isDownload("fileHash", 200*1024 - 1, 1000); // 0
//		
//		c2j_downloadExt("fileHash", 500*1024, 200*1024, 0);
//		
//		ret = c2j_isDownload("fileHash", 500*1024, 200 * 1024); // 1
//		ret = c2j_isDownload("fileHash", 500*1024, 201 * 1024); // 0
//		ret = c2j_isDownload("fileHash", 600*1024, 100*1024); // 1
		
		// 测试代码 --- end
		
		if (1==1)
			return;
		
		// 测试代码
//		if (isTest) {
//		    Timer timer = new Timer();
//		    TimerTask timerTask = new TimerTask() {
//		        @Override
//		        public void run() {
//		        	handlerUI.sendEmptyMessage(1);
//		        }
//		    };
//			
//			timer.schedule(timerTask, 1000, 1000 * 3);
//			
//			return;
//		}
				
		//final DownloadManager dm = new DownloadManager(context);
		final DownloadManager dm = DownloadManager.getInstance();
				
		dm.startDownload(fileHash);
		
		//TODO:
		// 1. 如果文件已存在
		// 2. 如果文件已下载了x%

		// TODO: 如果有多个对象需要接收进度消息，怎么传递 ...
		dm.setOnProgressListener(new DownloadManager.OnProgressListener() {
			@Override
			public void onProgress(String fileHash, String pathName, int progress) {
				// 目前设置为 下载了1%的数据后即可以播放
				if (mOnPreparedListener.containsKey(fileHash) && progress >= 1) { // 1%
					
//					dm.cancelTimer();
//					dm.clearOnProgressListener();
					
					mOnPreparedListener.get(fileHash).onPrepared(fileHash, "vdata_concat_path", progress);
					clearOnPreparedListener(fileHash);
				}
			}
		});
	
	}
	
	public static native void _prepareToPlay(String fileHash, String filenameExtension, long fileSize);
	
	private static native void _downloadFinish(String fileHash, int index);
	
	// 转发到UI线程再处理
	private static void postMsgToUI(int msgID, int arg1, int arg2, Object obj) {
		Message msg = sHandler.obtainMessage();
		msg.what = msgID;
		msg.arg1 = arg1;
		msg.arg2 = arg2;
		msg.obj = obj;
		sHandler.sendMessage(msg);
	}

	private static void c2j_postMsgToUI(int msgID, int arg1, int arg2, String str) {
		postMsgToUI(msgID, arg1, arg2, str);
	}
	
	private static void c2j_prepareOK(String fileHash) {
		Log.d(TAG, "c2j_prepareOK(): fileHash = " + fileHash);
		
		postMsgToUI(LM_MSG_PREPARE_OK, 0, 0, fileHash);
	}
	
	// 非阻塞
	private static void c2j_download(String fileHash, int index, long offset, long size) {
		Log.d(TAG, "c2j_download(): fileHash | index | offset | size = " 
					+ fileHash + " " 
					+ index + " " + offset + " " + size);
		
		Bundle data = new Bundle();
		data.putLong("offset", offset);
		data.putLong("size", size);
				
		Message msg = sHandler.obtainMessage();
		msg.what = LM_MSG_DOWNLOAD_REQ;
		msg.arg1 = index;
		msg.arg2 = 0;
		msg.obj = fileHash;
		msg.setData(data);
		
		sHandler.sendMessage(msg);
	}

	// 测试数据
	private static final String str_oldFilePath = "/sdcard/001/old.mp4";
	private static final String str_newFilePath = "/sdcard/001/new.mp4";
	
	private static final long ONE_BLOCK_SIZE = 100 * 1024; // 以100KB为单位
	private static Set<Integer> dlFileBlockSet = new HashSet<Integer>(); 

	private static int copyBlock(long offset, long size) {
		long offset_end = offset + size - 1;
		
		int firstBlock = (int) (offset / ONE_BLOCK_SIZE);
		int lastBlock = (int) (offset_end / ONE_BLOCK_SIZE);
		
		long new_offset = firstBlock * ONE_BLOCK_SIZE;
		long new_size = (lastBlock - firstBlock + 1) * ONE_BLOCK_SIZE;
		
		// 最后一块往往是不足100KB的。
		long fileSize = c2j_getFileSize(str_oldFilePath);
		if (new_offset + new_size > fileSize)
			new_size = fileSize - new_offset;
		
		int ret = _copyBlock(new_offset, new_size);
		
		for (int i = firstBlock; i < lastBlock + 1; i++)
			dlFileBlockSet.add(i);

		return ret;
	}
		
	private static int _copyBlock(long offset, long size) {
		int ret = 0;
		
		File oldFile = new File(str_oldFilePath);
		File newFile = new File(str_newFilePath);
		
		if (dlFileBlockSet.isEmpty()) {
			genNewFile(oldFile.length());
		}

		byte[] buf = new byte[(int) size];
		
		RandomAccessFile raf_old = null;
		RandomAccessFile raf_new = null;
		
        try {
            raf_old = new RandomAccessFile(str_oldFilePath, "r");
            raf_new = new RandomAccessFile(str_newFilePath, "rw");
            
            raf_old.seek(offset);
            raf_old.read(buf);
            
            raf_new.seek(offset);
            raf_new.write(buf);

            raf_old.close();
            raf_new.close();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (raf_old != null) {
                try {
                	raf_old.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            
            if (raf_new != null) {
                try {
                	raf_new.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
		
		return ret;
	}
	
	private static int genNewFile(long fileSize) {
		int ret = 0;

		File newFile = new File(str_newFilePath);
		if (newFile.exists())
			newFile.delete();
		
		int blockNum = (int) (fileSize / ONE_BLOCK_SIZE);
		int leftBytes = (int) (fileSize % ONE_BLOCK_SIZE);
		
		byte[] buf = new byte[(int) ONE_BLOCK_SIZE];
		byte[] buf_1 = new byte[leftBytes];
		
		RandomAccessFile raf_new = null;
		
        try {
            raf_new = new RandomAccessFile(str_newFilePath, "rw");
        
            //raf_new.seek(0);
            for (int i = 0; i < blockNum; i++)
            	raf_new.write(buf);
            raf_new.write(buf_1);

            raf_new.close();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (raf_new != null) {
                try {
                	raf_new.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
		
		return ret;
	}
	
	// 阻塞
	private static int c2j_downloadExt(String fileHash, long offset, long size, int timeout) {
		Log.d(TAG, "c2j_downloadExt(): fileHash | offset | size | timeout = " 
					+ fileHash + " " + offset + " " + size + " " + timeout);
		
		// TODO：调用DM

		int flag = 0;
		while (true) {
			
			// check is download
			
			try {
				Thread.sleep(10);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			
			copyBlock(offset, size); // 测试代码
			
			flag++;
			if (flag >= 1)
				break;
		}
		
		return 0;
	}

	// 1-已下载，0-未下载
	private static int c2j_isDownload(String fileHash, long offset, long size) {
		Log.d(TAG, "c2j_isDownload(): fileHash | offset | size = " 
					+ fileHash + " " + offset + " " + size);
		
		// TODO：调用DM
		
		// 测试代码
		if (dlFileBlockSet.isEmpty())
			return 0;
		
		long offset_end = offset + size - 1;
		
		int firstBlock = (int) (offset / ONE_BLOCK_SIZE);
		int lastBlock = (int) (offset_end / ONE_BLOCK_SIZE);

		int flag = 1;
		for (int i = firstBlock; i < lastBlock + 1; i++) {
			if (!dlFileBlockSet.contains(i)) {
				flag = 0;
				break;
			}
		}
		
		return flag;
	}
	
	// 1-已下载，0-未下载
	private static String c2j_getFilePath(String fileHash) {

		// TODO：调用DM
		
		String filePath = str_newFilePath;
		
		Log.d(TAG, "c2j_getFilePath(): fileHash | filePath = " + fileHash + " " + filePath);
		
		return filePath;
	}
	
	// -1 表示获取失败
	private static long c2j_getFileSize(String fileHash) {
		long fsize = 0;
		
		// TODO：调用DM
		
		File oldFile = new File(str_oldFilePath);
		fsize = oldFile.length();
		Log.d(TAG, "c2j_getFileSize(): fileHash | fileSize = " + fileHash + " " + fsize);

		return fsize;
	}
	
}
