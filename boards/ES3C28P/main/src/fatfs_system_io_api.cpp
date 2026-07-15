
#include "ff.h"
#include "flint.h"
#include "flint_common.h"
#include "flint_system_api.h"

using namespace FlintAPI::IO;

static FileResult convertFileResult(FRESULT ret) {
    static const FileResult map[] = {
        [FR_OK] = FILE_RESULT_OK,
        [FR_DISK_ERR] = FILE_RESULT_ERR,
        [FR_INT_ERR] = FILE_RESULT_ERR,
        [FR_NOT_READY] = FILE_RESULT_ERR,
        [FR_NO_FILE] = FILE_RESULT_NO_PATH,
        [FR_NO_PATH] = FILE_RESULT_NO_PATH,
        [FR_INVALID_NAME] = FILE_RESULT_NO_PATH,
        [FR_DENIED] = FILE_RESULT_DENIED,
        [FR_EXIST] = FILE_RESULT_EXIST,
        [FR_INVALID_OBJECT] = FILE_RESULT_ERR,
        [FR_WRITE_PROTECTED] = FILE_RESULT_WRITE_PROTECTED,
        [FR_INVALID_DRIVE] = FILE_RESULT_ERR,
        [FR_NOT_ENABLED] = FILE_RESULT_ERR,
        [FR_NO_FILESYSTEM] = FILE_RESULT_ERR,
        [FR_MKFS_ABORTED] = FILE_RESULT_ERR,
        [FR_TIMEOUT] = FILE_RESULT_ERR,
        [FR_LOCKED]= FILE_RESULT_BUSY,
        [FR_NOT_ENOUGH_CORE] = FILE_RESULT_ERR,
        [FR_TOO_MANY_OPEN_FILES] = FILE_RESULT_TOO_MANY_OPEN_FILES,
        [FR_INVALID_PARAMETER] = FILE_RESULT_ERR,
    };
    return map[ret];
}

static uint32_t startWith(const char *str, const char *prefix) {
    uint32_t i = 0;
    for(; str[i] && prefix[i]; i++) {
        if(str[i] != prefix[i])
            return 0;
    }
    return (prefix[i]) == 0 ? i : 0;
}

static uint32_t diskMapping(const char **path, char *buf, uint32_t bufLen) {
    typedef struct {
        const char *basePath;
        uint32_t diskNum;
    } MappingInfo;

    static MappingInfo diskMappingInfo[] = {
        {"/mnt/sd0", 1}
    };

    for(uint32_t i = 0; i < LENGTH(diskMappingInfo); i++) {
        uint32_t prefixLen = startWith(*path, diskMappingInfo[i].basePath);
        if(prefixLen > 0) {
            snprintf(buf, bufLen, "%d:/%s", (int)diskMappingInfo[i].diskNum, &(*path)[prefixLen + 1]);
            *path = buf;
            return diskMappingInfo[i].diskNum;
        }
    }
    return 0;
}

FileResult FlintAPI::IO::finfo(const char *fileName, FileInfo *fileInfo) {
    char buf[FILE_NAME_BUFF_SIZE];
    FRESULT ret;
    diskMapping(&fileName, buf, sizeof(buf));
    if(fileInfo != NULL) {
        FILINFO fno;
        ret = f_stat(fileName, &fno);
        if(ret == FR_OK) {
            uint16_t index = 0;
            uint16_t year = (fno.fdate >> 9) + 1980;
            uint8_t month = (fno.fdate >> 5) & 0x0F;
            uint8_t day = fno.fdate & 0x1F;
            uint8_t hour = fno.ftime >> 11;
            uint8_t minute = (fno.ftime >> 5) & 0x3F;
            uint8_t second = (fno.ftime & 0x1F) * 2;

            fileInfo->attribute = fno.fattrib;
            fileInfo->size = (fileInfo->directory) ? 0 : fno.fsize;
            fileInfo->time = UnixTime(year, month, day, hour, minute, second);
            while(fno.fname[index] != 0) {
                if(index < (sizeof(fileInfo->name) - 1)) {
                    fileInfo->name[index] = fno.fname[index];
                    index++;
                }
                else
                    return FILE_RESULT_ERR;
            }
            fileInfo->name[index] = 0;
        }
    }
    else
        ret = f_stat(fileName, NULL);
    return convertFileResult(ret);
}

