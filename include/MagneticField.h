#ifndef MAGNETIC_FIELD_H
#define MAGNETIC_FIELD_H

#include <map>
#include <memory>
#include <string>

#include <yaml-cpp/yaml.h>
#include <armadillo>

#include "RegisterSubclass.h"

namespace MagneticField {

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
	virtual const arma::vec3 advance(const arma::vec3& s0, double t0,
					 double t,
					 const arma::vec3& bconst) = 0;
	virtual ~Base() {}
};

template <typename T>
class Subclass_policy : public Base, private T {
       public:
	using base_t = Base;
	explicit Subclass_policy(const T& t) : T(t) {}
	explicit Subclass_policy(T&& t) : T(std::move(t)) {}
	const arma::vec3 advance(const arma::vec3& s0, double t0, double t,
				 const arma::vec3& bconst) override {
		return T::advance(s0, t0, t, bconst);
	}
};

template <typename T>
using Subclass = PolyphormicSubclass<T, Subclass_policy>;

class Zero {
       public:
	const arma::vec3 advance(const arma::vec3& s0, double t0, double t,
				 const arma::vec3& bconst);

	static constexpr const auto& name = "Zero";
	static constexpr const auto& keywords = make_array<const char*>();
	static auto factory() { return Zero{}; }
};

class Step {
       private:
	arma::vec3 field;
	double tstep;

       public:
	Step(const arma::vec3& field, double tstep)
	    : field(field), tstep(tstep) {}
	const arma::vec3 advance(const arma::vec3& s0, double t0, double t,
				 const arma::vec3& bconst);

	static constexpr const auto& name = "Step";
	static constexpr const auto& keywords =
	    make_array<const char*>("field", "t0");
	static auto factory(const arma::vec3& field, double t0) {
		return Step(field, t0);
	}
};

class Echo {
       private:
	double tflip;

       public:
	Echo(double tflip) : tflip(tflip) {}
	const arma::vec3 advance(const arma::vec3& s0, double t0, double t,
				 const arma::vec3& bconst);

	static constexpr const auto& name = "Echo";
	static constexpr const auto& keywords =
	    make_array<const char*>("tflip");
	static auto factory(double tflip) { return Echo(tflip); }
};

}  // namespace MagneticField

namespace YAML {

template <>
std::unique_ptr<MagneticField::Base> Node::as() const;

}  // namespace YAML

#endif  // MAGNETIC_FIELD_H
