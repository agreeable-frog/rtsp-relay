#pragma once

#include <json/json.h>
#include <string>

struct AddressPool {
    AddressPool(){};
    std::string ipMin = "";
    std::string ipMax = "";
    uint portMin = 0;
    uint portMax = 0;
    uint ttl = 0;
};

struct Mount {
    Mount(){};
    std::string mountPoint = "";
    std::string rtspSrc = "";
    bool shared = true;
    bool bindMcastAddress = false;
    std::string mcastIface = "";
    uint maxMcastTtl = 0;
    bool setProtocolsUDPMcast = false;
    bool enableRtcp = true;
    uint latency = 0;
    bool hasAddressPool = false;
    AddressPool addressPool;
};

struct ServerConfig {
    ServerConfig(const std::string& path);
    std::string ip = "";
    std::string port = "";
    std::vector<Mount> mounts;
};