#include <stdio.h>
#include <stdlib.h>
#include "hbt.h"

// Function to perform clockwise rotation
void rotateClockwise(Tnode **node) {
    Tnode *temp = (*node)->left;
    (*node)->left = temp->right;
    temp->right = *node;
    *node = temp;
}

// Function to perform counter-clockwise rotation
void rotateCounterClockwise(Tnode **node) {
    Tnode *temp = (*node)->right;
    (*node)->right = temp->left;
    temp->left = *node;
    *node = temp;
}

// Function to perform insertion into a height-balanced BST
void insertNode(Tnode **root, int key) {
    if (*root == NULL) {
        *root = (Tnode *)malloc(sizeof(Tnode));
        (*root)->key = key;
        (*root)->balance = 0;
        (*root)->left = (*root)->right = NULL;
    } else {
        if (key < (*root)->key) {
            insertNode(&((*root)->left), key);
        } else {
            insertNode(&((*root)->right), key);
        }
        
        // Update balance
        int balance = ((*root)->left ? (*root)->left->balance + 1 : 0) - ((*root)->right ? (*root)->right->balance + 1 : 0);
        (*root)->balance = balance;

        // Perform rotations if necessary to maintain balance
        if (balance > 1) {
            if (key < (*root)->left->key) {
                rotateClockwise(root);
            } else {
                rotateCounterClockwise(&((*root)->left));
                rotateClockwise(root);
            }
        } else if (balance < -1) {
            if (key > (*root)->right->key) {
                rotateCounterClockwise(root);
            } else {
                rotateClockwise(&((*root)->right));
                rotateCounterClockwise(root);
            }
        }
    }
}

// Function to perform deletion from a height-balanced BST
void deleteNode(Tnode **root, int key) {
    if (*root == NULL) {
        return;
    }

    if (key < (*root)->key) {
        deleteNode(&((*root)->left), key);
    } else if (key > (*root)->key) {
        deleteNode(&((*root)->right), key);
    } else {
        if ((*root)->left == NULL || (*root)->right == NULL) {
            Tnode *temp = (*root)->left ? (*root)->left : (*root)->right;
            if (temp == NULL) {
                temp = *root;
                *root = NULL;
            } else {
                **root = *temp;
            }
            free(temp);
        } else {
            Tnode *temp = (*root)->left;
            while (temp->right != NULL) {
                temp = temp->right;
            }
            (*root)->key = temp->key;
            deleteNode(&((*root)->left), temp->key);
        }
    }

    // Update balance
    int balance = ((*root)->left ? (*root)->left->balance + 1 : 0) - ((*root)->right ? (*root)->right->balance + 1 : 0);
    (*root)->balance = balance;

    // Perform rotations if necessary to maintain balance
    if (balance > 1) {
        if ((*root)->left->balance >= 0) {
            rotateClockwise(root);
        } else {
            rotateCounterClockwise(&((*root)->left));
            rotateClockwise(root);
        }
    } else if (balance < -1) {
        if ((*root)->right->balance <= 0) {
            rotateCounterClockwise(root);
        } else {
            rotateClockwise(&((*root)->right));
            rotateCounterClockwise(root);
        }
    }
}

// Function to evaluate whether a tree is a BST
int isBSTUtil(Tnode *root, int min, int max) {
    if (root == NULL) {
        return 1;
    }
    if (root->key < min || root->key > max) {
        return 0;
    }
    return isBSTUtil(root->left, min, root->key - 1) && isBSTUtil(root->right, root->key + 1, max);
}

int isBST(Tnode *root) {
    return isBSTUtil(root, HBT_MIN, HBT_MAX);
}

// Function to evaluate whether a tree is height-balanced
int isBalanced(Tnode *root) {
    if (root == NULL) {
        return 1;
    }
    int leftHeight = (root->left ? root->left->balance + 1 : 0);
    int rightHeight = (root->right ? root->right->balance + 1 : 0);
    if (abs(leftHeight - rightHeight) <= 1 && isBalanced(root->left) && isBalanced(root->right)) {
        return 1;
    }
    return 0;
}

// Read the tree from input file and evaluate
Tnode* readTree(FILE *input_file) {
    // Read the key and branch information
    int key;
    char branch;
    fread(&key, sizeof(int), 1, input_file);
    fread(&branch, sizeof(char), 1, input_file);

    // If branch is 0, it indicates a leaf node
    if (branch == 0) {
        Tnode *leaf = (Tnode*)malloc(sizeof(Tnode));
        leaf->key = key;
        leaf->left = leaf->right = NULL;
        return leaf;
    }

    // Otherwise, create a node with the given key and recursively read left and right subtrees
    Tnode *root = (Tnode*)malloc(sizeof(Tnode));
    root->key = key;
    if (branch & 2) {
        root->left = readTree(input_file);
    } else {
        root->left = NULL;
    }
    if (branch & 1) {
        root->right = readTree(input_file);
    } else {
        root->right = NULL;
    }
    return root;
}

