#include <memory>

#include <yaml-cpp/yaml.h>
#include <armadillo>

#include "SOCModel.h"

namespace YAML {

template <>
std::unique_ptr<SOCModel::Base> Node::as() const {
	auto type_name = (*this)["type"].as<std::string>();
	return SOCModel::Base::get_factories()
	    .at(type_name)
	    ->create_from_YAML(*this);
}

}  // namespace YAML

namespace SOCModel {

constexpr char Isotropic3D::type_name[];

std::unique_ptr<Base> Isotropic3D::Factory::create_from_YAML(
    const YAML::Node& node) {
	return std::make_unique<Isotropic3D>(node["omega"].as<double>());
}
const arma::vec3 Isotropic3D::omega(const arma::vec3& k) { return k; }
}
