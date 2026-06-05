/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

#include <cassert>
#include <iostream>

namespace sjtu {

template<
    class Key,
    class T,
    class Compare = std::less <Key>
> class map {
 public:
  /**
   * the internal type of data.
   * it should have a default constructor, a copy constructor.
   * You can use sjtu::map as value_type by typedef.
   */
  typedef pair<const Key, T> value_type;
 private:
  enum Color: unsigned char {
    red = 0,
    black = 1
  };
  struct Node;
  const Compare comp = Compare();
  struct BaseNode {
    BaseNode *father;
    BaseNode *son[2];
    Color color;
  };
  struct Node: public BaseNode {
    value_type data;
    Node() = delete;
    Node(const Key &key, const T &value) : data(key, value) {
      this->son[0] = this->son[1] = nullptr;
      this->father = nullptr;
      this->color = red;
    }
    Node(const Node &other) : data(other.data) {
      this->son[0] = this->son[1] = nullptr;
      this->father = nullptr;
      this->color = other.color;
    }
  };
  BaseNode _header;
  size_t _size;

  Node* copy(Node *other) {
    if (other == nullptr) {
      return nullptr;
    }
    Node *node = new Node(*other);
    node->son[0] = copy(static_cast<Node*>(other->son[0]));
    node->son[1] = copy(static_cast<Node*>(other->son[1]));
    if (node->son[0] != nullptr) {
      node->son[0]->father = node;
    }
    if (node->son[1] != nullptr) {
      node->son[1]->father = node;
    }
    return node;
  }
  void _clear(Node* node) {
    if (node == nullptr) {
      return;
    }
    _clear(static_cast<Node*>(node->son[0]));
    _clear(static_cast<Node*>(node->son[1]));
    delete node;
  }
  static BaseNode* leftmost(BaseNode *node) {
    if (node->son[0] == node) {
      return node;
    }
    while (node->son[0] != nullptr) {
      node = node->son[0];
    }
    return node;
  }
  static BaseNode* rightmost(BaseNode *node) {
    if (node->son[1] == node) {
      return node;
    }
    while (node->son[1] != nullptr) {
      node = node->son[1];
    }
    return node;
  }
  bool is_red(BaseNode *node) {
    if (node == &_header) {
      return false;
    }
    return node == nullptr ? false : node->color == red;
  }
  bool is_black(BaseNode *node) {
    if (node == &_header) {
      return false;
    }
    return node == nullptr ? true : node->color == black;
  }
  struct _const_find_result {
    const Node *node;
    const BaseNode *fa;
    bool dir;
  };
  struct _find_result {
    Node *node;
    BaseNode *fa;
    bool dir;
  };
  _const_find_result _find(const Key &key) const {
    Node *fa = nullptr;
    Node *node = static_cast<Node*>(_header.son[0]);
    bool dir = 0;
    while (node != nullptr) {
      if (comp(key, node->data.first)) {
        fa = node;
        node = static_cast<Node*>(node->son[0]);
        dir = 0;
      } else if (comp(node->data.first, key)) {
        fa = node;
        node = static_cast<Node*>(node->son[1]);
        dir = 1;
      } else {
        return {node, fa, dir};
      }
    }
    return {node, fa, dir};
  }
  _find_result _find(const Key &key) {
    BaseNode *fa = &_header;
    Node *node = static_cast<Node*>(_header.son[0]);
    bool dir = 0;
    while (node != nullptr) {
      if (comp(key, node->data.first)) {
        fa = node;
        node = static_cast<Node*>(node->son[0]);
        dir = 0;
      } else if (comp(node->data.first, key)) {
        fa = node;
        node = static_cast<Node*>(node->son[1]);
        dir = 1;
      } else {
        return {node, fa, dir};
      }
    }
    return {node, fa, dir};
  }
  template<typename _T>
  void swap(_T &a, _T &b) {
    _T tmp = a;
    a = b;
    b = tmp;
  }
  void rotate(BaseNode *node, bool dir) { // false left, true right
    assert(node != &_header);
    BaseNode *s = node->son[!dir];
    assert(s != nullptr);
    node->father->son[node == node->father->son[1]] = s;
    s->father = node->father;
    node->father = s;
    node->son[!dir] = s->son[dir];
    if (s->son[dir]) {
      s->son[dir]->father = node;
    }
    s->son[dir] = node;
  }
  void fix_insert(BaseNode *node) {
    BaseNode *p;
    while (is_red(p = node->father)) {
      // std::cerr << node << " " << node->father << " " << (node->father == &_header) << "\n";
      BaseNode *g = p->father;
      bool p_dir = (p == g->son[1]);
      BaseNode *u = g->son[!p_dir];
      if (is_red(u)) {
        g->color = red;
        p->color = u->color = black;
        node = g;
        continue;
      }
      if (node != p->son[p_dir]) {
        rotate(p, p_dir);
        swap(node, p);
      }
      p->color = black;
      g->color = red;
      rotate(g, !p_dir);
      break;
    }
    _header.son[0]->color = black;
  }
 public:
  /**
   * see BidirectionalIterator at CppReference for help.
   *
   * if there is anything wrong throw invalid_iterator.
   *     like it = map.begin(); --it;
   *       or it = map.end(); ++end();
   */
  class const_iterator;
  class iterator {
   private:
    map *map_ptr;
    BaseNode* ptr;
    /**
     * TODO add data members
     *   just add whatever you want.
     */
   public:
    friend class map;
    iterator() {
      // TODO
      map_ptr = nullptr;
      ptr = nullptr;
    }
    iterator(map *map_ptr, BaseNode *ptr) {
      this->map_ptr = map_ptr;
      this->ptr = ptr;
    }
    iterator(const iterator &other) {
      // TODO
      map_ptr = other.map_ptr;
      ptr = other.ptr;
    }

    /**
     * TODO iter++
     */
    iterator operator++(int) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    /**
     * TODO ++iter
     */
    iterator &operator++() {
      if (ptr == nullptr || ptr->son[1] == ptr) {
        throw invalid_iterator();
      }
      if (ptr->son[1] != nullptr) {
        ptr = leftmost(ptr->son[1]);
        return *this;
      }
      while (ptr->father != nullptr && ptr == ptr->father->son[1]) {
        ptr = ptr->father;
      }
      if (ptr->father == nullptr) {
        throw invalid_iterator();
      }
      ptr = ptr->father;
      return *this;
    }

    /**
     * TODO iter--
     */
    iterator operator--(int) {
      iterator tmp = *this;
      --(*this);
      return tmp;
    }

    /**
     * TODO --iter
     */
    iterator &operator--() {
      if (ptr == nullptr) {
        throw invalid_iterator();
      }
      if (ptr->son[0] != nullptr) {
        ptr = rightmost(ptr->son[0]);
        return *this;
      }
      while (ptr->father != nullptr && ptr == ptr->father->son[0]) {
        ptr = ptr->father;
      }
      if (ptr->father == nullptr) {
        throw invalid_iterator();
      }
      ptr = ptr->father;
      return *this;
    }

    /**
     * a operator to check whether two iterators are same (pointing to the same memory).
     */
    value_type &operator*() const {
      return static_cast<Node*>(ptr)->data;
    }

    bool operator==(const iterator &rhs) const {
      return ptr == rhs.ptr;
    }

    bool operator==(const const_iterator &rhs) const {
      return ptr == rhs.ptr;
    }

    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
      return ptr != rhs.ptr;
    }

    bool operator!=(const const_iterator &rhs) const {
      return ptr != rhs.ptr;
    }

    /**
     * for the support of it->first.
     * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
     */
    value_type *operator->() const
    noexcept {
      return &(static_cast<Node*>(ptr)->data);
    }
  };
  class const_iterator {
    // it should has similar member method as iterator.
    //  and it should be able to construct from an iterator.
   private:
    // data members.
    const map *map_ptr;
    const BaseNode *ptr;
   public:
    friend class map;
    const_iterator() {
      // TODO
      map_ptr = nullptr;
      ptr = nullptr;
    }
    const_iterator(const map *map_ptr, const BaseNode *ptr) {
      this->map_ptr = map_ptr;
      this->ptr = ptr;
    }
    const_iterator(const const_iterator &other) {
      // TODO
      map_ptr = other.map_ptr;
      ptr = other.ptr;
    }

