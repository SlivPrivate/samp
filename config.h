#pragma once
#include <string>
struct SampConfig {
    std::string host = "127.0.0.1";
    int port = 7777;
    std::string nick = "Player";
    bool loadMonet = false;
    bool loadCleo = false;
};
std::string GetPackageName();
bool LoadConfig(SampConfig& out);
