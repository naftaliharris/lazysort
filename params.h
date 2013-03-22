/* Parameters for the sorting function */

/* SORT_THRESH: Sort if the sublist has SORT_THRESH or fewer elements */
#define SORT_THRESH 8

/* CONTIG_THRESH: When computing slices with integer step sizes, sort all data
 * between start and stop and then populate the list with it if 
 * |step| <= CONTIG_THRESH, otherwise select each element individually.
 * CONTIG_THRESH should always be bigger than SORT_THRESH */
#define CONTIG_THRESH 32
