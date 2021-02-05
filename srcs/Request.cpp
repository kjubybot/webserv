#include "Request.hpp"

// Public methods

Request::Request() : firstPart(false), secondPart(false), flagError(false), contentLen(0) {}

Request::~Request() {
    headers.clear();
}

void Request::setPath(const std::string &path) {
	Request::path = path;
}

void Request::setHtmlPage(const std::string &htmlPage) {
	Request::htmlPage = htmlPage;
}

const std::pair<std::string, std::string> Request::getError() const {
    return error;
}

void Request::setMethod(const std::string &method) {
	Request::method = method;
}

const std::string &Request::getHtmlPage() const {
	return htmlPage;
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

void Request::parse(std::string& line) {
    if (!firstPart) {
        if (line.find("\r\n\r\n") == std::string::npos) {
            addError("400", "Bad request");
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
            addError("400", "Bad request");
            return;
        }
        Request::setMethod(arr[0]);
        Request::setPath(arr[1]);
    } else {
        addError("400", "Bad request");
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
            if (headerName[0] == ' ' || headerName[colon - 1] == ' ') {
                addError("400", "Bad request");
                return;
            }
            std::transform(headerName.begin(), headerName.end(), headerName.begin(), ::tolower);
            std::string headerValue = trim(l.substr(colon + 2));
            headers[headerName] = headerValue;
            if (headerName == "content-length") {
                if (headers.count("content-length") > 1 || method == "GET") {
                    addError("400", "Bad request");
                    return;
                }
                contentLen = std::stoi(headerValue);
            }
        }
        line.erase(0, crlf + 2);
    }
    firstPart = true;
}


bool Request::isFlagError() const {
	return flagError;
}

// Private methods

void Request::parseSecond(std::string &line) {
    static size_t toRead;

    if (toRead == 0) {
        if (headers.find("transfer-encoding") == headers.end())
            toRead = contentLen;
        else {
            size_t crlf = line.find("\r\n");
            if (crlf != std::string::npos) {
                toRead = std::stoi(line, 0, 16);
                line.erase(0, crlf + 2);
            } else
                return;
        }
    }
    if (toRead == 0) {
        secondPart = true;
        return;
    }
    if (toRead >= line.length()) {
        content += line;
        toRead -= line.length();
        line.clear();
    } else {
        content += line.substr(0, toRead);
        line.erase(0, toRead);
        toRead = 0;
        secondPart = true;
    }
/*    std::cout << "FINISHED PARSING REQUEST" << std::endl;
    std::cout << "Method: " << method << "; path: " << path << std::endl;
    std::cout << "Headers: " << std::endl;
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
        std::cout << it->first << ": " << it->second << std::endl << std::endl;
    std::cout << content << std::endl;*/
}
//	// if (GET)
//		// return;
//
//	// if : Content-Len && POST - need limit size read
//	if (contentLen > 0 && headers.count("transfer-encoding") == 0) {
//		content += line;
//		content.erase(contentLen + 1);
//		line.erase(0, contentLen);
//		finishBody = true;
//		return;
//	}
//
//
//	// if : Transfer-Encoding and == chunked, read len symbols
//	if (headers.count("transfer-encoding") == 1) {
//		if (headers["transfer-encoding"] == "chunked") {
//			if (transferLen == 0 && remainder == 0) {
//				transferLen = std::stoi(line, 0, 16);
//				if (transferLen == 0) {
//					finishBody = true;
//					return ;
//				}
//			}
//			else {
//				if (remainder > 0) {
//					if (line.length() > remainder) {
//						line.erase(remainder + 1);
////						content += line.substr(0, remainder);
//						remainder = 0;
//					}
//					else {
//						remainder -= line.length();
//					}
//					content += line;
//				}
//				else {
//					if (line.length() < transferLen) {
//						remainder = transferLen - line.length();
//					}
//					else { // line.length() > transferLen
//						line.erase(transferLen);
//					}
//					content += line;
//					transferLen = 0;
//				}
//			}
//		}
//		else {
//			addError("501", "Not Implemented");
//			throw HttpErrorException("501", "Not Implemented", *this);
//		}
//	} else
//	    finishBody = true;
//}

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
