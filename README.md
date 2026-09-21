# DSA First Project Assignment

This repository contains four standalone C implementations for the data-structure assignments. Each task is organized in its own `QuestionN/` directory with its C source, task-specific input/output files where applicable, and algorithm-analysis document.

## Repository structure

```text
Question1/
  qsort.c
  orders.txt
  priority_orders.txt
  qsort_algorithm_analysis.docx

Question2/
  lab_processor.c
  D-linked_list_algorithm_analysis.docx

Question3/
  e-sports_tournment.c
  tournment-BST_algorithm_analysis.docx

Question4/
  pharmarcy_inventory.c
  inventory.txt
  inventory-BST_algorithm_analysis.docx
```

The sample coding conventions used as a reference are documented in `.github/sample-coding-style.md`.

## Task overview

| Task | Source | Main data structure or algorithm | Main result |
|---|---|---|---|
| 1 | `Question1/qsort.c` | Dynamic array and custom Quick Sort | Prioritized order records in `Question1/priority_orders.txt` |
| 2 | `Question2/lab_processor.c` | Doubly linked list | Interactive laboratory sample queue |
| 3 | `Question3/e-sports_tournment.c` | Binary Search Tree | Interactive tournament-tree relationship queries |
| 4 | `Question4/pharmarcy_inventory.c` | Binary Search Tree | Interactive pharmacy inventory search and traversal |

## Task 1: Order prioritization

`Question1/qsort.c` reads records from `Question1/orders.txt` and writes sorted records to `Question1/priority_orders.txt`.

Orders are sorted by:

1. Order value, descending
2. Customer name, ascending lexicographical order
3. Order ID, ascending lexicographical order

The program uses a dynamically growing array and implements Quick Sort manually without `qsort()`. It reports the number of valid orders processed and their total value.

Algorithm analysis: `Question1/qsort_algorithm_analysis.docx`

## Task 2: Laboratory sample queue

`Question2/lab_processor.c` stores laboratory samples in a dynamically allocated doubly linked list. Each node contains an identifier, sample type, priority, and `prev`/`next` pointers.

The interactive menu supports:

- Moving to the next sample
- Moving to the previous sample
- Displaying the current sample
- Adding a sample to the end of the queue
- Reviewing the queue forward
- Reviewing the queue backward
- Exiting safely

The queue maintains a tail pointer, making end insertion `O(1)`. Traversal in either direction is `O(n)`.

Algorithm analysis: `Question2/D-linked_list_algorithm_analysis.docx`

## Task 3: Tournament tree

`Question3/e-sports_tournment.c` builds an ordinary Binary Search Tree from the participant identifiers supplied in the assignment. The first identifier becomes the root; smaller identifiers are inserted to the left and larger identifiers to the right.

The menu supports:

- Displaying the root
- Displaying all leaf participants
- Finding a participant’s parent
- Finding a participant’s sibling
- Displaying a participant’s grandchildren
- Handling missing identifiers and special tree relationships

Insertion complexity is `O(h)`, where `h` is the tree height: `O(log n)` for a balanced tree and `O(n)` in the worst case for an unbalanced tree.

The assignment text says there are 68 identifiers, but the provided list contains 69 values. The implementation preserves all listed values.

Algorithm analysis: `Question3/tournment-BST_algorithm_analysis.docx`

## Task 4: Pharmacy inventory

`Question4/pharmarcy_inventory.c` reads medicine records from `Question4/inventory.txt` using:

```text
MedicineCode|MedicineName|Quantity|UnitPrice
```

The records are stored in a Binary Search Tree keyed by medicine code. The program validates records, replaces the quantity for duplicate codes, supports repeated searches, displays the complete inventory in ascending code order, and frees the tree before exit.

BST search is `O(h)`: `O(1)` in the best case, `O(log n)` for a balanced tree, and `O(n)` in the worst case. Because the supplied codes are already ascending, inserting them into an ordinary BST creates a highly skewed tree.

Algorithm analysis: `Question4/inventory-BST_algorithm_analysis.docx`

## Building and running

Compile each program from its task directory so its relative input/output filenames resolve correctly:

```bash
cd Question1
gcc -std=c11 -Wall -Wextra -pedantic qsort.c -o qsort
./qsort
```

```bash
cd Question2
gcc -std=c11 -Wall -Wextra -pedantic lab_processor.c -o lab_processor
./lab_processor
```

```bash
cd Question3
gcc -std=c11 -Wall -Wextra -pedantic e-sports_tournment.c -o e-sports_tournment
./e-sports_tournment
```

```bash
cd Question4
gcc -std=c11 -Wall -Wextra -pedantic pharmarcy_inventory.c -o pharmarcy_inventory
./pharmarcy_inventory
```