// Function to free allocated memory for tree nodes
void freeTree(Tnode *root) {
    if (root == NULL) {
        return;
    }
    // Recursively free left and right subtrees
    freeTree(root->left);
    freeTree(root->right);
    // Free current node
    free(root);
}

// Function to write the pre-order traversal of the tree to a file and print to console
void writePreOrder(Tnode *root, FILE *output, int printToConsole) {
    if (root == NULL) {
        return;
    }

    // Write key and branch information to the output file
    fwrite(&(root->key), sizeof(int), 1, output);
    char branch = 0;
    if (root->left != NULL) {
        branch |= 2; // Set bit 1 to 1 if left child exists
    }
    if (root->right != NULL) {
        branch |= 1; // Set bit 0 to 1 if right child exists
    }
    fwrite(&branch, sizeof(char), 1, output);

    // Print node to console if required
    if (printToConsole) {
        printf("%d ", root->key);
    }

    // Recursively write left and right subtrees
    writePreOrder(root->left, output, printToConsole);
    writePreOrder(root->right, output, printToConsole);
}

// Function to build a height-balanced BST based on operations from input file
int buildHeightBalancedBST(char *operations_file, char *output_file) {
    // Open operations input file
    FILE *ops_file = fopen(operations_file, "rb");
    if (ops_file == NULL) {
        perror("Error opening operations input file");
        return EXIT_FAILURE;
    }

    // Initialize an empty tree
    Tnode *root = NULL;

    // Read operations from input file and perform corresponding operations
    int key;
    char op;
    while (fread(&key, sizeof(int), 1, ops_file) == 1 && fread(&op, sizeof(char), 1, ops_file) == 1) {
        if (op == 'i') {
            // Insert operation
            insertNode(&root, key);
        } else if (op == 'd') {
            // Delete operation
            deleteNode(&root, key);
        } else {
            // Invalid operation
            fprintf(stderr, "Invalid operation in input file\n");
            fclose(ops_file);

            freeTree(root);
            return EXIT_FAILURE;
        }
    }

    // Close operations file
    fclose(ops_file);

    // Open output file for writing the constructed tree
    FILE *output = fopen(output_file, "wb");
    if (output == NULL) {
        perror("Error opening output file");
        freeTree(root);
        return EXIT_FAILURE;
    }

    // Write the pre-order traversal of the constructed tree to the output file
    writePreOrder(root, output, 1);

    // Close output file
    fclose(output);

    // Free allocated memory
    freeTree(root);

    return EXIT_SUCCESS;
}

// Function to evaluate a tree specified in the input file
int evaluateTree(char *input_file) {
    // Open tree input file
    FILE *file = fopen(input_file, "rb");
    if (file == NULL) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    // Read the tree from input file
    Tnode *root = readTree(file);

    // Close file
    fclose(file);

    // Evaluate the tree
    int is_bst = isBST(root);
    int is_balanced = isBalanced(root);

    // Print evaluation results
    printf("Is BST: %s\n", is_bst ? "Yes" : "No");
    printf("Is Height-Balanced: %s\n", is_balanced ? "Yes" : "No");
    printf("%d,%d,%d\n", 1, is_bst, is_balanced);

    // Free allocated memory
    freeTree(root);

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {

    // Check the option provided
    if (argv[1][0] == '-' && argv[1][2] == '\0') {
        // Build height-balanced BST option
        if (argv[1][1] == 'b') {
            if (argc != 4) {
                fprintf(stderr, "Usage: %s -b operations_input_file tree_output_file\n", argv[0]);
                return EXIT_FAILURE;
            }
            // Extract input arguments
            char *operations_file = argv[2];
            char *output_file = argv[3];

            // Build the height-balanced BST
            int result = buildHeightBalancedBST(operations_file, output_file);

            if (result == EXIT_SUCCESS) {
                printf("\nSuccessfully built height-balanced BST\n");

            } else {
                fprintf(stderr, "Failed to build height-balanced BST\n");
            }

            return result;
        }
        // Evaluate tree option
        else if (argv[1][1] == 'e') {
            if (argc != 3) {
                fprintf(stderr, "Usage: %s -e tree_input_file\n", argv[0]);
                return EXIT_FAILURE;
            }
            // Evaluate the tree
            return evaluateTree(argv[2]);
        }
    }

    // Invalid option
    fprintf(stderr, "Invalid option\n");
    return EXIT_FAILURE;
}
