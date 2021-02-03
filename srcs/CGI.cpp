#include "CGI.hpp"

CGI::CGI(const std::string& path, const std::string& source)
	: _cgiPath(path), _cgiSource(source)
{ }

CGI::~CGI()
{ }

CGI::CGI(const CGI& cgi)
	: _cgiPath(cgi._cgiPath), _cgiSource(cgi._cgiSource)
{ }

std::string CGI::processCGI()
{
	std::string result;

	try {
		result = executeCGI();
	}
	catch (const std::exception& ex) {
		std::cerr << "CGI exception: " << ex.what() << std::endl;
	}
	return (result);
}

std::string CGI::executeCGI()
{
	int		fd[2];
	pid_t	pid;
	int 	exec_status = 0;

	if (pipe(fd) < 1)
		throw std::runtime_error("pipe fails");

	if ((pid = fork()) < 0) {
		throw std::runtime_error("fork fails");
	}
	else if (pid > 0) {
		int status = 0;
		close(fd[0]);
		// write(fd[1], content, content.size()); // content get by request
		close(fd[1]);
		waitpid(pid, &status, 0);
	}
	else {
		close(fd[1]);
		if (dup2(fd[0], 0) < 0)
			throw std::runtime_error("dup2 fails");
		int outputFd = open("cgi_response", O_RDWR | O_CREAT,
			S_IRWXU | S_IRGRP | S_IROTH);
		if (dup2(fd[1], 1) < 0)
			throw std::runtime_error("dup2 fails");
		char** args = formArgs();
		char** envs = formEnvs();
		// exec_status = execve(_cgiPath, args, envs);
		close(outputFd);
		close(fd[0]);
		freeMatrix(args);
		freeMatrix(envs);
		exit(exec_status);
	}
	return (getFileContent("cgi_response"));
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
	// http://www.example.com/php/path_info.php/some/stuff?foo=bar

	std::map<std::string, std::string> strEnvs;

	strEnvs["SERVER_SOFTWARE"] = "webserv/14.88";
	strEnvs["SERVER_NAME"] = "0.1.2.3"; // config->host
	strEnvs["GATEWAY_INTERFACE"] = "CGI/1.1";
	strEnvs["SERVER_PROTOCOL"] = "HTTP/1.1";
	strEnvs["SERVER_PORT"] = "1000"; // config->port
	strEnvs["REQUEST_METHOD"] = "POST"; // request->method
	strEnvs["PATH_INFO"] = "/"; // for tester
	//strEnvs["PATH_INFO"] = "/some/stuff"; // path after cgi script, request line before ? (if ? not exist - fulll line)
	strEnvs["PATH_TRANSLATED"] = "/"; // for tester
	//strEnvs["PATH_TRANSLATED"] = "/home/bla/bla/some/stuff"; // absolute path to server + PATH_INFO
	strEnvs["SCRIPT_NAME"] = "php/path_info.php"; // location server+ cgi->path
	strEnvs["CONTENT_TYPE"] = "text/html"; // request->content_type
	strEnvs["CONTENT_LENGTH"] = "100"; // request->body->length
	strEnvs["AUTH_TYPE"] = "Basic"; // if authorization header presented - its value before hash, otherwise - not set
	strEnvs["QUERY_STRING"] = "foo=bar"; // request line after ? (if ? not exist - empty line)
	strEnvs["REQUEST_URI"] = "www.example.com/php/path_info.php"; // request->uri
	strEnvs["REMOTE_ADDR"] = "127.0.0.1"; // client ip
	strEnvs["REMOTE_IDENT"] = ""; // if authorization header presented - its value after hash, otherwise - empty
	strEnvs["REMOTE_USER"] = ""; // if authorization header presented - its value after hash, otherwise - empty
	// maybe user can be get by hash of authorization header
	// add custom headers if they are not presented to map (this headers started with HTTP_, and - replaced by _)
	// add passed to main envs

	char** envs = new char* [strEnvs.size() + 1];
	size_t i = 0;
	for (std::map<std::string, std::string>::iterator it = strEnvs.begin(); it != strEnvs.end(); it++)
		envs[i++] = stringDup(it->first + "=" + it->second);
	envs[i] = NULL;
	return (envs);
}

const std::string& CGI::getPath() const
{ return (this->_cgiPath); }

const std::string& CGI::getSource() const
{ return (this->_cgiSource); }
