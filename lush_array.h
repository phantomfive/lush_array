/* COPYRIGHT 2005 Andrew Thompson */
/* You may use this file according to the conditions specified in the GPL.
   All other rights reserved.  Use at your own risk. 
   lush array version 1.0
*/

/* This is an expandable array, much like the ones found in perl. You can
   add something at any index, if it goes beyond the end of the array extra
   memory will be added.  Everything is by default set to NULL. Negative
   numbers will wrap around to the end of the array, so Array[-5] is really
   Array[end - 4].  The array starts at index 0.
*/

#ifndef LUSH_ARRAY_H
#define LUSH_ARRAY_H 1

/* array data type */
typedef struct _array Array;


/* all functions return 0 or NULL on failure and a pointer or a non-zero
   integer on success. */

Array *array_init();
void array_destroy(Array *a);

/* set index of array a to item: a[index] = item; */
int array_set(Array *a, int index, void *item);

/* get the array element at index: return a[index]; */
void *array_get(Array *a, int index);

/* push an element to the end off the array */
int array_add_end(Array *a, void *item);

/* take the element off the end of the array and returns it*/
void *array_pop_end(Array *a);

/* adds an element to the beginning of the array */
int array_add_front(Array *a,void *item);

/* takes the element off the beginning of the array and returns it */
void *array_pop_front(Array *a);

/* returns the number of elements in the array */
inline int array_size(Array *a);

#ifdef DEBUG
/* this will print the array as strings */
void array_prints(Array *a);
/* this will print the array as integers */
void array_printi(Array *a);
/* this will print it the way it is stored internally */
void array_print_internal(Array *a);
#endif

/* An explanation of the algorithm used. 

main_array
|-|
| |
|-|
| |     sub arrays              
|-|   -------------------------------
| |-->|1 |2 |3 |4 |5 |6 |7 |8*|9 |10|
| |   -------------------------------
|-|
| |   -------------------------------
| |-->|11|12|13|14|15|16|17|18|19|20|
|-|   -------------------------------
| |
|-|
| |
|-|
| |
|-|

The data is stored in the sub-arrays, as shown.  Sub-arrays are only
allocated when they are needed.  For example, if you wanted to access
array element 8 like this: array_set(a,8,someObject) then the index with
the * would be accessed.  It takes some annoying calculations to find that 
element, though.
If our main array gets too full, then we create a bigger one. and copy all
the pointers over.
The reason we start in the middle is so we can easily handle pushes onto the
front of the array.
By the way, this isn't conceptually hard but it is tricky and there are
a lot of special cases.
*/

#endif
