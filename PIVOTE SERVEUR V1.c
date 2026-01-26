//Declaration des bibliotheques utilisees
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <locale.h>

// Liaison avec la librairie Winsock
#pragma comment(lib, "ws2_32.lib")

//Declaration des constantes
#define MAX 100
#define PORT 8888
#define BUFFER 2048
#define FICHIER_SAUVEGARDE "vote_data.txt"
#define FICHIER_EXCEL "resultats_vote.csv"

typedef struct {
    int id;
    char nom[50];
    int a_vote;
    int vote_blanc;
} Electeur;

typedef struct {
    int id;
    char nom[50];
    int voix;
} Candidat;

Electeur electeurs[MAX];
Candidat candidats[MAX];
int nbElecteurs = 0;
int nbCandidats = 0;
int voteOuvert = 0;
int affichageAutoActif = 0;

// --- PROTOTYPES ---
void menuServeur(void);
void ajouterElecteur(void);
void afficherElecteurs(void);
void ajouterCandidat(void);
void afficherCandidats(void);
void ouvrirVote(void);
void fermerVote(void);
void afficherResultats(void);
void afficherStatistiques(void);
void lancerServeurReseau(void);
void sauvegarderDonnees(void);
void chargerDonnees(void);
void exporterVersExcel(void);

// --- THREAD SERVEUR ---
DWORD WINAPI threadServeurReseau(LPVOID arg) {
    WSADATA wsa;
    SOCKET serveur, client;
    struct sockaddr_in addr;
    char buffer[BUFFER];
    char listeCandidatsStr[BUFFER];
    int addrlen = sizeof(addr);
    int idE, idC, ok, i;

    WSAStartup(MAKEWORD(2,2), &wsa);
    serveur = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(serveur, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        printf("[ERREUR] Impossible de lier le port %d.\n", PORT);
        return 1;
    }
    listen(serveur, 5);
    printf(">> Serveur réseau ACTIF sur le port %d.\n", PORT);

    while (1) {
        client = accept(serveur, (struct sockaddr*)&addr, &addrlen);
        if (client == INVALID_SOCKET) continue;

        strcpy(listeCandidatsStr, "\n--- LISTE DES CANDIDATS ---\n");
        char ligne[100];
        for(int k=0; k<nbCandidats; k++) {
            sprintf(ligne, "[%d] %s\n", candidats[k].id, candidats[k].nom);
            strcat(listeCandidatsStr, ligne);
        }
        strcat(listeCandidatsStr, "[0] VOTE BLANC\n---------------------------\n");

        send(client, listeCandidatsStr, strlen(listeCandidatsStr), 0);

        int recv_size = recv(client, buffer, BUFFER, 0);
        if (recv_size > 0) {
            buffer[recv_size] = '\0';
            sscanf(buffer, "%d %d", &idE, &idC);
            ok = 0;

            if (voteOuvert) {
                for (i = 0; i < nbElecteurs; i++) {
                    if (electeurs[i].id == idE && electeurs[i].a_vote == 0) {
                        int candidatTrouve = 0;
                        for(int j=0; j<nbCandidats; j++){
                            if(candidats[j].id == idC) {
                                candidats[j].voix++;
                                candidatTrouve = 1;
                                break;
                            }
                        }
                        electeurs[i].vote_blanc = (candidatTrouve) ? 0 : 1;
                        electeurs[i].a_vote = 1;
                        ok = 1;
                        break;
                    }
                }
            }

            if (ok) {
                send(client, "OK", 2, 0);
                sauvegarderDonnees();
                exporterVersExcel();
            } else {
                send(client, "ERREUR", 6, 0);
            }
        }
        closesocket(client);
    }
    return 0;
}

// --- THREAD AFFICHAGE ---
DWORD WINAPI threadAffichageTempsReel(LPVOID arg) {
    while (affichageAutoActif) {
        system("cls");
        printf("===== CONTROLE EN TEMPS REEL  =====\n");
        afficherResultats();
        printf("\n");
        afficherStatistiques();
        printf("\n[INFO] Fichier Excel mis à jour automatiquement.\n");
        printf("Appuie sur une touche du menu pour quitter...\n");
        Sleep(3000);
    }
    return 0;
}

// --- GESTION FICHIERS ---
void sauvegarderDonnees(void) {
    FILE *f = fopen(FICHIER_SAUVEGARDE, "w");
    if (!f) return;
    fprintf(f, "%d\n%d\n", voteOuvert, nbElecteurs);
    for (int i = 0; i < nbElecteurs; i++) {
        fprintf(f, "%d %s %d %d\n", electeurs[i].id, electeurs[i].nom, electeurs[i].a_vote, electeurs[i].vote_blanc);
    }
    fprintf(f, "%d\n", nbCandidats);
    for (int i = 0; i < nbCandidats; i++) {
        fprintf(f, "%d %s %d\n", candidats[i].id, candidats[i].nom, candidats[i].voix);
    }
    fclose(f);
}

