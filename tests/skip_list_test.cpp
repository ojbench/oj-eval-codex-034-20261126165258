#include "skip_list.hpp"

#include <cassert>
#include <climits>
#include <random>
#include <set>

struct Key {
    int key;
    explicit Key(int value) : key(value) {}
    Key &operator=(const Key &) = delete;
    bool operator<(const Key &other) const { return key < other.key; }
};

int main() {
    SkipList<Key> list;
    std::set<int> reference;
    std::mt19937 random(12345);
    for (int i = 0; i < 500000; ++i) {
        int key = static_cast<int>(random() % 100000) - 50000;
        switch (random() % 3) {
        case 0: list.insert(Key(key)); reference.insert(key); break;
        case 1: list.deleteItem(Key(key)); reference.erase(key); break;
        default: assert(list.search(Key(key)) == (reference.count(key) != 0));
        }
    }
    for (int key = -50000; key < 50000; ++key)
        assert(list.search(Key(key)) == (reference.count(key) != 0));
    for (int key : reference) list.deleteItem(Key(key));
    for (int key = -50000; key < 50000; ++key) assert(!list.search(Key(key)));
    for (int key : {INT_MIN, 0, INT_MAX}) {
        list.insert(Key(key));
        list.insert(Key(key));
        assert(list.search(Key(key)));
        list.deleteItem(Key(key));
        assert(!list.search(Key(key)));
        list.deleteItem(Key(key));
    }
    for (int i = 0; i < 500000; ++i) list.insert(Key(i));
    for (int i = 499999; i >= 0; --i) assert(list.search(Key(i)));
    for (int i = 0; i < 500000; ++i) list.deleteItem(Key(i));
}
