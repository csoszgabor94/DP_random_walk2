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
	static const auto& get_factories() { return factories(); }
	virtual arma::vec3 omega(const arma::vec3& k) const = 0;
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
	arma::vec3 omega(const arma::vec3& k) const override;
};

class Dresselhaus : public Base {
       private:
	double o;

       public:
	static constexpr char type_name[] = "Dresselhaus";
	class Factory : public Base::Factory {
	       public:
		virtual std::unique_ptr<Base> create_from_YAML(
		    const YAML::Node&) override;
	};
	Dresselhaus() = delete;
	Dresselhaus(double omega) : o(omega) {}
	arma::vec3 omega(const arma::vec3& k) const override;
};

class Zeeman : public Base {
       private:
	arma::vec3 bfield;
	std::unique_ptr<Base> base_model;

       public:
	static constexpr char type_name[] = "Zeeman";
	class Factory : public Base::Factory {
	       public:
		virtual std::unique_ptr<Base> create_from_YAML(
		    const YAML::Node&) override;
	};
	Zeeman() = delete;
	Zeeman(const arma::vec3& bfield, std::unique_ptr<Base> base_model)
	    : bfield(bfield), base_model(std::move(base_model)) {}
	arma::vec3 omega(const arma::vec3& k) const override;
};

}  // namespace SOCModel
template class RegisterSubclass<SOCModel::Base, SOCModel::Isotropic3D>;
template class RegisterSubclass<SOCModel::Base, SOCModel::Dresselhaus>;
template class RegisterSubclass<SOCModel::Base, SOCModel::Zeeman>;

namespace YAML {

template <>
std::unique_ptr<SOCModel::Base> Node::as() const;

}  // namespace YAML

#endif  // SOC_MODEL_H
