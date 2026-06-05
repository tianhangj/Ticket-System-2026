#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

namespace sjtu
{
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector
{
private:
	T* _data;
	size_t _size;
	size_t _capacity;

	size_t _expand_capacity(size_t capacity) {
		return capacity == 0 ? 16 : capacity * 2;
	}
	T* alloc_memory(size_t capacity) {
		char* _ptr = (char*)malloc(capacity * sizeof(T));
		for (size_t i = 0; i < capacity * sizeof(T); ++i) {
			_ptr[i] = 0;
		}
		return (T*)_ptr;
	}
	void _expend_capacity() {
		_capacity = _expand_capacity(_capacity);
		T* _new_data = alloc_memory( _capacity);
		for (size_t i = 0; i < _size; ++i) {
			new (_new_data+i) T(_data[i]);
			_data[i].~T();
		}
		free(_data);
		_data = _new_data;
	}
public:
	/**
	 * TODO
	 * a type for actions of the elements of a vector, and you should write
	 *   a class named const_iterator with same interfaces.
	 */
	/**
	 * you can see RandomAccessIterator at CppReference for help.
	 */
	class const_iterator;
	class iterator
	{
	// The following code is written for the C++ type_traits library.
	// Type traits is a C++ feature for describing certain properties of a type.
	// For instance, for an iterator, iterator::value_type is the type that the
	// iterator points to.
	// STL algorithms and containers may use these type_traits (e.g. the following
	// typedef) to work properly. In particular, without the following code,
	// @code{std::sort(iter, iter1);} would not compile.
	// See these websites for more information:
	// https://en.cppreference.com/w/cpp/header/type_traits
	// About value_type: https://blog.csdn.net/u014299153/article/details/72419713
	// About iterator_category: https://en.cppreference.com/w/cpp/iterator
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::output_iterator_tag;

	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
		vector<T>* vec;
		size_t idx;
	public:
		friend class vector<T>;
		iterator(vector<T>* _vec, size_t _idx) {
			vec = _vec;
			idx = _idx;
		}
		/**
		 * return a new iterator which pointer n-next elements
		 * as well as operator-
		 */
		iterator operator+(const int &n) const
		{
			//TODO
			return iterator(vec, idx + n);
		}
		iterator operator-(const int &n) const
		{
			//TODO
			return iterator(vec, idx - n);
		}
		// return the distance between two iterators,
		// if these two iterators point to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const
		{
			//TODO
			if (vec == rhs.vec) {
				return idx - rhs.idx;
			}
			throw invalid_iterator();
		}
		iterator& operator+=(const int &n)
		{
			//TODO
			idx += n;
			return *this;
		}
		iterator& operator-=(const int &n)
		{
			//TODO
			idx -= n;
			return *this;
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
			iterator res(vec, idx);
			++idx;
			return res;
		}
		/**
		 * TODO ++iter
		 */
		iterator& operator++() {
			++idx;
			return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
			iterator res(vec, idx);
			--idx;
			return res;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--() {
			--idx;
			return *this;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const{
			return vec->at(idx);
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory address).
		 */
		bool operator==(const iterator &rhs) const {
			return vec == rhs.vec && idx == rhs.idx;
		}
		bool operator==(const const_iterator &rhs) const {
			return vec == rhs.vec && idx == rhs.idx;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return vec != rhs.vec || idx != rhs.idx;
		}
		bool operator!=(const const_iterator &rhs) const {
			return vec != rhs.vec || idx != rhs.idx;
		}
	};
	/**
	 * TODO
	 * has same function as iterator, just for a const object.
	 */
	class const_iterator
	{
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::output_iterator_tag;

	private:
		using const_vector = const vector<T>;
		/*TODO*/
		const_vector* vec;
		size_t idx;
	public:
		const_iterator(const_vector* _vec, size_t _idx) {
			vec = _vec;
			idx = _idx;
		}
		const_iterator(iterator rhs) {
			vec = rhs.vec;
			idx = rhs.idx;
		}
		/**
		 * return a new iterator which pointer n-next elements
		 * as well as operator-
		 */
		const_iterator operator+(const int &n) const
		{
			//TODO
			return const_iterator(vec, idx + n);
		}
		const_iterator operator-(const int &n) const
		{
			//TODO
			return const_iterator(vec, idx - n);
		}
		// return the distance between two iterators,
		// if these two iterators point to different vectors, throw invaild_iterator.
		int operator-(const const_iterator &rhs) const
		{
			//TODO
			if (vec == rhs.vec) {
				return idx - rhs.idx;
			}
			throw invalid_iterator();
		}
		const_iterator& operator+=(const int &n)
		{
			//TODO
			idx += n;
			return *this;
		}
		const_iterator& operator-=(const int &n)
		{
			//TODO
			idx -= n;
			return *this;
		}
		/**
		 * TODO iter++
		 */
		const_iterator operator++(int) {
			const_iterator res(vec, idx);
			++idx;
			return res;
		}
		/**
		 * TODO ++iter
		 */
		const_iterator& operator++() {
			++idx;
			return *this;
		}
		/**
		 * TODO iter--
		 */
		const_iterator operator--(int) {
			const_iterator res(vec, idx);
			--idx;
			return res;
		}
		/**
		 * TODO --iter
		 */
		const_iterator& operator--() {
			--idx;
			return *this;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const{
			return vec->_data[idx];
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory address).
		 */
		bool operator==(const iterator &rhs) const {
			return vec == rhs.vec && idx == rhs.idx;
		}
		bool operator==(const const_iterator &rhs) const {
			return vec == rhs.vec && idx == rhs.idx;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return vec != rhs.vec || idx != rhs.idx;
		}
		bool operator!=(const const_iterator &rhs) const {
			return vec != rhs.vec || idx != rhs.idx;
		}

	};
	/**
	 * TODO Constructs
	 * At least two: default constructor, copy constructor
	 */
	vector() : _data(nullptr), _size(0), _capacity(0)  {}
	vector(const vector &other) {
		_data = alloc_memory(other._capacity * sizeof(T));
		_size = other._size;
		_capacity = other._capacity;
		for (size_t i = 0; i < _size; ++i) {
			new (_data + i) T(other._data[i]);
		}
	}
	/**
	 * TODO Destructor
	 */
	~vector() {
		for (size_t i = 0; i < _size; ++i) {
			_data[i].~T();
		}
		free(_data);
	}
	/**
	 * TODO Assignment operator
	 */
	vector &operator=(const vector &other) {
		if (this == &other) {
			return *this;
		}
		if ( _capacity < other._size ) {
			for (size_t i = 0; i < _size; ++i) {
				_data[i].~T();
			}
			free(_data);
			while (_capacity < other._size ) {
				_capacity = _expand_capacity(_capacity);
			}
			_data = alloc_memory(_capacity * sizeof(T));
			_size = 0;
		}
		for (size_t i = 0; i < std::max(_size, other._size); ++i) {
			if (i < _size && i < other._size) {
				_data[i] = other._data[i];
			} else if (i < other._size) {
				new (_data + i) T(other._data[i]);
			} else {
				_data[i].~T();
			}
		}
		_size = other._size;
		return *this;
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 */
	T & at(const size_t &pos) {
		if ( pos > _size ) {
			throw index_out_of_bound();
		}
		return _data[pos];
	}
	const T & at(const size_t &pos) const {
		if ( pos > _size ) {
			throw index_out_of_bound();
		}
		return _data[pos];
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 * !!! Pay attentions
	 *   In STL this operator does not check the boundary but I want you to do.
	 */
	T & operator[](const size_t &pos) {
		if ( pos > _size ) {
			throw index_out_of_bound();
		}
		return _data[pos];
	}
	const T & operator[](const size_t &pos) const {
		if ( pos > _size ) {
			throw index_out_of_bound();
		}
		return _data[pos];
	}
	/**
	 * access the first element.
	 * throw container_is_empty if size == 0
	 */
	const T & front() const {
		if (_size == 0) {
			throw container_is_empty();
		}
		return _data[0];
	}
	/**
	 * access the last element.
	 * throw container_is_empty if size == 0
	 */
	const T & back() const {
		if (_size == 0) {
			throw container_is_empty();
		}
		return _data[_size-1];
	}
	T & back() {
		if (_size == 0) {
			throw container_is_empty();
		}
		return _data[_size-1];
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() {
		return iterator(this, 0);
	}
	const_iterator begin() const {
		return const_iterator(this, 0);
	}
	const_iterator cbegin() const {
		return const_iterator(this, 0);
	}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() {
		return iterator(this, _size);
	}
	const_iterator end() const {
		return const_iterator(this, _size);
	}
	const_iterator cend() const {
		return const_iterator(this, _size);
	}
	/**
	 * checks whether the container is empty
	 */
	bool empty() const {
		return _size == 0;
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const {
		return _size;
	}
	/**
	 * clears the contents
	 */
	void clear() {
		for (size_t i = 0; i < _size; ++i) {
			_data[i].~T();
		}
		_size = 0;
	}
	/**
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value.
	 */
	iterator insert(iterator pos, const T &value) {
		if (_size == _capacity) {
			_expend_capacity();
		}
		for (size_t i = _size; i > pos.idx; --i) {
			new (_data + i) T(_data[i-1]);
			_data[i-1].~T();
		}
		new (_data + pos.idx) T(value);
		++_size;
		return iterator(this, pos.idx);
	}
	/**
	 * inserts value at index ind.
	 * after inserting, this->at(ind) == value
	 * returns an iterator pointing to the inserted value.
	 * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
	 */
	iterator insert(const size_t &ind, const T &value) {
		if (ind > _size) {
			throw index_out_of_bound();
		}
		if (_size == _capacity) {
			_expend_capacity();
		}
		for (size_t i = _size; i > ind; --i) {
			new (_data + i) T(_data[i-1]);
			_data[i-1].~T();
		}
		new (_data + ind) T(value);
		++_size;
		return iterator(this, ind);
	}
	/**
	 * removes the element at pos.
	 * return an iterator pointing to the following element.
	 * If the iterator pos refers the last element, the end() iterator is returned.
	 */
	iterator erase(iterator pos) {
		for (size_t i = pos.idx; i < _size - 1; ++i) {
			_data[i] = _data[i+1];
		}
		_data[_size-1].~T();
		--_size;
		return iterator(this, pos.idx);
	}
	/**
	 * removes the element with index ind.
	 * return an iterator pointing to the following element.
	 * throw index_out_of_bound if ind >= size
	 */
	iterator erase(const size_t &ind) {
		if (ind >= _size) {
			throw index_out_of_bound();
		}
		for (size_t i = ind; i < _size - 1; ++i) {
			_data[i] = _data[i+1];
		}
		_data[_size-1].~T();
		--_size;
		return iterator(this, ind);
	}
	/**
	 * adds an element to the end.
	 */
	void push_back(const T &value) {
		if (_size == _capacity) {
			_expend_capacity();
		}
		new (_data + _size) T(value);
		++_size;
	}

	template <class... Args>
	void emplace_back(Args&&... args) {
		if (_size == _capacity) {
			_expend_capacity();
		}
		new (_data + _size) T(args...);
		++_size;
	}
	/**
	 * remove the last element from the end.
	 * throw container_is_empty if size() == 0
	 */
	void pop_back() {
		if (_size == 0) {
			throw container_is_empty();
		}
		_data[_size-1].~T();
		--_size;
	}
};


}

#endif
