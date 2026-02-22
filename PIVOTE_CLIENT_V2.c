/*
 * PIVOTE - CLIENT / VOTANT - VERSION 2
 * Architecture client/serveur avec authentification synchronisee
 * Compile sous Windows avec Code::Blocks + MinGW (C99)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <locale.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT   8888
#define BUFFER 2048

static void viderBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static void lire_ligne(const char *invite, char *buffer, size_t taille)
{
    printf("%s", invite);
    if (fgets(buffer, (int)taille, stdin)) {
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len-1] == '\n' || buffer[len-1] == '\r'))
            buffer[len-1] = '\0';
    }
}

int main(void)
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    setlocale(LC_ALL, "");

    WSADATA wsa;
    SOCKET  sock;
    struct sockaddr_in server_addr;
    char send_buffer[BUFFER];
    char recv_buffer[BUFFER];
    char server_ip[50];
    char username[65];
    char password[65];
    int  idE, idC, confirmation, len;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

    printf("===================================================\n");
    printf("                     PIVOTE\n");
    printf("===================================================\n\n");

    /* -------------------------------------------------------
     * 1. Demande de l'IP du serveur
     * ------------------------------------------------------- */
    printf("Entrez l'adresse IP du serveur (ex: 192.168.1.15) : ");
    scanf("%49s", server_ip);
    viderBuffer();

    /* -------------------------------------------------------
     * 2. Creation socket et connexion
     * ------------------------------------------------------- */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Erreur socket.\n");
        return 1;
    }

    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(PORT);

    printf("Tentative de connexion a %s...\n", server_ip);
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("\n[ERREUR FATALE] Impossible de joindre le serveur.\n");
        printf("Verifiez :\n1. L'adresse IP est correcte.\n2. Le serveur a lance l'option 9.\n3. Le pare-feu Windows.\n");
        system("pause");
        return 1;
    }
    printf("Connexion etablie.\n\n");

    /* -------------------------------------------------------
     * 3. Authentification (verifiee par le serveur)
     *    L'admin vous a fourni votre identifiant et mot de passe.
     * ------------------------------------------------------- */
    int authentifie = 0;
    int tentatives  = 3;

    while (!authentifie && tentatives > 0) {
        printf("=== CONNEXION ===\n");
        lire_ligne("Identifiant : ", username, sizeof(username));
        lire_ligne("Mot de passe : ", password, sizeof(password));

        /* Envoi de la demande d'authentification */
        snprintf(send_buffer, sizeof(send_buffer), "AUTH %s %s", username, password);
        send(sock, send_buffer, strlen(send_buffer), 0);

        /* Reponse du serveur */
        len = recv(sock, recv_buffer, BUFFER - 1, 0);
        if (len > 0) {
            recv_buffer[len] = '\0';
            if (strcmp(recv_buffer, "AUTH_OK") == 0) {
                authentifie = 1;
                printf("\nConnexion reussie. Bonjour %s !\n", username);
            } else {
                tentatives--;
                if (tentatives > 0) {
                    printf("\n[ECHEC] Identifiant ou mot de passe incorrect. %d tentative(s) restante(s).\n", tentatives);
                    printf("[?] Mot de passe oublie ? Contactez l'administrateur du scrutin.\n");
                    printf("    Il pourra reinitialiser votre mot de passe depuis le serveur.\n\n");
                } else {
                    printf("\n[ECHEC] Trop de tentatives. Acces refuse.\n");
                    printf("[?] Contactez l'administrateur du scrutin pour reinitialiser votre mot de passe.\n");
                    closesocket(sock);
                    WSACleanup();
                    system("pause");
                    return 1;
                }
            }
        }
    }

    /* -------------------------------------------------------
     * 4. Reception et affichage de la liste des candidats
     * ------------------------------------------------------- */
    len = recv(sock, recv_buffer, BUFFER - 1, 0);
    if (len > 0) {
        recv_buffer[len] = '\0';
        printf("%s", recv_buffer); /* Affiche la liste envoyee par le serveur */
    }

    /* -------------------------------------------------------
     * 5. Boucle de vote et de confirmation
     * ------------------------------------------------------- */
    int voteValide = 0;
    do {
        printf("\n--- FORMULAIRE DE VOTE ---\n");
        printf("Entrer votre ID electeur s'il vous plait : ");
        scanf("%d", &idE);
        viderBuffer();

        printf("ID du candidat choisi : ");
        scanf("%d", &idC);
        viderBuffer();

        printf("\nVOUS ALLEZ VOTER :\n");
        printf(" Vous avez pour ID : %d\n", idE);
        printf(" Le Candidat que vous choisissez a pour ID : %d\n", idC);
        printf("Confirmez-vous ce choix ? (1=OUI / 0=NON) : ");
        scanf("%d", &confirmation);
        viderBuffer();

        if (confirmation == 1) {
            voteValide = 1;
        } else {
            printf("\n[INFO] Vote annule. Recommencez.\n");
        }

    } while (voteValide == 0);

    /* -------------------------------------------------------
     * 6. Envoi du vote
     *    Format : "VOTE <idElecteur> <idCandidat>"
     * ------------------------------------------------------- */
    snprintf(send_buffer, sizeof(send_buffer), "VOTE %d %d", idE, idC);
    send(sock, send_buffer, strlen(send_buffer), 0);

    /* -------------------------------------------------------
     * 7. Reception confirmation finale
     * ------------------------------------------------------- */
    len = recv(sock, recv_buffer, BUFFER - 1, 0);
    if (len > 0) {
        recv_buffer[len] = '\0';
        if (strcmp(recv_buffer, "OK") == 0)
            printf("\n[SUCCES] A PIVOTE ! Merci de votre participation.\n");
        else
            printf("\n[ECHEC] Vote refuse (l'ID est invalide, Vous avez deja vote, ou le scrutin est ferme).\n");
    }

    closesocket(sock);
    WSACleanup();

    printf("\nAppuyez sur Entrer pour quitter...");
    viderBuffer();
    getchar();
    return 0;
}
