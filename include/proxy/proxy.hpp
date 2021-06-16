#pragma once

#include <string>
#include <utility>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "proxy/httplib.h"

// TODO: Update readme.md. State what is a MANIFEST and SEGMENT wasn't clear

class Proxy final
{
public:
    Proxy(const char* const host, const int port, const std::string& uri);

    inline std::string get_uri() const
    {
        return m_uri;
    }

    inline bool run()
    {
        std::cout << "Server up and running at: " << get_uri() << std::endl
                  << "Press CTRL+C to stop the server." << std::endl;
        return m_client.is_valid() && m_server.listen(m_host, m_port);
    }

private:
    void registerRoutes();

private:
    using HostPort = std::pair<std::string, std::string>;

    const int           m_port;
    const char* const   m_host;
    const std::string   m_uri;
    const HostPort      m_forward_addr;
    httplib::Server     m_server;
    httplib::Client     m_client;
};
