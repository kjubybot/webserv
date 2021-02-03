#include "Response.hpp"

Response::Response() {}

Response::Response(const std::string& code) : code(code) {
    if (code[0] == '4' || code[0] == '5')
        headers["Connection"] = "close";
}

Response::Response(const std::string& code, const std::string& body) : code(code), body(body) {
    if (code[0] == '4' || code[0] == '5')
        headers["Connection"] = "close";
}

Response::Response(const Response& r) : code(r.code), body(r.body), headers(r.headers) {}

Response& Response::operator=(const Response& r) {
    code = r.code;
    body = r.body;
    headers = r.headers;
    return *this;
}

Response::~Response() {}

std::string Response::getData() const {
    std::string data = "HTTP/1.1 " + code + " MSG\n";
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
        data.append(it->first + ": " + it->second + "\n");
    data.append("\n");
    data.append(body);
    return data;
}

Response Response::fromFile(const std::string& code, const std::string& filename) {
    Response ret(code);

    char buf[4096];
    int r;
    int fd = open(filename.c_str(), O_RDONLY);

    while ((r = read(fd, buf, 4096)))
        ret.body.append(buf, r);
    close(fd);
    ret.headers["Content-Length"] = std::to_string(ret.body.length());
    return ret;
}

Response Response::fromString(const std::string& code, const std::string& body) {
    Response ret(code);

    ret.body = body;
    ret.headers["Content-Length"] = std::to_string(ret.body.length());
    return ret;
}