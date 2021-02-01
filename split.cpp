#include <string>
#include <vector>

std::vector<std::string> split(const std::string& str, const std::string& delimeter)
{
	std::vector<std::string> items;
	size_t prev = 0, pos = str.find(delimeter, prev);

	while ((pos < str.length()) && (prev < str.length())) {
		if (pos == std::string::npos)
			pos = str.length();
		std::string item = str.substr(prev, pos - prev);
		if (!item.empty())
			items.push_back(item);
		prev = pos + delimeter.length();
		pos = str.find(delimeter, prev);
	}
	if (str.size() != prev)
		items.push_back(str.substr(prev, pos - prev));
	return (items);
}