    const_iterator(const iterator &other) {
      // TODO
      map_ptr = other.map_ptr;
      ptr = other.ptr;
    }
    // And other methods in iterator.
    // And other methods in iterator.
    // And other methods in iterator.
    const_iterator operator++(int) {
      const_iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    /**
     * TODO ++iter
     */
    const_iterator &operator++() {
      if (ptr == nullptr || ptr->son[1] == ptr) {
        throw invalid_iterator();
      }
      if (ptr->son[1] != nullptr) {
        ptr = leftmost(ptr->son[1]);
        return *this;
      }
      while (ptr->father != nullptr && ptr == ptr->father->son[1]) {
        ptr = ptr->father;
      }
      if (ptr->father == nullptr) {
        throw invalid_iterator();
      }
      ptr = ptr->father;
      return *this;
    }

    /**
     * TODO iter--
     */
    const_iterator operator--(int) {
      const_iterator tmp = *this;
      --(*this);
      return tmp;
    }

    /**
     * TODO --iter
     */
    const_iterator &operator--() {
      if (ptr == nullptr) {
        throw invalid_iterator();
      }
      if (ptr->son[0] != nullptr) {
        ptr = rightmost(ptr->son[0]);
        return *this;
      }
      while (ptr->father != nullptr && ptr == ptr->father->son[0]) {
        ptr = ptr->father;
      }
      if (ptr->father == nullptr) {
        throw invalid_iterator();
      }
      ptr = ptr->father;
      return *this;
    }

    /**
     * a operator to check whether two iterators are same (pointing to the same memory).
     */
    const value_type &operator*() const {
      return static_cast<const Node*>(ptr)->data;
    }

    bool operator==(const iterator &rhs) const {
      return ptr == rhs.ptr;
    }

    bool operator==(const const_iterator &rhs) const {
      return ptr == rhs.ptr;
    }

    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
      return ptr != rhs.ptr;
    }

