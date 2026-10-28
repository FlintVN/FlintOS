
#include <new>
#include "flint_system_api.h"
#include "flintos_default_apps.h"

class DefaultAppsDictNode : public DictNode {
private:
    const char *program;
    uint32_t hash;
    char extName[];

    DefaultAppsDictNode(const char *extName, uint16_t extLen, const char *program) {
        this->program = program;
        this->hash = Hash(extName, extLen);
        this->extName[extLen] = 0;
        memcpy(this->extName, extName, extLen);
    }
public:
    uint32_t getHashKey(void) const override {
        return hash;
    }

    int32_t compareKey(const char *key, uint16_t length) const override {
        return strncmp(key, extName, length);
    }

    int32_t compareKey(DictNode *other) const override {
        return strcmp(extName, ((DefaultAppsDictNode *)other)->extName);
    }

    const char *getProgram(void) {
        return program;
    }
private:
    DefaultAppsDictNode(const DefaultAppsDictNode &) = delete;
    void operator=(const DefaultAppsDictNode &) = delete;

    friend class DefaultApps;
};

static FDict<DefaultAppsDictNode> appsDefault;

bool DefaultApps::add(const char *extName, uint16_t extLen, const char *program) {
    DefaultAppsDictNode *node = (DefaultAppsDictNode *)FlintAPI::System::malloc(sizeof(DefaultAppsDictNode) + extLen + 1);
    if (node == NULL)
        return false;
    new (node)DefaultAppsDictNode(extName, extLen, program);
    appsDefault.add(node);
    return true;
}

const char *DefaultApps::find(const char *extName, uint16_t length) {
    DefaultAppsDictNode *node = appsDefault.find(extName, length);
    if (node == NULL)
        return NULL;
    return node->getProgram();
}
