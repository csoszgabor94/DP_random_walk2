#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
using namespace std::string_literals;

#include <yaml-cpp/yaml.h>

#include "Misc.h"
#include "Output.h"

namespace YAML {

template <>
std::unique_ptr<Output::Base> Node::as() const {
	const auto type_name = Misc::mapat(*this, "type").as<std::string>();
	try {
	return Output::Base::get_factories()
	       .at(type_name)
	       ->create_from_YAML(*this);
	} catch (const std::out_of_range&) {
		throw YAML::RepresentationException(
		    this->Mark(), "Unrecognized type: "s + type_name);
	}
}

}  // namespace YAML

namespace Output {

CSVFile::CSVFile(const std::string& path, bool header)
    : out(new std::ofstream(path)), header(header) {}

void CSVFile::write_header(const std::vector<std::string>& h) {
	auto it = h.begin();
	const auto end_it = h.end();

	// No header
	if (it == end_it) { return; }

	// Else
	*out << "# " << *it;
	++it;
	for (; it != end_it; ++it) {
		*out << ", " << *it;
	}
	*out << '\n';
}

void CSVFile::write_record(const std::vector<double>& r) {
	auto it = r.begin();
	const auto end_it = r.end();

	// No record
	if (it == end_it) { return; }

	// Else
	*out << *it;
	++it;
	for (; it != end_it; ++it) {
		*out << ", " << *it;
	}
	*out << '\n';
}

}  // namespace Output

template class RegisterSubclass2<Output::CSVFile, Output::Subclass_policy>;