    bool operator!=(const const_iterator &rhs) const {
      return ptr != rhs.ptr;
    }

    /**
     * for the support of it->first.
     * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
     */
    const value_type *operator->() const
    noexcept {
      return &(static_cast<const Node*>(ptr)->data);
    }
  };

  /**
   * TODO two constructors
   */
  map() {
    _header.father = nullptr;
    _header.son[0] = nullptr;
    _header.son[1] = &_header;
    _header.color = black;
    _size = 0;
  }

  map(const map &other) {
    _header.father = nullptr;
    _header.son[1] = &_header;
    _header.son[0] = copy(static_cast<Node*>(other._header.son[0]));
    if (_header.son[0] != nullptr) {
      _header.son[0]->father = &_header;
    }
    _header.color = black;
    _size = other._size;
  }

  /**
   * TODO assignment operator
   */
  map &operator=(const map &other) {
    if (&other == this) {
      return *this;
    }
    _clear(static_cast<Node*>(_header.son[0]));
    _header.son[0] = copy(static_cast<Node*>(other._header.son[0]));
    if (_header.son[0] != nullptr) {
      _header.son[0]->father = &_header;
    }
    _size = other._size;
    return *this;
  }

  /**
   * TODO Destructors
   */
  ~map() {
    _clear(static_cast<Node*>(_header.son[0]));
  }

  /**
   * TODO
   * access specified element with bounds checking
   * Returns a reference to the mapped value of the element with key equivalent to key.
   * If no such element exists, an exception of type `index_out_of_bound'
   */
  T &at(const Key &key) {
    _find_result res = _find(key);
    if (res.node == nullptr) {
      throw index_out_of_bound();
    }
    return res.node->data.second;
  }

  const T &at(const Key &key) const {
    _const_find_result res = _find(key);
    if (res.node == nullptr) {
      throw index_out_of_bound();
    }
    return res.node->data.second;
  }

  /**
   * TODO
   * access specified element
   * Returns a reference to the value that is mapped to a key equivalent to key,
   *   performing an insertion if such key does not already exist.
   */
  T &operator[](const Key &key) {
    _find_result res = _find(key);
    Node *node = res.node;
    BaseNode *fa = res.fa;
    if (node == nullptr) {
      node = new Node(key, T());
      fa->son[res.dir] = node;
      node->father = fa;
      fix_insert(node);
      ++_size;
      return node->data.second;
    }
    return node->data.second;
  }

  /**
   * behave like at() throw index_out_of_bound if such key does not exist.
   */
  const T &operator[](const Key &key) const {
    _const_find_result res = _find(key);
    if (res.node == nullptr) {
      throw index_out_of_bound();
    }
    return res.node->data.second;
  }

  /**
   * return a iterator to the beginning
   */
  iterator begin() {
    if (_size == 0) {
      return end();
    }
    return iterator(this, leftmost(_header.son[0]));
  }

  const_iterator cbegin() const {
    if (_size == 0) {
      return cend();
    }
    return const_iterator(this, leftmost(_header.son[0]));
  }

  /**
   * return a iterator to the end
   * in fact, it returns past-the-end.
   */
  iterator end() {
    return iterator(this, &_header);
  }

  const_iterator cend() const {
    return const_iterator(this, &_header);
  }

  /**
   * checks whether the container is empty
   * return true if empty, otherwise false.
   */
  bool empty() const {
    return _size == 0;
  }

  /**
   * returns the number of elements.
   */
  size_t size() const {
    return _size;
  }

  /**
   * clears the contents
   */
  void clear() {
    _clear(static_cast<Node*>(_header.son[0]));
    _header.son[0] = nullptr;
    _size = 0;
  }

