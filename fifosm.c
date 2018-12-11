#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


#define SHM_ACTIVE "shm_active648646"
#define SHM_NB "shm_nb674272985365427"
#define TAILLE_SHM sizeof(sem_t)
#define DEFAULT_NUMBER 10

struct info {
  const char *name;
  size_t elementSize;
  int shared;
};

struct file {
  size_t sharedSize;
  size_t elementSize;
  size_t count;
  size_t first;
  sem_t *active;
  sem_t *number;
  char array[];
};

//sem_t *create_shm(char *name, int oflag, mode_t mode) {
  //int f;
  //if ((f = shm_open(name, oflag, mode)) == -1) {
    //perror("Ne peux pas ouvrir de la mémoire partager");
    //return NULL;
  //}

  //if (shm_unlink(name) == -1) {
    //perror("shm_unlink");
    //return NULL;
  //}

  //if (ftruncate(f, TAILLE_SHM) == -1) {
    //perror("Ne peux pas obtenier la taille voulus");
    //return NULL;
  //}

  //sem_t *p = mmap(NULL, TAILLE_SHM, PROT_READ | PROT_WRITE, MAP_SHARED, f, 0);
  //if (p == MAP_FAILED) {
    //perror("mmap");
    //return NULL;
  //}
  //if (sem_init(p, 1, 0) == -1) {
    //perror("sem_init");
    //return NULL;
  //}
  //return p;
//}

struct info *file_vide(const char *name, int oflag, mode_t mode, size_t size) {
  if (size <= 0) {
    return NULL;
  }

  int f;
  struct info *descriptor = malloc(sizeof *descriptor);
  if (descriptor == NULL) {
    return NULL;
  }

  if ((f = shm_open(name, oflag, mode)) == -1) {
    perror("Ne peux pas ouvrir de la mémoire partager");
    return NULL;
  }
  if (shm_unlink(name) == -1) {
    perror("shm_unlink");
    return NULL;
  }

  descriptor -> elementSize = size;
  descriptor -> name = name;
  descriptor -> shared = f;

  if (ftruncate(f, sizeof(struct file) + DEFAULT_NUMBER * sizeof(char)) == -1) {
    perror("Ne peux pas obtenier la taille voulus");
    return NULL;
  }
  struct file *fi = mmap(NULL, TAILLE_SHM, PROT_READ | PROT_WRITE, MAP_SHARED, f, 0);
  if (fi == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }
  fi -> sharedSize = sizeof(struct file) + DEFAULT_NUMBER * sizeof(char);
  fi -> elementSize = size;
  fi -> count = 0;
  fi -> first = 0;
  if (sem_init(fi -> active, 1, 1) == -1) {
    perror("sem_init");
    return NULL;
  }
  if (sem_init(fi -> number, 1, 0) == -1) {
    perror("sem_init");
    return NULL;
  }
  return descriptor;
}

const void *file_ajout(const struct info *f, const void *ptr) {
  if (ptr == NULL) {
    return NULL;
  }
  return ptr;
  if ((f -> count)) {

  }
}
