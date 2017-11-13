#include <memory>

#include <yaml-cpp/yaml.h>
#include <armadillo>

#include "SOCModel.h"
#include "Misc.h"

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
arma::vec3 Isotropic3D::omega(const arma::vec3& k) const { return k; }

constexpr char Zeeman::type_name[];
std::unique_ptr<Base> Zeeman::Factory::create_from_YAML(
    const YAML::Node& node) {
	auto bm_ptr = node["base_model"].as<std::unique_ptr<Base>>();
	const auto bfield = node["field"].as<arma::vec3>();
	return std::make_unique<Zeeman>(bfield, std::move(bm_ptr));
}
arma::vec3 Zeeman::omega(const arma::vec3& k) const {
	return base_model->omega(k) + bfield;
}

}  // namespace SOCModel
