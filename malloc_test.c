#include <stdlib.h>//for malloc
#include <stddef.h>//for NULL
#include <stdio.h>//for printf
#include <unistd.h>//for write
#include <string.h> //for strlen

int main(void)
{
  char *pointer1 = malloc(42);

  if (pointer1 == NULL)
  {
    char *mes = "Pointer 1 failed and returned NULL\n";
    write(1, mes, strlen(mes));

    return -1;
  }

  char *pointer2 = malloc(168);

  if (pointer2 == NULL)
  {
    char *mes2 = "Pointer 2 failed returned NULL\n";
    write(1, mes2, strlen(mes2));

    return -1;
  }

  for (int i = 0; i < 24; i++)
  {
    pointer1[i] = 'a';
    pointer2[i] = 'b';
    printf("I am in a loop!\n");
  }

  free(pointer1);
  free(pointer2);

  return 88;
}
