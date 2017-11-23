#include <memory>
#include <random>
#include <stdexcept>
#include <string>
using namespace std::string_literals;

#include <yaml-cpp/yaml.h>
#include <armadillo>
#include <boost/random/uniform_on_sphere.hpp>

#include "Random.h"
#include "ScatteringModel.h"
#include "Misc.h"

namespace YAML {

template <>
std::unique_ptr<ScatteringModel::Base> Node::as() const {
	auto type_name = Misc::mapat(*this, "type").as<std::string>();
	try {
	return ScatteringModel::Base::get_factories()
	    .at(type_name)
	    ->create_from_YAML(*this);
	} catch (const std::out_of_range&) {
		throw YAML::RepresentationException(
		    this->Mark(), "Unrecognized type: "s + type_name);

	}
}

}  // namespace YAML

namespace ScatteringModel {

Event Isotropic3D::NextEvent(const arma::vec3&) {
	boost::random::uniform_on_sphere<double, arma::vec> RandUnitVec(3);
	std::exponential_distribution<> ExpDist(scattering_rate);

	return Event{RandUnitVec(get_random_engine()),
		     ExpDist(get_random_engine())};
}

}  // namespace ScatteringModel
template class RegisterSubclass2<ScatteringModel::Isotropic3D,
				 ScatteringModel::Subclass_policy>;
