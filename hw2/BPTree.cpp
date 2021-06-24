/* Limited B+ tree in C++

https://www.programiz.com/terms-of-use

Copyright © 2020 Parewa Labs Pvt. Ltd.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <math.h>
using namespace std;
int MAX = 3;

// BP node
class Node {
  bool IS_LEAF;
  int *key, size;
  Node **ptr;
  friend class BPTree;

  public:
    Node();
};

// BP tree
class BPTree {
  Node *root;
  void insertInternal(int, Node *, Node *);
  void deleteInternal(Node*, int, int);
  Node *findParent(Node *, Node *);

  public:
    BPTree();
    void search(int);
    void insert(int);
    void deletes(int);
    void display(Node *);
    Node *getRoot();
};

Node::Node() {
  key = new int[MAX];
  ptr = new Node *[MAX + 1];
}

BPTree::BPTree() {
  root = NULL;
}

// Search operation
void BPTree::search(int x) {
  if (root == NULL) {
    cout << "Tree is empty\n";
  } else {
    Node *cursor = root;
    while (cursor->IS_LEAF == false) {
      for (int i = 0; i < cursor->size; i++) {
        if (x < cursor->key[i]) {
          cursor = cursor->ptr[i];
          break;
        }
        if (i == cursor->size - 1) {
          cursor = cursor->ptr[i + 1];
          break;
        }
      }
    }
    for (int i = 0; i < cursor->size; i++) {
      if (cursor->key[i] == x) {
        cout << "Found\n";
        return;
      }
    }
    cout << "Not found\n";
  }
}

// Insert Operation
void BPTree::insert(int x) {
  if (root == NULL) {
    root = new Node;
    root->key[0] = x;
    root->IS_LEAF = true;
    root->size = 1;
  } else {
    Node *cursor = root;
    Node *parent;
    while (cursor->IS_LEAF == false) {
      parent = cursor;
      for (int i = 0; i < cursor->size; i++) {
        if (x < cursor->key[i]) {
          cursor = cursor->ptr[i];
          break;
        }
        if (i == cursor->size - 1) {
          cursor = cursor->ptr[i + 1];
          break;
        }
      }
    }
    if (cursor->size < MAX) {
      int i = 0;
      while (x > cursor->key[i] && i < cursor->size)
        i++;
      for (int j = cursor->size; j > i; j--) {
        cursor->key[j] = cursor->key[j - 1];
      }
      cursor->key[i] = x;
      cursor->size++;
      cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
      cursor->ptr[cursor->size - 1] = NULL;
    } else {
      Node *newLeaf = new Node;
      int virtualNode[MAX + 1];
      for (int i = 0; i < MAX; i++) {
        virtualNode[i] = cursor->key[i];
      }
      int i = 0, j;
      while (x > virtualNode[i] && i < MAX)
        i++;
      for (int j = MAX + 1; j > i; j--) {               // isn't this supposed to be MAX, not MAX+1? Did we mean to go out of bounds with the array index?
        virtualNode[j] = virtualNode[j - 1];
      }
      virtualNode[i] = x;
      newLeaf->IS_LEAF = true;
      cursor->size = (MAX + 1) / 2;
      newLeaf->size = MAX + 1 - (MAX + 1) / 2;
      cursor->ptr[cursor->size] = newLeaf;
      newLeaf->ptr[newLeaf->size] = cursor->ptr[MAX];
      cursor->ptr[MAX] = NULL;
      for (i = 0; i < cursor->size; i++) {
        cursor->key[i] = virtualNode[i];
      }
      for (i = 0, j = cursor->size; i < newLeaf->size; i++, j++) {
        newLeaf->key[i] = virtualNode[j];
      }
      if (cursor == root) {
        Node *newRoot = new Node;
        newRoot->key[0] = newLeaf->key[0];
        newRoot->ptr[0] = cursor;
        newRoot->ptr[1] = newLeaf;
        newRoot->IS_LEAF = false;
        newRoot->size = 1;
        root = newRoot;
      } else {
        insertInternal(newLeaf->key[0], parent, newLeaf);
      }
    }
  }
}

// Insert Operation
void BPTree::insertInternal(int x, Node *cursor, Node *child) {
  if (cursor->size < MAX) {
    int i = 0;
    while (x > cursor->key[i] && i < cursor->size)
      i++;
    for (int j = cursor->size; j > i; j--) {
      cursor->key[j] = cursor->key[j - 1];
    }
    for (int j = cursor->size + 1; j > i + 1; j--) {
      cursor->ptr[j] = cursor->ptr[j - 1];
    }
    cursor->key[i] = x;
    cursor->size++;
    cursor->ptr[i + 1] = child;
  } else {
    Node *newInternal = new Node;
    int virtualKey[MAX + 1];
    Node *virtualPtr[MAX + 2];
    for (int i = 0; i < MAX; i++) {
      virtualKey[i] = cursor->key[i];
    }
    for (int i = 0; i < MAX + 1; i++) {
      virtualPtr[i] = cursor->ptr[i];
    }
    int i = 0, j;
    while (x > virtualKey[i] && i < MAX)
      i++;
    for (int j = MAX + 1; j > i; j--) {
      virtualKey[j] = virtualKey[j - 1];
    }
    virtualKey[i] = x;
    for (int j = MAX + 2; j > i + 1; j--) {
      virtualPtr[j] = virtualPtr[j - 1];
    }
    virtualPtr[i + 1] = child;
    newInternal->IS_LEAF = false;
    cursor->size = (MAX + 1) / 2;
    newInternal->size = MAX - (MAX + 1) / 2;
    for (i = 0, j = cursor->size + 1; i < newInternal->size; i++, j++) {
      newInternal->key[i] = virtualKey[j];
    }
    for (i = 0, j = cursor->size + 1; i < newInternal->size + 1; i++, j++) {
      newInternal->ptr[i] = virtualPtr[j];
    }
    if (cursor == root) {
      Node *newRoot = new Node;
      newRoot->key[0] = cursor->key[cursor->size];
      newRoot->ptr[0] = cursor;
      newRoot->ptr[1] = newInternal;
      newRoot->IS_LEAF = false;
      newRoot->size = 1;
      root = newRoot;
    } else {
      insertInternal(cursor->key[cursor->size], findParent(root, cursor), newInternal);
    }
  }
}

void BPTree::deleteInternal(Node* parent, int cursorI, int x){
  cout << "\nLooking for " << x << endl << endl;
  Node *leftSibling;
  Node *rightSibling;
  Node* cursor = parent->ptr[cursorI];
  if(cursorI == 0){
    leftSibling =  NULL;
  } else {
    leftSibling = parent->ptr[cursorI-1];
  }
  if(cursorI == parent->size){
    rightSibling =  NULL;
  } else {
    rightSibling = parent->ptr[cursorI+1];
  }
  // search cursor for the value x
  int i = -1;
  for(int j=0;j<cursor->size;j++){
    if(cursor->key[j] == x){
      i = j;
      cout << "\nFound the value in the " << i << " slot\n\n";
    }
  }
      // if i < 0, return
  if(i < 0){
    return;
  }

  // delete it 

  // shift everything left 1
  for(int j=i;j<cursor->size - 1;j++){
    cursor->key[j] = cursor->key[j+1];
  }
  for(int j=i;j<cursor->size;j++){
    cursor->ptr[j] = cursor->ptr[j+1];
  }
  cursor->size--;
  
    // else, check the size of cursor
      // if size >= MAX/2 + 1 
  if(cursor->size >= ceil(MAX/2.0+1)){
    cout << "\nJust deleting\tNode size is " << cursor->size << endl << endl;
    return;
  }

  if(rightSibling != NULL && rightSibling->size > ceil(MAX/2.0)){
    cout << "\nRedistributing from right sibling\n\n";
    // redistribute from rightSibling
    cursor->key[cursor->size-1] = rightSibling->key[0];
    cursor->ptr[cursor->size] = rightSibling->ptr[0];
    for(int j=0;j<rightSibling->size-1;j++){
      rightSibling->key[j] = rightSibling->key[j+1];
      rightSibling->ptr[j] = rightSibling->ptr[j+1];
    }
    rightSibling->ptr[rightSibling->size-1] = rightSibling->ptr[rightSibling->size];
    rightSibling->size--;
    cursor->size++;
    return;
  }

  if(leftSibling != NULL && leftSibling->size > ceil(MAX/2.0)){
    cout << "\nRedistributing from left sibling\n\n";
    cursor->ptr[cursor->size+1] = cursor->ptr[cursor->size];
    for(int j=cursor->size;j>0;j--){
      cursor->key[j] = cursor->key[j-1];
      cursor->ptr[j] = cursor->ptr[j-1];
    }
    cursor->key[0] = leftSibling->key[leftSibling->size-1];
    cursor->ptr[0] = leftSibling->ptr[leftSibling->size];
    leftSibling->size--;
    cursor->size++;
    return;
  }
      // else check size of left sibling
        // if leftSibling != NULL
          // if leftSibling->size + cursor->size <= MAX
  if(leftSibling != NULL && (leftSibling->size + cursor->size <= MAX)){
    // combine them
    for(int j=0;j<cursor->size;j++){
      leftSibling->key[leftSibling->size+j] = cursor->key[j];
      leftSibling->ptr[leftSibling->size+j] = cursor->ptr[j];
    }
    leftSibling->ptr[leftSibling->size+cursor->size] = cursor->ptr[cursor->size];
    leftSibling->size += cursor->size;
    parent->ptr[cursorI] = NULL;
    // findParent of parent
    Node* grandparent = findParent(root, parent);
    // find parentI
    int parentI = 0;
    for(int j=0;j<grandparent->size+1;j++){
      if(grandparent->ptr[j] == parent){
        parentI = j;
      }
    }
    // deleteinternal parents parent, parentI, parent->key[cursorI]
    if(rightSibling){
      deleteInternal(grandparent, parentI, parent->key[cursorI]);
    } else {
      deleteInternal(grandparent, parentI, parent->key[cursorI-1]);
    }
    return;
  }
      // else check size of right sibling
        // if rightSibling != NULL
          // if rightSibling->size + cursor->size <= MAX
  if(rightSibling != NULL && (rightSibling->size + cursor->size <= MAX)){
    // combine them
    for(int j=0;j<rightSibling->size;j++){
      cursor->key[cursor->size+j] = rightSibling->key[j];
      cursor->ptr[cursor->size+j] = rightSibling->ptr[j];
    }
    cursor->ptr[cursor->size+rightSibling->size] = rightSibling->ptr[rightSibling->size];
    cursor->size += rightSibling->size;
    parent->ptr[cursorI+1] = NULL;
    // findParent of parent
    Node* grandparent = findParent(root, parent);
    // find parentI
    int parentI = 0;
    for(int j=0;j<grandparent->size+1;j++){
      if(grandparent->ptr[j] == parent){
        parentI = j;
      }
    }
    // deleteinternal parents parent, parentI, parent->key[cursorI]
    deleteInternal(grandparent, parentI, parent->key[cursorI]);
    return;
  }
      // else move a value from the right side to the end of cursor, update the cursor size and take the ptr with it, and then left shift the rightsibling and update its size
      // finally, update the parents key[cursorI+1] to the new value of rightsibling->key[0]
      // return
}


// Delete Operation
void BPTree::deletes(int x) {
    // empty case
    if(root == NULL){
      return;
    }
    // find the proper leaf node
    Node *cursor = root;
    Node *parent;
    int cursorI;
    while (cursor->IS_LEAF == false) {
      parent = cursor;
      // select the proper pointer to continue further into the tree
      for (int i = 0; i < cursor->size; i++) {
        if (x < cursor->key[i]) {
          cursor = cursor->ptr[i];
          cursorI = i;
          break;
        }
        if (i == cursor->size - 1) {
          cursor = cursor->ptr[i + 1];
          cursorI = i+1;
          break;
        }
      }
    }
    // cursor is now the proper leaf node that x would be in
    deleteInternal(parent, cursorI, x);
}



// Find the parent
Node *BPTree::findParent(Node *cursor, Node *child) {
  Node *parent;
  if (cursor->IS_LEAF || (cursor->ptr[0])->IS_LEAF) {
    return NULL;
  }
  for (int i = 0; i < cursor->size + 1; i++) {
    if (cursor->ptr[i] == child) {
      parent = cursor;
      return parent;
    } else {
      parent = findParent(cursor->ptr[i], child);
      if (parent != NULL)
        return parent;
    }
  }
  return parent;
}

// Print the tree
void BPTree::display(Node *cursor) {
  if (cursor != NULL) {
    for (int i = 0; i < cursor->size; i++) {
      cout << cursor->key[i] << " ";
    }
    cout << "\n";
    if (cursor->IS_LEAF != true) {
      for (int i = 0; i < cursor->size + 1; i++) {
        display(cursor->ptr[i]);
      }
    }
  }
}

// Get the root
Node *BPTree::getRoot() {
  return root;
}

int main() {
  BPTree node;
  node.insert(5);
  node.insert(15);
  node.insert(25);
  node.insert(35);
  node.insert(45);
  node.insert(55);
  node.insert(40);
  node.insert(30);
  node.insert(20);
  node.display(node.getRoot());
  cout << endl << endl;
  node.deletes(25);
  node.display(node.getRoot());
}