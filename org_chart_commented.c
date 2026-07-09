#include <stdio.h>    // Include standard input/output library for printf, snprintf, etc.
#include <stdlib.h>   // Include standard library for memory allocation and utility functions.
#include <string.h>   // Include string manipulation library for strcpy, strcmp, strtok, etc.
#include <time.h>     // Include time library to measure execution duration.

// Constants as defined in the PRD
#define MAX_CHILDREN 20     // Maximum number of direct subordinates an employee can have.
#define MAX_NAME 64         // Maximum character length for employee names and roles.
#define MAX_EMPLOYEES 100   // Maximum total number of employees in the organization.
#define OUT_BUF_SIZE 8192   // Size of the buffer used to store the output text before printing.

// Employee Node (N-ary Tree Node)
typedef struct Employee {                   // Define a structure to represent an employee.
    char name[MAX_NAME];                    // Array to store the employee's name.
    char role[MAX_NAME];                    // Array to store the employee's job role.
    int childCount;                         // Counter for how many direct subordinates this employee has.
    struct Employee* children[MAX_CHILDREN];// Array of pointers to subordinate Employee structures.
} Employee;                                 // Name the structure type 'Employee'.

// Global static pool to avoid malloc complexity
Employee pool[MAX_EMPLOYEES];   // Pre-allocate an array of Employees to avoid dynamic memory allocation.
int poolSize = 0;               // Keep track of how many employees have been added to the pool.

// Output buffer to hold the traversal result before printing
char outBuf[OUT_BUF_SIZE] = {0}; // Initialize a large character array to hold output with zeros.
int outPos = 0;                  // Track the current writing position in the output buffer.

// Helper function to append strings to our output buffer
void emit(const char* str) {                     // Define a function that takes a string to append.
    int len = strlen(str);                       // Calculate the length of the string to append.
    if (outPos + len < OUT_BUF_SIZE) {           // Check if there is enough space left in the buffer.
        strcpy(&outBuf[outPos], str);            // Copy the string into the buffer at the current position.
        outPos += len;                           // Update the position index by the length of the string.
    }                                            // End of if condition.
}                                                // End of emit function.

// 5.2 Core Recursive Traversal Function
// Time: O(n) visits every node exactly once
// Space: O(h) call stack depth = tree height
void traverseOrg(Employee* emp, int level) {     // Function to print the tree, takes an employee and their depth level.
    if (emp == NULL) return; /* BASE CASE */     // If the employee is null, stop and return.

    char indent[256] = {0};                      // Buffer to hold the indentation spaces.
    for (int i = 0; i < level; i++) strcat(indent, "    "); // Append 4 spaces for each level of depth.

    char line[512];                              // Buffer to hold the formatted line for this employee.
    snprintf(line, sizeof(line), "%s- %s (%s)\n", indent, emp->name, emp->role); // Format the string with indent, name, and role.
    emit(line);                                  // Append the formatted line to the global output buffer.

    /* RECURSIVE CALL: visit each child at level+1 */
    for (int i = 0; i < emp->childCount; i++) {  // Loop through all direct subordinates.
        traverseOrg(emp->children[i], level + 1);// Recursively call traverseOrg for each child, increasing the level.
    }                                            // End of loop.
}                                                // End of traverseOrg function.

// 5.3 Helper Functions (countNodes & treeHeight)
/* Count total nodes -> O(n) */
int countNodes(Employee *emp) {                  // Function to count the total number of employees in the tree.
    if (emp == NULL) return 0;                   // If the node is null, it counts as 0.
    int count = 1;                               // Start count at 1 for the current employee.
    for (int i = 0; i < emp->childCount; i++) {  // Loop over all children.
        count += countNodes(emp->children[i]);   // Recursively add the node count of each child's subtree.
    }                                            // End of loop.
    return count;                                // Return the total count.
}                                                // End of countNodes function.

/* Tree height -> O(n) */
int treeHeight(Employee *emp) {                  // Function to calculate the maximum depth (height) of the tree.
    if (emp == NULL) return 0;                   // If the node is null, its height is 0.
    int maxH = 0;                                // Variable to keep track of the maximum child height found.
    for (int i = 0; i < emp->childCount; i++) {  // Loop over all children.
        int h = treeHeight(emp->children[i]);    // Recursively find the height of the child's subtree.
        if (h > maxH) maxH = h;                  // If this child's height is the largest so far, update maxH.
    }                                            // End of loop.
    return maxH + 1;                             // Return the max child height plus 1 for the current level.
}                                                // End of treeHeight function.

// Helper for the parser
Employee* findByName(const char* name) {         // Function to search for an employee in the pool by name.
    for (int i = 0; i < poolSize; i++) {         // Loop through all employees currently in the pool.
        if (strcmp(pool[i].name, name) == 0) return &pool[i]; // If names match, return a pointer to that employee.
    }                                            // End of loop.
    return NULL;                                 // Return NULL if no employee with that name is found.
}                                                // End of findByName function.

