#include <yaml-cpp/yaml.h>
#include <boost/random.hpp>
#include <memory>
#include "InitialCondition.h"
#include "MagneticField.h"
#include "SOCModel.h"
#include "ScatteringModel.h"

int main() {
	using polymorphic_step = MagneticField::Subclass<MagneticField::Step>;
	const auto ext_magnetic_field =
	    polymorphic_step(MagneticField::Step({0., 0., 1.}, 0.));

	std::unique_ptr<MagneticField::Base> ext_magnetic_field2 =
	    polymorphic_step::Factory{}.create_from_YAML(
		YAML::LoadFile("MagneticFieldStep.yaml"));
	SOCModel::Isotropic3D soc_model = SOCModel::Isotropic3D{0.2};
	ScatteringModel::Isotropic3D scattering_model{1.};
	InitialCondition::Polarized3D initial_condition{{0., 0., 1.}};

	return 0;
}
