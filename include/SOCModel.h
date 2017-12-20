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

template <typename T>
class Subclass_policy : public Base, private T {
       public:
	using base_t = Base;
	explicit Subclass_policy(const T& t) : T(t) {}
	explicit Subclass_policy(T&& t) : T(std::move(t)) {}
	arma::vec3 omega(const arma::vec3& k) const override {
		return T::omega(k);
	}
};

template <typename T>
using Subclass = PolyphormicSubclass<T, Subclass_policy>;

class Isotropic3D {
       private:
	double o;

       public:
	Isotropic3D(double omega) : o(omega) {}
	arma::vec3 omega(const arma::vec3& k) const;

	static constexpr const auto& name = "Isotropic3D";
	static constexpr const auto& keywords =
	    make_array<const char*>("omega");
	static auto factory(double omega) { return Isotropic3D(omega); }
};

class Dresselhaus {
       private:
	double o;

       public:
	Dresselhaus(double omega) : o(omega) {}
	arma::vec3 omega(const arma::vec3& k) const;

	static constexpr const auto& name = "Dresselhaus";
	static constexpr const auto& keywords =
	    make_array<const char*>("omega");
	static auto factory(double omega) { return Dresselhaus(omega); }
};

class Zeeman {
       private:
	arma::vec3 bfield;
	std::unique_ptr<Base> base_model;

       public:
	Zeeman(const arma::vec3& bfield, std::unique_ptr<Base> base_model)
	    : bfield(bfield), base_model(std::move(base_model)) {}
	arma::vec3 omega(const arma::vec3& k) const;

	static constexpr const auto& name = "Zeeman";
	static constexpr const auto& keywords =
	    make_array<const char*>("field", "base_model");
	static auto factory(const arma::vec3& bfield,
			    std::unique_ptr<Base> base_model) {
		return Zeeman(bfield, std::move(base_model));
	}
};

class Stretch {
       private:
	arma::vec3 lambdas;
	std::unique_ptr<Base> base_model;

       public:
	Stretch(const arma::vec3& lambdas, std::unique_ptr<Base> base_model)
	    : lambdas(lambdas), base_model(std::move(base_model)) {}
	arma::vec3 omega(const arma::vec3& k) const;

	static constexpr const auto& name = "Stretch";
	static constexpr const auto& keywords =
	    make_array<const char*>("lambdas", "base_model");
	static auto factory(const arma::vec3& lambdas,
			    std::unique_ptr<Base> base_model) {
		return Stretch(lambdas, std::move(base_model));
	}
};

}  // namespace SOCModel

namespace YAML {

template <>
std::unique_ptr<SOCModel::Base> Node::as() const;

}  // namespace YAML

#endif  // SOC_MODEL_H
