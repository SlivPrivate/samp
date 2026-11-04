#include "plugins.h"
#include <android/log.h>
#include <dlfcn.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#define TAG "libsamp"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

uintptr_t FindModuleBase(const char* name) {
    FILE* f = fopen("/proc/self/maps", "r");
    if (!f) return 0;
    char line[512];
    uintptr_t base = 0;
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, name)) { base = strtoull(line, nullptr, 16); break; }
    }
    fclose(f);
    return base;
}

static void TryLoad(const char* lib) {
    void* h = dlopen(lib, RTLD_NOW | RTLD_GLOBAL);
    LOGI("dlopen %s -> %s", lib, h ? "OK" : dlerror());
}

// Положи libmonetloader.so / libcleo.so в lib/<abi>/ внутри APK.
void LoadPlugins(const SampConfig& cfg) {
    if (cfg.loadMonet) TryLoad("libmonetloader.so");
    if (cfg.loadCleo) TryLoad("libcleo.so");
}
