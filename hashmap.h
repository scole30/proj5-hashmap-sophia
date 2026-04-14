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

  ChainNode* curr;
  size_t curr_idx;

 public:
  HashMap() : sz(0), capacity(10), curr(nullptr), curr_idx(0) {
    data = new ChainNode*[capacity];
    for (size_t i = 0; i < capacity; ++i) {
      data[i] = nullptr;
    }
  }

  HashMap(size_t capacity)
      : sz(0), capacity(capacity), curr(nullptr), curr_idx(0) {
    data = new ChainNode*[capacity];
    for (size_t i = 0; i < capacity; ++i) {
      data[i] = nullptr;
    }
  }

  bool empty() const {
    return sz == 0;
  }

  size_t size() const {
    return sz;
  }

  void insert(KeyT key, ValT value) {
    size_t idx = hash<KeyT>{}(key) % capacity;
    ChainNode* ptr = data[idx];

    while (ptr != nullptr) {
      if (ptr->key == key) return;
      ptr = ptr->next;
    }

    data[idx] = new ChainNode(key, value, data[idx]);
    sz++;

    if (static_cast<double>(sz) / capacity > 1.5) {
      size_t new_capacity = capacity * 2;
      ChainNode** new_data = new ChainNode*[new_capacity];
      for (size_t i = 0; i < new_capacity; ++i) new_data[i] = nullptr;

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

  ValT& at(const KeyT& key) const {
    size_t idx = hash<KeyT>{}(key) % capacity;
    ChainNode* ptr = data[idx];
    while (ptr != nullptr) {
      if (ptr->key == key) return ptr->value;
      ptr = ptr->next;
    }
    throw out_of_range("Key not found");
  }

  bool contains(const KeyT& key) const {
    size_t idx = hash<KeyT>{}(key) % capacity;
    ChainNode* ptr = data[idx];
    while (ptr != nullptr) {
      if (ptr->key == key) return true;
      ptr = ptr->next;
    }
    return false;
  }

  void clear() {
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

  ~HashMap() {
    clear();
    delete[] data;
  }

  ValT erase(const KeyT& key) {
    size_t idx = hash<KeyT>{}(key) % capacity;
    ChainNode* ptr = data[idx];
    ChainNode* prev = nullptr;

    while (ptr != nullptr) {
      if (ptr->key == key) {
        ValT ret_val = ptr->value;
        if (prev == nullptr)
          data[idx] = ptr->next;
        else
          prev->next = ptr->next;

        delete ptr;
        sz--;
        return ret_val;
      }
      prev = ptr;
      ptr = ptr->next;
    }
    throw out_of_range("Key not found");
  }

  HashMap(const HashMap& other)
      : sz(other.sz), capacity(other.capacity), curr(nullptr), curr_idx(0) {
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

  HashMap& operator=(const HashMap& other) {
    if (this == &other) return *this;

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

  bool operator==(const HashMap& other) const {
    if (sz != other.sz) return false;

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

  void begin() {
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

  bool next(KeyT& key, ValT& value) {
    if (curr == nullptr) return false;

    key = curr->key;
    value = curr->value;

    if (curr->next != nullptr) {
      curr = curr->next;
    } else {
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

  void* get_data() const {
    return this->data;
  }
  size_t get_capacity() {
    return this->capacity;
  }
};
