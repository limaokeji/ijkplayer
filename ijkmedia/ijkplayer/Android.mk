#
# Copyright (c) 2013 Zhang Rui <bbcallen@gmail.com>
#
# This file is part of ijkPlayer.
#
# ijkPlayer is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# ijkPlayer is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with ijkPlayer; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

LOCAL_PATH := $(call my-dir)
#include $(call all-subdir-makefiles)
include $(CLEAR_VARS)
# -mfloat-abi=soft is a workaround for FP register corruption on Exynos 4210
# http://www.spinics.net/lists/arm-kernel/msg368417.html
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_CFLAGS += -mfloat-abi=soft
endif
LOCAL_CFLAGS += -std=c99
LOCAL_LDLIBS += -llog -landroid
LOCAL_ALLOW_UNDEFINED_SYMBOLS := true
# add by lmk
LOCAL_CFLAGS += -D_ANDROID 
LOCAL_CFLAGS += -D_FILE_OFFSET_BITS=64 # largefile  lseek64
LOCAL_CFLAGS += -D_LARGEFILE64_SOURCE  # largefile  lseek64
#LOCAL_CFLAGS += -Wno-error=format-security	
LOCAL_CPPFLAGS += -fexceptions
LOCAL_CPPFLAGS += -fpermissive

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(realpath $(LOCAL_PATH)/..)
LOCAL_C_INCLUDES += $(MY_APP_FFMPEG_INCLUDE_PATH)

LOCAL_SRC_FILES += ff_cmdutils.c
LOCAL_SRC_FILES += ff_ffplay.c
LOCAL_SRC_FILES += ff_ffpipeline.c
LOCAL_SRC_FILES += ff_ffpipenode.c
LOCAL_SRC_FILES += ijkmeta.c
LOCAL_SRC_FILES += ijkplayer.c

LOCAL_SRC_FILES += pipeline/ffpipeline_ffplay.c
LOCAL_SRC_FILES += pipeline/ffpipenode_ffplay_vdec.c
LOCAL_SRC_FILES += pipeline/ffpipenode_ffplay_vout.c

LOCAL_SRC_FILES += android/ffmpeg_api_jni.c
LOCAL_SRC_FILES += android/ijkplayer_android.c
LOCAL_SRC_FILES += android/ijkplayer_jni.c
LOCAL_SRC_FILES += android/limao_api_jni.c    # add by lmk
LOCAL_SRC_FILES += android/limao_msg_loop.c   
LOCAL_SRC_FILES += android/limao_mediafile_download.c   

LOCAL_SRC_FILES += android/pipeline/ffpipeline_android.c
LOCAL_SRC_FILES += android/pipeline/ffpipenode_android_mediacodec_vdec.c
LOCAL_SRC_FILES += android/pipeline/ffpipenode_android_mediacodec_vout.c

# add by lmk
LOCAL_SRC_FILES += 	 mediadownloadmodule/mp4parse/mp4parse/Atom.cpp \
			 mediadownloadmodule/mp4parse/mp4parse/Mp4Parse.cpp \
			 mediadownloadmodule/mp4parse/mp4parse/Stbl.cpp \
			 mediadownloadmodule/mp4parse/mp4parse/Stco.cpp \
			 mediadownloadmodule/mp4parse/mp4parse/Stsc.cpp \
			 mediadownloadmodule/mp4parse/mp4parse/Stsd.cpp \
			 mediadownloadmodule/mp4parse/mp4parse/Stss.cpp \
			 mediadownloadmodule/mp4parse/mp4parse/Stsz.cpp \
			 mediadownloadmodule/mp4parse/mp4parse/Stts.cpp \
			 mediadownloadmodule/mp4parse/Mp4DownldMediaFile.cpp \
			 mediadownloadmodule/rmvbparse/RmvbDownldMediaFile.cpp \
			 mediadownloadmodule/mkvparse/MkvDownldMediaFile.cpp \
			 mediadownloadmodule/rmvbparse/RmvbPrase.cpp \
			 mediadownloadmodule/DownldMediaFile.cpp \
			 mediadownloadmodule/DataParse.cpp \
		   	 mediadownloadmodule/mediafile_download_log.cpp \
		   	 mediadownloadmodule/mediafile_downld_module.cpp

LOCAL_SHARED_LIBRARIES := ijkffmpeg ijksdl lmp2p
LOCAL_STATIC_LIBRARIES := android-ndk-profiler

LOCAL_MODULE := ijkplayer
include $(BUILD_SHARED_LIBRARY)
