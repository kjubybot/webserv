#include <iostream>
#include "HttpErrorException.hpp"
#include "HttpErrorPage.hpp"

int main()
{
	try {
		throw HttpErrorException("404", "Not found");
	}
	catch (const HttpErrorException& ex) {
		std::cout << "what: " << ex.what() << "\n";
		std::cout << "code: " << ex.getCode() << "\n";
		std::cout << "description: " << ex.getDescription() << "\n";
		HttpErrorPage page(ex.getCode(), ex.getDescription());
		page.createPage();
	}


}
