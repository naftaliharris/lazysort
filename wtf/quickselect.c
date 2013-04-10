/* quickselect.c
 * By Naftali Harris
 * Depending on whether you want to simulate pointers or not, this compiles with
 * $ gcc quickselect.c
 * or
 * $ gcc -DPOINTER quickselect.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef POINTER
typedef int *object;
#define ISLT(obj1, obj2) (*(obj1) < *(obj2))
#else
typedef int object;
#define ISLT(obj1, obj2) ((obj1) < (obj2))
#endif

object *make_random_data(int n)
{
    object *res = malloc(n * sizeof(object));
    int i;
    for (i = 0; i < n; i++) {
#ifdef POINTER
        res[i] = (int *)malloc(sizeof(int *));
        *res[i] = rand();
#else
        res[i] = rand();
#endif
    }
    return res;
}

#define SWAP(i, j) tmp = dat[i]; dat[i] = dat[j]; dat[j] = tmp

/* Partitions the data around a a pivot and returns the sorted pivot index */
int partition(object *dat, int len)
{
    object tmp;
    int pivot_index = rand() % len;
    object pivot = dat[pivot_index];
    int last_less = 0;
    SWAP(last_less, pivot_index);

    /* Invariant: last_less and everything to its left is less the pivot or
     * the pivot itself. Also, last_less <= i */
    int i;
    for (i = 1; i < len; i++) {
        if (ISLT(dat[i], pivot)) {
            last_less++;
            SWAP(last_less, i);
        }
    }

    SWAP(last_less, 0); /* Put the pivot back in its place */
    return last_less;
}

void shuffle(object *dat, int len)
{
    object tmp;
    int i;
    for (i = 0; i < len; i++) {
        SWAP(i, i + rand() % (len - i));
    }
}

void insertion_sort(object *dat, int len)
{
    int i, j;
    for (i = 0; i < len; i++) {
        object item = dat[i];
        for (j = i; j > 0 && ISLT(item, dat[j - 1]); j--) {
            dat[j] = dat[j - 1];
        }
        dat[j] = item;
    }
}

/* Returns the kth smallest element of dat */
object quickselect(object *dat, int k, int len)
{
    if (len > 16) {
        int pivot_index = partition(dat, len);
        if (pivot_index > k) {
            return(quickselect(dat, k, pivot_index));
        }
        else if (pivot_index < k) {
            return(quickselect(&dat[pivot_index + 1], k - pivot_index - 1, len - pivot_index - 1));
        }
        else {
            return dat[k];
        }
    }
    else {
        insertion_sort(dat, len);
        return dat[k];
    }
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s list_size reps\n", argv[0]);
        exit(-1);
    }

    srand(time(NULL));

    int n = atoi(argv[1]);
    int reps = atoi(argv[2]);

    object *data = make_random_data(n);
    shuffle(data, n);
    object *tmp = malloc(n * sizeof(object));
    
    clock_t begin = clock();
    int i;
    for (i = 0; i < reps; i++) {
        memcpy(tmp, data, n * sizeof(object));
        object median = quickselect(tmp, n / 2, n);
    }
    clock_t end = clock();
    double time_elapsed = (double)(end - begin) / CLOCKS_PER_SEC;

    fprintf(stdout, "Average Time: %.7f\n", time_elapsed / reps);

    return 0;
}
