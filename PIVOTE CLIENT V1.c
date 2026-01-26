#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <locale.h>
#pragma comment(lib, "ws2_32.lib")
#define PORT 8888
#define BUFFER 2048
//
void viderBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
//
int main(void)
{
    system("chcp 65001");
    setlocale(LC_ALL,"");
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server_addr;
    char send_buffer[BUFFER];
    char recv_buffer[BUFFER];
    char server_ip[50];
    int idE, idC, confirmation;

    // Initialisation Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

    printf("===================================================\n");
    printf("                     PIVOTE\n");
    printf("===================================================\n\n");

    // 1. Demande de l'IP pour le réseau
    printf("Entrez l'adresse IP du serveur (ex: 192.168.1.15) : ");
    scanf("%s", server_ip);

    // Création socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Erreur socket.\n");
        return 1;
    }

    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Connexion
    printf("Tentative de connexion a %s...\n", server_ip);
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("\n[ERREUR FATALE] Impossible de joindre le serveur.\n");
        printf("Vérifiez :\n1. L'adresse IP est correcte.\n2. Le serveur a lancé l'option 9.\n3. Le pare-feu Windows.\n");
        system("pause");
        return 1;
    }

    // 2. Réception et Affichage de la liste des candidats
    int len = recv(sock, recv_buffer, BUFFER - 1, 0);
    if (len > 0) {
        recv_buffer[len] = '\0';
        printf("%s", recv_buffer); // Affiche la liste envoyée par le serveur
    }

    // 3. Boucle de vote et de confirmation
    int voteValide = 0;
    do {
        printf("\n--- FORMULAIRE DE VOTE ---\n");
        printf("Entrer votre ID électeur s'il vous plait : ");
        scanf("%d", &idE);

        printf("ID du candidat choisi : ");
        scanf("%d", &idC);

        printf("\nVOUS ALLEZ VOTER :\n");
        printf(" Vous avez pour ID : %d\n", idE);
        printf(" Le Candidat que vous choisissez a pour ID : %d\n", idC);
        printf("Confirmez-vous ce choix ? (1=OUI / 0=NON) : ");
        scanf("%d", &confirmation);

        if (confirmation == 1) {
            voteValide = 1;
        } else {
            printf("\n[INFO] Vote annule. Recommencez.\n");
        }

    } while (voteValide == 0);

    // 4. Envoi du vote
    sprintf(send_buffer, "%d %d", idE, idC);
    send(sock, send_buffer, strlen(send_buffer), 0);

    // 5. Réception confirmation finale
    len = recv(sock, recv_buffer, BUFFER - 1, 0);
    if (len > 0) {
        recv_buffer[len] = '\0';
        if (strcmp(recv_buffer, "OK") == 0)
            printf("\n[SUCCES] A PIVOTE ! Merci de votre participation.\n");
        else
            printf("\n[ECHEC] Vote refuse (l'ID est invalide,Vous avez déjà voté, ou le scrutin est fermé).\n");
    }

    closesocket(sock);
    WSACleanup();

    printf("\nAppuyez sur Entrer pour quitter...");
    viderBuffer(); getchar();
    return 0;
}
