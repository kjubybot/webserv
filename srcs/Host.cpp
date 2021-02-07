#include "Host.hpp"

Host::Host(const Config::ConfigServer& server)
{
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = 0;

	sockAddr.sin_addr.s_addr = inet_addr(server.getHost().c_str());
	sockAddr.sin_port = server.getPort();
	sockAddr.sin_port = (sockAddr.sin_port >> 8) | (sockAddr.sin_port << 8);
	names = server.getNames();
	errorPages = server.getErrorPages();
	maxBodySize = server.getMaxBodySize();
	root = server.getRoot();
	index = server.getIndexPages();
	locations = server.getLocations();
}

Host::~Host()
{ }

Host::Host(const Host& h)
    : sockAddr(h.sockAddr), names(h.names), errorPages(h.errorPages), maxBodySize(h.maxBodySize), root(h.root), index(h.index), locations(h.locations)
{ }

Host& Host::operator=(const Host& h) {
    sockAddr = h.sockAddr;
    names = h.names;
    errorPages = h.errorPages;
	maxBodySize = h.maxBodySize;
	root = h.root;
	index = h.index;
	locations = h.locations;
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

std::string Host::getName() const {
	if (names.empty())
		return ("");
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
     fullPath = root + r.getPath();
     if (stat(fullPath.c_str(), &fStat))
         return makeError("404", "Not Found");
     if (r.getMethod() == "GET") {
         if (fStat.st_mode & S_IFDIR) {
			 ret = Response::fromString("200", "OK", "");
//             if (autoindex)
//                 ret = Response::fromString("200", "OK", makeAutoindex(r.getPath()));
//             else
//                 ret = makeError("403", "Forbidden");
         } else
             ret = Response::fromFile("200", "OK", fullPath);
     } else
         ret = makeError("501", "Not Implemented");
    return ret;
}
