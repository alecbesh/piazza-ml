#ifndef MAP_H
#define MAP_H

using namespace std;

#include "BinarySearchTree.h"
#include <cassert>  //assert
#include <utility>  //pair

template <typename Key_type, typename Value_type,
          typename Key_compare=std::less<Key_type> // default argument
         >
class Map {

private:
  using Pair_type = std::pair<Key_type, Value_type>;

  // A custom comparator
  class PairComp {
    private:
      Key_compare less;

    public:
      const bool operator()(Pair_type lhs, Pair_type rhs) {
        return less(lhs.first, rhs.first);
      }
  };

private:
  BinarySearchTree<Pair_type, PairComp> *bst;

public:

  // OVERVIEW: Maps are associative containers that store elements
  // formed by a combination of a key value and a mapped value,
  // following a specific order.
  using Iterator = typename BinarySearchTree<Pair_type, PairComp>::Iterator;
  // Constructor
  Map() {
    bst = new BinarySearchTree<Pair_type, PairComp>;
  }

  // Destructor
  ~Map() {
    delete bst;
  }

  // EFFECTS : Returns whether this Map is empty.
  bool empty() const {
    return bst->empty();
  }

  // EFFECTS : Returns the number of elements in this Map.
  // NOTE : size_t is an integral type from the STL
  size_t size() const {
    return bst->size();
  }

  // EFFECTS : Searches this Map for an element with a key equivalent
  //           to k and returns an Iterator to the associated value if found,
  //           otherwise returns an end Iterator.
  Iterator find(const Key_type& k) const {
    Pair_type temp_pair;
    temp_pair.first = k;
    temp_pair.second = Value_type();
    Iterator itor = bst->find(temp_pair);
    if (itor == this->end()) {
      return this->end();
    }
    else {
      return itor;
    }
    return itor;
  }

  // MODIFIES: this
  // EFFECTS : Returns a reference to the mapped value for the given
  //           key. If k matches the key of an element in the
  //           container, the function returns a reference to its
  //           mapped value. If k does not match the key of any
  //           element in the container, the function inserts a new
  //           element with that key and a value-initialized mapped
  //           value and returns a reference to the mapped value.
  //           Note: value-initialization for numeric types guarantees the
  //           value will be 0 (rather than memory junk).
  Value_type& operator[](const Key_type& k) {
    Iterator itor = this->find(k);
    Pair_type temp_pair;
    temp_pair.first = k;
    temp_pair.second = Value_type();
    if (itor != this->end()) {
      Pair_type &pair_two = *itor;
      return pair_two.second;
    }
    else {
      std::pair<Iterator, bool> test;
      test = insert(temp_pair);
      itor = test.first;
      Pair_type &test_two = *itor;
      return test_two.second;
    }
  }
  // MODIFIES: this
  // EFFECTS : Inserts the given element into this Map if the given key
  //           is not already contained in the Map. If the key is
  //           already in the Map, returns an iterator to the
  //           corresponding existing element, along with the value
  //           false. Otherwise, inserts the given element and returns
  //           an iterator to the newly inserted element, along with
  //           the value true.
  std::pair<Iterator, bool> insert(const Pair_type &val) {
    Iterator itor = this->find(val.first);
    if (itor == this->end()) {
      Iterator i = bst->insert(val);
      std::pair<Iterator, bool> return_pair_true;
      return_pair_true.first = i;
      return_pair_true.second = true;
      return return_pair_true;
    }
    else {
      std::pair<Iterator, bool> return_pair_false;
      return_pair_false.first = itor;
      return_pair_false.second = false;
      return return_pair_false;
    }
  }

  // EFFECTS : Returns an iterator to the first key-value pair in this Map.
  Iterator begin() const {
    return bst->begin();
  }

  // EFFECTS : Returns an iterator to "past-the-end".
  Iterator end() const {
    return bst->end();
  }
};

#endif // DO NOT REMOVE!!!