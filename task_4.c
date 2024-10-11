#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Hash table size
#define ARRAY_SIZE 64000
// Max entry size
#define MAX_STRING_SIZE 100

// Struct for person details
typedef struct Person {
    char personID[MAX_STRING_SIZE];
    char depositionID[MAX_STRING_SIZE];
    char forename[MAX_STRING_SIZE];
    int age;
    char personType[MAX_STRING_SIZE];
    char gender[MAX_STRING_SIZE];
    char nationality[MAX_STRING_SIZE];
    char religion[MAX_STRING_SIZE];
    char occupation[MAX_STRING_SIZE];
    struct Person *next;  // Linked list to handle multiple people with same surname
} Person;

// Hash table entry struct
typedef struct {
    char surname[MAX_STRING_SIZE];
    Person *people;  // Pointer to a linked list of people with the same surname
} HashEntry;

// Hash table
HashEntry hashTable[ARRAY_SIZE];
// Keep track of collisions
int collisions = 0;
// Track the number of terms
int numTerms = 0;

// Hash function from assignment handout
unsigned int hash1(char *s) {
    unsigned int hash = 0;
    while (*s) {
        hash = (hash * 31 + *s - 'A') % ARRAY_SIZE;
        s++;
    }
    return hash;
}

unsigned int hash3(char *s) {
    unsigned int hash = 0;
    while (*s) {
        hash = 1 + (hash * 31 + *s - 'A') % (ARRAY_SIZE - 1);
        s++;
    }
    return hash;
}

// Simple function to strip leading/trailing quotes and remove trailing newline character
void stripQuotes(char *str) {
    // Check if the first character is a quote and remove it
    if (str[0] == '"') {
        memmove(str, str + 1, strlen(str));
    }

    // Get the length of the string after modification
    int len = strlen(str);

    // Check if the last character is a quote and remove it
    if (str[len - 1] == '"') {
        str[len - 1] = '\0';
    }
}

// Function to create a person from CSV data
Person *createPerson(char *personID, char *depositionID, char *forename, int age, char *personType, char *gender, char *nationality, char *religion, char *occupation) {
    Person *newPerson = (Person *)malloc(sizeof(Person));
    strcpy(newPerson->personID, personID);
    strcpy(newPerson->depositionID, depositionID);
    strcpy(newPerson->forename, forename);
    newPerson->age = age;
    strcpy(newPerson->personType, personType);
    strcpy(newPerson->gender, gender);
    strcpy(newPerson->nationality, nationality);
    strcpy(newPerson->religion, religion);
    strcpy(newPerson->occupation, occupation);
    newPerson->next = NULL;  // Set next pointer to NULL
    return newPerson;
}

// Function to insert a person into the hash table using double hashing
void insert(char *surname, Person *person) {
    unsigned int h1 = hash1(surname);
    unsigned int h2 = hash3(surname);
    unsigned int index = h1;

    // Double hashing to handle collisions
    while (hashTable[index].people != NULL && strcmp(hashTable[index].surname, surname) != 0) {
        index = (index + h2) % ARRAY_SIZE;
        collisions++;
    }

    // If the surname is already in the table, add the person to the linked list
    if (hashTable[index].people != NULL) {
        Person *current = hashTable[index].people;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = person;
    } else {
        // Otherwise, insert the surname and the person as the first entry in the linked list
        strcpy(hashTable[index].surname, surname);
        hashTable[index].people = person;
        numTerms++;  // Increment unique surname count
    }
}

// Function to search for a surname and print all associated people
void search(char *surname) {
    unsigned int h1 = hash1(surname);
    unsigned int h2 = hash3(surname);
    unsigned int index = h1;

    while (hashTable[index].people != NULL) {
        if (strcmp(hashTable[index].surname, surname) == 0) {
            // Print the header with column widths for better readability
            printf("%-12s %-15s %-12s %-20s %-4s %-12s %-8s %-12s %-10s %-12s\n",
                   "Person ID", "Deposition ID", "Surname", "Forename", "Age",
                   "Person Type", "Gender", "Nationality", "Religion", "Occupation");

            // Print all people with this surname in a formatted way
            Person *current = hashTable[index].people;
            while (current != NULL) {
                printf("%-12s %-15s %-12s %-20s %-4d %-12s %-8s %-12s %-10s %-12s\n",
                       current->personID, current->depositionID, surname, current->forename,
                       current->age, current->personType, current->gender, current->nationality,
                       current->religion, current->occupation);
                current = current->next;
            }
            return;
        }
        index = (index + h2) % ARRAY_SIZE;
    }

    printf("%s not in table\n", surname);
}

// Function to load people from a CSV file and insert them into the hash table
void loadCSV(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file\n");
        return;
    }

    char line[MAX_STRING_SIZE * 10];  // Adjust buffer size for long lines
    while (fgets(line, sizeof(line), file)) {
        // Tokenize and parse CSV fields
        char *personID = strtok(line, ",");
        char *depositionID = strtok(NULL, ",");
        char *surname = strtok(NULL, ",");
        char *forename = strtok(NULL, ",");
        int age = atoi(strtok(NULL, ","));
        char *personType = strtok(NULL, ",");
        char *gender = strtok(NULL, ",");
        char *nationality = strtok(NULL, ",");
        char *religion = strtok(NULL, ",");
        char *occupation = strtok(NULL, "\n");

        // Strip quotes from all parsed fields
        stripQuotes(personID);
        stripQuotes(depositionID);
        stripQuotes(surname);
        stripQuotes(forename);
        stripQuotes(personType);
        stripQuotes(gender);
        stripQuotes(nationality);
        stripQuotes(religion);
        stripQuotes(occupation);

        // Create a person and insert into the hash table
        Person *person = createPerson(personID, depositionID, forename, age, personType, gender, nationality, religion, occupation);
        insert(surname, person);
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

// Main function
int main(int argc, char *argv[]) {
    // Check that file name was provided
    if (argc < 2) {
        printf("Usage: %s <csv_filename>\n", argv[0]);
        return 1;
    }

    // Initialize the hash table
    for (int i = 0; i < ARRAY_SIZE; i++) {
        hashTable[i].people = NULL;
        hashTable[i].surname[0] = '\0';
    }

    // Load the names from the CSV file
    loadCSV(argv[1]);

    // Display results
    displayStats();

    // User input to search for a surname
    char input[MAX_STRING_SIZE];
    while (1) {
        printf("Enter surname to get list of people or type 'quit' to exit:\n>>> ");

        // Read user input
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }

        // Remove newline character
        input[strcspn(input, "\n")] = '\0';

        // Quit if user inputs quit
        if (strcmp(input, "quit") == 0) {
            break;
        }

        // Search for people with the input surname
        if (strlen(input) > 0) {
            search(input);
        }
    }

    return 0;
}