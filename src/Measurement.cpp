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
#include "Rotation.h"

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
		   std::unique_ptr<SOCModel::Base>&& soc_model,
		   std::unique_ptr<Output::Base>&& output)
    : spin_count(spin_count),
      duration(duration),
      time_step(time_step),
      t0(t0),
      initial_condition(std::move(initial_condition)),
      scattering_model(std::move(scattering_model)),
      magnetic_field(std::move(magnetic_field)),
      soc_model(std::move(soc_model)),
      output(std::move(output)) {
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
	    mapat(node,"soc_model").as<std::unique_ptr<SOCModel::Base>>(),
	    mapat(node,"output").as<std::unique_ptr<Output::Base>>());
}

void Ensamble::run() {
	auto size = (size_t)(duration / time_step);
	auto result = arma::mat(4, size, arma::fill::zeros);

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

	output->write_header({"t", "s_x", "s_y", "s_z"});

	for (size_t k = 0; k < size; k++) {
		output->write_record({
		  k*time_step,
		  result(1,k) / spin_count,
		  result(2,k) / spin_count,
		  result(3,k) / spin_count
		});
	}
}

void Ensamble::run(unsigned int) {
	this->run();  // TODO multithread
}

constexpr char EchoDecay::type_name[];

EchoDecay::EchoDecay(
    unsigned int spin_count, double duration, double time_step, double t0,
    std::unique_ptr<InitialCondition::Base>&& initial_condition,
    std::unique_ptr<ScatteringModel::Base>&& scattering_model,
    std::unique_ptr<SOCModel::Base>&& soc_model,
    std::unique_ptr<Output::Base>&& output)
    : spin_count(spin_count),
      duration(duration),
      time_step(time_step),
      t0(t0),
      initial_condition(std::move(initial_condition)),
      scattering_model(std::move(scattering_model)),
      soc_model(std::move(soc_model)),
      output(std::move(output)) {
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

std::unique_ptr<Base> EchoDecay::Factory::create_from_YAML(
    const YAML::Node& node) {
	using Misc::mapat;
	return std::make_unique<EchoDecay>(
	    mapat(node, "spin_count").as<unsigned int>(),
	    mapat(node, "duration").as<double>(),
	    mapat(node, "time_step").as<double>(),
	    mapat(node, "t0").as<double>(),
	    mapat(node, "initial_condition")
		.as<std::unique_ptr<InitialCondition::Base>>(),
	    mapat(node, "scattering_model")
		.as<std::unique_ptr<ScatteringModel::Base>>(),
	    mapat(node, "soc_model").as<std::unique_ptr<SOCModel::Base>>(),
	    mapat(node, "output").as<std::unique_ptr<Output::Base>>());
}

void EchoDecay::run() {
	const auto size = (size_t)(duration / time_step);
	auto result = arma::mat(3, size, arma::fill::zeros);

	for (size_t k = 0; k < spin_count; ++k) {
		auto rotations = std::vector<Rotation::rotation>(2 * size);

		const auto initial_state = initial_condition->roll();
		auto last_k = initial_state.k;
		auto last_step = Rotation::rotation(
		    arma::vec3(soc_model->omega(last_k) * time_step));
		rotations[0] = Rotation::rotation::identity();

		auto next = scattering_model->NextEvent(last_k);

		// Populate rotations
		const auto half_step = time_step / 2.;
		for (size_t i = 1; i < 2 * size; ++i) {
			if (t0 + i * half_step > next.t) {
				rotations[i] =
				    rotations[i - 1]
				    * Rotation::rotation(arma::vec3(
					soc_model->omega(last_k)
					* (next.t - (t0 + (i - 1) * half_step))
				      ))
				    * Rotation::rotation(arma::vec3(
					soc_model->omega(next.k)
					* (t0 + i * half_step - next.t)
				      ));
				last_k = next.k;
				last_step = Rotation::rotation(arma::vec3(
				    soc_model->omega(last_k) * half_step));
				next = scattering_model->NextEvent(last_k);

			} else {
				rotations[i] = rotations[i - 1] * last_step;
			}
		}

		// Accumulate result spin
		const auto first_spin = initial_state.spin;
		for (size_t i = 0; i < size; ++i) {
			const auto rot_pulse = rotations[i];
			const auto rot_echo = rotations[2 * i];

			// Right-to-left multiplication is more performant
			const auto spin =
			    rot_pulse
			    * (rot_echo.inverse()
			       * (rot_pulse * first_spin));
			result.col(i) += spin;
		}
	}
	output->write_header({"t", "s_x", "s_y", "s_z"});

	for (size_t k = 0; k < size; k++) {
		output->write_record({k * time_step,
				      result(0, k) / spin_count,
				      result(1, k) / spin_count,
				      result(2, k) / spin_count});
	}
}

void EchoDecay::run(unsigned int) {
	this->run();  // TODO multithread
}

}  // namespace Measurement
