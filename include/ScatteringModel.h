#ifndef SCATTERING_MODEL_H
#define SCATTERING_MODEL_H

#include <armadillo>
#include <boost/random.hpp>
#include <memory>

namespace ScatteringModel {

struct Event {
    arma::vec3 k;
    double t;
};

class Base {
   public:
    virtual const Event NextEvent(const arma::vec3& k0) =0;
};

template <typename Engine>
class Isotropic3D : public Base {
   private:
    double scattering_rate;
    std::shared_ptr<Engine> engine;

   public:
    Isotropic3D() = delete;
    Isotropic3D(double scattering_rate, std::shared_ptr<Engine> engine);
    const Event NextEvent(const arma::vec3& k0) override;
};

template <typename Engine>
Isotropic3D<Engine>::Isotropic3D(double scattering_rate, std::shared_ptr<Engine> engine)
    : scattering_rate(scattering_rate), engine(engine){};

template <typename Engine>
const Event Isotropic3D<Engine>::NextEvent(const arma::vec3& k0) {
    boost::random::uniform_on_sphere<double, arma::vec> RandUnitVec(3);
    boost::random::exponential_distribution<> ExpDist(scattering_rate);

    return Event{RandUnitVec(*(this->engine)), ExpDist(*(this->engine))};
}
}


#endif  // SCATTERING_MODEL_H
