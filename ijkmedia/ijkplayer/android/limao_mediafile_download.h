/*
 * limao_mediafile_download.h
 */

#ifndef LIMAO_MEDIAFILE_DOWNLOAD_H_
#define LIMAO_MEDIAFILE_DOWNLOAD_H_

#include "../mediadownloadmodule/DataType.h"

static DOWNLOADBLOCKINFO * g_pdownload_blockinfo_list = NULL;
static int g_media_download_blockCount = 0;

void media_file_download_module_thread(void *argv);

#endif
