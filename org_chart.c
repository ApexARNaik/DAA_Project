#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CHILDREN 20 // Constants as defined in the PRD
#define MAX_NAME 64
#define MAX_EMPLOYEES 100
#define OUT_BUF_SIZE 8192

typedef struct Employee { // Employee Node (N-ary Tree Node)
    char name[MAX_NAME];
    char role[MAX_NAME];
    int childCount;
    struct Employee* children[MAX_CHILDREN];
} Employee;

Employee pool[MAX_EMPLOYEES]; // Global static pool to avoid malloc complexity
int poolSize = 0;

char outBuf[OUT_BUF_SIZE] = {0}; // Output buffer to hold the traversal result before printing
int outPos = 0;

void emit(const char* str) { // Helper function to append strings to our output buffer
    int len = strlen(str);
    if (outPos + len < OUT_BUF_SIZE) {
        strcpy(&outBuf[outPos], str);
        outPos += len;
    }
}

void traverseOrg(Employee* emp, int level) { // 5.2 Core Recursive Traversal Function // Time: O(n) visits every node exactly once // Space: O(h) call stack depth = tree height
    if (emp == NULL) return; /* BASE CASE */

    char indent[256] = {0};
    for (int i = 0; i < level; i++) strcat(indent, "    ");

    char line[512];
    snprintf(line, sizeof(line), "%s- %s (%s)\n", indent, emp->name, emp->role);
    emit(line);

    for (int i = 0; i < emp->childCount; i++) { /* RECURSIVE CALL: visit each child at level+1 */
        traverseOrg(emp->children[i], level + 1);
    }
}

int countNodes(Employee *emp) { // 5.3 Helper Functions (countNodes & treeHeight) /* Count total nodes -> O(n) */
    if (emp == NULL) return 0;
    int count = 1;
    for (int i = 0; i < emp->childCount; i++) {
        count += countNodes(emp->children[i]);
    }
    return count;
}

int treeHeight(Employee *emp) { /* Tree height -> O(n) */
    if (emp == NULL) return 0;
    int maxH = 0;
    for (int i = 0; i < emp->childCount; i++) {
        int h = treeHeight(emp->children[i]);
        if (h > maxH) maxH = h;
    }
    return maxH + 1;
}

Employee* findByName(const char* name) { // Helper for the parser
    for (int i = 0; i < poolSize; i++) {
        if (strcmp(pool[i].name, name) == 0) return &pool[i];
    }
    return NULL;
}

Employee* parseInput(const char *input) { // 5.4 Input Parser
    char buf[4096];
    strncpy(buf, input, sizeof(buf));
    poolSize = 0;
    Employee* root = NULL;

    char managers[MAX_EMPLOYEES][MAX_NAME]; // We will store manager names during Pass 1 to link them in Pass 2 

    char* rec = strtok(buf, "|"); /* Pass 1: create all nodes */
    while (rec != NULL && poolSize < MAX_EMPLOYEES) {
        char name[MAX_NAME], role[MAX_NAME], mgr[MAX_NAME];
        
        sscanf(rec, " %63[^,], %63[^,], %63s", name, role, mgr); // Parse the comma-separated values
        
        strncpy(pool[poolSize].name, name, MAX_NAME);
        strncpy(pool[poolSize].role, role, MAX_NAME);
        pool[poolSize].childCount = 0;
        
        strncpy(managers[poolSize], mgr, MAX_NAME); // Save the manager name for Pass 2
        
        if (strcmp(mgr, "ROOT") == 0) {
            root = &pool[poolSize];
        }
        
        poolSize++;
        rec = strtok(NULL, "|");
    }

    for (int i = 0; i < poolSize; i++) { /* Pass 2: build parent-child links */
        if (strcmp(managers[i], "ROOT") != 0) {
            Employee* parent = findByName(managers[i]);
            if (parent != NULL && parent->childCount < MAX_CHILDREN) {
                parent->children[parent->childCount++] = &pool[i];
            }
        }
    }
    return root;
}

int main(int argc, char *argv[]) { // 5.5 main() - Entry Point Called by Flask // 5.5 main() - Entry Point Called by Flask
    char input[4096] = {0};
    
    if (argc >= 2) { // Flask passes the string as the first argument
        strncpy(input, argv[1], sizeof(input) - 1);
    } else {
        fgets(input, sizeof(input), stdin);
    }

    Employee* root = parseInput(input);
    if (!root) {
        printf("ERROR: No ROOT node.\n");
        return 1;
    }

    emit("ORGANIZATION HIERARCHY\n======================\n");
    
    clock_t start_time = clock(); // --- TIMING START ---
    
    traverseOrg(root, 0);
    
    clock_t end_time = clock(); // --- TIMING END ---
    double elapsed_ms = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000.0;

    int n = countNodes(root);
    int h = treeHeight(root);

    size_t node_memory_bytes = n * sizeof(Employee); // --- SPACE CALCULATION --- // Total memory used by the actual nodes dynamically matched from the pool
    size_t max_stack_bytes = h * 48; // Estimated stack space (approx 48 bytes per recursive frame on 64-bit systems)

    char stats[512]; // Format the stats to include both PRD Big-O and real numbers
    snprintf(stats, sizeof(stats), 
        "\n[STATS]\n"
        "n = %d\n"
        "h = %d\n"
        "Time: O(n) -> %.3f ms\n"
        "Space: O(h) -> %zu bytes (Call Stack) + %zu bytes (Nodes)\n", 
        n, h, elapsed_ms, max_stack_bytes, node_memory_bytes);
    emit(stats);

    printf("%s", outBuf); // Print everything to stdout at once for Flask to capture
    return 0;
}