// set.hpp

#pragma once

#include "detail.hpp"

#include <algorithm>
#include <functional>
#include <iterator>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <variant>

template<class...>
class Set;

struct init_list_t { constexpr explicit init_list_t() noexcept = default; };
inline constexpr init_list_t init_list{};

template <class ...T> 
class Set {
private:
    template <class... Us>
    friend class Set;

    friend struct std::hash<Set<T...>>;
    
    using variant_t = detail::remove_duplicates_t<std::variant, T...>;
    using container_t = std::unordered_set<variant_t>;

    container_t set_{};

public:
    using value_types = variant_t;

    // Constructors
    Set() = default;
    Set(Set const&) = default;
    Set(Set&&) = default;

    template<class ...Us>
    explicit Set(init_list_t, Us&&... us);

    // Modifiers
    template<class U, class ...Args>
    void emplace(std::in_place_type_t<U>, Args&&...);
    template<class ...Args>
    void emplace(Args&&...);
    
    template<class U>
    bool erase(U&&);

    void clear() { set_.clear(); }

    // Lookup
    size_t size() const noexcept { return set_.size(); }
    bool empty() const noexcept { return set_.empty(); }
    template<class U>
    bool contains(U&& val) const noexcept;

    // Iterators
    auto begin() noexcept { return set_.begin(); }
    auto begin() const noexcept {return set_.begin();}
    auto cbegin() const noexcept { return set_.cbegin(); }
    auto end() noexcept { return set_.end(); }
    auto end() const noexcept {return set_.end();}
    auto cend() const noexcept { return set_.cend(); }
    
    // Set Operations
    template<class ...U>
    bool operator==(Set<U...> const&) const noexcept;
    template<class ...U>
    bool operator!=(Set<U...> const& other) const noexcept {return !(*this == other);}

    template<class ...U>
    auto intersection(Set<U...> const&) const;
    template<class ...U>
    auto operator&(Set<U...> const& other) const {return intersection(other);}
    
    template<class ...U>
    auto Union(Set<U...> const&) const; 
    template<class ...U>
    auto operator|(Set<U...> const& other) const {return Union(other);}

    template<class ...U>
    auto difference(Set<U...> const&) const;
    template<class ...U>
    auto operator-(Set<U...> const& other) const {return difference(other);} 

    template<class ...U>
    auto symmetric_difference(Set<U...> const&) const;
    template<class ...U>
    auto operator^(Set<U...> const& other) const {return symmetric_difference(other);}  
    
    template<class ...U>
    auto cross_product(Set<U...> const&) const ;
    template<class ...U>
    auto operator*(Set<U...> const& other) const {return cross_product(other);}

    auto power_set() const;
    template<class ...U>
    friend auto P(Set<U...> const& s) {return s.power_set();};

    size_t cardinality() const noexcept {return size();}

    template<class ...U>
    bool is_subset(Set<U...> const&) const noexcept;
    template<class ...U>
    bool operator<=(Set<U...> const& other) {return is_subset(other);}
    
    template<class ...U>
    bool is_proper_subset(Set<U...> const&) const noexcept;
    template<class ...U>
    bool operator<(Set<U...> const& other) {return is_proper_subset(other);}
    
    template<class ...U>
    bool is_superset(Set<U...> const&) const noexcept; 
    template<class ...U>
    bool operator>=(Set<U...> const& other) const noexcept {return is_superset(other);}
    
    template<class ...U>
    bool is_proper_superset(Set<U...> const&) const noexcept; 
    template<class ...U>
    bool operator>(Set<U...> const& other) const noexcept {return is_proper_superset(other);} 

    template<class ...U>
    bool is_equivalent(Set<U...> const&) const noexcept;

    template<class ...U>
    bool is_overlapping(Set<U...> const&) const noexcept;

    template<class ...U>
    bool is_disjoint(Set<U...> const&) const noexcept;

    bool is_finite() const noexcept;
    bool is_infinite() const noexcept;
    bool is_singleton() const noexcept;
};

template<class... Us>
explicit Set(init_list_t, Us&&... us) -> Set<Us...>;

template<class U, class... Us>
explicit Set(init_list_t, U&& u, Us&&... us) -> Set<U, Us...>;

