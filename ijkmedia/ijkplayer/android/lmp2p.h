#ifndef LMP2P_H
#define LMP2P_H
#include <stdint.h> // define int8_t...

#ifdef WIN32
#ifdef LMP2P_EXPORTS
#define LMP2P_API __declspec(dllexport)
#else
#define LMP2P_API __declspec(dllimport)
#endif
#else
#define LMP2P_API __attribute__ ((visibility("default")))
#endif

#ifdef __cplusplus
namespace limao
{
extern "C"
{
#endif

// designed to check file downloading progress
typedef struct _DownloadFileInfo
{
    uint64_t downloaded_size;
    uint64_t file_size;
    char     md4[48];        // "limaoso://ed2k:md4", if md4 is NULL,  reset to ""
    char     sha1[56];       // "limaoso://btih:sha1",if sha1 is NULL, reset to ""
    char     filepath[1024]; //if filepath is NULL, reset to ""
} DownloadFileInfo;

// API defination for P2PControl begin
/**
 * @param uri magnet or ed2k
 * @param file_path the file path result, the caller should allocate the memory and free it
 */
LMP2P_API void GetFilePath(const char* uri, char* file_path);

/**
 * Get the file size
 * @param uri magnet or ed2k
 * @return if > 0, file exists, if == 0, file does not exist or other exceptions
 */
LMP2P_API uint64_t GetFileSize(const char* uri);

/**
 * Download specified file by uri, if offset == 0 & size == 0, download whole file.
 * Non-blocking download
 * @return 0 for success, non-zero if failed
 */
LMP2P_API int Download(const char* uri, uint64_t offset, uint64_t size);

/**
 * Blocking download
 * @param uri, magnet or ed2k
 * @param offset from where to start downloading
 * @param size how many Bytes to download
 * @param quit, stop wait and quit
 * @param timeout: seconds
 * @return 0 for success, non-zero if failed
 */
LMP2P_API int WaitFinish(const char* uri, uint64_t offset, uint64_t size, volatile const int *quit, int timeout);

/**
 * @param uri magnet or ed2k
 * @return >=0 for success, failed otherwise
 */
LMP2P_API int GetDownloadSpeed(const char* uri);

/**
 * Get how many bytes has been downloaded of the downloading file specified by uri
 * @param uri magnet or ed2k
 * @return >=0 for success, <0 for failure
 */
LMP2P_API int64_t GetDownloadedFileSize(const char* uri);

/**
 * Get all downloading files' downloading progress
 * @param dlFileInfo store downloading files' progress info
 * @param cnt the array size of dlFileInfo[]
              if cnt == 0, return the actual Downloading files count
 * @warn  the caller should allocate dlFileInfo memory and free it
 * @return N >= 0 for success, N files found, < 0 if failed
 */
LMP2P_API int GetAllFilesProgressInfo(DownloadFileInfo* dlFileInfo, const int cnt);

/**
 * @param uri magnet or ed2k
 * @return 0 for success, non-zero if failed
 */
LMP2P_API int StopDownload(const char* uri);

/**
 * Delete the specified downloaded file by uri
 * @param uri magnet or ed2k
 * @return 0 for success, non-zero if failed
 */
LMP2P_API int DeleteDownload(const char* uri);

/**
 * Delete All downloaded files
 * @reutrn file num that how many files have been deleted
 */
LMP2P_API int DeleteAllDownload();

/**
 * Notify p2pclient the status of Wifi
 * @param flag 0 for Wifi off, 1 for Wifi on
 * @reutrn 0 for success, non-zero if failed
 */
LMP2P_API int NotifyWifi(int flag);
// API defination for P2PControl end

/**
 * Set the path where the files store
 * @param downloadPath The destination path where to store the files
 * @return 0 for success, non-zero if failed
 */
LMP2P_API int SetDownloadPath(const char* downloadPath);


// API defination for P2PService begin
/**
 * Start Lmp2pservice
 * @param downloadPath where the files store
 * @param savaPath where the 'sava' file stores
 * @return windows: 0 for success, non-zero if falied
           android: P2pClient listening port, > 0 for success, < 0 for failure
 */
LMP2P_API int StartP2pService(const char* savePath, const char* downloadPath);

LMP2P_API void StopService();
// API defination for P2PService end


// API defination for BT Download begin
/**
 * Notify P2PClient the new file downloaded by BitTorrent
 * @param infoFilepath the local infofilepath that created according to Video file, utf-8 formatted
 * @return 0 for success, non-zero if failed
 */
LMP2P_API int AddBtDownloadedFile(const char* infoFilepath);

/**
 * Delete the specified file downloaded by BitTorrent
 * @param uri magnet, format like: magnet:?xt=urn:btih:c12fe1c06bba254a9dc9f519b335aa7c1367a88a
 * @return 0 for success, non-zero if failed
 */
LMP2P_API int DeleteBtDownloadedFile(const char* uri);
// API defination for BT Download end


#ifdef __cplusplus
};   // cplusplus
}    // namespace declaration
#endif

#endif //LMP2P_H

