#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treemap.h"

typedef struct TreeNode TreeNode;

struct TreeNode {
    Pair* pair;
    TreeNode * left;
    TreeNode * right;
    TreeNode * parent;
};

struct TreeMap {
    TreeNode * root;
    TreeNode * current;
    int (*lower_than) (void* key1, void* key2);
};

int is_equal(TreeMap* tree, void* key1, void* key2){
    if(tree->lower_than(key1,key2)==0 &&  
        tree->lower_than(key2,key1)==0) return 1;
    else return 0;
}


TreeNode * createTreeNode(void* key, void * value) {
    TreeNode * new = (TreeNode *)malloc(sizeof(TreeNode));
    if (new == NULL) return NULL;
    new->pair = (Pair *)malloc(sizeof(Pair));
    new->pair->key = key;
    new->pair->value = value;
    new->parent = new->left = new->right = NULL;
    return new;
}


TreeMap * createTreeMap(int (*lower_than) (void* key1, void* key2)) {
    TreeMap * mapa = (TreeMap *) malloc(sizeof(TreeMap));
    if (mapa == NULL) return NULL; //Por si falla el malloc.
    mapa->root = NULL;
    mapa->current = NULL;
    mapa->lower_than = lower_than;
    return mapa;
}

void insertTreeMap(TreeMap * tree, void* key, void * value) {
    TreeNode * aux = tree->root;
    TreeNode * padre = NULL;

    //Busqueda para encontrar dónde debería ubicarse el dato.
    while (aux != NULL) {
        padre = aux;
        if (is_equal(tree, key, aux->pair->key)) return;
        if (tree->lower_than(key, aux->pair->key)) 
            aux = aux->left;
        else aux = aux->right;
    }
    //Se crea el nuevo nodo y se enlaza.
    TreeNode * n = createTreeNode(key, value);
    n->parent = padre;
    if (tree->lower_than(key, padre->pair->key))
        padre->left = n;
    else 
        padre->right = n;
    tree->current = n;
}

TreeNode * minimum(TreeNode * x){
    if (x == NULL) return NULL;
    while (x->left != NULL) {
        x = x->left;
    }
    return x;
}

void removeNode(TreeMap * tree, TreeNode* node) {
    //SIN HIJOS
    if (node->left == NULL && node->right == NULL) {
        if (node->parent == NULL)
            tree->root = NULL;
        else if (node->parent->left == node)
            node->parent->left = NULL;
        else
            node->parent->right = NULL;
        free(node->pair);
        free(node);
    } //UN HIJO
    else if (node->left == NULL || node->right == NULL) {
        TreeNode* child;
        if (node->left != NULL) 
            child = node->left;
        else 
            child = node->right;
        
        child->parent = node->parent;
        if (node->parent == NULL)
            tree->root = child;
        else if (node->parent->left == node)
            node->parent->left = child;
        else
            node->parent->right = child;
        free(node->pair);
        free(node);
    } //DOS HIJOS
    else {
        TreeNode * min = minimum(node->right);
        node->pair = min->pair;
        removeNode(tree, min);
    }

}

void eraseTreeMap(TreeMap * tree, void* key){
    if (tree == NULL || tree->root == NULL) return;

    if (searchTreeMap(tree, key) == NULL) return;
    TreeNode* node = tree->current;
    removeNode(tree, node);

}

Pair * searchTreeMap(TreeMap * tree, void* key) {
    TreeNode * aux = tree->root;
    while (aux != NULL) {
        if (is_equal(tree, key, aux->pair->key)) {
            tree->current = aux;
            return aux->pair;
        } else if (tree->lower_than(key, aux->pair->key)) {
            aux = aux->left;
        } else {
            aux = aux->right;
        }
    }
    return NULL;
}

Pair * upperBound(TreeMap * tree, void* key) {
    TreeNode * aux = tree->root;
    TreeNode * ub_node = NULL;
    while (aux != NULL) {
        if (is_equal(tree, key, aux->pair->key)) {
            tree->current = aux;
            return aux->pair;
        } else if (tree->lower_than(key, aux->pair->key)) {
            ub_node = aux;
            aux = aux->left;
        } else {
            aux = aux->right;
        }
    }
    if (ub_node != NULL) {
        return ub_node->pair;
    } else {
        return NULL;
    }
}

Pair * firstTreeMap(TreeMap * tree) {
    if (tree->root == NULL) return NULL;
    TreeNode * minNodo = minimum(tree->root);
    tree->current = minNodo;
    return minNodo->pair;
}

Pair * nextTreeMap(TreeMap * tree) {
    if (tree->current == NULL) return NULL;
    TreeNode * current = tree->current;

    //Si el current tiene hijo derecho.
    if (current->right != NULL) {
        TreeNode * aux = current->right;
        while (aux->left !=NULL)
            aux = aux->left;
        tree->current = aux;
        return aux->pair;
    }
    //Si el current no tiene hijo derecho.
    TreeNode * aux = current->parent;
    while (aux != NULL && aux->pair->key != NULL && tree->lower_than(current->pair->key, aux->pair->key) == 0) {
        current = aux;
        aux = aux->parent;
    }
    tree->current = aux;
    if (aux == NULL) return NULL;
    return aux->pair;
}
