#include <memory>

#include <yaml-cpp/yaml.h>
#include <armadillo>
#include <boost/random/uniform_on_sphere.hpp>

#include "InitialCondition.h"
#include "Misc.h"
#include "Random.h"

namespace InitialCondition {

constexpr char Isotropic3D::type_name[];

std::unique_ptr<Base> Isotropic3D::Factory::create_from_YAML(
    const YAML::Node&) {
	return std::make_unique<Isotropic3D>();
}

State Isotropic3D::roll() {
	boost::random::uniform_on_sphere<double, arma::vec> RandUnitVec(3);

	return State{RandUnitVec(get_random_engine()),
		     RandUnitVec(get_random_engine())};
}

constexpr char Polarized3D::type_name[];

std::unique_ptr<Base> Polarized3D::Factory::create_from_YAML(
    const YAML::Node& node) {
	return std::make_unique<Polarized3D>(node["spin"].as<arma::vec3>());
}

State Polarized3D::roll() {
	boost::random::uniform_on_sphere<double, arma::vec> RandUnitVec(3);

	return State{RandUnitVec(get_random_engine()), this->spin};
}
}  // namespace InitialCondition
