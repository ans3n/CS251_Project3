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
void splay_tree<K,V>::rightRotation(std::shared_ptr<splay_tree_node>& nodeY) {
    std::shared_ptr<splay_tree_node> nodeX = nodeY->m_left;
    if (nodeX == nullptr) {
        return;
    }
    nodeY->m_left = nodeX->m_right;
    if (nodeX->m_right != nullptr) {
        (nodeX->m_right)->m_parent = nodeY;
    }

    nodeX->m_parent = nodeY->m_parent;  //x TODO changed
    std::shared_ptr<splay_tree_node> parentY = nodeY->m_parent.lock();
    if (parentY != nullptr) {
        // parentY = nodeY->m_parent.lock();

        // y's parent now pointing at x
        if (parentY->m_left == nodeY) {
            parentY->m_left = nodeX;
        } else {
            parentY->m_right = nodeX;
        }

        //nodeX->m_parent.lock() = parentY;
    } else {
        m_root = nodeX; // TODO, this is the y is at root case, you never set m_root to x, added that
        //nodeX->m_parent.lock() = nullptr; // TODO this is wrong, the parent should be a weak ptr type
    }
    nodeX->m_right = nodeY;
    nodeY->m_parent = nodeX;

}

template <typename K, typename V>   // TODO all the changes same as above
void splay_tree<K,V>::leftRotation(std::shared_ptr<splay_tree_node>& nodeX) {
    std::shared_ptr<splay_tree_node> nodeY = nodeX->m_right;
    if (nodeY == nullptr) {
        return;
    }
    nodeX->m_right = nodeY->m_left;

    if (nodeY->m_left != nullptr) {
        (nodeY->m_left)->m_parent = nodeX;
    }

    nodeY->m_parent = nodeX->m_parent;
    std::shared_ptr<splay_tree_node> parentX = nodeX->m_parent.lock();
    if (parentX != nullptr) {
        if (parentX->m_left == nodeX) {
            parentX->m_left = nodeY;
        } else {
            parentX->m_right = nodeY;
        }
        // nodeY->m_parent.lock() = parentX;
    } else {
        m_root = nodeY;
        // nodeY->m_parent.lock() = nullptr;
    }
    nodeY->m_left = nodeX;
    nodeX->m_parent = nodeY;
}

template <typename K, typename V>
void splay_tree<K,V>::zig(std::shared_ptr<splay_tree_node>& current) {
    std::shared_ptr<splay_tree_node> parent = current->m_parent.lock();
    if (parent->m_left == current) {
        rightRotation(parent);
    } else {
        leftRotation(parent);
    }
    /*if (m_root != nullptr) {
        if (parent == m_root) {

        }
    }*/
}

template <typename K, typename V>
void splay_tree<K,V>::zigZig(std::shared_ptr<splay_tree_node>& current) {
    std::shared_ptr<splay_tree_node> parent = current->m_parent.lock();
    std::shared_ptr<splay_tree_node> grandparent = parent->m_parent.lock();
    if (((grandparent->m_left == parent) && (parent->m_left == current))) {
        rightRotation(grandparent);
        rightRotation(parent);
    } else {
        leftRotation(grandparent);
        leftRotation(parent);
    }
    /*if (parent != nullptr && grandparent != nullptr) {

    }*/
}

template <typename K, typename V>
void splay_tree<K,V>::zigZag(std::shared_ptr<splay_tree_node>& current) {
    std::shared_ptr<splay_tree_node> parent = current->m_parent.lock();
    std::shared_ptr<splay_tree_node> grandparent = parent->m_parent.lock();
    if (((grandparent->m_left == parent) && (parent->m_right == current))) {
        leftRotation(parent);
        rightRotation(grandparent);
    } else {
        rightRotation(parent);
        leftRotation(grandparent);
    }
    /*if (parent != nullptr && grandparent != nullptr) {

    }*/
}

