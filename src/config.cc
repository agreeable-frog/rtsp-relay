#include "config.hh"

#include <iostream>
#include <fstream>
#include <sstream>

ServerConfig::ServerConfig(const std::string& path) {
    std::ifstream ifs(path,
                      std::ios::binary);
    if (!ifs.is_open()) {
        throw std::runtime_error("Could not open config file.");
    }
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    std::string content = buffer.str();
    std::cout << content << '\n';

    Json::CharReaderBuilder builder;
    JSONCPP_STRING err;
    Json::Value root;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(content.c_str(),
                       content.c_str() + static_cast<int>(content.length()),
                       &root, &err)) {
        throw std::runtime_error("Failed to parse config file.");
    }

    ip = root["ip"].asString();
    port = root["port"].asString();
    for (const Json::Value& mountValue : root["mounts"]) {
        Mount mount = Mount();
        mount.mountPoint = mountValue["mountPoint"].asString();
        mount.rtspSrc = mountValue["rtspSrc"].asString();
        mount.shared = mountValue["shared"].asBool();
        mount.bindMcastAddress = mountValue["bind_mcast_address"].asBool();
        mount.mcastIface = mountValue["mcast_iface"].asString();
        mount.maxMcastTtl = mountValue["max_mcast_ttl"].asUInt();
        mount.setProtocolsUDPMcast = mountValue["force_UDP_mcast"].asBool();
        mount.enableRtcp = mountValue["enableRtcp"].asBool();
        mount.latency = mountValue["latency"].asUInt();
        mount.hasAddressPool = mountValue["hasAddressPool"].asBool();
        if (mount.hasAddressPool) {
            mount.addressPool.ipMin =
                mountValue["addressPool"]["ipMin"].asString();
            mount.addressPool.ipMax =
                mountValue["addressPool"]["ipMax"].asString();
            mount.addressPool.portMin =
                mountValue["addressPool"]["portMin"].asUInt();
            mount.addressPool.portMax =
                mountValue["addressPool"]["portMax"].asUInt();
            mount.addressPool.ttl = mountValue["addressPool"]["ttl"].asUInt();
        }
        mounts.push_back(mount);
    }
}