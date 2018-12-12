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

#include "infosm.h"
#include "lanceur.h"

// Structure de transmission de demande
struct argsc {
  char* argv[3];
  char* envp[1];
};

#define NOM_FILE "/ma_file_a_moi_786432985365428"
#define N 10

info *info_p = NULL;

#define TAILLE_SHM (sizeof(info) + N)

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
    // fermer le tube en cas d'erreur
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

  info_p = (struct info*) shm_ptr;
  info_p->tete = 0;
  info_p->queue = 0;
  
  argsc comm;
  do {
    // bloquant a lexe pas besoin de while et doc
    while (file_est_vide((const) info_p));
    argsc = file_retirer(info_p);
    pthread_t th;
    if (pthread_create(&th, NULL, run, *argsc) != 0) {
        fprintf(stderr, "Erreur\n");
        exit(EXIT_FAILURE);
    }
    pthread_exit(NULL);

  } while (1);
  
  return EXIT_SUCCESS;
}