void exporterVersExcel(void) {
    FILE *f = fopen(FICHIER_EXCEL, "w");
    if (!f) return;
    fprintf(f, "ID Candidat;Nom Candidat;Nombre de Voix\n");
    for (int i = 0; i < nbCandidats; i++) {
        fprintf(f, "%d;%s;%d\n", candidats[i].id, candidats[i].nom, candidats[i].voix);
    }
    int blancs = 0;
    for(int i=0; i<nbElecteurs; i++) if(electeurs[i].vote_blanc) blancs++;
    fprintf(f, "0;VOTE BLANC;%d\n", blancs);
    fclose(f);
}

void chargerDonnees(void) {
    FILE *f = fopen(FICHIER_SAUVEGARDE, "r");
    if (!f) return;
    fscanf(f, "%d", &voteOuvert);
    fscanf(f, "%d", &nbElecteurs);
    for (int i = 0; i < nbElecteurs; i++) {
        fscanf(f, "%d %s %d %d", &electeurs[i].id, electeurs[i].nom, &electeurs[i].a_vote, &electeurs[i].vote_blanc);
    }
    fscanf(f, "%d", &nbCandidats);
    for (int i = 0; i < nbCandidats; i++) {
        fscanf(f, "%d %s %d", &candidats[i].id, candidats[i].nom, &candidats[i].voix);
    }
    fclose(f);
    printf(">> Données chargées.\n");
}

// --- MAIN & MENU ---
int main(void) {
    system("chcp 65001");
    setlocale(LC_ALL,"");
    chargerDonnees();
    menuServeur();
    return 0;
}

void menuServeur(void) {
    int choix;
    do {
        printf("\n===== MENU PIVOTE ADMINISTRATEUR =====\n");
        printf("1. Ajouter un électeur\n2. Afficher les électeurs\n3. Ajouter un candidat\n4. Afficher les candidats\n");
        printf("5. Ouvrir le vote\n6. Fermer le vote\n7. Les résultats\n8. Les Statistiques\n");
        printf("9. Lancer le mode RESEAU\n10. Exporter vers Excel \n0. Quitter ET REINITIALISER\n");
        printf("Choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1: ajouterElecteur(); sauvegarderDonnees(); break;
            case 2: afficherElecteurs(); break;
            case 3: ajouterCandidat(); sauvegarderDonnees(); break;
            case 4: afficherCandidats(); break;
            case 5: ouvrirVote(); sauvegarderDonnees(); break;
            case 6: fermerVote(); sauvegarderDonnees(); break;
            case 7: afficherResultats(); break;
            case 8: afficherStatistiques(); break;
            case 9: lancerServeurReseau(); break;
            case 10: exporterVersExcel(); printf("Fichier Excel genere !\n"); break;
            case 0:
                affichageAutoActif = 0;
                // --- REINITIALISATION ---
                remove(FICHIER_SAUVEGARDE);
                printf(">> Session terminée. Fichiers de sauvegarde supprimés.\n");
                break;
        }
    } while (choix != 0);
}

// --- FONCTIONS LOGIQUES ---
void ajouterElecteur(void) {
    if (nbElecteurs >= MAX) return;
    Electeur e;
    printf("ID : "); scanf("%d", &e.id);
    printf("Nom : "); scanf("%s", e.nom);
    e.a_vote = 0; e.vote_blanc = 0;
    electeurs[nbElecteurs++] = e;
}

void afficherElecteurs(void) {
    for (int i = 0; i < nbElecteurs; i++)
        printf("ID:%d | %s | A voté: %s\n", electeurs[i].id, electeurs[i].nom, electeurs[i].a_vote ? "OUI" : "NON");
}

void ajouterCandidat(void) {
    if (nbCandidats >= MAX) return;
    Candidat c;
    printf("ID : "); scanf("%d", &c.id);
    printf("Nom : "); scanf("%s", c.nom);
    c.voix = 0;
    candidats[nbCandidats++] = c;
}

void afficherCandidats(void) {
    for (int i = 0; i < nbCandidats; i++)
        printf("ID:%d | %s | Voix: %d\n", candidats[i].id, candidats[i].nom, candidats[i].voix);
}

void ouvrirVote(void) { voteOuvert = 1; printf("Vote OUVERT.\n"); }
void fermerVote(void) { voteOuvert = 0; printf("Vote FERME.\n"); }

void afficherResultats(void) {
    for (int i = 0; i < nbCandidats; i++)
        printf("%s : %d voix\n", candidats[i].nom, candidats[i].voix);
}

void afficherStatistiques(void) {
    int v=0, b=0;
    for(int i=0; i<nbElecteurs; i++) { if(electeurs[i].a_vote) { v++; if(electeurs[i].vote_blanc) b++; } }
    printf("Votants: %d / %d | Votes blancs: %d\n", v, nbElecteurs, b);
}

void lancerServeurReseau(void) {
    HANDLE thread = CreateThread(NULL, 0, threadServeurReseau, NULL, 0, NULL);
    if (!thread) { printf("Erreur thread réseau.\n"); return; }
    affichageAutoActif = 1;
    CreateThread(NULL, 0, threadAffichageTempsReel, NULL, 0, NULL);
    printf("Mode réseau actif. Appuyez sur 0 pour quitter proprement.\n");
}
