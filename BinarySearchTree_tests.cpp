//

#include <sstream>
#include <cstring>
#include <cmath>

#include "BinarySearchTree.h"
#include "unit_test_framework.h"

using namespace std;

TEST (test_empty) {
    BinarySearchTree<int> bst;
    ASSERT_TRUE(bst.empty());
    bst.insert(5);
    ASSERT_FALSE(bst.empty());
}

TEST(test_size) {
    BinarySearchTree<char> bst;
    ASSERT_EQUAL(bst.size(), 0);
    bst.insert('a');
    ASSERT_EQUAL(bst.size(), 1);
    bst.insert('b');
    ASSERT_EQUAL(bst.size(), 2);
    bst.insert('c');
    ASSERT_EQUAL(bst.size(), 3);
    bst.insert('d');
    ASSERT_EQUAL(bst.size(), 4);
}

TEST(test_insert) {
    BinarySearchTree<int> bst;
    ASSERT_TRUE(bst.empty());
    ASSERT_EQUAL(bst.size(), 0);
    bst.insert(1);
    bst.insert(2);
    ASSERT_FALSE(bst.empty());
    ASSERT_EQUAL(bst.size(), 2);
    bst.insert(3);
    bst.insert(4);
    ASSERT_FALSE(bst.empty());
    ASSERT_EQUAL(bst.size(), 4);
}

TEST(test_height) {
    BinarySearchTree<int> bst;
    ASSERT_EQUAL(bst.height(), 0);
    bst.insert(1);
    bst.insert(2);
    bst.insert(3);
    bst.insert(4);
    bst.insert(5);
    ASSERT_EQUAL(bst.height(), 5);
}

TEST(test_copy) {
    BinarySearchTree<char> bst;
    BinarySearchTree<char> bst_two;
    bst.insert('a');
    bst.insert('l');
    bst.insert('e');
    bst.insert('c');
    bst_two = bst;
    ASSERT_EQUAL(bst.size(), bst_two.size());
    ASSERT_EQUAL(bst.height(), bst_two.height());
    bst_two.insert('z');
    ASSERT_NOT_EQUAL(bst.size(), bst_two.size());
    ASSERT_EQUAL(bst.height(), bst_two.height());
}

TEST(test_copy2) {
    BinarySearchTree<char> bst;
    BinarySearchTree<char> bst_two;
    bst_two = bst;
    ASSERT_EQUAL(bst.size(), bst_two.size());
    ASSERT_EQUAL(bst.height(), bst_two.height());
    bst_two.insert('z');
    ASSERT_NOT_EQUAL(bst.size(), bst_two.size());
    ASSERT_NOT_EQUAL(bst.height(), bst_two.height());
    bst.insert('s');
    ASSERT_EQUAL(bst.size(), bst_two.size());
    ASSERT_EQUAL(bst.height(), bst_two.height());
}

TEST(test_min) {
    BinarySearchTree<int> bst;
    bst.insert(5);
    bst.insert(-5);
    bst.insert(0);
    ASSERT_EQUAL(*bst.min_element(), -5);
    bst.insert(-10);
    ASSERT_EQUAL(*bst.min_element(), -10);
}

TEST(test_max) {
    BinarySearchTree<int> bst;
    bst.insert(5);
    bst.insert(-5);
    bst.insert(0);
    ASSERT_EQUAL(*bst.max_element(), 5);
    bst.insert(10);
    ASSERT_EQUAL(*bst.max_element(), 10);
}

TEST(test_maxmin2) {
    BinarySearchTree<int> bst;
    bst.insert(5);
    ASSERT_EQUAL(*bst.max_element(), 5);
    ASSERT_EQUAL(*bst.min_element(), 5);
    bst.insert(10);
    ASSERT_EQUAL(*bst.max_element(), 10);
    ASSERT_EQUAL(*bst.min_element(), 5);
}

TEST(test_check_sorting_invariant) {
    BinarySearchTree<int> bst;
    ASSERT_TRUE(bst.check_sorting_invariant());
    bst.insert(1);
    bst.insert(2);
    ASSERT_TRUE(bst.check_sorting_invariant());
    bst.insert(-100);
    bst.insert(-150);
    bst.insert(1000);
    ASSERT_TRUE(bst.check_sorting_invariant());
    bst.insert(10);
    bst.insert(12);
    bst.insert(9);
    bst.insert(11);
    ASSERT_TRUE(bst.check_sorting_invariant());
}

