#include "Response.hpp"

Response::Response() : complete(false) {}

#include <iostream>
Response::Response(const std::string& code) : code(code), complete(false) {
//    std::cout << "Creating response with code " << code << std::endl;
    if (code[0] == '4' || code[0] == '5')
        headers["Connection"] = "close";
}

Response::Response(const Response& r) : code(r.code), message(r.message), body(r.body), headers(r.headers), complete(true) {}

Response& Response::operator=(const Response& r) {
    code = r.code;
    message = r.message;
    body = r.body;
    headers = r.headers;
    complete = r.complete;
    return *this;
}

Response::~Response() {}

std::string Response::getDate() {
    char buf[100];
    struct tm *dateTm;
    struct timeval tv;

    gettimeofday(&tv, 0);
    dateTm = gmtime(&tv.tv_sec);
    strftime(buf, 100, "%a, %d %b %G %T GMT", dateTm);
    return std::string(buf);
}

std::string Response::getLastModified(const std::string& filename) {
    char buf[100];
    struct tm *dateTm;
    struct timeval tv;
    struct stat fStat;

    if (filename.empty()) {
        gettimeofday(&tv, 0);
        dateTm = gmtime(&tv.tv_sec);
    } else {
        stat(filename.c_str(), &fStat);
        dateTm = gmtime(&fStat.st_mtimespec.tv_sec);
    }
    strftime(buf, 100, "%a, %d %b %G %T GMT", dateTm);
    return std::string(buf);
}

std::string Response::getContentType(const std::string& filename) {
    std::string ext = filename.substr(filename.rfind('.'));
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    std::pair<std::string, std::string> types[7] = {
            std::pair<std::string, std::string>(".html", "text/html"),
            std::pair<std::string, std::string>(".css", "text/css"),
            std::pair<std::string, std::string>(".js", "text/javascript"),
            std::pair<std::string, std::string>(".gif", "image/gif"),
            std::pair<std::string, std::string>(".jpg", "image/jpeg"),
            std::pair<std::string, std::string>(".jpeg", "image/jpeg"),
            std::pair<std::string, std::string>(".png", "image/png")
    };

    if (ext.empty())
        return "text/plain";
    for (size_t i = 0; i < 7; ++i)
        if (types[i].first == ext)
            return types[i].second;
    return "text/plain";
}

std::string Response::getData() {
    std::string data = "HTTP/1.1 " + code + " " + message + "\r\n";
    headers["Server"] = "webserv/0.0";
    headers["Date"] = getDate();
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
        data.append(it->first + ": " + it->second + "\r\n");
    data.append("\r\n");
    data.append(body);
    complete = true;
    return data;
}

bool Response::isComplete() const {
    return complete;
}

void Response::setCode(const std::string& code) {
    this->code = code;
}

void Response::setMessage(const std::string& message) {
    this->message = message;
}

void Response::setBody(const std::string& body) {
    this->body = body;
}

void Response::setHeader(const std::string& key, const std::string& value) {
    headers[key] = value;
}

Response Response::fromFile(const std::string& code, const std::string& message, const std::string& filename) {
    Response ret(code);

    ret.message = message;
    ret.body = getFileContent(filename);
    ret.headers["Content-Length"] = std::to_string(ret.body.length());
    ret.headers["Content-Type"] = getContentType(filename);
    ret.headers["Last-Modified"] = getLastModified(filename);
    return ret;
}

Response Response::fromFileNoBody(const std::string& code, const std::string& message, const std::string& filename) {
    Response ret(code);
    struct stat fStat;

    stat(filename.c_str(), &fStat);
    ret.message = message;
    ret.headers["Content-Length"] = std::to_string(fStat.st_size);
    ret.headers["Content-Type"] = getContentType(filename);
    ret.headers["Last-Modified"] = getLastModified(filename);
    return ret;
}

Response Response::fromString(const std::string& code, const std::string& message, const std::string& body) {
    Response ret(code);

    ret.message = message;
    ret.body = body;
    ret.headers["Content-Length"] = std::to_string(ret.body.length());
    ret.headers["Content-Type"] = "text/html";
    ret.headers["Last-Modified"] = getLastModified("");
    return ret;
}

Response Response::fromStringNoBody(const std::string& code, const std::string& message, const std::string& body) {
    Response ret(code);

    ret.message = message;
    if (code != "204") {
        ret.headers["Content-Length"] = std::to_string(body.length());
        ret.headers["Content-Type"] = "text/html";
        ret.headers["Last-Modified"] = getLastModified("");
    }
    return ret;
}

Response Response::fromCGI(const std::string& cgiResponse) {
    Response ret;
    std::string status;
    size_t crlf;

    crlf = cgiResponse.find("\r\n");
    status = cgiResponse.substr(0, crlf);
    status = trim(status.substr(status.find(':') + 1));
    ret.code = status.substr(0, status.find(' '));
    ret.message = status.substr(status.find(' ') + 1);
    status = cgiResponse.substr(crlf + 2);
    while (!status.empty()) {
        std::string line, headerName, headerValue;
        crlf = status.find("\r\n");
        line = status.substr(0, crlf);
        if (line.empty()) {
            status.erase(0, crlf + 2);
            break;
        }
        headerName = line.substr(0, line.find(':'));
        headerValue = trim(line.substr(line.find(':') + 1));
        ret.headers[headerName] = headerValue;
        status.erase(0, crlf + 2);
    }
    ret.body = status;
    ret.headers["content-length"] = std::to_string(ret.body.length());
    return ret;
}

//Response Response::fromCGIFD(int fd) {
//    Response ret;
//    std::string status, cgiResponse;
//    size_t crlf;
//    int r;
//    char buf[IOSIZE];
//
//    while ((r = read(fd, buf, IOSIZE)))
//        cgiResponse.append(buf, r);
//    crlf = cgiResponse.find("\r\n");
//    status = cgiResponse.substr(0, crlf);
//    status = trim(status.substr(status.find(':') + 1));
//    ret.code = status.substr(0, status.find(' '));
//    ret.message = status.substr(status.find(' ') + 1);
//    status = cgiResponse.substr(crlf + 2);
//    while (!status.empty()) {
//        std::string line, headerName, headerValue;
//        crlf = status.find("\r\n");
//        line = status.substr(0, crlf);
//        if (line.empty()) {
//            status.erase(0, crlf + 2);
//            break;
//        }
//        headerName = line.substr(0, line.find(':'));
//        headerValue = trim(line.substr(line.find(':') + 1));
//        ret.headers[headerName] = headerValue;
//        status.erase(0, crlf + 2);
//    }
//    ret.body = status;
//    ret.headers["content-length"] = std::to_string(ret.body.length());
//    return ret;
//}