#include <cstddef>
#include <cstring>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "cli_parser.h"

namespace cli_parser {

struct token {
	const char *begin;
	std::size_t length;
	bool is_key;
};

class exception : public std::runtime_error {
       public:
	explicit exception(const std::string &what_arg)
	    : std::runtime_error(what_arg) {}
	explicit exception(const char *what_arg)
	    : std::runtime_error(what_arg) {}
};

static std::vector<token> tokenize(const int argc, const char *const *argv) {
	bool key = true;
	std::vector<token> result;
	result.reserve(argc);

	for (int i = 0; i < argc; i++) {
		if (key) {
			auto arg_str = std::string(*(argv + i));
			if (arg_str.compare(0, 2, "--") != 0) {
				auto what_stream = std::ostringstream{};
				what_stream
				    << "Expected option type argument at place "
				    << i + 1 << " instead of \"" << arg_str
				    << '\"';
				throw exception(what_stream.str());
			}

			auto xpos = arg_str.find('=', 2);
			if (xpos != std::string::npos) {
				result.push_back(
				    token{*(argv + i) + 2, xpos - 2, true});
				result.push_back(
				    token{*(argv + i) + xpos + 1,
					  arg_str.size() - xpos - 1, false});
			} else {
				result.push_back(token{
				    *(argv + i) + 2, arg_str.size() - 2, true});
				key = false;
			}
		} else {
			result.push_back(token{
			    *(argv + i), std::strlen(*(argv + i)), false});
			key = true;
		}
	}
	return result;
}

std::map<std::string, std::string> parse(const int argc,
					 const char *const *argv) {
	std::map<std::string, std::string> result;
	const auto tokens = tokenize(argc, argv);
	const auto end_it = tokens.end();
	for (auto it = tokens.begin(); it != end_it;) {
		const auto key_token = (*it);
		++it;
		const auto value_token = (*it);
		++it;

		result[std::string(key_token.begin, key_token.length)] =
		    std::string(value_token.begin, value_token.length);
	}
	return result;
}

}  // namespace cli_parser
