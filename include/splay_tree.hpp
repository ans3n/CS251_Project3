#pragma once
#include <sstream>
#include <exception>
#include <memory>
namespace cs251 {

// Custom exception classes
class duplicate_key : public std::runtime_error {
	public: duplicate_key() : std::runtime_error("Duplicate key!") {} };
class nonexistent_key : public std::runtime_error {
	public: nonexistent_key() : std::runtime_error("Key does not exist!") {} };
class empty_tree : public std::runtime_error {
	public: empty_tree() : std::runtime_error("Tree is empty!") {} };

template <typename K, typename V>
class splay_tree {
public:
	struct splay_tree_node {
		// Pointer to the left child
		std::shared_ptr<splay_tree_node> m_left {};
		// Pointer to the right child
		std::shared_ptr<splay_tree_node> m_right {};
		// Weak pointer to the parent
		std::weak_ptr<splay_tree_node> m_parent {};

		// The key of this element
		K m_key {};
		// Pointer to the value of this element
		std::unique_ptr<V> m_value {};
	};

	// Return a pointer to the root of the tree
	std::shared_ptr<splay_tree_node> get_root() const;

	// Default constructor - create an empty splay tree
	splay_tree();

	// Insert the key/value pair into the tree, if the key doesn't already exist
	// Throw duplicate_key if the key already exists
	void insert(const K& key, std::unique_ptr<V> value);
	// Return a const reference to the value associated with the given key
	// Throw nonexistent_key if the key is not in the splay tree
	const std::unique_ptr<V>& peek(const K& key);
	// Remove and return the key-value pair associated with the given key
	// Throw nonexistent_key if the key is not in the splay tree
	std::unique_ptr<V> extract(const K& key);

	// Return the minimum key in the splay tree, and splay the node
	// Throw empty_tree if the tree is empty
	K minimum_key();
	// Return the maximum key in the splay tree, and splay the node
	// Throw empty_tree if the tree is empty
	K maximum_key();

	// Return the current number of elements in the splay tree
	bool empty() const;
	// Return whether the splay tree is currently empty
	size_t size() const;

private:
	// Pointer to the root node of the splay tree
	std::shared_ptr<splay_tree_node> m_root {};

	// TODO: Add any additional methods or variables here
    void rightRotation(std::shared_ptr<splay_tree_node>& current);
    void leftRotation(std::shared_ptr<splay_tree_node>& current);
    void splay(std::shared_ptr<splay_tree_node>& current);
    void zig(std::shared_ptr<splay_tree_node>& current);
    void zigZig(std::shared_ptr<splay_tree_node>& current);
    void zigZag(std::shared_ptr<splay_tree_node>& current);

