#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <linux/limits.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>

#include "fifosm.h"
#include "lanceur.h"
#include "client.h"

int main(void) {
  printf("Démarage du client : \n");
  char wtubename[MAX_NOM_SIZE];
  strcpy(wtubename, NOM_FIFOW);
  sprintf(wtubename + strlen(wtubename), "%d", getpid());
  char rtubename[MAX_NOM_SIZE];
  strcpy(rtubename, NOM_FIFOR);
  sprintf(rtubename + strlen(rtubename), "%d", getpid());
  printf("Creation des tubes :\nCreation tube lecture\n");
  if (mkfifo(wtubename, S_IRUSR | S_IWUSR) == -1) {
    perror("mkfifo wtube");
    exit(EXIT_FAILURE);
  }
  printf("Creation tube écriture\n");
  if (mkfifo(rtubename, S_IRUSR | S_IWUSR) == -1) {
    perror("mkfifo rtube");
    exit(EXIT_FAILURE);
  }

  int fdw = open(wtubename, O_WRONLY);
  if (fdw == -1) {
    perror("open wtube");
    exit(EXIT_FAILURE);
  }

  int fdr = open(rtubename, O_RDONLY);
  if (fdr == -1) {
    perror("open rtube");
    exit(EXIT_FAILURE);
  }

  argsc cmd;
  printf("Donner vos argument, le premier étant la commande :\n");
  for (int i = 0; i < 3; ++i) {
    printf("Argument numéro %d : ", i);
    if (fgets(cmd.argv[i], sizeof(cmd.argv[i]), stdin) == NULL) {
      perror("fgets");
      exit(EXIT_FAILURE);
    }
  }
  printf("Donner vos variables d'environements:");
  if (fgets(cmd.envp, sizeof(cmd.envp), stdin) == NULL) {
    perror("fgets");
    exit(EXIT_FAILURE);
  }
  strcpy(cmd.tube_in, wtubename);
  strcpy(cmd.tube_out, rtubename);

  const info *descriptor = file_ouvre(NOM_FILE, O_RDWR | O_CREAT | O_EXCL,
      S_IRUSR | S_IWUSR);
  if (descriptor == NULL) {
    perror("ouverture file");
    exit(EXIT_FAILURE);
  }
  printf("Envois du méssage et remplacement des entrée et sortie standards\n");
  dup2(0, fdw);
  dup2(1, fdr);
  file_ajout((info *) descriptor, &cmd);

  return EXIT_SUCCESS;
}
