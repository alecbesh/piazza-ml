#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include <cassert>  //assert
#include <iostream> //ostream
#include <functional> //less

template <typename T,
          typename Compare=std::less<T> // default if argument isn't provided
         >
class BinarySearchTree {

private:

  struct Node {

    // Default constructor - does nothing
    Node() {}

    // Custom constructor provided for convenience
    Node(const T &datum_in, Node *left_in, Node *right_in)
            : datum(datum_in), left(left_in), right(right_in) { }

    T datum;
    Node *left;
    Node *right;
  };

public:

  // Default constructor
  // (Note this will default construct the less comparator)
  BinarySearchTree()
    : root(nullptr) { }

  // Copy constructor
  BinarySearchTree(const BinarySearchTree &other)
    : root(copy_nodes_impl(other.root)) { }

  // Assignment operator
  BinarySearchTree &operator=(const BinarySearchTree &rhs) {
    if (this == &rhs) {
      return *this;
    }
    destroy_nodes_impl(root);
    root = copy_nodes_impl(rhs.root);
    return *this;
  }

  // Destructor
  ~BinarySearchTree() {
    destroy_nodes_impl(root);
  }

  // EFFECTS: Returns whether this BinarySearchTree is empty.
  bool empty() const {
    return empty_impl(root);
  }

  // EFFECTS: Returns the height of the tree.
  size_t height() const {
    return static_cast<size_t>(height_impl(root));
  }

  // EFFECTS: Returns the number of elements in this BinarySearchTree.
  size_t size() const {
    return static_cast<size_t>(size_impl(root));
  }

  // EFFECTS: Traverses the tree using an in-order traversal,
  //          printing each element to os in turn. Each element is followed
  //          by a space (there will be an "extra" space at the end).
  //          If the tree is empty, nothing is printed.
  void traverse_inorder(std::ostream &os) const {
    traverse_inorder_impl(root, os);
  }

  // EFFECTS: Traverses the tree using a pre-order traversal,
  //          printing each element to os in turn. Each element is followed
  //          by a space (there will be an "extra" space at the end).
  //          If the tree is empty, nothing is printed.
  void traverse_preorder(std::ostream &os) const {
    traverse_preorder_impl(root, os);
  }

  // EFFECTS: Returns whether or not the sorting invariant holds on
  //          the root of this BinarySearchTree.
  //
  // NOTE: This function must be recursive.
  bool check_sorting_invariant() const {
    return check_sorting_invariant_impl(root, less);
  }

  class Iterator {
    // OVERVIEW: Iterator interface for BinarySearchTree.
    //           Iterates over the elements in ascending order as defined
    //           by the sorted ordering of the BinarySearchTree.

    // Big Three for Iterator not needed

  public:
    Iterator()
      : root(nullptr), current_node(nullptr) {}

    // EFFECTS:  Returns the current element by reference.
    // WARNING:  Dereferencing an iterator returns an element from the tree
    //           by reference, which could be modified. It is the
    //           responsibility of the user to ensure that any
    //           modifications result in a new value that compares equal
    //           to the existing value. Otherwise, the sorting invariant
    //           will no longer hold.
    T &operator*() const {
      return current_node->datum;
    }

    // EFFECTS:  Returns the current element by pointer.
    // WARNING:  Dereferencing an iterator returns an element from the tree
    //           by reference, which could be modified. It is the
    //           responsibility of the user to ensure that any
    //           modifications result in a new value that compares equal
    //           to the existing value. Otherwise, the sorting invariant
    //           will no longer hold.
    // NOTE:     This allows the -> operator to be applied to an iterator
    //           to access a member of the pointed-to element:
    //             BinarySearchTree<std::pair<int, double>> tree;
    //             auto it = tree.insert({ 3, 4.1 });
    //             cout << it->first << endl; // prints 3
    //             cout << it->second << endl; // prints 4.1
    T *operator->() const {
      return &current_node->datum;
    }

    // Prefix ++
    Iterator &operator++() {
      if (current_node->right) {
        // If has right child, next element is minimum of right subtree
        current_node = min_element_impl(current_node->right);
      }
      else {
        // Otherwise, look in the whole tree for the next biggest element
        current_node = min_greater_than_impl(root, current_node->datum, less);
      }
      return *this;
    }

    // Postfix ++ (implemented in terms of prefix ++)
    Iterator operator++(int) {
      Iterator result(*this);
      ++(*this);
      return result;
    }

    bool operator==(const Iterator &rhs) const {
      return current_node == rhs.current_node;
    }

    bool operator!=(const Iterator &rhs) const {
      return current_node != rhs.current_node;
    }

  private:
    friend class BinarySearchTree;

    Node *root;
    Node *current_node;
    Compare less;

    Iterator(Node *root_in, Node* current_node_in, Compare less_in)
      : root(root_in), current_node(current_node_in), less(less_in) { }

  }; // BinarySearchTree::Iterator
  ////////////////////////////////////////


