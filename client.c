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
  char wtubename[MAX_NOM_SIZE];
  strcpy(wtubename, NOM_FIFOW);
  sprintf(wtubename + strlen(wtubename), "%d", getpid());
  char rtubename[MAX_NOM_SIZE];
  strcpy(rtubename, NOM_FIFOR);
  sprintf(rtubename + strlen(rtubename), "%d", getpid());

  if (mkfifo(wtubename, S_IRUSR | S_IWUSR) == -1) {
    perror("mkfifo wtube");
    exit(EXIT_FAILURE);
  }
  if (mkfifo(rtubename, S_IRUSR | S_IWUSR) == -1) {
    perror("mkfifo rtube");
    exit(EXIT_FAILURE);
  }

  int fdw = open(wtubename, O_WRONLY);
  if (fdw == -1) {
    perror("open wtube");
    exit(EXIT_FAILURE);
  }
  dup2(0, fdw);

  int fdr = open(rtubename, O_RDONLY);
  if (fdr == -1) {
    perror("open rtube");
    exit(EXIT_FAILURE);
  }
  dup2(1, fdr);

  argsc cmd;
  for (int i = 0; i < 3; ++i) {
    if (fgets(cmd.argv[i], sizeof(cmd.argv[i]), stdin) == NULL) {
      perror("fgets");
      exit(EXIT_FAILURE);
    }
  }
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
  file_ajout((info *) descriptor, &cmd);

  return EXIT_SUCCESS;
}
