#pragma once

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std;

template <typename KeyT, typename ValT>
class HashMap {
 private:
  struct ChainNode {
    const KeyT key;
    ValT value;
    ChainNode* next;

    ChainNode(KeyT key, ValT value) : key(key), value(value), next(nullptr) {
    }

    ChainNode(KeyT key, ValT value, ChainNode* next)
        : key(key), value(value), next(next) {
    }
  };

  ChainNode** data;
  size_t sz;
  size_t capacity;

  // Utility members for begin/next
  ChainNode* curr;
  size_t curr_idx;

 public:
  /**
   * Creates an empty `HashMap` with 10 buckets.
   */
  HashMap() : sz(0), capacity(10), curr(nullptr), curr_idx(0) {
    data = new ChainNode*[capacity];
    for (size_t i = 0; i < capacity; ++i) {
      data[i] = nullptr;
    }
  }

  /**
   * Creates an empty `HashMap` with `capacity` buckets.
   */
  HashMap(size_t capacity) : sz(0), capacity(capacity), curr(nullptr), curr_idx(0) {
    data = new ChainNode*[capacity];
    for (size_t i = 0; i < capacity; ++i) {
      data[i] = nullptr;
    }
  }

  /**
   * Checks if the `HashMap` is empty. Runs in O(1).
   */
  bool empty() const {
    return sz == 0;
  }

  /**
   * Returns the number of mappings in the `HashMap`. Runs in O(1).
   */
  size_t size() const {
    // TODO_STUDENT
    return sz;
  }

  /**
   * Adds the mapping `{key -> value}` to the `HashMap`. If the key already
   * exists, does not update the mapping (like the C++ STL map).
   *
   * Uses the hash value of the key to determine the location in the
   * underlying hash table. Creates exactly one new node; resizes by doubling
   * when the load factor exceeds 1.5.
   *
   * On resize, doesn't create new nodes, but rearranges existing ones.
   *
   * Runs in O(L), where L is the length of the longest chain.
   */
  void insert(KeyT key, ValT value) {
    // TODO_STUDENT
    size_t idx = hash<KeyT>{}(key) % capacity;
    ChainNode* ptr = data[idx];
    
    while (ptr != nullptr) {
      if (ptr->key == key) {
        return; 
      }
      ptr = ptr->next;
    }

    data[idx] = new ChainNode(key, value, data[idx]);
    sz++;

    if (static_cast<double>(sz) / capacity > 1.5) {
      size_t new_capacity = capacity * 2;
      ChainNode** new_data = new ChainNode*[new_capacity];
      for (size_t i = 0; i < new_capacity; ++i) {
        new_data[i] = nullptr;
      }

      for (size_t i = 0; i < capacity; ++i) {
        ChainNode* current = data[i];
        while (current != nullptr) {
          ChainNode* next_node = current->next;
          size_t new_idx = hash<KeyT>{}(current->key) % new_capacity;
          
          current->next = new_data[new_idx];
          new_data[new_idx] = current;
          
          current = next_node;
        }
      }

      delete[] data;
      data = new_data;
      capacity = new_capacity;
    }
  }

  /**
   * Return a reference to the value stored for `key` in the map.
   *
   * If key is not present in the map, throw `out_of_range` exception.
   *
   * Runs in O(L), where L is the length of the longest chain.
   */
  ValT& at(const KeyT& key) const {
    // TODO_STUDENT
    size_t idx = hash<KeyT>{}(key) % capacity;
    ChainNode* ptr = data[idx];
    
    while (ptr != nullptr) {
      if (ptr->key == key) {
        return ptr->value;
      }
      ptr = ptr->next;
    }
    
    throw out_of_range("Key not found in HashMap");
  }

  /**
   * Returns `true` if the key is present in the map, and false otherwise.
   *
   * Runs in O(L), where L is the length of the longest chain.
   */
  bool contains(const KeyT& key) const {
    // TODO_STUDENT
    size_t idx = hash<KeyT>{}(key) % capacity;
    ChainNode* ptr = data[idx];
    
    while (ptr != nullptr) {
      if (ptr->key == key) {
        return true;
      }
      ptr = ptr->next;
    }
    
    return false;
  }

  /**
   * Empties the `HashMap`, freeing all nodes. The bucket array may be left
   * alone.
   *
   * Runs in O(N+B), where N is the number of mappings and B is the number of
   * buckets.
   */
  void clear() {
    // TODO_STUDENT
    for (size_t i = 0; i < capacity; ++i) {
      ChainNode* ptr = data[i];
      while (ptr != nullptr) {
        ChainNode* temp = ptr;
        ptr = ptr->next;
        delete temp;
      }
      data[i] = nullptr;
    }
    sz = 0;
    curr = nullptr;
    curr_idx = 0;
  }

  /**
   * Destructor, cleans up the `HashMap`.
   *
   * Runs in O(N+B), where N is the number of mappings and B is the number of
   * buckets.
   */
  ~HashMap() {
    // TODO_STUDENT
    clear();
    delete[] data;
  }

