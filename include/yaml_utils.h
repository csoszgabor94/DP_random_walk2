#ifndef UUID_8BCEBF6F_8C9F_4144_858C_C320A209ACD5
#define UUID_8BCEBF6F_8C9F_4144_858C_C320A209ACD5

#include <array>
#include <tuple>
#include <utility>

#include <yaml-cpp/yaml.h>

namespace Detail {

template <class... Formats, size_t N, size_t... Is>
std::tuple<Formats...> YAML_array_as_tuple_seq(
    std::array<YAML::Node, N> const& arr, std::index_sequence<Is...>) {
	return std::make_tuple(arr[Is].template as<Formats>()...);
}

template <class Tuple, size_t N>
struct YAML_array_as_tuple_struct;

template <class... Formats, size_t N>
struct YAML_array_as_tuple_struct<std::tuple<Formats...>, N> {
	static std::tuple<Formats...> func(
	    std::array<YAML::Node, N> const& arr) {
		return YAML_array_as_tuple_seq<Formats...>(
		    arr, std::make_index_sequence<N>{});
	}
};

}  // namespace Detail

template <class Tuple, size_t N>
Tuple YAML_array_as_tuple(std::array<YAML::Node, N> const& arr) {
	return Detail::YAML_array_as_tuple_struct<Tuple, N>::func(arr);
};

#endif  //  UUID_8BCEBF6F_8C9F_4144_858C_C320A209ACD5
