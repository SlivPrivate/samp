#include "config.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>

std::string GetPackageName() {
    char buf[256] = {0};
    FILE* f = fopen("/proc/self/cmdline", "r");
    if (!f) return "";
    fread(buf, 1, sizeof(buf) - 1, f);
    fclose(f);
    return std::string(buf);
}

// Файл: /sdcard/Android/data/<package>/files/samp_settings.ini
// host=1.2.3.4 / port=7777 / nick=Name / monetloader=1 / cleo=1
bool LoadConfig(SampConfig& c) {
    std::string path = "/sdcard/Android/data/" + GetPackageName() + "/files/samp_settings.ini";
    std::ifstream in(path);
    if (!in) return false;
    std::string line;
    while (std::getline(in, line)) {
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string k = line.substr(0, eq), v = line.substr(eq + 1);
        while (!v.empty() && (v.back() == '\r' || v.back() == ' ')) v.pop_back();
        if (k == "host") c.host = v;
        else if (k == "port") c.port = atoi(v.c_str());
        else if (k == "nick") c.nick = v;
        else if (k == "monetloader") c.loadMonet = v == "1";
        else if (k == "cleo") c.loadCleo = v == "1";
    }
    return true;
}
