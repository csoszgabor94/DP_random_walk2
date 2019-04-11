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
		virtual ~Base() {}
};

template <typename T>
class Subclass_policy : public Base, private T {
       public:
	using base_t = Base;
	explicit Subclass_policy(const T& t) : T(t) {}
	explicit Subclass_policy(T&& t) : T(std::move(t)) {}
	void run() override {
		T::run();
	}
};

template <typename T>
using Subclass = PolyphormicSubclass<T, Subclass_policy>;

class Ensamble {
       private:
	unsigned int spin_count;
	double duration;
	double time_step;
	double t0;
	unsigned int threads;
	std::unique_ptr<InitialCondition::Base> initial_condition;
	std::unique_ptr<ScatteringModel::Base> scattering_model;
	std::unique_ptr<MagneticField::Base> magnetic_field;
	std::unique_ptr<SOCModel::Base> soc_model;
	std::unique_ptr<Output::Base> output;
	arma::mat do_run(unsigned int thread_nr);

       public:
	Ensamble(unsigned int spin_count, double duration, double time_step, double t0, unsigned int threads,
		 std::unique_ptr<InitialCondition::Base>&& initial_condition,
		 std::unique_ptr<ScatteringModel::Base>&& scattering_model,
		 std::unique_ptr<MagneticField::Base>&& magnetic_field,
		 std::unique_ptr<SOCModel::Base>&& soc_model,
		 std::unique_ptr<Output::Base>&& output);

	void run();

	static constexpr const auto &name = "Ensamble";
	static constexpr const auto &keywords = make_array<const char*>(
		"spin_count",
		"duration",
		"time_step",
		"t0",
		"threads",
		"initial_condition",
		"scattering_model",
		"magnetic_field",
		"soc_model",
		"output"
		);
	static auto factory(unsigned int spin_count, double duration, double time_step, double t0, unsigned int threads,
		 std::unique_ptr<InitialCondition::Base>&& initial_condition,
		 std::unique_ptr<ScatteringModel::Base>&& scattering_model,
		 std::unique_ptr<MagneticField::Base>&& magnetic_field,
		 std::unique_ptr<SOCModel::Base>&& soc_model,
		 std::unique_ptr<Output::Base>&& output){
		return Ensamble(
		    spin_count,
		    duration,
		    time_step,
		    t0,
		    threads,
		    std::move(initial_condition),
		    std::move(scattering_model),
		    std::move(magnetic_field),
		    std::move(soc_model),
		    std::move(output)
		    );
	}
};

class EchoDecay {
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
	EchoDecay(unsigned int spin_count, double duration, double time_step,
		  double t0,
		  std::unique_ptr<InitialCondition::Base>&& initial_condition,
		  std::unique_ptr<ScatteringModel::Base>&& scattering_model,
		  std::unique_ptr<SOCModel::Base>&& soc_model,
		  std::unique_ptr<Output::Base>&& output);

	void run();
	void run(unsigned int threads);

	static constexpr const auto &name = "EchoDecay";
	static constexpr const auto &keywords = make_array<const char*>(
		"spin_count",
		"duration",
		"time_step",
		"t0",
		"initial_condition",
		"scattering_model",
		"soc_model",
		"output"
		);
	static auto factory(unsigned int spin_count, double duration, double time_step, double t0,
		 std::unique_ptr<InitialCondition::Base>&& initial_condition,
		 std::unique_ptr<ScatteringModel::Base>&& scattering_model,
		 std::unique_ptr<SOCModel::Base>&& soc_model,
		 std::unique_ptr<Output::Base>&& output){
		return EchoDecay(
		    spin_count,
		    duration,
		    time_step,
		    t0,
		    std::move(initial_condition),
		    std::move(scattering_model),
		    std::move(soc_model),
		    std::move(output)
		    );
	}
};

class EchoDecayTest {
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
	EchoDecayTest(unsigned int spin_count, double duration, double time_step,
		  double t0,
		  std::unique_ptr<InitialCondition::Base>&& initial_condition,
		  std::unique_ptr<ScatteringModel::Base>&& scattering_model,
		  std::unique_ptr<SOCModel::Base>&& soc_model,
		  std::unique_ptr<Output::Base>&& output);

	void run();
	void run(unsigned int threads);

	static constexpr const auto &name = "EchoDecayTest";
	static constexpr const auto &keywords = make_array<const char*>(
		"spin_count",
		"duration",
		"time_step",
		"t0",
		"initial_condition",
		"scattering_model",
		"soc_model",
		"output"
		);
	static auto factory(unsigned int spin_count, double duration, double time_step, double t0,
		 std::unique_ptr<InitialCondition::Base>&& initial_condition,
		 std::unique_ptr<ScatteringModel::Base>&& scattering_model,
		 std::unique_ptr<SOCModel::Base>&& soc_model,
		 std::unique_ptr<Output::Base>&& output){
		return EchoDecayTest(
		    spin_count,
		    duration,
		    time_step,
		    t0,
		    std::move(initial_condition),
		    std::move(scattering_model),
		    std::move(soc_model),
		    std::move(output)
		    );
	}
};

}  // namespace Measurement

namespace YAML {

template <>
std::unique_ptr<Measurement::Base> Node::as() const;

}  // namespace YAML

#endif
