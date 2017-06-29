#include <memory>

#include <yaml-cpp/yaml.h>
#include <armadillo>

#include "InitialCondition.h"
#include "MagneticField.h"
#include "Measurement.h"
#include "SOCModel.h"
#include "ScatteringModel.h"

namespace YAML {

template <>
std::unique_ptr<Measurement::Base> Node::as() const {
	auto type_name = (*this)["type"].as<std::string>();
	return Measurement::Base::get_factories()
	    .at(type_name)
	    ->create_from_YAML(*this);
}

}  // namespace YAML

namespace Measurement {

constexpr char Ensamble::type_name[];

std::unique_ptr<Base> Ensamble::Factory::create_from_YAML(
    const YAML::Node& node) {
	return std::make_unique<Ensamble>(
	    node["spin_count"].as<unsigned int>(),
	    node["duration"].as<double>(), node["time_step"].as<double>(),
	    node["initial_condition"]
		.as<std::unique_ptr<InitialCondition::Base>>(),
	    node["scattering_model"]
		.as<std::unique_ptr<ScatteringModel::Base>>(),
	    node["magnetic_field"].as<std::unique_ptr<MagneticField::Base>>(),
	    node["soc_model"].as<std::unique_ptr<SOCModel::Base>>());
}

arma::mat Ensamble::run() {
	return arma::mat{};  // TODO implement
}

arma::mat Ensamble::run(unsigned int) {
	return this->run();  // TODO multithread
}

}  // namespace Measurement