    size_t m_numElements;
};

template <typename K, typename V>
std::shared_ptr<typename splay_tree<K,V>::splay_tree_node> splay_tree<K,V>::get_root() const {
	return m_root;
}

template <typename K, typename V>
void splay_tree<K,V>::rightRotation(std::shared_ptr<splay_tree_node>& current) {
    std::shared_ptr<splay_tree_node> temp = current->m_left;
    current->m_left = temp->m_right;
    temp->m_right = current;
    current = temp;
}

template <typename K, typename V>
void splay_tree<K,V>::leftRotation(std::shared_ptr<splay_tree_node>& current) {
    std::shared_ptr<splay_tree_node> temp = current->m_right;
    current->m_right = temp->m_left;
    temp->m_left = current;
    current = temp;
}

template <typename K, typename V>
void splay_tree<K,V>::zig(std::shared_ptr<splay_tree_node>& current) {
    if (current->m_parent.lock() == m_root) {
        if (m_root->m_left == current) {
            rightRotation(m_root);
        } else {
            leftRotation(m_root);
        }
    }
}

template <typename K, typename V>
void splay_tree<K,V>::zigZig(std::shared_ptr<splay_tree_node>& current) {
    std::shared_ptr<splay_tree_node> parent = current->m_parent.lock();
    if (((parent->m_parent.lock()->m_left == parent) && (parent->m_left == current))) {
        rightRotation(parent->m_parent.lock());
        rightRotation(parent);
    } else {
        leftRotation(parent->m_parent.lock());
        leftRotation(parent);
    }
}

template <typename K, typename V>
void splay_tree<K,V>::zigZag(std::shared_ptr<splay_tree_node>& current) {
    std::shared_ptr<splay_tree_node> parent = current->m_parent.lock();
    if (((parent->m_parent.lock()->m_left == parent) && (parent->m_right == current))) {
        leftRotation(parent);
        rightRotation(parent->m_parent.lock());
    } else {
        rightRotation(parent);
        leftRotation(parent->m_parent.lock());
    }
}

//splay the key up to the root
template <typename K, typename V>
void splay_tree<K,V>::splay(std::shared_ptr<splay_tree_node>& current) {

}

template <typename K, typename V>
splay_tree<K,V>::splay_tree() {
    m_root = std::make_shared<splay_tree_node>();
    m_numElements = 0;
}

template <typename K, typename V>
void splay_tree<K,V>::insert(const K& key, std::unique_ptr<V> value) {
    if (empty()) {
        m_root = std::make_shared<splay_tree_node>();                                                       //check if needed
        m_root->m_value = std::move(value);
        m_root->m_key = std::move(key);
        m_root->m_parent = std::weak_ptr<splay_tree_node>();
        m_root->m_left = nullptr;
        m_root->m_right = nullptr;
    } else {
        std::shared_ptr<splay_tree_node> current = m_root;
        std::shared_ptr<splay_tree_node> parent = current;

        //traverse through tree
        while (current != nullptr) {
            parent = current;

            if (current->m_key == key) {
                throw duplicate_key();
            }

            if (key < current->m_key) {
                current = current->m_left;
            } else {
                current = current->m_right;
            }
        }

        current = std::make_shared<splay_tree_node>();
        current->m_parent.lock() = parent;
        current->m_key = std::move(key);
        current->m_value = std::move(value);
        current->m_right = nullptr;
        current->m_left = nullptr;

        if (key < parent->m_key) {
            parent->m_left = current;
        } else {
            parent->m_right = current;
        }

        splay(current);
    }
    m_numElements++;
}

template <typename K, typename V>
const std::unique_ptr<V>& splay_tree<K,V>::peek(const K& key) {
    bool found = false;
    std::shared_ptr<splay_tree_node> current = m_root;

	if (m_numElements == 1 && current->m_key == key) {
        found = true;
    } else {
        //traverse through tree
        while (current != nullptr) {
            if (current->m_key == key) {
                found = true;
                splay(current);
                break;
            }

            if (key < current->m_key) {
                current = current->m_left;
            } else {
                current = current->m_right;
            }
        }
    }

    if (!found) {
        throw nonexistent_key();
    }
    return current->m_value;
}

template <typename K, typename V>
std::unique_ptr<V> splay_tree<K,V>::extract(const K& key) {
    bool found = false;
    std::shared_ptr<splay_tree_node> current = m_root;
    std::unique_ptr<V> nodeValue;
    std::shared_ptr<splay_tree_node> successor;

    while (current != nullptr) {
        if (current->m_key == key) {
            found = true;
            splay(current);
            break;
        }

        //loop to find key
        if (key < current->m_key) {
            current = current->m_left;
        } else {
            current = current->m_right;
        }
    }

    if (found) {
        if (current->m_right != nullptr && current->m_left != nullptr) {

        } else if (current->m_right != nullptr) {
            successor = current->m_right;
            while (successor->m_left != nullptr) {
                successor = successor->m_left;
            }

            splay(successor);                      //ensure splay takes care of all connections


            /*
            if (current->m_left != nullptr) {                                           //check if these are needed still
                (current->m_left)->m_parent.lock() = successor;
            }
            //successor is not root's immediate right
            if ((current->m_right)->m_key != successor->m_key) {                         //check if these are needed still
                (current->m_right)->m_parent.lock() = successor;
            }
             */



        } else if (current->m_left != nullptr){          //no need for connection breaking with root as splay auto sets successor's parent to null?
            //no right successor, use left child
            successor = m_root->m_left;
            splay(successor);
        } else {
            //no children
            m_root = nullptr;
        }
    } else {
        throw nonexistent_key();
    }

    m_numElements--;
    return nodeValue;
}

template <typename K, typename V>
K splay_tree<K,V>::minimum_key() {
	if (m_numElements == 0) {
        throw empty_tree();
    }

    std::shared_ptr<splay_tree_node> current = m_root;
    while (current->m_left != nullptr) {
        current = current->m_left;
    }

    splay(current, current->m_key);
    return current->m_key;
}

template <typename K, typename V>
K splay_tree<K,V>::maximum_key() {
    if (m_numElements == 0) {
        throw empty_tree();
    }

    std::shared_ptr<splay_tree_node> current = m_root;
    while (current->m_right != nullptr) {
        current = current->m_right;
    }

    splay(current, current->m_key);
    return current->m_key;
}

template <typename K, typename V>
bool splay_tree<K,V>::empty() const {
	return m_numElements == 0;
}

template <typename K, typename V>
size_t splay_tree<K,V>::size() const {
	return m_numElements;
}

}