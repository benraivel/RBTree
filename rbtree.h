/**
 * rbtree header file
 * 
 * Ben Raivel
 * 
 */
#include <stdbool.h>

typedef struct RBNode RBNode;

// Red-Black node struct
struct RBNode{
    
    unsigned long key; // lu key
    bool red; // node color

    RBNode* parent; // parent node
    RBNode* lchild; // left child node
    RBNode* rchild; // right child node

};    

//////////////////
// RBNode Methods:
//////////////////

// creates a node with key, returns pointer to node
RBNode* RBNode_create(unsigned long key);

// true if node is a left child
bool is_lchild(RBNode* node);

// gets the uncle
RBNode* get_uncle(RBNode* node);


typedef struct RBTree RBTree;

// Red-Black tree struct
struct RBTree{

    RBNode* nil; // nil ptr
    RBNode* root; // root ptr
    RBNode* min_vruntime; // min_vruntime ptr

};

//////////////////
// RBTree Methods:
//////////////////

// creates and returns an empty red-black tree
RBTree* RBTree_create();

// inserts node with key into tree
void RBTree_insert(RBTree* T, unsigned long key);

// fix tree after inserting a node
void insert_fixup(RBTree* T, RBNode* node);

// removes node with key from tree
unsigned long RBTree_remove(RBTree* T, unsigned long key);

// fix tree after removing node
void remove_fixup(RBTree* T, RBNode* node);

// rotate left at node
void rotate_l(RBTree* T, RBNode* node);

// rotate right at node
void rotate_r(RBTree* T, RBNode* node);

// transplant node
void transplant(RBTree* T, RBNode* N1, RBNode* N2);

// get node with key in subtree rooted at node 
RBNode* RBTree_get(RBTree* T, unsigned long key, RBNode* node);

RBNode* RBTree_min(RBTree* T, RBNode* node);