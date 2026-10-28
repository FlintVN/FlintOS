
#include <new>
#include <string.h>
#include "flint.h"
#include "flint_common.h"
#include "flint_system_api.h"
#include "flintos_zip_class_cache.h"

class ZipEntryDictNode : public DictNode {
private:
    ZipEntry entry;
    uint32_t hash;
    char entryName[];

    ZipEntryDictNode(ZipEntry *entry, const char *name, uint32_t nameLen) {
        this->entry = *entry;
        this->hash = Hash(name, nameLen);
        this->entryName[nameLen] = 0;
        memcpy(this->entryName, name, nameLen);
    }
public:
    uint32_t getHashKey(void) const override {
        return hash;
    }

    int32_t compareKey(const char *key, uint16_t length) const override {
        return strncmp(entryName, key, length);
    }

    int32_t compareKey(DictNode *other) const override {
        return strcmp(entryName, ((ZipEntryDictNode *)other)->entryName);
    }

    ZipEntry *getEntry(void) {
        return &entry;
    }
private:
    ZipEntryDictNode(const ZipEntryDictNode &) = delete;
    void operator=(const ZipEntryDictNode &) = delete;

    friend class ZipClassCache;
};

static FDict<ZipEntryDictNode> entryCache;

bool ZipClassCache::init(void) {
    static const char *jars[] = {
        "/lib/flintos.jar",
        "/lib/flint.drawing.jar",
        "/lib/flint.net.jar",
        "/lib/java.base.jar",
        "/lib/midp.jar",
    };

    for (uint32_t i = 0; i < LENGTH(jars); i++)
        if (!add(jars[i])) return false;

    return true;
}

bool ZipClassCache::add(const char *jar) {
    char buff[FILE_NAME_BUFF_SIZE];
    ZipFileReader zip(NULL, jar);
    if (zip.open()) {
        ZipEntry *entry;
        zip.reset();
        while ((entry = zip.nextEntry()) != NULL) {
            uint32_t len = entry->getNameLength();
            if (len <= 6)
                continue;
            if (!zip.readEntryName(buff, sizeof(buff))) {
                zip.close();
                return false;
            }
            if (strncmp(&buff[len - 6], ".class", 6) == 0) {
                ZipEntryDictNode *node = (ZipEntryDictNode *)FlintAPI::System::malloc(sizeof(ZipEntryDictNode) + (len - 6 + 1));
                new (node)ZipEntryDictNode(entry, buff, len - 6);
                entryCache.add(node);
            }
        }
        return zip.close();
    }
    return false;
}

ZipEntry *ZipClassCache::find(const char *clsName, uint16_t length) {
    ZipEntryDictNode *node = entryCache.find(clsName, length);
    if (node == NULL)
        return NULL;
    return node->getEntry();
}
