#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INITIAL_CAPACITY 16
#define LINE_SIZE 512
#define ORDER_ID_SIZE 32
#define CUSTOMER_NAME_SIZE 64
#define CATEGORY_SIZE 64

typedef struct
{
    char order_id[ORDER_ID_SIZE];
    char customer_name[CUSTOMER_NAME_SIZE];
    char category[CATEGORY_SIZE];
    long long order_value;
} Order;

static int parse_order(const char *line, Order *order)
{
    char extra;

    return sscanf(line, "%31s %63s %63s %lld %c",
                  order->order_id,
                  order->customer_name,
                  order->category,
                  &order->order_value,
                  &extra) == 4;
}

static int read_orders(const char *filename, Order **orders_out,
                       size_t *count_out, long long *total_out)
{
    FILE *file;
    Order *orders;
    size_t count;
    size_t capacity;
    char line[LINE_SIZE];
    size_t line_number;

    file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error opening input file: %s\n", filename);
        return 0;
    }

    capacity = INITIAL_CAPACITY;
    orders = malloc(capacity * sizeof(*orders));
    if (orders == NULL)
    {
        fprintf(stderr, "Error allocating order storage.\n");
        fclose(file);
        return 0;
    }

    count = 0;
    *total_out = 0;
    line_number = 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        Order order;
        char *newline;

        line_number++;
        newline = strchr(line, '\n');
        if (newline == NULL && !feof(file))
        {
            int character;

            while ((character = fgetc(file)) != '\n' && character != EOF)
                ;
            fprintf(stderr, "Skipping malformed record on line %zu.\n",
                    line_number);
            continue;
        }

        if (line[0] == '\n' || line[0] == '\0')
            continue;

        if (!parse_order(line, &order) || order.order_value < 0)
        {
            fprintf(stderr, "Skipping malformed record on line %zu.\n",
                    line_number);
            continue;
        }

        if (count == capacity)
        {
            Order *expanded_orders;

            capacity *= 2;
            expanded_orders = realloc(orders, capacity * sizeof(*orders));
            if (expanded_orders == NULL)
            {
                fprintf(stderr, "Error expanding order storage.\n");
                free(orders);
                fclose(file);
                return 0;
            }
            orders = expanded_orders;
        }

        orders[count] = order;
        *total_out += order.order_value;
        count++;
    }

    if (ferror(file))
    {
        fprintf(stderr, "Error reading input file: %s\n", filename);
        free(orders);
        fclose(file);
        return 0;
    }

    fclose(file);
    *orders_out = orders;
    *count_out = count;
    return 1;
}

static int compare_orders(const Order *first, const Order *second)
{
    int comparison;

    if (first->order_value > second->order_value)
        return -1;
    if (first->order_value < second->order_value)
        return 1;

    comparison = strcmp(first->customer_name, second->customer_name);
    if (comparison != 0)
        return comparison;

    return strcmp(first->order_id, second->order_id);
}

static void swap_orders(Order *first, Order *second)
{
    Order temporary;

    temporary = *first;
    *first = *second;
    *second = temporary;
}

static int partition(Order orders[], int low, int high)
{
    int pivot_index;
    int store_index;
    int index;
    Order pivot;

    pivot_index = low + rand() % (high - low + 1);
    swap_orders(&orders[pivot_index], &orders[high]);
    pivot = orders[high];
    store_index = low;

    for (index = low; index < high; index++)
    {
        if (compare_orders(&orders[index], &pivot) < 0)
        {
            swap_orders(&orders[index], &orders[store_index]);
            store_index++;
        }
    }

    swap_orders(&orders[store_index], &orders[high]);
    return store_index;
}

static void quick_sort(Order orders[], int low, int high)
{
    int pivot_index;

    if (low >= high)
        return;

    pivot_index = partition(orders, low, high);
    quick_sort(orders, low, pivot_index - 1);
    quick_sort(orders, pivot_index + 1, high);
}

static int write_orders(const char *filename, const Order orders[], size_t count)
{
    FILE *file;
    size_t index;

    file = fopen(filename, "w");
    if (file == NULL)
    {
        fprintf(stderr, "Error opening output file: %s\n", filename);
        return 0;
    }

    for (index = 0; index < count; index++)
    {
        if (fprintf(file, "%s %s %s %lld\n",
                    orders[index].order_id,
                    orders[index].customer_name,
                    orders[index].category,
                    orders[index].order_value) < 0)
        {
            fprintf(stderr, "Error writing output file: %s\n", filename);
            fclose(file);
            return 0;
        }
    }

    if (fclose(file) != 0)
    {
        fprintf(stderr, "Error closing output file: %s\n", filename);
        return 0;
    }

    return 1;
}

int main(void)
{
    Order *orders;
    size_t count;
    long long total;

    srand((unsigned int)time(NULL));

    if (!read_orders("orders.txt", &orders, &count, &total))
        return EXIT_FAILURE;

    if (count > 0)
        quick_sort(orders, 0, (int)count - 1);

    if (!write_orders("priority_orders.txt", orders, count))
    {
        free(orders);
        return EXIT_FAILURE;
    }

    printf("Orders processed: %zu\n", count);
    printf("Total order value: %lld\n", total);

    free(orders);
    return EXIT_SUCCESS;
}
