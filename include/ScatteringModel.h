#ifndef SCATTERING_MODEL_H
#define SCATTERING_MODEL_H

#include <map>
#include <memory>
#include <string>

#include <yaml-cpp/yaml.h>
#include <armadillo>

#include "RegisterSubclass.h"
#include "arraysize.h"
#include "yaml_utils.h"
#include "tuple_apply.h"

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
	static const auto& get_factories() { return factories(); }
	virtual Event NextEvent(const arma::vec3& k0) = 0;
	virtual ~Base() {}
};

template <typename T>
class Subclass_policy : public Base, private T {
       public:
	using base_t = Base;
	explicit Subclass_policy(const T& t) : T(t) {}
	explicit Subclass_policy(T&& t) : T(std::move(t)) {}
	Event NextEvent(const arma::vec3& k0) override {
		return T::NextEvent(k0);
	}
};

template <typename T>
using Subclass = PolyphormicSubclass<T, Subclass_policy>;

class Isotropic3D {
       private:
	double scattering_rate;

       public:
	Isotropic3D(double scattering_rate)
	    : scattering_rate(scattering_rate) {}
	Event NextEvent(const arma::vec3& k0);

	static constexpr const auto& name = "Isotropic3D";
	static constexpr const auto& keywords =
	    make_array<const char*>("scattering_rate");
	static auto factory(double scattering_rate) {
		return Isotropic3D(scattering_rate);
	}
};

}  // namespace ScatteringModel

namespace YAML {

template <>
std::unique_ptr<ScatteringModel::Base> Node::as() const;

}  // namespace YAML

#endif  // SCATTERING_MODEL_H
