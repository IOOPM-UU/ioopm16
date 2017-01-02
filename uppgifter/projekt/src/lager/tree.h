#pragma once
#include <stdbool.h>
#include <stdint.h>

enum traversal_order { postorder, preorder, inorder };

typedef struct tree tree;
typedef void (*tree_visitor_func)(void *key, void *value, void *ptr);
typedef int (*cmp_func)(void *a, void *b);

tree *tree_new(cmp_func key_cmp);
void tree_delete(tree *tree);
void tree_delete_visit(tree *t, tree_visitor_func f);

void *tree_insert(tree *tree, void *key, void *value);
bool tree_remove(tree *tree, void *key);
void tree_balance(tree *tree);
void *tree_put(tree *tree, void *key, void *value);
void *tree_get(tree *tree, void *key);

bool tree_contains(tree *tree, void *key);

// 
void tree_visit(tree *tree, enum traversal_order order, tree_visitor_func f, void *ptr);

// Return the size of the tree
uint32_t tree_size(tree *tree);

// Return the max depth of the tree
uint32_t tree_depth(tree *tree);

// Given a path to walk in the tree tree, return the
// pointer to the key for the node at the end of this path.
// See tree_path_for_key for more details.
void *tree_key_for_path(tree *tree, char *path);

// Given a tree t, and a key key in this tree, generate
// a null-terminated string that describes the path leading
// from the root of t to the node with key key. 
//
// For example, imagine the following tree with nodes a - g.
//
//              d
//             / \                                 .
//            c   f
//           / \ / \                               .
//          a  b e  g
//
// Here is a full description of the paths for all nodes in
// this tree:
// Path(t, a) => "LL"
// Path(t, b) => "LR"
// Path(t, c) => "L"
// Path(t, d) => ""
// Path(t, e) => "RL"
// Path(t, f) => "R"
// Path(t, g) => "RR"
// Path(t, x) => NULL (x not in the tree)
//
// A path is a null-terminated string whose grammar is
// path ::= <empty>
//       |  L path
//       |  R path
// (for example "LLRR", "", or "RR" as above). The function
// takes a pointer, path, which is a pointer to a result buffer. 
// Analogous with getline, path may either be NULL or point to
// a buffer with size > Depth(t). If path is NULL, the function
// will allocate a new result buffer, and store it in path. 
//
// The function tree_key_for_path looks up the key for a
// path returned by the current function. The following
// relation is expected to hold:
//
// char *path = NULL;
// tree_path_for_key(t, key, &path)
// assert(path == NULL || tree_key_for_path(t, path) == key)
// 
void tree_path_for_key(tree *tree, void *key, char **path);

// For a given tree t and path p, walk p to the extent possible
// and return the remainder of the path -- i.e., that part that
// could not be walked. Thus, if t is empty, tree_walk will behave
// as the identity function. If t is
//                                    a
//                                   / \                .
//                                  b   c
//
// then p in { "", "L", "R" } will return the string "". If p is
// "LLR", the string "LR" will be returned because after taking a
// left ('L'), arriving at b, no more steps can be taken.
char *tree_walk(tree *t, char *p);
