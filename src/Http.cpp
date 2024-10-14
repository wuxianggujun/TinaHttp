#include "Http.hpp"

namespace Tina {
    Http::Http(int port): port(port), sockfd(-1) {
        sockfd = socket(AF_INET,SOCK_STREAM, 0);
        if (sockfd == -1) {
            perror("socket");
            exit(1);
        }

        setNonBlocking(sockfd); // 设置非阻塞模式


        epfd = epoll_create(1);
        if (epfd == -1) {
            perror("epoll_create");
            exit(1);
        }

        epoll_event event{};
        event.events = EPOLLIN | EPOLLET;
        event.data.fd = sockfd;
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event) == -1) {
            perror("epoll_ctl");
            exit(1);
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if ((setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR, &code, sizeof(code))) < 0) {
            perror("setsockopt");
            exit(1);
        }
    }

    Http::~Http() {
        if (sockfd != -1) {
            close(sockfd);
        }
        if (epfd != -1) {
            close(epfd);
        }
    }
    

    void Http::sendReuqest(const std::string &host, int port, const std::string &request) {
        int client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (client_sockfd == -1) {
            perror("socket");
            return;
        }

        sockaddr_in client_addr{};
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(port);
        if (inet_pton(AF_INET, host.c_str(), &client_addr.sin_addr) != 1) {
            perror("inet_pton");
            close(client_sockfd);
        }
        if (connect(client_sockfd, reinterpret_cast<sockaddr *>(&client_addr), sizeof(client_addr)) == -1) {
            perror("connect");
            close(client_sockfd);
            return;
        }

        // 发送请求
        ssize_t bytes_sent = send(client_sockfd, request.c_str(), request.size(), 0);
        if (bytes_sent == -1) {
            perror("send");
            close(client_sockfd);
            return;
        }

        // 接收响应
        char buffer[1024];
        ssize_t bytes_received = recv(client_sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received == -1) {
            perror("recv");
        } else {
            buffer[bytes_received] = '\0'; // 确保字符串终止
            printf("Received response:\n%s\n", buffer);
        }
        // 关闭客户端socket
        close(client_sockfd);
    }


    void Http::start() {
        if (bind(sockfd, reinterpret_cast<sockaddr *>(&serv_addr), sizeof(serv_addr)) == -1) {
            perror("bind");
            exit(1);
        }

        if (port == 0) {
            socklen_t len = sizeof(serv_addr);
            if (getsockname(sockfd, reinterpret_cast<sockaddr *>(&serv_addr), &len) == -1) {
                perror("getsockname");
                exit(1);
            }
            port = ntohs(serv_addr.sin_port);
        }

        if (listen(sockfd, 20) == -1) {
            perror("listen");
            exit(1);
        }


        while (true) {
            int num_event_fds = epoll_wait(epfd, events, MAX_EPOLL_EVENTS, -1);
            if (num_event_fds == -1) {
                perror("epoll_wait");
                exit(1);
            }

            for (int i = 0; i < num_event_fds; ++i) {
                if (events[i].data.fd == sockfd) {
                    while (true) {
                        sockaddr_in client_addr{};
                        socklen_t client_addr_len = sizeof(client_addr);
                        int client_sockfd = accept(sockfd, reinterpret_cast<sockaddr *>(&client_addr),
                                                   &client_addr_len);
                        if (client_sockfd == -1) {
                            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                                break;
                            }
                            perror("accept");
                            continue;
                        }

                        setNonBlocking(client_sockfd);

                        epoll_event client_event{};
                        client_event.events = EPOLLIN | EPOLLET; // 边沿触发
                        client_event.data.fd = client_sockfd;

                        if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_sockfd, &client_event) == -1) {
                            perror("epoll_ctl");
                            close(client_sockfd);
                        }
                    }
                } else if (events[i].events & EPOLLIN) {
                    handleRequest(events[i].data.fd);
                }
            }
        }
    }

    int Http::setNonBlocking(int fd) {
        const int flags = fcntl(fd, F_GETFL, 0);
        return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }

    /*void Http::handleRequest(intptr_t client_sockfd) {
        printf("handleRequest %ld\n", client_sockfd);
        // 构建HTTP响应
        const std::string response =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 2\r\n"
                "\r\n"
                "OK";

        // 发送响应给客户端
        ssize_t bytes_sent = send(client_sockfd, response.c_str(), response.size(), 0);
        if (bytes_sent == -1) {
            perror("send");
        }
        close(client_sockfd);
    }*/

    void Http::handleRequest(intptr_t client_sockfd) {
        char buffer[1024];
        ssize_t bytes_received = recv(client_sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0'; // 确保字符串终止
            printf("Received request: %s\n", buffer);

            // 构建HTTP响应
            const std::string response =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 2\r\n"
                "\r\n"
                "OK";

            send(client_sockfd, response.c_str(), response.size(), 0);
        } else {
            if (bytes_received == 0 || (errno != EAGAIN && errno != EWOULDBLOCK)) {
                printf("Closing connection to client: %d\n", client_sockfd);
                close(client_sockfd);
            }
        }
    }

    /*void *Http::threadRoutine(void *arg) {
        auto client_sockfd = reinterpret_cast<intptr_t>(arg);
        handleRequest(client_sockfd);
        return nullptr;
    }*/
}
