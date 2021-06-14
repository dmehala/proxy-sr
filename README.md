# proxy-sr
## Exercise 1
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

![](mockup.png)

## Decision making
There are many reasons why I choose Python for the implementation, here ar some:
   - Crossplatform, as long as you have Python installed, the proxy will work
   - The standard library is HUGE and will help me to iterat fast on the problem
   - You don't need root access to the server
Python is a greate language for this kind of job.