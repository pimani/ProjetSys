// Interface pour la spécification d'une pile synchrone polymorphe

#ifndef FILE_H
#define FILE_H

#include <stdbool.h>
#include <stddef.h>
#include <fcntl.h>
#include <sys/stat.h>


 //Structure permettant de définire une file avec des éléments quelconques.
 //La structure ne stock pas la file, elle permet d'y accéder via les fonctions
 //proposer par la librairie.
typedef struct info info;

// Créer une structure correspondant à une file vide avec des éléments de taille
// fixe de taille size.
// Renvois NULL en cas de dépassement de capacité mémoire ou si size <= 0
// sinon renvois l'addresse d'un descripteur permétant d'y accéder.
extern const struct info *file_vide(const char *name, int oflag, mode_t mode, size_t size);

// Ouvre un espace mémoire éxistant et renvois une structure affin d'y accéder
// avec les autres fonctions de la librairie.
// Renvois NULL en cas d'érreur.
extern const struct info *file_ouvre(const char *name, int oflag, mode_t mod);

// Ajoute l'éléments pointer par ptr à la fin de la file décrit pas f.
// Si la file est pleine, tante de doubler la taille de la liste, en cas d'échec
// devient bloquant en attendant que la file se vide.
// Renvois NULL si ptr == null ou en cas de dépassement de capacité mémoire,
// sinon renvois ptr.
extern const void *file_ajout(struct info *f, const void *ptr);

// Défile la file s, renvois nulle si la file et vide sinon renvois l'address
// d'une copie de l'élément qui était au début de la file, la libération de la
// mémoire lier à cette éléments est laisser à la discrétion de l'utisateur.
extern const void *file_retirer(info *f);

// Renvois vrais ou faux si la file s est vide.
extern bool file_est_vide(const info *f);

// libérer l'espace de mémoire partager et détruit les sémaphore, renvois -1 en cas
// d'érreur
extern int file_vider(info *f);
