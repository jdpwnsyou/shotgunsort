/** SHOTGUN SORT TEST-OUTIFIER **/

// shotgunsort.c
// John Dickinson
// October 14, 2014

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>

#define MAX_ELEMENTS 100
#define CLOSEST 0
#define SORTED 1
#define UNSORTED 2
#define ONE_THOUSAND    1000
#define ONE_MILLION     1000000
#define ONE_BILLION     1000000000
#define ONE_TRILLION    1000000000000
#define ONE_QUADRILLION 1000000000000000

/** GLOBAL VARIABLES (ugly hacks...) **/
unsigned int closest_attempt = 0;
static bool keep_running = true;
long long num_sort_attempts;	 // Number of sorting attempts
long long closest_attempt_itr;   // The iteration of the closest attempt
int num_array_elements;		 // Number of elements in the sorting array
int closest_array[MAX_ELEMENTS];

/** Function declarations **/
bool is_sorted(int array[]);
void int_handler();
void print_array(int array[], int array_status);
void print_dotted_line(void);
void print_loop_interval(long long i);
void print_human_readable_number(long long i);
int find_remainder(long long i);

/** MAIN **/
int main(int argc, char *argv[]) {

    int i;
    int limit_attempts;    // Limits shotgun sorting attempts (optional)
    bool array_sorted;     // True if array is sorted and false otherwise
    int temp;              // Needed for shotgun shuffling the array
    int random_array_pos;
    int max_array_elements = MAX_ELEMENTS;

    srand(time(NULL));     // Seed the random number generator
    num_sort_attempts = 0;
    signal(SIGINT, int_handler);

    // Test number of command line arguments and set defaults if necessary
    if (argc < 2) {
        num_array_elements = 5;
    }
    else {
        num_array_elements = atoi(argv[1]);
        // Array must contain more than 1 element to sort...
        if (num_array_elements < 2) {
            fprintf(stderr, "Array must contain 2 or more elements!\n");
            exit(0);
        }
    }

    //Check to see if attempts are limited
    if (argc > 2) {
        limit_attempts = atoi(argv[2]);
        printf("Attempting to shotgun sort a %d element array in %d tries...\n", num_array_elements, limit_attempts);
    }
    else {   //Unlimited
        limit_attempts = -1;
        printf("Attempting to shotgun sort a %d element array...\n", num_array_elements);
    }

    // Cap number of array elements
    if (num_array_elements > max_array_elements) {
        fprintf(stderr, "The number of elements in the array cannot exceed %d.\n", max_array_elements);
        exit(0);
    }

    int sorting_array[num_array_elements];

    // Populate the array to be sorted
    for(i = 0; i < num_array_elements; i++) {
        sorting_array[i] = rand() % 100;
    }

    print_array(sorting_array, UNSORTED);
    print_dotted_line();

    /** MAIN SORTING LOOP **/
    while(keep_running == true) {

        //Print loop count every 100,000,000 attempts
        if(num_sort_attempts > 0) {
            print_loop_interval(num_sort_attempts);
        }

        //Shotgun Sort the array
        for(i = 0; i < num_array_elements; i++) {
            random_array_pos = rand() % num_array_elements;
            temp = sorting_array[i];
            sorting_array[i] = sorting_array[random_array_pos];
            sorting_array[random_array_pos] = temp;
        }

        //Check order
        array_sorted = is_sorted(sorting_array);

        // If sorted...
        if (array_sorted == true) {
            print_dotted_line();
            printf("Array successfully sorted after %lld attempts! ", num_sort_attempts);
            print_human_readable_number(num_sort_attempts);
            printf("\n");
            print_array(sorting_array, SORTED);
            break;
        }

        //If not sorted (unlimited attempts)
        else if (limit_attempts == -1) {
            num_sort_attempts++;
            continue;
        }

        //If not sorted (limited attempts)
        else if (num_sort_attempts < limit_attempts) {
            num_sort_attempts++;
            continue;
        }

        //If attempt limit reached
        else {
            print_dotted_line();
            closest_attempt++; // Convert to human counting...
            printf("Array not sorted after %lld attempts! ", num_sort_attempts);
            print_human_readable_number(num_sort_attempts);
            printf("\nThe closest attempt had %d elements in ascending order\n", closest_attempt);
            print_array(closest_array, CLOSEST);
            break;
        }
    }
    return 0;  
    //ttfn
}

