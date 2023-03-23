#include <iostream>
#include <vector>
#include <string>
#include <sstream>

int main()
{
	std::string hex_chars("AE 15 00 00");

	std::istringstream hex_chars_stream(hex_chars);
	std::vector<unsigned char> bytes;

	unsigned int c;
	while (hex_chars_stream >> std::hex >> c)
	{
    		bytes.push_back(c);
	}
	for (std::vector<unsigned char>::const_iterator i = bytes.begin(); i != bytes.end(); ++i)
    		std::cout << *i << ' ';
}
