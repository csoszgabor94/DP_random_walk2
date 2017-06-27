#ifndef MISC_H
#define MISC_H

#include <yaml-cpp/yaml.h>
#include <armadillo>

namespace arma_types {
#include <armadillo_bits/typedef_elem.hpp>
}  // namespace arma_types

namespace Misc {
arma::vec3 Rotate(const arma::vec3& v0, const arma::vec3& phi);
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
			rhs[k] = node[0].as<double>();
		}
		return true;
	}
};
}  // namespace YAML

#endif  // MISC_H
