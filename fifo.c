#include "fifo.h"

typedef struct node node;

struct file {
  node *first;
  node *last;
  size_t height;
}

struct node {
  void *value;
  node *next;
}

file *file_vide(void) {
  file *f = malloc(sizeof *f);
  if (f == NULL) {
    return NULL;
  }
  f -> first = NULL;
  f -> last = NULL;
  f -> height = 0;
  return s;
}

const void *file_ajout(file *f, const void *ptr) {
  if (ptr == NULL) {
    return NULL;
  }
  if (s ->)) #

  }
}
