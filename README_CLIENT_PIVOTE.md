**PIVOTE --- README CLIENT**

Système de vote électronique --- Module Votant

# 1. Introduction

Le client PIVOTE est l\'application que chaque électeur lance sur sa
machine. Il se connecte au serveur via TCP/IP, s\'authentifie avec son
login et mot de passe, reçoit la liste des candidats, soumet son vote et
reçoit la confirmation.

**Fichiers sources du projet client :**

  ---------------------------------- -------------------------------------------
  **client.h**                       Déclarations : constantes PORT, BUFFER,
                                     prototypes de toutes les fonctions.

  **FONCTIONS_PIVOTE_CLIENT_V2.c**   Implémentation : socket, connexion,
                                     authentification, vote, confirmation.

  **PIVOTE_CLIENT_V2.c**             Point d\'entrée : enchaîne les 8 étapes
                                     dans l\'ordre.
  ---------------------------------- -------------------------------------------

# 2. Compilation et lancement

> gcc -std=c99 -Wall FONCTIONS_PIVOTE_CLIENT_V2.c
>
> PIVOTE_CLIENT_V2.c -o client.exe -lws2_32

Le paramètre -lws2_32 est obligatoire pour lier la bibliothèque Winsock2
sous Windows.

# 3. Flux complet d\'exécution

Le client_main.c enchaîne exactement ces 8 étapes dans l\'ordre. Si une
étape échoue, le programme appelle fermerConnexion() et s\'arrête
proprement.

  --------------------- -------------------------------------------------
  **Étape 1**           initialiserSocket() --- Démarre Winsock2, crée la
                        socket TCP.

  **Étape 2**           connecterAuServeur() --- Demande l\'IP, établit
                        la connexion TCP port 8888.

  **Étape 3**           authentifier() --- Envoie AUTH login mdp, attend
                        AUTH_OK ou AUTH_FAIL.

  **Étape 4**           recevoirListeCandidats() --- Reçoit et affiche la
                        liste envoyée par le serveur.

  **Étape 5**           saisirVote() --- Demande ID électeur + ID
                        candidat avec confirmation.

  **Étape 6**           envoyerVote() --- Envoie \"VOTE idE idC\" au
                        serveur.

  **Étape 7**           recevoirConfirmation() --- Reçoit \"OK\" ou
                        \"ERREUR\" et affiche le message.

  **Étape 8**           fermerConnexion() --- closesocket() +
                        WSACleanup().
  --------------------- -------------------------------------------------

# 4. Fonctions détaillées

## 4.1 initialiserSocket()

Initialise la bibliothèque réseau Windows et crée la socket de
communication.

  ----------------------------- ----------------------------------------------
  **WSAStartup(MAKEWORD(2,2),   Démarre Winsock2. Obligatoire avant tout appel
  &wsa)**                       réseau.

  **socket(AF_INET,             Crée une socket TCP (IPv4, mode connecté,
  SOCK_STREAM, 0)**             protocole auto).

  **Valeur de retour**          Retourne 1 si succès, 0 si WSAStartup ou
                                socket() échouent.
  ----------------------------- ----------------------------------------------

## 4.2 connecterAuServeur()

Demande l\'adresse IP du serveur à l\'utilisateur et établit la
connexion TCP.

  --------------------- -------------------------------------------------
  **lire_ligne()**      Lit l\'adresse IP saisie par l\'utilisateur.

  **inet_addr(ip)**     Convertit la chaîne IP en format binaire réseau.

  **addr.sin_port =     Définit le port de connexion (même valeur que le
  htons(8888)**         serveur).

  **connect(sock,       Établit la connexion TCP. Retourne 0 si succès,
  &addr, \...)**        -1 sinon.
  --------------------- -------------------------------------------------

## 4.3 authentifier()

