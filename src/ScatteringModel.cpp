#include <memory>
#include <random>

#include <yaml-cpp/yaml.h>
#include <armadillo>
#include <boost/random/uniform_on_sphere.hpp>

#include "Random.h"
#include "ScatteringModel.h"

namespace ScatteringModel {

constexpr char Isotropic3D::type_name[];

std::unique_ptr<Base> Isotropic3D::Factory::create_from_YAML(
    const YAML::Node& node) {
        return std::make_unique<Isotropic3D>(
            node["scattering_rate"].as<double>());
}

const Event Isotropic3D::NextEvent(const arma::vec3& k0) {
        boost::random::uniform_on_sphere<double, arma::vec> RandUnitVec(3);
        std::exponential_distribution<> ExpDist(scattering_rate);

        return Event{RandUnitVec(get_random_engine()),
                     ExpDist(get_random_engine())};
}
}
