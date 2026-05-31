#ifndef _BPT_HPP_
#define _BPT_HPP_

#include <ios>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <filesystem>

#include <vector>
#include <cassert>

template <class K, class V>
class BPT {
public:
    const static size_t M = 20;
private:
    std::fstream file;
    const static size_t BUFFER_SIZE = 4096;
    char *buf;
    template<class T>
    void read(T &data, size_t pos) {
        file.seekg(pos);
        file.read(reinterpret_cast<char*>(&data), sizeof(T));
    }
    template<class T>
    void write(const T &data, size_t pos) {
        file.seekp(pos);
        file.write(reinterpret_cast<const char*>(&data), sizeof(T));
    }
    template<class T>
    size_t new_block(const T &data) {
        size_t pos = file.seekp(0, std::ios::end).tellp();
        write(data, pos);
        return pos;
    }
    struct KV {
        K key;
        V value;
        KV() : key(), value() {}
        KV(const K &k, const V &v): key(k), value(v) {}
        const bool operator < (const KV &other) const {
            if (key == other.key) {
                return value < other.value;
            }
            return key < other.key;
        }
        const bool operator == (const KV &other) const {
            return key == other.key && value == other.value;
        }
        const bool operator != (const KV &other) const {
            return key != other.key || value != other.value;
        }
    };
    struct Metadata {
        size_t root;
    } metadata;
    struct Node {
        bool is_leaf;
        size_t size;
        KV val[M];
        size_t child[M];
        size_t next;
        Node() {
            is_leaf = false;
            size = 0;
            next = 0;
        }
    };
    int lower_bound(const Node& node, K key) {
        if (node.size == 0) {
            return 0;
        }
        int l = 0, r = node.size - 1;
        int ret = node.size;
        while (l <= r) {
            int mid = (l+r)/2;
            if (node.val[mid].key < key) {
                l = mid + 1;
            } else {
                ret = mid;
                r = mid - 1;
            }
        }
        return ret;
    }
    int lower_bound(const Node& node, KV val) {
        if (node.size == 0) {
            return 0;
        }
        int l = 0, r = node.size - 1;
        int ret = node.size;
        while (l <= r) {
            int mid = (l+r)/2;
            if (node.val[mid] < val) {
                l = mid + 1;
            } else {
                ret = mid;
                r = mid - 1;
            }
        }
        return ret;
    }
    void _insert(Node &node, KV val, size_t child_pos) {
        int pos = lower_bound(node, val);
        for (int i = node.size; i > pos; --i) {
            node.val[i] = node.val[i-1];
            node.child[i] = node.child[i-1];
        }
        node.val[pos] = val;
        node.child[pos] = child_pos;
        node.size++;
    }
public:
    BPT(const char* filename) {
        buf = static_cast<char*>(malloc(BUFFER_SIZE));
        file.rdbuf()->pubsetbuf(buf, BUFFER_SIZE);
        if (std::filesystem::exists(filename)) {
            file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
            read(metadata, 0);
        } else {
            file.open(filename, std::ios::out | std::ios::binary);
            metadata.root = sizeof(Metadata);
            write(metadata, 0);
            Node root;
            root.is_leaf = true;
            root.size = 0;
            write(root, metadata.root);
            file.close();
            file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        }
    }
    ~BPT() {
        file.close();
        free(buf);
    }
    std::vector<V> find(const K &key) {
        std::vector<V> result;
        Node node;
        read(node, metadata.root);
        if (node.size == 0) {
            return result;
        }
        while (true) {
            assert(node.size);
            int idx = lower_bound(node, key);
            if (node.is_leaf) {
                while (true) {
                    for (int i = idx; i < node.size && node.val[i].key == key; ++i) {
                        result.push_back(node.val[i].value);
                    }
                    if (node.val[node.size-1].key != key || node.next == 0) {
                        break;
                    }
                    read(node, node.next);
                    idx = 0;
                }
                break;
            } else {
                if (idx >= node.size) {
                    idx--;
                }
                read(node, node.child[idx]);
            }
        }
        return result;
    }
    void insert(const K &key, const V &value) {
        std::vector<Node>path;
        std::vector<size_t>idx;
        std::vector<size_t>pos;
        path.emplace_back();
        read(path.back(), metadata.root);
        pos.emplace_back(metadata.root);
        KV kv(key, value);
        while (!path.back().is_leaf) {
            int _idx = lower_bound(path.back(), kv);
            if (_idx >= path.back().size) {
                _idx--;
            }
            idx.emplace_back(_idx);
            path.emplace_back();
            pos.emplace_back(path[path.size()-2].child[_idx]);
            read(path.back(), path[path.size()-2].child[_idx]);
        }
        bool to_ins = true;
        KV ins_val(key, value);
        size_t ins_pos = 0;
        for (int i = path.size() - 1; i >= 1 && to_ins; --i) {
            if (path[i].size < M) {
                _insert(path[i], ins_val, ins_pos);
                write(path[i], pos[i]);
                for (int j = i; j >= 1; --j) {
                    if (path[j].val[path[j].size-1] != path[j-1].val[idx[j-1]]) {
                        path[j-1].val[idx[j-1]] = path[j].val[path[j].size-1];
                        write(path[j-1], pos[j-1]);
                    } else {
                        break;
                    }
                }
                to_ins = false;
                break;
            } else {
                // split
                Node new_node;
                new_node.is_leaf = path[i].is_leaf;
                new_node.size = M / 2;
                for (int j = 0; j < new_node.size; ++j) {
                    new_node.val[j] = path[i].val[j + M / 2];
                    new_node.child[j] = path[i].child[j + M / 2];
                }
                path[i].size = M / 2;
                if (ins_val < path[i].val[path[i].size-1]) {
                    _insert(path[i], ins_val, ins_pos);
                } else {
                    _insert(new_node, ins_val, ins_pos);
                }
                new_node.next = path[i].next;
                path[i].next = new_block(new_node);
                write(path[i], pos[i]);
                path[i-1].val[idx[i-1]] = path[i].val[path[i].size-1];
                ins_val = new_node.val[new_node.size-1];
                ins_pos = path[i].next;
            }
        }
        if (to_ins) {
            if (path[0].size < M) {
                _insert(path[0], ins_val, ins_pos);
                write(path[0], pos[0]);
            } else {
                // split
                Node new_node;
                new_node.is_leaf = path[0].is_leaf;
                new_node.size = M / 2;
                for (int j = 0; j < new_node.size; ++j) {
                    new_node.val[j] = path[0].val[j + M / 2];
                    new_node.child[j] = path[0].child[j + M / 2];
                }
                path[0].size = M / 2;
                if (ins_val < path[0].val[path[0].size-1]) {
                    _insert(path[0], ins_val, ins_pos);
                } else {
                    _insert(new_node, ins_val, ins_pos);
                }
                Node new_root;
                new_root.is_leaf = false;
                new_root.size = 2;
                new_root.val[0] = path[0].val[path[0].size-1];
                new_root.child[0] = pos[0];
                new_root.val[1] = new_node.val[new_node.size-1];
                new_root.child[1] = new_block(new_node);
                if (path[0].is_leaf) {
                    path[0].next = new_root.child[1];
                }
                metadata.root = new_block(new_root);
                write(path[0], pos[0]);
                write(metadata, 0);
            }
        }
    }
    void erase(const K &key, const V &value) {
        std::vector<Node>path;
        std::vector<size_t>idx;
        std::vector<size_t>pos;
        path.emplace_back();
        read(path.back(), metadata.root);
        pos.emplace_back(metadata.root);
        KV kv(key, value);
        while (!path.back().is_leaf) {
            int _idx = lower_bound(path.back(), kv);
            if (_idx >= path.back().size) {
                _idx--;
            }
            idx.emplace_back(_idx);
            path.emplace_back();
            pos.emplace_back(path[path.size()-2].child[_idx]);
            read(path.back(), path[path.size()-2].child[_idx]);
        }
        int _idx = lower_bound(path.back(), kv);
        if (_idx == path.back().size || path.back().val[_idx] != kv) {
            return;
        }
        bool to_erase = true;
        for (int i = path.size()-1; i >= 1 && to_erase; --i) {
            if (path[i].size > M / 2) {
                // std::cerr << "erase from node with size > M/2\n";
                for (int j = _idx; j < path[i].size - 1; ++j) {
                    path[i].val[j] = path[i].val[j+1];
                    path[i].child[j] = path[i].child[j+1];
                }
                path[i].size--;
                write(path[i], pos[i]);
                for (int j = i; j >= 1; --j) {
                    if (path[j].val[path[j].size-1] != path[j-1].val[idx[j-1]]) {
                        path[j-1].val[idx[j-1]] = path[j].val[path[j].size-1];
                        write(path[j-1], pos[j-1]);
                    } else {
                        break;
                    }
                }
                to_erase = false;
                break;
            } else {
                for (int j = _idx; j < path[i].size - 1; ++j) {
                    path[i].val[j] = path[i].val[j+1];
                    path[i].child[j] = path[i].child[j+1];
                }
                --path[i].size;
                // borrow
                Node sibling[2];
                if (idx[i-1] > 0) {
                    read(sibling[0], path[i-1].child[idx[i-1]-1]);
                    if (sibling[0].size > M / 2) {
                        // std::cerr << "borrow from left sibling\n";
                        for (int j = path[i].size; j > 0; --j) {
                            path[i].val[j] = path[i].val[j-1];
                            path[i].child[j] = path[i].child[j-1];
                        }
                        path[i].val[0] = sibling[0].val[sibling[0].size-1];
                        path[i].child[0] = sibling[0].child[sibling[0].size-1];
                        path[i].size++;
                        sibling[0].size--;
                        write(path[i], pos[i]);
                        write(sibling[0], path[i-1].child[idx[i-1]-1]);
                        path[i-1].val[idx[i-1]-1] = sibling[0].val[sibling[0].size-1];
                        path[i-1].val[idx[i-1]] = path[i].val[path[i].size-1];
                        write(path[i-1], pos[i-1]);
                        for (int j = i-1; j >= 1; --j) {
                            if (path[j].val[path[j].size-1] != path[j-1].val[idx[j-1]]) {
                                path[j-1].val[idx[j-1]] = path[j].val[path[j].size-1];
                                write(path[j-1], pos[j-1]);
                            } else {
                                break;
                            }
                        }
                        to_erase = false;
                        break;
                    }
                }
                if (idx[i-1] < path[i-1].size - 1) {
                    read(sibling[1], path[i-1].child[idx[i-1]+1]);
                    if (sibling[1].size > M / 2) {
                        // std::cerr << "borrow from right sibling\n";
                        path[i].val[path[i].size] = sibling[1].val[0];
                        path[i].child[path[i].size] = sibling[1].child[0];
                        for (int j = 1; j < sibling[1].size; ++j) {
                            sibling[1].val[j-1] = sibling[1].val[j];
                            sibling[1].child[j-1] = sibling[1].child[j];
                        }
                        path[i].size++;
                        sibling[1].size--;
                        write(path[i], pos[i]);
                        write(sibling[1], path[i-1].child[idx[i-1]+1]);
                        path[i-1].val[idx[i-1]] = path[i].val[path[i].size-1];
                        write(path[i-1], pos[i-1]);
                        for (int j = i-1; j >= 1; --j) {
                            if (path[j].val[path[j].size-1] != path[j-1].val[idx[j-1]]) {
                                path[j-1].val[idx[j-1]] = path[j].val[path[j].size-1];
                                write(path[j-1], pos[j-1]);
                            } else {
                                break;
                            }
                        }
                        to_erase = false;
                        break;
                    }
                }
                // merge
                if (idx[i-1] > 0) {
                    // std::cerr << "merge with left sibling\n";
                    for (int j = 0; j < path[i].size; ++j) {
                        sibling[0].val[sibling[0].size + j] = path[i].val[j];
                        sibling[0].child[sibling[0].size + j] = path[i].child[j];
                    }
                    sibling[0].next = path[i].next;
                    sibling[0].size += path[i].size;
                    write(sibling[0], path[i-1].child[idx[i-1]-1]);
                    path[i-1].val[idx[i-1]-1] = sibling[0].val[sibling[0].size-1];
                    _idx = idx[i-1];
                    to_erase = true;
                    continue;
                } else {
                    // std::cerr << "merge with right sibling\n";
                    for (int j = 0; j < sibling[1].size; ++j) {
                        path[i].val[path[i].size + j] = sibling[1].val[j];
                        path[i].child[path[i].size + j] = sibling[1].child[j];
                    }
                    path[i].next = sibling[1].next;
                    path[i].size += sibling[1].size;
                    write(path[i], pos[i]);
                    path[i-1].val[idx[i-1]] = path[i].val[path[i].size-1];
                    _idx = idx[i-1] + 1;
                    to_erase = true;
                    continue;
                }
            }
        }
        if (to_erase) {
            // std::cerr << "erase from root\n";
            for (int j = _idx; j < path[0].size - 1; ++j) {
                path[0].val[j] = path[0].val[j+1];
                path[0].child[j] = path[0].child[j+1];
            }
            path[0].size--;
            if (path[0].size == 1 && !path[0].is_leaf) {
                metadata.root = path[0].child[0];
                write(metadata, 0);
            } else {
                write(path[0], metadata.root);
            }
        }
    }
};
#endif