/*
 * bistree.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/bool.h"
#include "../lib/bistree.h"
#include "../lib/heap.h"
#include "../lib/ascii_tree.h"

void bistree_init(BisTree* tree) {
    tree->root = NULL;
    tree->size = 0;
    tree->to_clean=0;
    return;
}

bool bitreenode_lookup(BiTreeNode* node, int data) {
   if (node == NULL)
      return false;
   if (data < node->data)
      return bitreenode_lookup(node->left, data);
   if (data > node->data)
      return bitreenode_lookup(node->right, data);
   return true;
}

bool bistree_lookup(BisTree* tree, int data) {
    return bitreenode_lookup(tree->root, data);
}

BiTreeNode* bitreenode_insert(BiTreeNode* node, int data, BiTreeNode* nnode) {
    if (node == NULL) {
        BiTreeNode* node = nnode;
        node->data = data;
        node->left = NULL;
        node->right= NULL;
        return node;
    }
    else if(data < node->data)
        node->left = bitreenode_insert(node->left, data, nnode);
    else
        node->right= bitreenode_insert(node->right, data, nnode);
    return node;
}

bool bistree_insert(BisTree* tree, int data) {
    if (bistree_lookup(tree, data))
        return false;

    BiTreeNode* node = (BiTreeNode*)my_malloc(sizeof(BiTreeNode));
    if (node == NULL)
    {
        printf("Exiting Gracefully :)\n");
        exit(1);
    }
    tree->root = bitreenode_insert(tree->root, data, node);
    tree->size = tree->size + 1;
    //bistree_inorder(tree);
    return true;
}

BiTreeNode* bitreenode_remove(BiTreeNode* node, int data) {
   if(data < node->data)
      node->left = bitreenode_remove(node->left, data);
   else if(data > node->data)
      node->right = bitreenode_remove(node->right, data);
   else if(node->left == NULL)
      node = node->right;
   else if(node->right == NULL)
      node = node->left;
   else {
     BiTreeNode* lnode = node->left;
     while(lnode->right != NULL)
        lnode = lnode->right;
     node->data = lnode->data;
     node->left = bitreenode_remove(node->left, lnode->data);
     //free(lnode);
   }
   return node;
}

bool bistree_remove(BisTree* tree, int data) {
   if (!bistree_lookup(tree, data))
      return false;
   tree->root = bitreenode_remove(tree->root, data);
   tree->size = tree->size - 1;
   tree->to_clean = tree->to_clean +1;
   return true;
}

void bitreenode_inorder(BiTreeNode* node) {
   if(node == NULL)
      return;
   bitreenode_inorder(node->left);
   printf(" %d ", node->data);
   bitreenode_inorder(node->right);
}

void bistree_inorder(BisTree* tree) {
   printf("root: %p\n", tree);
   bitreenode_inorder(tree->root);
   printf("\n");
}

void bitreenode_fancy(char* prefix, BiTreeNode* node, bool isLeft)
{

    char nprefix[100];
    strcpy(nprefix, prefix);
    printf("%s",nprefix);
    
    if(isLeft)
        printf( "├──");
    else
        printf("└──" );

    // print the value of the node
    if( node == NULL){
        printf("\n");
        return;
    }
    printf("%d\n",node->data);

    
    if(isLeft)
        strcat(nprefix,"│   ");
    else 
        strcat(nprefix,"    ");

    // enter the next tree level - left and right branch
    bitreenode_fancy( nprefix, node->left, true);
    bitreenode_fancy( nprefix, node->right, false);
}

void bistree_fancy(BisTree* t)
{
    //bitreenode_fancy("", t->root, false);    
    print_ascii_tree(t->root);
}
