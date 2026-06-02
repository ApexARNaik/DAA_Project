#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Constants as defined in the PRD
#define MAX_CHILDREN 20
#define MAX_NAME 64
#define MAX_EMPLOYEES 100
#define OUT_BUF_SIZE 8192

// Employee Node (N-ary Tree Node)
typedef struct Employee {
    char name[MAX_NAME];
    char role[MAX_NAME];
    int childCount;
    struct Employee* children[MAX_CHILDREN];
} Employee;

// Global static pool to avoid malloc complexity
Employee pool[MAX_EMPLOYEES];
int poolSize = 0;

// Output buffer to hold the traversal result before printing
char outBuf[OUT_BUF_SIZE] = {0};
int outPos = 0;

// Helper function to append strings to our output buffer
void emit(const char* str) {
    int len = strlen(str);
    if (outPos + len < OUT_BUF_SIZE) {
        strcpy(&outBuf[outPos], str);
        outPos += len;
    }
}

// 5.2 Core Recursive Traversal Function
// Time: O(n) visits every node exactly once
// Space: O(h) call stack depth = tree height
void traverseOrg(Employee* emp, int level) {
    if (emp == NULL) return; /* BASE CASE */

    char indent[256] = {0};
    for (int i = 0; i < level; i++) strcat(indent, "    ");

    char line[512];
    snprintf(line, sizeof(line), "%s- %s (%s)\n", indent, emp->name, emp->role);
    emit(line);

    /* RECURSIVE CALL: visit each child at level+1 */
    for (int i = 0; i < emp->childCount; i++) {
        traverseOrg(emp->children[i], level + 1);
    }
}

// 5.3 Helper Functions (countNodes & treeHeight)
/* Count total nodes -> O(n) */
int countNodes(Employee *emp) {
    if (emp == NULL) return 0;
    int count = 1;
    for (int i = 0; i < emp->childCount; i++) {
        count += countNodes(emp->children[i]);
    }
    return count;
}

/* Tree height -> O(n) */
int treeHeight(Employee *emp) {
    if (emp == NULL) return 0;
    int maxH = 0;
    for (int i = 0; i < emp->childCount; i++) {
        int h = treeHeight(emp->children[i]);
        if (h > maxH) maxH = h;
    }
    return maxH + 1;
}

// Helper for the parser
Employee* findByName(const char* name) {
    for (int i = 0; i < poolSize; i++) {
        if (strcmp(pool[i].name, name) == 0) return &pool[i];
    }
    return NULL;
}

// 5.4 Input Parser
Employee* parseInput(const char *input) {
    char buf[4096];
    strncpy(buf, input, sizeof(buf));
    poolSize = 0;
    Employee* root = NULL;

    // We will store manager names during Pass 1 to link them in Pass 2
    char managers[MAX_EMPLOYEES][MAX_NAME]; 

    /* Pass 1: create all nodes */
    char* rec = strtok(buf, "|");
    while (rec != NULL && poolSize < MAX_EMPLOYEES) {
        char name[MAX_NAME], role[MAX_NAME], mgr[MAX_NAME];
        
        // Parse the comma-separated values
        sscanf(rec, " %63[^,], %63[^,], %63s", name, role, mgr);
        
        strncpy(pool[poolSize].name, name, MAX_NAME);
        strncpy(pool[poolSize].role, role, MAX_NAME);
        pool[poolSize].childCount = 0;
        
        // Save the manager name for Pass 2
        strncpy(managers[poolSize], mgr, MAX_NAME);
        
        if (strcmp(mgr, "ROOT") == 0) {
            root = &pool[poolSize];
        }
        
        poolSize++;
        rec = strtok(NULL, "|");
    }

    /* Pass 2: build parent-child links */
    for (int i = 0; i < poolSize; i++) {
        if (strcmp(managers[i], "ROOT") != 0) {
            Employee* parent = findByName(managers[i]);
            if (parent != NULL && parent->childCount < MAX_CHILDREN) {
                parent->children[parent->childCount++] = &pool[i];
            }
        }
    }
    return root;
}

// 5.5 main() - Entry Point Called by Flask
// 5.5 main() - Entry Point Called by Flask
int main(int argc, char *argv[]) {
    char input[4096] = {0};
    
    // Flask passes the string as the first argument
    if (argc >= 2) {
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
    
    // --- TIMING START ---
    clock_t start_time = clock();
    
    traverseOrg(root, 0);
    
    // --- TIMING END ---
    clock_t end_time = clock();
    double elapsed_ms = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000.0;

    int n = countNodes(root);
    int h = treeHeight(root);

    // --- SPACE CALCULATION ---
    // Total memory used by the actual nodes dynamically matched from the pool
    size_t node_memory_bytes = n * sizeof(Employee);
    // Estimated stack space (approx 48 bytes per recursive frame on 64-bit systems)
    size_t max_stack_bytes = h * 48; 

    // Format the stats to include both PRD Big-O and real numbers
    char stats[512];
    snprintf(stats, sizeof(stats), 
        "\n[STATS]\n"
        "n = %d\n"
        "h = %d\n"
        "Time: O(n) -> %.3f ms\n"
        "Space: O(h) -> %zu bytes (Call Stack) + %zu bytes (Nodes)\n", 
        n, h, elapsed_ms, max_stack_bytes, node_memory_bytes);
    emit(stats);

    // Print everything to stdout at once for Flask to capture
    printf("%s", outBuf);
    return 0;
}