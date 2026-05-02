**PIVOTE --- README SERVEUR**

Système de vote électronique --- Module Administrateur

# 1. Introduction

Le serveur PIVOTE est l\'application que l\'administrateur lance sur sa
machine. Il gère l\'intégralité du scrutin : création des comptes
électeurs, ouverture et fermeture du vote, affichage des résultats,
génération du rapport final et écoute des connexions réseau des clients.

**Fichiers sources du projet serveur :**

  ------------------------------------------------------------------------------
  **auth.h / auth.c**                 Module d\'authentification fourni par le
                                      professeur. Gère users.csv.
  ----------------------------------- ------------------------------------------
  **serveur.h**                       Déclarations : structures, constantes,
                                      prototypes de toutes les fonctions.

  **FONCTIONS_PIVOTE_SERVEUR_V2.c**   Implémentation complète : logique vote,
                                      réseau, menus, nouvelles fonctionnalités.

  **PIVOTE_SERVEUR_V2.c**             Point d\'entrée : initialise auth,
                                      connexion admin, charge données, lance
                                      menu.
  ------------------------------------------------------------------------------

# 2. Compilation et lancement

Environnement requis : Windows 10/11, Code::Blocks, compilateur MinGW
(C99).

### Commande de compilation

> gcc -std=c99 -Wall FONCTIONS_PIVOTE_SERVEUR_V2.c
>
> PIVOTE_SERVEUR_V2.c-o serveur.exe -lws2_32

### Paramètre linker obligatoire

-   -lws2_32 : bibliothèque Winsock2 pour les sockets réseau Windows.

### Dans Code::Blocks

-   Project \> Build Options \> Linker settings \> Other linker options
    : -lws2_32

-   Compiler flags : -std=c99

# 3. Démarrage --- Connexion administrateur

### 3.1 ecranConnexionAdmin()

Appelée en premier dans main(). Elle vérifie si un compte admin existe
déjà dans users.csv. Si le fichier est vide (première utilisation), elle
propose de créer le compte administrateur principal.

  ----------------------------------------------------------------------------
  **auth_list_users()**      Charge tous les utilisateurs existants depuis
                             users.csv.
  -------------------------- -------------------------------------------------
  **strcmp(role, admin)**    Vérifie qu\'au moins un compte a le rôle
                             \"admin\".

  **auth_register_user()**   Crée le premier compte admin si aucun n\'existe.

  **auth_authenticate()**    Vérifie login + mot de passe. Retourne AUTH_OK si
                             valide.
  ----------------------------------------------------------------------------

Limite de 3 tentatives. Un compte avec le rôle \"votant\" est refusé
même avec le bon mot de passe.

# 4. Gestion des électeurs et candidats

### 4.1 ajouterElecteur()

Cette fonction fait deux choses simultanément : elle ajoute l\'électeur
dans le tableau electeurs\[\] ET crée son compte dans users.csv via
auth_register_user(). C\'est le lien clé entre les deux systèmes.

  ----------------------------------------------------------------------------
  **scanf / fgets**          Lit l\'ID numérique et le nom de l\'électeur.
  -------------------------- -------------------------------------------------
  **lire_ligne_srv()**       Lit le login et le mot de passe initial.

  **auth_register_user()**   Crée le compte dans users.csv avec le rôle
                             \"votant\".

  **e.username**             Champ stockant le login pour lier ID et compte
                             auth.
  ----------------------------------------------------------------------------

### 4.2 ajouterCandidat()

Ajoute un candidat dans le tableau candidats\[\] avec son ID, son nom et
un compteur voix initialisé à 0.

# 5. Logique de vote

### 5.1 ouvrirVote() / fermerVote()

ouvrirVote() positionne simplement la variable globale voteOuvert à 1.

fermerVote() est la fonction centrale des nouveautés V2. Elle exécute
automatiquement trois actions dans l\'ordre :

  -----------------------------------------------------------------------------
  **voteOuvert = 0**          Bloque immédiatement tout nouveau vote réseau.
  --------------------------- -------------------------------------------------
  **afficherBarresASCII()**   Affiche les résultats visuels en barres dans le
                              terminal.

  **afficherGagnant()**       Proclame le gagnant ou déclare une égalité.

  **genererRapportFinal()**   Génère le fichier rapport_final.txt horodaté.
  -----------------------------------------------------------------------------