template<class ...T>
template<class ...Us>
Set<T...>::Set(init_list_t, Us&&... us)  {
    static_assert(sizeof...(Us) > 0, "Set must have at least one type");
    set_.reserve(sizeof...(Us));
    (emplace(std::in_place_type<Us>, std::forward<Us>(us)), ...);
}

template<class ...T>
template<class U, class ...Args>
void Set<T...>::emplace(std::in_place_type_t<U>, Args&&... args) {
    static_assert(std::is_constructible_v<U, Args...>, "Invalid arguments for type U");
    set_.emplace(std::in_place_type<std::remove_cvref_t<U>>, std::forward<Args>(args)...);
}

template<class ...T>
template<class ...Args>
void Set<T...>::emplace(Args&&... args) {
    (emplace(std::in_place_type<Args>, std::forward<Args>(args)), ...);
}

template<class ...T>
template<class U>
bool Set<T...>::erase(U&& u) {
    using arg_t = std::remove_cvref_t<U>;
    if constexpr (std::is_same_v<arg_t, variant_t> || detail::has_type_v<arg_t, variant_t>) {
        if (set_.erase(std::forward<U>(u)) > 0)
            return true;
        else
            return false;    
    }
    else
        return false;
}

template<class ...T>
template<class U>
bool Set<T...>::contains(U&& val) const noexcept {
    using arg_t = std::remove_cvref_t<U>;
    if constexpr (std::is_same_v<arg_t, variant_t> || detail::has_type_v<arg_t, variant_t>)
        return empty() ? false : set_.find(val) != set_.end();
    else
        return false;
}

template<class ...T>
template<class ...U>
bool Set<T...>::operator==(Set<U...> const& b) const noexcept {
    if (size() != b.size())
        return false;        
    if constexpr (std::is_same_v<variant_t, typename Set<U...>::variant_t>)
        return set_ == b.set_;
    else {
        for (auto const& var : set_) {
            if(!std::visit([&](auto&& val){return b.contains(val);}, var))
                return false;
        }
        return true;
    }
}

template<class ...T>
template<class ...U>
auto Set<T...>::intersection(Set<U...> const& other) const {
    detail::set_intersection_t<Set, Set<T...>, Set<U...>> set;
    if (empty() || other.empty())
        return set;
    if constexpr (std::is_same_v<value_types, typename Set<U...>::value_types>) {
        auto const& [small, big] = size() < other.size() 
            ? std::tie(set_, other.set_) : std::tie(other.set_, set_);
        for (auto const& var : small) {
            if (big.find(var) != big.end())
                std::visit([&](auto const& val){set.emplace(val);}, var);
        }
    }
    else {
        for (auto const& var : set_)
            std::visit([&](auto const& val){
                if constexpr (detail::has_type_v<std::remove_cvref_t<decltype(val)>, typename Set<U...>::variant_t>) {
                    if (other.contains(val))
                        set.emplace(val);
                }
            }, var);
    }
    return set;
}

template<class ...T>
template<class ...U>
auto Set<T...>::Union(Set<U...> const& other) const {
    if constexpr (std::is_same_v<value_types, typename Set<U...>::value_types>) {
        Set<T...> set;
        for (auto&& var : *this)
            set.set_.emplace(var);
        for (auto&& var : other)
            set.set_.emplace(var);
        return set;
    }
    else {
        detail::remove_duplicates_t<Set, T..., U...> set;
        for (auto&& var : *this)
            std::visit([&](auto&& val) { set.emplace(val); }, var);
        for (auto&& var : other)
            std::visit([&](auto&& val) { set.emplace(val); }, var);
        return set;
    }
}

template<class ...T>
template<class ...U>
auto Set<T...>::difference(Set<U...> const& other) const {
    Set<T...> set;
    for (auto const& var : set_) {
        std::visit([&](auto&& val){
            if constexpr (detail::has_type_v<std::remove_cvref_t<decltype(val)>, typename Set<U...>::variant_t>) {
                if (!other.contains(val))
                    set.emplace(val);
            }
            else
                set.emplace(val);
        }, var);
    }
    return set;
}

template<class ...T>
template<class ...U>
auto Set<T...>::symmetric_difference(Set<U...> const& other) const {
    detail::remove_duplicates_t<Set, T..., U...> set;
    auto insertElem = [&](auto const& set_a, auto const& set_b) {
        for (auto const& var : set_a) {
            std::visit([&](auto&& val){
                if (!set_b.contains(val))
                    set.emplace(val);
            }, var);
        }
    };
    insertElem(*this, other);
    insertElem(other, *this);
    return set;
}

