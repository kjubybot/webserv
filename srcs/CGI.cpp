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

	if (this->_request.getHeaders().count("authorization")) {
		std::vector<std::string> authVec = split(this->_request.getHeaders()
			.at("authorization"), " ");
		strEnvs["AUTH_TYPE"] = authVec[0];
		if (authVec[0] == "Basic") {
			std::vector<std::string> splitBase64 = split(decode(authVec[1]), ":");
			strEnvs["REMOTE_USER"] = splitBase64[0];
			strEnvs["REMOTE_IDENT"] = splitBase64[1];
		}
		else {
			strEnvs["REMOTE_USER"] = "";
			strEnvs["REMOTE_IDENT"] = "";
		}
	}

	//
	strEnvs["REMOTE_ADDR"] = iptoa(this->_request.getSockAddr().sin_addr.s_addr); // client ip

	strEnvs["CONTENT_LENGTH"] = this->_request.getContent().length() != 0 ?
		std::to_string(this->_request.getContent().length()) : "";
	strEnvs["CONTENT_TYPE"] = this->_request.getHeaders().count("content-type") ?
		this->_request.getHeaders().at("content-type") : "";
	strEnvs["GATEWAY_INTERFACE"] = "CGI/1.1";
	strEnvs["SERVER_PROTOCOL"] = "HTTP/1.1";
	strEnvs["SERVER_SOFTWARE"] = "webserv/1.0";

	//
	strEnvs["SERVER_NAME"] = "localhost"; // set in config
	strEnvs["SERVER_PORT"] = "8081"; // set in config

	strEnvs["REQUEST_METHOD"] = this->_request.getMethod();



	// PATH_INFO
	//PATH_TRANSLATED
	//QUERY_STRING
	//REQUEST_URI
	//SCRIPT_NAME






//	getcwd(pwd, 1024);
    strEnvs["PATH_INFO"] = this->_request.getPath();


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

std::string CGI::decode(const std::string& input) const
{
	char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	std::string result;
	std::vector<int> base(256,-1);
	for (int i = 0; i < 64; i++)
		base[base64[i]] = i;

	int val = 0, valb = -8;
	for (size_t i = 0; i < input.size(); i++) {
		if (base[input[i]] == -1)
			break;
		val = (val << 6) + base[input[i]];
		valb += 6;
		if (valb >= 0) {
			result.push_back(char((val >> valb) & 0xff));
			valb -= 8;
		}
	}
	return result;
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
