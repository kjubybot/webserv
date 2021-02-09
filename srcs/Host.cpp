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
	locations.sort(Host::forSortingByLength);
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

std::string Host::makeAutoindex(const std::string& path) const {
    std::string ret = "<!DOCTYPE HTML>"
                      "<html><head><title>Index of " + path
                      + "</title></head>"
                      "<body><h1>Index of " + path
                      + "</h1><hr><pre>";
    std::string fName;
    DIR* dir = opendir(path.c_str());
    struct dirent* dp;

    while ((dp = readdir(dir))) {
        fName = std::string(dp->d_name);
        if (dp->d_type & DT_DIR)
            fName += "/";
        ret +="<a href=\"" + fName;
        ret += "\">" + fName + "</a><br>";
    }
    ret += "</pre></body></html>";
    closedir(dir);
    return ret;
}

Response Host::makeError(const std::string& code, const std::string& message, const std::string& root) {
    Response ret;
    struct stat fStat;

    if (errorPages.find(code) != errorPages.end() && stat(joinPath(root, errorPages[code]).c_str(), &fStat) == 0)
        ret = Response::fromFile(code, message, joinPath(root, errorPages[code]));
    else
        ret = Response::fromString(code, message, HttpErrorPage(code, message).createPage());
    return ret;
}

bool Host::forSortingByLength(const conf_loc& a, const conf_loc& b) {
    return a._root.length() > b._root.length();
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

std::list<Config::ConfigServer::ConfigLocation>::iterator Host::matchLocation(const std::string& loc) {
    std::list<conf_loc>::iterator it = locations.begin();

    if (locations.empty())
        return locations.end();
    while (it != locations.end()) {
        if (it->_root.compare(0, loc.length() - (loc[loc.length() - 1] == '/' ? 2 : 1), loc) == 0)
            return it;
        ++it;
    }
    return --locations.end();
}

Response Host::processRequest(const Request& r) {
    Response ret;
    std::string fullPath, realRoot;
    struct stat fStat;
    std::map<std::string, std::string> errorMap;
    std::list<conf_loc>::iterator locIt;
    std::vector<std::string> indexes;

    if ((locIt = matchLocation(r.getPath())) == locations.end())
        realRoot = root;
    else
        realRoot = locIt->_root;
    if (r.isFlagError())
        return makeError(r.getError().first, r.getError().second, realRoot);
    fullPath = joinPath(realRoot, r.getPath());
     if (r.getMethod() == "GET" || r.getMethod() == "HEAD") {
         if (stat(fullPath.c_str(), &fStat))
             return makeError("404", "Not Found", realRoot);
         if (fStat.st_mode & S_IFDIR) {
             if (locIt != locations.end() && !locIt->_index.empty())
                 indexes = locIt->_index;
             else
                 indexes = index;
             if (!indexes.empty()) {
                 for (size_t i = 0; i < index.size(); ++i) {
                     fullPath = joinPath(realRoot, index[i]);
                     if (stat(fullPath.c_str(), &fStat) == 0) {
                         if (r.getMethod() == "GET")
                             return Response::fromFile("200", "OK", fullPath);
                         else
                             return Response::fromFileNoBody("200", "OK", fullPath);
                     }
                 }
             }
             if ((locIt != locations.end() && locIt->getAutoIndex())) {
                 if (r.getMethod() == "GET")
                     return Response::fromString("200", "OK", makeAutoindex(realRoot));
                 else
                     return Response::fromStringNoBody("200", "OK", makeAutoindex(realRoot));
             } else
                 return makeError("403", "Forbidden", realRoot);
         } else {
             if (r.getMethod() == "GET")
                 return Response::fromFile("200", "OK", realRoot);
             else
                 return Response::fromFileNoBody("200", "OK", realRoot);
         }
     } else if (r.getMethod() == "PUT") {
         int fd;
         if (stat(fullPath.c_str(), &fStat)) {
             fd = open(fullPath.c_str(), O_WRONLY | O_CREAT);
             write(fd, r.getContent().data(), r.getContent().length());
             close(fd);
             return Response::fromStringNoBody("201", "Created", r.getContent());
         } else {
             fd = open(fullPath.c_str(), O_WRONLY | O_TRUNC);
             write(fd, r.getContent().data(), r.getContent().length());
             close(fd);
             return Response::fromStringNoBody("200", "OK", r.getContent());
         }
     } else
         return makeError("501", "Not Implemented", realRoot);
    return makeError("404", "Not Found", realRoot);
}
