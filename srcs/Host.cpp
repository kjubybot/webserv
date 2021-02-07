#include "Host.hpp"

Host::Host(struct sockaddr_in sockAddr, const Config& config) : sockAddr(sockAddr)
{
	names = config.getDefaultServerNames();
	errorPages = config.getDefaultServerErrorPages();
	maxBodySize = config.getDefaultServerMaxBodySize();
	root = config.getDefaultServerRoot();
	index = config.getDefaultServerIndexPages();
}

Host::~Host()
{ }

Host::Host(const Host& h)
    : sockAddr(h.sockAddr), names(h.names), errorPages(h.errorPages), maxBodySize(h.maxBodySize), root(h.root), index(h.index)
{ }

Host& Host::operator=(const Host& h) {
    sockAddr = h.sockAddr;
    names = h.names;
    errorPages = h.errorPages;
	maxBodySize = h.maxBodySize;
	root = h.root;
	index = h.index;
	return (*this);
}

//std::string Host::makeAutoindex(const std::string& path) const {
//    std::string ret = "<!DOCTYPE HTML>"
//                      "<html><head><title>Index of " + path
//                      + "</title></head>"
//                      "<body><h1>Index of " + path
//                      + "</h1><hr><pre>";
//    std::string fullPath = "." + path;
//    std::string fName;
//    DIR* dir = opendir(".");
//    struct dirent* dp;
//
//    while ((dp = readdir(dir))) {
//        fName = std::string(dp->d_name);
//        if (dp->d_type & DT_DIR)
//            fName += "/";
//        ret +="<a href=\"" + fName;
//        ret += "\">" + fName + "</a>";
//    }
//    ret += "</pre></body></html>";
//    closedir(dir);
//    return ret;
//}
//
Response Host::makeError(const std::string& code, const std::string& message) {
    Response ret;

    if (errorPages.count(code))
        ret = Response::fromFile(code, message, errorPages[code]);
    else
        ret = Response::fromString(code, message, HttpErrorPage(code, message).createPage());
    return ret;
}

struct sockaddr_in Host::getSockAddr() const {
	return sockAddr;
}

const std::string& Host::getName() const {
    return names.front();
}

const std::map<std::string, std::string>& Host::getErrorPages() const
{ return (errorPages); }

uint64_t Host::getMaxBodySize() const
{ return (maxBodySize); }

const std::string& Host::getRoot() const
{ return (root); }

const std::vector<std::string>& Host::getIndexPages() const
{ return (index); }

Response Host::processRequest(const Request& r) {
    Response ret;
    std::string fullPath;
    struct stat fStat;
    std::map<std::string, std::string> errorMap;

     if (r.isFlagError())
         return makeError(r.getError().first, r.getError().second);
    fullPath = joinPath(root, r.getPath());
    if (stat(fullPath.c_str(), &fStat))
         return makeError("404", "Not Found");
     if (r.getMethod() == "GET") {
         if (fStat.st_mode & S_IFDIR) {
             if (index.size() > 0) {
                 for (size_t i = 0; i < index.size(); ++i) {
                     fullPath = joinPath(root, index[i]);
                     if (stat(fullPath.c_str(), &fStat) == 0)
                         return Response::fromFile("200", "OK", fullPath);
                 }
             }
//			 return Response::fromString("200", "OK", "");
//             if (autoindex)
//                 ret = Response::fromString("200", "OK", makeAutoindex(r.getPath()));
//             else
//                 ret = makeError("403", "Forbidden");
         } else
             return Response::fromFile("200", "OK", fullPath);
     } else if (r.getMethod() == "HEAD") {
         if (fStat.st_mode & S_IFDIR)
             return makeError("403", "Forbidden");
         return Response::fromFileNoBody("200", "OK", fullPath);
     } else
         return makeError("501", "Not Implemented");
    return makeError("404", "Not Found");
}
