#include <chrono>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "proxy/httplib.h"
#include "proxy/helper.hpp"

class Proxy final
{
public:
    Proxy(const char* const host, const int port, const std::string& uri)
        : m_port(port)
        , m_host(host)
        , m_addr(helper::cut(uri, '/', 8))
        , m_client(m_addr.first.c_str())
    {
        registerRoutes();
    }

    bool run()
    {
        std::cout << "Server up and running at: " << *this << std::endl
                  << "Press CTRL+C to stop the server." << std::endl;
        return m_server.listen(m_host, m_port);
    }

    friend std::ostream& operator<<(std::ostream& os, const Proxy& proxy);

private:
    struct ForwardRequest final
    {
        using Clock = std::chrono::high_resolution_clock;

        httplib::Client& m_client;
        std::string m_base;
        std::string curr_manifest;
        // static const std::unordered_map<std::string, std::string> ext_map = {
        //     {".ts", "SEGMENT"},
        //     {".m3u", "MANIFEST"},
        //     {".m3u8", "MANIFEST"},
        //     {".vtt", "SUBTITLE"}
        // };
        // std::vector<std::string> m_cache_uris;

        ForwardRequest(httplib::Client& client, const std::string& base)
            : m_client(client)
            , m_base(base)
            // , m_cache_uris({base})
        {}

        std::string make_uri(const std::string& path)
        {
            if (path == "/")
                return m_base;
            
            return "";
            // for (const auto& uri : )
        }

        void processRequest(const httplib::Request& req, httplib::Response& res, const std::string& uri)
        {
            if (uri.empty())
            {
                res.status = 404;
                res.set_content("Not Found", "text/plain");
                return;
            }
                      
            if (auto ret = m_client.Get(uri.c_str())) {
                // TODO: parse body and retrieve links
                // if (ret->status == 200)
                const auto& content_type = ret->get_header_value("Content-Type");
                res.status = ret->status;
                res.set_content(ret->body, content_type.c_str());
            }
        }

        void operator()(const httplib::Request& req, httplib::Response& res) {
            std::string uri = make_uri(req.path);
            //TODO: Make get extension resilient
            const auto& ext = helper::get_extension(uri);

            std::string kind;
            if (ext == ".m3u" || ext == ".m3u8")
                kind = "MANIFEST";
            else if (ext == ".vtt")
                kind = "MANIFEST";
            else if (ext == ".ts")
                kind = "SEGMENT";
            else
                kind = "UNKNOWN";

            // TODO: Check track switch is between in-out or before in
            // if (kind == "MANIFEST" and curr_manifest != uri) {
            //     std::cout << "[TRACK SWITCH]" << std::endl;
            //     curr_manifest = uri;
            // }

            std::cout << "[IN][" << kind << "] " << req.remote_addr << ':' << req.remote_port << req.path << std::endl;
            const auto tick = Clock::now();
            
            processRequest(req, res, uri);

            const auto tock = Clock::now();
            const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(tock-tick);
            std::cout << "[OUT][" << kind << "] " << req.remote_addr << ':' << req.remote_port << req.path << " (" << elapsed.count() << "ms)" << std::endl;
        }
    };

    void registerRoutes()
    {
        m_server.Get(".*", ForwardRequest(m_client, m_addr.second));
    }

private:
    const int           m_port;
    const char* const   m_host;
    const std::pair<std::string, std::string> m_addr;
    httplib::Server     m_server;
    httplib::Client     m_client;
};

std::ostream& operator<<(std::ostream& os, const Proxy& proxy)
{
    os << proxy.m_host << ':' << proxy.m_port;
    return os;
}

void print_usage(const char* const bin)
{
    std::cout << "Usage: " << bin << " <uri>" << std::endl << std::endl
              << "Reverse proxy server." << std::endl << std::endl;
}

// ./build/proxy-sr https://bitdash-a.akamaihd.net/content/MI201109210084_1/m3u8s/f08e80da-bf1d-4e3d-8899-f0f6155f6efa.m3u8
int main(int argc, char* argv[])
{
    const char* const bin = argv[0];

    if (argc < 2) {
        print_usage(bin);
        std::cout << "Error: missing input." << std::endl;
        return 0;
    }

    // https://bitdash-a.akamaihd.net/
    // https://bitdash-a.akamaihd.net/content/MI201109210084_1/video/180_250000/hls/segment_47.ts
    const char* const uri = argv[1];
    // httplib::Client cli(uri);
    // httplib::Client cli("https://bitdash-a.akamaihd.net");
    // auto res = cli.Get("/content/MI201109210084_1/m3u8s/f08e80da-bf1d-4e3d-8899-f0f6155f6efa.m3u8");
    // auto res = cli.Get("/content/MI201109210084_1/video/180_250000/hls/segment_47.ts");

    // if (res)
    // {
    //     auto headers = res->headers;
    //     for (const auto& h : headers) {
    //         std::cout << h.first << ": " << h.second << std::endl;
    //     }
    //     // std::cout << "Headers: " << headers. << std::endl;
    //     std::cout << "Status: " << res->status << std::endl;
    //     // std::cout << "Body: " << res->body << std::endl;
    // }

    // httplib::Server server;
    // server.Get(".*", [&cli](const httplib::Request &, httplib::Response &res) {
        // if (auto ret = cli.Get("/content/MI201109210084_1/video/180_250000/hls/segment_47.ts")) {
        //     res.set_content(ret->body, "video/mp2t");
        // }
    //     // res.set_content("Hello World!", "text/plain");
    //     // res.set_content_provider()
    // });

    // server.listen("127.0.0.1", 8081);

    Proxy proxy("127.0.0.1", 8081, uri);
    if (!proxy.run())
    {
        std::cerr << "Server failed to start" << std::endl;
        return 1;
    }

    return 0;
}