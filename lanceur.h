// Interface pour le lanceur

#ifndef LANCEUR_H
#define LANCEUR_H

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NOM_FILE "/shm_file_synchro10101000101"
#define MAX_NOM_SIZE 32
#define TUBE_LENGHT 32
#define ARG_LENGHT 64
#define ARG_NUMBER 5
#define ENVIRONEMENT_LENGHT 20

// Structure de transmission de demande, exemple :
// char* argv[] = {"/bin/ps", "-j", NULL};
// char* envp[] = {NULL};
// char* tube_in = "mon tube pour savoir se qu'il se passe";
// char* tube_out = "mon tube pour envoyer mes info";
typedef struct argsc {
  char argv[ARG_NUMBER][ARG_LENGHT];
  char envp[ENVIRONEMENT_LENGHT];
  char tube_in[TUBE_LENGHT];
  char tube_out[TUBE_LENGHT];
} argsc;

// Structure permettant de gérer toutes les variables d'arguments et
// d'environnements
typedef struct argsc argsc;

//Exécute le thread avec les arguments passés dans *arg
extern void * run(argsc * arg);

// Remplace le comportement par défaut pour les signaux d'intéruptions
extern void handlerStop(int signum);

// Gérer les signaux de fin de vie des enfants pour gérer les procéssus zombies
extern void handlerFils(int signum);

#endif