  /**
   * insert an element.
   * return a pair, the first of the pair is
   *   the iterator to the new element (or the element that prevented the insertion),
   *   the second one is true if insert successfully, or false.
   */
  pair<iterator, bool> insert(const value_type &value) {
    _find_result res = _find(value.first);
    // std::cerr << "insert: " << std::endl;
    // std::cerr << node << " " << fa << std::endl;
    if (res.node != nullptr) {
      return {iterator(this, res.node), false};
    }
    Node *node = new Node(value.first, value.second);
    node->father = res.fa;
    if (res.fa == &_header) {
      _header.son[0] = node;
    } else {
      res.fa->son[res.dir] = node;
    }
    fix_insert(node);
    ++_size;
    return {iterator(this, node), true};
  }

  /**
   * erase the element at pos.
   *
   * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
   */
  void erase(iterator pos) {
    if (pos == end() || pos.map_ptr != this) {
      throw invalid_iterator();
    }
    --_size;
    BaseNode *n = pos.ptr;
    if (n->son[0] && n->son[1]) {
      BaseNode *x = rightmost(n->son[0]);
      if (x->father == n) {
        x->father = n->father;
        x->father->son[n == x->father->son[1]] = x;
        n->son[0] = x->son[0];
        n->father = x;
        x->son[0] = n;
        swap(x->son[1], n->son[1]);
      } else {
        x->father->son[x == x->father->son[1]] = n;
        n->father->son[n == n->father->son[1]] = x;
        swap(x->father, n->father);
        swap(x->son[0], n->son[0]);
        swap(x->son[1], n->son[1]);
      }
      if (n->son[0]) {
        n->son[0]->father = n;
      }
      if (n->son[1]) {
        n->son[1]->father = n;
      }
      if (x->son[0]) {
        x->son[0]->father = x;
      }
      if (x->son[1]) {
        x->son[1]->father = x;
      }
      swap(x->color, n->color);
    }
    if (n->son[0] || n->son[1]) {
      BaseNode *s = n->son[0] ? n->son[0] : n->son[1];
      s->father = n->father;
      if (n == n->father->son[0]) {
        n->father->son[0] = s;
      } else {
        n->father->son[1] = s;
      }
      s->color = black;
      delete static_cast<Node*>(n);
      return;
    }
    if (n->father == &_header || n->color == red) {
      n->father->son[n == n->father->son[1]] = nullptr;
      delete static_cast<Node*>(n);
      return;
    }
    assert(n->son[0] == nullptr && n->son[1] == nullptr);
    BaseNode *p = n->father;
    bool n_dir = (n == p->son[1]);
    p->son[n_dir] = nullptr;
    while (p && is_black(n)) {
      BaseNode *s = p->son[!n_dir];
      if (is_red(s)) {
        p->color = red;
        s->color = black;
        rotate(p, n_dir);
        s = p->son[!n_dir];
      }
      BaseNode *c = s->son[n_dir];
      BaseNode *d = s->son[!n_dir];
      if (is_black(s) && is_black(c) && is_black(d)) {
        s->color = red;
        n = p;
      } else if (is_black(d)){
        c->color = black;
        s->color = red;
        rotate(s, !n_dir);
        s = p->son[!n_dir];
        c = s->son[n_dir];
        d = s->son[!n_dir];
      }
      if (is_black(s) && is_red(d)) {
        s->color = p->color;
        p->color = d->color = black;
        rotate(p, n_dir);
        n = _header.son[0];
      }
      p = n->father;
      if (p == &_header) break;
      n_dir = n == p->son[1];
    }
    n->color = black;
    delete static_cast<Node*>(pos.ptr);
  }

  /**
   * Returns the number of elements with key
   *   that compares equivalent to the specified argument,
   *   which is either 1 or 0
   *     since this container does not allow duplicates.
   * The default method of check the equivalence is !(a < b || b > a)
   */
  size_t count(const Key &key) const {
    _const_find_result res = _find(key);
    return res.node == nullptr ? 0 : 1;
  }

  /**
   * Finds an element with key equivalent to key.
   * key value of the element to search for.
   * Iterator to an element with key equivalent to key.
   *   If no such element is found, past-the-end (see end()) iterator is returned.
   */
  iterator find(const Key &key) {
    _find_result res = _find(key);
    if (res.node == nullptr) {
      return end();
    }
    return iterator(this, res.node);
  }

  const_iterator find(const Key &key) const {
    _const_find_result res = _find(key);
    if (res.node == nullptr) {
      return cend();
    }
    return const_iterator(this, res.node);
  }
};

}

#endif
