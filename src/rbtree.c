#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>

typedef struct LLNode
{
  const node_t *data;
  struct LLNode *next;
} LLNode;

typedef struct
{
  LLNode *top;
} Stack;

void printTree(rbtree *t, node_t *cur, int level, int isLeft)
{
  if (cur == t->nil)
  {
    return;
  }

  // 오른쪽 자식 노드 출력
  printTree(t, cur->right, level + 1, 0);

  // 현재 노드 출력
  for (int i = 0; i < level - 1; i++)
  {
    printf("    ");
  }
  if (level > 0)
  {
    printf(isLeft ? " \\_ " : " /⎺ "); // 왼쪽 자식일 경우 "\\" 출력, 오른쪽 자식일 경우 "/" 출력
  }
  if (cur->color == RBTREE_RED)
  {
    printf("\x1b[31m%d\x1b[0m\n", cur->key);
  }
  else
  {
    printf("%d\n", cur->key);
  }

  // 왼쪽 자식 노드 출력
  printTree(t, cur->left, level + 1, 1);
}

Stack *new_stack(void)
{
  Stack *s = malloc(sizeof(Stack));
  s->top = NULL;

  return s;
}

int isEmpty(Stack *s)
{
  return !s->top;
}

void push(Stack *s, const node_t *node)
{
  LLNode *new_node = malloc(sizeof(LLNode));
  if (!new_node)
  {
    printf("%s", "Failed to initialize a new node");
    return;
  }

  new_node->data = node;
  new_node->next = s->top;
  s->top = new_node;
}

const node_t *pop(Stack *s)
{
  if (!s->top)
    return NULL;

  LLNode *pop_node = s->top;
  const node_t *data_node = pop_node->data;

  s->top = s->top->next;
  free(pop_node);

  return data_node;
}

const node_t *peek(Stack *s)
{
  if (isEmpty(s))
    return NULL;

  return s->top->data;
}

rbtree *new_rbtree(void)
{
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  if (!p)
    return NULL;

  p->nil = malloc(sizeof(node_t));
  p->nil->color = RBTREE_BLACK;
  p->nil->left = p->nil;
  p->nil->right = p->nil;
  p->nil->parent = p->nil;
  p->root = p->nil;

  return p;
}

void delete_rbtree(rbtree *t)
{
  // TODO: reclaim the tree nodes's memory
  if (!t)
    return;

  Stack *s = new_stack();
  node_t *current = t->root;
  node_t *last_visited = NULL;

  while (!isEmpty(s) || current != t->nil)
  {
    if (current != t->nil)
    {
      push(s, current);
      current = current->left;
    }
    else
    {
      node_t *peek_node = peek(s);
      if (peek_node->right != t->nil && peek_node->right != last_visited)
      {
        current = peek_node->right;
      }
      else
      {
        peek_node = pop(s);
        last_visited = peek_node;
        free(peek_node);
      }
    }
  }

  free(s);
  free(t->nil);
  free(t);
}

void left_rotate(rbtree *t, node_t *x)
{
  if (!t || !x)
    return;

  node_t *y = x->right; // y = right child of x

  // y's left subtree is becoming x's right subtree
  x->right = y->left;
  if (y->left != t->nil)
    y->left->parent = x;

  // x's parent is y's parent
  y->parent = x->parent;
  if (x->parent == t->nil)
    t->root = y;
  else if (x == x->parent->left)
    x->parent->left = y;
  else
    x->parent->right = y;

  // put x on y's left
  y->left = x;
  x->parent = y;
}

void right_rotate(rbtree *t, node_t *x)
{
  if (!t || !x)
    return;

  node_t *y = x->left;

  x->left = y->right;
  if (y->right != t->nil)
    y->right->parent = x;
  y->parent = x->parent;

  if (x->parent == t->nil)
    t->root = y;

  else if (x == x->parent->right)
    x->parent->right = y;

  else
    x->parent->left = y;

  y->right = x;
  x->parent = y;
}

