#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>



#define SHM_ACTIVE "shm_active648646"
#define SHM_NB "shm_nb674272985365427"
#define TAILLE_SHM sizeof(sem_t)
#define DEFAULT_NUMBER 10

struct info {
  const char *name;
  int shared;
};

struct file {
  size_t sharedNumber;
  size_t elementSize;
  size_t count;
  size_t head;
  size_t tail;
  sem_t mutex;
  sem_t full;
  sem_t free;
  char array[];
};

int expend_shm(int f);

struct info *file_vide(const char *name, int oflag, mode_t mode, size_t size) {
  if (size <= 0) {
    return NULL;
  }

  int f;
  struct info *descriptor = malloc(sizeof *descriptor);
  descriptor -> name = name;
  if (descriptor == NULL) {
    return NULL;
  }

  if ((f = shm_open(name, oflag, mode)) == -1) {
    perror("Ne peux pas ouvrir de la mémoire partager\n");
    return NULL;
  }
  descriptor -> shared = f;

  if (ftruncate(f, (off_t)(sizeof(struct file) + DEFAULT_NUMBER * size)) == -1) {
    perror("Ne peux pas obtenier la taille voulus\n");
    return NULL;
  }
  char *temp = mmap(NULL, TAILLE_SHM, PROT_WRITE, MAP_SHARED, f, 0);
  if (temp == MAP_FAILED) {
    perror("mmap\n");
    exit(EXIT_FAILURE);
  }

  struct file *fi = (struct file *) temp;

  fi -> sharedNumber = DEFAULT_NUMBER;
  fi -> elementSize = size;
  fi -> count = 0;
  fi -> head = 0;
  fi -> tail = 0;
  if (sem_init(&fi -> mutex, 1, 1) == -1) {
    perror("sem_init\n");
    return NULL;
  }
  if (sem_init(&fi -> full, 1, 0) == -1) {
    perror("sem_init\n");
    return NULL;
  }
  if (sem_init(&fi -> free, 1, DEFAULT_NUMBER) == -1) {
    perror("sem_init\n");
    return NULL;
  }
  return descriptor;
}

const struct info *file_ouvre(const char *name) {
  int f;
  struct info *descriptor = malloc(sizeof *descriptor);
  if (descriptor == NULL) {
    return NULL;
  }

  if ((f = shm_open(name, O_RDWR, S_IRWXU)) == -1) {
    perror("Ne peux pas ouvrir de la mémoire partager\n");
    return NULL;
  }

  struct file *fi = mmap(NULL, TAILLE_SHM, PROT_WRITE, MAP_SHARED, f, 0);
  if (fi == MAP_FAILED) {
    perror("mmap\n");
    exit(EXIT_FAILURE);
  }

  descriptor -> name = name;
  descriptor -> shared = f;
  return descriptor;
}

const void *file_ajout(const struct info *f, const void *ptr) {
  if (ptr == NULL) {
    return NULL;
  }
  char *temp = (char *)ptr;
  perror("tante d'ajouter\n");
  struct file *fi = mmap(NULL, TAILLE_SHM, PROT_WRITE, MAP_SHARED,
      f -> shared, 0);
  if (fi == MAP_FAILED) {
    perror("mmap\n");
    exit(EXIT_FAILURE);
  }
  if (sem_wait(&fi->mutex) == -1) {
    perror("sem_wait\n");
    exit(EXIT_FAILURE);
  }
  perror("passer le mutex\n");
  int test = 0;
  if (fi -> count == fi -> sharedNumber) {
    test = expend_shm(f -> shared);
  }
  if (test == -1) {
    if (sem_post(&fi->mutex) == -1) {
      perror("sem_post\n");
      exit(EXIT_FAILURE);
    }
    if (sem_wait(&fi->free) == -1) {
      perror("sem_wait\n");
      exit(EXIT_FAILURE);
    }
    if (sem_wait(&fi->mutex) == -1) {
      perror("sem_wait\n");
      exit(EXIT_FAILURE);
    }
  }
  memcpy(&(fi -> array[fi -> tail]), temp, fi -> elementSize);
  fi -> head = (fi -> head + fi -> elementSize) % (fi -> sharedNumber * fi -> elementSize);
  fi -> count += 1;

  if (sem_post(&fi->mutex) == -1) {
    perror("sem_post\n");
    exit(EXIT_FAILURE);
  }
  if (sem_post(&fi->full) == -1) {
    perror("sem_post\n");
    exit(EXIT_FAILURE);
  }
  return ptr;
}

