#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE "inventory.txt"
#define LINE_SIZE 512
#define CODE_SIZE 32
#define NAME_SIZE 128
#define INPUT_SIZE 128

typedef struct MedicineNode
{
    char code[CODE_SIZE];
    char name[NAME_SIZE];
    int quantity;
    int unit_price;
    struct MedicineNode *left;
    struct MedicineNode *right;
} MedicineNode;

static void free_tree(MedicineNode *root);

static void trim(char *text)
{
    char *start;
    size_t length;

    start = text;
    while (isspace((unsigned char)*start))
        start++;

    if (start != text)
        memmove(text, start, strlen(start) + 1);

    length = strlen(text);
    while (length > 0 && isspace((unsigned char)text[length - 1]))
    {
        text[length - 1] = '\0';
        length--;
    }
}

static int parse_nonnegative_int(const char *text, int *value_out)
{
    char *end;
    long value;

    errno = 0;
    value = strtol(text, &end, 10);
    if (errno == ERANGE || end == text)
        return 0;

    while (isspace((unsigned char)*end))
        end++;

    if (*end != '\0' || value < 0 || value > INT_MAX)
        return 0;

    *value_out = (int)value;
    return 1;
}

static int parse_record(char *line, MedicineNode *record)
{
    char *fields[5];
    char *field;
    int field_count;

    field_count = 0;
    field = strtok(line, "|");
    while (field != NULL && field_count < 5)
    {
        fields[field_count] = field;
        field_count++;
        field = strtok(NULL, "|");
    }

    if (field_count != 4 || field != NULL)
        return 0;

    trim(fields[0]);
    trim(fields[1]);
    trim(fields[2]);
    trim(fields[3]);

    if (fields[0][0] == '\0' || fields[1][0] == '\0' ||
        fields[2][0] == '\0' || fields[3][0] == '\0' ||
        strlen(fields[0]) >= CODE_SIZE || strlen(fields[1]) >= NAME_SIZE)
        return 0;

    if (!parse_nonnegative_int(fields[2], &record->quantity) ||
        !parse_nonnegative_int(fields[3], &record->unit_price))
        return 0;

    snprintf(record->code, sizeof(record->code), "%s", fields[0]);
    snprintf(record->name, sizeof(record->name), "%s", fields[1]);
    record->left = NULL;
    record->right = NULL;
    return 1;
}

static MedicineNode *create_node(const MedicineNode *record)
{
    MedicineNode *node;

    node = malloc(sizeof(*node));
    if (node == NULL)
    {
        fprintf(stderr, "Error allocating medicine node.\n");
        return NULL;
    }

    *node = *record;
    node->left = NULL;
    node->right = NULL;
    return node;
}

static MedicineNode *insert_or_update(MedicineNode *root,
                                      const MedicineNode *record,
                                      int *success,
                                      int *updated)
{
    int comparison;

    if (root == NULL)
    {
        MedicineNode *node = create_node(record);

        if (node == NULL)
            *success = 0;
        return node;
    }

    comparison = strcmp(record->code, root->code);
    if (comparison < 0)
    {
        root->left = insert_or_update(root->left, record, success, updated);
    }
    else if (comparison > 0)
    {
        root->right = insert_or_update(root->right, record, success, updated);
    }
    else
    {
        root->quantity = record->quantity;
        *updated = 1;
    }

    return root;
}

static MedicineNode *load_inventory(const char *filename, size_t *valid_count,
                                    size_t *invalid_count, size_t *update_count)
{
    FILE *file;
    MedicineNode *root;
    char line[LINE_SIZE];
    size_t line_number;

    file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error opening inventory file: %s\n", filename);
        return NULL;
    }

    root = NULL;
    line_number = 0;
    *valid_count = 0;
    *invalid_count = 0;
    *update_count = 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        MedicineNode record;
        MedicineNode *new_root;
        char *newline;
        int success;
        int updated;

        line_number++;
        newline = strchr(line, '\n');
        if (newline == NULL && !feof(file))
        {
            int character;

            while ((character = fgetc(file)) != '\n' && character != EOF)
                ;
            fprintf(stderr, "Skipping malformed record on line %zu.\n",
                    line_number);
            (*invalid_count)++;
            continue;
        }

        if (line[0] == '\n' || line[0] == '\0')
            continue;

        if (!parse_record(line, &record))
        {
            fprintf(stderr, "Skipping malformed record on line %zu.\n",
                    line_number);
            (*invalid_count)++;
            continue;
        }

        success = 1;
        updated = 0;
        new_root = insert_or_update(root, &record, &success, &updated);
        if (!success)
        {
            free_tree(new_root);
            fclose(file);
            return NULL;
        }
        root = new_root;
        (*valid_count)++;
        if (updated)
            (*update_count)++;
    }

    if (ferror(file))
    {
        fprintf(stderr, "Error reading inventory file: %s\n", filename);
        free_tree(root);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return root;
}

