#ifndef SOC_MODEL_H
#define SOC_MODEL_H

#include <map>
#include <memory>
#include <string>

#include <yaml-cpp/yaml.h>
#include <armadillo>

#include "RegisterSubclass.h"

namespace SOCModel {

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
	virtual const arma::vec3 omega(const arma::vec3& k) = 0;
	virtual ~Base() {}
};

class Isotropic3D : public Base {
       private:
	double o;

       public:
	static constexpr char type_name[] = "Isotropic3D";
	class Factory : public Base::Factory {
	       public:
		virtual std::unique_ptr<Base> create_from_YAML(
		    const YAML::Node&) override;
	};
	Isotropic3D() = delete;
	Isotropic3D(double omega) : o(omega) {}
	const arma::vec3 omega(const arma::vec3& k) override;
};
}  // namespace SOCModel
template class RegisterSubclass<SOCModel::Base, SOCModel::Isotropic3D>;

#endif  // SOC_MODEL_H
