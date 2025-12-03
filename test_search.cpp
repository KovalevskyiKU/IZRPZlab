#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "../src/search_algorithms.hpp"
#include <vector>
#include <string>

using namespace std;

TEST_CASE("Basic search on small text") {
    vector<string> patterns = {"cat", "dog", "cow"};
    string text = "cat and dog and another cat and cow";

    vector<size_t> naive_counts;
    size_t naive_total = naive_search(text, patterns, naive_counts);

    AhoCorasick aho;
    aho.build_automaton(patterns);
    vector<size_t> aho_counts;
    size_t aho_total = aho.search(text, aho_counts);

    CHECK(naive_total == aho_total);
    CHECK(naive_counts == aho_counts);
}

TEST_CASE("Empty text") {
    vector<string> patterns = {"cat", "dog"};
    string text = "";

    vector<size_t> naive_counts;
    vector<size_t> aho_counts;

    size_t naive_total = naive_search(text, patterns, naive_counts);

    AhoCorasick aho;
    aho.build_automaton(patterns);
    size_t aho_total = aho.search(text, aho_counts);

    CHECK(naive_total == 0);
    CHECK(aho_total == 0);
    CHECK(naive_counts == vector<size_t>({0, 0}));
    CHECK(naive_counts == aho_counts);
}

TEST_CASE("Empty patterns") {
    vector<string> patterns; // пустий
    string text = "some text with cat and dog";

    vector<size_t> naive_counts;
    vector<size_t> aho_counts;

    size_t naive_total = naive_search(text, patterns, naive_counts);

    AhoCorasick aho;
    aho.build_automaton(patterns);
    size_t aho_total = aho.search(text, aho_counts);

    CHECK(naive_total == 0);
    CHECK(aho_total == 0);
    CHECK(naive_counts.size() == 0);
    CHECK(aho_counts.size() == 0);
}

TEST_CASE("Overlapping matches") {
    vector<string> patterns = {"ana"};
    string text = "bananas"; // "bananas" -> "ana" зустрічається двічі

    vector<size_t> naive_counts;
    vector<size_t> aho_counts;

    size_t naive_total = naive_search(text, patterns, naive_counts);

    AhoCorasick aho;
    aho.build_automaton(patterns);
    size_t aho_total = aho.search(text, aho_counts);

    CHECK(naive_total == 2);
    CHECK(aho_total == 2);
    CHECK(naive_counts == aho_counts);
}

TEST_CASE("Text with spaces and punctuation") {
    vector<string> patterns = {"cat", "dog"};
    string text = "cat, dog! another cat; dog?";

    vector<size_t> naive_counts;
    size_t naive_total = naive_search(text, patterns, naive_counts);

    AhoCorasick aho;
    aho.build_automaton(patterns);
    vector<size_t> aho_counts;
    size_t aho_total = aho.search(text, aho_counts);

    CHECK(naive_total == aho_total);
    CHECK(naive_counts == aho_counts);
}

// ---- Слово відсутнє в тексті ----
TEST_CASE("Pattern not present in text") {
    vector<string> patterns = {"cat"};
    string text = "there is no animal here";

    vector<size_t> naive_counts;
    vector<size_t> aho_counts;

    size_t naive_total = naive_search(text, patterns, naive_counts);

    AhoCorasick aho;
    aho.build_automaton(patterns);
    size_t aho_total = aho.search(text, aho_counts);

    CHECK(naive_total == 0);
    CHECK(aho_total == 0);
    CHECK(naive_counts[0] == 0);
    CHECK(naive_counts == aho_counts);
}

// ---- Декілька слів, частина є, частина нема ----
TEST_CASE("Some patterns present, some not") {
    vector<string> patterns = {"cat", "dog", "cow"};
    string text = "cat and dog only";

    vector<size_t> naive_counts;
    vector<size_t> aho_counts;

    size_t naive_total = naive_search(text, patterns, naive_counts);

    AhoCorasick aho;
    aho.build_automaton(patterns);
    size_t aho_total = aho.search(text, aho_counts);

    CHECK(naive_total == aho_total);
    CHECK(naive_counts == aho_counts);

    CHECK(naive_counts[0] == 1); // cat
    CHECK(naive_counts[1] == 1); // dog
    CHECK(naive_counts[2] == 0); // cow
}

