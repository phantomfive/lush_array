#include <stdio.h>
#define DEBUG 1
#include "lush_array.h"
Array *a;

void set(){
  int loc, val;
  printf("Enter a location: ");
  scanf("%d",&loc);
  printf("Enter a value: ");
  scanf("%d",&val);
  array_set(a,loc,(void *)val);
}

void view(){
  int loc;
  printf("Enter a location: ");
  scanf("%d",&loc);
  printf("****value is %d ****\n", (int)array_get(a,loc));
}

void pushEnd(){
  int val;
  printf("Enter a value: ");
  scanf("%d",&val);
  array_add_end(a, (void *)val);
}

void popEnd(){
  printf("****value is %d *****\n",(int)array_pop_end(a));
}

void pushFront(){
  int val;
  printf("Enter value to push: ");
  scanf("%d",&val);
  array_add_front(a, (void *)val);
}

void popFront(){
  printf("*****value is %d *****\n",(int)array_pop_front(a));
}

void printArray(){
  array_printi(a);
}

void printInternal(){
  array_print_internal(a);
}
 
void destroy(){
  array_destroy(a);
  a  = array_init();
}

int main(){

  int choice;

  a = array_init();

  while(1){
    printf("Menu:\n");
    printf("1.Set\n");
    printf("2.View\n");
    printf("3.Push end\n");
    printf("4.Pop end\n");
    printf("5.Push front\n");
    printf("6.Pop front\n");
    printf("7.Print array\n");
    printf("8.Print internal\n");
    printf("9.destroy array\n");
    printf("10.Quit\n");

    scanf("%d",&choice);
    
    switch(choice){
    case 1: set();  break;
    case 2: view(); break;
    case 3: pushEnd(); break;
    case 4: popEnd();  break;
    case 5: pushFront(); break;
    case 6: popFront(); break;
    case 7: printArray();break;
    case 8: printInternal();break;
    case 9: destroy(); break;
    case 10:return 0;
    }
  }
 return -1;
}
