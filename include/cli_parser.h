#ifndef UUID_7F20AFF3_B8A6_49D4_875C_7C4FDF0C0FC3
#define UUID_7F20AFF3_B8A6_49D4_875C_7C4FDF0C0FC3

#include <map>
#include <string>

// Primitive command line parser
//
// Only accepts long options, for each long option exactly one argument.
// An option can be given by --option=value or --option value.
// An option can be given more than once, only the last option is taken
// into account. The result is map of the parsed options.

namespace cli_parser {

std::map<std::string, std::string> parse(const int argc,
					 const char *const *argv);

}  // namespace cli_parser

#endif  //  UUID_7F20AFF3_B8A6_49D4_875C_7C4FDF0C0FC3