Gère la boucle de connexion avec 3 tentatives maximum. C\'est la
fonction la plus importante côté client.

  --------------------------------- ----------------------------------------------
  **tentatives = 3**                Compteur décrémenté à chaque échec.

  **lire_ligne(\"Identifiant\")**   Lit le login saisi au clavier.

  **lire_ligne(\"Mot de passe\")**  Lit le mot de passe saisi au clavier.

  **snprintf(buf, \..., \"AUTH %s   Formate le message à envoyer : \"AUTH alice
  %s\")**                           motdepasse\".

  **send(sock, buf, strlen(buf),    Envoie le message AUTH au serveur via la
  0)**                              socket.

  **recv(sock, resp, BUFFER-1, 0)** Attend la réponse du serveur (AUTH_OK ou
                                    AUTH_FAIL).

  **strcmp(resp, \"AUTH_OK\")**     Compare la réponse. Si égal → accès accordé,
                                    retour 1.

  **tentatives\--**                 Si AUTH_FAIL → décrémente et affiche
                                    tentatives restantes.
  --------------------------------- ----------------------------------------------

Si toutes les tentatives sont épuisées, la fonction retourne 0 et main()
appelle fermerConnexion() immédiatement.

## 4.4 saisirVote()

Boucle do/while qui demande confirmation avant de valider le vote. Les
valeurs sont retournées via des pointeurs.

  --------------------- -------------------------------------------------
  **scanf(\"%d\",       Lit l\'ID numérique de l\'électeur (doit
  idE)**                correspondre au login).

  **scanf(\"%d\",       Lit l\'ID du candidat choisi (0 = vote blanc).
  idC)**                

  **Confirmation        Si l\'utilisateur tape 0, la boucle recommence
  (1=OUI)**             sans voter.

  **\*idE et \*idC**    Paramètres pointeurs : les valeurs sont passées à
                        envoyerVote().
  --------------------- -------------------------------------------------

## 4.5 envoyerVote()

Formate et envoie le message de vote au serveur.

> snprintf(buf, sizeof(buf), \"VOTE %d %d\", idE, idC);
>
> send(sock, buf, strlen(buf), 0);

## 4.6 recevoirConfirmationVote()

Reçoit la réponse finale du serveur et affiche le message correspondant.

  --------------------- -------------------------------------------------
  **\"OK\"**            Vote enregistré : affiche « A PIVOTE ! Merci de
                        votre participation. »

  **\"ERREUR\"**        Vote refusé : vote fermé, déjà voté, ou ID ne
                        correspond pas au login.
  --------------------- -------------------------------------------------

## 4.7 fermerConnexion()

Ferme proprement la socket et libère les ressources réseau Windows.

  ----------------------- -------------------------------------------------
  **closesocket(sock)**   Ferme la connexion TCP et libère la socket.

  **WSACleanup()**        Libère les ressources Winsock2. À appeler une
                          fois à la fin.
  ----------------------- -------------------------------------------------

# 5. Protocole réseau avec le serveur

Le client et le serveur communiquent par messages texte simples. Le
protocole se déroule toujours dans le même ordre :

  --------------------- -------------------------------------------------
  **CLIENT envoie**     AUTH alice.d motdepasse

  **SERVEUR répond**    AUTH_OK ou AUTH_FAIL

  **SERVEUR envoie**    \[Liste des candidats en texte\]

  **CLIENT envoie**     VOTE 42 3

  **SERVEUR répond**    OK ou ERREUR
  --------------------- -------------------------------------------------

En cas d\'AUTH_FAIL : le client décrémente son compteur de tentatives et
peut réessayer (jusqu\'à 3 fois). En cas d\'ERREUR sur le vote : le vote
était fermé, l\'électeur avait déjà voté, ou l\'ID ne correspond pas au
login authentifié.

# 6. Messages d\'erreur courants

  --------------------- -------------------------------------------------
  **Connexion refusée** Le serveur n\'est pas démarré ou l\'IP est
                        incorrecte.

  **AUTH_FAIL répété**  Login ou mot de passe incorrect. Contacter
                        l\'admin pour reset.

  **ERREUR sur le       Vote fermé par l\'admin, ou cet électeur a déjà
  vote**                voté.

  **Timeout / pas de    Vérifier que le serveur est toujours actif et sur
  réponse**             le même réseau.
  --------------------- -------------------------------------------------

# 7. Notes importantes

-   Le client et le serveur doivent être sur le même réseau local (ou
    utiliser l\'IP locale 127.0.0.1 pour tester sur la même machine).

-   Le port utilisé est 8888. Vérifier qu\'il n\'est pas bloqué par un
    pare-feu.

-   Un électeur ne peut voter qu\'une seule fois. Une seconde tentative
    recevra ERREUR.

-   Pour les accents dans la console : SetConsoleOutputCP(1252) dans
    main().
