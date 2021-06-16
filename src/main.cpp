#include <iostream>
#include "proxy/proxy.hpp"

void print_usage(const char* const bin)
{
    std::cout << "Usage: " << bin << " <uri>" << std::endl << std::endl
              << "Proxy server." << std::endl
              << "Try: " << bin << " https://bitdash-a.akamaihd.net/content/MI201109210084_1/m3u8s/f08e80da-bf1d-4e3d-8899-f0f6155f6efa.m3u8" << std::endl << std::endl;
}

int main(int argc, char* argv[])
{
    const char* const bin = argv[0];

    if (argc < 2) {
        print_usage(bin);
        std::cout << "Error: missing input." << std::endl;
        return 0;
    }

    const char* const default_uri = argv[1];

    Proxy proxy("127.0.0.1", 8081, default_uri);
    if (!proxy.run()) {
        std::cerr << "Server failed to start" << std::endl;
        return 1;
    }

    return 0;
}