// Interface pour la spécification d'une pile polymorphe

#ifndef FILE_H
#define FILE_H

#include <stdbool.h>
#include <stddef.h>
#include <fcntl.h>
#include <sys/stat.h>


// Structure permettant de définire une file avec des éléments quelconques.
// La structure ne stock pas la file, elle permet d'y accéder via les fonctions
// proposer par la librairie.
typedef struct info info;

// Créer une structure correspondant à une file vide avec des éléments de taille
// fixe de taille size.
// Renvois NULL en cas de dépassement de capacité mémoire ou si size <= 0
// sinon renvois l'addresse d'un descripteur permétant d'y accéder.
extern const struct info *file_vide(const char *name, int oflag, mode_t mode, size_t size);

// ajoute l'éléments pointer par ptr à la fin de la file décrit pas f.
// Renvois NULL si ptr == null ou en cas de dépassement de capacité mémoire,
// sinon renvois ptr;
extern const void *file_ajout(const info *f, const void *ptr);

// Défile la file s, renvois nulle si la file et vide sinon renvois l'address
// de l'élément qui était au début de la file
extern const void *file_retirer(info *f);

// Renvois vrais ou faux si la file s est vide.
extern bool file_est_vide(const info *f);

// Renvois la hauteur de la file S.
extern size_t file_hauter(const info *f);

// SI *sptr n'est pas NULL, libére la mémoire alouer à la pile puis donne à
// *sptr la valeur NULL
extern void file_vider(stack **fptr);