// ---- Патерн дорівнює всьому тексту ----
TEST_CASE("Pattern equals whole text") {
    vector<string> patterns = {"banana"};
    string text = "banana";

    vector<size_t> naive_counts;
    vector<size_t> aho_counts;

    size_t naive_total = naive_search(text, patterns, naive_counts);

    AhoCorasick aho;
    aho.build_automaton(patterns);
    size_t aho_total = aho.search(text, aho_counts);

    CHECK(naive_total == 1);
    CHECK(aho_total == 1);
    CHECK(naive_counts == aho_counts);
}

// ---- Патерни, що є префіксами один одного ----
TEST_CASE("Patterns with common prefixes") {
    vector<string> patterns = {"a", "ab", "abc"};
    string text = "abc ab a";

    vector<size_t> naive_counts;
    vector<size_t> aho_counts;

    size_t naive_total = naive_search(text, patterns, naive_counts);

    AhoCorasick aho;
    aho.build_automaton(patterns);
    size_t aho_total = aho.search(text, aho_counts);

    CHECK(naive_total == aho_total);
    CHECK(naive_counts == aho_counts);

    // просто перевіряємо, що всі > 0
    CHECK(naive_counts[0] > 0); // "a"
    CHECK(naive_counts[1] > 0); // "ab"
    CHECK(naive_counts[2] > 0); // "abc"
}

// ---- Перетинні збіги між різними патернами ----
TEST_CASE("Overlapping matches between different patterns") {
    vector<string> patterns = {"ana", "na"};
    string text = "banana"; // ana: 2 рази, na: 2 рази

    vector<size_t> naive_counts;
    vector<size_t> aho_counts;

    size_t naive_total = naive_search(text, patterns, naive_counts);

    AhoCorasick aho;
    aho.build_automaton(patterns);
    size_t aho_total = aho.search(text, aho_counts);

    CHECK(naive_total == aho_total);
    CHECK(naive_counts == aho_counts);

    CHECK(naive_counts[0] == 2); // "ana"
    CHECK(naive_counts[1] == 2); // "na"
}

// ---- Порожні рядки в словнику ----
TEST_CASE("Empty strings in patterns list") {
    vector<string> patterns = {"cat", "", "dog"};
    string text = "cat and dog and cat";

    vector<size_t> naive_counts;
    vector<size_t> aho_counts;

    size_t naive_total = naive_search(text, patterns, naive_counts);

    AhoCorasick aho;
    aho.build_automaton(patterns);
    size_t aho_total = aho.search(text, aho_counts);

    CHECK(naive_total == aho_total);
    CHECK(naive_counts == aho_counts);

    // порожній патерн має 0 збігів
    CHECK(naive_counts[1] == 0);
}

// ---- Довший текст з повторенням (міні-стрес тест) ----
TEST_CASE("Long repeated text small stress test") {
    vector<string> patterns = {"cat", "dog"};
    string base = "cat and dog ";
    string text;

    const int REPEAT = 500;
    text.reserve(base.size() * REPEAT);
    for (int i = 0; i < REPEAT; ++i) {
        text += base;
    }

    vector<size_t> naive_counts;
    vector<size_t> aho_counts;

    size_t naive_total = naive_search(text, patterns, naive_counts);

    AhoCorasick aho;
    aho.build_automaton(patterns);
    size_t aho_total = aho.search(text, aho_counts);

    CHECK(naive_total == aho_total);
    CHECK(naive_counts == aho_counts);

    // в кожному повторі по одному "cat" і одному "dog"
    CHECK(naive_counts[0] == REPEAT);
    CHECK(naive_counts[1] == REPEAT);
}
