#include "Misc.h"
#include <armadillo>

namespace Misc {

arma::vec3 Rotate(const arma::vec3& v0, const arma::vec3& phi) {
    double phi_scal = arma::norm(phi, 2);
    if (phi_scal == 0.) return v0;
    arma::vec3 dir = phi / phi_scal;
    return cos(phi_scal) * v0 + sin(phi_scal) * arma::cross(dir, v0) +
           (1 - cos(phi_scal)) * arma::dot(dir, v0) * dir;
}

}
