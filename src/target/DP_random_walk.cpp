#include <iostream>

#include <yaml-cpp/yaml.h>
#include <armadillo>

#include "Measurement.h"
#include "cli_parser.h"
#include "globals.h"

int main(int argc, const char* argv[]) try {
	if (argc < 2) {
		std::cerr << "Usage: DP_random_walk <filename> [args...]'n";
		return 1;
	}
	globals::options = cli_parser::parse(argc - 2, argv + 2);
	YAML::Node node = YAML::LoadFile(argv[1]);
	auto measurement_uptr = node.as<std::unique_ptr<Measurement::Base>>();
	measurement_uptr->run();
	return 0;
} catch (const YAML::Exception& e) {
	std::cerr << "YAML Parsing error at line " << e.mark.line << std::endl;
	throw;
}
