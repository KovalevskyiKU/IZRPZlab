#include "search_algorithms.hpp"
#include <queue>
#include <algorithm>
using namespace std;

// ---- Наївний ----
size_t naive_search(const string &text,
                    const vector<string> &patterns,
                    vector<size_t> &per_pattern) {
    per_pattern.assign(patterns.size(), 0);
    size_t total_matches = 0;

    for (size_t i = 0; i < patterns.size(); ++i) {
        const string &p = patterns[i];
        if (p.empty()) continue;

        size_t pos = text.find(p, 0);
        while (pos != string::npos) {
            ++per_pattern[i];
            ++total_matches;
            pos = text.find(p, pos + 1);
        }
    }

    return total_matches;
}

// ---- Ахо–Корасік ----

AhoNode::AhoNode() {
    fill(begin(next), end(next), -1);
    link = -1;
}

AhoCorasick::AhoCorasick() {
    trie.emplace_back();
}

int AhoCorasick::char_id(char c) {
    if (c >= 'a' && c <= 'z') return c - 'a';
    if (c >= 'A' && c <= 'Z') return c - 'A';
    return -1;
}

void AhoCorasick::add_pattern(const string &s, int idx) {
    int v = 0;
    for (char c : s) {
        int id = char_id(c);
        if (id == -1) continue;
        if (trie[v].next[id] == -1) {
            trie[v].next[id] = (int)trie.size();
            trie.emplace_back();
        }
        v = trie[v].next[id];
    }
    trie[v].out.push_back(idx);
}

void AhoCorasick::build_automaton(const vector<string> &patterns_) {
    patterns = patterns_;
    for (int i = 0; i < (int)patterns.size(); ++i) {
        if (!patterns[i].empty()) add_pattern(patterns[i], i);
    }

    queue<int> q;
    trie[0].link = 0;

    for (int c = 0; c < AhoNode::ALPHA; ++c) {
        int to = trie[0].next[c];
        if (to != -1) {
            trie[to].link = 0;
            q.push(to);
        } else {
            trie[0].next[c] = 0;
        }
    }

    while (!q.empty()) {
        int v = q.front(); q.pop();
        int link = trie[v].link;

        for (int pattern_idx : trie[link].out) {
            trie[v].out.push_back(pattern_idx);
        }

        for (int c = 0; c < AhoNode::ALPHA; ++c) {
            int to = trie[v].next[c];
            if (to != -1) {
                trie[to].link = trie[link].next[c];
                q.push(to);
            } else {
                trie[v].next[c] = trie[link].next[c];
            }
        }
    }
}

size_t AhoCorasick::search(const string &text, vector<size_t> &per_pattern) const {
    per_pattern.assign(patterns.size(), 0);
    size_t total_matches = 0;
    int v = 0;

    for (char ch : text) {
        int id = char_id(ch);
        if (id == -1) {
            v = 0;
            continue;
        }
        v = trie[v].next[id];

        for (int pattern_idx : trie[v].out) {
            ++per_pattern[pattern_idx];
            ++total_matches;
        }
    }

    return total_matches;
}
