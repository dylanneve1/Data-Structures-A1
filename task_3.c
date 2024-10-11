#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Hash table size
#define ARRAY_SIZE 59
// Max entry size
#define MAX_STRING_SIZE 100

// Hash able entry struct
typedef struct {
    char name[MAX_STRING_SIZE];
    int frequency;
} HashEntry;

// Hash table
HashEntry hashTable[ARRAY_SIZE];
// Keep track of collisions
int collisions = 0;
// Track the number of terms
int numTerms = 0;

// Hash function from assignment handout
int hash1(char *s) {
    int hash = 0;
    while (*s) {
        hash = (hash + *s - 'A') % ARRAY_SIZE;
        s++;
    }
    return hash;
}

int hash3(char *s) {
    int hash = 0;
    while (*s) {
        hash = 1 + (hash + *s - 'A') % (ARRAY_SIZE - 1);
        s++;
    }
    return hash;
}

// Function to insert name into hash table using double hashing
void insert(char *name) {
    // Get hashes using both hash functions
    int h1 = hash1(name);
    int h2 = hash3(name);
    // Index is h1, h2 is step
    int index = h1;

    // Double hashing to handle collisions
    while (hashTable[index].frequency != 0 && strcmp(hashTable[index].name, name) != 0) {
        // Increment index by h2
        index = (index + h2) % ARRAY_SIZE;
        collisions++;
    }

    // If the name already exists, increment the frequency
    if (hashTable[index].frequency != 0) {
        hashTable[index].frequency++;
    } else {
        // Otherwise, insert the name with frequency 1
        strcpy(hashTable[index].name, name);
        hashTable[index].frequency = 1;
        numTerms++;  // Increment unique term count
    }
}

// Function to search for a name in the hash table
void search(char *name) {
    // Get hashes using both hash functions
    int h1 = hash1(name);
    int h2 = hash3(name);
    // Index is h1, h2 is step
    int index = h1;

    while (hashTable[index].frequency != 0) {
        if (strcmp(hashTable[index].name, name) == 0) {
            printf("%s %d\n", name, hashTable[index].frequency);
            return;
        }
        index = (index + h2) % ARRAY_SIZE;
    }

    printf("%s not in table\n", name);
}

// Function to load names from a CSV file and insert them into the hash table
void loadCSV(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file\n");
        return;
    }

    char line[MAX_STRING_SIZE];
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) > 0) { // Ensure the line is not empty
            insert(line);
        }
    }

    fclose(file);
    printf("CSV loaded!\n");
}

// Function to display hash table statistics
void displayStats() {
    printf("Capacity : %d\n", ARRAY_SIZE);
    printf("Num Terms : %d\n", numTerms);
    printf("Collisions : %d\n", collisions);

    // Calculate load factor as a percentage
    double loadFactor = ((double)numTerms / ARRAY_SIZE);
    printf("Load : %.2f%%\n", loadFactor);
}

// Main
int main(int argc, char *argv[]) {
    // Check that file name was provided
    if (argc < 2) {
        // If not then exit
        printf("Usage: %s <csv_filename>\n", argv[0]);
        return 1;
    }

    // Initialize the hash table
    for (int i = 0; i < ARRAY_SIZE; i++) {
        hashTable[i].frequency = 0;
        hashTable[i].name[0] = '\0';
    }

    // Load the names from the CSV file
    loadCSV(argv[1]);

    // Display results
    displayStats();

    // User input to search for a name
    char input[MAX_STRING_SIZE];
    while (1) {
        printf("Enter name to get frequency or type 'quit' to exit:\n>>> ");

        // Read user input, ensure it isn't out of bounds
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }

        // Remove newline at end of input
        input[strcspn(input, "\n")] = '\0';

        // Quit if user inputs quit
        if (strcmp(input, "quit") == 0) {
            break;
        }

        // If input isn't blank then search
        if (strlen(input) > 0) {
            search(input);
        }
    }

    return 0;
}