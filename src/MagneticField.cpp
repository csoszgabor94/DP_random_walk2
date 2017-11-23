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

const arma::vec3 Zero::advance(const arma::vec3& s0, double t0, double t,
			       const arma::vec3& bconst) {
	return Misc::Rotate(s0, bconst * (t - t0));
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

const arma::vec3 Echo::advance(const arma::vec3& s0, double t0, double t,
			       const arma::vec3& bconst) {
	if (t0 < this->tflip) {
		if (t < this->tflip) {
			return Misc::Rotate(s0, bconst * (t - t0));
		} else {
			return Misc::Rotate(s0, bconst * (2 * tflip - t0 - t));
		}
	} else {
		return Misc::Rotate(s0, -bconst * (t - t0));
	}
}

}  // namespace MagneticField

template class RegisterSubclass2<MagneticField::Zero, MagneticField::Subclass_policy>;
template class RegisterSubclass2<MagneticField::Step, MagneticField::Subclass_policy>;
template class RegisterSubclass2<MagneticField::Echo, MagneticField::Subclass_policy>;
