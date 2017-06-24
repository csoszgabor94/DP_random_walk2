#ifndef INITIAL_CONDITION_H
#define INITIAL_CONDITION_H

#include <armadillo>
#include <boost/random.hpp>
#include <memory>

namespace InitialCondition {

struct State {
    arma::vec3 k;
    arma::vec3 spin;
};

class Base {
   public:
    virtual State roll() = 0;
};

template <typename Engine>
class Isotropic3D : public Base {
   private:
    std::shared_ptr<Engine> engine;

   public:
    Isotropic3D() = delete;
    Isotropic3D(std::shared_ptr<Engine> engine) : engine(engine) {}
    State roll() override;
};

template <typename Engine>
State Isotropic3D<Engine>::roll() {
    boost::random::uniform_on_sphere<double, arma::vec> RandUnitVec(3);

    return State { RandUnitVec(*(this->engine)), RandUnitVec(*(this->engine)) };
}

template <typename Engine>
class Polarized3D : public Base {
   private:
    arma::vec3 spin;
    std::shared_ptr<Engine> engine;

   public:
    Polarized3D() = delete;
    Polarized3D(arma::vec3 spin, std::shared_ptr<Engine> engine) : spin(spin), engine(engine) {}
    State roll() override;
};

template <typename Engine>
State Polarized3D<Engine>::roll() {
    boost::random::uniform_on_sphere<double, arma::vec> RandUnitVec(3);

    return State { RandUnitVec(*(this->engine)), this->spin };
}

}

#endif  // INITIAL_CONDITION_H
