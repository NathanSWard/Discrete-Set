#pragma once

#include "util.hpp"

#include <algorithm>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <variant>

template <typename ...T> 
class Set {
    static_assert(!(std::is_reference_v<T> || ...), "Set type must not be a reference type.");
    static_assert((std::is_unbounded_array_v<T> || ...), "Set type must not be an unbounded array type.");

    private:
        template <typename... U>
        friend class Set;
        
        using variant_t = remove_duplicates<std::variant<T...>>;
        using container_t = std::unordered_set<variant_t>;

        container_t _set{};
    public:
        using value_types = variant_t;

        constexpr Set() noexcept = default;  
         
        template<typename ...Args>
        explicit constexpr Set(Args&&... args) noexcept
            :_set{({std::in_place_type<std::remove_cvref_t<Args>>
                , std::forward<std::remove_cvref_t<Args>>(args)}, ...)}
        {}

        explicit constexpr Set(Set<T...> const& set) noexcept
            :_set{set._set}
        {}

        explicit constexpr Set(Set<T...> const&& set) noexcept
            :_set{std::move(set._set)}
        {}

        template<typename ...U
            , std::enable_if_t<std::is_same_v<container_t, typename Set<U...>::container_t>>>
        explicit constexpr Set(Set<U...>&& set) noexcept
            :_set{std::forward<container_t>(set._set)}
        {}

        template<typename U, typename ...Args>
        constexpr void emplace(Args&&... args) noexcept {
            static_assert(std::is_bounded_array_v<U>, "Array storage size must be known.");
            _set.emplace(std::in_place_type<std::remove_cvref<U>>, std::forward<Args>(args)...);
        }

        template<typename ...Args>
        constexpr void emplace(Args&&... args) noexcept {
            (emplace<Args>(args), ...);
        }

        template<typename Arg>
        constexpr void erase(Arg&& arg) noexcept {
            if constexpr (std::holds_alternative<std::remove_cvref<Arg>>(varaint_t{}))
                _set.erase(std::forward<Arg>(arg));
        }

        constexpr size_t size() const noexcept { return _set.size(); }
        constexpr bool empty() const noexcept { return _set.empty(); }
        constexpr void clear() noexcept { _set.clear(); }
        constexpr auto &begin() noexcept { return _set.begin(); }
        constexpr auto const &begin() const noexcept { return _set.begin(); }
        constexpr auto &end() noexcept { return _set.end(); }
        constexpr auto const &end() const noexcept { return _set.end(); }

        template<typename ...U>
        constexpr bool operator==(Set<U...> const&) const noexcept;
        template <typename... U>
        constexpr bool operator!=(Set<U...> const & other) const noexcept { return *this != other; }

        template<typename ...U>
        constexpr auto intersection(Set<U...> const&) const noexcept;
        template<typename ...U>
        constexpr auto operator&(Set<U...> const& other) const noexcept {return intersection(other);}
        template<typename ...U>
        constexpr auto& operator&=(Set<U...> const&) const noexcept;
        
        template<typename ...U>
        constexpr auto Union(Set<U...> const&) const noexcept; 
        template<typename ...U>
        constexpr auto operator|(Set<U...> const& other) const noexcept {return Union(other);}
        template<typename ...U>
        constexpr auto& operator|=(Set<U...> const&) const noexcept;

        template<typename ...U>
        constexpr auto difference(Set<U...> const&) const noexcept;
        template<typename ...U>
        constexpr auto operator-(Set<U...> const& other) const noexcept {return difference(other);} 
        template<typename ...U>
        constexpr auto& operator-=(Set<U...> const&) noexcept;
        
        template<typename ...U>
        constexpr auto symmetric_difference(Set<U...> const&) const noexcept;
        template<typename ...U>
        constexpr auto operator^(Set<U...> const& other) const noexcept{return symmetric_difference(other);} 
        template<typename ...U>
        constexpr auto& operator^=(Set<U...> const&) noexcept;     
        
        template<typename ...U>
        constexpr auto cross_product(Set<U...> const&) const noexcept;
        template<typename ...U>
        constexpr auto operator*(Set<U...> const& other) const noexcept {return cross_product(other);}
        template<typename ...U>
        constexpr auto operator*=(Set<U...> const&) const noexcept;

        constexpr auto complement() const noexcept; 
        constexpr auto operator~() const noexcept {return complement();}

        constexpr auto power_set() const noexcept;
        template<typename ...U>
        friend constexpr auto P(Set<U...> const& set) {return set.power_set();}

        constexpr size_t cardinality() const noexcept {return size();}

        template<typename ...U>
        constexpr bool is_subset(Set<U...> const&) const noexcept;
        template<typename ...U>
        constexpr bool operator<=(Set<U...> const& other) {return is_subset(other);}
        
