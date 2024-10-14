#ifndef TINA_HTTP_ERROR_HPP
#define TINA_HTTP_ERROR_HPP

#include <string>
#include <stdexcept>
#include <system_error>

#include "Status.hpp"

namespace Tina {
    class RequestError final : public std::logic_error {
    public:
        using std::logic_error::logic_error;
        using std::logic_error::operator=;
    };

    class ResponseError final : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
        using std::runtime_error::operator=;
    };

    class ErrorCategory final : public std::error_category {
    public:
        static ErrorCategory &getInstance() {
            static ErrorCategory instance;
            return instance;
        }

        const char *name() const noexcept override {
            return "TinaHttpSocketError";
        }

        std::string message(const int condition) const override {
            switch (condition) {
                case Status::Code::OK:
                    return "Success";
                default:
                    return "Unknown error (" + std::to_string(condition) + ")";
            }
        }
    };

    inline std::error_code make_error_code(Status::Code value) {
        return {value, ErrorCategory::getInstance()};
    }
}


#endif //TINA_HTTP_ERROR_HPP