TEST(test_traverse_inorder) {
    BinarySearchTree<int> bst;
    bst.insert(-10);
    bst.insert(-5);
    bst.insert(0);
    bst.insert(5);
    bst.insert(10);
    ostringstream fout;
    bst.traverse_inorder(fout);
    ASSERT_EQUAL(fout.str(), "-10 -5 0 5 10 ");
}

TEST(test_traverse_inorder2) {
    BinarySearchTree<int> bst;
    bst.insert(5);
    bst.insert(0);
    bst.insert(-5);
    bst.insert(10);
    bst.insert(-10);
    ostringstream fout;
    bst.traverse_inorder(fout);
    ASSERT_EQUAL(fout.str(), "-10 -5 0 5 10 ");
}

TEST(test_traverse_inorder3) {
    BinarySearchTree<int> bst;
    bst.insert(5);
    ostringstream fout;
    bst.traverse_inorder(fout);
    ASSERT_EQUAL(fout.str(), "5 ");
}

TEST(test_traverse_inorder_empty) {
    BinarySearchTree<int> bst;
    ASSERT_TRUE(bst.empty());
    ostringstream fout;
    bst.traverse_inorder(fout);
    ASSERT_EQUAL(fout.str(), "");
}

TEST(test_traverse_preorder) {
    BinarySearchTree<int> bst;
    bst.insert(1);
    bst.insert(2);
    bst.insert(-1);
    ostringstream fout;
    bst.traverse_preorder(fout);
    ASSERT_EQUAL(fout.str(), "1 -1 2 ");
}

TEST(test_traverse_preorder2) {
    BinarySearchTree<int> bst;
    bst.insert(3);
    bst.insert(2);
    bst.insert(4);
    bst.insert(1);
    ostringstream fout;
    bst.traverse_preorder(fout);
    ASSERT_EQUAL(fout.str(), "3 2 1 4 ");
}

TEST(test_traverse_preorder3) {
    BinarySearchTree<int> bst;
    bst.insert(3);
    ostringstream fout;
    bst.traverse_preorder(fout);
    ASSERT_EQUAL(fout.str(), "3 ");
}

TEST(test_find) {
    BinarySearchTree<int> bst;
    bst.insert(1);
    bst.insert(2);
    bst.insert(10);
    bst.insert(0);
    ASSERT_FALSE(bst.empty());
    ASSERT_EQUAL(*bst.find(2), 2);
    ASSERT_EQUAL(*bst.find(0), 0);
    ASSERT_EQUAL(bst.find(5), bst.end());
    ASSERT_EQUAL(*bst.find(1), 1);
    ASSERT_EQUAL(*bst.find(10), 10);
    ASSERT_EQUAL(bst.find(11), bst.end());
}

TEST(test_min_greater_than) {
    BinarySearchTree<int> bst;
    ASSERT_EQUAL(bst.min_greater_than(0), bst.end());
    bst.insert(1);
    bst.insert(3);
    bst.insert(10);
    bst.insert(0);
    ASSERT_EQUAL(*bst.min_greater_than(4), 10);
    ASSERT_EQUAL(*bst.min_greater_than(2), 3);
    ASSERT_EQUAL(*bst.min_greater_than(3), 10);
    ASSERT_EQUAL(*bst.min_greater_than(0), 1);
    ASSERT_EQUAL(bst.min_greater_than(11), bst.end());
    bst.insert(5);
    bst.insert(12);
    bst.insert(8);
    bst.insert(6);
    ASSERT_EQUAL(*bst.min_greater_than(5), 6);
    ASSERT_EQUAL(*bst.min_greater_than(9), 10);
    ASSERT_EQUAL(*bst.min_greater_than(6), 8);
    ASSERT_EQUAL(*bst.min_greater_than(0), 1);
    ASSERT_EQUAL(bst.min_greater_than(12), bst.end());
}


TEST_MAIN()