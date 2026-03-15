/**
 * @file snake.c
 * @brief Mini-jeu Snake en console Windows — PIVOTE V2.
 *
 * Controles :
 *   Fleches directionnelles  -> deplacer le serpent
 *   Q ou Echap               -> quitter le jeu
 *
 * Pas de bibliotheque supplementaire, uniquement :
 *   windows.h, conio.h, stdio.h, stdlib.h, time.h
 */

#include "snake.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

/* =========================================================
 * HELPERS CONSOLE — positionnement et couleurs
 * ========================================================= */

static void snake_cacherCurseur(void)
{
    CONSOLE_CURSOR_INFO ci = { 1, FALSE };
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);
}

static void snake_montrerCurseur(void)
{
    CONSOLE_CURSOR_INFO ci = { 25, TRUE };
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);
}

static void snake_goto(int x, int y)
{
    COORD c = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

static void snake_couleur(int c)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

/* Vide le buffer clavier avant d'attendre une touche */
static void snake_viderBuffer(void)
{
    while (_kbhit()) _getch();
}

/* =========================================================
 * ETAT DU JEU — variables statiques
 * ========================================================= */
static SnakePoint corps[SNAKE_MAX];  /* Tableau du corps du serpent */
static int        longueur;          /* Longueur courante           */
static SnakeDir   direction;         /* Direction courante          */
static SnakePoint pomme;             /* Position de la pomme        */
static int        score;             /* Score du joueur             */
static int        gameOver;          /* 1 = partie terminee         */
static int        quitter;           /* 1 = le joueur veut sortir   */

/* =========================================================
 * PLACEMENT DE LA POMME
 * ========================================================= */
static void placerPomme(void)
{
    int libre;
    int i;
    do {
        libre = 1;
        pomme.x = 1 + rand() % (SNAKE_LARGEUR - 2);
        pomme.y = 1 + rand() % (SNAKE_HAUTEUR - 2);
        for (i = 0; i < longueur; i++) {
            if (corps[i].x == pomme.x && corps[i].y == pomme.y) {
                libre = 0;
                break;
            }
        }
    } while (!libre);
}

/* =========================================================
 * INITIALISATION D'UNE PARTIE
 * ========================================================= */
static void initJeu(void)
{
    int i;
    longueur  = 4;
    direction = DIR_DROITE;
    score     = 0;
    gameOver  = 0;
    quitter   = 0;

    /* Serpent demarre au centre, horizontal */
    for (i = 0; i < longueur; i++) {
        corps[i].x = SNAKE_LARGEUR / 2 - i;
        corps[i].y = SNAKE_HAUTEUR / 2;
    }

    placerPomme();
}

/* =========================================================
 * DESSIN — BORDURE
 * ========================================================= */
static void dessinerBordure(void)
{
    int x, y;
    snake_couleur(11); /* Cyan clair */

    /* Coin haut-gauche */
    snake_goto(0, 0);
    printf("\xda");
    for (x = 1; x < SNAKE_LARGEUR - 1; x++) printf("\xc4");
    printf("\xbf");

    /* Cotes */
    for (y = 1; y < SNAKE_HAUTEUR - 1; y++) {
        snake_goto(0, y);
        printf("\xb3");
        snake_goto(SNAKE_LARGEUR - 1, y);
        printf("\xb3");
    }

    /* Coin bas-gauche */
    snake_goto(0, SNAKE_HAUTEUR - 1);
    printf("\xc0");
    for (x = 1; x < SNAKE_LARGEUR - 1; x++) printf("\xc4");
    printf("\xd9");

    snake_couleur(7);
}

/* =========================================================
 * DESSIN — PANNEAU D'INFORMATION (a droite de la grille)
 * ========================================================= */
static void dessinerPanneau(void)
{
    int px = SNAKE_LARGEUR + 2;

    snake_goto(px, 0);
    snake_couleur(14); /* Jaune */
    printf("== SNAKE PIVOTE ==");

    snake_goto(px, 2);
    snake_couleur(7);
    printf("Score  : ");
    snake_couleur(10); /* Vert */
    printf("%-5d", score);

    snake_goto(px, 3);
    snake_couleur(7);
    printf("Taille : ");
    snake_couleur(11); /* Cyan */
    printf("%-5d", longueur);

    snake_goto(px, 5);
    snake_couleur(8); /* Gris */
    printf("\x18\x19 : bouger");
    snake_goto(px, 6);
    printf("Q / Esc : quitter");

    snake_goto(px, 8);
    snake_couleur(12); /* Rouge */
    printf("Vote envoye !");
    snake_goto(px, 9);
    snake_couleur(7);
    printf("En attente des");
    snake_goto(px, 10);
    printf("resultats...");

    snake_goto(px, 12);
    snake_couleur(10);
    printf("\x04 = +10 pts");  /* losange = pomme */

    snake_couleur(7);
}

/* =========================================================
 * DESSIN — SERPENT ET POMME (dessin incremental)
 * ========================================================= */
static void dessinerTete(void)
{
    snake_goto(corps[0].x, corps[0].y);
    snake_couleur(10); /* Vert vif */
    printf("\x02");    /* Smiley */
}

static void dessinerSegment(int i)
{
    snake_goto(corps[i].x, corps[i].y);
    snake_couleur(2); /* Vert sombre */
    printf("\xfe");   /* Petit carre */
}

static void dessinerPomme(void)
{
    snake_goto(pomme.x, pomme.y);
    snake_couleur(12); /* Rouge */
    printf("\x04");    /* Losange */
    snake_couleur(7);
}

static void effacerCase(int x, int y)
{
    snake_goto(x, y);
    printf(" ");
}

/* Dessin complet initial (une seule fois au debut de la partie) */
static void dessinerTout(void)
{
    int i;
    dessinerBordure();
    dessinerPanneau();
    dessinerPomme();
    dessinerTete();
    for (i = 1; i < longueur; i++) dessinerSegment(i);
    snake_couleur(7);
}

/* =========================================================
 * LECTURE CLAVIER (non bloquant)
 * ========================================================= */
static void lireTouche(void)
{
    int t;
    if (!_kbhit()) return;

    t = _getch();

    if (t == 0 || t == 224) {        /* Touche speciale : fleche */
        t = _getch();
        switch (t) {
            case 72: if (direction != DIR_BAS)    direction = DIR_HAUT;   break;
            case 80: if (direction != DIR_HAUT)   direction = DIR_BAS;    break;
            case 75: if (direction != DIR_DROITE) direction = DIR_GAUCHE; break;
            case 77: if (direction != DIR_GAUCHE) direction = DIR_DROITE; break;
        }
    } else if (t == 'q' || t == 'Q' || t == 27) {
        quitter  = 1;
        gameOver = 1;
    }
}

/* =========================================================
 * DEPLACEMENT DU SERPENT
 * ========================================================= */
static void deplacerSerpent(void)
{
    int i;
    /* Sauvegarde la position de la queue avant de decaler */
    int qx = corps[longueur - 1].x;
    int qy = corps[longueur - 1].y;

    /* Decale tout le corps d'un cran vers l'arriere */
    for (i = longueur - 1; i > 0; i--)
        corps[i] = corps[i - 1];

    /* Avance la tete dans la direction courante */
    switch (direction) {
        case DIR_HAUT:   corps[0].y--; break;
        case DIR_BAS:    corps[0].y++; break;
        case DIR_GAUCHE: corps[0].x--; break;
        case DIR_DROITE: corps[0].x++; break;
    }

    /* --- Collision mur --- */
    if (corps[0].x <= 0 || corps[0].x >= SNAKE_LARGEUR - 1 ||
        corps[0].y <= 0 || corps[0].y >= SNAKE_HAUTEUR - 1)
    {
        gameOver = 1;
        return;
    }

    /* --- Collision avec soi-meme --- */
    for (i = 1; i < longueur; i++) {
        if (corps[0].x == corps[i].x && corps[0].y == corps[i].y) {
            gameOver = 1;
            return;
        }
    }

    /* --- Mange la pomme ? --- */
    if (corps[0].x == pomme.x && corps[0].y == pomme.y) {
        score += 10;
        if (longueur < SNAKE_MAX) {
            corps[longueur].x = qx;
            corps[longueur].y = qy;
            longueur++;
        }
        placerPomme();
        dessinerPomme();     /* Dessine la nouvelle pomme */
        dessinerPanneau();   /* Met a jour le score       */
    } else {
        /* Efface la case qui etait la queue */
        effacerCase(qx, qy);
    }

    /* Dessine la nouvelle tete et le 2e segment (qui etait tete avant) */
    dessinerSegment(1);
    dessinerTete();
}

/* =========================================================
 * ECRAN GAME OVER
 * ========================================================= */
static void afficherGameOver(void)
{
    /* Boite centree sur la grille */
    int cx = SNAKE_LARGEUR / 2 - 9;
    int cy = SNAKE_HAUTEUR / 2 - 2;

    snake_couleur(12);
    snake_goto(cx, cy);     printf("\xda\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xbf");
    snake_goto(cx, cy + 1); printf("\xb3                 \xb3");
    snake_goto(cx, cy + 2); printf("\xb3                 \xb3");
    snake_goto(cx, cy + 3); printf("\xb3                 \xb3");
    snake_goto(cx, cy + 4); printf("\xc0\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xd9");

    snake_goto(cx + 4, cy + 1);
    snake_couleur(14);
    printf("GAME OVER !");

    snake_goto(cx + 2, cy + 2);
    snake_couleur(7);
    printf("Score : ");
    snake_couleur(10);
    printf("%d", score);

    snake_goto(cx + 1, cy + 3);
    snake_couleur(8);
    printf("Une touche pour cont.");

    snake_couleur(7);
    snake_viderBuffer();
    _getch();
}

/* =========================================================
 * ECRAN D'ACCUEIL (avant la premiere partie)
 * ========================================================= */
static void afficherAccueil(void)
{
    system("cls");

    snake_couleur(11);
    printf("\n\n");
    printf("  +==========================================+\n");
    printf("  |                                          |\n");
    snake_couleur(14);
    printf("  |         MINI-JEU SNAKE  -  PIVOTE        |\n");
    snake_couleur(11);
    printf("  |                                          |\n");
    printf("  +==========================================+\n\n");

    snake_couleur(7);
    printf("  Ton vote a ete envoye avec succes !\n");
    printf("  Les resultats seront disponibles quand\n");
    printf("  l'administrateur fermera le scrutin.\n\n");

    snake_couleur(10);
    printf("  En attendant... joue au Snake !\n\n");

    snake_couleur(8);
    printf("  Fleches directionnelles : deplacer\n");
    printf("  Q / Echap               : quitter\n\n");

    snake_couleur(14);
    printf("  Appuyez sur une touche pour commencer...\n");
    snake_couleur(7);

    snake_viderBuffer();
    _getch();
}

/* =========================================================
 * BOUCLE DE JEU (une partie)
 * ========================================================= */
static void uneParte(void)
{
    system("cls");
    snake_cacherCurseur();
    initJeu();
    dessinerTout();

    while (!gameOver) {
        lireTouche();
        if (!gameOver) {
            deplacerSerpent();
        }
        Sleep(SNAKE_VITESSE);
    }

    if (!quitter) {
        afficherGameOver();
    }
}

/* =========================================================
 * POINT D'ENTREE PRINCIPAL
 * ========================================================= */
void lancerSnake(void)
{
    char rep;

    srand((unsigned int)time(NULL));

    afficherAccueil();

    do {
        uneParte();

        if (quitter) break;

        /* Propose de rejouer */
        system("cls");
        snake_montrerCurseur();
        snake_couleur(11);
        printf("\n  +==================================+\n");
        snake_couleur(14);
        printf("  |   Score final : %-5d            |\n", score);
        snake_couleur(11);
        printf("  +==================================+\n\n");
        snake_couleur(7);
        printf("  Rejouer ? (O = oui  /  N = quitter) : ");

        snake_viderBuffer();
        rep = (char)_getch();
        printf("%c\n", rep);

    } while (rep == 'o' || rep == 'O');

    system("cls");
    snake_montrerCurseur();
    snake_couleur(7);
}
