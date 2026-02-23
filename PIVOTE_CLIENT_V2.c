/**
 * @file client_main.c
 * @brief Point d'entree du CLIENT PIVOTE V2 (votant).
 *
 * Flux d'execution :
 *   1. initialiserSocket       -> init Winsock + creation socket TCP
 *   2. connecterAuServeur      -> saisie IP + connect()
 *   3. authentifier            -> boucle login/mdp (3 tentatives max)
 *   4. recevoirListeCandidats  -> affichage liste envoyee par le serveur
 *   5. saisirVote              -> saisie ID electeur + ID candidat + confirmation
 *   6. envoyerVote             -> envoi "VOTE <idE> <idC>"
 *   7. recevoirConfirmationVote-> affichage resultat final
 *   8. fermerConnexion         -> closesocket + WSACleanup
 *
 * Compilation (MinGW / Code::Blocks, C99) :
 * gcc -std=c99 -Wall client_impl.c client_main.c -o client.exe -lws2_32
 */

#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <winsock2.h>
#include <windows.h>
#include "auth.h"

int main(void)
{
    /* Support des accents sous Windows */
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    setlocale(LC_ALL, "");

    SOCKET sock;
    char   server_ip[50];
    char   username[65];
    char   password[65];
    int    idE, idC;

    printf("===================================================\n");
    printf("                     PIVOTE\n");
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
     * Le message "mot de passe oublie" s'affiche uniquement
     * en cas d'echec, pas systematiquement.
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
     * Etape 7 : Reception de la confirmation finale
     * -------------------------------------------------- */
    recevoirConfirmationVote(sock);

    /* --------------------------------------------------
     * Etape 8 : Fermeture propre de la connexion
     * -------------------------------------------------- */
    fermerConnexion(sock);

    printf("\nAppuyez sur Entree pour quitter...");
    getchar();
    return 0;
}
