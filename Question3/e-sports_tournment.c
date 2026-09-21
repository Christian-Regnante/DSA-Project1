#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PARTICIPANT_COUNT 69
#define INPUT_SIZE 128

static const int participants[PARTICIPANT_COUNT] = {
    42, 17, 68, 9, 23, 55, 81, 4, 13, 20, 31, 49, 61, 75, 90,
    2, 7, 11, 15, 19, 21, 27, 35, 45, 52, 58, 64, 72, 78, 85,
    95, 1, 3, 5, 6, 8, 10, 12, 14, 16, 18, 22, 24, 26, 29, 33,
    37, 41, 44, 47, 50, 54, 57, 60, 63, 66, 70, 74, 77, 80, 83,
    87, 92, 97, 25, 28, 30, 34, 39
};

typedef struct TreeNode
{
    int value;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

static void free_tree(TreeNode *root);

static int read_integer(const char *prompt, int minimum, int maximum)
{
    char buffer[INPUT_SIZE];
    char extra;
    int value;

    for (;;)
    {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
            return -1;

        if (sscanf(buffer, " %d %c", &value, &extra) == 1 &&
            value >= minimum && value <= maximum)
            return value;

        if (strchr(buffer, '\n') == NULL)
        {
            int character;

            while ((character = getchar()) != '\n' && character != EOF)
                ;
        }
        printf("Invalid input. Enter a value from %d to %d.\n",
               minimum, maximum);
    }
}

static TreeNode *create_node(int value)
{
    TreeNode *node;

    node = malloc(sizeof(*node));
    if (node == NULL)
    {
        fprintf(stderr, "Error allocating tree node.\n");
        return NULL;
    }

    node->value = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

static TreeNode *insert_node(TreeNode *root, int value, int *success)
{
    if (root == NULL)
    {
        TreeNode *node = create_node(value);

        if (node == NULL)
            *success = 0;
        return node;
    }

    if (value < root->value)
        root->left = insert_node(root->left, value, success);
    else if (value > root->value)
        root->right = insert_node(root->right, value, success);

    return root;
}

static TreeNode *build_tree(void)
{
    TreeNode *root;
    int index;
    int success;

    root = NULL;
    success = 1;
    for (index = 0; index < PARTICIPANT_COUNT && success; index++)
        root = insert_node(root, participants[index], &success);

    if (!success)
    {
        fprintf(stderr, "Unable to build the tournament tree.\n");
        free_tree(root);
        return NULL;
    }
    return root;
}

static TreeNode *find_node(TreeNode *root, int value, TreeNode **parent_out)
{
    TreeNode *parent;

    parent = NULL;
    while (root != NULL)
    {
        if (value == root->value)
        {
            if (parent_out != NULL)
                *parent_out = parent;
            return root;
        }

        parent = root;
        if (value < root->value)
            root = root->left;
        else
            root = root->right;
    }

    if (parent_out != NULL)
        *parent_out = NULL;
    return NULL;
}

static void display_leaves(const TreeNode *root, int *found)
{
    if (root == NULL)
        return;

    if (root->left == NULL && root->right == NULL)
    {
        printf("%d ", root->value);
        *found = 1;
        return;
    }

    display_leaves(root->left, found);
    display_leaves(root->right, found);
}

static void display_parent(const TreeNode *root, int value)
{
    TreeNode *parent;
    TreeNode *node;

    node = find_node((TreeNode *)root, value, &parent);
    if (node == NULL)
    {
        printf("Identifier %d does not exist in the tournament tree.\n", value);
    }
    else if (parent == NULL)
    {
        printf("%d is the root and has no parent.\n", value);
    }
    else
    {
        printf("Parent of %d: %d\n", value, parent->value);
    }
}

static void display_sibling(const TreeNode *root, int value)
{
    TreeNode *parent;
    TreeNode *node;
    const TreeNode *sibling;

    node = find_node((TreeNode *)root, value, &parent);
    if (node == NULL)
    {
        printf("Identifier %d does not exist in the tournament tree.\n", value);
    }
    else if (parent == NULL)
    {
        printf("%d is the root and has no sibling.\n", value);
    }
    else
    {
        sibling = parent->left == node ? parent->right : parent->left;
        if (sibling == NULL)
            printf("%d has no sibling.\n", value);
        else
            printf("Sibling of %d: %d\n", value, sibling->value);
    }
}

static void display_grandchildren(const TreeNode *root, int value)
{
    TreeNode *node;
    int found;

    node = find_node((TreeNode *)root, value, NULL);
    if (node == NULL)
    {
        printf("Identifier %d does not exist in the tournament tree.\n", value);
        return;
    }

    found = 0;
    if (node->left != NULL)
    {
        if (node->left->left != NULL)
        {
            printf("%d ", node->left->left->value);
            found = 1;
        }
        if (node->left->right != NULL)
        {
            printf("%d ", node->left->right->value);
            found = 1;
        }
    }
    if (node->right != NULL)
    {
        if (node->right->left != NULL)
        {
            printf("%d ", node->right->left->value);
            found = 1;
        }
        if (node->right->right != NULL)
        {
            printf("%d ", node->right->right->value);
            found = 1;
        }
    }

    if (found)
        printf("\n");
    else
        printf("%d has no grandchildren.\n", value);
}

static void free_tree(TreeNode *root)
{
    if (root == NULL)
        return;

    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

static void print_menu(void)
{
    printf("\n===== Tournament Tree =====\n");
    printf("1. Display root\n");
    printf("2. Display all leaf participants\n");
    printf("3. Find parent\n");
    printf("4. Find sibling\n");
    printf("5. Display grandchildren\n");
    printf("6. Exit\n");
}

int main(void)
{
    TreeNode *root;
    int choice;
    int value;
    int found;

    root = build_tree();
    if (root == NULL)
        return EXIT_FAILURE;

    for (;;)
    {
        print_menu();
        choice = read_integer("Enter choice: ", 1, 6);
        if (choice < 0)
        {
            free_tree(root);
            return EXIT_FAILURE;
        }

        if (choice == 1)
        {
            printf("Root participant/match identifier: %d\n", root->value);
        }
        else if (choice == 2)
        {
            found = 0;
            printf("Leaf participants: ");
            display_leaves(root, &found);
            if (!found)
                printf("none");
            printf("\n");
        }
        else if (choice == 3)
        {
            value = read_integer("Enter participant identifier: ",
                                 -2147483647 - 1, 2147483647);
            if (value >= 0)
                display_parent(root, value);
        }
        else if (choice == 4)
        {
            value = read_integer("Enter participant identifier: ", 0, 2147483647);
            if (value >= 0)
                display_sibling(root, value);
        }
        else if (choice == 5)
        {
            value = read_integer("Enter participant identifier: ", 0, 2147483647);
            if (value >= 0)
                display_grandchildren(root, value);
        }
        else
        {
            break;
        }
    }

    free_tree(root);
    printf("Tournament tree closed.\n");
    return EXIT_SUCCESS;
}
