#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <pthread.h>
#include <string.h>

#include "fifosm.h"
#include "lanceur.h"

struct argsc {
  char* argv[3];
  char* envp[1];
};

/**
 * Un nom de segment de mémoire partagé avec un identifiant pour nous assurer de son unicité.
 */
#define NOM_FILE "/ma_file_a_moi_786432985365428"

// Taille de notre tampon
#define N 10

fifo *fifo_p = NULL;

/**
 * On met dans notre shm :
 * - une ent-ête contenant toutes les variables permettant la manipulation du tampon ;
 * - un tampon de N caractères.
 */
#define TAILLE_SHM (sizeof(fifo) + N)

void * run(void * arg) {
  if (arg == NULL) {
    fprintf(stderr, "Unexpected argument value\n");
    exit(EXIT_FAILURE);
  }
  char* argv[] = {"/bin/ps", "-j", NULL};
  char* envp[] = {NULL};
  
  switch (fork()) {
  case -1:
    perror("fork");
    exit(EXIT_FAILURE);
  case 0:
    exit(EXIT_SUCCESS);
  default:
    // On exécute la commande
    execve(argv[0], argv, envp);
    perror("execve");
  }
  
  return NULL;
}

int main(void) {
  // On commence par créer un segment de mémoire partagée
  int shm_fd = shm_open(NOM_SHM, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
  if (shm_fd == -1) {
    perror("shm_open");
    exit(EXIT_SUCCESS);
  }

  if (shm_unlink(NOM_SHM) == -1) {
    perror("shm_unlink");
    exit(EXIT_FAILURE);
  }

  if (ftruncate(shm_fd, TAILLE_SHM) == -1) {
    perror("ftruncate");
    exit(EXIT_FAILURE);
  }

  char *shm_ptr = mmap(NULL, TAILLE_SHM, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (shm_ptr == MAP_FAILED) {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  fifo_p = (struct fifo *) shm_ptr;

  // Initialisation des variables
  if (sem_init(&fifo_p->mutex, 1, 1) == -1) {
    perror("sem_init");
    exit(EXIT_FAILURE);
  }

  if (sem_init(&fifo_p->vide, 1, N) == -1) {
    perror("sem_init");
    exit(EXIT_FAILURE);
  }

  if (sem_init(&fifo_p->plein, 1, 0) == -1) {
    perror("sem_init");
    exit(EXIT_FAILURE);
  }

  fifo_p->tete = 0;
  fifo_p->queue = 0;
  
  argsc comm;
  do {
    argsc = defiler(*la_file_synchro);
    pthread_t th;
    if (pthread_create(&th, NULL, run, *argsc) != 0) {
        fprintf(stderr, "Erreur\n");
        exit(EXIT_FAILURE);
    }
    pthread_exit(NULL);

  } while (c md != NULL);

  
  
  
  // Destruction des sémaphores
  if (sem_destroy(&fifo_p->mutex) == -1) {
    perror("sem_destroy");
    exit(EXIT_FAILURE);
  }
  if (sem_destroy(&fifo_p->plein) == -1) {
    perror("sem_destroy");
    exit(EXIT_FAILURE);
  }
  if (sem_destroy(&fifo_p->vide) == -1) {
    perror("sem_destroy");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}
