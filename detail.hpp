#pragma once

#include <tuple>
#include <type_traits>
#include <variant>

namespace std {

//==================================================================================

template< class T >
struct remove_cvref {
    using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template< class T >
using remove_cvref_t = typename remove_cvref<T>::type;

//==================================================================================

template<class T>
struct is_unbounded_array: std::false_type {};
 
template<class T>
struct is_unbounded_array<T[]> : std::true_type {};

template< class T >
inline constexpr bool is_unbounded_array_v = is_unbounded_array<T>::value;

//==================================================================================

template<class T>
struct is_bounded_array: std::false_type {};
 
template<class T, std::size_t N>
struct is_bounded_array<T[N]> : std::true_type {};

template< class T >
inline constexpr bool is_bounded_array_v = is_bounded_array<T>::value;

//==================================================================================

} // namespace std

//==================================================================================

namespace detail {

//==================================================================================

template <class Haystack, class Needle>
struct contains;

template <class T, class... Ts, class Needle, template<class...> class Container>
struct contains<Container<T, Ts...>, Needle> : contains<Container<Ts...>, Needle>
{};

template <class... Ts, class Needle, template<class...> class Container>
struct contains<Container<Needle, Ts...>, Needle> : std::true_type
{};

template <class Needle, template<class...>class Container>
struct contains<Container<>, Needle> : std::false_type
{};

//=================================================================================

template <class Out, class In>
struct filter;

template <class... Out, class InT, class... InTs, template<class...> class Container>
struct filter<Container<Out...>, Container<InT, InTs...>> {
  using type = typename std::conditional_t<
    contains<Container<Out...>, InT>::value
    , typename filter<Container<Out...>, Container<InTs...>>::type
    , typename filter<Container<Out..., InT>, Container<InTs...>>::type>;
};

template <class Out, template<class...> class Container>
struct filter<Out, Container<>> {
  using type = Out;
};

template <template<class...> class Container, class ...Ts>
using remove_duplicates_t = typename filter<Container<>, Container<Ts...>>::type;

//=================================================================================

template<class Out, class InA, class InB>
struct intersect;

template<class ...Out, class T, class ...Ts, class ...Us, template<class...> class Container>
struct intersect<Container<Out...>, Container<T, Ts...>, Container<Us...>> {
  using type = typename std::conditional_t<
    contains<Container<Us...>, T>::value
    , typename intersect<Container<Out..., T>, Container<Ts...>, Container<Us...>>::type
    , typename intersect<Container<Out...>, Container<Ts...>, Container<Us...>>::type>;
}; 

template <class Out, template<class...> class InA, class InB>
struct intersect<Out, InA<>, InB> {
  using type = Out;
};

template <template<class...> class Container, class InA, class InB>
using set_intersection_t = typename intersect<Container<>, InA, InB>::type;

//==================================================================================

template<class T, class Container>
struct has_type;

template<class T, template<class...> class Container, class... Us>
struct has_type<T, Container<Us...>> : std::disjunction<std::is_same<T, Us>...> 
{}; 

template<class T, class Container>
inline constexpr bool has_type_v = has_type<T, Container>::value;

//==================================================================================

} // namespace detail

//==================================================================================