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
	virtual State roll() = 0;
	virtual ~Base() {}
};

class Isotropic3D : public Base {
       public:
	static constexpr char type_name[] = "Isotropic3D";
	class Factory : public Base::Factory {
	       public:
		virtual std::unique_ptr<Base> create_from_YAML(
		    const YAML::Node&) override;
	};
	State roll() override;
};

class Polarized3D : public Base {
       private:
	arma::vec3 spin;

       public:
	static constexpr char type_name[] = "Polarized3D";
	class Factory : public Base::Factory {
	       public:
		virtual std::unique_ptr<Base> create_from_YAML(
		    const YAML::Node&) override;
	};
	Polarized3D() = delete;
	Polarized3D(arma::vec3 spin) : spin(spin) {}
	State roll() override;
};
}  // namespace InitialCondition
template class RegisterSubclass<InitialCondition::Base,
				InitialCondition::Isotropic3D>;
template class RegisterSubclass<InitialCondition::Base,
				InitialCondition::Polarized3D>;

#endif  // INITIAL_CONDITION_H
