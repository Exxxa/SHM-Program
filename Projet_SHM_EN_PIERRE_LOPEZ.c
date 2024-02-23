// By LOPEZ Pierre & MOY Zacharie, class 40
// ESIEA23_6

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// Maximum size of the string
#define SHM_SIZE 80

// Shared memory structure
struct SharedMemory {
    char message[SHM_SIZE];
};

// Function prototypes for reader and writer
void reader(int shmId);
void writer(int shmId);

int main(int argc, char *argv[]) {
    // Generates a key for the shared memory segment using ftok
    key_t key = ftok("/tmp", 'R');

    // Creates shared memory segment with a given key and size of SharedMemory structure
    int shmId = shmget(key, sizeof(struct SharedMemory), IPC_CREAT | 0666);

    // Handles a potential failure in creating or accessing the shared memory segment using shmget
    if (shmId == -1) {
        // Print an error message with perror
        perror("shmget");
        //Exit the program with failure status if unsuccessful
        exit(EXIT_FAILURE);
    }

    if (argc == 1) {
        // Launch as READER if no command-line arguments provided
        reader(shmId);
    } else {
        // Launch as WRITER if command-line arguments exists
        writer(shmId);
    }

    // Removes shared memory segment after usage
    if (shmctl(shmId, IPC_RMID, NULL) == -1) {
        // Prints an error message related to the last system call
        perror("shmctl");
        //Exits the program with failure status if unsuccessful
        exit(EXIT_FAILURE);
    }

    return 0;
}

void reader(int shmId) {
    // Attach to the shared memory segment
    struct SharedMemory *shmPtr = shmat(shmId, NULL, 0);

    // Checks if the shmat function failed to attach the shared memory segment
    if (shmPtr == (void *) -1) {
        // Prints an error message related to the failure of the shmat function
        perror("shmat");
        // Exits the program with a failure status if attaching shared memory failed
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Clears the terminal
        printf("\033[H\033[J");

        // Prints the content of the shared memory
        printf("Reader: %s\n", shmPtr->message);

        // 1Hz print loop
        if (strcmp(shmPtr->message, "QUIT") == 0 || strcmp(shmPtr->message, "quit") == 0) {
            break;
        }
        
        sleep(1);
    }
}

void writer(int shmId) {
    // Attach to the shared memory segment
    struct SharedMemory *shmPtr = shmat(shmId, NULL, 0);

    // Checks if the shmat function failed to attach the shared memory segment
    if (shmPtr == (void *) -1) {
        // Prints an error message related to the failure of the shmat function
        perror("shmat");
        //Exits the program with failure status if unsuccessful
        exit(EXIT_FAILURE);
    }

    // Prompt the user to enter strings
    printf("Writer: Enter strings (type QUIT to terminate):\n");

    //Infinite loop
    while (1) {
        // Read up to 79 characters (excluding newline)
        scanf("%79[^\n]", shmPtr->message);

        // Consume the newline character
        getchar();

        // Terminate on QUIT or quit
        if (strcmp(shmPtr->message, "QUIT") == 0 || strcmp(shmPtr->message, "quit") == 0) {
            break;
        }
    }

    // Detach from the shared memory segment
    if (shmdt(shmPtr) == -1) {
        // Prints an error message related to the failure of the shmdt function,
        // indicating any issues encountered while detaching the shared memory segment
        perror("shmdt");
        //Exits the program with failure status if unsuccessful
        exit(EXIT_FAILURE);
    }
}