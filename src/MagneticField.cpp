#include <memory>

#include <yaml-cpp/yaml.h>
#include <armadillo>

#include "MagneticField.h"
#include "Misc.h"

namespace YAML {

template <>
std::unique_ptr<MagneticField::Base> Node::as() const {
	auto type_name = (*this)["type"].as<std::string>();
	return MagneticField::Base::get_factories()
	    .at(type_name)
	    ->create_from_YAML(*this);
}

}  // namespace YAML

namespace MagneticField {

constexpr char Zero::type_name[];
std::unique_ptr<Base> Zero::Factory::create_from_YAML(const YAML::Node&) {
	return std::make_unique<Zero>();
}

const arma::vec3 Zero::advance(const arma::vec3& s0, double t0, double t,
			       const arma::vec3& bconst) {
	return Misc::Rotate(s0, bconst * (t - t0));
}

constexpr char Step::type_name[];
std::unique_ptr<Base> Step::Factory::create_from_YAML(const YAML::Node& node) {
	auto field = node["field"].as<arma::vec3>();
	auto tstep = node["t0"].as<double>();
	return std::make_unique<Step>(field, tstep);
}
const arma::vec3 Step::advance(const arma::vec3& s0, double t0, double t,
			       const arma::vec3& bconst) {
	if (t0 < this->tstep) {
		if (t < this->tstep) {
			return Misc::Rotate(s0, bconst * (t - t0));
		} else {
			arma::vec3 sAtTstep =
			    Misc::Rotate(s0, bconst * (tstep - t0));
			return Misc::Rotate(
			    s0, (bconst + this->field) * (t - tstep));
		}
	} else {
		return Misc::Rotate(s0, (bconst + this->field) * (t - t0));
	}
}
}  // namespace MagneticField
