#include <jni.h>
#include <android/log.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctime>
#include <cstdio>
#include <string>
#include "config.h"
#include "plugins.h"

#define TAG "libsamp"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

static SampConfig g_cfg;

// Пишет файл-маркер в папку игры: Android/data/<пакет>/files/libsamp_loaded.txt
static void WriteMarker(const char* stage, uintptr_t base) {
    std::string pkg = GetPackageName();
    std::string dir = "/sdcard/Android/data/" + pkg + "/files";
    mkdir(dir.c_str(), 0777);
    std::string path = dir + "/libsamp_loaded.txt";
    FILE* f = fopen(path.c_str(), "w");
    if (!f) { LOGI("marker write failed: %s", path.c_str()); return; }
    fprintf(f, "libsamp v2.00 skeleton\nstage=%s\ntime=%ld\npackage=%s\n"
               "libGTASA_base=0x%lx\nhost=%s\nport=%d\nnick=%s\n"
               "monetloader=%d\ncleo=%d\n",
            stage, (long)time(nullptr), pkg.c_str(), (unsigned long)base,
            g_cfg.host.c_str(), g_cfg.port, g_cfg.nick.c_str(),
            g_cfg.loadMonet ? 1 : 0, g_cfg.loadCleo ? 1 : 0);
    fclose(f);
    LOGI("marker written: %s (%s)", path.c_str(), stage);
}

// TODO: здесь будут хуки в libGTASA.so (меню, ввод, чат) и сетевой слой.
static void* MainThread(void*) {
    LoadConfig(g_cfg);
    LOGI("config: %s:%d nick=%s", g_cfg.host.c_str(), g_cfg.port, g_cfg.nick.c_str());
    WriteMarker("loaded", 0);

    uintptr_t base = 0;
    for (int i = 0; i < 600 && !base; i++) {   // ждём до 60 сек
        base = FindModuleBase("libGTASA.so");
        if (!base) usleep(100 * 1000);
    }
    if (!base) { LOGI("libGTASA.so not found"); WriteMarker("gtasa_not_found", 0); return nullptr; }
    LOGI("libGTASA base = 0x%lx", (unsigned long)base);
    WriteMarker("gtasa_found", base);

    LoadPlugins(g_cfg);
    return nullptr;
}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM*, void*) {
    LOGI("libsamp loaded (v2.00 skeleton)");
    pthread_t t;
    pthread_create(&t, nullptr, MainThread, nullptr);
    return JNI_VERSION_1_6;
}

// Вызывается лаунчером. Пока только сохраняет параметры.
extern "C" void samp_connect(const char* host, int port, const char* nick) {
    g_cfg.host = host; g_cfg.port = port; g_cfg.nick = nick;
    LOGI("connect request %s:%d as %s", host, port, nick);
    // TODO: инициализация RakNet и подключение
}
