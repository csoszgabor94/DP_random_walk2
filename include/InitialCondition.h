#ifndef INITIAL_CONDITION_H
#define INITIAL_CONDITION_H

#include <map>
#include <memory>
#include <string>

#include <yaml-cpp/yaml.h>
#include <armadillo>

#include "RegisterSubclass.h"

namespace InitialCondition {

struct State {
	arma::vec3 k;
	arma::vec3 spin;
};

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
	virtual State roll() = 0;
	virtual ~Base() {}
};

template <typename T>
class Subclass_policy : public Base, private T {
       public:
	using base_t = Base;
	explicit Subclass_policy(const T& t) : T(t) {}
	explicit Subclass_policy(T&& t) : T(std::move(t)) {}
	State roll() override {
		return T::roll();
	}
};

template <typename T>
using Subclass = PolyphormicSubclass<T, Subclass_policy>;

class Isotropic3D {
       public:
	State roll();

	static constexpr const auto& name = "Isotropic3D";
	static constexpr const auto& keywords = make_array<const char*>();
	static auto factory() { return Isotropic3D{}; }
};

class Polarized3D {
       private:
	arma::vec3 spin;

       public:
	Polarized3D(arma::vec3 spin) : spin(spin) {}
	State roll();

	static constexpr const auto& name = "Polarized3D";
	static constexpr const auto& keywords = make_array<const char*>("spin");
	static auto factory(const arma::vec3& spin) {
		return Polarized3D(spin);
	}
};

}  // namespace InitialCondition

namespace YAML {

template <>
std::unique_ptr<InitialCondition::Base> Node::as() const;

}  // namespace YAML

#endif  // INITIAL_CONDITION_H
