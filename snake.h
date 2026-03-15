/**
 * @file snake.h
 * @brief Mini-jeu Snake en console Windows pour PIVOTE V2.
 *        Lance automatiquement apres le vote, pendant l'attente des resultats.
 *
 * Compilation : ajouter snake.c au projet client.
 * gcc -std=c99 -Wall FONCTIONS_PIVOTE_CLIENT_V2.c PIVOTE_CLIENT_V2.c snake.c -o client.exe -lws2_32
 */

#ifndef SNAKE_H
#define SNAKE_H

/* =========================================================
 * CONSTANTES DU JEU
 * ========================================================= */
#define SNAKE_LARGEUR   32    /* Largeur de la grille (cases) */
#define SNAKE_HAUTEUR   18    /* Hauteur de la grille (cases) */
#define SNAKE_MAX      400    /* Longueur maximale du serpent */
#define SNAKE_VITESSE  130    /* Delai entre chaque frame (ms) */

/* =========================================================
 * TYPES
 * ========================================================= */
typedef struct {
    int x;
    int y;
} SnakePoint;

typedef enum {
    DIR_HAUT   = 0,
    DIR_BAS    = 1,
    DIR_GAUCHE = 2,
    DIR_DROITE = 3
} SnakeDir;

/* =========================================================
 * FONCTION PRINCIPALE
 * ========================================================= */
/**
 * @brief Lance le mini-jeu Snake.
 *        Appelee depuis FONCTIONS_PIVOTE_CLIENT_V2.c apres confirmation du vote.
 *        Le joueur peut rejouer ou quitter avec Q / Echap.
 */
void lancerSnake(void);

#endif /* SNAKE_H */
