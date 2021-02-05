#include "CGI.hpp"

CGI::CGI(const std::string& path, const std::string& source, Config& config)
	: _cgiPath(path), _cgiSource(source), _config(config)
{ }

CGI::~CGI()
{ }

CGI::CGI(const CGI& cgi)
	: _cgiPath(cgi._cgiPath), _cgiSource(cgi._cgiSource), _config(cgi._config)
{ }

std::string CGI::processCGI()
{
	std::string result;

	try {
		result = executeCGI();
	}
	catch (const std::exception& ex) {
		std::cerr << "CGI exception: " << ex.what() << std::endl;
		// throw 500 internal error ???
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
		status = 0;
		// write(fd[1], content, content-length); ???
		// close(fd[1]); ???
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			status = WEXITSTATUS(status);
		std::cout << "status: " << status << std::endl;
	}
	else {
		// close(fd[1]); ???
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

	/*
	if request->headers.count("Authorization") == 1 {
	 	std::vector<std::string> authVec = split(request->headers["Authorization"];
	 	if (authVec.size() == 2) {
	 		strEnvs["AUTH_TYPE"] = authVec[0];
	 		if (strEnvs["AUTH_TYPE"] == "Basic") {
				strEnvs["REMOTE_USER"] = encode this shit
				strEnvs["REMOTE_IDENT"] = encode this shit
			}
			else {
				strEnvs["REMOTE_USER"] = "";
				strEnvs["REMOTE_IDENT"] = "";
			}
		}
	}

	// example : http://lemp.test/test.php/foo/bar.php?v=1

	strEnvs["SERVER_SOFTWARE"] = "webserv";						// constant
	strEnvs["GATEWAY_INTERFACE"] = "CGI/1.1";					// constant
	strEnvs["SERVER_PROTOCOL"] = "HTTP/1.1";					// constant
	strEnvs["SERVER_NAME"] = "";								// store in Config object
	strEnvs["SERVER_PORT"] = "1000";							// store in Config object
	strEnvs["REQUEST_METHOD"] = "GET";							// store in Request object
	strEnvs["REQUEST_URI"] = "/test.php/foo/bar.php?v=1";		// store in Request object, but need to remove chars after ? (if its presented)
	strEnvs["QUERY_STRING"] = "v=1";							// store in Request object but need to remove chars before ? (if its presented)
	strEnvs["CONTENT_TYPE"] = "text/html";						// store in Request object
	strEnvs["CONTENT_LENGTH"] = "100";							// store in Request object
	strEnvs["REMOTE_ADDR"] = "127.0.0.1";						// store in Connection

	strEnvs["PATH_INFO"] = "/"; // for tester
	// strEnvs["PATH_INFO"] = "/foo/bar.php"; // path after cgi script, request line before ? (if ? not exist - fulll line)
	strEnvs["PATH_TRANSLATED"] = "/"; // for tester
	// strEnvs["PATH_TRANSLATED"] = "/home/foo/bar.php"; // absolute path to server + PATH_INFO
	strEnvs["SCRIPT_NAME"] = "/test.php"; // file without full path

	// add headers if they are not presented to map (this headers started with HTTP_, and - replaced by _)
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

const std::string& CGI::getPath() const
{ return (this->_cgiPath); }

const std::string& CGI::getSource() const
{ return (this->_cgiSource); }
