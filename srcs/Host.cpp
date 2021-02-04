#include "Host.hpp"

Host::Host() {}

Host::Host(struct sockaddr_in sockAddr, const std::string& name) : sockAddr(sockAddr), name(name) {}

Host::~Host() {}

Host::Host(const Host& h) : sockAddr(h.sockAddr), name(h.name) {}

Host& Host::operator=(const Host& h) {
    sockAddr = h.sockAddr;
    name = h.name;
    return *this;
}

std::string Host::makeAutoindex(const std::string& path) const {
    std::string ret = "<!DOCTYPE HTML>"
                      "<html><head><title>Index of " + path
                      + "</title></head>"
                      "<body><h1>Index of " + path
                      + "</h1><hr><pre>";
    std::string fullPath = root + path;
    std::string fName;
    DIR* dir = opendir(root.c_str());
    struct dirent* dp;

    while ((dp = readdir(dir))) {
        fName = std::string(dp->d_name);
        if (dp->d_type & DT_DIR)
            fName += "/";
        ret +="<a href=\"" + fName;
        ret += "\">" + fName + "</a>";
    }
    ret += "</pre></body></html>";
    closedir(dir);
    return ret;
}

Response Host::makeError(const std::string& code, const std::string& message) {
    Response ret;

    if (errorPages.find(code) != errorPages.end())
        ret = Response::fromFile(code, message, errorPages[code]);
    else
        ret = Response::fromString(code, message, HttpErrorPage(code, message).createPage());
    return ret;
}

struct sockaddr_in Host::getSockAddr() const {
    return sockAddr;
}

const std::string& Host::getName() const {
    return name;
}

const std::string& Host::getRoot() const {
    return root;
}

void Host::setRoot(const std::string& root) {
    this->root = root;
}

void Host::setErrorPages(const std::map<std::string, std::string> errorPages) {
    this->errorPages = errorPages;
}

Response Host::processRequest(const Request&) const {
    Response ret;
//    std::string fullPath;
//    struct stat fStat;

    /* if (r.hasError())
     *     return makeError(r.getErrorCode(), r.getErrorMessage());
     * fullPath = root + r.getUri();
     * if (stat(fullPath.c_str(), &fStat))
     *     return makeError("404", "Not found");
     * if (fStat.st_mode & S_IFDIR) {
     *     if (autoindex)
     *         ret = Response::fromString("200", "OK", makeAutoindex(r.getUri()));
     *     else
     *         ret = makeError("403", "Forbidden");
     * } else
     *     ret = Response::fromFile("200", "OK", fullPath);*/
    return ret;
}
