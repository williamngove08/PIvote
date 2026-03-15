/**
 * @file PIVOTE_CLIENT_V2.c
 * @brief Point d'entree du CLIENT PIVOTE V2 (votant).
 *
 * Flux d'execution :
 *   1. initialiserSocket        -> init Winsock + creation socket TCP
 *   2. connecterAuServeur       -> saisie IP + connect()
 *   3. authentifier             -> boucle login/mdp (3 tentatives max)
 *   4. recevoirListeCandidats   -> affichage liste envoyee par le serveur
 *   5. saisirVote               -> saisie ID electeur + ID candidat + confirmation
 *   6. envoyerVote              -> envoi "VOTE <idE> <idC>"
 *   7. recevoirConfirmationVote -> si OK : lance le Snake en attendant les resultats
 *                                  si ERREUR : affiche le motif de refus
 *   8. fermerConnexion          -> closesocket + WSACleanup
 *
 * Compilation (MinGW / Code::Blocks, C99) :
 * gcc -std=c99 -Wall FONCTIONS_PIVOTE_CLIENT_V2.c PIVOTE_CLIENT_V2.c snake.c -o client.exe -lws2_32
 *
 * Fichiers necessaires dans le projet :
 *   PIVOTE_CLIENT_V2.c           (ce fichier)
 *   FONCTIONS_PIVOTE_CLIENT_V2.c
 *   snake.c  +  snake.h
 *   client.h
 */

#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <winsock2.h>
#include <windows.h>

int main(void)
{
    /* Support des accents sous Windows (code page 1252 = Windows Latin-1) */
    SetConsoleOutputCP(1252);
    SetConsoleCP(1252);

    SOCKET sock;
    char   server_ip[50];
    char   username[65];
    char   password[65];
    int    idE, idC;

    printf("===================================================\n");
    printf("          PIVOTE - MODULE VOTANT V2\n");
    printf("===================================================\n\n");

    /* --------------------------------------------------
     * Etape 1 : Initialisation Winsock + creation socket
     * -------------------------------------------------- */
    if (!initialiserSocket(&sock)) {
        system("pause");
        return 1;
    }

    /* --------------------------------------------------
     * Etape 2 : Connexion au serveur
     * -------------------------------------------------- */
    if (!connecterAuServeur(sock, server_ip)) {
        fermerConnexion(sock);
        system("pause");
        return 1;
    }

    /* --------------------------------------------------
     * Etape 3 : Authentification (3 tentatives max)
     * -------------------------------------------------- */
    if (!authentifier(sock, username, password)) {
        fermerConnexion(sock);
        system("pause");
        return 1;
    }

    /* --------------------------------------------------
     * Etape 4 : Reception et affichage liste des candidats
     * -------------------------------------------------- */
    recevoirListeCandidats(sock);

    /* --------------------------------------------------
     * Etape 5 : Saisie et confirmation du vote
     * -------------------------------------------------- */
    saisirVote(&idE, &idC);

    /* --------------------------------------------------
     * Etape 6 : Envoi du vote au serveur
     * -------------------------------------------------- */
    envoyerVote(sock, idE, idC);

    /* --------------------------------------------------
     * Etape 7 : Confirmation + Snake si vote accepte
     * Le Snake est lance depuis recevoirConfirmationVote()
     * directement apres l'affichage du message de succes.
     * -------------------------------------------------- */
    recevoirConfirmationVote(sock);

    /* --------------------------------------------------
     * Etape 8 : Fermeture propre de la connexion
     * -------------------------------------------------- */
    fermerConnexion(sock);

    printf("\nAppuyez sur Entr\xe9e pour quitter...");
    getchar();
    return 0;
}
