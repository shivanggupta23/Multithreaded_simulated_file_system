#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_FILES 10
#define MAX_NAME  50
#define MAX_CONTENT 256

// ANSI colors
#define GREEN   "\x1b[32m"
#define RED     "\x1b[31m"
#define YELLOW  "\x1b[33m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

typedef struct File {
    char name[MAX_NAME];
    char content[MAX_CONTENT];
    pthread_mutex_t lock;   // fine-grained lock per file
} File;

File files[MAX_FILES];
int file_count = 0;
pthread_mutex_t dir_lock = PTHREAD_MUTEX_INITIALIZER;

// Create file
void create_file(const char *name) {
    pthread_mutex_lock(&dir_lock);
    if (file_count < MAX_FILES) {
        strcpy(files[file_count].name, name);
        files[file_count].content[0] = '\0';
        pthread_mutex_init(&files[file_count].lock, NULL);
        file_count++;
        printf(GREEN "✔ File created: %s\n" RESET, name);
    } else {
        printf(RED "✘ Directory full, cannot create more files.\n" RESET);
    }
    pthread_mutex_unlock(&dir_lock);
}

// Write to file
void write_file(const char *name, const char *data) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(files[i].name, name) == 0) {
            // Try to acquire lock first
            if (pthread_mutex_trylock(&files[i].lock) != 0) {
                printf(RED "⏳ File %s is currently locked, waiting...\n" RESET, name);
                pthread_mutex_lock(&files[i].lock); // block until available
            }
            printf(CYAN "🔒 Lock acquired on file: %s\n" RESET, name);
            strncpy(files[i].content, data, MAX_CONTENT);
            printf(GREEN "✔ Written to %s: %s\n" RESET, name, data);
            pthread_mutex_unlock(&files[i].lock);
            printf(CYAN "🔓 Lock released on file: %s\n" RESET, name);
            return;
        }
    }
    printf(RED "✘ File not found: %s\n" RESET, name);
}

// Read file
void read_file(const char *name) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(files[i].name, name) == 0) {
            if (pthread_mutex_trylock(&files[i].lock) != 0) {
                printf(RED "⏳ File %s is currently locked, waiting...\n" RESET, name);
                pthread_mutex_lock(&files[i].lock);
            }
            printf(CYAN "🔒 Lock acquired on file: %s\n" RESET, name);
            printf(YELLOW "📖 Read from %s: %s\n" RESET, name, files[i].content);
            pthread_mutex_unlock(&files[i].lock);
            printf(CYAN "🔓 Lock released on file: %s\n" RESET, name);
            return;
        }
    }
    printf(RED "✘ File not found: %s\n" RESET, name);
}

// Delete file
void delete_file(const char *name) {
    pthread_mutex_lock(&dir_lock);
    for (int i = 0; i < file_count; i++) {
        if (strcmp(files[i].name, name) == 0) {
            for (int j = i; j < file_count - 1; j++) {
                files[j] = files[j+1];
            }
            file_count--;
            printf(GREEN "✔ File deleted: %s\n" RESET, name);
            pthread_mutex_unlock(&dir_lock);
            return;
        }
    }
    printf(RED "✘ File not found: %s\n" RESET, name);
    pthread_mutex_unlock(&dir_lock);
}

int main() {
    int choice;
    char name[MAX_NAME], data[MAX_CONTENT];

    while (1) {
        printf("\n" YELLOW "=== File System Menu ===\n" RESET);
        printf("1. Create File\n2. Write File\n3. Read File\n4. Delete File\n5. Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            printf(RED "Invalid input. Please enter a number.\n" RESET);
            exit(1);
        }

        switch (choice) {
            case 1:
                printf("Enter file name: ");
                scanf("%s", name);
                create_file(name);
                break;
            case 2:
                printf("Enter file name: ");
                scanf("%s", name);
                printf("Enter content: ");
                scanf(" %[^\n]", data);
                write_file(name, data);
                break;
            case 3:
                printf("Enter file name: ");
                scanf("%s", name);
                read_file(name);
                break;
            case 4:
                printf("Enter file name: ");
                scanf("%s", name);
                delete_file(name);
                break;
            case 5:
                printf(GREEN "Exiting File System Simulator...\n" RESET);
                return 0;
            default:
                printf(RED "Invalid choice, try again.\n" RESET);
        }
    }
}
