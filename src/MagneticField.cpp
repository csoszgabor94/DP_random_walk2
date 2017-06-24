#include "MagneticField.h"
#include <armadillo>
#include "Misc.h"

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
            arma::vec3 sAtTstep = Misc::Rotate(s0, bconst * (tstep - t0));
            return Misc::Rotate(s0, (bconst + this->field) * (t - tstep));
        }
    } else {
        return Misc::Rotate(s0, (bconst + this->field) * (t - t0));
    }
}

}
