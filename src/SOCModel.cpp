#include "SOCModel.h"
#include <armadillo>

namespace SOCModel {

const arma::vec3 Isotropic3D::omega(const arma::vec3& k) { return k; }

}
