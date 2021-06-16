# proxy-sr
## Problem statement
For  this  first  exercise,  you  are  going  to  simply  print  all  the  requests  that  will  be  passing between  VLC  and  the  media  server.  To  do  so,  you  are  going  to  create  a  local  server.  This  local server will serve as a relay between VLC and the CDN. In order to make it work, you will have to consider the following points:

- You will give your local server URL to VLC so it hit your local server instead of the CDN
- You  will  request  the  CDN  yourself  and  send  back  the  result  of  each  request  to  VLC  through your server
- You  will  also  need  to  give  the  original  stream  URL  to  your  server  when  starting  it,  so  it  will know how to request content from the CDN
- Your server might need to be re-launched for each new stream to feed it with the right URL

The launch procedure will look like this:

1. Launch your local server with the stream URL
2. Then start a network video from VLC pointing to your local server

Each request is going to be printed two times, when it arrives from the player and when the CDN is going to answer to it.

The expected output will contain the following:
- [IN] <URL>When a request is intercepted from VLC
- [OUT] <URL> (<time>ms) When the CDN answers and we send back the answer to the player

![](asset/mockup.png)

## Prerequisites

- C++17 compiler
- CMake (version compatible with C++17 needed)
- libssl
- pthread

## How to build
````shell
$ cmake -S . -B build
$ cmake --build build
````

````shell
$ cd build && ctest && cd -
$ ./build/proxy-sr
Usage: ./build/proxy-sr <uri>

Proxy server.
Try: ./build/proxy-sr https://bitdash-a.akamaihd.net/content/MI201109210084_1/m3u8s/f08e80da-bf1d-4e3d-8899-f0f6155f6efa.m3u8

Error: missing input.
````

![](asset/demo.gif)


## Decision making
There are many reasons why I choose Python for the first implementation, here are some:
   - Crossplatform, as long as you have Python installed, the proxy will work
   - The standard library is HUGE and will help me to iterate faster on the problem
   - You don't need root access to the server

Once I'll have a good enough understanding of the problem and how to solve it I will start the C++ implementation.

### HTTP library
I use the [cpp-httplib](https://github.com/yhirose/cpp-httplib) library mainly because it is a single-file header-only, thus easy to setup.
