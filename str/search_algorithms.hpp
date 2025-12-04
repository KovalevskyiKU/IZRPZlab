#pragma once
#include <string>
#include <vector>

size_t naive_search(const std::string &text,
                    const std::vector<std::string> &patterns,
                    std::vector<size_t> &per_pattern);

// ---- Ахо–Корасік ----

struct AhoNode {
    static const int ALPHA = 26;
    int next[ALPHA];
    int link;
    std::vector<int> out;

    AhoNode();
};

struct AhoCorasick {
    std::vector<AhoNode> trie;
    std::vector<std::string> patterns;

    AhoCorasick();

    static int char_id(char c);
    void add_pattern(const std::string &s, int idx);
    void build_automaton(const std::vector<std::string> &patterns_);
    size_t search(const std::string &text, std::vector<size_t> &per_pattern) const;
};
