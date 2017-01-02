#include "tree.h"
#include <assert.h>
#include <stdlib.h>

// For testing
#include <stdio.h>
#include <string.h>

#ifdef GC_TEST
extern heap_t *h; // Assume single heap for this program for simplicity
#endif

typedef struct node node;
typedef void (*tree_internal_visitor_func)(node *n, void *ptr);

struct tree
{
  struct node *root;
  cmp_func key_cmp;
};

struct record
{
  void *key;
  void *value;
};

struct dump
{
  uint32_t size;
  struct record *data;
};

struct node
{
  void *key;
  void *value;
  struct node *left;
  struct node *right;
};

static node *tree_internal_node_new(void *key, void *value);
static inline node   **tree_internal_find(tree *tree, void *key);
static inline uint32_t tree_internal_size(node *node);
static inline uint32_t tree_internal_max(uint32_t a, uint32_t b);
static inline uint32_t tree_internal_depth(node *node);
static void tree_internal_dump_func(void *key, void *value, void *dump);
static inline void tree_internal_populate(tree *t, struct record *d, int size);
static inline void tree_internal_node_visit(node *n, enum traversal_order order, tree_internal_visitor_func f, void *ptr);
static inline void tree_internal_visit(tree *t, enum traversal_order order, tree_internal_visitor_func f, void *ptr);
                                
void tree_path_for_key(tree *t, void *key, char **buffer)
{
  node *n = t->root;

#ifdef GC_TEST
  char *path = *buffer ? *buffer : h_alloc_data(h, tree_depth(t) + 1);
#else
  char *path = *buffer ? *buffer : malloc(tree_depth(t) + 1);
#endif
  char *start = path;
  
  while (n)
    {
      if (t->key_cmp(key, n->key) < 0)
        {
          *path++ = 'L';
          n = n->left;
        }
      else if (t->key_cmp(key, n->key) > 0)
        {
          *path++ = 'R';
          n = n->right;
        }
      else 
        {
          *path++ = '\0';

          if (*buffer == NULL) *buffer = start;
          return;
        }
    }
  assert(false);
}

void *tree_key_for_path(tree *t, char *path)
{
  node *n = t->root;
  for (; *path; ++path)
    {
      switch (*path)
        {
        case 'l':
        case 'L':
          {
            n = n->left;
            break;
          }
        case 'r':
        case 'R':
          {
            n = n->right;
            break;
          }
        default:
          {
            assert(false);
          }
        }
    }
  return n->key;
}

char *tree_walk(tree *t, char *path)
{
  node *n = t->root;
  while (*path && n)
    {
      switch (*path)
        {
        case 'l':
        case 'L':
          {
            n = n->left;
            break;
          }
        case 'r':
        case 'R':
          {
            n = n->right;
            break;
          }
        default:
          {
            assert(false);
          }
        }

      if (n) ++path;
    }
  return path;
}

static node *tree_internal_node_new(void *key, void *value)
{
#ifdef GC_TEST
  node *result = h_alloc_struct(h, "****");
#else
  node *result = malloc(sizeof(*result));
#endif
  *result = (struct node) { .key = key, .value = value };
  return result;
}

tree *tree_new(cmp_func key_cmp)
{
#ifdef GC_TEST
  tree *result = h_alloc_struct(h, "**");
#else
  tree *result = malloc(sizeof(*result));
#endif
  *result = (struct tree) { .key_cmp = key_cmp };
  return result;
}

static inline void tree_internal_node_visit(node *n, enum traversal_order order, tree_internal_visitor_func f, void *ptr)
{
  if (n)
    {
      switch (order)
        {
        case postorder:
          {
            tree_internal_node_visit(n->left, order, f, ptr);
            tree_internal_node_visit(n->right, order, f, ptr);
            f(n, ptr);
            break;
          }
        case preorder:
          {
            f(n, ptr);
            tree_internal_node_visit(n->left, order, f, ptr);
            tree_internal_node_visit(n->right, order, f, ptr);
            break;
          }
        case inorder:
          {
            tree_internal_node_visit(n->left, order, f, ptr);
            f(n, ptr);
            tree_internal_node_visit(n->right, order, f, ptr);
            break;
          }
        default: /// Defensive
          assert(false);
        }
    }
}

static void node_visit(node *n, enum traversal_order order, tree_visitor_func f, void *ptr)
{
  if (n)
    {
      switch (order)
        {
        case postorder:
          {
            node_visit(n->left, order, f, ptr);
            node_visit(n->right, order, f, ptr);
            f(n->key, n->value, ptr);
            break;
          }
        case preorder:
          {
            f(n->key, n->value, ptr);
            node_visit(n->left, order, f, ptr);
            node_visit(n->right, order, f, ptr);
            break;
          }
        case inorder:
          {
            node_visit(n->left, order, f, ptr);
            f(n->key, n->value, ptr);
            node_visit(n->right, order, f, ptr);
            break;
          }
        default: /// Defensive
          assert(false);
        }
    }
}

void tree_visit(tree *t, enum traversal_order order, tree_visitor_func f, void *ptr)
{
  node_visit(t->root, order, f, ptr);
}

static inline void tree_internal_visit(tree *t, enum traversal_order order, tree_internal_visitor_func f, void *ptr)
{
  tree_internal_node_visit(t->root, order, f, ptr);
}

static inline void tree_internal_delete_func(node *node, tree_visitor_func f)
{
  if (f) f(node->key, node->value, NULL);
  free(node);
}

