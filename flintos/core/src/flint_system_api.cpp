
#include "flint.h"
#include "flint_system_api.h"
#include "flint_file_reader.h"
#include "flint_zip_file_reader.h"
#include "flintos_zip_class_cache.h"

bool FlintAPI::System::openClassFile(FExec *ctx, const char *clsName, uint16_t length, FileReader *reader) {
    ZipEntry *entry = ZipClassCache::find(clsName, length);
    if (entry == NULL) return false;
    ZipFileReader zip(ctx, entry->getZipFilePath());
    if (!zip.open()) return false;
    if (zip.openEntry(entry)) {
        memcpy(reader, &zip, sizeof(FileReader));
        return true;
    }
    zip.close();
    return false;
}
