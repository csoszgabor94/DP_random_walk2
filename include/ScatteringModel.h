#ifndef SCATTERING_MODEL_H
#define SCATTERING_MODEL_H

#include <map>
#include <memory>
#include <string>

#include <yaml-cpp/yaml.h>
#include <armadillo>

#include "RegisterSubclass.h"

namespace ScatteringModel {

struct Event {
	arma::vec3 k;
	double t;
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
	virtual const Event NextEvent(const arma::vec3& k0) = 0;
	virtual ~Base() {}
};

class Isotropic3D : public Base {
       private:
	double scattering_rate;

       public:
	static constexpr char type_name[] = "Isotropic3D";
	class Factory : public Base::Factory {
	       public:
		virtual std::unique_ptr<Base> create_from_YAML(
		    const YAML::Node&) override;
	};
	Isotropic3D() = delete;
	Isotropic3D(double scattering_rate)
	    : scattering_rate(scattering_rate){};
	const Event NextEvent(const arma::vec3& k0) override;
};
}  // namespace ScatteringModel

#endif  // SCATTERING_MODEL_H
