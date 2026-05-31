#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu { 

template<class T>
class deque {
private:
	static const size_t CHUNK_SIZE = 16;
	T** _chunks;
	size_t _chunk_count;
	// [start_pos, end_pos)
	size_t start_pos;
	size_t end_pos;
	
	void _expand_capacity() {
		size_t new_chunk_count = _chunk_count * 2;
		T** new_chunks = new T*[new_chunk_count];
		for (size_t i = 0; i < new_chunk_count; ++i) {
			if ( i < _chunk_count / 2 || i >= _chunk_count / 2 + _chunk_count ) {
				new_chunks[i] = (T*)malloc(sizeof(T) * CHUNK_SIZE);
			} else {
				new_chunks[i] = _chunks[i - _chunk_count / 2];
			}
		}
		delete[] _chunks;
		_chunks = new_chunks;
		start_pos += _chunk_count * CHUNK_SIZE / 2;
		end_pos += _chunk_count * CHUNK_SIZE / 2;
		_chunk_count = new_chunk_count;
	}
public:
	class const_iterator;
	class iterator {
	private:
		friend class deque<T>;
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
		deque<T> *deq;
		size_t pos; // offset from the head of the capacity
	public:
		iterator() : deq(nullptr), pos(0) {}
		iterator(deque<T> *deq, size_t pos) : deq(deq), pos(pos) {}
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, the behaviour is **undefined**.
		 * as well as operator-
		 */
		iterator operator+(const int &n) const {
			//TODO
			return iterator(deq, pos + n);
		}
		iterator operator-(const int &n) const {
			//TODO
			return iterator(deq, pos - n);
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const {
			//TODO
			if (deq != rhs.deq) {
				throw invalid_iterator();
			}
			return pos - rhs.pos;
		}
		iterator operator+=(const int &n) {
			//TODO
			pos += n;
			return *this;
		}
		iterator operator-=(const int &n) {
			//TODO
			pos -= n;
			return *this;
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
			iterator tmp = *this;
			++pos;
			return tmp;
		}
		/**
		 * TODO ++iter
		 */
		iterator& operator++() {
			++pos;
			return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
			iterator tmp = *this;
			--pos;
			return tmp;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--() {
			--pos;
			return *this;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const {
			if (deq == nullptr || pos < deq->start_pos || pos >= deq->end_pos) {
				throw invalid_iterator();
			}
			return deq->_chunks[pos / CHUNK_SIZE][pos % CHUNK_SIZE];
		}
		/**
		 * TODO it->field
		 */
		T* operator->() const noexcept {
			if (deq == nullptr || pos < deq->start_pos || pos >= deq->end_pos) {
				throw invalid_iterator();
			}
			return (deq->_chunks[pos / CHUNK_SIZE]) + pos % CHUNK_SIZE;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {
			return deq == rhs.deq && pos == rhs.pos;
		}
		bool operator==(const const_iterator &rhs) const {
			return deq == rhs.deq && pos == rhs.pos;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return deq != rhs.deq || pos != rhs.pos;
		}
		bool operator!=(const const_iterator &rhs) const {
			return deq != rhs.deq || pos != rhs.pos;
		}
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		private:
			typedef const deque<T> const_deque;
			// data members.
			const_deque* deq;
			size_t pos; // offset from the head of the capacity
		public:
			const_iterator() : deq(nullptr), pos(0) {}
			const_iterator(const_deque *deq, size_t pos) : deq(deq), pos(pos) {}
			const_iterator(const iterator &other) {
				// TODO
				deq = other.deq;
				pos = other.pos;
			}
			const_iterator operator+(const int &n) const {
				//TODO
				return const_iterator(deq, pos + n);
			}
			const_iterator operator-(const int &n) const {
				//TODO
				return const_iterator(deq, pos - n);
			}
			int operator-(const const_iterator &rhs) const {
				//TODO
				if (deq != rhs.deq) {
					throw invalid_iterator();
				}
				return pos - rhs.pos;
			}
			const_iterator operator+=(const int &n) {
				//TODO
				pos += n;
				return *this;
			}
			const_iterator operator-=(const int &n) {
				//TODO
				pos -= n;
				return *this;
			}
			const_iterator operator++(int) {
				const_iterator tmp = *this;
				++pos;
				return tmp;
			}
			const_iterator& operator++() {
				++pos;
				return *this;
			}
			const_iterator operator--(int) {
				const_iterator tmp = *this;
				--pos;
				return tmp;
			}
			const_iterator& operator--() {
				--pos;
				return *this;
			}
			const T& operator*() const {
				if (deq == nullptr || pos < deq->start_pos || pos >= deq->end_pos) {
					throw invalid_iterator();
				}
				return deq->_chunks[pos / CHUNK_SIZE][pos % CHUNK_SIZE];
			}
			const T* operator->() const noexcept {
				if (deq == nullptr || pos < deq->start_pos || pos >= deq->end_pos) {
					throw invalid_iterator();
				}
				return (deq->_chunks[pos / CHUNK_SIZE]) + pos % CHUNK_SIZE;
			}
			bool operator==(const iterator &rhs) const {
				return deq == rhs.deq && pos == rhs.pos;
			}
			bool operator==(const const_iterator &rhs) const {
				return deq == rhs.deq && pos == rhs.pos;
			}
			bool operator!=(const iterator &rhs) const {
				return deq != rhs.deq || pos != rhs.pos;
			}
			bool operator!=(const const_iterator &rhs) const {
				return deq != rhs.deq || pos != rhs.pos;
			}
	};
	/**
	 * TODO Constructors
	 */
	deque() {
		_chunk_count = 2;
		_chunks = new T*[2];
		_chunks[0] = (T*)malloc(sizeof(T) * CHUNK_SIZE);
		_chunks[1] = (T*)malloc(sizeof(T) * CHUNK_SIZE);
		start_pos = CHUNK_SIZE;
		end_pos = CHUNK_SIZE;
	}
	deque(const deque &other) {
		_chunk_count = other._chunk_count;
		start_pos = other.start_pos;
		end_pos = other.end_pos;
		_chunks = new T*[other._chunk_count];
		for (size_t i = 0; i < _chunk_count; ++i) {
			_chunks[i] = (T*)malloc(sizeof(T) * CHUNK_SIZE);
		}
		for (size_t i = start_pos; i < end_pos; ++i) {
			new (_chunks[i / CHUNK_SIZE] + i % CHUNK_SIZE) T(other._chunks[i / CHUNK_SIZE][i % CHUNK_SIZE]);
		}
	}
	/**
	 * TODO Deconstructor
	 */
	~deque() {
		for (size_t i = start_pos; i < end_pos; ++i) {
			_chunks[i / CHUNK_SIZE][i % CHUNK_SIZE].~T();
		}
		for (size_t i = 0; i < _chunk_count; ++i) {
			free(_chunks[i]);
		}
		delete[] _chunks;
	}
	/**
	 * TODO assignment operator
	 */
	deque &operator=(const deque &other) {
		if (this == &other) {
			return *this;
		}
		for (size_t i = start_pos; i < end_pos; ++i) {
			_chunks[i / CHUNK_SIZE][i % CHUNK_SIZE].~T();
		}
		for (size_t i = 0; i < _chunk_count; ++i) {
			free(_chunks[i]);
		}
		delete[] _chunks;
		_chunk_count = other._chunk_count;
		start_pos = other.start_pos;
		end_pos = other.end_pos;
		_chunks = new T*[other._chunk_count];
		for (size_t i = 0; i < _chunk_count; ++i) {
			_chunks[i] = (T*)malloc(sizeof(T) * CHUNK_SIZE);
		}
		for (size_t i = start_pos; i < end_pos; ++i) {
			new (_chunks[i / CHUNK_SIZE] + i % CHUNK_SIZE) T(other._chunks[i / CHUNK_SIZE][i % CHUNK_SIZE]);
		}
		return *this;
	}
	/**
	 * access specified element with bounds checking
	 * throw index_out_of_bound if out of bound.
	 */
	T & at(const size_t &pos) {
		if (start_pos + pos >= end_pos || start_pos + pos < start_pos) {
			throw index_out_of_bound();
		}
		return _chunks[(start_pos + pos) / CHUNK_SIZE][(start_pos + pos) % CHUNK_SIZE];
	}
	const T & at(const size_t &pos) const {
		if (start_pos + pos >= end_pos || start_pos + pos < start_pos) {
			throw index_out_of_bound();
		}
		return _chunks[(start_pos + pos) / CHUNK_SIZE][(start_pos + pos) % CHUNK_SIZE];
	}
	T & operator[](const size_t &pos) {
		return at(pos);
	}
	const T & operator[](const size_t &pos) const {
		return at(pos);
	}
	/**
	 * access the first element
	 * throw container_is_empty when the container is empty.
	 */
	const T & front() const {
		if (empty()) {
			throw container_is_empty();
		}
		return _chunks[start_pos / CHUNK_SIZE][start_pos % CHUNK_SIZE];
	}
	/**
	 * access the last element
	 * throw container_is_empty when the container is empty.
	 */
	const T & back() const {
		if (empty()) {
			throw container_is_empty();
		}
		return _chunks[(end_pos - 1) / CHUNK_SIZE][(end_pos - 1) % CHUNK_SIZE];
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() {
		return iterator(this, start_pos);
	}
	const_iterator cbegin() const {
		return const_iterator(this, start_pos);
	}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() {
		return iterator(this, end_pos);
	}
	const_iterator cend() const {
		return const_iterator(this, end_pos);
	}
	/**
	 * checks whether the container is empty.
	 */
	bool empty() const {
		return start_pos == end_pos;
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const {
		return end_pos - start_pos;
	}
	/**
	 * clears the contents
	 */
	void clear() {
		for (size_t i = start_pos; i < end_pos; ++i) {
			_chunks[i / CHUNK_SIZE][i % CHUNK_SIZE].~T();
		}
		start_pos = _chunk_count * CHUNK_SIZE / 2;
		end_pos = _chunk_count * CHUNK_SIZE / 2;
	}
	/**
	 * inserts elements at the specified locat on in the container.
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value
	 *     throw if the iterator is invalid or it point to a wrong place.
	 */
	iterator insert(iterator pos, const T &value) {
		if (pos.deq != this || pos.pos < start_pos || pos.pos > end_pos) {
			throw invalid_iterator();
		}
		size_t insert_pos = pos.pos;
		if (end_pos == _chunk_count * CHUNK_SIZE) {
			_expand_capacity();
			insert_pos += _chunk_count * CHUNK_SIZE / 4;
		}
		for (size_t i = end_pos; i > insert_pos; --i) {
			new (_chunks[i / CHUNK_SIZE] + i % CHUNK_SIZE) T(_chunks[(i - 1) / CHUNK_SIZE][(i - 1) % CHUNK_SIZE]);
			_chunks[(i - 1) / CHUNK_SIZE][(i - 1) % CHUNK_SIZE].~T();
		}
		new (_chunks[insert_pos / CHUNK_SIZE] + insert_pos % CHUNK_SIZE) T(value);
		++end_pos;
		return iterator(this, insert_pos);
	}
	/**
	 * removes specified element at pos.
	 * removes the element at pos.
	 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
	 */
	iterator erase(iterator pos) {
		if (pos.deq != this || pos.pos < start_pos || pos.pos >= end_pos) {
			throw invalid_iterator();
		}
		_chunks[pos.pos / CHUNK_SIZE][pos.pos % CHUNK_SIZE].~T();
		for (size_t i = pos.pos; i < end_pos - 1; ++i) {
			new (_chunks[i / CHUNK_SIZE] + i % CHUNK_SIZE) T(_chunks[(i + 1) / CHUNK_SIZE][(i + 1) % CHUNK_SIZE]);
			_chunks[(i + 1) / CHUNK_SIZE][(i + 1) % CHUNK_SIZE].~T();
		}
		--end_pos;
		return pos;
	}
	/**
	 * adds an element to the end
	 */
	void push_back(const T &value) {
		if (end_pos == _chunk_count * CHUNK_SIZE) {
			_expand_capacity();
		}
		new (_chunks[end_pos / CHUNK_SIZE] + end_pos % CHUNK_SIZE) T(value);
		++end_pos;
	}
	/**
	 * removes the last element
	 *     throw when the container is empty.
	 */
	void pop_back() {
		if (empty()) {
			throw container_is_empty();
		}
		--end_pos;
		_chunks[end_pos / CHUNK_SIZE][end_pos % CHUNK_SIZE].~T();
	}
	/**
	 * inserts an element to the beginning.
	 */
	void push_front(const T &value) {
		if (start_pos == 0) {
			_expand_capacity();
		}
		--start_pos;
		new (_chunks[start_pos / CHUNK_SIZE] + start_pos % CHUNK_SIZE) T(value);
	}
	/**
	 * removes the first element.
	 *     throw when the container is empty.
	 */
	void pop_front() {
		if (empty()) {
			throw container_is_empty();
		}
		_chunks[start_pos / CHUNK_SIZE][start_pos % CHUNK_SIZE].~T();
		++start_pos;
	}
};

}

#endif