//splay the key up to the root
template <typename K, typename V>
void splay_tree<K,V>::splay(std::shared_ptr<splay_tree_node>& current) {
    while (current != m_root) {
        std::shared_ptr<splay_tree_node> parent = current->m_parent.lock();
        if (!parent) {
            break;
        }
        std::shared_ptr<splay_tree_node> grandparent = parent->m_parent.lock();
        if (grandparent == nullptr) {
            if (parent->m_left == current) {
                rightRotation(parent);
            } else {
                leftRotation(parent);
            }
        } else if ((grandparent->m_right == parent) && (parent->m_right == current)) {
            leftRotation(grandparent);
            leftRotation(parent);
        } else if ((grandparent->m_left == parent) && (parent->m_left == current)) {
            rightRotation(grandparent);
            rightRotation(parent);
        } else if ((grandparent->m_left == parent) && (parent->m_right == current)) {
            leftRotation(parent);
            rightRotation(grandparent);
        } else {
            rightRotation(parent);
            leftRotation(grandparent);
        }
    }
}

template <typename K, typename V>
splay_tree<K,V>::splay_tree() {
    m_numElements = 0;
}

template <typename K, typename V>
void splay_tree<K,V>::insert(const K& key, std::unique_ptr<V> value) {
    if (m_numElements == 0) {
        m_root = std::make_shared<splay_tree_node>();                                                       //check if needed
        m_root->m_value = std::move(value);
        m_root->m_key = key;
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
        current->m_key = key;
        current->m_value = std::move(value);

        current->m_parent = parent; // TODO don't need to do .lock() here
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

    if (current != nullptr) {
        if (m_numElements == 1 && current->m_key == key) {
            found = true;
        } else {
            //traverse through tree
            while (current != nullptr) {
                if (current->m_key == key) {
                    found = true;
                    //splay(current);  // TODO deleted this line cuz it seems like u r splaying it twice with the line later
                    break;
                }

                if (key < current->m_key) {
                    current = current->m_left;
                } else {
                    current = current->m_right;
                }
            }
        }
    }

    if (!found) {
        throw nonexistent_key();
    }
    splay(current);
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
            nodeValue = std::move(current->m_value);
            break;
        } else if (key < current->m_key) {
            current = current->m_left;
        } else {
            current = current->m_right;
        }
    }

    if (found) {
        if (current->m_right != nullptr && current->m_left != nullptr) {
            successor = current->m_right;
            while (successor->m_left != nullptr) {
                successor = successor->m_left;
            }

            if (successor != current->m_right) {
                if (successor->m_right != nullptr) {
                    (successor->m_right)->m_parent = successor->m_parent;
                }
                (successor->m_parent.lock())->m_left = successor->m_right;
                successor->m_right = current->m_right;
                current->m_right->m_parent = successor; // TODO, didn't connect both ways
            }

            successor->m_left = current->m_left;
            (current->m_left)->m_parent = successor;
            m_root = successor;
            successor->m_parent = std::weak_ptr<splay_tree_node>();
            current->m_left = nullptr;
            current->m_right = nullptr;

        } else if (current->m_right != nullptr) {
            //if only right child just take immediate right regardless if the right subtree has left children
            (m_root->m_right)->m_parent = std::weak_ptr<splay_tree_node>();
            m_root = m_root->m_right;

        } else if (current->m_left != nullptr){
            //no right successor, use left child
            (m_root->m_left)->m_parent = std::weak_ptr<splay_tree_node>();
            m_root = m_root->m_left;
        } else {
            //no children
            m_root = nullptr;
        }
    } else {
        //not found
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

    splay(current);
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

    splay(current);
    return current->m_key;
}

template <typename K, typename V>
bool splay_tree<K,V>::empty() const {
	return !m_root;
}

template <typename K, typename V>
size_t splay_tree<K,V>::size() const {
	return m_numElements;
}

}
