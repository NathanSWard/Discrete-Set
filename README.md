##set##
* `Inside namespace::discrete`
* Constructors
  * `set()`
  * `set(set const&)`
  * `set(set&&)`
  * `template<class ...Us> set(init_list_t, Us&&... us)`
    * `init_list_t`: empty struct tag
    * `Us`: elements to insert into set
    * Set type is a combination of all `Us` types removing dulpicates
      * e.g. `set(init_list_t, 1, 'a', 2) -> set<int, char>`
  * `template<class It, class Fn> set(It first, It last, Fn fn)`
    * `It`: Iterator type
    * `Fn`: Function type
      * must return bool and process `It::value_type`
    * creates a set from the range of iterators filtering using `Fn` 
* Modifiers
  * `template<class U, class Args>`
  * `void emplace(std::in_place_type<U>, Args&&... args)`
    * in place constructs type U with arguments args
  * `template<class ...Args`
  * `void emplace(Args&&... args)`
    * inserts all arguments into the set
    * effetively calls
      * `(emplace<decltype(Args)>(args), ...);`
  * `template<class U>`
  * `bool erase(U&&)`
    * if argument exists in set, that element is removed
    * function does nothing if type is not present in the set
  * `void clear()`
    * clears the entire set
* Lookup
  * `size_t size() const noexcpet`
    * return the current size of the set
  * `bool empty() const noexcept`
    * retuns if the set is empty or not
  * `template<class U>`
  * `bool contains(U&& val) const noexcept`
    * returns true if the set contains val
* Iterators
  * `begin() noexcept`, `begin() const noexcept`, `cbegin() const noexcept`
    * returns an iterator to the beginning of the set
  * `end() noexcept`, `end() const noexcept`, `cend() const noexcept`
    * returns an iterator to the end of the set
* Set Operations
  * `operator==` and `operator!=`
    * return true is two sets are the same
    * note: the set types do not have to match
  * `template<class ...U>`
  * `auto intersection(set<U...> const&)`
    * `operator &`
    * returns a set of the elements that only appear in both sets
    * return type: `set<T..., U...>` with no duplicate types 
  * `template<class ...U>`
  * `auto Union(set<U...> const&)`
    * `operator |`
    * returns a set of both sets' elements (removing duplicate elements) 
    * return type: `set<T..., U...>` with no duplicate types
  * `template<class ...U>`
  * `auto difference(set<U...> const&)`
    * `operator -`
    * returns a set of of elements only found in the first set 
    * return type: `set<T...>`
  * `template<class ...U>`
  * `auto symmetric_difference(set<U...> const&)`
    * `operator ^`
    * returns a set of of elements only found in the first set or second set 
    * return type: `set<T..., U...>` with no duplicate types
  * `template<class ...U>`
  * `auto cross_product(set<U...> const&)`
    * `operator *`
    * returns a vector of sets with the result of both set's cartesian product 
    * return type: `std::vector<set<T..., U...>>`
  * `template<class ...U>`
  * `auto power_set()`
    * same as: `auto P(set<T...> const&)`
    * returns a vector of sets with the result of this set's power set 
    * return type: `std::vector<set<T...>>`
  * `size_t cardinality()` `size_t size()`
    * returns the size of the set
  * `template<class ...U>`  
  * `bool is_subset(set<U...> const&)`  
    * returns true is this set is a sub set of the parameter 
  * `template<class ...U>`  
  * `bool is_proper_subset(set<U...> const&)`  
    * returns true is this set is a proper sub set of the parameter 
  * `template<class ...U>`  
  * `bool is_superset(set<U...> const&)`  
    * returns true is this set is a super set of the parameter 
  * `template<class ...U>`  
  * `bool is_proper_superset(set<U...> const&)`  
    * returns true is this set is a proper superset set of the parameter 
  * `template<class ...U>`  
  * `bool is_equivalent(set<U...> const&)`  
    * returns true is this->size() == parameter's size 
  * `template<class ...U>`  
  * `bool is_overlapping(set<U...> const&)`  
    * returns true is this set has an common elements with the parameter 
  * `template<class ...U>`  
  * `bool is_disjoint(set<U...> const&)`  
    * returns true is if this set has not elements in common with the parameter 
  * `bool is_finite()` 
    * returns true is this set has finite amount of elements 
  * `bool is_infinite()` 
    * returns true is this set has an infinite amount of elements 
  * `bool is_singleton()` 
    * returns true is this set has one element
* Empty Set
  * `discrete::set<>`
    * a template specializtion of an empty set
    * it is not possible to add elements to this set
    * methods such as emplace() will not fail, but rather simply to nothing 
