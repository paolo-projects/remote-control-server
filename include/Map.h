#ifndef MAP_H
#define MAP_H

#include <functional>

/**
 * @brief A (bad) fixed-size implementation of a map/dictionary
 *
 * @tparam T The type of the keys
 * @tparam E The type of the values
 * @tparam S The pre-allocated max-number of elements the map is allowed to have. Defaults to 24 to spare device memory.
 * @tparam equals The functor class used for equalness check. It defaults to using the == operator
 */
template <typename T, typename E, int S = 24, class equals = std::equal_to<T>>
class Map
{
public:
	// Classes for iterating the map
	class MapEntry
	{
	public:
		MapEntry() = delete;
		MapEntry(Map<T, E, S, equals> &parent, int index)
			: parent(parent), index(index)
		{
		}
		T &key() const
		{
			return parent.keys[index];
		}
		E &value() const
		{
			return parent.values[index];
		}

	private:
		Map<T, E, S, equals> &parent;
		int index;
	};

	class iterator
	{
	public:
		iterator &operator++(int _)
		{
			index++;
			return *this;
		}
		MapEntry operator*()
		{
			return MapEntry(parent, index);
		}
		bool operator==(const iterator &rhs) const
		{
			return index == rhs.index;
		}
		bool operator!=(const iterator &rhs) const
		{
			return index != rhs.index;
		}

		iterator(Map<T, E, S, equals> &parent) : parent(parent) {}
		iterator(Map<T, E, S, equals> &parent, int index) : parent(parent), index(index) {}

	private:
		Map<T, E, S, equals> &parent;
		int index = 0;
	};

	/**
	 * @brief Construct a new Map object
	 *
	 */
	Map() {}
	/**
	 * @brief Puts the given key-value pair in the map
	 *
	 * @param key The key
	 * @param value The value
	 * @return true if the operation succeeded
	 * @return false if the operation failed because the map has no more room
	 */
	bool put(const T &key, const E &value)
	{
		int vindex = indexOf(key);
		if (vindex < 0)
		{
			if (size < S)
			{
				keys[cursor] = key;
				values[cursor] = value;
				size++;
				cursor++;
			}
			else
			{
				return false;
			}
		}
		else
		{
			values[vindex] = value;
		}
		return true;
	}
	/**
	 * @brief Gets the value by the given key
	 *
	 * @param key The key to search for
	 * @return E* The pointer of the value
	 * @retval nullptr When the key has not been found
	 */
	const E *get(const T &key) const
	{
		int vindex = indexOf(key);
		if (vindex >= 0)
		{
			return &values[vindex];
		}
		return nullptr;
	}
	/**
	 * @brief Attempts removing a key-value pair from the map
	 *
	 * @param key The key to remove
	 * @return true If the key-value pair has been removed successfully
	 * @return false If the key hasn't been found
	 */
	bool remove(const T &key)
	{
		int vindex = indexOf(key);
		if (vindex >= 0)
		{
			shiftLeft(vindex + 1);
			size--;
			cursor--;
			return true;
		}
		return false;
	}
	/**
	 * @brief Checks if the given key has been set inside the map
	 *
	 * @param key The key to search for
	 * @return true If the given key is present in the map
	 * @return false If the given key is not in the map
	 */
	bool has(const T &key) const
	{
		return indexOf(key) >= 0;
	}
	/**
	 * @brief Get the value paired with the given key
	 *
	 * @param key The key to search for
	 * @return E* The value
	 * @retval nullptr When the key has not been found
	 */
	const E *operator[](const T &key) const
	{
		return get(key);
	}
	/**
	 * @brief Get the map size
	 * 
	 * @return int The map size
	 */
	int getSize() const
	{
		return size;
	}

	iterator begin()
	{
		return iterator(*this);
	}

	iterator end()
	{
		return iterator(*this, size);
	}

protected:
	T keys[S];
	E values[S];
	int size = 0;

private:
	int cursor = 0;

	/**
	 * @brief Construct a new Map object from the given key-value arrays
	 *
	 * @param k The array of keys
	 * @param v The array of values
	 * @param size The size of the arrays
	 */
	Map(const T *k, const E *v, int size)
	{
		if (size < S)
		{
			for (int i = 0; i < size; i++)
			{
				keys[i] = k[i];
				values[i] = v[i];
			}
		}
		this->size = cursor = size;
	}
	/**
	 * @brief Shifts left the elements of the arrays starting from the given index
	 *
	 * @param startIndex The first index to shift left
	 */
	void shiftLeft(int startIndex)
	{
		for (int i = startIndex; i < size; i++)
		{
			keys[i - 1] = keys[i];
			values[i - 1] = values[i];
		}
	}
	/**
	 * @brief Returns the internal index of a given key.
	 * The comparison is either done based on the == operator of E
	 * or equals can be specified as a custom equalness comparator
	 *
	 * @param key The key to find
	 * @return int The index
	 * @retval -1 When the key has not been found
	 */
	int indexOf(const T &key) const
	{
		for (int i = 0; i < size; i++)
		{
			if (equals()(keys[i], key))
			{
				return i;
			}
		}
		return -1;
	}
};

#endif
