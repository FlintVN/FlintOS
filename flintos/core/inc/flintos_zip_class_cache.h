
#ifndef __FLINTOS_ZIP_CLASS_CACHE_H
#define __FLINTOS_ZIP_CLASS_CACHE_H

#include "flint_dictionary.h"
#include "flint_zip_file_reader.h"

class ZipClassCache {
private:
    static bool add(const char *jar);

    ZipClassCache(const ZipClassCache &) = delete;
    void operator=(const ZipClassCache &) = delete;
public:
    static bool init(void);
    static ZipEntry *find(const char *clsName, uint16_t length = 0xFFFF);
};

#endif /* __FLINTOS_ZIP_CLASS_CACHE_H */
