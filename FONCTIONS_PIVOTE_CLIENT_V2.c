/**
 * @file client_impl.c
 * @brief Implementation de toutes les fonctions du CLIENT PIVOTE V2.
 *
 * Compilation (MinGW / Code::Blocks, C99) :
 * gcc -std=c99 -Wall client_impl.c client_main.c -o client.exe -lws2_32
 */

#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

/* =========================================================
 * 1. HELPERS CONSOLE
 * ========================================================= */
void viderBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void lire_ligne(const char *invite, char *buffer, size_t taille)
{
    printf("%s", invite);
    if (fgets(buffer, (int)taille, stdin)) {
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len-1] == '\n' || buffer[len-1] == '\r'))
            buffer[len-1] = '\0';
    }
}

/* =========================================================
 * 2. CONNEXION RESEAU
 * ========================================================= */
int initialiserSocket(SOCKET *sock)
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("[ERREUR] Initialisation Winsock echouee.\n");
        return 0;
    }
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock == INVALID_SOCKET) {
        printf("[ERREUR] Creation de la socket echouee.\n");
        WSACleanup();
        return 0;
    }
    return 1;
}

int connecterAuServeur(SOCKET sock, char *server_ip)
{
    struct sockaddr_in server_addr;

    printf("Entrez l'adresse IP du serveur (ex: 192.168.1.15) : ");
    scanf("%49s", server_ip);
    viderBuffer();

    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port        = htons(PORT);

    printf("Tentative de connexion a %s...\n", server_ip);
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("\n[ERREUR FATALE] Impossible de joindre le serveur.\n");
        printf("Verifiez :\n");
        printf(" 1. L'adresse IP est correcte.\n");
        printf(" 2. Le serveur a lance l'option 9.\n");
        printf(" 3. Le pare-feu Windows.\n");
        return 0;
    }
    printf("Connexion etablie.\n\n");
    return 1;
}

/* =========================================================
 * 3. AUTHENTIFICATION
 * ========================================================= */
int authentifier(SOCKET sock, char *username, char *password)
{
    char send_buffer[BUFFER];
    char recv_buffer[BUFFER];
    int  tentatives = 3;
    int  len;

    while (tentatives > 0) {
        printf("=== CONNEXION ===\n");
        lire_ligne("Identifiant : ", username, 65);
        lire_ligne("Mot de passe : ", password, 65);

        /* Envoi "AUTH <username> <password>" */
        snprintf(send_buffer, sizeof(send_buffer), "AUTH %s %s", username, password);
        send(sock, send_buffer, strlen(send_buffer), 0);

        /* Lecture reponse */
        len = recv(sock, recv_buffer, BUFFER - 1, 0);
        if (len <= 0) return 0;
        recv_buffer[len] = '\0';

        if (strcmp(recv_buffer, "AUTH_OK") == 0) {
            printf("\nConnexion reussie. Bonjour %s !\n", username);
            return 1;
        }

        tentatives--;
        if (tentatives > 0) {
            printf("\n[ECHEC] Identifiant ou mot de passe incorrect. "
                   "%d tentative(s) restante(s).\n", tentatives);
            printf("[?] Mot de passe oublie ? Contactez l'administrateur du scrutin.\n");
            printf("    Il pourra reinitialiser votre mot de passe depuis le serveur.\n\n");
        } else {
            printf("\n[ECHEC] Trop de tentatives. Acces refuse.\n");
            printf("[?] Contactez l'administrateur du scrutin pour reinitialiser "
                   "votre mot de passe.\n");
        }
    }
    return 0;
}

/* =========================================================
 * 4. VOTE
 * ========================================================= */
void recevoirListeCandidats(SOCKET sock)
{
    char recv_buffer[BUFFER];
    int len = recv(sock, recv_buffer, BUFFER - 1, 0);
    if (len > 0) {
        recv_buffer[len] = '\0';
        printf("%s", recv_buffer);
    }
}

void saisirVote(int *idE, int *idC)
{
    int confirmation;
    int voteValide = 0;

    do {
        printf("\n--- FORMULAIRE DE VOTE ---\n");
        printf("Entrer votre ID electeur s'il vous plait : ");
        scanf("%d", idE);
        viderBuffer();

        printf("ID du candidat choisi : ");
        scanf("%d", idC);
        viderBuffer();

        printf("\nVOUS ALLEZ VOTER :\n");
        printf(" Vous avez pour ID : %d\n", *idE);
        printf(" Le Candidat que vous choisissez a pour ID : %d\n", *idC);
        printf("Confirmez-vous ce choix ? (1=OUI / 0=NON) : ");
        scanf("%d", &confirmation);
        viderBuffer();

        if (confirmation == 1) {
            voteValide = 1;
        } else {
            printf("\n[INFO] Vote annule. Recommencez.\n");
        }
    } while (!voteValide);
}

void envoyerVote(SOCKET sock, int idE, int idC)
{
    char send_buffer[BUFFER];
    /* Format : "VOTE <idElecteur> <idCandidat>" */
    snprintf(send_buffer, sizeof(send_buffer), "VOTE %d %d", idE, idC);
    send(sock, send_buffer, strlen(send_buffer), 0);
}

void recevoirConfirmationVote(SOCKET sock)
{
    char recv_buffer[BUFFER];
    int len = recv(sock, recv_buffer, BUFFER - 1, 0);
    if (len > 0) {
        recv_buffer[len] = '\0';
        if (strcmp(recv_buffer, "OK") == 0)
            printf("\n[SUCCES] A PIVOTE ! Merci de votre participation.\n");
        else
            printf("\n[ECHEC] Vote refuse (ID invalide, deja vote, ou scrutin ferme).\n");
    }
}

/* =========================================================
 * 5. NETTOYAGE
 * ========================================================= */
void fermerConnexion(SOCKET sock)
{
    closesocket(sock);
    WSACleanup();
}
