#include <memory>
#include <stdexcept>

#include <yaml-cpp/yaml.h>
#include <armadillo>

#include "InitialCondition.h"
#include "MagneticField.h"
#include "Measurement.h"
#include "SOCModel.h"
#include "ScatteringModel.h"
#include "Misc.h"

namespace YAML {

template <>
std::unique_ptr<Measurement::Base> Node::as() const {
	auto type_name = (*this)["type"].as<std::string>();
	return Measurement::Base::get_factories()
	    .at(type_name)
	    ->create_from_YAML(*this);
}

}  // namespace YAML

namespace Measurement {

constexpr char Ensamble::type_name[];

Ensamble::Ensamble(unsigned int spin_count, double duration, double time_step,
		   double t0,
		   std::unique_ptr<InitialCondition::Base>&& initial_condition,
		   std::unique_ptr<ScatteringModel::Base>&& scattering_model,
		   std::unique_ptr<MagneticField::Base>&& magnetic_field,
		   std::unique_ptr<SOCModel::Base>&& soc_model)
    : spin_count(spin_count),
      duration(duration),
      time_step(time_step),
      t0(t0),
      initial_condition(std::move(initial_condition)),
      scattering_model(std::move(scattering_model)),
      magnetic_field(std::move(magnetic_field)),
      soc_model(std::move(soc_model)) {
	if (duration <= 0) {
		throw std::invalid_argument{"\"duration\" must be positive."};
	}
	if (time_step <= 0) {
		throw std::invalid_argument{"\"time_step\" must be positive."};
	}
	if (time_step > duration) {
		throw std::invalid_argument{
		    "\"time_step\" must not be larger than \"duration\"."};
	}
}

std::unique_ptr<Base> Ensamble::Factory::create_from_YAML(
    const YAML::Node& node) {
	using Misc::mapat;
	return std::make_unique<Ensamble>(
	    mapat(node,"spin_count").as<unsigned int>(),
	    mapat(node,"duration").as<double>(),
	    mapat(node,"time_step").as<double>(),
	    mapat(node,"t0").as<double>(),
	    mapat(node,"initial_condition")
		.as<std::unique_ptr<InitialCondition::Base>>(),
	    mapat(node,"scattering_model")
		.as<std::unique_ptr<ScatteringModel::Base>>(),
	    mapat(node,"magnetic_field").as<std::unique_ptr<MagneticField::Base>>(),
	    mapat(node,"soc_model").as<std::unique_ptr<SOCModel::Base>>());
}

arma::mat Ensamble::run() {
	auto size = (size_t)(duration / time_step);
	auto result = arma::mat(4, size);

	for (size_t k = 0; k < spin_count; k++) {
		auto times = std::vector<double>{};
		auto spins = std::vector<arma::vec3>{};
		auto kvecs = std::vector<arma::vec3>{};

		// initial condition
		{
			auto initial_state = initial_condition->roll();

			times.push_back(t0);
			spins.push_back(initial_state.spin);
			kvecs.push_back(initial_state.k);
		}

		// steps
		{
			auto last_time = t0;
			while (last_time < t0 + duration) {
				auto t = times.back();
				auto k = kvecs.back();
				auto s = spins.back();

				auto next_scattering =
				    scattering_model->NextEvent(k);
				auto next_t = t + next_scattering.t;
				auto next_k = next_scattering.k;
				auto next_s = magnetic_field->advance(
				    s, t, next_t, soc_model->omega(k));
				last_time = next_t;

				times.push_back(next_t);
				spins.push_back(next_s);
				kvecs.push_back(next_k);
			}
		}

		// take samples
		{
			size_t ind = 0;
			double next_t = times[1];
			for (size_t i = 0; i < size; i++) {
				while (t0 + i * time_step > next_t) {
					ind++;
					next_t = times[ind + 1];
				}
				auto s = spins[ind];
				auto k = kvecs[ind];

				auto st = magnetic_field->advance(
				    s, times[ind], t0 + i * time_step,
				    soc_model->omega(k));
				result.col(i) +=
				    arma::join_vert(arma::vec{0.}, st);
			}
		}
	}

	// average and add times
	for (size_t k = 0; k < size; k++) {
		result.col(k) /= spin_count;
		result(0, k) = k * time_step;
	}

	return result;
}

arma::mat Ensamble::run(unsigned int) {
	return this->run();  // TODO multithread
}

}  // namespace Measurement
