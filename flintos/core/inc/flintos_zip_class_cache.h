
#ifndef __FLINTOS_ZIP_CLASS_CACHE_H
#define __FLINTOS_ZIP_CLASS_CACHE_H

#include "flint_dictionary.h"
#include "flint_zip_file_reader.h"

class ZipEntryDictNode : public DictNode {
private:
    ZipEntry entry;
    uint32_t hash;
    char entryName[];

    ZipEntryDictNode(ZipEntry *entry, const char *name, uint32_t nameLen);
public:
    uint32_t getHashKey(void) const override;
    int32_t compareKey(const char *key, uint16_t length) const override;
    int32_t compareKey(DictNode *other) const override;

    ZipEntry *getEntry(void);
private:
    ZipEntryDictNode(const ZipEntryDictNode &) = delete;
    void operator=(const ZipEntryDictNode &) = delete;

    friend class ZipClassCache;
};

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
