#ifndef SKIP_LIST_HPP
#define SKIP_LIST_HPP

#include <cstdint>
#include <vector>

template <typename T>
class SkipList {
private:
    static constexpr int maxLevel = 32;

    struct Links {
        std::vector<Links *> next;
        explicit Links(int height) : next(height, nullptr) {}
    };

    struct Node : Links {
        T value;
        Node(const T &item, int height) : Links(height), value(item) {}
    };

    Links head;
    int levels = 1;
    std::uint64_t randomState = 0x9e3779b97f4a7c15ULL;

    const T &value(const Links *node) const {
        return static_cast<const Node *>(node)->value;
    }

    int randomHeight() {
        randomState ^= randomState >> 12;
        randomState ^= randomState << 25;
        randomState ^= randomState >> 27;
        std::uint64_t bits = randomState * 2685821657736338717ULL;
        int height = 1;
        while (height < maxLevel && (bits & 1)) {
            ++height;
            bits >>= 1;
        }
        return height;
    }

    Links *locate(const T &item, Links **previous = nullptr) {
        Links *current = &head;
        for (int level = levels - 1; level >= 0; --level) {
            while (current->next[level] && value(current->next[level]) < item)
                current = current->next[level];
            if (previous) previous[level] = current;
        }
        return current->next[0];
    }

public:
    SkipList() : head(maxLevel) {}
    SkipList(const SkipList &) = delete;
    SkipList &operator=(const SkipList &) = delete;

    ~SkipList() {
        Links *current = head.next[0];
        while (current) {
            Links *next = current->next[0];
            delete static_cast<Node *>(current);
            current = next;
        }
    }

    void insert(const T &item) {
        Links *previous[maxLevel];
        Links *candidate = locate(item, previous);
        // locate already guarantees that candidate is not less than item.
        if (candidate && !(item < value(candidate))) return;
        int height = randomHeight();
        Node *node = new Node(item, height);
        for (int level = levels; level < height; ++level)
            previous[level] = &head;
        for (int level = 0; level < height; ++level) {
            node->next[level] = previous[level]->next[level];
            previous[level]->next[level] = node;
        }
        if (height > levels) levels = height;
    }

    bool search(const T &item) {
        Links *candidate = locate(item);
        return candidate && !(item < value(candidate));
    }

    void deleteItem(const T &item) {
        Links *previous[maxLevel];
        Links *candidate = locate(item, previous);
        if (!candidate || item < value(candidate)) return;
        for (int level = 0; level < levels; ++level) {
            if (previous[level]->next[level] != candidate) break;
            previous[level]->next[level] = candidate->next[level];
        }
        delete static_cast<Node *>(candidate);
        while (levels > 1 && !head.next[levels - 1]) --levels;
    }
};

#endif
