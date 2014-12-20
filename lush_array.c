/* COPYRIGHT 2005 Andrew Thompson */
/* You may use this file according to the conditions specified in the GPL.
   All other rights reserved.  Use at your own risk. 
   lush array Version 1.0
*/
#include <stdlib.h>
#include "lush_array.h"

#ifdef DEBUG
#include <stdio.h>
#endif

enum ARRAY_CONSTANTS{
    SUB_ARRAY_MAX = 100,
    MAIN_ARRAY_DEFAULT_SIZE = 50
};

/*Define the Array struct*/
struct _array{
    int  end;           /*this is the end of the virtual array...for pushing
    and popping. end is the first empty slot */
    void **main_array;  /*The main array: we have several sub-arrays coming off
        this main array.  We store the data in the sub-arrays
        and only create them when data is stored there */
    int main_array_max; /*The largest value for main_array for which memory
        has been allocated. */
    int main_array_zero;  /*the place in the main array that is considered zero*/
    /*This is where we will look for a[0] */
    int sub_array_zero;   /*the place in that sub-array that is considered zero*/
};

static void *create_sub_array(void);
static int create_new_main_array(Array *a, int new_size);
static void shift_up(Array *a);
static void shift_down(Array *a);

/* too weird to fit in a function, and too frequent and annoying to
   type each time it is needed, best to put it in a macro */
#define calc_main_and_sub_index()       \
                                        \
   main_array_index = index / SUB_ARRAY_MAX + a->main_array_zero;\
   sub_array_index  = (index+a->sub_array_zero) % SUB_ARRAY_MAX;\
   if(sub_array_index<a->sub_array_zero) main_array_index++;



/*-------------------*/
Array *array_init(){
  int i;
  Array *a;
  
  a = (Array*)malloc(sizeof(struct _array));

  /* create main_array */
  a->main_array = (void**)malloc(sizeof(void*)*MAIN_ARRAY_DEFAULT_SIZE);
  if(a->main_array==NULL) return NULL;
  for(i=0;i<MAIN_ARRAY_DEFAULT_SIZE;i++)
    a->main_array[i] = NULL;

  /* set defaults */
  a->main_array_zero = MAIN_ARRAY_DEFAULT_SIZE/2;
  a->main_array_max  = MAIN_ARRAY_DEFAULT_SIZE - 1;
  a->end = 0;
  /* create one sub_array */
  a->main_array[a->main_array_zero] = create_sub_array();
  if(a->main_array[a->main_array_zero] == NULL) return 0;
  a->sub_array_zero = 0;
  
  return a;
}

/*-------------------------*/
void array_destroy(Array *a){
  int i;
  /* free sub-arrays */
  for(i=0;i<=a->main_array_max;i++)
    if(a->main_array[i]!=NULL)
      free(a->main_array[i]);
  
  /* free main array */
  free(a->main_array);
}

/*------------------------------------------*/
int array_set(Array *a, int index, void *item){
  int main_array_index, sub_array_index;

  /* if index < 0, count it from the back */
  while(index < 0)
    index = a->end + index+1;

  /* calculate which one we are really going to put it in */
  calc_main_and_sub_index();
  /* if it is beyond the range of the main array, we will need to create
     a new one twice as big. */
  if(main_array_index > a->main_array_max){
    if(!create_new_main_array(a, main_array_index*4)) return 0;
    calc_main_and_sub_index();
  }else if(main_array_index < 0){
    /* create the new one with twice the size */
    if(!create_new_main_array(a, (a->main_array_max+(main_array_index*-1))*4))
      return 0;
    calc_main_and_sub_index();
  }
  
  /* if the sub_array hasn't been allocated, allocate it */
  if(a->main_array[main_array_index] == NULL)
    if((a->main_array[main_array_index] = create_sub_array())==NULL)return 0;
  if(a->main_array[main_array_index] == NULL)
    return 0;
  
  /* actually set the thing */
  ((char **)(a->main_array[main_array_index]))[sub_array_index] = item;

  /* check to see if the end changed */
  if(index>=a->end)
    a->end = index+1;
  return 1;
}

