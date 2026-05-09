/**
 * @file FONCTIONS_PIVOTE_SERVEUR_V2.c
 * @brief Implementation de toutes les fonctions du SERVEUR PIVOTE V2.
 *
 * Nouveautes V2 :
 *   - afficherBarresASCII() : resultats visuels en barres dans le terminal
 *   - afficherGagnant()     : proclamation automatique du gagnant
 *   - genererRapportFinal() : fichier rapport_final.txt complet
 *   - fermerVote()          : appelle les 3 fonctions ci-dessus automatiquement
 *   - naviguerMenu()        : navigation fleches + couleurs console
 *
 * Compilation (MinGW / Code::Blocks, C99) :
 * gcc -std=c99 -Wall FONCTIONS_PIVOTE_SERVEUR_V2.c PIVOTE_SERVEUR_V2.c auth.c -o serveur.exe -lws2_32
 */

#include <conio.h>
#include "serveur.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>
#include <windows.h>
#include "auth.h"
#include <locale.h>

/* =========================================================
 * COULEURS CONSOLE  — doivent etre avant toute fonction
 * ========================================================= */
#define COULEUR_NORMAL   7    /* Blanc sur fond noir      */
#define COULEUR_SELEC   11    /* Cyan clair sur fond noir */
#define COULEUR_TITRE   12    /* Rouge clair              */

static void setCouleur(int couleur)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), couleur);
}

#pragma comment(lib, "ws2_32.lib")

/* =========================================================
 * VARIABLES GLOBALES
 * ========================================================= */
Electeur electeurs[MAX];
Candidat candidats[MAX];
int nbElecteurs        = 0;
int nbCandidats        = 0;
int voteOuvert         = 0;
int affichageAutoActif = 0;
time_t voteDeadline    = 0;
int voteDurationSeconds = 0;

static AuthUser adminConnecte;
static HANDLE  hThreadServeur = NULL;
static SOCKET  gServeurSocket = INVALID_SOCKET;
static SOCKET  gClientSocket  = INVALID_SOCKET;
static CRITICAL_SECTION gNetCs;
static int gNetCsInit = 0;
static volatile LONG gStopReseau = 0;
static volatile LONG gSessionClosing = 0;

static void runtimeServeurInit(void)
{
    if (!gNetCsInit) {
        InitializeCriticalSection(&gNetCs);
        gNetCsInit = 1;
    }
}

static void setSocketValue(SOCKET *slot, SOCKET value)
{
    runtimeServeurInit();
    EnterCriticalSection(&gNetCs);
    *slot = value;
    LeaveCriticalSection(&gNetCs);
}

static SOCKET takeSocketValue(SOCKET *slot)
{
    SOCKET value;
    runtimeServeurInit();
    EnterCriticalSection(&gNetCs);
    value = *slot;
    *slot = INVALID_SOCKET;
    LeaveCriticalSection(&gNetCs);
    return value;
}

static void clearSocketIfMatches(SOCKET *slot, SOCKET expected)
{
    runtimeServeurInit();
    EnterCriticalSection(&gNetCs);
    if (*slot == expected)
        *slot = INVALID_SOCKET;
    LeaveCriticalSection(&gNetCs);
}

static void sendAndCloseSocket(SOCKET *slot, const char *message)
{
    SOCKET s = takeSocketValue(slot);
    if (s == INVALID_SOCKET) return;
    if (message && *message)
        send(s, message, (int)strlen(message), 0);
    shutdown(s, SD_BOTH);
    closesocket(s);
}

static void notifyAndShutdownSocket(SOCKET *slot, const char *message)
{
    SOCKET s = takeSocketValue(slot);
    if (s == INVALID_SOCKET) return;
    if (message && *message)
        send(s, message, (int)strlen(message), 0);
    shutdown(s, SD_BOTH);
}

static void clearClientVoteSession(void)
{
    voteDeadline = 0;
}

static void startClientVoteSession(void)
{
    voteDeadline = (voteDurationSeconds > 0) ? time(NULL) + voteDurationSeconds : 0;
}

static int voteOuvertAvecExpiration(void)
{
    return voteOuvert != 0;
}

