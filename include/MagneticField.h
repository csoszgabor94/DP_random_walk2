#ifndef MAGNETIC_FIELD_H
#define MAGNETIC_FIELD_H

#include <armadillo>

namespace MagneticField {

class Base {
   public:
    virtual const arma::vec3 advance(const arma::vec3& s0, double t0, double t,
                              const arma::vec3& bconst) = 0;
};

class Zero : public Base {
   public:
    const arma::vec3 advance(const arma::vec3& s0, double t0, double t,
                      const arma::vec3& bconst) override;
};

class Step : public Base {
   private:
    arma::vec3 field;
    double tstep;

   public:
    Step() = delete;
    inline Step(const arma::vec3& field, double tstep)
        : field(field), tstep(tstep) {}
    const arma::vec3 advance(const arma::vec3& s0, double t0, double t,
                      const arma::vec3& bconst) override;
};
}

#endif  // MAGNETIC_FIELD_H
