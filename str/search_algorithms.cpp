#include "search_algorithms.hpp"
#include <queue>
#include <algorithm>
using namespace std;

/**
 * @brief Наївний пошук шаблонів у тексті.
 *
 * Шукає всі входження кожного шаблону з списку вхідних шаблонів в тексті.
 * Використовує стандартну функцію find для кожного шаблону.
 *
 * @param text Текст для пошуку шаблонів.
 * @param patterns Список шаблонів для пошуку.
 * @param per_pattern Вектор, який зберігає кількість входжень кожного шаблону.
 * @return Загальна кількість входжень усіх шаблонів.
 */
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

/**
 * @brief Конструктор вершини AhoNode.
 * Встановлює значення за замовчуванням для переходів та суфіксного посилання.
 */
AhoNode::AhoNode() {
    fill(begin(next), end(next), -1);
    link = -1;
}

/**
 * @brief Створює автомат Ахо–Корасіка.
 * Початково автомат має лише кореневу вершину.
 */
AhoCorasick::AhoCorasick() {
    trie.emplace_back();
}

/**
 * @brief Перетворює символ у індекс від 0 до 25.
 *
 * Для літер a..z та A..Z, але не для інших символів.
 *
 * @param c Символ для перетворення.
 * @return Індекс символу або -1 для некоректних символів.
 */
int AhoCorasick::char_id(char c) {
    if (c >= 'a' && c <= 'z') return c - 'a';
    if (c >= 'A' && c <= 'Z') return c - 'A';
    return -1;
}

/**
 * @brief Додає шаблон до автомата Ахо–Корасіка.
 *
 * Кожен символ шаблону обробляється за допомогою переходів у бору.
 * Коли шаблон завершено, індекс шаблону додається до виходу поточної вершини.
 *
 * @param s Рядок-шаблон для додавання.
 * @param idx Індекс шаблону.
 */
void AhoCorasick::add_pattern(const string &s, int idx) {
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

/**
 * @brief Створює автомат Ахо–Корасіка.
 * Будує бор за усіма шаблонами і встановлює суфіксні посилання.
 *
 * @param patterns_ Список шаблонів для додавання в автомат.
 */
void AhoCorasick::build_automaton(const vector<string> &patterns_) {
    patterns = patterns_;
    for (int i = 0; i < (int)patterns.size(); ++i) {
        if (!patterns[i].empty()) add_pattern(patterns[i], i);
    }

    queue<int> q;
    trie[0].link = 0;

    // встановлюємо посилання для кожного переходу з кореня
    for (int c = 0; c < AhoNode::ALPHA; ++c) {
        int to = trie[0].next[c];
        if (to != -1) {
            trie[to].link = 0;
            q.push(to);
        } else {
            trie[0].next[c] = 0;
        }
    }

    // BFS для побудови суфіксних посилань
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

/**
 * @brief Пошук шаблонів в тексті за допомогою автомата Ахо–Корасіка.
 *
 * Виконується по кожному символу тексту. Кількість входжень кожного шаблону
 * записується у вектор per_pattern.
 *
 * @param text Текст для пошуку.
 * @param per_pattern Вектор, в який записується кількість входжень кожного шаблону.
 * @return Загальна кількість входжень усіх шаблонів.
 */
size_t AhoCorasick::search(const string &text, vector<size_t> &per_pattern) const {
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