  /**
   * Removes the mapping for the given key from the `HashMap`, and returns the
   * value.
   *
   * Throws `out_of_range` if the key is not present in the map. Creates no new
   * nodes, and does not update the key or value of any existing nodes.
   *
   * Runs in O(L), where L is the length of the longest chain.
   */
  ValT erase(const KeyT& key) {
    // TODO_STUDENT
    size_t idx = hash<KeyT>{}(key) % capacity;
    ChainNode* ptr = data[idx];
    ChainNode* prev = nullptr;
    
    while (ptr != nullptr) {
      if (ptr->key == key) {
        ValT ret_val = ptr->value;
        if (prev == nullptr) {
          data[idx] = ptr->next; // Removing head of the chain
        } else {
          prev->next = ptr->next; // Removing from middle/end
        }
        delete ptr;
        sz--;
        return ret_val;
      }
      prev = ptr;
      ptr = ptr->next;
    }
    
    throw out_of_range("Key not found in HashMap");
  }

  /**
   * Copy constructor.
   *
   * Copies the mappings from the provided `HashMap`.
   *
   * Runs in O(N+B), where N is the number of mappings in `other`, and B is the
   * number of buckets.
   */
  HashMap(const HashMap& other) {
    // TODO_STUDENT
    data = new ChainNode*[capacity];
    for (size_t i = 0; i < capacity; ++i) {
      data[i] = nullptr;
      ChainNode* other_ptr = other.data[i];
      ChainNode** tail_ptr = &data[i];
      
      while (other_ptr != nullptr) {
        *tail_ptr = new ChainNode(other_ptr->key, other_ptr->value);
        tail_ptr = &((*tail_ptr)->next);
        other_ptr = other_ptr->next;
      }
    }
  }

  /**
   * Assignment operator; `operator=`.
   *
   * Clears this table, and copies the mappings from the provided `HashMap`.
   *
   * Runs in O((N1+B1) + (N2+B2)), where N1 and B1 are the number of mappings
   * and buckets in `this`, and N2 and B2 are the number of mappings and buckets
   * in `other`.
   */
  HashMap& operator=(const HashMap& other) {
    // TODO_STUDENT
    if (this == &other) {
      return *this; // Protect against self-assignment
    }

    clear();
    delete[] data;

    capacity = other.capacity;
    sz = other.sz;
    data = new ChainNode*[capacity];
    
    for (size_t i = 0; i < capacity; ++i) {
      data[i] = nullptr;
      ChainNode* other_ptr = other.data[i];
      ChainNode** tail_ptr = &data[i];
      
      while (other_ptr != nullptr) {
        *tail_ptr = new ChainNode(other_ptr->key, other_ptr->value);
        tail_ptr = &((*tail_ptr)->next);
        other_ptr = other_ptr->next;
      }
    }
    
    curr = nullptr;
    curr_idx = 0;

    return *this;
  }

  // =====================

  /**
   *
   * Checks if the contents of `this` and `other` are equivalent.
   *
   * Two `HashMap` objects are equivalent if they contain the same
   * mappings from key to value. To be equivalent, the two `HashMap`
   * objects need not have the elements saved in the same order
   * inside of the buckets.
   *
   * Runs in worst-case O(B*L*L) time, where B is the maximum number
   * of buckets in either of the `HashMap` objects and L is the length
   * of the  largest chain on any of the buckets.
   *
   */
  bool operator==(const HashMap& other) const {
    // TODO_STUDENT
    if (sz != other.sz) {
      return false; // Fast fail if sizes don't match
    }

    for (size_t i = 0; i < capacity; ++i) {
      ChainNode* ptr = data[i];
      while (ptr != nullptr) {
        if (!other.contains(ptr->key) || other.at(ptr->key) != ptr->value) {
          return false;
        }
        ptr = ptr->next;
      }
    }
    return true;
  }

  /**
   * Resets internal state for an iterative traversal.
   *
   * See `next` for usage details. Modifies nothing except for `curr` and
   * `curr_idx`.
   *
   * Runs in worst-case O(B), where B is the number of buckets.
   */
  void begin() {
    // TODO_STUDENT
    curr_idx = 0;
    curr = nullptr;
    for (size_t i = 0; i < capacity; ++i) {
      if (data[i] != nullptr) {
        curr_idx = i;
        curr = data[i];
        break;
      }
    }
  }

  /**
   * Uses the internal state to return the "next" key and value
   * by reference, and advances the internal state. Returns `true` if the
   * reference parameters were set, and `false` otherwise.
   *
   * Example usage:
   *
   * ```c++
   * HashMap<string, int> hm;
   * hm.begin();
   * string key;
   * int value;
   * while (hm.next(key, val)) {
   *   cout << key << ": " << val << endl;
   * }
   * ```
   *
   * Does not visit the mappings in any defined order.
   *
   * Modifies nothing except for `curr` and `curr_idx`.
   *
   * Runs in worst-case O(B) where B is the number of buckets.
   */
  bool next(KeyT& key, ValT& value) {
    // TODO_STUDENT
    if (curr == nullptr) {
      return false;
    }

    // Set the output variables to the current node
    key = curr->key;
    value = curr->value;

    // Advance to the next node
    if (curr->next != nullptr) {
      curr = curr->next;
    } else {
      // Find the next non-empty bucket
      curr = nullptr;
      for (size_t i = curr_idx + 1; i < capacity; ++i) {
        if (data[i] != nullptr) {
          curr_idx = i;
          curr = data[i];
          break;
        }
      }
    }

    return true;
  }

  // ===============================================

  /**
   * Returns a pointer to the underlying memory managed by the `HashMap`.
   * For autograder testing purposes only.
   */
  void* get_data() const {
    return this->data;
  }

  /**
   * Returns the capacity of the underlying memory managed by the `HashMap`. For
   * autograder testing purposes only.
   */
  size_t get_capacity() {
    return this->capacity;
  }
};
