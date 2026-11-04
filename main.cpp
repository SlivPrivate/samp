#include <jni.h>
#include <android/log.h>
#include <pthread.h>
#include <unistd.h>
#include "config.h"
#include "plugins.h"

#define TAG "libsamp"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

static SampConfig g_cfg;

// TODO: здесь будут хуки в libGTASA.so (меню, ввод, чат) и сетевой слой.
static void* MainThread(void*) {
    LoadConfig(g_cfg);
    LOGI("config: %s:%d nick=%s", g_cfg.host.c_str(), g_cfg.port, g_cfg.nick.c_str());

    uintptr_t base = 0;
    for (int i = 0; i < 600 && !base; i++) {   // ждём до 60 сек
        base = FindModuleBase("libGTASA.so");
        if (!base) usleep(100 * 1000);
    }
    if (!base) { LOGI("libGTASA.so not found"); return nullptr; }
    LOGI("libGTASA base = 0x%lx", (unsigned long)base);

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