void tree_delete_visit(tree *t, tree_visitor_func f)
{
  tree_internal_visit(t, postorder, (tree_internal_visitor_func) tree_internal_delete_func, f);
  free(t);
}

void tree_delete(tree *t)
{
  tree_delete_visit(t, NULL);
}

static inline node **tree_internal_find(tree *t, void *key)
{
  node **n = &(t->root);
  while (*n)
    {
      if (t->key_cmp(key, (*n)->key) < 0)
        {
          n = &((*n)->left);
        }
      else if (t->key_cmp(key, (*n)->key) > 0)
        {
          n = &((*n)->right);
        }
      else
        {
          return n;
        }
    }
  return n;
}

void *tree_insert(tree *t, void *key, void *value)
{
  node **n = tree_internal_find(t, key);
  void *old_value = NULL;
  if (*n)
    {
      old_value = (*n)->value;
      (*n)->value = value;
    }
  else
    {
      *n = tree_internal_node_new(key, value);
    }
  return old_value;
}

bool tree_contains(tree *t, void *key)
{
  return *tree_internal_find(t, key);
}

static inline bool tree_internal_node_has_left(node *n)
{
  return n->left != NULL;
}

static inline bool tree_internal_node_has_right(node *n)
{
  return n->right != NULL;
}

static inline void tree_internal_node_remove_from_tree(node **pp)
{
  node *n = *pp;

  if (tree_internal_node_has_left(n) && tree_internal_node_has_right(n))
    { 
      node *cursor = n->left;
      while (cursor->right)
        {
          cursor = cursor->right; 
        }
      
      cursor->right = n->right; 

      *pp = n->left;

      n->left = NULL;
      n->right = NULL;
    }
  else if (tree_internal_node_has_left(n))
    {
      *pp = n->left;
      n->left = NULL;
    }
  else if (tree_internal_node_has_right(n))
    {
      *pp = n->right;
      n->right = NULL;
    }
  else
    {
      *pp = NULL;
    }
}

bool tree_remove(tree *t, void *key)
{
  node **n = tree_internal_find(t, key);

  if (*n)
    {
      node *to_remove = *n;
      tree_internal_node_remove_from_tree(n);
      free(to_remove);
      return true;
    }
  else
    {
      return false;
    }
}

static inline uint32_t tree_internal_size(node *node)
{
  return node ? 1 + tree_internal_size(node->left) + tree_internal_size(node->right) : 0;
}

uint32_t tree_size(tree *t)
{
  return tree_internal_size(t->root);
}

static inline uint32_t tree_internal_max(uint32_t a, uint32_t b)
{
  return a < b ? b : a;
}

static uint32_t tree_internal_depth(node *node)
{
  return node ? 1+ tree_internal_max(tree_internal_depth(node->left),
                                     tree_internal_depth(node->right)) : 0;
}

uint32_t tree_depth(tree *t)
{
  return tree_internal_depth(t->root);
}

static void tree_internal_dump_func(void *key, void *value, void *dump)
{
  struct dump *d = dump;
  d->data[d->size++] = (struct record) { .key = key, .value = value };
}

void tree_internal_populate(tree *t, struct record *d, int size)
{
  if (size > 0)
    {
      int pivot  = size / 2;
      int adjust = 1 - size % 2;

      tree_insert(t, d[pivot].key, d[pivot].value);

      tree_internal_populate(t, d, pivot);
      tree_internal_populate(t, d + pivot + 1, pivot - adjust);
    }
}

void tree_balance(tree *t)
{
#ifdef GC_TEST
  /// Since pointers to the middle of the data is not supported in
  /// the GC, must make sure this function is not called when GC
  /// is used
  assert(false);
#endif
  if (tree_size(t) < 3) return;

  struct dump dump = (struct dump) { .data = calloc(tree_size(t), sizeof(struct record)) };
  tree_visit(t, inorder, tree_internal_dump_func, &dump);

  /// Create new tree, to be deleted
  tree *new_tree = tree_new(t->key_cmp);
  tree_internal_populate(new_tree, dump.data, dump.size);

  node *old_root = t->root;
  t->root = new_tree->root;
  new_tree->root = old_root; 

  free(dump.data);
  tree_delete(new_tree);
}

struct depth_cache
{
  uint32_t depth;
  void *key;
};

static int tree_internal_calculate_depths(node *n, uint32_t *max_skew)
{
  if (n)
    {
      uint32_t depth_left = tree_internal_calculate_depths(n->left, max_skew);
      uint32_t depth_right = tree_internal_calculate_depths(n->right, max_skew);

      if (depth_right < depth_left)
        {
          uint32_t skew = depth_left - depth_right;
          if (skew > *max_skew) *max_skew = skew;
          return depth_left + 1;
        }
      else
        {
          uint32_t skew = depth_right - depth_left;
          if (skew > *max_skew) *max_skew = skew;
          return depth_right + 1;
        }
    }

  return 0;
}

void tree_internal_consistency_check(tree *t)
{
  uint32_t max_skew = 0;
  tree_internal_calculate_depths(t->root, &max_skew);
  fprintf(stderr, "Skew for tree %p is %d\n", (void *)t, max_skew);
}

void *tree_put(tree *tree, void *key, void *value)
{
  return tree_insert(tree, key, value);
}

void *tree_get(tree *tree, void *key)
{
  node **n = tree_internal_find(tree, key);
  if (*n)
    {
      return (*n)->value;
    }
  else
    {
      return NULL;
    }
}