// 5.4 Input Parser
Employee* parseInput(const char *input) {        // Function to build the tree from a pipe-separated string.
    char buf[4096];                              // Buffer to hold a modifiable copy of the input string.
    strncpy(buf, input, sizeof(buf));            // Copy the input into the local buffer safely.
    poolSize = 0;                                // Reset the global pool size counter to 0.
    Employee* root = NULL;                       // Pointer to keep track of the root (CEO) of the organization.

    // We will store manager names during Pass 1 to link them in Pass 2
    char managers[MAX_EMPLOYEES][MAX_NAME];      // 2D array to temporaily store the manager's name for each employee.

    /* Pass 1: create all nodes */
    char* rec = strtok(buf, "|");                // Get the first record by splitting the input string by '|'.
    while (rec != NULL && poolSize < MAX_EMPLOYEES) { // Loop as long as there's a record and pool isn't full.
        char name[MAX_NAME], role[MAX_NAME], mgr[MAX_NAME]; // Buffers for parsed name, role, and manager.
        
        // Parse the comma-separated values
        sscanf(rec, " %63[^,], %63[^,], %63s", name, role, mgr); // Extract name, role, and manager name from the record string.
        
        strncpy(pool[poolSize].name, name, MAX_NAME); // Copy the parsed name into the new pool employee.
        strncpy(pool[poolSize].role, role, MAX_NAME); // Copy the parsed role into the new pool employee.
        pool[poolSize].childCount = 0;                // Initialize the child count to 0.
        
        // Save the manager name for Pass 2
        strncpy(managers[poolSize], mgr, MAX_NAME);   // Store the manager's name in the temporary array.
        
        if (strcmp(mgr, "ROOT") == 0) {               // Check if the manager is "ROOT" (indicating the top boss).
            root = &pool[poolSize];                   // If so, set the root pointer to this employee.
        }                                             // End of if.
        
        poolSize++;                                   // Increment the pool size counter.
        rec = strtok(NULL, "|");                      // Get the next record separated by '|'.
    }                                                 // End of while loop.

    /* Pass 2: build parent-child links */
    for (int i = 0; i < poolSize; i++) {              // Loop through all employees we just created.
        if (strcmp(managers[i], "ROOT") != 0) {       // If the employee is NOT the root employee...
            Employee* parent = findByName(managers[i]); // ...find their manager by name in the pool.
            if (parent != NULL && parent->childCount < MAX_CHILDREN) { // If manager is found and has room for more subordinates...
                parent->children[parent->childCount++] = &pool[i];     // ...add this employee as a child to the manager and increment count.
            }                                         // End of inner if.
        }                                             // End of outer if.
    }                                                 // End of loop.
    return root;                                      // Return the pointer to the root employee.
}                                                     // End of parseInput function.

// 5.5 main() - Entry Point Called by Flask
int main(int argc, char *argv[]) {                    // Main execution function.
    char input[4096] = {0};                           // Buffer to store the input data.
    
    // Flask passes the string as the first argument
    if (argc >= 2) {                                  // Check if command line arguments were provided.
        strncpy(input, argv[1], sizeof(input) - 1);   // If yes, copy the first argument into the input buffer.
    } else {                                          // Otherwise (e.g., ran interactively without args)...
        fgets(input, sizeof(input), stdin);           // ...read the input from standard input (keyboard).
    }                                                 // End of if-else.

    Employee* root = parseInput(input);               // Call parseInput to build the tree and get the root.
    if (!root) {                                      // If root is NULL (parsing failed or no ROOT found)...
        printf("ERROR: No ROOT node.\n");             // ...print an error message...
        return 1;                                     // ...and exit with an error code.
    }                                                 // End of if.

    emit("ORGANIZATION HIERARCHY\n======================\n"); // Append the header to the output buffer.
    
    // --- TIMING START ---
    clock_t start_time = clock();                     // Record the current CPU time to measure performance.
    
    traverseOrg(root, 0);                             // Start generating the tree output from the root at level 0.
    
    // --- TIMING END ---
    clock_t end_time = clock();                       // Record the CPU time after traversal completes.
    double elapsed_ms = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000.0; // Calculate elapsed time in milliseconds.

    int n = countNodes(root);                         // Calculate total number of nodes in the tree.
    int h = treeHeight(root);                         // Calculate the height of the tree.

    // --- SPACE CALCULATION ---
    // Total memory used by the actual nodes dynamically matched from the pool
    size_t node_memory_bytes = n * sizeof(Employee);  // Calculate memory used by all Employee structs.
    // Estimated stack space (approx 48 bytes per recursive frame on 64-bit systems)
    size_t max_stack_bytes = h * 48;                  // Calculate estimated memory used by the call stack.

    // Format the stats to include both PRD Big-O and real numbers
    char stats[512];                                  // Buffer to hold formatted statistics output.
    snprintf(stats, sizeof(stats),                    // Format the statistics string safely.
        "\n[STATS]\n"                                 // Add Stats header.
        "n = %d\n"                                    // Add node count.
        "h = %d\n"                                    // Add height.
        "Time: O(n) -> %.3f ms\n"                     // Add time complexity and measured time.
        "Space: O(h) -> %zu bytes (Call Stack) + %zu bytes (Nodes)\n", // Add space complexity and measured space.
        n, h, elapsed_ms, max_stack_bytes, node_memory_bytes); // Pass variables to format string.
    emit(stats);                                      // Append the formatted statistics string to the output buffer.

    // Print everything to stdout at once for Flask to capture
    printf("%s", outBuf);                             // Print the entire constructed output buffer to standard output.
    return 0;                                         // Exit program successfully.
}                                                     // End of main function.
