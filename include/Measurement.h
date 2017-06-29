#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <map>
#include <memory>
#include <string>

#include <yaml-cpp/yaml.h>
#include <armadillo>

#include "InitialCondition.h"
#include "MagneticField.h"
#include "RegisterSubclass.h"
#include "SOCModel.h"
#include "ScatteringModel.h"

namespace Measurement {

class Base {
       public:
	class Factory {
	       public:
		virtual std::unique_ptr<Base> create_from_YAML(
		    const YAML::Node&) = 0;
		virtual ~Factory() {}
	};

       private:
	static auto& factories() {
		static std::map<std::string, std::unique_ptr<Factory>> f;
		return f;
	};
	template <typename Base, typename Child>
	friend RegisterSubclass<Base, Child>::Register::Register();

       public:
	static const auto& get_factories() { return factories(); }
	virtual arma::mat run() = 0;
	virtual arma::mat run(unsigned int threads) = 0;
	virtual ~Base() {}
};

class Ensamble : public Base {
       private:
	unsigned int spin_count;
	double duration;
	double time_step;
	std::unique_ptr<InitialCondition::Base> initial_condition;
	std::unique_ptr<ScatteringModel::Base> scattering_model;
	std::unique_ptr<MagneticField::Base> magnetic_field;
	std::unique_ptr<SOCModel::Base> soc_model;

       public:
	static constexpr char type_name[] = "Ensamble";
	class Factory : public Base::Factory {
	       public:
		virtual std::unique_ptr<Base> create_from_YAML(
		    const YAML::Node&) override;
	};
	Ensamble() = delete;
	Ensamble(unsigned int spin_count, double duration, double time_step,
		 std::unique_ptr<InitialCondition::Base>&& initial_condition,
		 std::unique_ptr<ScatteringModel::Base>&& scattering_model,
		 std::unique_ptr<MagneticField::Base>&& magnetic_field,
		 std::unique_ptr<SOCModel::Base>&& soc_model)
	    : spin_count(spin_count),
	      duration(duration),
	      time_step(time_step),
	      initial_condition(std::move(initial_condition)),
	      scattering_model(std::move(scattering_model)),
	      magnetic_field(std::move(magnetic_field)),
	      soc_model(std::move(soc_model)) {}

	arma::mat run() override;
	arma::mat run(unsigned int threads) override;
};

}  // namespace Measurement
template class RegisterSubclass<Measurement::Base, Measurement::Ensamble>;

namespace YAML {

template <>
std::unique_ptr<Measurement::Base> Node::as() const;

}  // namespace YAML

#endif
