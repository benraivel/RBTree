/**
 * red black tree implementation
 * 
 * Ben Raivel
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "rbtree.h"

// create a node with key
RBNode* RBNode_create(unsigned long key){

    // allocate memory for node struct
    struct RBNode* node = malloc(sizeof(struct RBNode));
    
    // set key
    node->key = key;

    return node;
}

// true if node is a left child
bool is_lchild(RBNode* node){
    
    // check if node is the left child of it's parent
    if(node->parent->lchild == node){
        return true; // node is the left child
    }
    else{ // otherwise
        return false; // node is not the left child
    }
}

// gets the uncle
RBNode* get_uncle(RBNode* node){

    // if node's parent is a left child
    if(is_lchild(node->parent)){
        return node->parent->parent->rchild; // uncle is a right child
    }
    else{ // otherwise
        return node->parent->parent->lchild; // uncle is a left child
    }
}

// creates an empty RBTree, returns pointer
RBTree* RBTree_create(){

    // allocate memory for rbtree struct
    struct RBTree* tree = malloc(sizeof(struct RBTree));

    // create nil node with key 0
    tree->nil = RBNode_create(0);

    // set nil to be its own parent, children
    tree->nil->parent = tree->nil;
    tree->nil->lchild = tree->nil;
    tree->nil->rchild = tree->nil;

    // make root nil
    tree->root = tree->nil;

    //make min_vruntim
    tree->min_vruntime = tree->nil;
    
    // return pointer to rbtree struct
    return tree;
}

// inserts new node with key into RBTree
void RBTree_insert(RBTree* T, unsigned long key){
    
    // create a new node with key
    RBNode* new_node = RBNode_create(key);

    // if key is lower than min_vruntime
    if(T->min_vruntime == T->nil){
        T->min_vruntime = new_node;
    }
    else if(key < T->min_vruntime->key){
        T->min_vruntime = new_node; // update min_vruntime
    }

    // Starting at the root
    RBNode* current = T->root;
    RBNode* prev = T->nil;

    // Traverse tree until a leaf is reached 
    while(current != T->nil){

        prev = current; // update prev to current
        
        // compare keys
        if(new_node->key < current->key){
            current = current->lchild; // new_node belongs on current's left
        }
        else{
            current = current->rchild; // new_node belongs on current's right
        }
    }
    // prev becomes parent of new_node
    new_node->parent = prev;

    // special case: tree is empty
    if(prev == T->nil){
        T->root = new_node; // new_node becomes the root
    }
    else{ // tree is not empty, compare keys
        if(new_node->key < prev->key){
            prev->lchild = new_node; // new_node is left child
        }
        else{      
            prev->rchild = new_node; // new_node is right child
        }
    }

    // set new_node's children to be nil
    new_node->lchild = T->nil;
    new_node->rchild = T->nil;

    // color red
    new_node->red = true;

    // call RBTree_fixup
    insert_fixup(T, new_node);
}

// fix tree after inserting a node
void insert_fixup(RBTree* T, RBNode* node){

    // loop while node's parent is red
    while(node->parent->red){

        // get node's uncle
        RBNode* uncle = get_uncle(node);

        // case 1: uncle is red
        if(uncle->red){

            // color parent, uncle black, grandparent red
            node->parent->red = false;
            uncle->red = false;
            node->parent->parent->red = true;

            // set node to be it's grandparent
            node = node->parent->parent;
        }

        else{ // check if node forms triangle

            // triangle pointing right
            if(is_lchild(node) && !is_lchild(node->parent)){
                node = node->parent;
                rotate_l(T, node->parent);
            }

            // triangle pointing left
            else if(!is_lchild(node) && is_lchild(node->parent)){
                node = node->parent;
                rotate_r(T, node->parent);
            }

            // color parent black, grandparent red
            node->parent->red = false;
            node->parent->parent->red = true;

            // rotate grandparent
            if(is_lchild(node))
                rotate_r(T, node->parent->parent);
            else
                rotate_l(T, node->parent->parent);
        }
    }

    // color root black
    T->root->red = false;
}

// removes node with key
unsigned long RBTree_remove(RBTree* T, unsigned long key){

    // get node associated with key
    RBNode* node = RBTree_get(T,key,T->root);

    // if node being removed is min_vruntime
    if(node == T->min_vruntime){

        // if node is root
        if(node->parent == T->nil)
            T->min_vruntime = T->nil; // min_vruntime in nil

        // if node has no right child
        else if(node->rchild == T->nil)
            T->min_vruntime = node->parent; // min_vruntime is node's parent

        else // node has a right child
            T->min_vruntime = node->rchild; // min_vruntime is node's right child
    }

    // two temporary node pointers
    RBNode* t1 = node;
    RBNode* t2;

    // is t1 red?
    bool t1_is_red = t1->red;

    // if node's left child is nil
    if(node->lchild == T->nil){
        t2 = node->rchild;
        transplant(T, node, node->rchild);
    }

    // if node's right child is nil
    else if(node->rchild == T->nil){
        t2 = node->lchild;
        transplant(T, node, node->lchild);
    }
    
    else{
        t1 = RBTree_min(T, node->rchild);
        t1_is_red = t1->red;
        t2 = t1->rchild;

        if(t1->parent == node)
            t2->parent = node;
        else{
            transplant(T, t1, t1->rchild);
            t1->rchild = node->rchild;
            t1->rchild->parent = t1;
        }
        transplant(T, node, t1);
        t1->lchild = node->lchild;
        t1->lchild->parent = t1;

        if(node->red)
            t1->red = true;
        else
            t1->red = false;
    }

    if(!t1_is_red)
        remove_fixup(T, t2);

    // temp variable to hold key
    unsigned long return_val = node->key;

    // free node
    free(node);

    return return_val;
}

// fix tree after removing a node
void remove_fixup(RBTree* T, RBNode* node){

    // pointer to sibling
    RBNode* sibling;

    // loop while node is non-root and doubly black
    while(node != T->nil && !node->red){
        
        // two symetrical situations:
        if(is_lchild(node)){ // 1 - node is a left child
            
            // => sibling is right child
            sibling = node->parent->rchild;

            // case 1: sibling is red
            if(sibling->red){ 
                sibling->red = false;               // case 1
                sibling->parent->red = false;       // case 1
                rotate_l(T, node->parent);          // case 1

                sibling = node->parent->rchild;     // now have case 2, 3, or 4
            }

            // case 2: both children are black
            if(!sibling->lchild->red && !sibling->rchild->red){

                sibling->red = true;                // case 2

                node = node->parent;                // now have case 3 or 4
            }
            else{
                // case 3: sibling's right child is red, left child is black
                if(!sibling->rchild->red){          
                    sibling->lchild->red = false;   // case 3
                    sibling->red = true;            // case 3
                    rotate_l(T, sibling);           // case 3

                    sibling = node->parent->rchild; // now have case 4
                }

                // case 4: sibling's right child is red
                if(node->parent->red)
                    sibling->red = true;            // case 4

                else
                    sibling->red = false;           // case 4

                node->parent->red = false;          // case 4
                sibling->rchild->red = false;       // case 4
                rotate_l(T, node->parent);          // case 4

                node = T->root;
            }
        }
        
        else{ // 2 - node is a right child
            // => sibling is left child
            sibling = node->parent->lchild;

            // case 1: sibling is red
            if(sibling->red){ 
                sibling->red = false;               // case 1
                sibling->parent->red = false;       // case 1
                rotate_r(T, node->parent);          // case 1

                sibling = node->parent->lchild;     // now have case 2, 3, or 4
            }

            // case 2: both children are black
            if(!sibling->lchild->red && !sibling->rchild->red){

                sibling->red = true;                // case 2

                node = node->parent;                // now have case 3 or 4
            }
            else{
                // case 3: sibling's right child is red, left child is black
                if(!sibling->lchild->red){          
                    sibling->rchild->red = false;   // case 3
                    sibling->red = true;            // case 3
                    rotate_r(T, sibling);           // case 3

                    sibling = node->parent->lchild; // now have case 4
                }

                // case 4: sibling's right child is red
                if(node->parent->red)
                    sibling->red = true;            // case 4

                else
                    sibling->red = false;           // case 4

                node->parent->red = false;          // case 4
                sibling->lchild->red = false;       // case 4
                rotate_r(T, node->parent);          // case 4

                node = T->root;
            }
        }
    }
    node->red = false;
}

// rotate left at node
void rotate_l(RBTree* T, RBNode* node){

    // set pivot to be the right child of node
    RBNode* pivot = node->rchild;

    // set node's right child to bo pivot's left
    node->rchild = pivot->lchild;

    // set pivots's left child's parent to node
    pivot->lchild->parent = node;

    // pivot's parent becomes node's parent
    pivot->parent = node->parent;

    // if node was root
    if(node->parent == T->nil){
        T->root = pivot; // pivot becomes root
    }
    else{ // otherwise
        if(is_lchild(node)){

            // pivot becomes left child of node's parent
            node->parent->lchild = pivot;
        }
        else{ // node is right child

            // pivot becomes right child of node's parent
            node->parent->rchild = pivot;
        }
    }
    pivot->lchild = node;
    node->parent = pivot;
}

// rotate right at node
void rotate_r(RBTree* T, RBNode* node){
    
    // set pivot to be the left child of node
    RBNode* pivot = node->rchild;

    // set node's left child to bo pivot's right
    node->lchild = pivot->rchild;

    // set pivots's right child's parent to node
    pivot->rchild->parent = node;

    // pivot's parent becomes node's parent
    pivot->parent = node->parent;

    // if node was root
    if(node->parent == T->nil){
        T->root = pivot; // pivot becomes root
    }
    else{ // otherwise
        if(is_lchild(node)){

            // pivot becomes left child of node's parent
            node->parent->lchild = pivot;
        }
        else{ // node is right child

            // pivot becomes right child of node's parent
            node->parent->rchild = pivot;
        }
    }
    pivot->rchild = node;
    node->parent = pivot;
}

// transplant
void transplant(RBTree* T, RBNode* N1, RBNode* N2){

    // special case: node is root
    if(N1->parent == T->nil){
        T->root = N2;
    }

    else if(is_lchild(N1)){
        N1->parent->lchild = N2;
    }

    else{
        N1->parent->rchild = N2;
    }

    N2->parent = N1->parent;
}

// get node with key in subtree rooted at node 
RBNode* RBTree_get(RBTree* T, unsigned long key, RBNode* node){
    
    // if a nil node is reached
    if(node == T->nil){
        return NULL;    // node is not in tree
    }

    // if keys match...
    else if(key == node->key){
        return node;    // node found!
    }

    // if target is less than node->key
    else if(key < node->key){

        // call recursivley on lchild
        return RBTree_get(T, key, node->lchild);
    }
    else{ // target is greater than node->key

        // call recursivley on rchild
        return RBTree_get(T, key, node->rchild);
    }
}

// get the minimum node of the subtree rooted at node
RBNode* RBTree_min(RBTree* T, RBNode* node){
    if(node->lchild == T->nil)
        return node;
    else
        return RBTree_min(T, node->lchild);
}

int main(){
    RBTree* tree = RBTree_create();
    for(int i = 0; i < 10; i++){
        RBTree_insert(tree, i);
    }
    for(int i = 9; i >=0; i--){
        printf("Removed node: %lu\n", RBTree_remove(tree, i));
    }

    return 0;
}