static int recvWithVoteChecks(SOCKET client, char *buffer, int capacity)
{
    while (!InterlockedCompareExchange(&gStopReseau, 0, 0)) {
        fd_set rfds;
        struct timeval tv;
        int ready;

        if (InterlockedCompareExchange(&gSessionClosing, 0, 0))
            return -3;
        if (verifierExpirationVote())
            return -2;

        FD_ZERO(&rfds);
        FD_SET(client, &rfds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        ready = select(0, &rfds, NULL, NULL, &tv);
        if (ready == SOCKET_ERROR)
            return -1;
        if (ready == 0)
            continue;

        ready = recv(client, buffer, capacity - 1, 0);
        if (ready <= 0)
            return ready;

        buffer[ready] = '\0';
        return ready;
    }

    return -3;
}

/* =========================================================
 * 1. HELPERS CONSOLE
 * ========================================================= */
void lire_ligne_srv(const char *invite, char *buf, size_t taille)
{
    printf("%s", invite);
    if (fgets(buf, (int)taille, stdin)) {
        size_t len = strlen(buf);
        if (len > 0 && (buf[len-1] == '\n' || buf[len-1] == '\r'))
            buf[len-1] = '\0';
    }
}

void vider_buffer_stdin(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static void afficher_utilisateur(const AuthUser *u)
{
    printf(" - %s (role=%s, actif=%s)\n",
           u->username, u->role, u->active ? "oui" : "non");
}

/* =========================================================
 * 2. GESTION DES ELECTEURS ET CANDIDATS
 * ========================================================= */
void ajouterElecteur(void)
{
    if (nbElecteurs >= MAX) {
        printf("Nombre maximum d'\xe9lecteurs atteint.\n");
        return;
    }

    Electeur e;
    char username[AUTH_MAX_USERNAME + 1];
    char password[AUTH_MAX_PASSWORD + 1];

    printf("ID num\xe9rique de l'\xe9lecteur : ");
    scanf("%d", &e.id);
    vider_buffer_stdin();

    printf("Nom de l'\xe9lecteur : ");
    fgets(e.nom, sizeof(e.nom), stdin);
    {
        size_t l = strlen(e.nom);
        if (l > 0 && e.nom[l-1] == '\n') e.nom[l-1] = '\0';
    }

    lire_ligne_srv("Identifiant de connexion (login) : ", username, sizeof(username));
    lire_ligne_srv("Mot de passe initial             : ", password, sizeof(password));

    for (int i = 0; i < nbElecteurs; i++) {
        if (electeurs[i].id == e.id) {
            printf("Erreur : un \xe9lecteur avec l'ID %d existe d\xe9j\xe0.\n", e.id);
            return;
        }
    }

    AuthStatus st = auth_register_user(CSV_PATH, username, password, "votant");
    if (st == AUTH_ERR_EXISTS) {
        printf("Erreur : un compte '%s' existe d\xe9j\xe0.\n", username);
        return;
    } else if (st != AUTH_OK) {
        printf("Erreur cr\xe9ation du compte (code=%d).\n", st);
        return;
    }

    e.a_vote     = 0;
    e.vote_blanc = 0;
    strncpy(e.username, username, AUTH_MAX_USERNAME);
    e.username[AUTH_MAX_USERNAME] = '\0';

    electeurs[nbElecteurs++] = e;
    printf("\xc9lecteur '%s' (login: %s) enregistr\xe9 avec succ\xe8s.\n", e.nom, username);
}

void afficherElecteurs(void)
{
    for (int i = 0; i < nbElecteurs; i++)
        printf("ID:%d | %s (login:%s) | A vot\xe9: %s\n",
               electeurs[i].id, electeurs[i].nom,
               electeurs[i].username,
               electeurs[i].a_vote ? "OUI" : "NON");
}

void ajouterCandidat(void)
{
    if (nbCandidats >= MAX) return;
    Candidat c;
    printf("ID : ");
    scanf("%d", &c.id);
    vider_buffer_stdin();
    printf("Nom : ");
    fgets(c.nom, sizeof(c.nom), stdin);
    {
        size_t l = strlen(c.nom);
        if (l > 0 && c.nom[l-1] == '\n') c.nom[l-1] = '\0';
    }
    c.voix = 0;
    candidats[nbCandidats++] = c;
    printf("Candidat ajout\xe9.\n");
}

void afficherCandidats(void)
{
    for (int i = 0; i < nbCandidats; i++)
        printf("ID:%d | %s | Voix: %d\n",
               candidats[i].id, candidats[i].nom, candidats[i].voix);
}

/* =========================================================
 * 3. LOGIQUE DE VOTE
 * ========================================================= */
void ouvrirVoteAvecDuree(int dureeSecondes)
{
    if (dureeSecondes < 0) dureeSecondes = 0;

    voteOuvert = 1;
    voteDurationSeconds = dureeSecondes;
    clearClientVoteSession();

    if (dureeSecondes > 0)
        printf("Vote OUVERT. Chaque votant dispose maintenant de %d seconde(s) apres sa connexion.\n", dureeSecondes);
    else
        printf("Vote OUVERT sans limite de temps par votant.\n");
}

void ouvrirVote(void)
{
    ouvrirVoteAvecDuree(0);
}

/*
 * fermerVote() :
 * Ferme le scrutin puis enchaine automatiquement :
 *   1. Affichage des barres ASCII
 *   2. Proclamation du gagnant
 *   3. Generation du rapport final TXT
 */
void fermerVote(void)
{
    if (!voteOuvert && voteDeadline == 0) {
        printf("Vote deja ferme.\n");
        return;
    }

    voteOuvert = 0;
    clearClientVoteSession();
    voteDurationSeconds = 0;
    printf("Vote FERM\xc9.\n\n");

    printf("========================================\n");
    printf("   RESULTATS FINAUX DU SCRUTIN\n");
    printf("========================================\n");
    afficherBarresASCII();

    printf("\n");
    afficherGagnant();

    printf("\n");
    genererRapportFinal();
}

int getVoteTimeRemainingSeconds(void)
{
    if (!voteOuvert) return -1;
    if (voteDeadline == 0)
        return voteDurationSeconds > 0 ? voteDurationSeconds : -1;

    {
        time_t now = time(NULL);
        if (now >= voteDeadline) return 0;
        return (int)difftime(voteDeadline, now);
    }
}

int verifierExpirationVote(void)
{
    if (!voteOuvert || voteDeadline == 0) return 0;
    if (time(NULL) < voteDeadline) return 0;

    sendAndCloseSocket(&gClientSocket, "VOTE_TIMEOUT");
    clearClientVoteSession();
    return 1;
}

void fermerSessionVote(void)
{
    InterlockedExchange(&gSessionClosing, 1);

    if (voteOuvert)
        fermerVote();
    else {
        clearClientVoteSession();
        voteDurationSeconds = 0;
    }

    sauvegarderDonnees();
    exporterVersExcel();

    notifyAndShutdownSocket(&gClientSocket, "SESSION_CLOSED");
    arreterServeurReseau();

    InterlockedExchange(&gSessionClosing, 0);
    printf("Session fermee. Tous les votants connectes ont ete expulses.\n");
}

void afficherResultats(void)
{
    for (int i = 0; i < nbCandidats; i++)
        printf("%s : %d voix\n", candidats[i].nom, candidats[i].voix);
}

void afficherStatistiques(void)
{
    int v = 0, b = 0;
    for (int i = 0; i < nbElecteurs; i++) {
        if (electeurs[i].a_vote) {
            v++;
            if (electeurs[i].vote_blanc) b++;
        }
    }
    printf("Votants: %d / %d | Votes blancs: %d\n", v, nbElecteurs, b);
}

/* =========================================================
 * 4. NOUVELLES FONCTIONNALITES
 * ========================================================= */

/*
 * afficherBarresASCII()
 * --------------------
 * Affiche chaque candidat avec une barre de 20 caracteres '#'
 * proportionnelle a son score, suivie du nombre de voix et
 * du pourcentage.
 *
 * Exemple :
 *   Alice   [################    ] 16 voix (80.0%)
 *   Bob     [####                ]  4 voix (20.0%)
 *   BLANC   [                    ]  0 voix  (0.0%)
 */
void afficherBarresASCII(void)
{
    if (nbCandidats == 0) {
        printf("Aucun candidat enregistr\xe9.\n");
        return;
    }

    /* Calcul du total des voix (candidats + blancs) */
    int totalVoix = 0;
    for (int i = 0; i < nbCandidats; i++)
        totalVoix += candidats[i].voix;

    int blancs = 0;
    for (int i = 0; i < nbElecteurs; i++)
        if (electeurs[i].vote_blanc) blancs++;
    totalVoix += blancs;

    /* Largeur de la barre */
    int largeur = 20;

    printf("\n");
    for (int i = 0; i < nbCandidats; i++) {
        double pct    = (totalVoix > 0) ? (100.0 * candidats[i].voix / totalVoix) : 0.0;
        int    rempli = (totalVoix > 0) ? (largeur * candidats[i].voix / totalVoix) : 0;

        printf("  %-15s [", candidats[i].nom);
        for (int k = 0; k < largeur; k++)
            printf("%c", k < rempli ? '#' : ' ');
        printf("] %3d voix (%5.1f%%)\n", candidats[i].voix, pct);
    }

    /* Ligne votes blancs */
    double pctBlanc    = (totalVoix > 0) ? (100.0 * blancs / totalVoix) : 0.0;
    int    rempliBlanc = (totalVoix > 0) ? (largeur * blancs / totalVoix) : 0;
    printf("  %-15s [", "VOTE BLANC");
    for (int k = 0; k < largeur; k++)
        printf("%c", k < rempliBlanc ? '#' : ' ');
    printf("] %3d voix (%5.1f%%)\n", blancs, pctBlanc);

    printf("\n  Total votes exprim\xe9s : %d\n", totalVoix);
}

/*
 * afficherGagnant()
 * -----------------
 * Parcourt les candidats pour trouver le maximum de voix.
 * Si plusieurs candidats sont a egalite, tous sont affiches.
 * Les votes blancs ne peuvent pas gagner.
 */
void afficherGagnant(void)
{
    if (nbCandidats == 0) {
        printf("Aucun candidat enregistr\xe9.\n");
        return;
    }

    /* Recherche du maximum */
    int maxVoix = 0;
    for (int i = 0; i < nbCandidats; i++)
        if (candidats[i].voix > maxVoix)
            maxVoix = candidats[i].voix;

    if (maxVoix == 0) {
        printf("Aucun vote exprim\xe9. Pas de gagnant.\n");
        return;
    }

    /* Compte les candidats a egalite */
    int nbGagnants = 0;
    for (int i = 0; i < nbCandidats; i++)
        if (candidats[i].voix == maxVoix)
            nbGagnants++;

    printf("========================================\n");
    if (nbGagnants == 1) {
        printf("   GAGNANT DU SCRUTIN\n");
        printf("========================================\n");
        for (int i = 0; i < nbCandidats; i++) {
            if (candidats[i].voix == maxVoix)
                printf("   >> %s avec %d voix <<\n", candidats[i].nom, maxVoix);
        }
    } else {
        printf("   EGALITE PARFAITE !\n");
        printf("========================================\n");
        printf("   Les candidats suivants sont \xe0 \xe9galit\xe9 avec %d voix :\n", maxVoix);
        for (int i = 0; i < nbCandidats; i++) {
            if (candidats[i].voix == maxVoix)
                printf("   >> %s <<\n", candidats[i].nom);
        }
    }
    printf("========================================\n");
}

/*
 * genererRapportFinal()
 * ----------------------
 * Cree le fichier rapport_final.txt avec :
 *   - Date et heure de generation
 *   - Nom de chaque candidat, voix, pourcentage
 *   - Votes blancs
 *   - Gagnant (ou egalite)
 *   - Taux de participation
 */
void genererRapportFinal(void)
{
    FILE *f = fopen(FICHIER_RAPPORT, "w");
    if (!f) {
        printf("[ERREUR] Impossible de creer le rapport final.\n");
        return;
    }

    /* Date et heure */
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char dateBuf[64];
    strftime(dateBuf, sizeof(dateBuf), "%d/%m/%Y a %H:%M:%S", t);

    fprintf(f, "================================================\n");
    fprintf(f, "         RAPPORT FINAL - SCRUTIN PIVOTE\n");
    fprintf(f, "================================================\n");
    fprintf(f, "Genere le : %s\n\n", dateBuf);

    /* Statistiques de participation */
    int votants = 0, blancs = 0;
    for (int i = 0; i < nbElecteurs; i++) {
        if (electeurs[i].a_vote) {
            votants++;
            if (electeurs[i].vote_blanc) blancs++;
        }
    }
    double tauxParticipation = (nbElecteurs > 0)
                               ? (100.0 * votants / nbElecteurs)
                               : 0.0;

    fprintf(f, "------------------------------------------------\n");
    fprintf(f, "PARTICIPATION\n");
    fprintf(f, "------------------------------------------------\n");
    fprintf(f, "Electeurs inscrits : %d\n", nbElecteurs);
    fprintf(f, "Votes exprimes     : %d\n", votants);
    fprintf(f, "Votes blancs       : %d\n", blancs);
    fprintf(f, "Taux participation : %.1f%%\n\n", tauxParticipation);

    /* Resultats par candidat */
    int totalVoix = 0;
    for (int i = 0; i < nbCandidats; i++) totalVoix += candidats[i].voix;
    totalVoix += blancs;

    fprintf(f, "------------------------------------------------\n");
    fprintf(f, "RESULTATS PAR CANDIDAT\n");
    fprintf(f, "------------------------------------------------\n");
    for (int i = 0; i < nbCandidats; i++) {
        double pct = (totalVoix > 0) ? (100.0 * candidats[i].voix / totalVoix) : 0.0;
        fprintf(f, "  %-20s : %3d voix  (%.1f%%)\n",
                candidats[i].nom, candidats[i].voix, pct);
    }
    double pctBlanc = (totalVoix > 0) ? (100.0 * blancs / totalVoix) : 0.0;
    fprintf(f, "  %-20s : %3d voix  (%.1f%%)\n", "VOTE BLANC", blancs, pctBlanc);
    fprintf(f, "\n  Total votes : %d\n\n", totalVoix);

    /* Gagnant */
    fprintf(f, "------------------------------------------------\n");
    fprintf(f, "RESULTAT FINAL\n");
    fprintf(f, "------------------------------------------------\n");

    int maxVoix = 0;
    for (int i = 0; i < nbCandidats; i++)
        if (candidats[i].voix > maxVoix) maxVoix = candidats[i].voix;

    if (maxVoix == 0) {
        fprintf(f, "Aucun vote exprime. Pas de gagnant.\n");
    } else {
        int nbGagnants = 0;
        for (int i = 0; i < nbCandidats; i++)
            if (candidats[i].voix == maxVoix) nbGagnants++;

        if (nbGagnants == 1) {
            for (int i = 0; i < nbCandidats; i++) {
                if (candidats[i].voix == maxVoix)
                    fprintf(f, "GAGNANT : %s avec %d voix\n",
                            candidats[i].nom, maxVoix);
            }
        } else {
            fprintf(f, "EGALITE entre les candidats suivants (%d voix chacun) :\n", maxVoix);
            for (int i = 0; i < nbCandidats; i++)
                if (candidats[i].voix == maxVoix)
                    fprintf(f, "  - %s\n", candidats[i].nom);
        }
    }

    fprintf(f, "\n================================================\n");
    fprintf(f, "           FIN DU RAPPORT\n");
    fprintf(f, "================================================\n");

    fclose(f);
    printf("[INFO] Rapport final g\xe9n\xe9r\xe9 : %s\n", FICHIER_RAPPORT);
}

/* =========================================================
 * 5. PERSISTANCE DES DONNEES
 * ========================================================= */
void sauvegarderDonnees(void)
{
    FILE *f = fopen(FICHIER_SAUVEGARDE, "w");
    if (!f) return;
    fprintf(f, "%d %d %lld\n%d\n",
            voteOuvert,
            voteDurationSeconds,
            0LL,
            nbElecteurs);
    for (int i = 0; i < nbElecteurs; i++)
        fprintf(f, "%d %s %d %d %s\n",
                electeurs[i].id, electeurs[i].nom,
                electeurs[i].a_vote, electeurs[i].vote_blanc,
                electeurs[i].username);
    fprintf(f, "%d\n", nbCandidats);
    for (int i = 0; i < nbCandidats; i++)
        fprintf(f, "%d %s %d\n",
                candidats[i].id, candidats[i].nom, candidats[i].voix);
    fclose(f);
}

void chargerDonnees(void)
{
    FILE *f = fopen(FICHIER_SAUVEGARDE, "r");
    if (!f) return;
    {
        char line[256];
        long long savedDeadline = 0;
        int parsed = 0;

        if (!fgets(line, sizeof(line), f)) {
            fclose(f);
            return;
        }

        parsed = sscanf(line, "%d %d %lld", &voteOuvert, &voteDurationSeconds, &savedDeadline);
        (void)savedDeadline;
        voteDeadline = 0;

        if (parsed < 2) {
            voteDurationSeconds = 0;
            voteDeadline = 0;
        }

        if (!fgets(line, sizeof(line), f)) {
            fclose(f);
            return;
        }
        nbElecteurs = atoi(line);
    }
    for (int i = 0; i < nbElecteurs; i++)
        fscanf(f, "%d %s %d %d %s",
               &electeurs[i].id, electeurs[i].nom,
               &electeurs[i].a_vote, &electeurs[i].vote_blanc,
               electeurs[i].username);
    fscanf(f, "%d", &nbCandidats);
    for (int i = 0; i < nbCandidats; i++)
        fscanf(f, "%d %s %d",
               &candidats[i].id, candidats[i].nom, &candidats[i].voix);
    fclose(f);
    printf(">> Donn\xe9es charg\xe9es.\n");
    verifierExpirationVote();
}

void exporterVersExcel(void)
{
    FILE *f = fopen(FICHIER_EXCEL, "w");
    if (!f) return;
    fprintf(f, "ID Candidat;Nom Candidat;Nombre de Voix\n");
    for (int i = 0; i < nbCandidats; i++)
        fprintf(f, "%d;%s;%d\n",
                candidats[i].id, candidats[i].nom, candidats[i].voix);
    int blancs = 0;
    for (int i = 0; i < nbElecteurs; i++)
        if (electeurs[i].vote_blanc) blancs++;
    fprintf(f, "0;VOTE BLANC;%d\n", blancs);
    fclose(f);
}

/* =========================================================
 * 6. SERVEUR RESEAU (threads Windows)
 * ========================================================= */
static void buildCandidatePayload(char *buffer, size_t size)
{
    int remaining = getVoteTimeRemainingSeconds();
    int written = snprintf(buffer, size,
                           "INFO DEADLINE %lld\nINFO REMAINING %d\n--- LISTE DES CANDIDATS ---\n",
                           (long long)voteDeadline,
                           remaining < 0 ? 0 : remaining);

    if (written < 0 || (size_t)written >= size) {
        if (size > 0) buffer[0] = '\0';
        return;
    }

    for (int k = 0; k < nbCandidats; k++) {
        char ligne[128];
        int n = snprintf(ligne, sizeof(ligne), "[%d] %s\n", candidats[k].id, candidats[k].nom);
        if (n <= 0) continue;
        if ((size_t)written + (size_t)n + 32 >= size) break;
        memcpy(buffer + written, ligne, (size_t)n);
        written += n;
        buffer[written] = '\0';
    }

    if ((size_t)written + 32 < size)
        strcat(buffer, "[0] VOTE BLANC\n---------------------------\n");
}

DWORD WINAPI threadServeurReseau(LPVOID arg)
{
    WSADATA wsa;
    SOCKET  serveur, client;
    struct sockaddr_in addr;
    char   buffer[BUFFER];
    char   listeCandidatsStr[BUFFER];
    int    addrlen = sizeof(addr);

    runtimeServeurInit();
    InterlockedExchange(&gSessionClosing, 0);
    WSAStartup(MAKEWORD(2,2), &wsa);
    serveur = socket(AF_INET, SOCK_STREAM, 0);
    setSocketValue(&gServeurSocket, serveur);

    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(PORT);

    if (bind(serveur, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        printf("[ERREUR] Impossible de lier le port %d.\n", PORT);
        sendAndCloseSocket(&gServeurSocket, NULL);
        WSACleanup();
        return 1;
    }
    listen(serveur, 5);
    printf(">> Serveur r\xe9seau ACTIF sur le port %d.\n", PORT);

    while (!InterlockedCompareExchange(&gStopReseau, 0, 0)) {
        verifierExpirationVote();
        client = accept(serveur, (struct sockaddr*)&addr, &addrlen);
        if (client == INVALID_SOCKET) {
            if (InterlockedCompareExchange(&gStopReseau, 0, 0)) break;
            Sleep(50);
            continue;
        }

        setSocketValue(&gClientSocket, client);

        if (InterlockedCompareExchange(&gSessionClosing, 0, 0)) {
            sendAndCloseSocket(&gClientSocket, "SESSION_CLOSED");
            continue;
        }

        if (verifierExpirationVote()) {
            sendAndCloseSocket(&gClientSocket, "VOTE_TIMEOUT");
            continue;
        }

        /* Etape 1 : Authentification */
        int recv_size = recvWithVoteChecks(client, buffer, BUFFER);
        if (recv_size == -2) {
            continue;
        }
        if (recv_size <= 0) {
            clearSocketIfMatches(&gClientSocket, client);
            closesocket(client);
            continue;
        }

        char cmd[16];
        char username[AUTH_MAX_USERNAME + 1];
        char password[AUTH_MAX_PASSWORD + 1];
        int  parsed = sscanf(buffer, "%15s %64s %64s", cmd, username, password);

        if (parsed != 3 || strcmp(cmd, "AUTH") != 0) {
            send(client, "AUTH_FAIL", 9, 0);
            clearSocketIfMatches(&gClientSocket, client);
            closesocket(client);
            continue;
        }

        AuthUser uAuth;
        AuthStatus stAuth = auth_authenticate(CSV_PATH, username, password, &uAuth);
        if (stAuth != AUTH_OK || strcmp(uAuth.role, "votant") != 0) {
            send(client, "AUTH_FAIL", 9, 0);
            clearSocketIfMatches(&gClientSocket, client);
            closesocket(client);
            continue;
        }

        if (InterlockedCompareExchange(&gSessionClosing, 0, 0)) {
            send(client, "SESSION_CLOSED", 14, 0);
            clearSocketIfMatches(&gClientSocket, client);
            closesocket(client);
            continue;
        }

        if (verifierExpirationVote()) {
            send(client, "VOTE_TIMEOUT", 12, 0);
            clearSocketIfMatches(&gClientSocket, client);
            closesocket(client);
            continue;
        }

        if (!voteOuvertAvecExpiration()) {
            send(client, "VOTE_CLOSED", 11, 0);
            clearSocketIfMatches(&gClientSocket, client);
            closesocket(client);
            continue;
        }

        startClientVoteSession();
        if (verifierExpirationVote()) {
            continue;
        }

        send(client, "AUTH_OK\n", 8, 0);

        /* Etape 2 : Envoi liste candidats */
        buildCandidatePayload(listeCandidatsStr, sizeof(listeCandidatsStr));
        send(client, listeCandidatsStr, strlen(listeCandidatsStr), 0);

        /* Etape 3 : Reception du vote */
        recv_size = recvWithVoteChecks(client, buffer, BUFFER);
        if (recv_size == -2) {
            continue;
        }
        if (recv_size <= 0) {
            clearClientVoteSession();
            clearSocketIfMatches(&gClientSocket, client);
            closesocket(client);
            continue;
        }

        char cmd2[16];
        int  idE = -1, idC = -1;
        sscanf(buffer, "%15s %d %d", cmd2, &idE, &idC);

        int ok = 0;
        const char *reply = "ERREUR";
        int replyLen = 6;

        if (InterlockedCompareExchange(&gSessionClosing, 0, 0)) {
            reply = "SESSION_CLOSED";
            replyLen = 14;
        } else if (verifierExpirationVote()) {
            reply = "VOTE_TIMEOUT";
            replyLen = 12;
        } else if (strcmp(cmd2, "VOTE") == 0 && voteOuvertAvecExpiration()) {
            for (int i = 0; i < nbElecteurs; i++) {
                if (electeurs[i].id == idE
                    && strcmp(electeurs[i].username, username) == 0
                    && electeurs[i].a_vote == 0)
                {
                    int candidatTrouve = 0;
                    for (int j = 0; j < nbCandidats; j++) {
                        if (candidats[j].id == idC) {
                            candidats[j].voix++;
                            candidatTrouve = 1;
                            break;
                        }
                    }
                    electeurs[i].vote_blanc = candidatTrouve ? 0 : 1;
                    electeurs[i].a_vote = 1;
                    ok = 1;
                    break;
                }
            }
            if (!ok) {
                reply = "ERREUR";
                replyLen = 6;
            }
        } else if (!voteOuvert) {
            reply = "VOTE_CLOSED";
            replyLen = 11;
        }

        if (ok) {
            send(client, "OK", 2, 0);
            sauvegarderDonnees();
            exporterVersExcel();
        } else {
            send(client, reply, replyLen, 0);
        }
        clearClientVoteSession();
        clearSocketIfMatches(&gClientSocket, client);
        closesocket(client);
    }

    clearClientVoteSession();
    notifyAndShutdownSocket(&gClientSocket, NULL);
    sendAndCloseSocket(&gServeurSocket, NULL);
    WSACleanup();
    return 0;
}

DWORD WINAPI threadAffichageTempsReel(LPVOID arg)
{
    while (affichageAutoActif) {
        verifierExpirationVote();
        system("cls");
        printf("===== CONTROLE EN TEMPS REEL =====\n");
        afficherBarresASCII();
        printf("\n");
        afficherStatistiques();
        printf("\n[INFO] Fichier Excel mis \xe0 jour automatiquement.\n");
        printf("Appuie sur une touche du menu pour quitter...\n");
        Sleep(3000);
    }
    return 0;
}

void lancerServeurReseau(void)
{
    if (!demarrerServeurReseauAsync()) {
        printf("Erreur thread r\xe9seau.\n");
        return;
    }
    affichageAutoActif = 1;
    CreateThread(NULL, 0, threadAffichageTempsReel, NULL, 0, NULL);
    printf("Mode r\xe9seau actif. Appuyez sur 0 pour quitter proprement.\n");
}

int demarrerServeurReseauAsync(void)
{
    runtimeServeurInit();

    if (serveurReseauActif())
        return 0;

    if (hThreadServeur) {
        CloseHandle(hThreadServeur);
        hThreadServeur = NULL;
    }

    InterlockedExchange(&gStopReseau, 0);
    InterlockedExchange(&gSessionClosing, 0);
    hThreadServeur = CreateThread(NULL, 0, threadServeurReseau, NULL, 0, NULL);
    return hThreadServeur != NULL;
}

void arreterServeurReseau(void)
{
    InterlockedExchange(&gStopReseau, 1);
    affichageAutoActif = 0;

    notifyAndShutdownSocket(&gClientSocket, NULL);
    sendAndCloseSocket(&gServeurSocket, NULL);

    if (hThreadServeur) {
        WaitForSingleObject(hThreadServeur, 2000);
        CloseHandle(hThreadServeur);
        hThreadServeur = NULL;
    }

    sendAndCloseSocket(&gClientSocket, NULL);
}

int serveurReseauActif(void)
{
    if (!hThreadServeur) return 0;
    return WaitForSingleObject(hThreadServeur, 0) == WAIT_TIMEOUT;
}

/* =========================================================
 * 7. GESTION DES COMPTES UTILISATEURS
 * ========================================================= */
void menu_inscription_admin(void)
{
    char username[AUTH_MAX_USERNAME + 1];
    char password[AUTH_MAX_PASSWORD + 1];
    char role[AUTH_MAX_ROLE + 1];

    lire_ligne_srv("Identifiant : ", username, sizeof(username));
    lire_ligne_srv("Mot de passe : ", password, sizeof(password));
    lire_ligne_srv("Role (votant/admin) : ", role, sizeof(role));

    AuthStatus st = auth_register_user(CSV_PATH, username, password, role);
    if (st == AUTH_OK)
        printf("Utilisateur cr\xe9\xe9 avec succ\xe8s.\n");
    else if (st == AUTH_ERR_EXISTS)
        printf("Erreur : identifiant d\xe9j\xe0 existant.\n");
    else
        printf("Erreur creation (code=%d).\n", st);
}

void menu_changer_mdp(void)
{
    char username[AUTH_MAX_USERNAME + 1];
    char old_password[AUTH_MAX_PASSWORD + 1];
    char new_password[AUTH_MAX_PASSWORD + 1];

    lire_ligne_srv("Identifiant : ", username, sizeof(username));
    lire_ligne_srv("Ancien mot de passe : ", old_password, sizeof(old_password));
    lire_ligne_srv("Nouveau mot de passe : ", new_password, sizeof(new_password));

    AuthStatus st = auth_change_password(CSV_PATH, username, old_password, new_password);
    if (st == AUTH_OK)
        printf("Mot de passe mis \xe0 jour.\n");
    else if (st == AUTH_ERR_NOTFOUND)
        printf("Utilisateur inconnu.\n");
    else if (st == AUTH_ERR_INVALID)
        printf("Ancien mot de passe incorrect.\n");
    else
        printf("Erreur (code=%d).\n", st);
}

void menu_reinitialiser_mdp(void)
{
    char username[AUTH_MAX_USERNAME + 1];
    char new_password[AUTH_MAX_PASSWORD + 1];

    printf("\n[REINITIALISATION MOT DE PASSE]\n");
    printf("R\xe9serv\xe9e \xe0 l'administrateur (sans contr\xf4le de l'ancien mdp).\n\n");

    lire_ligne_srv("Identifiant de l'\xe9lecteur : ", username, sizeof(username));
    lire_ligne_srv("Nouveau mot de passe       : ", new_password, sizeof(new_password));

    AuthStatus st = auth_change_password(CSV_PATH, username, NULL, new_password);
    if (st == AUTH_OK)
        printf("Mot de passe de '%s' r\xe9initialis\xe9.\n", username);
    else if (st == AUTH_ERR_NOTFOUND)
        printf("Utilisateur inconnu.\n");
    else
        printf("Erreur (code=%d).\n", st);
}

void menu_activation(int activer)
{
    char username[AUTH_MAX_USERNAME + 1];
    lire_ligne_srv("Identifiant : ", username, sizeof(username));

    AuthStatus st = auth_set_active(CSV_PATH, username, activer);
    if (st == AUTH_OK)
        printf("Compte %s %s.\n", username, activer ? "activ\xe9" : "d\xe9sactiv\xe9");
    else if (st == AUTH_ERR_NOTFOUND)
        printf("Utilisateur inconnu.\n");
    else
        printf("Erreur (code=%d).\n", st);
}

void menu_lister(void)
{
    AuthUser *users = NULL;
    size_t    count = 0;

    AuthStatus st = auth_list_users(CSV_PATH, &users, &count);
    if (st != AUTH_OK) {
        printf("Impossible de lire la liste (code=%d).\n", st);
        return;
    }
    printf("Utilisateurs (%zu) :\n", count);
    for (size_t i = 0; i < count; ++i)
        afficher_utilisateur(&users[i]);
    auth_free_user_list(users);
}

/* =========================================================
 * AFFICHAGE DU SOUS-MENU GESTION AVEC OPTION SURLIGNEE
 * ========================================================= */
void afficherMenuGestionNavigue(int sel)
{
    const char *options[] = {
        "1. Cr\xe9er un compte (admin/autre)",
        "2. Changer un mot de passe",
        "3. Activer un compte",
        "4. D\xe9sactiver un compte",
        "5. Lister les utilisateurs",
        "6. R\xe9initialiser le mot de passe d'un \xe9lecteur",
        "0. Retour"
    };
    int nbOptions = 7;

    system("cls");

    setCouleur(COULEUR_TITRE);
    printf("\n  ===== GESTION DES COMPTES =====\n\n");

    for (int i = 0; i < nbOptions; i++) {
        if (i == sel) {
            setCouleur(COULEUR_SELEC);
            printf("  > %-50s <\n", options[i]);
        } else {
            setCouleur(COULEUR_NORMAL);
            printf("    %-50s\n", options[i]);
        }
    }

    setCouleur(COULEUR_NORMAL);
    printf("\n  [Fleches pour naviguer  |  Entree pour valider]\n");
}

void menuGestionComptes(void)
{
    static const int indexVersOptionGestion[] = {
        1, 2, 3, 4, 5, 6, 0
    };

    int sel     = 0;
    int nbItems = 7;
    int choix   = -1;
    int touche;

    do {
        afficherMenuGestionNavigue(sel);

        /* Navigation fleches */
        while (1) {
            touche = _getch();
            if (touche == 0 || touche == 224) {
                touche = _getch();
                if (touche == 72) {         /* Fleche HAUT */
                    sel = (sel - 1 + nbItems) % nbItems;
                    afficherMenuGestionNavigue(sel);
                } else if (touche == 80) {  /* Fleche BAS */
                    sel = (sel + 1) % nbItems;
                    afficherMenuGestionNavigue(sel);
                }
            } else if (touche == 13) {      /* Entree */
                choix = indexVersOptionGestion[sel];
                setCouleur(COULEUR_NORMAL);
                system("cls");
                break;
            }
        }

        switch (choix) {
            case 1: menu_inscription_admin(); break;
            case 2: menu_changer_mdp();       break;
            case 3: menu_activation(1);       break;
            case 4: menu_activation(0);       break;
            case 5: menu_lister();            break;
            case 6: menu_reinitialiser_mdp(); break;
            case 0: break;
            default: break;
        }

        /* Pause apres chaque action sauf Retour */
        if (choix != 0) {
            printf("\n  Appuyez sur une touche pour revenir...");
            _getch();
        }

    } while (choix != 0);
}

/* =========================================================
 * AFFICHAGE DU MENU PRINCIPAL AVEC OPTION SURLIGNEE
 * ========================================================= */
void afficherMenuNavigue(int sel)
{
    const char *options[] = {
        "1.  Ajouter un \xe9lecteur",
        "2.  Afficher les \xe9lecteurs",
        "3.  Ajouter un candidat",
        "4.  Afficher les candidats",
        "5.  Ouvrir le vote",
        "6.  Fermer le vote  [=> gagnant + rapport auto]",
        "7.  Les r\xe9sultats (texte)",
        "8.  Les r\xe9sultats (barres ASCII)",
        "9.  Les statistiques",
        "10. Lancer le mode R\xc9SEAU",
        "11. Exporter vers Excel",
        "12. Gestion des comptes",
        "0.  Quitter ET R\xc9INITIALISER"
    };
    int nbOptions = 13;

    system("cls");

    setCouleur(COULEUR_TITRE);
    printf("\n  ===== MENU PIVOTE ADMINISTRATEUR =====\n\n");

    for (int i = 0; i < nbOptions; i++) {
        if (i == sel) {
            setCouleur(COULEUR_SELEC);
            printf("  > %-48s <\n", options[i]);
        } else {
            setCouleur(COULEUR_NORMAL);
            printf("    %-48s\n", options[i]);
        }
    }

    setCouleur(COULEUR_NORMAL);
    printf("\n  [Fleches pour naviguer  |  Entree pour valider]\n");
}

/* =========================================================
 * NAVIGATION AU CLAVIER (fleches + Entree)
 * ========================================================= */

/* Table de correspondance : index dans le menu -> numero d'option reel */
static const int indexVersOption[] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0
};

int naviguerMenu(void)
{
    int sel     = 0;   /* index courant dans la liste */
    int nbItems = 13;  /* nombre total d'options      */
    int touche;

    afficherMenuNavigue(sel);

    while (1) {
        touche = _getch();

        /* Les fleches envoient 2 codes : 0 ou 224, puis le code reel */
        if (touche == 0 || touche == 224) {
            touche = _getch();
            if (touche == 72) {          /* Fleche HAUT */
                sel = (sel - 1 + nbItems) % nbItems;
                afficherMenuNavigue(sel);
            }
            else if (touche == 80) {     /* Fleche BAS */
                sel = (sel + 1) % nbItems;
                afficherMenuNavigue(sel);
            }
        }
        else if (touche == 13) {         /* Touche ENTREE */
            setCouleur(COULEUR_NORMAL);
            system("cls");
            return indexVersOption[sel];
        }
    }
}

/* =========================================================
 * 8. MENU PRINCIPAL ADMINISTRATEUR
 * ========================================================= */
void menuServeur(void)
{
    int choix;
    do {
        /* Navigation avec fleches au lieu de scanf */
        choix = naviguerMenu();

        switch (choix) {
        case 1:  ajouterElecteur();      sauvegarderDonnees(); break;
        case 2:  afficherElecteurs();    break;
        case 3:  ajouterCandidat();      sauvegarderDonnees(); break;
        case 4:  afficherCandidats();    break;
        case 5:  ouvrirVote();           sauvegarderDonnees(); break;
        case 6:  fermerVote();           sauvegarderDonnees(); break;
        case 7:  afficherResultats();    break;
        case 8:  afficherBarresASCII();  break;
        case 9:  afficherStatistiques(); break;
        case 10: lancerServeurReseau();  break;
        case 11:
            exporterVersExcel();
            printf("Fichier Excel g\xe9n\xe9r\xe9 !\n");
            break;
        case 12:
            menuGestionComptes();
            break;
        case 0:
            affichageAutoActif = 0;
            remove(FICHIER_SAUVEGARDE);
            printf(">> Session termin\xe9e. Fichiers de sauvegarde supprim\xe9s.\n");
            break;
        default:
            printf("Choix invalide.\n");
            break;
        }

        /* Pause apres chaque action pour lire le resultat */
        if (choix != 0 && choix != 10) {
            printf("\n  Appuyez sur une touche pour revenir au menu...");
            _getch();
        }

    } while (choix != 0);
}

/* =========================================================
 * 9. CONNEXION ADMINISTRATEUR
 * ========================================================= */
int ecranConnexionAdmin(void)
{
    char username[AUTH_MAX_USERNAME + 1];
    char password[AUTH_MAX_PASSWORD + 1];

    printf("\n===================================================\n");
    printf("          PIVOTE - ESPACE ADMINISTRATEUR\n");
    printf("===================================================\n");

    AuthUser *users = NULL;
    size_t    count = 0;
    int adminExiste = 0;
    if (auth_list_users(CSV_PATH, &users, &count) == AUTH_OK) {
        for (size_t i = 0; i < count; i++) {
            if (strcmp(users[i].role, "admin") == 0) {
                adminExiste = 1;
                break;
            }
        }
        auth_free_user_list(users);
    }

    if (!adminExiste) {
        printf("\n[PREMIERE UTILISATION] Aucun administrateur trouv\xe9.\n");
        printf("Veuillez cr\xe9er le compte administrateur principal :\n");
        lire_ligne_srv("Identifiant admin  : ", username, sizeof(username));
        lire_ligne_srv("Mot de passe admin : ", password, sizeof(password));
        AuthStatus st = auth_register_user(CSV_PATH, username, password, "admin");
        if (st != AUTH_OK) {
            printf("Erreur cr\xe9ation admin (code=%d).\n", st);
            return 0;
        }
        printf("Compte admin cr\xe9\xe9. Veuillez vous connecter.\n\n");
    }

    int tentatives = 3;
    while (tentatives > 0) {
        lire_ligne_srv("Identifiant : ", username, sizeof(username));
        lire_ligne_srv("Mot de passe : ", password, sizeof(password));

        AuthStatus st = auth_authenticate(CSV_PATH, username, password, &adminConnecte);
        if (st == AUTH_OK && strcmp(adminConnecte.role, "admin") == 0) {
            printf("\nAuthentification r\xe9ussie. Bonjour %s !\n", adminConnecte.username);
            return 1;
        }
        tentatives--;
        if (tentatives > 0)
            printf("Identifiants incorrects ou compte non-admin. %d tentative(s) restante(s).\n", tentatives);
        else
            printf("Trop de tentatives. Acc\xe8s refus\xe9.\n");
    }
    return 0;
}
