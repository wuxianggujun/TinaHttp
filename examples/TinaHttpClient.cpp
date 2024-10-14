#include "TinaHttp.hpp"

using namespace Tina;

int main() {
    
    for (int i = 0; i < 100000; i++) {
        printf("%d\n", i);
        // 构造简单的 HTTP GET 请求
        std::string request =
                "GET / HTTP/1.1\r\n"
                "Host: 127.0.0.1\r\n"
                "Connection: close\r\n\r\n";
        Http::sendReuqest("127.0.0.1", 8080, request);
    }
    
    return 0;
}
