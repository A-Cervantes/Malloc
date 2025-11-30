#include <stdlib.h>//for malloc
#include <stddef.h>//for NULL

int main (void) 
{
  char *shadow = malloc(100);
  if (shadow == NULL)
  {
    return -10;
  }
  return 20;
}
