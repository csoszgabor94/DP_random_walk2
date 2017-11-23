#include <memory>

#include <yaml-cpp/yaml.h>
#include <armadillo>
#include <boost/random/uniform_on_sphere.hpp>

#include "InitialCondition.h"
#include "Misc.h"
#include "Random.h"

namespace YAML {

template <>
std::unique_ptr<InitialCondition::Base> Node::as() const {
	auto type_name = (*this)["type"].as<std::string>();
	return InitialCondition::Base::get_factories()
	    .at(type_name)
	    ->create_from_YAML(*this);
}

}  // namespace YAML

namespace InitialCondition {

State Isotropic3D::roll() {
	boost::random::uniform_on_sphere<double, arma::vec> RandUnitVec(3);

	return State{RandUnitVec(get_random_engine()),
		     RandUnitVec(get_random_engine())};
}

State Polarized3D::roll() {
	boost::random::uniform_on_sphere<double, arma::vec> RandUnitVec(3);

	return State{RandUnitVec(get_random_engine()), this->spin};
}

}  // namespace InitialCondition

template class RegisterSubclass2<InitialCondition::Isotropic3D,
				 InitialCondition::Subclass_policy>;
template class RegisterSubclass2<InitialCondition::Polarized3D,
				 InitialCondition::Subclass_policy>;