template<class ...T>
template<class ...U>
auto Set<T...>::cross_product(Set<U...> const& other) const {
    std::vector<detail::remove_duplicates_t<Set, T..., U...>> r_set;
    if (!empty() && !other.empty()) {
        r_set.reserve(size() * other.size());
        for (int i = 0; i < size(); ++i) {
            for (int j = 0; j < other.size(); ++j) {
                detail::remove_duplicates_t<Set, T..., U...> sub_set;
                auto fill_sub_set = [&](auto&& iter, int index) {
                    std::advance(iter, index);
                    std::visit([&](auto const& val){
                        sub_set.emplace(val);
                    }, *iter);
                };
                fill_sub_set(begin(), i);
                fill_sub_set(other.begin(), j);
                r_set.emplace_back(std::move(sub_set));
            }
        }
    }
    return r_set;
}

template<class ...T>
auto Set<T...>::power_set() const {
    std::vector<Set<T...>> r_set;
    r_set.reserve(size() * size());
    size_t const total = 1 << size();
    for (size_t i = 0; i < total; ++i) {
        auto make_sub_set = [&]{
            Set<T...> sub_set;
            for (size_t j = 0; j < size(); ++j) {
                if ((i >> j) & 1) {
                    auto iter = begin();
                    std::advance(iter, j);
                    std::visit([&](auto const& val){
                        sub_set.emplace(val);
                    }, *iter);
                }
            }
            return std::move(sub_set);
        };
        r_set.emplace_back(make_sub_set());
    }
    return r_set;
}

template<class ...T>
template<class ...U>
bool Set<T...>::is_subset(Set<U...> const& other) const noexcept {
    if (size() > other.size())
        return false;
    for (auto const& var : set_) {
        if (!std::visit([&](auto const& val){return other.contains(val) ? true : false;}, var))
            return false;
    }
    return true;
}

template<class ...T>
template<class ...U>
bool Set<T...>::is_proper_subset(Set<U...> const& other) const noexcept {
    if (size() >= other.size())
        return false;
    for (auto const& var : set_) {
        if (!std::visit([&](auto const& val){return other.contains(val) ? true : false;}, var))
            return false;
    }
    return true;
}

template<class ...T>
template<class ...U>
bool Set<T...>::is_superset(Set<U...> const& other) const noexcept {
    return other.is_subset(*this);
}

template<class ...T>
template<class ...U>
bool Set<T...>::is_proper_superset(Set<U...> const& other) const noexcept {
    return other.is_proper_subset(*this);
}

template<class ...T>
template<class ...U>
bool Set<T...>::is_equivalent(Set<U...> const& other) const noexcept {
    return size() == other.size();
}

template<class ...T>
template<class ...U>
bool Set<T...>::is_overlapping(Set<U...> const& other) const noexcept {
    for (auto const& var : set_) {
        if (std::visit([&](auto const& val) {return other.contains(val) ? true : false;}, var)) 
            return true;
    }
    return false;
}

template<class ...T>
template<class ...U>
bool Set<T...>::is_disjoint(Set<U...> const& other) const noexcept {
    for (auto const& var : set_) {
        if (std::visit([&](auto const& val) {return other.contains(val) ? true : false;}, var))
            return false;
    }
    return true;
}

template<class ...T>
bool Set<T...>::is_finite() const noexcept {
    return true;
}

template<class ...T>
bool Set<T...>::is_infinite() const noexcept {
    return false;
}

template<class ...T>
bool Set<T...>::is_singleton() const noexcept {
    return size() == 1;
}

//==================================================================================
// Empty Set Specialization
//==================================================================================
struct empty_iterator {
    inline constexpr static std::variant<std::monostate> var_{};
    constexpr empty_iterator() noexcept {}
    constexpr empty_iterator const& operator++() const noexcept {return *this;}
    constexpr std::variant<std::monostate> const& operator*() const noexcept {return var_;}
    constexpr bool operator!=(empty_iterator const&) const noexcept {return false;}
};

template<>
class Set<> {
private:
    template <class... Us>
    friend class Set;
    friend class empty_iterator;

