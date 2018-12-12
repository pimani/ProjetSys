#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>

#include "fifosm.h"
#include "lanceur.h"
#include "client.h"

int main(void) {
  char wtubename[MAX_NOM_SIZE];
  strcpy(wtubename, NOM_FIFOW);
  sprintf(strpid + strlen(wtubename), "%d", getpid());
  char rtubename[MAX_NOM_SIZE];
  strcpy(rtubename, NOM_FIFOR);
  sprintf(strpid + strlen(rtubename), "%d", getpid());
  
  if (mkfifo(wtubename, S_IRUSR | S_IWUSR) == -1) {
    perror("mkfifo wtube");
    exit(EXIT_FAILURE);
  }
  if (mkfifo(rtubename, S_IRUSR | S_IWUSR) == -1) {
    perror("mkfifo rtube");
    exit(EXIT_FAILURE);
  }
  
  fdw = open(wtubename, O_WRONLY);
  if (fdw == -1) {
    perror("open wtube");
    exit(EXIT_FAILURE);
  }
  dup2(0, fdw);
  
  fdr = open(rtubename, O_RDONLY);
  if (fdr == -1) {
    perror("open rtube");
    exit(EXIT_FAILURE);
  }
  dup2(1, fdr);
  
  argsc cmd;
  for (int i = 0; i < 3; ++i) {
    if (fgets(cmd->argv[i], sizeof(cmd->argv[i]), stdin) == NULL) {
      perror("fgets");
      exit(EXIT_FAILURE);
    }
  }
  if (fgets(cmd->envp, sizeof(cmd->envp), stdin) == NULL) {
    perror("fgets");
    exit(EXIT_FAILURE);
  }
  
  int shm_fd = shm_open(NOM_SHM, O_RDWR, S_IRUSR | S_IWUSR);
  if (shm_fd == -1) {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }
  if (shm_unlink(NOM_SHM) == -1) {
    perror("shm_unlink");
    exit(EXIT_FAILURE);
  }
  // TAILLE_STRUCT = sizeof(struct d'un élément)
  info *info_p = mmap(NULL, TAILLE_STRUCT, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (info_p == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }
  return EXIT_SUCCESS;
}
