#ifndef UUID_E8AD5D93_1918_4BD0_A45E_5A81C5F1D8F8
#define UUID_E8AD5D93_1918_4BD0_A45E_5A81C5F1D8F8

#include <array>
#include <cstddef>
#include <utility>

template <typename T, std::size_t N>
constexpr std::size_t arraysize(const T (&)[N]) noexcept {
	return N;
}

template <typename T, std::size_t N>
constexpr std::size_t arraysize(const std::array<T, N>&) noexcept {
	return N;
}

template <typename T, typename... Ts>
constexpr std::array<T, sizeof...(Ts)> make_array(Ts&&... args) {
	return {std::forward<Ts>(args)...};
}

#endif  // UUID_E8AD5D93_1918_4BD0_A45E_5A81C5F1D8F8