    inline constexpr static empty_iterator iter_{};

    friend struct std::hash<Set<>>;
public:
    using value_types = std::variant<std::monostate>;

    // Constructors
    constexpr Set() noexcept {}

    // Lookup
    constexpr size_t size() const noexcept { return 0; }
    constexpr bool empty() const noexcept { return true; }
    template<class U>
    constexpr bool contains(U&& val) const noexcept {return false;}

    // Iterators
    auto begin() noexcept { return iter_; }
    auto begin() const noexcept{return iter_;}
    auto cbegin() const noexcept { return iter_; }
    auto end() noexcept { return iter_; }
    auto end() const noexcept {return iter_;}
    auto cend() const noexcept { return iter_; }
    
    // Set Operations
    template<class ...U>
    constexpr bool operator==(Set<U...> const&) const noexcept {
        if constexpr (std::is_same_v<Set<U...>, Set<>>)
            return true;
        else
            return false;
    }

    template<class ...U>
    constexpr bool operator!=(Set<U...> const& other) const noexcept {return !(*this == other);}

    template<class... Args>
    void emplace(Args&&...) const noexcept {}

    template<class ...U>
    constexpr auto intersection(Set<U...> const&) const noexcept {return *this; }
    template<class ...U>
    constexpr auto operator&(Set<U...> const& other) const noexcept {return intersection(other);}
    
    template<class ...U>
    constexpr auto Union(Set<U...> const& other) const noexcept {return other;}
    template<class ...U>
    constexpr auto operator|(Set<U...> const& other) const noexcept {return Union(other);}

    template<class ...U>
    constexpr auto difference(Set<U...> const&) const noexcept {return *this;}
    template<class ...U>
    constexpr auto operator-(Set<U...> const& other) const noexcept {return difference(other);} 

    template<class ...U>
    constexpr auto symmetric_difference(Set<U...> const& other) const noexcept {return other;}
    template<class ...U>
    constexpr auto operator^(Set<U...> const& other) const noexcept{return symmetric_difference(other);}  
    
    template<class ...U>
    constexpr auto cross_product(Set<U...> const&) const noexcept {return *this;}
    template<class ...U>
    constexpr auto operator*(Set<U...> const& other) const noexcept {return cross_product(other);}

    constexpr auto power_set() const noexcept {return *this;}
    template<class ...U>
    friend constexpr auto P(Set<U...> const& set) noexcept {return set.power_set();};

    constexpr size_t cardinality() const noexcept {return 0;}

    template<class ...U>
    constexpr bool is_subset(Set<U...> const&) const noexcept {return true;}
    template<class ...U>
    constexpr bool operator<=(Set<U...> const& other) {return is_subset(other);}
    
    template<class ...U>
    bool is_proper_subset(Set<U...> const&) const noexcept {
        if constexpr (std::is_same_v<Set<U...>, Set<>>)
            return false;
        else
            return true;
    }
    template<class ...U>
    bool operator<(Set<U...> const& other) {return is_proper_subset(other);}
    
    template<class ...U>
    bool is_superset(Set<U...> const&) const noexcept {
        if constexpr (std::is_same_v<Set<U...>, Set<>>)
            return true;
        else
            return false;
    }

    template<class ...U>
    bool operator>=(Set<U...> const& other) const noexcept {return is_superset(other);}
    
    template<class ...U>
    bool is_proper_superset(Set<U...> const&) const noexcept {return false;}
    template<class ...U>
    bool operator>(Set<U...> const& other) const noexcept {return is_proper_superset(other);} 

    template<class ...U>
    bool is_equivalent(Set<U...> const& other) const noexcept {return *this == other;}

    template<class ...U>
    bool is_overlapping(Set<U...> const&) const noexcept {return true;}

    template<class ...U>
    bool is_disjoint(Set<U...> const&) const noexcept {return false;}

    bool is_finite() const noexcept {return true;}
    bool is_infinite() const noexcept {return false;}
    bool is_singleton() const noexcept {return false;}
};

namespace std {
    template<>
    struct hash<Set<>> {
        hash() noexcept {};
        std::size_t operator()(Set<> const& s) const noexcept {
            using hash_t = std::hash<typename Set<>::value_types>;
            return hash_t{}(*Set<>::iter_);
        }
    };
}

using empty_set_t = Set<>;