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

# add by lmk
LOCAL_CFLAGS += -D_ANDROID
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
LOCAL_SRC_FILES += 	 mediadownloadmodule/mp4parse/sources/3gp.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_amr.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_avc1.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_avcC.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_d263.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_damr.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_dref.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_elst.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_enca.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_encv.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_free.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_ftyp.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_hdlr.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_hinf.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_hnti.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_href.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_mdat.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_mdhd.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_meta.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_mp4s.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_mp4v.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_mvhd.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_ohdr.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_root.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_rtp.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_s263.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_sdp.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_smi.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_sound.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_standard.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_stbl.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_stdp.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_stsc.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_stsd.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_stsz.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_tfhd.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_tkhd.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_treftype.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_trun.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_udta.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_url.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_urn.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_video.cpp \
			 mediadownloadmodule/mp4parse/sources/atom_vmhd.cpp \
			 mediadownloadmodule/mp4parse/sources/descriptors.cpp \
			 mediadownloadmodule/mp4parse/sources/isma.cpp \
			 mediadownloadmodule/mp4parse/sources/mp4atom.cpp \
			 mediadownloadmodule/mp4parse/sources/mp4container.cpp \
			 mediadownloadmodule/mp4parse/sources/mp4.cpp \
			 mediadownloadmodule/mp4parse/sources/mp4descriptor.cpp \
			 mediadownloadmodule/mp4parse/sources/mp4file.cpp \
			 mediadownloadmodule/mp4parse/sources/mp4file_io.cpp \
			 mediadownloadmodule/mp4parse/sources/mp4info.cpp \
			 mediadownloadmodule/mp4parse/sources/mp4meta.cpp \
			 mediadownloadmodule/mp4parse/sources/mp4property.cpp \
			 mediadownloadmodule/mp4parse/sources/mp4track.cpp \
			 mediadownloadmodule/mp4parse/sources/mp4util.cpp \
			 mediadownloadmodule/mp4parse/sources/ocidescriptors.cpp \
			 mediadownloadmodule/mp4parse/sources/odcommands.cpp \
			 mediadownloadmodule/mp4parse/sources/qosqualifiers.cpp \
			 mediadownloadmodule/mp4parse/sources/rtphint.cpp \
			 mediadownloadmodule/mp4parse/Mp4DownldMediaFile.cpp \
			 mediadownloadmodule/rmvbparse/RmvbDownldMediaFile.cpp \
			 mediadownloadmodule/rmvbparse/RmvbPrase.cpp \
			 mediadownloadmodule/DownldMediaFile.cpp \
			 mediadownloadmodule/DataParse.cpp \
		   	 mediadownloadmodule/mediafile_download_log.cpp \
		   	 mediadownloadmodule/mediafile_downld_module.cpp

LOCAL_SHARED_LIBRARIES := ijkffmpeg ijksdl
LOCAL_STATIC_LIBRARIES := android-ndk-profiler

LOCAL_MODULE := ijkplayer
include $(BUILD_SHARED_LIBRARY)