const void *file_retirer(struct info *f) {
  struct file *fi = mmap(NULL, TAILLE_SHM, PROT_READ | PROT_WRITE, MAP_SHARED,
      f -> shared, 0);
  if (fi == MAP_FAILED) {
    perror("mmap\n");
    exit(EXIT_FAILURE);
  }
  if (sem_wait(&fi -> full) == -1) {
    perror("sem_wait\n");
    exit(EXIT_FAILURE);
  }
  if (sem_wait(&fi -> mutex) == -1) {
    perror("sem_wait\n");
    exit(EXIT_FAILURE);
  }
  char *value = malloc(fi -> elementSize);
  if (value == NULL) {
    return NULL;
  }

  memcpy(value, &(fi -> array[fi -> tail]), fi -> elementSize);
  fi -> tail = (fi -> tail + fi -> elementSize) % (fi -> sharedNumber * fi -> elementSize);
  fi -> count -= 1;
  if (sem_post(&fi -> mutex) == -1) {
    perror("sem_post\n");
    exit(EXIT_FAILURE);
  }
  if (sem_post(&fi -> free) == -1) {
    perror("sem_post\n");
    exit(EXIT_FAILURE);
  }

  return value;
}

bool file_est_vide(const struct info *f) {
  struct file *fi = mmap(NULL, TAILLE_SHM, PROT_READ | PROT_WRITE, MAP_SHARED,
      f -> shared, 0);
  if (fi == MAP_FAILED) {
    perror("mmap\n");
    exit(EXIT_FAILURE);
  }
  return fi -> count == 0;
}

int file_vider(struct info *f) {
  struct file *fi = mmap(NULL, TAILLE_SHM, PROT_READ | PROT_WRITE, MAP_SHARED,
      f -> shared, 0);
  if (fi == MAP_FAILED) {
    perror("mmap\n");
    return -1;
  }
  if (shm_unlink(f -> name) == -1) {
    perror("shm_unlink\n");
    return -1;
  }
  if (sem_destroy(&fi -> mutex) == -1) {
    perror("sem_destroy\n");
    return -1;
  }
  if (sem_destroy(&fi -> full) == -1) {
    perror("sem_destroy\n");
    return -1;
  }
  if (sem_destroy(&fi -> free) == -1) {
    perror("sem_destroy\n");
    return -1;
  }
  if (close(f -> shared) == -1) {
    perror("server close\n");
    return -1;
  }
  return 0;
}

// Renvois f en cas de succés sinon renvois -1
int expend_shm(int f) {
  struct file *fi = mmap(NULL, TAILLE_SHM, PROT_READ | PROT_WRITE, MAP_SHARED,
      f, 0);
  if (fi == MAP_FAILED) {
    perror("mmap\n");
    exit(EXIT_FAILURE);
  }
  size_t newsize = fi -> elementSize * (fi -> sharedNumber * 2);
  if (ftruncate(f, (off_t)(sizeof(struct file) + newsize)) == -1) {
    perror("Ne peux pas obtenier la taille voulus\n");
    return -1;
  }
  for (size_t i = 0; i < fi -> sharedNumber; i += 1) {
    sem_post(&fi -> free);
  }
  fi -> sharedNumber *= 2;
  return f;
}
