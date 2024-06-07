#include <iostream>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <memory>
#include <getopt.h>

#include "config.hh"

int main(int argc, char** argv) {
    gst_init(&argc, &argv);

    const ServerConfig config =
        ServerConfig(std::string(CONFIG_PATH) + "rtsp_relay.json");

    GMainContext* runContext = g_main_context_new();
    GMainLoop* runLoop = g_main_loop_new(runContext, false);
    GstRTSPServer* server;
    GstRTSPMountPoints* mountPoints;
    server = gst_rtsp_server_new();
    gst_rtsp_server_set_address(server, config.ip.c_str());
    gst_rtsp_server_set_service(server, config.port.c_str());
    mountPoints = gst_rtsp_server_get_mount_points(server);
    for (const Mount& mount : config.mounts) {
        GstRTSPMediaFactory* factory;
        factory = gst_rtsp_media_factory_new();
        const std::string launch =
            "( rtspsrc location=rtsp://" + mount.rtspSrc +
            " latency=0 ! "
            "application/x-rtp,media=video,clock-rate=90000,encoding-name=H264 "
            "! rtph264depay ! rtph264pay name=pay0 pt=96 )";
        std::cout << "Pipeline :\n" << launch << "\n";
        gst_rtsp_media_factory_set_launch(factory, launch.c_str());
        gst_rtsp_media_factory_set_shared(factory, mount.shared);
        if (mount.bindMcastAddress) {
            gst_rtsp_media_factory_set_bind_mcast_address(
                factory, mount.bindMcastAddress);
            gst_rtsp_media_factory_set_multicast_iface(
                factory, mount.mcastIface.c_str());
            gst_rtsp_media_factory_set_max_mcast_ttl(factory,
                                                     mount.maxMcastTtl);
            if (mount.setProtocolsUDPMcast) {
                gst_rtsp_media_factory_set_protocols(
                    factory, GST_RTSP_LOWER_TRANS_UDP_MCAST);
            } else {
                gst_rtsp_media_factory_set_protocols(factory,
                                                     GST_RTSP_LOWER_TRANS_UDP);
            }
        }
        gst_rtsp_media_factory_set_enable_rtcp(factory, mount.enableRtcp);
        gst_rtsp_media_factory_set_latency(factory, mount.latency);

        if (mount.hasAddressPool) {
            GstRTSPAddressPool* pool = gst_rtsp_address_pool_new();
            gst_rtsp_address_pool_add_range(
                pool, mount.addressPool.ipMin.c_str(),
                mount.addressPool.ipMax.c_str(), mount.addressPool.portMin,
                mount.addressPool.portMax, mount.addressPool.ttl);
            gst_rtsp_media_factory_set_address_pool(factory, pool);
            g_object_unref(pool);
        }

        gst_rtsp_mount_points_add_factory(
            mountPoints, ("/" + mount.mountPoint).c_str(), factory);
        std::cout << "stream available at rtsp://" << config.ip << ":"
                  << config.port << "/" + mount.mountPoint << std::endl;
    }

    gst_rtsp_server_attach(server, runContext);
    g_main_loop_run(runLoop);
}