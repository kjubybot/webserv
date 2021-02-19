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
	port = server.getPort();
	auth = server.getAuth();
	locations = server.getLocations();
	locations.sort(Host::forSortingByLength);
	regLocations = server.getRegexLocations();
}

Host::~Host()
{ }

Host::Host(const Host& h)
    :   sockAddr(h.sockAddr), names(h.names), errorPages(h.errorPages), maxBodySize(h.maxBodySize), root(h.root),
        index(h.index), port(h.port), auth(h.auth), locations(h.locations), regLocations(h.regLocations)
{ }

Host& Host::operator=(const Host& h) {
    sockAddr = h.sockAddr;
    names = h.names;
    errorPages = h.errorPages;
	maxBodySize = h.maxBodySize;
	root = h.root;
	index = h.index;
	port = h.port;
	auth = h.auth;
	locations = h.locations;
	regLocations = h.regLocations;
	return (*this);
}

Response Host::makeError(const std::string& code, const std::string& message, const std::string& curr_root) {
	Response ret;
	struct stat fStat;

	if (errorPages.find(code) != errorPages.end() && stat(joinPath(curr_root, errorPages[code]).c_str(), &fStat) == 0)
		ret = Response::fromFile(code, message, joinPath(curr_root, errorPages[code]));
	else
		ret = Response::fromString(code, message, HttpErrorPage(code, message).createPage());
	return ret;
}

bool Host::forSortingByLength(const conf_loc& a, const conf_loc& b) {
	return a._name.length() > b._name.length();
}

struct sockaddr_in Host::getSockAddr() const {
	return sockAddr;
}

std::string Host::getName() const {
	return names.front();
}

uint64_t Host::getMaxBodySize(const Request& request) {
	std::list<conf_loc>::iterator it = matchLocation(request.getPath());
	if (it != locations.end())
		return it->getMaxBodySize();
	else
		return maxBodySize;
}

const std::string& Host::getRoot() const
{ return (root); }

uint16_t Host::getPort() const
{ return (port); }

const std::vector<std::string>& Host::getIndexPages() const
{ return (index); }

bool Host::matchExtension(const std::string& ext, conf_loc& loc) {
	for (std::vector<std::string>::iterator it = loc._cgiExtensions.begin(); it != loc._cgiExtensions.end(); ++it)
		if (ext == *it)
			return true;
	return false;
}

