#ifndef UUID_3E3C9699_1213_4327_9B3C_55CA78BC94E4
#define UUID_3E3C9699_1213_4327_9B3C_55CA78BC94E4

#include <tuple>
#include <type_traits>
#include <utility>

namespace Detail {

template <typename F, typename Tuple, size_t... S>
decltype(auto) apply_tuple_impl(F&& fn, Tuple&& t, std::index_sequence<S...>) {
	return std::forward<F>(fn)(std::get<S>(std::forward<Tuple>(t))...);
}
template <typename F, typename Tuple>
decltype(auto) apply_from_tuple(F&& fn, Tuple&& t) {
	std::size_t constexpr tSize =
	    std::tuple_size<typename std::remove_reference<Tuple>::type>::value;
	return apply_tuple_impl(std::forward<F>(fn), std::forward<Tuple>(t),
				std::make_index_sequence<tSize>());
}

}  // namespace Detail

using Detail::apply_from_tuple;

template <typename Func>
struct arg_tuple;

template <typename R, typename... T>
struct arg_tuple<R(T...)> {
	using type = std::tuple<std::decay_t<T>...>;
};

template <typename R, typename... T>
struct arg_tuple<R (&)(T...)> {
	using type = std::tuple<std::decay_t<T>...>;
};

template <typename R, typename... T>
struct arg_tuple<R (*)(T...)> {
	using type = std::tuple<std::decay_t<T>...>;
};

#endif  //  UUID_3E3C9699_1213_4327_9B3C_55CA78BC94E4
