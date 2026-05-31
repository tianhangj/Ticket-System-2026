#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cmath>       // maybe you need it
#include <cstddef>     // for size_t
#include <functional>  // for std::less

#include "exceptions.hpp"

namespace sjtu {

template <class T, class Compare = std::less<T>>
class priority_queue {
private:
    static size_t popcount(size_t x) {
        size_t res = 0;
        while (x) {
            res += (x & 1);
            x >>= 1;
        }
        return res;
    }
    struct Node {
        struct Node* father;
        struct Node* child;
        struct Node* next;
        size_t size;
        T value;
        Node() = delete;
        Node(const T& value) : father(nullptr), child(nullptr), next(nullptr), size(1), value(value) {}
    };
    Node* root;
    size_t _size;
    void copy(Node *& node, Node* other) {
        if (other == nullptr) {
            node = nullptr;
            return;
        }
        node = new Node(other->value);
        node->size = other->size;
        copy(node->child, other->child);
        copy(node->next, other->next);
    }
    void release(Node* node) {
        if (node == nullptr) {
            return;
        }
        release(node->child);
        release(node->next);
        delete node;
    }
    Node* link(Node* a, Node* b) {
        if (Compare()(a->value, b->value)) {
            std::swap(a, b);
        }
        b->father = a;
        b->next = a->child;
        a->child = b;
        a->size += b->size;
        return a;
    }
    void _merge(Node* a[], size_t len_a, Node* b[], size_t len_b, Node* res[]) {
        int i = len_a - 1, j = len_b - 1, k = 0;
        while (i >= 0 || j >= 0) {
            if (i == -1 || (i >= 0 && j >= 0 && a[i]->size > b[j]->size)) {
                res[k++] = b[j--];
            } else {
                res[k++] = a[i--];
            }
        }
    }
public:
    priority_queue() {
        root = nullptr;
        _size = 0;
    }
    priority_queue(const priority_queue& other) {
        copy(root, other.root);
        _size = other._size;
    }
    ~priority_queue() {
        release(root);
    }

    priority_queue& operator=(const priority_queue& other) {
        if (this == &other) {
            return *this;
        }
        release(root);
        copy(root, other.root);
        _size = other._size;
        return *this;
    }

    /**
     * @brief Push one element into the queue.
     * @note Its time complexity shall be O(log n).
     */
    void push(const T& value) {
        priority_queue<T, Compare> new_queue;
        new_queue.root = new Node(value);
        new_queue._size = 1;
        try {
            merge(new_queue);
        } catch (...) {
            throw;
        }
    }

    /**
     * @return A const reference of the top element in the queue.
     * @throws container_is_empty when the top element does not exist.
     * @note Its time complexity shall be O(1).
     */
    const T& top() const {
        if (root == nullptr) {
            throw container_is_empty();
        }
        Node* res = root;
        Node* p = root->next;
        while (p) {
            if (Compare()(res->value, p->value)) {
                res = p;
            }
            p = p->next;
        }
        return res->value;
    }

    /**
     * @brief remove the top element in the queue.
     * @throws container_is_empty when the top element does not exist.
     * @note Its time complexity shall be O(n).
     */
    void pop() {
        if (_size == 0) {
            throw container_is_empty();
        }
        Node* max_node = root;
        Node* p = root;
        while (p) {
            if (Compare()(max_node->value, p->value)) {
                max_node = p;
            }
            p = p->next;
        }
        if (max_node == root) {
            root = root->next;
        } else {
            Node* pre = root;
            while (pre->next != max_node) {
                pre = pre->next;
            }
            pre->next = max_node->next;
        }
        if (max_node->size == 1) {
            delete max_node;
            _size--;
            return;
        }
        priority_queue<T, Compare> new_queue;
        new_queue.root = max_node->child;
        new_queue.root->father = nullptr;
        new_queue._size = max_node->size - 1;
        _size -= max_node->size;
        try {
            merge(new_queue);
            delete max_node;
        } catch (...) {
            Node* p = root;
            while (p) {
                if (p->size > max_node->size) {
                    max_node->next = p->next;
                    p->next = max_node;
                    break;
                }
            }
            max_node->child->father = max_node;
            new_queue.root = nullptr;
            throw;
        }
    }

    /**
     * @return number of elements in the queue.
     */
    size_t size() const {
        return _size;
    }

    /**
     * @return whether there is any element in the queue.
     */
    bool empty() const {
        return _size == 0;
    }

    /**
     * @brief Clear all elements in the queue.
     * @note Its time complexity shall be O(n).
     */
    void clear() {
        release(root);
        root = nullptr;
        _size = 0;
    }

    /**
     * @brief Merge element sets of two queues.
     * @note Its time complexity shall be O(log n).
     */
    void merge(priority_queue& other) {
        if (this == &other) {
            return;
        }
        if (other.root == nullptr) {
            return;
        }
        if (root == nullptr) {
            root = other.root;
            _size = other._size;
            other.root = nullptr;
            other._size = 0;
            return;
        }
        size_t new_size = _size + other._size;
        size_t d1 = popcount(_size);
        size_t d2 = popcount(other._size);
        Node* p1[d1]; Node* c1[d1]; size_t s1[d1];
        Node* p2[d2]; Node* c2[d2]; size_t s2[d2];
        for (size_t i = 0; i < d1; ++i) {
            p1[i] = root;
            c1[i] = root->child; 
            s1[i] = root->size;
            root = root->next;
        }
        for (size_t i = 0; i < d2; ++i) {
            p2[i] = other.root;
            c2[i] = other.root->child;
            s2[i] = other.root->size;
            other.root = other.root->next;
        }
        try {
            Node* _merged[d1 + d2];
            Node* _new[popcount(new_size)];
            int idx = 0;
            _merge(p1, d1, p2, d2, _merged);
            size_t i = 0, j = 0;
            size_t remain = new_size;
            for (size_t i = 0;i < d1 + d2;) {
                if (remain & _merged[i]->size) {
                    _new[idx++] = _merged[i];
                    remain ^= _merged[i]->size;
                    _merged[i] = nullptr;
                    ++i;
                } else {
                    if (i && _merged[i-1] && _merged[i]->size == _merged[i-1]->size) {
                        _merged[i] = link(_merged[i-1], _merged[i]);
                        _merged[i-1] = nullptr;
                    } else {
                        ++i;
                    }
                }
            }
            for (int i = 0; i < idx; ++i) {
                _new[i]->next = root;
                _new[i]->father = nullptr;
                root = _new[i];
            }
            _size = new_size;
            other.root = nullptr;
            other._size = 0;
        } catch (...) {
            root = p1[0];
            for (size_t i = 0; i < d1; ++i) {
                p1[i]->father = nullptr;
                p1[i]->child = c1[i];
                p1[i]->size = s1[i];
                p1[i]->next = nullptr;
                if (i) {
                    p1[i-1]->next = p1[i];
                }
            }
            other.root = p2[0];
            for (size_t i = 0; i < d2; ++i) {
                p2[i]->father = nullptr;
                p2[i]->child = c2[i];
                p2[i]->size = s2[i];
                p2[i]->next = nullptr;
                if (i) {
                    p2[i-1]->next = p2[i];
                }
            }
            throw;
        }
    }
};

}  // namespace sjtu

#endif