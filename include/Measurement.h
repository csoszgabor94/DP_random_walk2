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
#include "Output.h"

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
	virtual void run() = 0;
	virtual void run(unsigned int threads) = 0;
	virtual ~Base() {}
};

class Ensamble : public Base {
       private:
	unsigned int spin_count;
	double duration;
	double time_step;
	double t0;
	std::unique_ptr<InitialCondition::Base> initial_condition;
	std::unique_ptr<ScatteringModel::Base> scattering_model;
	std::unique_ptr<MagneticField::Base> magnetic_field;
	std::unique_ptr<SOCModel::Base> soc_model;
	std::unique_ptr<Output::Base> output;

       public:
	static constexpr char type_name[] = "Ensamble";
	class Factory : public Base::Factory {
	       public:
		virtual std::unique_ptr<Base> create_from_YAML(
		    const YAML::Node&) override;
	};
	Ensamble() = delete;
	Ensamble(unsigned int spin_count, double duration, double time_step, double t0,
		 std::unique_ptr<InitialCondition::Base>&& initial_condition,
		 std::unique_ptr<ScatteringModel::Base>&& scattering_model,
		 std::unique_ptr<MagneticField::Base>&& magnetic_field,
		 std::unique_ptr<SOCModel::Base>&& soc_model,
		 std::unique_ptr<Output::Base>&& output);

	void run() override;
	void run(unsigned int threads) override;
};

class EchoDecay : public Base {
       private:
	unsigned int spin_count;
	double duration;
	double time_step;
	double t0;
	std::unique_ptr<InitialCondition::Base> initial_condition;
	std::unique_ptr<ScatteringModel::Base> scattering_model;
	std::unique_ptr<SOCModel::Base> soc_model;
	std::unique_ptr<Output::Base> output;

       public:
	static constexpr char type_name[] = "EchoDecay";
	class Factory : public Base::Factory {
	       public:
		virtual std::unique_ptr<Base> create_from_YAML(
		    const YAML::Node&) override;
	};
	EchoDecay() = delete;
	EchoDecay(unsigned int spin_count, double duration, double time_step,
		  double t0,
		  std::unique_ptr<InitialCondition::Base>&& initial_condition,
		  std::unique_ptr<ScatteringModel::Base>&& scattering_model,
		  std::unique_ptr<SOCModel::Base>&& soc_model,
		  std::unique_ptr<Output::Base>&& output);

	void run() override;
	void run(unsigned int threads) override;
};

}  // namespace Measurement
template class RegisterSubclass<Measurement::Base, Measurement::Ensamble>;
template class RegisterSubclass<Measurement::Base, Measurement::EchoDecay>;

namespace YAML {

template <>
std::unique_ptr<Measurement::Base> Node::as() const;

}  // namespace YAML

#endif
