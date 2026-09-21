#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ID_SIZE 32
#define TYPE_SIZE 64
#define INPUT_SIZE 256

typedef struct Sample
{
    char identifier[ID_SIZE];
    char type[TYPE_SIZE];
    int priority;
    struct Sample *prev;
    struct Sample *next;
} Sample;

typedef struct
{
    Sample *head;
    Sample *tail;
    Sample *current;
    size_t count;
} SampleQueue;

static int read_line(char *buffer, size_t size)
{
    size_t length;
    int character;

    if (fgets(buffer, (int)size, stdin) == NULL)
        return 0;

    length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n')
    {
        buffer[length - 1] = '\0';
        return 1;
    }

    while ((character = getchar()) != '\n' && character != EOF)
        ;
    return 1;
}

static int read_integer(const char *prompt, int minimum, int maximum)
{
    char buffer[INPUT_SIZE];
    char extra;
    int value;

    for (;;)
    {
        printf("%s", prompt);
        if (!read_line(buffer, sizeof(buffer)))
            return -1;

        if (sscanf(buffer, " %d %c", &value, &extra) == 1 &&
            value >= minimum && value <= maximum)
            return value;

        printf("Invalid input. Enter a value from %d to %d.\n",
               minimum, maximum);
    }
}

static int read_text(const char *prompt, char *buffer, size_t size)
{
    printf("%s", prompt);
    if (!read_line(buffer, size))
        return 0;

    if (buffer[0] == '\0')
    {
        printf("Input cannot be empty.\n");
        return 0;
    }

    return 1;
}

static const char *priority_label(int priority)
{
    if (priority == 1)
        return "Urgent";
    if (priority == 2)
        return "Normal";
    return "Routine";
}

static Sample *create_sample(const char *identifier, const char *type,
                             int priority)
{
    Sample *sample;

    sample = malloc(sizeof(*sample));
    if (sample == NULL)
    {
        fprintf(stderr, "Error allocating sample memory.\n");
        return NULL;
    }

    snprintf(sample->identifier, sizeof(sample->identifier), "%s", identifier);
    snprintf(sample->type, sizeof(sample->type), "%s", type);
    sample->priority = priority;
    sample->prev = NULL;
    sample->next = NULL;
    return sample;
}

static void insert_at_end(SampleQueue *queue, Sample *sample)
{
    if (queue->tail == NULL)
    {
        queue->head = sample;
        queue->tail = sample;
        queue->current = sample;
    }
    else
    {
        sample->prev = queue->tail;
        queue->tail->next = sample;
        queue->tail = sample;
    }

    queue->count++;
}

static void display_sample(const Sample *sample)
{
    if (sample == NULL)
    {
        printf("The queue is empty.\n");
        return;
    }

    printf("Identifier: %s\n", sample->identifier);
    printf("Sample type: %s\n", sample->type);
    printf("Priority: %d (%s)\n", sample->priority,
           priority_label(sample->priority));
}

static void move_next(SampleQueue *queue)
{
    if (queue->current == NULL)
    {
        printf("The queue is empty.\n");
    }
    else if (queue->current->next == NULL)
    {
        printf("Already at the last sample.\n");
    }
    else
    {
        queue->current = queue->current->next;
        printf("Moved to the next sample.\n");
        display_sample(queue->current);
    }
}

static void move_previous(SampleQueue *queue)
{
    if (queue->current == NULL)
    {
        printf("The queue is empty.\n");
    }
    else if (queue->current->prev == NULL)
    {
        printf("Already at the first sample.\n");
    }
    else
    {
        queue->current = queue->current->prev;
        printf("Moved to the previous sample.\n");
        display_sample(queue->current);
    }
}

static void pause_for_enter(void)
{
    char buffer[INPUT_SIZE];

    printf("Press Enter to continue.\n");
    read_line(buffer, sizeof(buffer));
}

static void review_forward(const SampleQueue *queue)
{
    const Sample *sample;

    if (queue->head == NULL)
    {
        printf("The queue is empty.\n");
        return;
    }

    sample = queue->head;
    while (sample != NULL)
    {
        display_sample(sample);
        sample = sample->next;
        if (sample != NULL)
            pause_for_enter();
    }
    printf("Reached the end of the queue.\n");
}

static void review_backward(const SampleQueue *queue)
{
    const Sample *sample;

    if (queue->tail == NULL)
    {
        printf("The queue is empty.\n");
        return;
    }

    sample = queue->tail;
    while (sample != NULL)
    {
        display_sample(sample);
        sample = sample->prev;
        if (sample != NULL)
            pause_for_enter();
    }
    printf("Reached the beginning of the queue.\n");
}

static int add_sample_from_input(SampleQueue *queue)
{
    char identifier[ID_SIZE];
    char type[TYPE_SIZE];
    int priority;
    Sample *sample;

    if (!read_text("Identifier: ", identifier, sizeof(identifier)) ||
        !read_text("Sample type: ", type, sizeof(type)))
        return 0;

    priority = read_integer("Priority (1=Urgent, 2=Normal, 3=Routine): ",
                            1, 3);
    if (priority < 0)
        return 0;

    sample = create_sample(identifier, type, priority);
    if (sample == NULL)
        return 0;

    insert_at_end(queue, sample);
    printf("Sample added to the end of the queue.\n");
    return 1;
}

static void free_queue(SampleQueue *queue)
{
    Sample *sample;
    Sample *next;

    sample = queue->head;
    while (sample != NULL)
    {
        next = sample->next;
        free(sample);
        sample = next;
    }

    queue->head = NULL;
    queue->tail = NULL;
    queue->current = NULL;
    queue->count = 0;
}

static void print_menu(void)
{
    printf("\n===== Laboratory Sample Queue =====\n");
    printf("1. Move to next sample\n");
    printf("2. Move to previous sample\n");
    printf("3. Display current sample\n");
    printf("4. Add sample to end\n");
    printf("5. Review queue forward\n");
    printf("6. Review queue backward\n");
    printf("7. Exit\n");
}

int main(void)
{
    SampleQueue queue = {NULL, NULL, NULL, 0};
    int initial_count;
    int index;
    int choice;

    initial_count = read_integer("Enter initial number of samples: ", 0, 1000000);
    if (initial_count < 0)
        return EXIT_FAILURE;

    for (index = 0; index < initial_count; index++)
    {
        printf("\nSample %d\n", index + 1);
        if (!add_sample_from_input(&queue))
        {
            free_queue(&queue);
            return EXIT_FAILURE;
        }
    }

    for (;;)
    {
        print_menu();
        choice = read_integer("Enter choice: ", 1, 7);
        if (choice < 0)
        {
            free_queue(&queue);
            return EXIT_FAILURE;
        }

        if (choice == 1)
            move_next(&queue);
        else if (choice == 2)
            move_previous(&queue);
        else if (choice == 3)
            display_sample(queue.current);
        else if (choice == 4)
            add_sample_from_input(&queue);
        else if (choice == 5)
            review_forward(&queue);
        else if (choice == 6)
            review_backward(&queue);
        else
            break;
    }

    free_queue(&queue);
    printf("Queue closed.\n");
    return EXIT_SUCCESS;
}
