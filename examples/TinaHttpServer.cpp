#include "TinaHttp.hpp"

using namespace Tina;
#include <iostream>

int main() {

   auto code =  make_error_code(Status::OK);

    std::cout << std::error_code(code).message() << std::endl;
    
    Http server(8080);
    server.start();
    
    return 0;
}
