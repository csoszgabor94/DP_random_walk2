#ifndef SOC_MODEL_H
#define SOC_MODEL_H

#include <armadillo>
#include <tuple>

namespace SOCModel {

class Base {
   public:
    virtual const arma::vec3 omega(const arma::vec3& k) = 0;
};

class Isotropic3D : public Base {
   private:
    double o;
   public:
    Isotropic3D() = delete;
    Isotropic3D(double omega): o(omega) {}
    const arma::vec3 omega(const arma::vec3& k) override;
};
}

#endif  // SOC_MODEL_H