  // EFFECTS : Returns an iterator to the first element
  //           in this BinarySearchTree.
  Iterator begin() const {
    if (root == nullptr) {
      return Iterator();
    }
    return Iterator(root, min_element_impl(root), less);
  }

  // EFFECTS: Returns an iterator to past-the-end.
  Iterator end() const {
    return Iterator();
  }


  // EFFECTS: Returns an Iterator to the minimum element in this
  //          BinarySearchTree or an end Iterator if the tree is empty.
  Iterator min_element() const {
    return Iterator(root, min_element_impl(root), less);
  }

  // EFFECTS: Returns an Iterator to the maximum element in this
  //          BinarySearchTree or an end Iterator if the tree is empty.
  Iterator max_element() const {
    return Iterator(root, max_element_impl(root), less);
  }

  // EFFECTS: Returns an Iterator to the minimum element in this
  //          BinarySearchTree greater than the given value.
  //          If the tree is empty, returns an end Iterator.
  Iterator min_greater_than(const T &value) const {
    return Iterator(root, min_greater_than_impl(root, value, less), less);
  }


  // EFFECTS: Searches this tree for an element equivalent to query.
  //          Returns an iterator to the existing element if found,
  //          and an end iterator otherwise.
  // WARNING: This function returns an Iterator that allows an element
  //          contained in this tree to be modified. It is the
  //          responsibility of the user to ensure that any
  //          modifications result in a new value that compares equal
  //          to the existing value. Otherwise, the sorting invariant
  //          will no longer hold.
  Iterator find(const T &query) const {
    return Iterator(root, find_impl(root, query, less), less);
  }

  // REQUIRES: The given item is not already contained in this BinarySearchTree
  // MODIFIES: this BinarySearchTree
  // EFFECTS : Inserts the element k into this BinarySearchTree, maintaining
  //           the sorting invariant.
  Iterator insert(const T &item) {
    assert(find(item) == end());
    root = insert_impl(root, item, less);
    return find(item);
  }

  // EFFECTS: Returns a human-readable string representation of this
  //          BinarySearchTree. Works best for small trees.
  //
  // NOTE: This member function is implemented for you in TreePrint.h.
  //       You may use it, but you don't need to worry about how it works.
  std::string to_string() const;


private:

  // DATA REPRESENTATION
  // The root node of this BinarySearchTree.
  Node *root;

  // An instance of the Compare type. Use this to compare elements.
  Compare less;

    
  // NOTE: These member types are implemented for you in TreePrint.h.
  //       They support the to_string function. You do not have to do
  //       anything with them.
  class Tree_grid_square;
  class Tree_grid;

  // NOTE: This member function is implemented for you in TreePrint.h.
  //       It supports the to_string function. You do not have to do
  //       anything with it.
  int get_max_elt_width() const;



  // EFFECTS: Returns whether the tree rooted at 'node' is empty.
  // NOTE:    This function must run in constant time.
  //          No iteration or recursion is allowed.
  static bool empty_impl(const Node *node) {
    if (node == nullptr) {
      return true;
    }
    else {
      return false;
    }
  }

  // EFFECTS: Returns the size of the tree rooted at 'node', which is the
  //          total number of nodes in that tree. The size of an empty
  //          tree is 0.
  // NOTE:    This function must be tree recursive.
  static int size_impl(const Node *node) {
    if (node == 0) return 0;
    return (size_impl(node->left) + size_impl(node->right)) + 1;
  }

  // EFFECTS: Returns the height of the tree rooted at 'node', which is the
  //          number of nodes in the longest path from the 'node' to a leaf.
  //          The height of an empty tree is 0.
  // NOTE:    This function must be tree recursive.
  static int height_impl(const Node *node) {
    if (node == 0) return 0;
    return (std::max(height_impl(node->left), height_impl(node->right)) + 1);
  }

  // EFFECTS: Creates and returns a pointer to the root of a new node structure
  //          with the same elements and EXACTLY the same structure as the
  //          tree rooted at 'node'.
  // NOTE:    This function must be tree recursive.
  static Node *copy_nodes_impl(Node *node) {
    if (node == 0) return nullptr;
    else {
      Node *new_node = new Node;
      new_node->datum = node->datum;
      new_node->left = copy_nodes_impl(node->left);
      new_node->right = copy_nodes_impl(node->right);
      return new_node;
    }
  }

  // EFFECTS: Frees the memory for all nodes used in the tree rooted at 'node'.
  // NOTE:    This function must be tree recursive.
  static void destroy_nodes_impl(Node *node) {
    if (node == nullptr) {
      return;
    }
    else {
    destroy_nodes_impl(node->left);
    destroy_nodes_impl(node->right);
    delete node;
    }
  }

