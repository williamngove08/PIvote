/**
 * @file client.h
 * @brief Signatures des fonctions du CLIENT PIVOTE V2 (votant).
 *
 * Compilation (MinGW / Code::Blocks, C99) :
 * gcc -std=c99 -Wall client_impl.c client_main.c -o client.exe -lws2_32
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <winsock2.h>
#include <windows.h>
#include <stddef.h>

/* =========================================================
 * CONSTANTES
 * ========================================================= */
#define PORT   8888
#define BUFFER 2048

/* =========================================================
 * 1. HELPERS CONSOLE
 * ========================================================= */
/**
 * @brief Vide le buffer stdin jusqu'au prochain '\n'.
 */
void viderBuffer(void);

/**
 * @brief Affiche une invite et lit une ligne depuis stdin.
 * @param invite Texte affiche avant la saisie.
 * @param buffer Buffer de destination.
 * @param taille Taille maximale du buffer.
 */
void lire_ligne(const char *invite, char *buffer, size_t taille);

/* =========================================================
 * 2. CONNEXION RESEAU
 * ========================================================= */
/**
 * @brief Initialise Winsock et cree la socket TCP.
 * @param sock Pointeur vers la socket a initialiser (sortie).
 * @return 1 si succes, 0 si erreur.
 */
int initialiserSocket(SOCKET *sock);

/**
 * @brief Demande l'IP du serveur a l'utilisateur et etablit la connexion.
 * @param sock Socket deja creee.
 * @param server_ip Buffer ou stocker l'IP saisie (taille >= 50).
 * @return 1 si connecte, 0 si echec.
 */
int connecterAuServeur(SOCKET sock, char *server_ip);

/* =========================================================
 * 3. AUTHENTIFICATION
 * ========================================================= */
/**
 * @brief Gere la boucle d'authentification (3 tentatives max).
 * Envoie "AUTH <username> <password>" et attend "AUTH_OK".
 * Affiche le message mot de passe oublie uniquement en cas d'echec.
 * @param sock     Socket connectee au serveur.
 * @param username Buffer ou stocker le login saisi (taille >= 65).
 * @param password Buffer ou stocker le mot de passe saisi (taille >= 65).
 * @return 1 si authentifie, 0 si toutes les tentatives epuisees.
 */
int authentifier(SOCKET sock, char *username, char *password);

/* =========================================================
 * 4. VOTE
 * ========================================================= */
/**
 * @brief Recoit et affiche la liste des candidats envoyee par le serveur.
 * @param sock Socket connectee au serveur.
 */
void recevoirListeCandidats(SOCKET sock);

/**
 * @brief Boucle de saisie du vote avec confirmation.
 * @param idE Pointeur vers l'ID electeur (sortie).
 * @param idC Pointeur vers l'ID candidat (sortie).
 */
void saisirVote(int *idE, int *idC);

/**
 * @brief Envoie le vote au serveur au format "VOTE <idE> <idC>".
 * @param sock Socket connectee au serveur.
 * @param idE  ID de l'electeur.
 * @param idC  ID du candidat choisi.
 */
void envoyerVote(SOCKET sock, int idE, int idC);

/**
 * @brief Recoit et affiche la confirmation finale du vote.
 * @param sock Socket connectee au serveur.
 */
void recevoirConfirmationVote(SOCKET sock);

/* =========================================================
 * 5. NETTOYAGE
 * ========================================================= */
/**
 * @brief Ferme la socket et nettoie Winsock.
 * @param sock Socket a fermer.
 */
void fermerConnexion(SOCKET sock);

#endif /* CLIENT_H */
