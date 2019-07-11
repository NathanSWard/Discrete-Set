#pragma once

#include <type_traits>
#include <variant>

template <class Haystack, class Needle>
struct contains;

template <class Car, class... Cdr, class Needle>
struct contains<std::variant<Car, Cdr...>, Needle> : contains<std::variant<Cdr...>, Needle>
{};

template <class... Cdr, class Needle>
struct contains<std::variant<Needle, Cdr...>, Needle> : std::true_type
{};

template <class Needle>
struct contains<std::variant<>, Needle> : std::false_type
{};

template <class Out, class In>
struct filter;

template <class... Out, class InCar, class... InCdr>
struct filter<std::variant<Out...>, std::variant<InCar, InCdr...>>
{
  using type = typename std::conditional<
    contains<std::variant<Out...>, InCar>::value
    , typename filter<std::variant<Out...>, std::variant<InCdr...>>::type
    , typename filter<std::variant<Out..., InCar>, std::variant<InCdr...>>::type
  >::type;
};

template <class Out>
struct filter<Out, std::variant<>>
{
  using type = Out;
};

template <class T>
using remove_duplicates = typename filter<std::variant<>, T>::type;

//==================================================================================

namespace std {

template< class T >
struct remove_cvref {
    typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};

template< class T >
using remove_cvref_t = typename remove_cvref<T>::type;

template<class T>
struct is_unbounded_array: std::false_type {};
 
template<class T>
struct is_unbounded_array<T[]> : std::true_type {};

template< class T >
inline constexpr bool is_unbounded_array_v = is_unbounded_array<T>::value;

template<class T>
struct is_bounded_array: std::false_type {};
 
template<class T, std::size_t N>
struct is_bounded_array<T[N]> : std::true_type {};

template< class T >
inline constexpr bool is_bounded_array_v = is_bounded_array<T>::value;

} // namespace std

template<template<typename T> typename F, typename V, std::size_t ...I>
constexpr void _variant_apply_impl(std::index_sequence<I...>) {
    (F<std::variant_alternative_t<I, V>{}(), ...);
}

template<template<typename T> typename F, typename V>
constexpr void varaint_apply() {
    _variant_apply_impl<F, V>(std::make_index_sequence<std::variant_size_v<V>>);
}

namespace unknown_type
{
  struct is_equal
  {
      template<typename T, typename U>
      constexpr auto operator()(T&& t, U&& u)
      -> std::enable_if_t<std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<U>>, bool> {
        return t == u;
      }

      template<typename T, typename U>
      constexpr auto operator()(T&&, U&&) 
      -> std::enable_if_t<!std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<U>>, bool> {
          return false;
      }
  };

  template<typename ...T>
  struct set_ref
  {
    using container_t = typename Set<T...>::container_t;
    
    constexpr explicit set_ref(container_t const& set)
      :_set{set}
    {
    }

    template<typename U>
    constexpr auto operator()(U&&) -> std::enable_if_t<!(std::is_same_v<std::remove_cvref_t<U>, T> || ...), bool> {
        return false;
    }

    template<typename U>
    constexpr auto operator()(U&& u) -> std::enable_if_t<(std::is_same_v<std::remove_cvref_t<U>, T> || ...), bool> { 
      return (_set.find(u) != _set.end());
    }

    private:
      container_t const& _set;
  };
} // namespace unknown_type



 