#include <boost/random.hpp>
#include <memory>
#include "InitialCondition.h"
#include "MagneticField.h"
#include "SOCModel.h"
#include "ScatteringModel.h"

int main() {
    typedef boost::random::mt19937 engine_t;

    auto engine = std::make_shared<engine_t>();
    MagneticField::Step ext_magnetic_field =
        MagneticField::Step({0., 0., 1.}, 0.);
    SOCModel::Isotropic3D soc_model = SOCModel::Isotropic3D{0.2};
    ScatteringModel::Isotropic3D<engine_t> scattering_model{1., engine};
    InitialCondition::Polarized3D<engine_t> initial_condition{{0., 0., 1.},
                                                              engine};

    return 0;
}