bool Host::isAuthorized(const Request& request)
{
	if (!request.getHeaders().count("authorization") || this->auth.empty())
		return (true);
	std::vector<std::string> authVec = split(request.getHeaders().at("authorization"), " ");
	if (authVec[0] == "Basic") {
		for (std::vector<std::string>::iterator it = this->auth.begin(); it != this->auth.end(); it++) {
			if (decodeBase64(authVec[1]) == *it)
				return (true);
		}
		return (false);
	}
	return (true);
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

std::list<Config::ConfigServer::ConfigLocation>::iterator Host::matchLocation(const std::string& loc) {
    std::list<conf_loc>::iterator it = locations.begin();
    size_t slash;
	if (locations.empty()) {
	    return locations.end();
    }
    while (it != --locations.end()) {
        slash = it->_name.rfind('/');
        if (it->_name.compare(0, slash > 0 ? slash : it->_name.length(), loc, 0, slash > 0 ? slash : it->_name.length()) == 0)
            return it;
        ++it;
    }
    if (loc == "/" && (--locations.end())->getName() == "/")
		return (--locations.end());
	std::list<conf_loc>::iterator jt = regLocations.begin();
    while (jt != regLocations.end()) {
    	std::regex regex(jt->getName(), std::regex_constants::ECMAScript);
	    if (std::regex_match(loc, regex))
	    	return (jt);
	    jt++;
    }
	return (--locations.end());
}

std::string Host::matchRegexUri(std::string path, std::string locRoot)
{
	std::vector<std::string> locs = split(locRoot, "/");
	std::vector<std::string> splitPath = split(path, "/");

	for (size_t i = 0; i < locs.size(); i++) {
		std::string currRoot;
		for (size_t j = i; j < locs.size(); j++) {
			if (j == i)
				currRoot = locs[j];
			else
				currRoot += "/" + locs[j];
			if (path.find(currRoot) == 0)
				return (matchRegexUri(path.erase(0, currRoot.size()),
				locRoot.erase(0, currRoot.size())));
			if (path.find("/" + currRoot) == 0)
				return (matchRegexUri(path.erase(0, currRoot.size() + 1),
				locRoot.erase(0, currRoot.size() + 1)));
		}
	}
	return (path);
}

Response Host::processRequest(const Request& r) {
    Response ret;
    std::string fullPath, realRoot, uri, indexPath;
    struct stat fStat;
    std::map<std::string, std::string> errorMap;
    std::list<conf_loc>::iterator locIt;
    std::vector<std::string> indexes;

	if ((locIt = matchLocation(r.getPath())) == locations.end()) {
		realRoot = root;
		uri = r.getPath();
	} else {
		realRoot = locIt->_root;
		std::vector<std::string> regLocs;
		for (std::list<conf_loc>::iterator itt = regLocations.begin(); itt != regLocations.end(); itt++)
			regLocs.push_back(itt->getName());
		if (isIn(regLocs, locIt->getName())) {
			if (r.getPath().length() > 0)
				uri = matchRegexUri(r.getPath(), realRoot);
			else
				uri = r.getPath();
		}
		else {
			if (r.getPath().length() > 0)
				uri = r.getPath().substr(locIt->_name.rfind('/'));
			else
				uri = r.getPath();
		}
	}
    if (r.isFlagError())
		return makeError(r.getError().first, r.getError().second, realRoot);
	if (!isAuthorized(r))
		return makeError("401", "Unauthorized", realRoot);
	if (locIt != locations.end() && !isIn(locIt->_allowedMethods, r.getMethod())) {
        ret = makeError("405", "Method Not Allowed", realRoot);
        std::string allow;
        for (size_t i = 0; i < locIt->_allowedMethods.size(); ++i) {
            allow += locIt->_allowedMethods[i];
            if (i + 1 != locIt->_allowedMethods.size())
                allow += ", ";
        }
        ret.setHeader("Allow", allow);
        return ret;
    }
    fullPath = joinPath(realRoot, uri);
     if (r.getMethod() == "GET" || r.getMethod() == "HEAD") {
         if (stat(fullPath.c_str(), &fStat))
             return makeError("404", "Not Found", realRoot);
         if (fStat.st_mode & S_IFDIR) {
             if (locIt != locations.end() && !locIt->_index.empty())
                 indexes = locIt->_index;
             else
                 indexes = index;
             if (!indexes.empty()) {
                 for (size_t i = 0; i < indexes.size(); ++i) {
                     indexPath = joinPath(fullPath, indexes[i]);
                     if (stat(indexPath.c_str(), &fStat) == 0) {
                         if (r.getMethod() == "GET")
                             return Response::fromFile("200", "OK", indexPath);
                         else
                             return Response::fromFileNoBody("200", "OK", indexPath);
                     }
                 }
                 return makeError("404", "Not Found", realRoot);
             } else if ((locIt != locations.end() && locIt->getAutoIndex())) {
                 if (r.getMethod() == "GET")
                     return Response::fromString("200", "OK", makeAutoindex(fullPath));
                 else
                     return Response::fromStringNoBody("200", "OK", makeAutoindex(fullPath));
             } else
                 return makeError("403", "Forbidden", realRoot);
         } else {
             if (r.getMethod() == "GET")
                 return Response::fromFile("200", "OK", fullPath);
             else
                 return Response::fromFileNoBody("200", "OK", fullPath);
         }
     } else if (r.getMethod() == "PUT") {
         int fd;
         if (locIt != locations.end())
             fullPath = joinPath(realRoot, locIt->_uploadPath);
         if (stat(fullPath.c_str(), &fStat) && mkdir(fullPath.c_str(), 0755))
             return makeError("501", "Internal Server Error", realRoot);
         fullPath = joinPath(fullPath, uri);
         if (stat(fullPath.c_str(), &fStat)) {
             fd = open(fullPath.c_str(), O_WRONLY | O_CREAT, 0664);
             write(fd, r.getContent().data(), r.getContent().length());
             close(fd);
             ret = Response::fromStringNoBody("201", "Created", "");
             ret.setHeader("Content-Location", "http://" + joinPath(names.front(), joinPath(locIt->_uploadPath, r.getPath())));
         } else {
             fd = open(fullPath.c_str(), O_WRONLY | O_TRUNC);
             write(fd, r.getContent().data(), r.getContent().length());
             close(fd);
             ret = Response::fromStringNoBody("204", "No Content", "");
         }
         return ret;
     }
	 else if (r.getMethod() == "POST") {
	     size_t dot = uri.rfind('.');
		 if (dot != std::string::npos && matchExtension(uri.substr(dot), *locIt)) {
			 CGI cgi(locIt->getCGIPath(), fullPath, r);
			 try {
				 std::string resp = cgi.processCGI(*this);
				 return Response::fromCGI(resp);
			 }
			 catch (const std::exception& ex) {
				 return Response::fromStringNoBody("500", "Internal Error", "");
			 }
		 }
		 return Response::fromStringNoBody("200", "OK", "");
	 }
	 else
		 return makeError("501", "Not Implemented", realRoot);
}
