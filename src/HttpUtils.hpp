#ifndef TINA_HTTP_UTILS_HPP
#define TINA_HTTP_UTILS_HPP

#include <string>

namespace Tina {
    class HttpUtils {
    public:
        bool equals(const std::string &a, const std::string &b) {
            return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin(), [](char ca, char cb) {
                return std::tolower(ca) == std::tolower(cb);
            });
        }

        bool fromHexToInt(const std::string &s, size_t i, size_t cnt, int &value) {
            if (i >= s.size()) return false;
            value = 0;
            for (; cnt; i++, cnt--) {
                if (!s[i]) return false;
                if (auto v = 0; isHex(s[i], v)) {
                    value = value * 16 + v;
                } else {
                    return false;
                }
            }
            return true;
        };

        std::string fromIntToHex(size_t n) {
            static const auto charsets = "0123456789abcdef";
            std::string result;
            do {
                result = charsets[n & 15] + result;
                n >>= 4;
            } while (n > 0);
            return result;
        }

        size_t toUtf8(int code, char *buff) {
            if (code < 0x0080) {
                buff[0] = static_cast<char>(code & 0x7F);
                return 1;
            }
            if (code < 0x0800) {
                buff[0] = static_cast<char>(0xC0 | ((code >> 6) & 0x1F));
                buff[1] = static_cast<char>(0x80 | (code & 0x3F));
                return 2;
            }
            if (code < 0xD800) {
                buff[0] = static_cast<char>(0xE0 | ((code >> 12) & 0xF));
                buff[1] = static_cast<char>(0x80 | ((code >> 6) & 0x3F));
                buff[2] = static_cast<char>(0x80 | (code & 0x3F));
                return 3;
            }
            if (code < 0xE000) {
                // D800 - DFFF is invalid...
                return 0;
            }
            if (code < 0x10000) {
                buff[0] = static_cast<char>(0xE0 | ((code >> 12) & 0xF));
                buff[1] = static_cast<char>(0x80 | ((code >> 6) & 0x3F));
                buff[2] = static_cast<char>(0x80 | (code & 0x3F));
                return 3;
            }
            if (code < 0x110000) {
                buff[0] = static_cast<char>(0xF0 | ((code >> 18) & 0x7));
                buff[1] = static_cast<char>(0x80 | ((code >> 12) & 0x3F));
                buff[2] = static_cast<char>(0x80 | ((code >> 6) & 0x3F));
                buff[3] = static_cast<char>(0x80 | (code & 0x3F));
                return 4;
            }
            return 0;
        }

    private:
        bool isHex(char c, int &value) {
            // Check if the character is a valid hex digit
            if (0x20 <= c && isdigit(c)) {
                value = c - '0';
                return true;
            }
            if ('A' <= c && c <= 'F') {
                value = c - 'A' + 10;
                return true;
            }
            if ('a' <= c && c <= 'f') {
                value = c - 'a' + 10;
                return true;
            }
            return false;
        }
    };
}


#endif //TINA_HTTP_UTILS_HPP
