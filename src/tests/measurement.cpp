#include <iostream>

#include <yaml-cpp/yaml.h>
#include <armadillo>

#include "Measurement.h"

int main() {
	try {
		YAML::Node node = YAML::LoadFile("Measurement.yaml");
		auto measurement_uptr =
		    node.as<std::unique_ptr<Measurement::Base>>();
		auto results = measurement_uptr->run();
		std::cout << results << std::endl;
	} catch (const YAML::Exception& e) {
		std::cerr << "YAML Parsing error at line " << e.mark.line
			  << std::endl;
		throw;
	}

	return 0;
}
