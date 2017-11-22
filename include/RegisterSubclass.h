#ifndef REGISTER_SUBCLASS_H
#define REGISTER_SUBCLASS_H

#include <memory>
#include <type_traits>

#include "arraysize.h"
#include "yaml_utils.h"
#include "tuple_apply.h"
#include "Misc.h"

template <typename Base, typename Child>
class RegisterSubclass {
       public:
	struct Register {
		Register() {
			using maptype = typename std::remove_reference<decltype(
			    Base::factories())>::type;
			std::unique_ptr<typename Child::Factory> factory =
			    std::make_unique<typename Child::Factory>();
			Base::factories().insert((typename maptype::value_type){
			    Child::type_name, std::move(factory)});
		}
	};

       private:
	static Register reg;

       protected:
	// Force instantiation of reg
	RegisterSubclass() { (void)&reg; }
};

template <typename Base, typename Child>
typename RegisterSubclass<Base, Child>::Register
    RegisterSubclass<Base, Child>::reg{};

template <typename T>
struct reflection_traits {
	static constexpr auto& name = T::name;
	static constexpr auto& keywords = T::keywords;

	static constexpr auto& factory = T::factory;
};

template <typename T, template <typename> class Policy>
class PolyphormicSubclass : public Policy<T> {
       private:
	using Base = typename Policy<T>::base_t;

       public:
	static constexpr const auto& type_name = reflection_traits<T>::name;
	explicit PolyphormicSubclass(const T& t) : Policy<T>(t) {}
	explicit PolyphormicSubclass(T&& t) : Policy<T>(std::move(t)) {}

	struct Factory : public Base::Factory {
		std::unique_ptr<Base> create_from_YAML(
		    const YAML::Node& node) override {
			using trait_t = reflection_traits<T>;
			using tuple_type = typename arg_tuple<decltype(
			    trait_t::factory)>::type;
			static_assert(std::tuple_size<tuple_type>::value ==
					  arraysize(trait_t::keywords),
				      "");
			constexpr auto arg_size =
			    std::tuple_size<tuple_type>::value;
			using array_type = std::array<YAML::Node, arg_size>;

			auto arg_array = array_type();
			for (size_t i = 0; i < arg_size; ++i) {
				arg_array[i] =
				    Misc::mapat(node, trait_t::keywords[i]);
			}

			return std::make_unique<PolyphormicSubclass>(
			    apply_from_tuple(
				trait_t::factory,
				YAML_array_as_tuple<tuple_type>(arg_array)));
		}
	};
};

template <typename T, template <typename> class Policy>
class RegisterSubclass2
    : private RegisterSubclass<typename Policy<T>::base_t,
			       PolyphormicSubclass<T, Policy>> {
       private:
	RegisterSubclass2() {}
};

#endif  // REGISTER_SUBCLASS_H
