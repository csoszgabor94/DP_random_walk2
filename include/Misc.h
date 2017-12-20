#ifndef MISC_H
#define MISC_H

#include <yaml-cpp/yaml.h>
#include <armadillo>
#include <iostream>
#include <string>

#include "globals.h"

namespace arma_types {
#include <armadillo_bits/typedef_elem.hpp>
}  // namespace arma_types

namespace Misc {
arma::vec3 Rotate(const arma::vec3& v0, const arma::vec3& phi);

template<typename... Ts> struct make_void { typedef void type;};
template<typename... Ts> using void_t = typename make_void<Ts...>::type;

template <typename, typename = void>
struct is_printable : std::false_type {};

template <typename T>
struct is_printable<
  T,
  void_t<
    decltype(
      std::declval<std::ostream>() << std::declval<const T>()
    )
  >
> : std::true_type {};

template <typename T>
std::enable_if_t<
  is_printable<T>::value,
  std::string
> to_string(const T& t) {
	std::ostringstream buf;
	buf << t;
	return buf.str();
}

template <typename T>
std::enable_if_t<
  !is_printable<T>::value,
  std::string
> to_string(const T&) {
	return {};
}


// Checked map key access
template <typename Key>
YAML::Node mapat(const YAML::Node& node, const Key& key) {
	if (!node.IsMap()) {
		throw YAML::RepresentationException(node.Mark(),
						    "Expected Map");
	}
	const auto value = node[key];
	if (!value) {
		throw YAML::RepresentationException(
		    node.Mark(), "Key \"" + to_string(key) + "\" not found.");
	}
	if (value.Tag() == "!option") {
		std::string option_arg;
		const auto& option_name = value.template as<std::string>();
		try {
			option_arg = globals::options.at(option_name);
		} catch (const std::out_of_range&) {
			throw std::runtime_error("Option \"" + option_name +
						 "\" not found.");
		}

		try {
			const auto new_node = YAML::Load(option_arg);
			return new_node;
		} catch (YAML::ParserException& e) {
			e.msg +=
			    ", parsing error in option \"" + option_name + '\"';
			throw YAML::ParserException(e.mark, e.msg);
		}
	}
	return value;
}

}  // namespace Misc

namespace YAML {

template <arma_types::uword size>
struct convert<arma::Col<double>::fixed<size>> {
	static Node encode(const arma::Col<double>::fixed<size>& rhs) {
		Node node;
		for (size_t k = 0; k < size; k++) {
			node.push_back(rhs[k]);
		}
		return node;
	}

	static bool decode(const Node& node,
			   arma::Col<double>::fixed<size>& rhs) {
		if (!node.IsSequence() || node.size() != size) {
			return false;
		}

		for (size_t k = 0; k < size; k++) {
			auto value = node[k];
			while (value.Tag() == "!option") {
				std::string option_arg;
				const auto& option_name = value.template as<std::string>();
				try {
					option_arg = globals::options.at(option_name);
				} catch (const std::out_of_range&) {
					throw std::runtime_error("Option \"" + option_name +
								 "\" not found.");
				}

				try {
					value = YAML::Load(option_arg);
				} catch (YAML::ParserException& e) {
					e.msg +=
					    ", parsing error in option \"" + option_name + '\"';
					throw YAML::ParserException(e.mark, e.msg);
				}
			}
			rhs[k] = value.as<double>();
		}
		return true;
	}
};
}  // namespace YAML

#endif  // MISC_H
