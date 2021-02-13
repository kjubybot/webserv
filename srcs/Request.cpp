#include "Request.hpp"

// Public methods

Request::Request(struct sockaddr_in sockAddr)
        : firstPart(false), secondPart(false), flagError(false), contentLen(0), sockAddr(sockAddr), toRead(0) {}

Request::~Request() {
    headers.clear();
}

void Request::setPath(const std::string &path) {
	Request::path = path;
}

std::pair<std::string, std::string> Request::getError() const {
    return error;
}

void Request::setMethod(const std::string &method) {
	Request::method = method;
}

const std::string &Request::getContent() const {
	return content;
}

const std::string &Request::getPath() const {
	return path;
}

const std::string &Request::getMethod() const {
	return method;
}

const std::map<std::string, std::string> &Request::getHeaders() const {
	return headers;
}

uint64_t Request::getContentLen() const {
    return content.length();
}

const sockaddr_in& Request::getSockAddr() const {
    return sockAddr;
}

void Request::parse(std::string& line) {
    if (!firstPart) {
        if (line.find("\r\n\r\n") == std::string::npos) {
            addError("400", "Bad Request");
            return;
        }
        parseFirst(line);
    }
    if (!secondPart)
        parseSecond(line);
}

void Request::parseFirst(std::string &line) {
    size_t crlf = line.find("\r\n");
    std::string l = line.substr(0, crlf);
    std::vector<std::string> arr = split(l, " ");

    if (arr.size() == 3)
    {
        if ((arr[0] != "GET") and (arr[0] != "POST") and (arr[0] != "PUT") and (arr[0] != "HEAD") and arr[2] != "HTTP/1.1") {
            addError("400", "Bad Request");
            return;
        }
        Request::setMethod(arr[0]);
        Request::setPath(arr[1]);
    } else {
        addError("400", "Bad Request");
        return;
    }
    line.erase(0, crlf + 2);
    while (!line.empty()) {
        crlf = line.find("\r\n");
        l = line.substr(0, crlf);
        if (l.empty()) {
            line.erase(0, crlf + 2);
            firstPart = true;
            return;
        }
        size_t colon = l.find(":");
        if (colon != std::string::npos) {
            std::string headerName = l.substr(0, colon);
            // check symbols >= 9 || <= 13
            if (checkSymbols(headerName[0]) || checkSymbols(headerName[colon - 1])) {
            //headerName[0] == ' ' || headerName[0] == '\t' || headerName[colon - 1] == ' ' || headerName[colon - 1] == '\t') {
                addError("400", "Bad Request");
                return;
            }
            std::transform(headerName.begin(), headerName.end(), headerName.begin(), ::tolower);
            std::string headerValue = trim(l.substr(colon + 2));
            // check transfer-encoding and content-length
            if (headerName == "content-length") {
                if (method == "GET" || headers.find("content-length") != headers.end()) {
                    addError("400", "Bad Request");
                    return;
                }
                contentLen = std::stoi(headerValue);
            }
            headers[headerName] = headerValue;
        }
        line.erase(0, crlf + 2);
    }
    if (headers.count("content-length") == 1 && headers.count("transfer-encoding") == 1) {
        headers.erase("content-length");
        contentLen = 0;
    }
    firstPart = true;
}

int Request::checkSymbols(char sym) {
	return ((9 <= sym && sym <= 13) || sym == ' ');
}

bool Request::isFlagError() const {
	return flagError;
}

// Private methods

void Request::parseSecond(std::string &line) {
	it_te = headers.find("transfer-encoding");
    if (it_te == headers.end() && contentLen == 0) {
        secondPart = true;
        return;
    }
    while (!line.empty()) {
        if (toRead == 0) {
            if (it_te == headers.end())
                toRead = contentLen;
            else {
                size_t crlf = line.find("\r\n");
                if (crlf != std::string::npos) {
                    toRead = ::stoul(line, 16);
                    if (toRead == 0 && line.find("\r\n\r\n") == std::string::npos)
                        return;
                    line.erase(0, crlf + 2);
//                    if (line.find("\r\n") == std::string::npos)
//                        return;
                } else
                    return;
            }
        }
        if (toRead == 0) {
            if (it_te != headers.end())
                line.erase(0, 2);
            secondPart = true;
            return;
        }
        if (toRead > line.length()) {
            content += line;
            toRead -= line.length();
            line.clear();
        } else {
            if (it_te != headers.end() && line.find("\r\n") == std::string::npos)
                return;
            content += line.substr(0, toRead);
            line.erase(0, toRead);
            toRead = 0;
            if (it_te == headers.end()) {
                secondPart = true;
                return;
            } else
                line.erase(0, 2); // erasing \r\n after chunk
        }
    }
}

void Request::addError(std::string errorKey, std::string errorValue) {
	firstPart = secondPart = flagError = true;
	error.first = errorKey;
	error.second = errorValue;
}

bool Request::isFirstPart() const {
    return firstPart;
}

bool Request::isSecondPart() const {
    return secondPart;
}