  // EFFECTS : Searches the tree rooted at 'node' for an element equivalent
  //           to 'query'. If one is found, returns a pointer to the node
  //           containing it. If the tree is empty or the element is not
  //           found, returns a null pointer.
  //
  static Node * find_impl(Node *node, const T &query, Compare less) {
    if (node == 0) return nullptr;
    else if (!less(node->datum, query) && !less(query, node->datum)) {
      return node;
    }
    else {
      if (less(query, node->datum)) {
        return find_impl(node->left, query, less);
      }
      else {
        return find_impl(node->right, query, less);
      }
    }
    return nullptr;
  }

  // REQUIRES: item is not already contained in the tree rooted at 'node'
  // MODIFIES: the tree rooted at 'node'
  // EFFECTS : If 'node' represents an empty tree, allocates a newexit
  //           Node to represent a single-element tree with 'item' as
  //           its only element and returns a pointer to the new Node.
  //           If the tree rooted at 'node' is not empty, inserts
  //           'item' into the proper location as a leaf in the
  //           existing tree structure according to the sorting
  //           invariant and returns the original parameter 'node'.
  static Node * insert_impl(Node *node, const T &item, Compare less) {
    if (node == nullptr) {
      Node *new_node = new Node;
      new_node->datum = item;
      new_node->left = nullptr;
      new_node->right = nullptr;
      return new_node;
    }
    else {
      if (less(item, node->datum)) {
        node->left = insert_impl(node->left, item, less);
        return node;
      }
      else if (less(node->datum, item)) {
        node->right = insert_impl(node->right, item, less);
        return node;
        }
      return nullptr;
    }
  }

  // EFFECTS : Returns a pointer to the Node containing the minimum element
  //           in the tree rooted at 'node' or a null pointer if the tree is empty.
  static Node * min_element_impl(Node *node) {
    if (node->left == nullptr) return node;
    else {
      return (min_element_impl(node->left));
    }
  }

  // EFFECTS : Returns a pointer to the Node containing the maximum element
  //           in the tree rooted at 'node' or a null pointer if the tree is empty.
  static Node * max_element_impl(Node *node) {
    if (node->right == nullptr) return node;
    else {
      return (max_element_impl(node->right));
    }
  }


  // EFFECTS: Returns whether the sorting invariant holds on the tree
  //          rooted at 'node'.
  // NOTE:    This function must be tree recursive.
  static bool check_sorting_invariant_impl(const Node *node, Compare less) {
    if (node == 0) return true;
    else {
      if (node->left) {
        if (less(node->left->datum, node->datum)) {
          return check_sorting_invariant_impl(node->left, less);
        }
        else {
          return false;
        }
        return check_sorting_invariant_impl(node->left, less);
      }
      if (node->right) {
        if (less(node->datum, node->right->datum)) {
          return check_sorting_invariant_impl(node->right, less);
      }
        else {
          return false;
        }
      }
      return check_sorting_invariant_impl(node->right, less);
    }
  }

  // EFFECTS : Traverses the tree rooted at 'node' using an in-order traversal,
  //           printing each element to os in turn. Each element is followed
  //           by a space (there will be an "extra" space at the end).
  //           If the tree is empty, nothing is printed.
  static void traverse_inorder_impl(const Node *node, std::ostream &os) {
    if (node) {
      traverse_inorder_impl(node->left, os);
      os << node->datum << " ";
      traverse_inorder_impl(node->right, os);
    }
  }

  // EFFECTS : Traverses the tree rooted at 'node' using a pre-order traversal,
  //           printing each element to os in turn. Each element is followed
  //           by a space (there will be an "extra" space at the end).
  //           If the tree is empty, nothing is printed.
  static void traverse_preorder_impl(const Node *node, std::ostream &os) {
    if (node == 0) return;
    else {
      os << node->datum << " ";
      traverse_preorder_impl(node->left, os);
      traverse_preorder_impl(node->right, os);
    }
  }

  // EFFECTS : Returns a pointer to the Node containing the smallest element
  //           in the tree rooted at 'node' that is greater than 'val'.
  //           Returns a null pointer if the tree is empty or if it does not
  //           contain any elements that are greater than 'val'.
  //
  static Node * min_greater_than_impl(Node *node, const T &val, Compare less) {
    if (node == 0) return nullptr;

    else if (!less(val, node->datum)) {
      return min_greater_than_impl(node->right, val, less);
    }
    else {
      Node * left_check = min_greater_than_impl(node->left, val, less);
      if (left_check == nullptr) {
        return node;
      }
      else {
        return left_check;
      }
    }
  }
}; // END of BinarySearchTree class

#include "TreePrint.h" // DO NOT REMOVE!!!

// MODIFIES: os
// EFFECTS : Prints the elements in the tree to the given ostream,
//           separated by a space. The elements are printed using an
//           in-order traversal, and an initial "[" and trailing "]"
//           are printed before the first and after the last element.
//           Does not print a newline. Returns os.
template <typename T, typename Compare>
std::ostream &operator<<(std::ostream &os,
                         const BinarySearchTree<T, Compare> &tree) {
  os << "[ ";
  for (T& elt : tree) {
    os << elt << " ";
  }
  return os << "]";
}

#endif // DO NOT REMOVE!!