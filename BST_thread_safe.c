#include <pthread.h>
#include "tlpi_hdr.h"

struct node {
    char key;
    int value;
    struct node *right;
    struct node *left;
    pthread_mutex_t mutex;
};


typedef struct node *node_link;


node_link create_node(char key, int value)
{
    node_link new_node = (node_link) malloc(sizeof(struct node));
    pthread_mutexattr_t attr;
    int s;
    s = pthread_mutexattr_init(&attr);
    if (s != 0)
        errExitEN(s, "attr_init");
    s = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    if (s != 0)
        errExitEN(s, "type_attr");
    s = pthread_mutex_init(&new_node->mutex, &attr);
    if (s != 0)
        errExitEN(s, "mutex_init");
    new_node->key = key;
    new_node->value = value;
    new_node->right = NULL;
    new_node->left = NULL;
    return new_node;
}

node_link add(node_link root, char key, int value)
{
    int s;
    if (root == NULL)
        root = create_node(key, value);
    else {
        s = pthread_mutex_lock(&(root->mutex));
        if (root->key > key)
            root->left = add(root->left, key, value);
        if (root->key < key)
            root->right = add(root->right, key, value);
        if (root->key == key)
            root->value = value;
    }
    s = pthread_mutex_unlock(&(root->mutex));
    return root;
}

void lookup(node_link root, char key)
{
    int s = 0;
    if (root == NULL)
        return;
    s = pthread_mutex_lock(&(root->mutex));
    int a = root->value;
    //printf("BEFORE: value = %d, key = %c  ", root->value, root->key);
    if (root->key == key) {
        if (root->value != a)
            printf("not safe");
        printf("value = %d, key = %c\n", root->value, root->key);
    }
    if (root->key < key)
        lookup(root->right, key);
    if (root->key > key)
        lookup(root->left, key);
    s = pthread_mutex_unlock(&(root->mutex));
    return;
}

void inorder(node_link root)
{
    if (root) {
        inorder(root->left);
        printf("(%c %d)\n", root->key, root->value);
        inorder(root->right);
    }
}

static void *threadFunc1(void *arg)
{
    node_link root = *((node_link *) arg);
    int j;
    for (j = 0; j < 1000; j++)
        lookup(root, 'b');
}

static void *threadFunc2(void *arg)
{
    node_link root = *((node_link *) arg);
    int j;
    for (j = 0; j < 10000; j++)
        add(root, 'b', (root->value)+1);
}

int main()
{
   node_link root = NULL;
   char key = 'a';
   char key2 = 'b';
   char key3 = 'c';
   root = add(root, key2, 2);
   add(root, key, 1);
   add(root, key3, 3);
   pthread_t t1, t2;
   int s;
   node_link arg = root;
   s = pthread_create(&t1, NULL, threadFunc1, &arg);
   if (s != 0)
       errExitEN(s, "pthread_create");
   s = pthread_create(&t2, NULL, threadFunc2, &arg);
   if (s != 0)
       errExitEN(s, "pthread_create");
   s = pthread_join(t1, NULL);
   if (s != 0)
       errExitEN(s, "pthread_join");
   s = pthread_join(t2, NULL);
   if (s != 0)
       errExitEN(s, "pthread_join");


   return 0;
}
