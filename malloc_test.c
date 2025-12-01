#include <stdlib.h>//for malloc
#include <stddef.h>//for NULL
#include <stdio.h>//for printf
#include <unistd.h>//for write

int main (void) 
{
  char *shadow = malloc(16);
  if (shadow == NULL)
  {
    return -10;
  }

  write(1,"\n\n\n\n\n", 5);

  char *shadow2 = malloc(152);
  if (shadow2 ==  NULL)
  {
    return -20;
  }

  write(1,"\n\n\n\n\n", 5);

  free(shadow);

  free(shadow2);

  return 30;
}