/** Function to check if the array is sorted **/
bool is_sorted(int array[]) {

    int i;
    int j;

    for(i = 0; i < num_array_elements-1; i++) {
        if (array[i] > array[i+1]) {

            //Track closest sorting attempt (incoming hack...)
            if (i > closest_attempt) {
                closest_attempt = i + 1; //Convert to human counting (1 initiator)
                closest_attempt_itr = num_sort_attempts;
                print_array(array, CLOSEST);
                closest_attempt = i;     //Revert to computer counting (0 initiator)
                for(j = 0; j < num_array_elements; j++) {
                    closest_array[j] = array[j];
                }
            }
            //Not sorted
            return false;
        }
    }
    //Sorted!
    return true;
}

/** Interrupt handler **/
void int_handler() {
    keep_running = false;
    closest_attempt++;	// Convert to human counting again...
    print_dotted_line();
    fprintf(stderr, "Sorting attempt cancelled after %lld attempts", num_sort_attempts);
    fprintf(stderr, "\nThe closest attempt had %d elements in ascending order\n", closest_attempt);
    print_array(closest_array, CLOSEST);
    exit(0);
}

/** Pretty array printer **/
void print_array(int array[], int array_status) {

    int i;

    if (array_status == CLOSEST) {
        printf("Closest attempt: [ ");
    }
    else if (array_status == SORTED) {
        printf("Sorted Array:    [ ");
    }
    else if (array_status == UNSORTED) {
        printf("Unsorted Array:  [ ");
    }

    for(i = 0; i < num_array_elements; i++) {
        printf("%d ", array[i]);
    }

    if (array_status == CLOSEST) {
        printf("] (");
        printf("%2d/%-2d", closest_attempt, num_array_elements);
        printf(") ");
        printf("(%lld)\n",closest_attempt_itr);
    }
    else {
        printf("]\n");
    }
} 

/* This function takes a long long number as input and 
 * prints it in human readable format.
 * eg. - 1200000000 is printed 1.2 billion */
void print_human_readable_number(long long number) {

    int remainder = find_remainder(number);

    if(number < ONE_MILLION) {
        number = number / ONE_THOUSAND;
        printf("(~%lld.%d thousand)", number, remainder);
    }
    else if(number < ONE_BILLION) {
        number = number / ONE_MILLION;
        printf("(~%lld.%d million)", number, remainder);
    }
    else if(number < ONE_TRILLION) {
        number = number / ONE_BILLION;
        printf("(~%lld.%d billion)", number, remainder);
    }
    else if(number < ONE_QUADRILLION) {
        number = number / ONE_TRILLION;
        printf("(~%lld.%d trillion)", number, remainder);
    }
    else {
        //No way this program ever runs this long
        printf("(YOU'RE WASTING CPU POWER...)");
    }
}

/* Called by print_human_readable_number to work out the remainder
 * for printing easily readable values. */
int find_remainder(long long number) {

    int remainder = 0;

    if(number < ONE_MILLION) {
        if ((number % ONE_THOUSAND) < 100) {
            remainder = 0;
        }
        else {
            remainder = (number % ONE_THOUSAND) / 100;
        }
    }
    else if(number < ONE_BILLION) {
        if ((number % ONE_MILLION) < 100000) {
            remainder = 0;
        }
        else {
            remainder = (number % ONE_MILLION) / 100000;
        }
    }
    else if(number < ONE_TRILLION) {
        if ((number % ONE_BILLION) < 100000000) {
            remainder = 0;
        }
        else {
            remainder = (number % ONE_BILLION) / 100000000;
        }
    }
    return(remainder);
}

/* Prints loop interval */
void print_loop_interval(long long attempts) {

    //Print out every 100 million iterations
    if(attempts < ONE_BILLION && attempts % (ONE_MILLION * 100) == 0) {
        printf("Passing %lld million attempts...\n", attempts / 1000000);
    }

    //Print every 1 billion iterations
    else if(attempts < ONE_TRILLION && attempts % ONE_BILLION == 0) {
        printf("Passing %lld billion attempts...\n", attempts / 1000000000);
    }

    //Print every 1 trillion iterations
    else if(attempts < ONE_QUADRILLION && attempts % ONE_TRILLION == 0) {
        printf("Passing %lld trillion attempts...\n", attempts / 1000000000000);
    }
}

/** Prints a single dotted line followed by a newline to stdout **/
void print_dotted_line(void) {
    printf("-------------------------------------------------------------\n");
}