#include "proxy/proxy.hpp"
#include "proxy/helper.hpp"

#include <chrono>
#include <vector>
#include <unordered_map>

constexpr int HTTPS_SCHEMA_LENGTH = 8;   ///< std::string("https://").size() = 8

struct ForwardRequest final
{
    using Clock = std::chrono::high_resolution_clock;

    httplib::Client&            m_client;
    const std::string           m_default_uri;
    const std::string           m_server_uri;
    std::string                 m_curr_manifest;
    std::vector<std::string>    m_cache_uris;

    ForwardRequest(httplib::Client& client, const std::string& default_uri, const std::string& server_uri)
        : m_client(client)
        , m_default_uri(default_uri)
        , m_server_uri(server_uri)
        , m_curr_manifest(default_uri)
    {}

    std::string make_uri(const std::string& path)
    {
        if (path.empty() || path == "/")
            return m_default_uri;

        for (const auto& uri : m_cache_uris) {
            if (helper::contains(uri, path))
                return uri;
        }

        return "";
    }

    void processRequest(const httplib::Request& req, httplib::Response& res, const std::string& uri, const std::string& kind)
    {
        if (uri.empty())
        {
            res.status = 404;
            res.set_content("Not Found", "text/plain");
            return;
        }

        if (auto ret = m_client.Get(uri.c_str())) {
            if (ret->status == 200 && kind == "MANIFEST") {
                for (const auto& uri : helper::extract_uri_from_manifest(ret->body)) {
                    m_cache_uris.emplace_back(helper::resolve_uri(m_curr_manifest, uri));
                }
            }

            const auto& content_type = ret->get_header_value("Content-Type");
            res.status = ret->status;
            res.set_content(ret->body, content_type.c_str());
        }
    }

    std::string get_uri_kind(const std::string& uri) const
    {
        const auto& ext = helper::get_extension(uri);
        if (ext == ".m3u" || ext == ".m3u8")
            return "MANIFEST";
        else if (ext == ".vtt")
            return "MANIFEST";
        else if (ext == ".ts")
            return "SEGMENT";

        return "UNKNOWN";
    }

    void operator()(const httplib::Request& req, httplib::Response& res) {
        std::string uri = make_uri(req.path);
        std::string kind = get_uri_kind(uri);

        if (kind == "MANIFEST" && m_curr_manifest != uri) {
            std::cout << "[TRACK SWITCH]" << std::endl;
            m_curr_manifest = uri;
        }

        std::cout << "[IN][" << kind << "] " << m_server_uri << req.path << std::endl;
        const auto tick = Clock::now();

        processRequest(req, res, uri, kind);

        const auto tock = Clock::now();
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(tock-tick);
        std::cout << "[OUT][" << kind << "] " << m_server_uri << req.path << " (" << elapsed.count() << "ms)" << std::endl;
    }
};

Proxy::Proxy(const char* const host, const int port, const std::string& uri)
    : m_port(port)
    , m_host(host)
    , m_uri(std::string("http://") + m_host + ':' + std::to_string(m_port))
    , m_forward_addr(helper::cut(uri, '/', HTTPS_SCHEMA_LENGTH + 1))    ///< +1 because we want to start after the URI schema
    , m_client(m_forward_addr.first.c_str())                            ///< Future bug: m_addr MUST be initialized before
{
    registerRoutes();
}

void Proxy::registerRoutes()
{
    // Client from the httplib library is nor copyable or moveable. That's the main reason why the server own the client and pass
    // by reference to ForwardRequest functor. Ideally, ForwardRequest should own the http client... BUT if we wanted to use the
    // same client for different routes, passing by ref is a good solution.
    m_server.Get(".*", ForwardRequest(m_client, m_forward_addr.second, get_uri()));
}