/*---------------------------------*/
void *array_get(Array *a, int index){
  int main_array_index, sub_array_index;
  
  /* if index < 0, count it from the back */
  while(index<0)
    index = a->end + index+1;

  calc_main_and_sub_index();
  
  if(main_array_index > a->main_array_max) return NULL;
  else if(a->main_array[main_array_index]==NULL) return NULL;
  else return ((char**)a->main_array[main_array_index])[sub_array_index];
}

/*-----------------------------------*/
int array_add_end(Array *a, void *item){
  return array_set(a,a->end,item);
}

/*--------------------------*/
void *array_pop_end(Array *a){
  void *rv;
  if(a->end <= 0)
    return NULL;
  rv = array_get(a,a->end-1);
  a->end--;
  return rv;
}

/*------------------------------------*/
int array_add_front(Array *a,void *item){
  if(a->end == 0) 
    return array_set(a,0,item);
  shift_up(a);
  return array_set(a,0,item);
}

/*----------------------------*/
void *array_pop_front(Array *a){
  void *rv;
  if(a->end == 0) return NULL;
  rv = array_get(a,0);
  shift_down(a);
  return rv;
}

/*----------------------------*/
inline int array_size(Array *a){
  return a->end;
}

/*----------------------------------*/
#ifdef DEBUG
void array_prints(Array *a){
  int i;
  for(i=0;i<a->end;i++)
    printf("%03d: %s\n",i,(char *)array_get(a,i));
}

void array_printi(Array *a){
  int i;
  for(i=0;i<a->end;i++)
    printf("%03d: %4d\n",i,(int)array_get(a,i));
}

void array_print_internal(Array *a){
  int i,j;

  for(i=0;i<=a->main_array_max;i++){
    printf("%04d: ",i);
    if(a->main_array[i]==NULL)
      printf("  NULL");
    else{
      for(j=0;j<SUB_ARRAY_MAX;j++)
	printf("%2d ",(int)((char **)a->main_array[i])[j]);
    }
    printf("\n");
  }
}
#endif


/*-------------------------------------------------------------------------*/
/*Internal functions */
/*-------------------------------------------------------------------------*/
static void *create_sub_array(void){
  int i;
  void * rv =  (void *)malloc(sizeof(void *) * SUB_ARRAY_MAX);
  if(rv == NULL) return NULL;
  for(i=0;i<SUB_ARRAY_MAX;i++)
    ((char **)rv)[i] = NULL;
  return rv;
}

/*----------------------------------------------------
  creates a new array and copies everything over.
  Doesn't do much error checking.
*/
static int create_new_main_array(Array *a, int new_size){
  int i,start,stop;
  void **new_main_array;

  /* allocate memory for the new array */
  new_main_array=(void**)malloc(sizeof(void*)*new_size);
  if(new_main_array==NULL) return NULL;

  /* figure out when we need to start copying over. */
  /* We copy the old array ino the middle of the new */
  start = (new_size - a->main_array_max)/2;
  stop  = start + a->main_array_max;
  
  /* initialize everything before the old array */
  for(i=0;i<start;i++)
    new_main_array[i] = NULL;
  /* copy the old array over */
  for(i=start;i<=stop;i++)
    new_main_array[i] = a->main_array[i-start];
  /* initialize the rest to NULL */
  for(i=stop+1;i<new_size;i++)
    new_main_array[i] = NULL;

  free(a->main_array);
  
  /* adjust the data */
  a->main_array = new_main_array;
  a->main_array_max = new_size - 1;
  a->main_array_zero = a->main_array_zero + start;

  return 1;
}

/*---------------------------------------------------
Shifts all the elements up one.  It really does this
by moving the indexes down, but has that effect.
Leaves an empty space at Array[0].
---------------------------------------------------*/
static void shift_up(Array *a){
  a->sub_array_zero--;
  /*if we ran off the end of the sub_array....*/
  if(a->sub_array_zero<0){
    a->sub_array_zero=SUB_ARRAY_MAX-1;
    a->main_array_zero--;
    /* we don't need to worry about allocating memory
       because it will be done in array_set()      */
  }
  a->end++;
}

/*--------------------------------
  sets Array[0] to NULL, moves
  everything else down.
---------------------------------*/
static void shift_down(Array *a){
  array_set(a,0,NULL);
  a->sub_array_zero++;
  if(a->sub_array_zero>=SUB_ARRAY_MAX){
    a->sub_array_zero = 0;
    a->main_array_zero++;
  }
  a->end--;
}
