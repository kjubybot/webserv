#include "CGI.hpp"
#include <iostream>

CGI::CGI(const std::string& path, const std::string& file) // ...
	: _cgiPath(path), _cgiFile(file) // ...
{ }

CGI::~CGI()
{ }

CGI::CGI(const CGI& cgi)
	: _cgiPath(cgi._cgiPath), _cgiFile(cgi._cgiFile)
{ }

void CGI::processContent() const
{

}

char* _strdup(const char* str, size_t size)
{
	char* result = new char [size + 1];
	size_t i = 0;
	while (i < size) {
		result[i] = str[i];
		i++;
	}
	result[i] = '\0';
	return (result);
}

char** CGI::getEnvs() const
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
	strEnvs["SCRIPT_NAME"] = "php/path_info.php"; // cgi->path
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
	for (std::map<std::string, std::string>::iterator it = strEnvs.begin(); it != strEnvs.end(); it++) {
		std::string env = it->first + "=" + it->second;
		envs[i++] = _strdup(env.c_str(), env.size());
	}
	envs[i] = NULL;
	return (envs);
}

void CGI::getProcessedContent() const
{
	char** envs = getEnvs();
	size_t i = 0;
	while (envs[i] != NULL) {
		std::cout << envs[i++] << "\n";
	}

	pid_t pid = fork();
	if (pid < 0) {
		// error
	}
	else if (pid > 0) {
		// parent
	}
	else {

	}
}
