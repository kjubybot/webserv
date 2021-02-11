#include "CGI.hpp"

CGI::CGI(const std::string& path, const std::string& source, const Request& request)
	: _cgiPath(path), _cgiSource(source), _request(request)
{ }

CGI::~CGI()
{ }

CGI::CGI(const CGI& cgi)
	: _cgiPath(cgi._cgiPath), _cgiSource(cgi._cgiSource), _request(cgi._request)
{ }

std::string CGI::processCGI()
{
	std::string result;

	try {
		result = executeCGI();
	}
	catch (const std::exception& ex) {
		std::cerr << "CGI exception: " << ex.what() << std::endl; // 500 internal error in host generated
	}
	return (result);
}

std::string CGI::executeCGI()
{
	int		fd[2];
	pid_t	pid;
	int 	exec_status = 0;
	int		status = 0;
	char** args = formArgs();
	char** envs = formEnvs();

	if (pipe(fd) < 0)
		throw std::runtime_error("pipe fails");
	pid = fork();
	if (pid < 0) {
		throw std::runtime_error("fork fails");
	}
	else if (pid > 0) {
		close(fd[0]);
		// write(fd[0], content, content-length);
		close(fd[1]);
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			status = WEXITSTATUS(status);
		if (status)
			throw std::runtime_error("CGI fails");
	}
	else {
		close(fd[1]);
		int outputFd = open("./cgi_response", O_RDWR | O_CREAT | O_TRUNC,
			S_IRWXU | S_IRGRP | S_IROTH);
		if (dup2(fd[0], 0) < 0)
			throw std::runtime_error("dup2 fails");
		if (dup2(outputFd, 1) < 0)
			throw std::runtime_error("dup2 fails");
		exec_status = execve(this->_cgiPath.c_str(), args, envs);
		close(outputFd);
		close(fd[0]);
		exit(exec_status);
	}
	freeMatrix(args);
	freeMatrix(envs);
	return (getFileContent("./cgi_response"));
}

char** CGI::formArgs() const
{
	char** args = new char*[3];
	args[0] = stringDup(this->_cgiPath);
	args[1] = stringDup(this->_cgiSource);
	args[2] = NULL;
	return (args);
}

char** CGI::formEnvs() const
{
	std::map<std::string, std::string> strEnvs;
	char dirPath[4096];

	if (_request.getHeaders().count("authorization")) {
		std::vector<std::string> authVec = split(_request.getHeaders().at("authorization"), " ");
		if (authVec.size() == 2 && authVec[0] == "Basic") {
			strEnvs["AUTH_TYPE"] = authVec[0];
			strEnvs["REMOTE_USER"] = ""; // encode this shit
			strEnvs["REMOTE_IDENT"] = ""; // encode this shit
		}
	}

	strEnvs["CONTENT_LENGTH"] = _request.getContent().length();
	strEnvs["CONTENT_TYPE"] = _request.getHeaders().at("content-type");
	strEnvs["GATEWAY_INTERFACE"] = "CGI/1.1";
	strEnvs["PATH_INFO"] = _request.getPath();
	getcwd(dirPath, 4096);
	strEnvs["PATH_TRANSLATED"] = std::string(dirPath) + _request.getPath();

	std::vector<std::string> uriElems = splitUri(_request.getPath());
	strEnvs["QUERY_STRING"] = uriElems[1];
	strEnvs["REQUEST_METHOD"] = _request.getMethod();
	strEnvs["REQUEST_URI"] = uriElems[0];

//	strEnvs["SCRIPT_NAME"] = "...";

	strEnvs["REMOTE_ADDR"] = "127.0.0.1";
	strEnvs["SERVER_NAME"] = "";
	strEnvs["SERVER_PORT"] = "8081";
	strEnvs["SERVER_PROTOCOL"] = "HTTP/1.1";
	strEnvs["SERVER_SOFTWARE"] = "webserv/1.0";


/*
	// add headers (this headers started with HTTP_, and - replaced by _)
	 for (iterator it = request->headers.begin(); it != request->headers.end(); it++) {
	 	std::string header = it->first;
	 	header.replace(header.find("-"), 1, "_");
	 	std::transform(str.begin(), str.end(),str.begin(), toupper);
	 	if (strEnvs.count(header) == 0)
	 		strEnvs["HTTP_" + header] = it->second;
	}

	// add passed to main envs
	 size_t i = 0;
	 while (mainEnvs[i] != NULL) {
	 	std::string env(mainEnvs[i]);
	 	std::string key = env.substr(0, env.find("="));
	 	std::string value = env.substr(env.find("=") + 1);
	 	strEnvs[key] = value;
	 	i++;
	 }
*/

	char** envs = new char* [strEnvs.size() + 1];
	size_t i = 0;
	for (std::map<std::string, std::string>::iterator it = strEnvs.begin(); it != strEnvs.end(); it++)
		envs[i++] = stringDup(it->first + "=" + it->second);
	envs[i] = NULL;

	return (envs);
}

std::vector<std::string> CGI::splitUri(const std::string& uri) const
{
	std::vector<std::string> uriElems;
	if (uri.find_first_of('?') != std::string::npos) {
		uriElems.push_back(uri.substr(0, uri.find_first_of('?')));
		uriElems.push_back(uri.substr(uri.find_first_of('?') + 1));
	}
	else {
		uriElems.push_back(uri);
		uriElems.push_back("");
	}
	return (uriElems);
}

const std::string& CGI::getPath() const
{ return (this->_cgiPath); }

const std::string& CGI::getSource() const
{ return (this->_cgiSource); }