void rbtree_insert_fixup(rbtree *t, node_t *z)
{
  while (z->parent->color == RBTREE_RED)
  {
    if (z->parent == z->parent->parent->left)
    {
      node_t *y = z->parent->parent->right;

      if (y->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else
      { // should wrap all the right_rotate
        if (z == z->parent->right)
        {
          z = z->parent;
          left_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    }
    else
    {
      node_t *y = z->parent->parent->left;

      if (y->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else
      {
        if (z == z->parent->left)
        {
          z = z->parent;
          right_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
  // TODO: implement insert
  if (!t)
  {
    printf("No tree exists\n");
    return NULL;
  }

  node_t *x = t->root;
  node_t *y = t->nil;
  node_t *z = calloc(1, sizeof(node_t));

  if (!z)
  {
    printf("Allocation for a new node failed\n");
    return NULL;
  }
  z->key = key;
  z->parent = t->nil;

  while (x != t->nil)
  {
    y = x;
    if (z->key < x->key)
      x = x->left;
    else
      x = x->right;
  }
  z->parent = y;

  if (y == t->nil)
    t->root = z;
  else if (z->key < y->key)
    y->left = z;
  else
    y->right = z;
  z->color = RBTREE_RED;
  z->left = t->nil;
  z->right = t->nil;
  rbtree_insert_fixup(t, z);

  return z;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
  // TODO: implement find
  if (!t || !t->root)
    return NULL;

  node_t *current = t->root;

  while (current != t->nil)
  {
    if (current->key == key)
      return current;
    else if (key > current->key)
      current = current->right;
    else
      current = current->left;
  }

  return NULL;
}

node_t *rbtree_min(const rbtree *t)
{
  // TODO: implement find
  if (!t || !t->root)
    return NULL;

  node_t *current = t->root;
  while (current->left != t->nil)
    current = current->left;

  return current;
}

node_t *rbtree_max(const rbtree *t)
{
  // TODO: implement find
  if (!t || !t->root)
    return NULL;

  node_t *current = t->root;
  while (current->right != t->nil)
    current = current->right;

  return current;
}

void rb_transplant(rbtree *t, node_t *u, node_t *v)
{
  if (u->parent == t->nil)
    t->root = v;
  else if (u == u->parent->left)
    u->parent->left = v;
  else
    u->parent->right = v;
  v->parent = u->parent;
}

node_t *find_inord_successor(rbtree *t, const node_t *z)
{
  node_t *cur = z;
  if (cur == t->nil)
    return cur;

  while (cur->left != t->nil)
    cur = cur->left;

  return cur;
}

void rb_delete_fixup(rbtree *t, node_t *x)
{
  while (x != t->root && x->color == RBTREE_BLACK)
  {
    if (x == x->parent->left)
    {
      node_t *w = x->parent->right;

      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }

      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }

      else
      {
        if (w->right->color == RBTREE_BLACK)
        {
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w = x->parent->right;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    }

    else
    {
      node_t *w = x->parent->left;

      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else
      {
        if (w->left->color == RBTREE_BLACK)
        {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *p)
{
  // TODO: implement erase
  if (!p)
    return -1;

  node_t *y = p;
  color_t y_original_color = y->color;
  node_t *x;

  if (p->left == t->nil)
  {
    x = p->right;
    rb_transplant(t, p, p->right);
  }
  else if (p->right == t->nil)
  {
    x = p->left;
    rb_transplant(t, p, p->left);
  }
  else
  {
    y = find_inord_successor(t, p->right);
    y_original_color = y->color;
    x = y->right;

    if (y->parent == p)
      x->parent = y;
    else
    {
      rb_transplant(t, y, y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    rb_transplant(t, p, y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }
  if (y_original_color == RBTREE_BLACK)
    rb_delete_fixup(t, x);

  free(p);
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  // TODO: implement to_array
  // inorder_traversal
  Stack *s = calloc(1, sizeof(Stack));
  int idx = 0;
  node_t *current = t->root;

  while (!isEmpty(s) || current != t->nil)
  {
    while (current != t->nil)
    {
      push(s, current);
      current = current->left;
    }

    if (idx >= n)
    {
      free(s);
      return NULL;
    }
    current = pop(s);
    arr[idx++] = current->key;
    current = current->right;
  }

  free(s);
  return 0;
}
