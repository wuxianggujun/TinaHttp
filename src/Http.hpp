#ifndef TINA_HTTP_HPP
#define TINA_HTTP_HPP

#include <cstdio>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <cctype>
#include <strings.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include<fcntl.h>
#include <unordered_map>
#include <functional>


namespace Tina {
    enum HttpMethod {
        GET,
        POST,
        PUT,
        DELETE,
        CONNECT,
        OPTIONS,
        TRACE,
        PATCH,
        HEAD
    };

    enum class HttpProtocol {
        Ipv4,
        Ipv6
    };

    class Http {
    public:
        Http(int port);

        ~Http();
        
        static void sendReuqest(const std::string &host, int port, const std::string &request);

        void start();

    private:
        std::string url;
        int port;
        int sockfd;
        int code{};
        int epfd{};
        sockaddr_in serv_addr{};

        static constexpr int MAX_BUFFER_SIZE = 1024;
        static constexpr int MAX_EPOLL_EVENTS = 10000;

        epoll_event events[MAX_EPOLL_EVENTS];

        int setNonBlocking(int fd);
        static void handleRequest(intptr_t clientfd);
        
    };
}


#endif //TINA_HTTP_HPP
