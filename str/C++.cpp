#include <bits/stdc++.h>
using namespace std;

// ---------------------- Наївний пошук ----------------------
// Рахує кількість входжень КОЖНОГО патерну + загальну
size_t naive_search(const string &text,
                    const vector<string> &patterns,
                    vector<size_t> &per_pattern) {
    per_pattern.assign(patterns.size(), 0);
    size_t total_matches = 0;

    for (size_t i = 0; i < patterns.size(); ++i) {
        const string &p = patterns[i];
        if (p.empty()) continue;

        for (size_t pos = 0; pos + p.size() <= text.size(); ++pos) {
            bool ok = true;
            for (size_t j = 0; j < p.size(); ++j) {
                if (text[pos + j] != p[j]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                ++per_pattern[i];
                ++total_matches;
            }
        }
    }

    return total_matches;
}

// ---------------------- Алгоритм Ахо-Корасік ----------------------
// Алфавіт: a..z / A..Z. Інші символи ігноруємо.

struct AhoNode {
    static const int ALPHA = 26; // 'a'..'z'
    int next[ALPHA];
    int link;
    vector<int> out;  // індекси патернів

    AhoNode() {
        fill(begin(next), end(next), -1);
        link = -1;
    }
};

struct AhoCorasick {
    vector<AhoNode> trie;
    vector<string> patterns;

    AhoCorasick() {
        trie.emplace_back(); // root = 0
    }

    static int char_id(char c) {
        if (c >= 'a' && c <= 'z') return c - 'a';
        if (c >= 'A' && c <= 'Z') return c - 'A';
        return -1; // не літера
    }

    void add_pattern(const string &s, int idx) {
        int v = 0;
        for (char c : s) {
            int id = char_id(c);
            if (id == -1) continue; // пропускаємо не-літери
            if (trie[v].next[id] == -1) {
                trie[v].next[id] = (int)trie.size();
                trie.emplace_back();
            }
            v = trie[v].next[id];
        }
        trie[v].out.push_back(idx);
    }

    void build_automaton(const vector<string> &patterns_) {
        patterns = patterns_;
        // будуємо бор
        for (int i = 0; i < (int)patterns.size(); ++i) {
            if (!patterns[i].empty()) add_pattern(patterns[i], i);
        }

        queue<int> q;
        trie[0].link = 0;

        // переходи з кореня
        for (int c = 0; c < AhoNode::ALPHA; ++c) {
            int to = trie[0].next[c];
            if (to != -1) {
                trie[to].link = 0;
                q.push(to);
            } else {
                trie[0].next[c] = 0;
            }
        }

        // BFS
        while (!q.empty()) {
            int v = q.front(); q.pop();
            int link = trie[v].link;

            // наслідуємо виходи
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

    // Рахуємо кількість входжень кожного патерну + загальну
    size_t search(const string &text, vector<size_t> &per_pattern) const {
        per_pattern.assign(patterns.size(), 0);
        size_t total_matches = 0;
        int v = 0;

        for (char ch : text) {
            int id = char_id(ch);
            if (id == -1) {
                v = 0; // не літера — повертаємось у корінь
                continue;
            }
            v = trie[v].next[id];

            // усі патерни, що закінчуються в цій вершині
            for (int pattern_idx : trie[v].out) {
                ++per_pattern[pattern_idx];
                ++total_matches;
            }
        }

        return total_matches;
    }
};

// ---------------------- Вимір часу (в мікросекундах) ----------------------

template <typename Func>
long long measure_us(Func f) {
    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    f();
    auto end = high_resolution_clock::now();
    return duration_cast<microseconds>(end - start).count();
}

// ---------------------- main ----------------------

int main() {
    // ==== Вбудований словник: 10 назв тварин ====
    vector<string> patterns = {
        "cat",
        "dog",
        "horse",
        "cow",
        "sheep",
        "pig",
        "goat",
        "rabbit",
        "bird",
        "fish"
    };

    // ==== Базовий текст (A1, про тварин) ====
    string base_text = R"(This text is about animals and people.
Many children like animals. They can see a cat, a dog, a horse, a cow, a sheep, a pig, a goat, a rabbit, a bird and a fish on a farm or in books.

A cat is a small animal. It has soft fur and a long tail.
The cat likes to sleep on a chair or near the window.
Sometimes the cat plays with a ball. Children like to hold the cat and touch its fur.

A dog is a good friend for people.
The dog runs in the yard and plays with a ball or a stick.
It can help to guard the house.
When the dog is happy, it wags its tail.

A horse is a big and strong animal.
People can ride a horse.
On a farm, a horse can help people work in the field.
The cow gives milk.
People drink milk and make cheese and butter.
The sheep gives wool.
People use wool to make warm clothes.

A pig and a goat also live on the farm.
The pig likes to eat a lot of food.
The goat likes to eat green grass and leaves.
A rabbit is a small animal with long ears.
It can jump very fast.

Near the farm there is a lake.
In the lake there is a fish.
In the sky you can see a bird.
The bird can fly and sing.
Animals are important, and people should be kind to them.)";

    // ==== Робимо великий текст: повторюємо base_text багато разів ====
    const int REPEAT = 1000; // можеш змінити на 100, 500, 2000 тощо
    string text;
    text.reserve(base_text.size() * REPEAT + 1);
    for (int i = 0; i < REPEAT; ++i) {
        text += base_text;
        text += "\n";
    }

    // ---------- Наївний пошук ----------
    vector<size_t> naive_per_pattern;
    size_t naive_total = 0;
    long long naive_time_us = measure_us([&]() {
        naive_total = naive_search(text, patterns, naive_per_pattern);
    });

    // ---------- Ахо-Корасік ----------
    AhoCorasick aho;
    long long build_time_us = measure_us([&]() {
        aho.build_automaton(patterns);
    });

    vector<size_t> aho_per_pattern;
    size_t aho_total = 0;
    long long aho_time_us = measure_us([&]() {
        aho_total = aho.search(text, aho_per_pattern);
    });

    // ---------- Результати ----------
    cout << "Number of patterns: " << patterns.size() << "\n";
    cout << "Base text length:   " << base_text.size() << " characters\n";
    cout << "Repeat count:       " << REPEAT << "\n";
    cout << "Full text length:   " << text.size() << " characters\n\n";

    cout << "Naive search:\n";
    cout << "  Total matches: " << naive_total << "\n";
    cout << "  Time:          " << naive_time_us << " microseconds ("
         << fixed << setprecision(3) << naive_time_us / 1000.0 << " ms)\n\n";

    cout << "Aho–Corasick:\n";
    cout << "  Build time:    " << build_time_us << " microseconds ("
         << fixed << setprecision(3) << build_time_us / 1000.0 << " ms)\n";
    cout << "  Total matches: " << aho_total << "\n";
    cout << "  Search time:   " << aho_time_us << " microseconds ("
         << fixed << setprecision(3) << aho_time_us / 1000.0 << " ms)\n\n";

    if (naive_total != aho_total) {
        cout << "[WARNING] Different total match counts, check implementation.\n\n";
    }

    cout << "Per-pattern counts (naive / Aho–Corasick):\n";
    for (size_t i = 0; i < patterns.size(); ++i) {
        cout << "  \"" << patterns[i] << "\": "
             << naive_per_pattern[i] << " / " << aho_per_pattern[i] << "\n";
    }

    return 0;
}
