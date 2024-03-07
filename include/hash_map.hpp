#pragma once
#include <sstream>
#include <exception>
#include <vector>
#include <memory>
namespace cs251 {

// Custom exception classes
class duplicate_key : public std::runtime_error {
	public: duplicate_key() : std::runtime_error("Duplicate key!") {} };
class nonexistent_key : public std::runtime_error {
	public: nonexistent_key() : std::runtime_error("Key does not exist!") {} };

template <typename K, typename V>
class hash_map {
public:
	class hash_map_node {
	public:
		// The key of current node.
		K m_key = {};
		// The value of current node.
		std::unique_ptr<V> m_value{};
	};

	// Return a constant reference to the hash table vector
	const std::vector<std::shared_ptr<hash_map_node>>& get_data() const;

	// Default constructor - create a hash map with an initial capacity of 1
	hash_map();
	// Constructor - create a hash map with an intial capacity of bucketCount
	hash_map(size_t bucketCount);

	// Get the hash code for a given key
	size_t hash_code(K key) const;

	// Change the size of the table to bucketCount, re-hashing all existing elements
	// bucketCount will never be 0 or less than the current number of elements
	void resize(size_t bucketCount);

	// Insert the key/value pair into the table, if the key doesn't already exist
	// Throw duplicate_key if the key already exists
	void insert(const K& key, std::unique_ptr<V> value);
	// Return a const reference to the value associated with the given key
	// Throw nonexistent_key if the key is not in the hash table
	const std::unique_ptr<V>& peek(const K& key);
	// Remove and return the key-value pair associated with the given key
	// Throw nonexistent_key if the key is not in the hash table
	std::unique_ptr<V> extract(const K& key);

	// Return the current number of elements in the hash table
	size_t size() const;
	// Return the current capacity of the hash table
	size_t bucket_count() const;
	// Return whether the hash table is currently empty
	bool empty() const;

private:
	// The array that holds key-value pairs
	std::vector<std::shared_ptr<hash_map_node>> m_data = {};

	// TODO: Add any additional methods or variables here
    size_t m_bucketCount;
    size_t m_numElements;
};

template <typename K, typename V>
const std::vector<std::shared_ptr<typename hash_map<K,V>::hash_map_node>>& hash_map<K,V>::get_data() const {
	return m_data;
}

template <typename K, typename V>
hash_map<K,V>::hash_map() {
    m_data = std::vector<std::shared_ptr<hash_map_node>>(1);
    m_bucketCount = 1;
    m_numElements = 0;
}

template <typename K, typename V>
hash_map<K,V>::hash_map(const size_t bucketCount) {
    m_data = std::vector<std::shared_ptr<hash_map_node>>(bucketCount);
    m_bucketCount = bucketCount;
    m_numElements = 0;
}

template <typename K, typename V>
size_t hash_map<K,V>::hash_code(K key) const {
	return key % m_bucketCount;
}

template <typename K, typename V>
void hash_map<K,V>::resize(const size_t bucketCount) {
	if (bucketCount >= m_numElements) {
        size_t originalSize = m_bucketCount;
        m_bucketCount = bucketCount;
        size_t newIndex = 0;

        std::vector<std::shared_ptr<hash_map_node>> resizedTable(bucketCount);
        //go through every slot in original hash table to rehash
        for (int i = 0; i < originalSize; i++) {
            //if there's an item in the bucket
            if (m_data[i] != nullptr) {
                newIndex = m_data[i]->m_key % bucketCount;

                //if the slot isn't available - linear probe to take care of collision
                while (resizedTable[newIndex] != nullptr) {
                    if (newIndex == bucketCount - 1) {                                                      //check if -1 needed
                        newIndex = 0;
                    } else {
                        newIndex++;
                    }
                }
                resizedTable[newIndex] = m_data[i];
            }
        }
        m_data.swap(resizedTable);
    }
}

template <typename K, typename V>
void hash_map<K,V>::insert(const K& key, std::unique_ptr<V> value) {
	for (int i = 0; i < m_bucketCount; i++) {
        if (m_data[i] != nullptr) {
            if (m_data[i]->m_key == key) {
                throw duplicate_key();
            }
        }
    }

    if (m_numElements == m_bucketCount) {
        resize(m_bucketCount * 2);
    }

    size_t location = key % m_bucketCount;
    //if the slot isn't available - linear probe to take care of collision
    while (m_data[location] != nullptr) {
        if (location == m_bucketCount - 1) {                                                      //check if -1 needed
            location = 0;
        } else {
            location++;
        }
    }
    //make a new node
    m_data[location] = std::make_shared<hash_map_node>();
    m_data[location]->m_key = std::move(key);
    m_data[location]->m_value = std::move(value);
    m_numElements++;
}

template <typename K, typename V>
const std::unique_ptr<V>& hash_map<K,V>::peek(const K& key) {
	bool found = false;
    size_t target = 0;
    for (int i = 0; i < m_bucketCount; i++) {
        if (m_data[i] != nullptr) {
            if (m_data[i]->m_key == key) {
                found = true;
                return m_data[i]->m_value;
            }
        }
    }

    if (!found) {
        throw nonexistent_key();
    }
}

template <typename K, typename V>
std::unique_ptr<V> hash_map<K,V>::extract(const K& key) {
    bool found = false;
    std::unique_ptr<V> nodeValue;
    for (int i = 0; i < m_bucketCount; i++) {
        if (m_data[i] != nullptr) {
            if (m_data[i]->m_key == key) {
                found = true;
                nodeValue = std::move(m_data[i]->m_value);
                m_data[i] = nullptr;
            }
        }
    }

    if (!found) {
        throw nonexistent_key();
    }

    m_numElements--;
    return nodeValue;
}

template <typename K, typename V>
size_t hash_map<K,V>::size() const {
    return m_numElements;
}

template <typename K, typename V>
size_t hash_map<K,V>::bucket_count() const {
	return m_bucketCount;
}

template <typename K, typename V>
bool hash_map<K,V>::empty() const {
    return m_numElements == 0;
}

}
