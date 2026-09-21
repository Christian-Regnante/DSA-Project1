# DSA First Project Assignment

This repository contains four standalone C programs demonstrating sorting, linked lists, binary trees, and binary search trees. Each task includes its assignment specification, an implementation, and a DOCX document containing the required algorithm analysis.

## Task overview

| Task | Program | Main data structure or algorithm | Purpose |
|---|---|---|---|
| 1 | `task1.c` | Dynamic array and custom Quick Sort | Read orders, prioritize them, write sorted output, and report totals |
| 2 | `task2.c` | Doubly linked list | Manage a laboratory sample queue with forward/backward navigation |
| 3 | `task3.c` | Binary Search Tree | Build a tournament tree and query parent, sibling, leaf, and grandchild relationships |
| 4 | `task4.c` | Binary Search Tree | Load medicine inventory, search by medicine code, and display sorted records |

## Task 1: Order prioritization

`task1.c` reads orders from `orders.txt` and writes the sorted records to `priority_orders.txt`.

Orders are sorted by:

1. Order value, descending
2. Customer name, ascending lexicographical order
3. Order ID, ascending lexicographical order

The program uses a dynamically growing array and implements Quick Sort manually without `qsort()`. It also displays the number of valid orders processed and their total value.

Algorithm analysis: `task1_algorithm_analysis.docx`

## Task 2: Laboratory sample queue

`task2.c` is an interactive program that stores laboratory samples in a doubly linked list. Each node contains an identifier, sample type, priority, and `prev`/`next` pointers.

Available operations include:

- Move to the next sample
- Move to the previous sample
- Display the current sample
- Add a sample to the end of the queue
- Review the queue forward
- Review the queue backward
- Exit

The queue maintains a tail pointer, making end insertion `O(1)`. Traversal in either direction is `O(n)`.

Algorithm analysis: `task2_algorithm_analysis.docx`

## Task 3: Tournament tree

`task3.c` builds an ordinary Binary Search Tree from the participant identifiers supplied in the assignment. The first identifier becomes the root; smaller identifiers are inserted to the left and larger identifiers to the right.

The menu supports:

- Displaying the root
- Displaying all leaf participants
- Finding a participant’s parent
- Finding a participant’s sibling
- Displaying a participant’s grandchildren
- Handling missing identifiers and special tree relationships

Insertion complexity is `O(h)`, where `h` is the tree height. This is `O(log n)` for a balanced tree and `O(n)` in the worst case for an unbalanced tree.

Note: the assignment text says there are 68 identifiers, but the provided list contains 69 values. The implementation preserves all listed values.

Algorithm analysis: `task3_algorithm_analysis.docx`

## Task 4: Pharmacy inventory

`task4.c` reads medicine records from `inventory.txt` using this format:

```text
MedicineCode|MedicineName|Quantity|UnitPrice
```

The records are stored in a Binary Search Tree keyed by medicine code. The program:

- Validates records and skips malformed entries
- Replaces the quantity when a duplicate medicine code is found
- Supports repeated medicine searches
- Displays the complete inventory in ascending medicine-code order using in-order traversal
- Frees all dynamically allocated nodes before exit

BST search is `O(h)`: `O(1)` in the best case, `O(log n)` for a balanced tree, and `O(n)` in the worst case. Because the supplied inventory codes are already ascending, inserting them into an ordinary BST creates a highly skewed tree.

Algorithm analysis: `task4_algorithm_analysis.docx`

## Building and running

Compile each program independently with GCC:

```bash
gcc -std=c11 -Wall -Wextra -pedantic task1.c -o task1
gcc -std=c11 -Wall -Wextra -pedantic task2.c -o task2
gcc -std=c11 -Wall -Wextra -pedantic task3.c -o task3
gcc -std=c11 -Wall -Wextra -pedantic task4.c -o task4
```

Run the programs from the repository root so their input files can be found:

```bash
./task1
./task2
./task3
./task4
```

Task 1 expects `orders.txt` and creates `priority_orders.txt`. Task 2 and Task 3 receive their data interactively. Task 4 expects `inventory.txt` and then provides an interactive search/display menu.

## Assignment references

The authoritative requirements and implementation guidance are available in:

- `task1_assignment.txt` and `Task1_assignment-instruction_guide.md`
- `task2_assignment.txt` and `Task2_assignment-instruction_guide.md`
- `task3_assignment.txt` and `Task3_assignment-instruction_guide.md`
- `task4_assignment.txt` and `Task4_assignment-instruction_guide.md`

The repository also includes `.github/sample-coding-style.md`, which records conventions observed in the supplied reference samples.
