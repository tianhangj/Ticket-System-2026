#ifndef BPT_MEMORYRIVER_HPP
#define BPT_MEMORYRIVER_HPP

#include <fstream>
#include <cassert>
#include <iostream>
#include <filesystem>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

template<class T>
class MemoryRiver {
private:
    /* your code here */
    string filename;
    fstream file;
    int sizeofT = sizeof(T);
    int alloc_index = 0;
public:
    void initialize(string filename_) {
        filename = filename_;
        if (!std::filesystem::exists(filename)) {
            file.open(filename, std::ios::out | std::ios::binary);
            file.close();
        }
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    }
    void clear() {
        file.close();
        file.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);
        file.close();
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    }
    void close() {
        file.close();
    }
    int insert(T &t) {
        file.seekp(0, std::ios::end);
        int pos = file.tellp();
        file.write(reinterpret_cast<char*>(&t), sizeofT);
        return pos;
    }
    void update(T &t, const int pos) {
        file.seekp(pos, std::ios::beg);
        file.write(reinterpret_cast<char*>(&t), sizeofT);
    }
    void read(T &t, const int pos) {
        file.seekg(pos, std::ios::beg);
        file.read(reinterpret_cast<char*>(&t), sizeofT);
    }
};


#endif //BPT_MEMORYRIVER_HPP