FileHandle FlintAPI::IO::fopen(const char *fileName, FileMode mode) {
    char buf[FILE_NAME_BUFF_SIZE];
    FIL *fp = (FIL *)FlintAPI::System::malloc(sizeof(FIL));
    if(fp == NULL) return NULL;
    memset(fp, 0, sizeof(FIL));
    diskMapping(&fileName, buf, sizeof(buf));
    if(f_open(fp, fileName, (BYTE)mode) != FR_OK) { FlintAPI::System::free(fp); return NULL; }
    return fp;
}

FileResult FlintAPI::IO::fread(FileHandle handle, void *buff, uint32_t btr, uint32_t *br) {
    UINT tmp;
    FileResult ret = convertFileResult(f_read((FIL *)handle, buff, btr, &tmp));
    *br = tmp;
    return ret;
}

FileResult FlintAPI::IO::fwrite(FileHandle handle, void *buff, uint32_t btw, uint32_t *bw) {
    UINT tmp;
    FileResult ret = convertFileResult(f_write((FIL *)handle, buff, btw, &tmp));
    *bw = tmp;
    return ret;
}

uint32_t FlintAPI::IO::fsize(FileHandle handle) {
    return f_size((FIL *)handle);
}

uint32_t FlintAPI::IO::ftell(FileHandle handle) {
    return f_tell((FIL *)handle);
}

FileResult FlintAPI::IO::fseek(FileHandle handle, uint32_t offset) {
    return convertFileResult(f_lseek((FIL *)handle, offset));
}

FileResult FlintAPI::IO::fclose(FileHandle handle) {
    if(handle != NULL) {
        FileResult ret = convertFileResult(f_close((FIL *)handle));
        FlintAPI::System::free(handle);
        return ret;
    }
    return FILE_RESULT_OK;
}

FileResult FlintAPI::IO::fremove(const char *fileName) {
    char buf[FILE_NAME_BUFF_SIZE];
    diskMapping(&fileName, buf, sizeof(buf));
    return convertFileResult(f_unlink(fileName));
}

FileResult FlintAPI::IO::frename(const char *oldName, const char *newName) {
    char buf1[FILE_NAME_BUFF_SIZE];
    char buf2[FILE_NAME_BUFF_SIZE];
    uint32_t diskOld = diskMapping(&oldName, buf1, sizeof(buf1));
    uint32_t diskNew = diskMapping(&newName, buf2, sizeof(buf2));
    if(diskOld != diskNew)
        return FILE_RESULT_ERR;
    return convertFileResult(f_rename(oldName, newName));
}

DirHandle FlintAPI::IO::opendir(const char *dirName) {
    char buf[FILE_NAME_BUFF_SIZE];
    FF_DIR *dir = (FF_DIR *)FlintAPI::System::malloc(sizeof(FF_DIR));
    if(dir == NULL) return NULL;
    diskMapping(&dirName, buf, sizeof(buf));
    FRESULT ret = f_opendir(dir, dirName);
    if(ret == FR_OK)
        return (void *)dir;
    else {
        FlintAPI::System::free(dir);
        return NULL;
    }
}

FileResult FlintAPI::IO::readdir(DirHandle handle, FileInfo *fileInfo) {
    FILINFO fno;
    FRESULT ret = f_readdir((FF_DIR *)handle, &fno);
    if(ret == FR_OK) {
        uint16_t index = 0;
        uint16_t year = (fno.fdate >> 9) + 1980;
        uint8_t month = (fno.fdate >> 5) & 0x0F;
        uint8_t day = fno.fdate & 0x1F;
        uint8_t hour = fno.ftime >> 11;
        uint8_t minute = (fno.ftime >> 5) & 0x3F;
        uint8_t second = (fno.ftime & 0x1F) * 2;

        fileInfo->attribute = fno.fattrib;
        fileInfo->size = (fileInfo->directory) ? 0 : fno.fsize;
        fileInfo->time = UnixTime(year, month, day, hour, minute, second);
        while(fno.fname[index] != 0) {
            if(index < (sizeof(fileInfo->name) - 1)) {
                fileInfo->name[index] = fno.fname[index];
                index++;
            }
            else
                return FILE_RESULT_ERR;
        }
        fileInfo->name[index] = 0;
    }
    return convertFileResult(ret);
}

FileResult FlintAPI::IO::closedir(DirHandle handle) {
    FRESULT ret = f_closedir((FF_DIR *)handle);
    FlintAPI::System::free(handle);
    return convertFileResult(ret);
}

FileResult FlintAPI::IO::mkdir(const char *path) {
    char buf[FILE_NAME_BUFF_SIZE];
    diskMapping(&path, buf, sizeof(buf));
    return convertFileResult(f_mkdir(path));
}