static MedicineNode *search_medicine(MedicineNode *root, const char *code)
{
    int comparison;

    while (root != NULL)
    {
        comparison = strcmp(code, root->code);
        if (comparison == 0)
            return root;
        root = comparison < 0 ? root->left : root->right;
    }

    return NULL;
}

static void display_medicine(const MedicineNode *medicine)
{
    printf("Medicine Code: %s\n", medicine->code);
    printf("Medicine Name: %s\n", medicine->name);
    printf("Quantity: %d\n", medicine->quantity);
    printf("Unit Price: %d\n", medicine->unit_price);
}

static void display_inventory(const MedicineNode *root)
{
    if (root == NULL)
        return;

    display_inventory(root->left);
    display_medicine(root);
    printf("\n");
    display_inventory(root->right);
}

static void free_tree(MedicineNode *root)
{
    if (root == NULL)
        return;

    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

static int read_menu_choice(void)
{
    char buffer[INPUT_SIZE];
    char extra;
    int choice;

    for (;;)
    {
        printf("\n===== Pharmacy Inventory =====\n");
        printf("1. Search medicine\n");
        printf("2. Display all medicines\n");
        printf("3. Exit\n");
        printf("Enter choice: ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
            return -1;

        if (sscanf(buffer, " %d %c", &choice, &extra) == 1 &&
            choice >= 1 && choice <= 3)
            return choice;

        printf("Invalid input. Enter a choice from 1 to 3.\n");
    }
}

static int read_code(char *code, size_t size)
{
    printf("Enter medicine code: ");
    if (fgets(code, (int)size, stdin) == NULL)
        return 0;

    if (strchr(code, '\n') == NULL)
    {
        int character;

        while ((character = getchar()) != '\n' && character != EOF)
            ;
        printf("Medicine code is too long.\n");
        return 0;
    }

    code[strcspn(code, "\n")] = '\0';
    trim(code);
    if (code[0] == '\0')
    {
        printf("Medicine code cannot be empty.\n");
        return 0;
    }
    return 1;
}

int main(void)
{
    MedicineNode *root;
    size_t valid_count;
    size_t invalid_count;
    size_t update_count;
    char code[CODE_SIZE];
    int choice;

    root = load_inventory(INPUT_FILE, &valid_count, &invalid_count,
                          &update_count);
    if (root == NULL)
    {
        if (invalid_count == 0)
            printf("No valid medicine records found.\n");
        else if (valid_count > 0)
            fprintf(stderr, "Unable to build the medicine inventory.\n");
        return EXIT_FAILURE;
    }

    printf("Loaded %zu valid records", valid_count);
    if (update_count > 0)
        printf(" (%zu duplicate quantities updated)", update_count);
    printf(".\n");
    if (invalid_count > 0)
        printf("Skipped %zu invalid records.\n", invalid_count);

    for (;;)
    {
        MedicineNode *medicine;

        choice = read_menu_choice();
        if (choice < 0)
        {
            free_tree(root);
            return EXIT_FAILURE;
        }

        if (choice == 1)
        {
            if (read_code(code, sizeof(code)))
            {
                medicine = search_medicine(root, code);
                if (medicine == NULL)
                    printf("Medicine %s was not found.\n", code);
                else
                    display_medicine(medicine);
            }
        }
        else if (choice == 2)
        {
            display_inventory(root);
        }
        else
        {
            break;
        }
    }

    free_tree(root);
    printf("Inventory closed.\n");
    return EXIT_SUCCESS;
}
