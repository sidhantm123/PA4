#ifndef HBT_H
#define HBT_H

// Define HBT_MIN and HBT_MAX
#define HBT_MIN -268435456
#define HBT_MAX 268435455

// Define the tree node structure
typedef struct _Tnode {
    int key;
    int balance;
    struct _Tnode *left;
    struct _Tnode *right;
} Tnode;

#endif /* HBT_H */
