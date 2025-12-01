#include <stdlib.h>//for malloc
#include <stddef.h>//for NULL
#include <stdio.h>//for printf

int main (void) 
{
  char *shadow = malloc(100);
  if (shadow == NULL)
  {
    return -10;
  }

  printf("YAY! We passed!\n");

  free(shadow);

  return 20;
}