# 6. Nouvelles fonctionnalités V2

Ces trois fonctions constituent les ajouts principaux de la version 2.
Elles sont toutes appelées automatiquement à la fermeture du vote via
fermerVote().

## 6.1 afficherBarresASCII()

Affiche dans le terminal une représentation visuelle proportionnelle des
résultats. Chaque candidat est représenté par une barre de 20 caractères
\'#\'.

### Algorithme

  -----------------------------------------------------------------------
  **Étape 1**           Calcul du total des voix : somme de tous les
                        candidats + votes blancs.
  --------------------- -------------------------------------------------
  **Étape 2**           Pour chaque candidat : pct = 100.0 \* voix /
                        total

  **Étape 3**           Nombre de \'#\' = 20 \* voix / total (arrondi
                        entier par défaut C).

  **Étape 4**           printf avec \'#\' répétés + espaces pour
                        compléter à 20 caractères.

  **Étape 5**           Affichage identique pour la ligne VOTE BLANC.
  -----------------------------------------------------------------------

### Exemple de sortie

> Alice Dupont \[############ \] 12 voix ( 48.0%)
>
> Bob Martin \[######## \] 8 voix ( 32.0%)
>
> Clara Petit \[### \] 3 voix ( 12.0%)
>
> VOTE BLANC \[## \] 2 voix ( 8.0%)

Cette fonction est également utilisée dans threadAffichageTempsReel()
qui rafraîchit l\'affichage toutes les 3 secondes pendant que le vote
est ouvert.

## 6.2 afficherGagnant()

Détermine et annonce le gagnant du scrutin. Gère proprement les cas
d\'égalité.

### Algorithme en deux passes

  -----------------------------------------------------------------------
  **Passe 1**           Parcourt candidats\[\] et trouve la valeur
                        maximale de voix.
  --------------------- -------------------------------------------------
  **Vérif**             Si maxVoix == 0, aucun vote exprimé → message et
                        retour.

  **Passe 2**           Compte combien de candidats atteignent cette
                        valeur maximale.

  **Cas 1**             nbGagnants == 1 → affiche « GAGNANT DU SCRUTIN »
                        avec le nom.

  **Cas 2**             nbGagnants \> 1 → affiche « EGALITE PARFAITE »
                        avec tous les noms.
  -----------------------------------------------------------------------

Note : les votes blancs sont comptabilisés séparément et ne peuvent
jamais être déclarés gagnants.

## 6.3 genererRapportFinal()

Crée le fichier rapport_final.txt sur le disque avec l\'ensemble des
données du scrutin.

### Fonctions C standard utilisées

  -----------------------------------------------------------------------
  **time(NULL)**        Récupère l\'horodatage UNIX actuel (secondes
                        depuis 1970).
  --------------------- -------------------------------------------------
  **localtime(&now)**   Convertit en structure tm : jour, mois, année,
                        heure, minute, seconde.

  **strftime()**        Formate la date en texte lisible : \"15/06/2025 a
                        14:32:07\".

  **fopen(RAPPORT,      Crée ou écrase le fichier rapport_final.txt.
  \"w\")**              

  **fprintf()**         Écrit chaque section dans le fichier :
                        participation, résultats, gagnant.

  **fclose()**          Ferme proprement le fichier après écriture.
  -----------------------------------------------------------------------

### Contenu généré

-   Date et heure de génération du rapport.

-   Nombre d\'électeurs inscrits, votes exprimés, votes blancs.

-   Taux de participation en pourcentage.

-   Résultats par candidat avec voix et pourcentage.

-   Gagnant ou liste des ex-æquo.

# 7. Serveur réseau --- Protocol AUTH + VOTE

### 7.1 lancerServeurReseau()

Lance deux threads Windows via CreateThread() : un thread réseau
principal et un thread d\'affichage temps réel.

### 7.2 threadServeurReseau()

Tourne en boucle infinie et traite chaque client connecté en 3 étapes
séquentielles :

  -----------------------------------------------------------------------
  **Étape 1 --- AUTH**  Reçoit \"AUTH login mdp\", vérifie avec
                        auth_authenticate() + rôle votant.
  --------------------- -------------------------------------------------
  **Étape 2 --- Liste** Envoie la liste des candidats sous forme de texte
                        formaté.

  **Étape 3 --- VOTE**  Reçoit \"VOTE idElecteur idCandidat\", vérifie
                        triple condition.
  -----------------------------------------------------------------------

Triple vérification du vote : (1) auth OK + rôle votant, (2) idElecteur
correspond au username connecté, (3) l\'électeur n\'a pas encore voté
(a_vote == 0).

# 8. Navigation clavier et couleurs console

### 8.1 naviguerMenu() / afficherMenuNavigue()

Remplace l\'ancien système scanf. Utilise \_getch() de \<conio.h\> pour
capturer les touches sans appuyer sur Entrée.

  ------------------------------------------------------------------------------
  **\_getch()**                   Lit une touche clavier sans affichage ni
                                  Entrée.
  ------------------------------- ----------------------------------------------
  **Code 0 ou 224**               Préfixe signalant une touche spéciale (flèche,
                                  F1\...).

  **Code 72 (après 224)**         Flèche HAUT → sel = (sel - 1 + n) % n

  **Code 80 (après 224)**         Flèche BAS → sel = (sel + 1) % n

  **Code 13**                     Touche ENTRÉE → retourne
                                  indexVersOption\[sel\]

  **SetConsoleTextAttribute()**   Change la couleur du texte : 11 = cyan, 12 =
                                  rouge, 7 = blanc.
  ------------------------------------------------------------------------------

### 8.2 Codes couleurs utilisés

  -----------------------------------------------------------------------
  **COULEUR_TITRE =     Rouge clair --- titre du menu principal.
  12**                  
  --------------------- -------------------------------------------------
  **COULEUR_SELEC =     Cyan clair --- option actuellement sélectionnée.
  11**                  

  **COULEUR_NORMAL =    Blanc standard --- toutes les autres options.
  7**                   
  -----------------------------------------------------------------------

# 9. Persistance des données

  -----------------------------------------------------------------------------
  **sauvegarderDonnees()**    Écrit voteOuvert, tous les électeurs et candidats
                              dans vote_data.txt.
  --------------------------- -------------------------------------------------
  **chargerDonnees()**        Relit vote_data.txt au démarrage pour reprendre
                              une session précédente.

  **exporterVersExcel()**     Génère resultats_vote.csv avec ID;Nom;Voix pour
                              chaque candidat.

  **genererRapportFinal()**   Génère rapport_final.txt à la fermeture (voir
                              section 6.3).
  -----------------------------------------------------------------------------

# 10. Gestion des comptes utilisateurs

Accessible via Option 12 du menu principal. Toutes ces fonctions
s\'appuient sur le module auth.c fourni par le professeur.

  ------------------------------------------------------------------------------
  **menu_inscription_admin()**   Crée un compte avec login, mot de passe et rôle
                                 (admin ou votant).
  ------------------------------ -----------------------------------------------
  **menu_changer_mdp()**         Change le mot de passe en vérifiant l\'ancien.

  **menu_reinitialiser_mdp()**   Reset forcé : appelle
                                 auth_change_password(\..., NULL, \...). Le NULL
                                 saute la vérification de l\'ancien mot de
                                 passe.

  **menu_activation(1/0)**       Active ou désactive un compte sans le
                                 supprimer.

  **menu_lister()**              Affiche tous les comptes avec rôle et statut
                                 actif.
  ------------------------------------------------------------------------------

# 11. Fichiers générés par le serveur

  --------------------------------------------------------------------------
  **users.csv**            Comptes utilisateurs (login;mdp;role;actif). Géré
                           par auth.c.
  ------------------------ -------------------------------------------------
  **vote_data.txt**        Sauvegarde complète électeurs + candidats + état
                           du vote.

  **resultats_vote.csv**   Export tableur : ID;Nom Candidat;Nombre de Voix.

  **rapport_final.txt**    Rapport complet généré automatiquement à la
                           fermeture du vote.
  --------------------------------------------------------------------------

# 12. Support des accents

Pour afficher correctement les caractères accentués (é, è, à, ç\...)
dans la console Windows :

-   Dans main() : SetConsoleOutputCP(1252) et SetConsoleCP(1252).

-   Encoder les fichiers source en Windows-1252 (ANSI) via Bloc-notes.
