#include "HttpErrorPage.hpp"

HttpErrorPage::HttpErrorPage(const std::string& code, const std::string& description)
	: _code(code), _description(description)
{
	this->_content = "<!DOCTYPE html>\n\
					 <html lang=\"en\">\n\
					 <head>\n\
					 \t<meta charset=\"UTF-8\">\n\
					 \t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
					 \t<title>Error page</title>\n\
					 \n\
					 \t<style>\n\
					 \t\t* {\n\
					 \t\t\ttext-align: center;\n\
					 \t\t\tfont-family: 'Menlo', sans-serif;\n\
					 \t\t\tcolor: #424744;\n\
					 \t\t}\n\
					 \n\
					 \t\t.errorCode {\n\
					 \t\t\tfont-size: 28px;\n\
					 \t\t\tfont-weight: bold;\n\
					 \t\t\tmargin-top: 5%;\n\
					 \t\t\tcolor: #000;\n\
					 \t\t}\n\
					 \n\
					 \t\t.errorDescription {\n\
					 \t\t\tfont-size: 24px;\n\
					 \t\t\tmargin-top: 3%;\n\
					 \t\t}\n\
					 \n\
					 \t\t.plainText {\n\
					 \t\t\twidth: 80%;\n\
					 \t\t\tposition: absolute;\n\
					 \t\t\tleft: 10%;\n\
					 \t\t\tbottom: 5%;\n\
					 \t\t\tpadding-top: 1%;\n\
					 \t\t\tborder-top: 2px solid #404a43;\n\
					 \t\t}\n\
					 \t</style>\n\
					 </head>\n\
					 <body>\n\
					 \t<div class=\"errorCode\">Error code: %***%</div>\n\
					 \t<div class=\"errorDescription\">Error description: %&&&%</div>\n\
					 \t<div class=\"plainText\">webserv</div>\n\
					 </body>\n\
					 </html>";

		this->_content.replace(this->_content.find("%***%"),
			std::string("%***%").size(), this->_code);
		this->_content.replace(this->_content.find("%&&&%"),
			std::string("%&&&%").size(), this->_description);
}

HttpErrorPage::~HttpErrorPage()
{ }

std::string HttpErrorPage::createPage()
{
	std::string filename = "Page" + this->_code + ".html";
	int fd = open(filename.c_str(), O_WRONLY | O_CREAT,
		S_IRWXU | S_IRGRP | S_IROTH);
	write(fd, this->_content.c_str(), this->_content.size());
	close(fd);
	return (filename);
}
