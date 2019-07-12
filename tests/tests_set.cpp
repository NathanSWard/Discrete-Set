// tests_set.cpp

#include "catch.hpp"
#include "../set.hpp"

TEST_CASE("Constructor") {
    SECTION("Default") {
        Set<int> s{};
        SECTION("Copy") {
            Set<int> const copy{s};
            CHECK(copy == s);
        }
        SECTION("Move") {
            Set<int> const copy{s};
            Set<int> move{std::move(s)};
            CHECK(move == copy);
        }
    }

    SECTION("Construct with values") {
        Set<int, char, float> const s{init_list, 1, 'a', 3.14f};
        CHECK(std::is_same_v<std::variant<int, char, float>, typename decltype(s)::value_types>);
        CHECK(s.size() == 3);
    }
}

TEST_CASE("Lookup Methods") {
    Set<int, char, float> const s{init_list, 1, 'a', 3.14f};

    SECTION("size") {
        CHECK(s.size() == 3);
    }

    SECTION("empty") {
        CHECK(!s.empty());
    }

    SECTION("contains") {
        CHECK(s.contains(1));
        CHECK(s.contains('a'));
        CHECK(s.contains(3.14f));
        CHECK(!s.contains(2));
        CHECK(!s.contains('b'));
        CHECK(!s.contains(0.f));
        CHECK(!s.contains(std::string{"hello"}));
    }
}

TEST_CASE("Set Operations") {
    Set const s1{init_list, 1, 'a'};
    Set const s2{init_list, 'a', 3.14f};
    Set const s3{init_list, 0.0f};

    using result_t = Set<int, char, float>;

    SECTION("Comparison") {
        CHECK(s1 != s2);
        auto const copy{s1};
        CHECK(copy == s1);
    }

    SECTION("intersection") {
        CHECK(s1.intersection(s2) == result_t{init_list, 'a'});
        auto const s{s1 & s3};
        CHECK(s.empty());
    }

    SECTION("Union") {
        CHECK(s1.Union(s2) == result_t{init_list, 1, 'a', 3.14f});
        auto const s{s1 | s3};
        CHECK(s == result_t{init_list, 1, 'a', 0.0f});
    }

    SECTION("difference") {
        CHECK(s1.difference(s2) == result_t{init_list, 1});
        auto const s{s1 - s3};
        CHECK(s == result_t{init_list, 1, 'a'});
    }

    SECTION("symmetric_difference") {
        CHECK(s1.symmetric_difference(s2) == result_t{init_list, 1, 3.14f});
        auto const s{s1 ^ s3};
        CHECK(s == result_t{init_list, 1, 'a', 0.0f});
    }

    SECTION("cross_product") {
        {
            std::vector<result_t> v{result_t{init_list, 1, 'a'}, result_t{init_list, 1, 3.14f}, result_t{init_list, 'a', 'a'}, result_t{init_list, 'a', 3.14f}};
            auto result{s1.cross_product(s2)};
            for (auto&& s : v) 
                CHECK(std::find(result.begin(), result.end(), s) != result.end());
        }
        {
            std::vector<result_t> v{result_t{init_list, 1, 0.0f}, result_t{init_list, 'a', 0.0f}};
            auto const result{s1 * s3};
            for (auto&& s : v) 
                CHECK(std::find(result.begin(), result.end(), s) != result.end());
        }

    }

    SECTION("power_set") {
        {
            using s1_t = Set<int, char>;
            std::vector<s1_t> v{s1_t{}, s1_t{init_list, 1}, s1_t{init_list, 'a'}, s1_t{init_list, 1, 'a'}};
            auto result{s1.power_set()};
            for (auto&& s : v) 
                CHECK(std::find(result.begin(), result.end(), s) != result.end());
        }
        {
            using s3_t = Set<float>;
            std::vector<s3_t> v{s3_t{}, s3_t{init_list, 0.f}};
            auto result{P(s3)};
            for (auto&& s : v) 
                CHECK(std::find(result.begin(), result.end(), s) != result.end());
        }
    }

    SECTION("subset/superset") {
        Set<int> const s{init_list, 1};
        
        SECTION("is_subest") {
            CHECK(s.is_subset(s1));
            CHECK(!s3.is_subset(s1));
        }

        SECTION("is_proper_subset") {
            CHECK(s.is_proper_subset(s1));
            auto const copy{s1};
            CHECK(!copy.is_proper_subset(s1));
        }

        SECTION("is_superset") {
            CHECK(s1.is_superset(s));
            CHECK(!s1.is_superset(s3));
        }

        SECTION("is_proper_superset") {
            CHECK(s1.is_proper_superset(s));
            auto const copy(s1);
            CHECK(!s1.is_proper_superset(copy));
        }
    }

    SECTION("is_equivalent") {
        CHECK(s1.is_equivalent(s1));
        CHECK(!s1.is_equivalent(s3));
    }

    SECTION("is_overlapping") {
        CHECK(s1.is_overlapping(s2));
        CHECK(!s1.is_overlapping(s3));
    }

    SECTION("is_disjoint") {
        CHECK(!s1.is_disjoint(s2));
        CHECK(s1.is_disjoint(s3));       
    }

    SECTION("is_finite") {
        CHECK(s1.is_finite());
        CHECK(Set<int>{}.is_finite());
    }

    SECTION("is_infinite") {
        CHECK(!s1.is_infinite());
    }

    SECTION("is_singleton") {
        CHECK(!s1.is_singleton());
        CHECK(s3.is_singleton());
    }

}