        template<typename ...U>
        constexpr bool is_proper_subset(Set<U...> const&) const noexcept;
        template<typename ...U>
        constexpr bool operator<(Set<U...> const& other) {return is_proper_subset(other);}
        
        template<typename ...U>
        constexpr bool is_superset(Set<U...> const&); 
        template<typename ...U>
        constexpr bool operator>=(Set<U...> const& other) {return is_superset(other);}
        
        template<typename ...U>
        constexpr bool is_proper_superset(Set<U...> const&); 
        template<typename ...U>
        constexpr bool operator>(Set<U...> const& other) {return is_proper_superset(other);} 
       
        template<typename U>
        constexpr bool contains(U&& val) const noexcept {
            if constexpr (std::holds_alternative<std::remove_cvref<U>>(variant_t{})) {
                if (this->empty())
                    return false;
                return this->_set.find(val) != this->_set.end();
            }
            else
                return false;          
        }

        template<typename ...U>
        constexpr bool is_equivalent(Set<U...> const&) const noexcept;
        template<typename ...U>
        constexpr bool is_overlapping(Set<U...> const&) const noexcept;
        template<typename ...U>
        constexpr bool is_disjoint(Set<U...> const&) const noexcept;

        constexpr bool is_finite() const noexcept;
        constexpr bool is_infinite() const noexcept;
        constexpr bool is_singleton() const noexcept { return size() == 0; }
};

template<typename ...T>
template<typename ...U>
constexpr bool Set<T...>::operator==(Set<U...> const& other) const noexcept {        
    if constexpr (std::is_same_v<value_types, typename Set<U...>::value_types>)
        return this->_set == other._set;
    else {
        if (this->size() != other.size())
            return false;
        for (auto const& var1 : this->_set) {
            for (auto const& var2 : other._set) {
                    if (std::visit(unknown_type::is_equal(), var1, var2))
                        break;
                return false;
            }
        }
        return true;
    }
}

template<typename ...T>
template<typename ...U>
constexpr auto Set<T...>::intersection(Set<U...> const& other) const noexcept {
    Set<T..., U...> set;
    if (this->empty() || other.empty())
        return set;
    if constexpr (std::is_same_v<container_t, typename Set<U...>::container_t>) {
        auto const& [small, big] = this->size() < other.size() 
            ? std::tie(this->_set, other._set) : std::tie(other._set, this->_set);
        for (auto&& var : small) {
            std::visit([&](auto &&val) {
                if (big.find(val) != big.end())
                    set.emplace(val);
            }, var);
        }
    }
    else {
        for (auto const& var : this->_set) {
            std::visit([&](auto&& val){
                if (other._set.find(val) != other._set.end())
                    set.emplace(val);
            }, var);
        }
    }
    return set;
}

template<typename ...T>
template<typename ...U>
constexpr auto Set<T...>::Union(Set<U...> const& other) const noexcept {
    if constexpr (std::is_same_v<value_types, typename Set<U...>::value_types>) {
        Set<T..., U...> set{*this};
        for (auto &&elem : other._set)
            set.emplace(elem);
        return set;
    }
    else {
        Set<T..., U...> set;
        for (auto&& var : this->_set)
            std::visit([&](auto&& val) { set.emplace(val); }, var);
        for (auto&& var : other._set)
            std::visit([&](auto&& val) { set.emplace(val); }, var);
        return set;
    }
}

template<typename ...T>
template<typename ...U>
constexpr auto Set<T...>::difference(Set<U...> const&) const noexcept {

}

template<typename ...T>
constexpr auto Set<T...>::complement() const noexcept {

}

template<typename ...T>
template<typename ...U>
constexpr auto Set<T...>::cross_product(Set<U...> const&) const noexcept {

}

template<typename ...T>
constexpr auto Set<T...>::power_set() const noexcept {

}

template<typename ...T>
template<typename ...U>
constexpr bool Set<T...>::is_subset(Set<U...> const&) const noexcept {

}

template<typename ...T>
template<typename ...U>
constexpr bool Set<T...>::is_proper_subset(Set<U...> const&) const noexcept {

}

template<typename ...T>
template<typename ...U>
constexpr bool Set<T...>::is_equivalent(Set<U...> const&) const noexcept {

}

template<typename ...T>
template<typename ...U>
constexpr bool Set<T...>::is_overlapping(Set<U...> const&) const noexcept {

}

template<typename ...T>
template<typename ...U>
constexpr bool Set<T...>::is_disjoint(Set<U...> const&) const noexcept {

}

template<typename ...T>
constexpr bool Set<T...>::is_finite() const noexcept {

}

template<typename ...T>
constexpr bool Set<T...>::is_infinite() const noexcept {

}

template<typename ...T>
constexpr bool Set<T...>::is_singleton() const noexcept {

}


