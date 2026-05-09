/**
 * @file PIVOTE_SERVEUR_GUI.c
 * @brief Interface graphique Win32 Ã¢â‚¬â€ Serveur PIVOTE V2 (Administrateur)
 *        ThÃƒÂ¨me sombre professionnel, navigation par panneau latÃƒÂ©ral.
 *
 * Compilation (MinGW / Code::Blocks, C99) :
 * gcc -std=c99 -Wall FONCTIONS_PIVOTE_SERVEUR_V2.c PIVOTE_SERVEUR_GUI.c auth.c \
 *     -o serveur_gui.exe -lws2_32 -lgdi32 -lcomctl32 -mwindows
 *
 * Dans Code::Blocks :
 *  - Ajouter ce fichier au projet serveur
 *  - Retirer PIVOTE_SERVEUR_V2.c (il a son propre main)
 *  - Linker settings : ajouter -lgdi32 -lcomctl32 -lws2_32
 *  - Compiler flags  : -std=c99 -mwindows
 */

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "serveur.h"
#include "auth.h"

/* Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â
 *  DIMENSIONS
 * Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â */
#define DEF_WIN_W  1260
#define DEF_WIN_H   860
#define SIDE_W      260
#define HEAD_H       84
#define STAT_H       34
#define CONT_X   SIDE_W
#define CONT_Y   HEAD_H
#define CONT_W   (gWinW - SIDE_W)
#define CONT_H   (gWinH - HEAD_H - STAT_H)
#define WIN_W    (gWinW)
#define WIN_H    (gWinH)

/* Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â
 *  PALETTE Ã¢â‚¬â€ dark GitHub-style
 * Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â */
#define C_BG    RGB( 8,12,20)
#define C_SIDE  RGB(18,24,36)
#define C_HEAD  RGB(18,58,112)
#define C_CARD  RGB(28,37,55)
#define C_BORD  RGB(62,74,100)
#define C_ACCE  RGB(106,191,255)
#define C_SUCC  RGB( 76,212,112)
#define C_DANG  RGB(255,102, 96)
#define C_WARN  RGB(255,195, 74)
#define C_TEXT  RGB(238,244,255)
#define C_SUBT  RGB(182,198,224)
#define C_NSEL  RGB(31,70,124)
#define C_BPRI  RGB(41,126,255)
#define C_BGRN  RGB(29,138,67)
#define C_BRED  RGB(194,53,53)
#define C_WHITE RGB(255,255,255)

/* Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â
 *  IDs DES CONTROLES
 * Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â */
/* Login */
#define IDC_EUSER   100
#define IDC_EPASS   101
#define IDC_BLOGIN  102
#define IDC_LERR    103
#define IDC_LUSER   104
#define IDC_LPASS   105

/* Navigation sidebar */
#define IDC_NAV0    200
#define IDC_NAV1    201
#define IDC_NAV2    202
#define IDC_NAV3    203
#define IDC_NAV4    204
#define IDC_NAV5    205
#define IDC_BQUIT   210

/* Panel Electeurs */
#define IDC_LBELEC  300
#define IDC_BADE    301
#define IDC_LBELES  302

/* Panel Candidats */
#define IDC_LBCAND  400
#define IDC_BADC    401
#define IDC_LBCAS   402

/* Panel Vote */
#define IDC_BOVOTE  500
#define IDC_BCVOTE  501
#define IDC_BRPT    502
#define IDC_BSESSION 503

/* Panel RÃƒÂ©seau */
#define IDC_BNET    600
#define IDC_LBLOG   601
#define IDC_LNETST  602

/* Panel Comptes */
#define IDC_LBUSERS 700
#define IDC_BADUSER 701
#define IDC_BRSTPWD 702
#define IDC_BACTIV  703

/* Timers */
#define IDT_REFRESH 1001

/* Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â
 *  VARIABLES GLOBALES
 * Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â */
static HINSTANCE hInst;
static HWND      hMain;
static int       gWinW = DEF_WIN_W;
static int       gWinH = DEF_WIN_H;

/* GDI objects */
static HFONT  hFTitle, hFNorm, hFSub, hFBig, hFNav, hFMono;
static HBRUSH hBrBg, hBrSide, hBrHead, hBrCard, hBrAcce;
static HBRUSH hBrSucc, hBrDang, hBrWarn, hBrNSel, hBrBord;

/* App state */
static int  screenMode  = 0;  /* 0=login  1=main */
static int  activePanel = 0;  /* 0..5             */
static char adminUser[AUTH_MAX_USERNAME+1] = "";
static BOOL netRunning  = FALSE;

/* Dialog data (shared, one dialog at a time) */
typedef struct { char v[5][128]; BOOL ok; } DlgData;
static DlgData gDlg;

/* Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â
 *  LABELS NAVIGATION
 * Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â */
static const char *navLabels[] = {
    "   Tableau de bord",
    "   Electeurs",
    "   Candidats",
    "   Vote & Resultats",
    "   Mode Reseau",
    "   Comptes"
};
static const int navIDs[] = {
    IDC_NAV0, IDC_NAV1, IDC_NAV2, IDC_NAV3, IDC_NAV4, IDC_NAV5
};

/* Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â
 *  PROTOTYPES
 * Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â */
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK InputDlgProc(HWND, UINT, WPARAM, LPARAM);
static void InitGDI(void);
static void FreeGDI(void);
static void CreateLoginControls(HWND);
static void CreateMainControls(HWND);
static void LayoutLoginControls(HWND);
static void LayoutMainControls(HWND);
static void LayoutControls(HWND);
static void TransitionToMain(HWND);
static void ShowPanel(int idx);
static void DrawHeader(HDC, RECT*);
static void DrawSidebar(HDC, RECT*);
static void DrawStatusBar(HDC, RECT*);
static void DrawDashboard(HDC, RECT*);
static void DrawResultBars(HDC, RECT*);
static void DrawFilledRect(HDC, int, int, int, int, COLORREF);
static void DrawTextAt(HDC, const char*, int, int, int, int, COLORREF, HFONT, DWORD);
static void RefreshElecList(void);
static void RefreshCandList(void);
static void RefreshUserList(void);
static void AddLogLine(const char*);
static BOOL ShowInputDialog(HWND, const char*, int, const char**, const int*, char[][128]);
static void DoAddVoter(HWND);
static void DoAddCandidat(HWND);
static void DoOpenVote(HWND);
static void DoCloseVote(HWND);
static void DoCloseSession(HWND);
static void DoStartNet(HWND);
static void DoAddUser(HWND);
static void DoResetPwd(HWND);
static void DoToggleActive(HWND);
static void DoReport(HWND);
static BOOL DoLogin(HWND);
static void FormatVoteRemaining(char*, size_t);

/* Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â
 *  GDI Ã¢â‚¬â€ CREATION / LIBERATION
 * Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â */
static void InitGDI(void)
{
    /* Fonts (Segoe UI disponible sur Windows Vista+) */
    hFTitle = CreateFont(-28,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
                DEFAULT_PITCH,"Segoe UI");
    hFNorm  = CreateFont(-18,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
                DEFAULT_PITCH,"Segoe UI");
    hFSub   = CreateFont(-16,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
                DEFAULT_PITCH,"Segoe UI");
    hFBig   = CreateFont(-44,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
                DEFAULT_PITCH,"Segoe UI");
    hFNav   = CreateFont(-18,0,0,0,FW_SEMIBOLD,0,0,0,DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
                DEFAULT_PITCH,"Segoe UI");
    hFMono  = CreateFont(-15,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
                FIXED_PITCH,"Consolas");

    /* Brushes */
    hBrBg   = CreateSolidBrush(C_BG);
    hBrSide = CreateSolidBrush(C_SIDE);
    hBrHead = CreateSolidBrush(C_HEAD);
    hBrCard = CreateSolidBrush(C_CARD);
    hBrAcce = CreateSolidBrush(C_ACCE);
    hBrSucc = CreateSolidBrush(C_SUCC);
    hBrDang = CreateSolidBrush(C_DANG);
    hBrWarn = CreateSolidBrush(C_WARN);
    hBrNSel = CreateSolidBrush(C_NSEL);
    hBrBord = CreateSolidBrush(C_BORD);
}

static void FreeGDI(void)
{
    DeleteObject(hFTitle); DeleteObject(hFNorm);
    DeleteObject(hFSub);   DeleteObject(hFBig);
    DeleteObject(hFNav);   DeleteObject(hFMono);
    DeleteObject(hBrBg);   DeleteObject(hBrSide);
    DeleteObject(hBrHead); DeleteObject(hBrCard);
    DeleteObject(hBrAcce); DeleteObject(hBrSucc);
    DeleteObject(hBrDang); DeleteObject(hBrWarn);
    DeleteObject(hBrNSel); DeleteObject(hBrBord);
}

/* Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â
 *  HELPERS DESSIN
 * Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â */
static void DrawFilledRect(HDC hdc, int x, int y, int w, int h, COLORREF c)
{
    RECT r = { x, y, x+w, y+h };
    HBRUSH br = CreateSolidBrush(c);
    FillRect(hdc, &r, br);
    DeleteObject(br);
}

static void DrawTextAt(HDC hdc, const char *txt, int x, int y, int w, int h,
                       COLORREF col, HFONT font, DWORD fmt)
{
    HFONT old = (HFONT)SelectObject(hdc, font);
    SetTextColor(hdc, col);
    SetBkMode(hdc, TRANSPARENT);
    RECT r = { x, y, x+w, y+h };
    DrawText(hdc, txt, -1, &r, fmt);
    SelectObject(hdc, old);
}

static void FormatVoteRemaining(char *buf, size_t size)
{
    if (!voteOuvert) {
        snprintf(buf, size, "Vote ferme");
        return;
    }
    if (voteDeadline > 0) {
        int remaining = getVoteTimeRemainingSeconds();
        if (remaining <= 0) {
            snprintf(buf, size, "Votant : 00:00");
            return;
        }
        snprintf(buf, size, "Votant : %02d:%02d",
                 remaining / 60,
                 remaining % 60);
        return;
    }
    if (voteDurationSeconds > 0) {
        snprintf(buf, size, "Par votant : %02d:%02d",
                 voteDurationSeconds / 60,
                 voteDurationSeconds % 60);
        return;
    }
    snprintf(buf, size, "En attente d'un votant");
}

static void LayoutLoginControls(HWND hWnd)
{
    int cx = WIN_W / 2 - 230;
    int cy = WIN_H / 2 - 180;

    MoveWindow(GetDlgItem(hWnd, IDC_LUSER),  cx, cy + 78, 220, 24, TRUE);
    MoveWindow(GetDlgItem(hWnd, IDC_EUSER),  cx, cy + 108, 460, 40, TRUE);
    MoveWindow(GetDlgItem(hWnd, IDC_LPASS),  cx, cy + 164, 220, 24, TRUE);
    MoveWindow(GetDlgItem(hWnd, IDC_EPASS),  cx, cy + 194, 460, 40, TRUE);
    MoveWindow(GetDlgItem(hWnd, IDC_BLOGIN), cx, cy + 258, 460, 46, TRUE);
    MoveWindow(GetDlgItem(hWnd, IDC_LERR),   cx, cy + 318, 460, 28, TRUE);
}

static void LayoutMainControls(HWND hWnd)
{
    int navY = HEAD_H + 56;
    int navStep = 68;
    int navH = 58;
    int pad = 24;
    int listY = CONT_Y + 82;
    int buttonH = 48;
    int buttonY = CONT_Y + CONT_H - buttonH - 18;
    int listH = buttonY - listY - 18;
    int fullW = CONT_W - 2 * pad;

    for (int i = 0; i < 6; i++) {
        HWND hNav = GetDlgItem(hWnd, navIDs[i]);
        if (hNav) MoveWindow(hNav, 0, navY + i * navStep, SIDE_W, navH, TRUE);
    }
    if (GetDlgItem(hWnd, IDC_BQUIT))
        MoveWindow(GetDlgItem(hWnd, IDC_BQUIT), 0, WIN_H - STAT_H - 64, SIDE_W, 52, TRUE);

    if (GetDlgItem(hWnd, IDC_LBELEC))
        MoveWindow(GetDlgItem(hWnd, IDC_LBELEC), CONT_X + pad, listY, fullW, listH, TRUE);
    if (GetDlgItem(hWnd, IDC_BADE))
        MoveWindow(GetDlgItem(hWnd, IDC_BADE), CONT_X + pad, buttonY, 320, buttonH, TRUE);

    if (GetDlgItem(hWnd, IDC_LBCAND))
        MoveWindow(GetDlgItem(hWnd, IDC_LBCAND), CONT_X + pad, listY, fullW, listH, TRUE);
    if (GetDlgItem(hWnd, IDC_BADC))
        MoveWindow(GetDlgItem(hWnd, IDC_BADC), CONT_X + pad, buttonY, 320, buttonH, TRUE);

    {
        int gap = 12;
        int btnW = (fullW - 3 * gap) / 4;
        if (GetDlgItem(hWnd, IDC_BOVOTE))
            MoveWindow(GetDlgItem(hWnd, IDC_BOVOTE), CONT_X + pad, buttonY, btnW, buttonH, TRUE);
        if (GetDlgItem(hWnd, IDC_BCVOTE))
            MoveWindow(GetDlgItem(hWnd, IDC_BCVOTE), CONT_X + pad + btnW + gap, buttonY, btnW, buttonH, TRUE);
        if (GetDlgItem(hWnd, IDC_BSESSION))
            MoveWindow(GetDlgItem(hWnd, IDC_BSESSION), CONT_X + pad + 2 * (btnW + gap), buttonY, btnW, buttonH, TRUE);
        if (GetDlgItem(hWnd, IDC_BRPT))
            MoveWindow(GetDlgItem(hWnd, IDC_BRPT), CONT_X + pad + 3 * (btnW + gap), buttonY, btnW, buttonH, TRUE);
    }

    if (GetDlgItem(hWnd, IDC_LBLOG))
        MoveWindow(GetDlgItem(hWnd, IDC_LBLOG), CONT_X + pad, CONT_Y + 108, fullW, buttonY - (CONT_Y + 108) - 18, TRUE);
    if (GetDlgItem(hWnd, IDC_BNET))
        MoveWindow(GetDlgItem(hWnd, IDC_BNET), CONT_X + pad, buttonY, 410, buttonH, TRUE);

    if (GetDlgItem(hWnd, IDC_LBUSERS))
        MoveWindow(GetDlgItem(hWnd, IDC_LBUSERS), CONT_X + pad, listY, fullW, listH, TRUE);
    {
        int gap = 12;
        int btnW = (fullW - 2 * gap) / 3;
        if (GetDlgItem(hWnd, IDC_BADUSER))
            MoveWindow(GetDlgItem(hWnd, IDC_BADUSER), CONT_X + pad, buttonY, btnW, buttonH, TRUE);
        if (GetDlgItem(hWnd, IDC_BRSTPWD))
            MoveWindow(GetDlgItem(hWnd, IDC_BRSTPWD), CONT_X + pad + btnW + gap, buttonY, btnW, buttonH, TRUE);
        if (GetDlgItem(hWnd, IDC_BACTIV))
            MoveWindow(GetDlgItem(hWnd, IDC_BACTIV), CONT_X + pad + 2 * (btnW + gap), buttonY, btnW, buttonH, TRUE);
    }
}

static void LayoutControls(HWND hWnd)
{
    LayoutLoginControls(hWnd);
    LayoutMainControls(hWnd);
}

/* Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â
 *  DESSIN Ã¢â‚¬â€ HEADER
 * Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â */
static void DrawHeader(HDC hdc, RECT *rc)
{
    FillRect(hdc, rc, hBrHead);

    /* Titre */
    DrawTextAt(hdc, "PIVOTE", rc->left+24, rc->top+10, 220, 34,
               C_ACCE, hFTitle, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
    DrawTextAt(hdc, "Espace Administrateur", rc->left+24, rc->top+46, 320, 22,
               C_SUBT, hFSub, DT_LEFT|DT_SINGLELINE);

    if (screenMode == 1) {
        /* Admin name (right side) */
        char buf[80];
        sprintf(buf, "Admin : %s", adminUser);
        DrawTextAt(hdc, buf, rc->right-380, rc->top+12, 360, 24,
                   C_TEXT, hFNorm, DT_RIGHT|DT_SINGLELINE);

        COLORREF c = voteOuvert ? C_SUCC : C_DANG;
        const char *lbl = voteOuvert ? "  VOTE OUVERT  " : "  VOTE FERME  ";
        int bw = 170, bh = 30;
        int bx = rc->right - bw - 18;
        int by = rc->top + 38;
        DrawFilledRect(hdc, bx, by, bw, bh, c);
        DrawTextAt(hdc, lbl, bx, by, bw, bh,
                   C_WHITE, hFNav, DT_CENTER|DT_VCENTER|DT_SINGLELINE);

        if (voteOuvert) {
            char tbuf[64];
            FormatVoteRemaining(tbuf, sizeof(tbuf));
            DrawFilledRect(hdc, bx - 252, by - 1, 236, bh + 2, RGB(15,25,50));
            DrawTextAt(hdc, tbuf, bx - 252, by, 236, bh,
                       C_ACCE, hFNav, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        }
    }

    /* Bottom border */
    DrawFilledRect(hdc, rc->left, rc->bottom-2, rc->right-rc->left, 2, C_ACCE);
}

/* Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â
 *  DESSIN Ã¢â‚¬â€ SIDEBAR
 * Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â */
static void DrawSidebar(HDC hdc, RECT *rc)
{
    FillRect(hdc, rc, hBrSide);

    /* Right border */
    DrawFilledRect(hdc, rc->right-1, rc->top, 1, rc->bottom-rc->top, C_BORD);

    /* PIVOTE logo area */
    DrawFilledRect(hdc, rc->left, rc->top, rc->right-rc->left, 8, C_ACCE);

    /* "MENU" label */
    DrawTextAt(hdc, "NAVIGATION", rc->left+12, rc->top+18, 180, 16,
               C_SUBT, hFSub, DT_LEFT|DT_SINGLELINE);

    /* Separator after label */
    DrawFilledRect(hdc, rc->left+12, rc->top+36, rc->right-rc->left-24, 1, C_BORD);
}

/* Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â
 *  DESSIN Ã¢â‚¬â€ STATUS BAR
 * Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â */
static void DrawStatusBar(HDC hdc, RECT *rc)
{
    DrawFilledRect(hdc, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top, C_BG);
    DrawFilledRect(hdc, rc->left, rc->top, rc->right-rc->left, 1, C_BORD);

    char buf[256];
    char tbuf[64];
    int v = 0;
    for (int i = 0; i < nbElecteurs; i++) if (electeurs[i].a_vote) v++;
    FormatVoteRemaining(tbuf, sizeof(tbuf));
    sprintf(buf,
            "  Electeurs inscrits: %d   Candidats: %d   Votes exprimes: %d   Statut: %s   Reseau: %s   Temps: %s",
            nbElecteurs, nbCandidats, v,
            voteOuvert ? "OUVERT" : "FERME",
            netRunning ? "ACTIF" : "ARRETE",
            voteOuvert ? tbuf : "n/a");
    DrawTextAt(hdc, buf, rc->left + 8, rc->top + 7, rc->right-rc->left - 16, 18,
               C_SUBT, hFSub, DT_LEFT|DT_SINGLELINE);
}

/* Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â
 *  DESSIN Ã¢â‚¬â€ DASHBOARD (4 cartes)
 * Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â */
static void DrawDashboard(HDC hdc, RECT *rc)
{
    DrawFilledRect(hdc, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top, C_BG);

    int cx = rc->left + 24, cy = rc->top + 24;
    int gap = 22;
    int cw = (CONT_W - 2 * 24 - gap) / 2;
    int ch = 136;

    /* Helper: draw one stat card */
    #define DRAW_CARD(X, Y, LABEL, VAL, SUB, COL) do { \
        DrawFilledRect(hdc, X, Y, cw, ch, C_CARD); \
        DrawFilledRect(hdc, X, Y, 4, ch, COL); \
        DrawTextAt(hdc, LABEL, X+18, Y+14, cw-24, 20, C_SUBT, hFSub, DT_LEFT|DT_SINGLELINE); \
        DrawTextAt(hdc, VAL,   X+18, Y+44, cw-24, 48, COL,    hFBig, DT_LEFT|DT_SINGLELINE); \
        DrawTextAt(hdc, SUB,   X+18, Y+100, cw-24, 22, C_TEXT, hFSub, DT_LEFT|DT_SINGLELINE); \
    } while(0)

    /* Card 1 Ã¢â‚¬â€ Electeurs */
    char sNbE[32], sVoted[64];
    sprintf(sNbE, "%d", nbElecteurs);
    int nVoted = 0;
    for (int i = 0; i < nbElecteurs; i++) if (electeurs[i].a_vote) nVoted++;
    sprintf(sVoted, "%d ont deja vote", nVoted);
    DRAW_CARD(cx, cy, "ELECTEURS INSCRITS", sNbE, sVoted, C_ACCE);

    /* Card 2 Ã¢â‚¬â€ Candidats */
    char sNbC[32];
    sprintf(sNbC, "%d", nbCandidats);
    DRAW_CARD(cx + cw + gap, cy, "CANDIDATS EN LICE", sNbC, "en competition", C_WARN);

    /* Card 3 Ã¢â‚¬â€ Participation */
    char sPct[32], sPctSub[64];
    double pct = (nbElecteurs > 0) ? 100.0 * nVoted / nbElecteurs : 0.0;
    sprintf(sPct, "%.1f%%", pct);
    sprintf(sPctSub, "%d votes exprimes", nVoted);
    DRAW_CARD(cx, cy + ch + gap, "PARTICIPATION", sPct, sPctSub, C_SUCC);

    /* Card 4 Ã¢â‚¬â€ Statut */
    const char *statVal = voteOuvert ? "OUVERT" : "FERME";
    COLORREF    statCol = voteOuvert ? C_SUCC : C_DANG;
    DRAW_CARD(cx + cw + gap, cy + ch + gap, "STATUT DU VOTE", statVal,
              voteOuvert ? "Votes acceptes" : "Vote non demarre", statCol);

    #undef DRAW_CARD

    /* Buttons zone */
    int by2 = cy + 2*(ch+gap) + 20;
    DrawTextAt(hdc, "Actions rapides :", cx, by2, 300, 18,
               C_SUBT, hFSub, DT_LEFT|DT_SINGLELINE);
}

/* Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â
 *  DESSIN Ã¢â‚¬â€ BARRES DE RESULTATS
 * Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â */
static void DrawResultBars(HDC hdc, RECT *rc)
{
    DrawFilledRect(hdc, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top, C_BG);

    DrawTextAt(hdc, "RESULTATS EN DIRECT", rc->left + 24, rc->top + 12, 420, 28,
               C_ACCE, hFTitle, DT_LEFT|DT_SINGLELINE);

    int total = 0;
    int blancs = 0;
    int bx = rc->left + 24;
    int labelW = 220;
    int valueW = 180;
    int maxBarW = CONT_W - labelW - valueW - 80;
    int y = rc->top + 64;
    int rowH = 60;

    for (int i = 0; i < nbCandidats; i++) total += candidats[i].voix;
    for (int i = 0; i < nbElecteurs; i++) if (electeurs[i].vote_blanc) blancs++;
    total += blancs;

    static const COLORREF palette[] = {
        RGB(106,191,255), RGB(76,212,112), RGB(255,195,74),
        RGB(255,102,96), RGB(163,113,247), RGB(64,200,201)
    };

    for (int i = 0; i < nbCandidats && i < 6; i++) {
        double pct = (total > 0) ? 100.0 * candidats[i].voix / total : 0.0;
        int bw = (total > 0) ? (int)(maxBarW * candidats[i].voix / total) : 0;
        COLORREF col = palette[i % 6];

        DrawTextAt(hdc, candidats[i].nom, bx, y, labelW - 10, 22,
                   C_TEXT, hFNorm, DT_LEFT|DT_SINGLELINE);
        DrawFilledRect(hdc, bx + labelW, y + 3, maxBarW, 24, C_CARD);

        if (bw > 0) {
            DrawFilledRect(hdc, bx + labelW, y + 3, bw, 24, col);
            DrawFilledRect(hdc, bx + labelW, y + 3, bw, 4, C_WHITE);
        }

        {
            char num[48];
            sprintf(num, "%d voix (%.1f%%)", candidats[i].voix, pct);
            DrawTextAt(hdc, num, bx + labelW + maxBarW + 12, y, valueW, 22,
                       col, hFNorm, DT_LEFT|DT_SINGLELINE);
        }

        y += rowH;
        if (y + rowH > rc->bottom - 62) break;
    }

    if (nbElecteurs > 0) {
        double pctB = (total > 0) ? 100.0 * blancs / total : 0.0;
        int bwB = (total > 0) ? (int)(maxBarW * blancs / total) : 0;
        DrawTextAt(hdc, "VOTE BLANC", bx, y, labelW - 10, 22,
                   C_SUBT, hFNorm, DT_LEFT|DT_SINGLELINE);
        DrawFilledRect(hdc, bx + labelW, y + 3, maxBarW, 24, C_CARD);
        if (bwB > 0) DrawFilledRect(hdc, bx + labelW, y + 3, bwB, 24, C_SUBT);
        {
            char numB[48];
            sprintf(numB, "%d (%.1f%%)", blancs, pctB);
            DrawTextAt(hdc, numB, bx + labelW + maxBarW + 12, y, valueW, 22,
                       C_SUBT, hFNorm, DT_LEFT|DT_SINGLELINE);
        }
    }

    if (!voteOuvert && total > 0) {
        int maxV = 0;
        int winners[16];
        int winnerCount = 0;

        for (int i = 0; i < nbCandidats; i++)
            if (candidats[i].voix > maxV) maxV = candidats[i].voix;
        for (int i = 0; i < nbCandidats && winnerCount < 16; i++)
            if (candidats[i].voix == maxV) winners[winnerCount++] = i;

        if (winnerCount > 0 && maxV > 0) {
            char gbuf[256] = "";
            int gy = rc->bottom - 58;
            COLORREF banner = (winnerCount > 1) ? C_WARN : C_SUCC;

            if (winnerCount == 1) {
                snprintf(gbuf, sizeof(gbuf), "  GAGNANT : %s  (%d voix)  ",
                         candidats[winners[0]].nom, maxV);
            } else {
                strcpy(gbuf, "  EGALITE : ");
                for (int i = 0; i < winnerCount; i++) {
                    if (i > 0) strcat(gbuf, " / ");
                    strcat(gbuf, candidats[winners[i]].nom);
                }
                {
                    char suffix[32];
                    snprintf(suffix, sizeof(suffix), "  (%d voix)  ", maxV);
                    strcat(gbuf, suffix);
                }
            }

            DrawFilledRect(hdc, bx, gy, CONT_W - 48, 36, banner);
            DrawTextAt(hdc, gbuf, bx, gy, CONT_W - 48, 36,
                       C_BG, hFNav, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        }
    }
}
static void CreateLoginControls(HWND hWnd)
{
    CreateWindow("STATIC","Identifiant :",WS_CHILD|SS_LEFT,
                 0,0,120,20,hWnd,(HMENU)IDC_LUSER,hInst,NULL);
    CreateWindow("EDIT","",WS_CHILD|WS_BORDER|ES_AUTOHSCROLL,
                 0,0,220,32,hWnd,(HMENU)IDC_EUSER,hInst,NULL);

    CreateWindow("STATIC","Mot de passe :",WS_CHILD|SS_LEFT,
                 0,0,120,20,hWnd,(HMENU)IDC_LPASS,hInst,NULL);
    CreateWindow("EDIT","",WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|ES_PASSWORD,
                 0,0,220,32,hWnd,(HMENU)IDC_EPASS,hInst,NULL);

    CreateWindow("BUTTON","  CONNEXION",WS_CHILD|BS_OWNERDRAW,
                 0,0,220,42,hWnd,(HMENU)IDC_BLOGIN,hInst,NULL);

    CreateWindow("STATIC","",WS_CHILD|SS_CENTER,
                 0,0,220,24,hWnd,(HMENU)IDC_LERR,hInst,NULL);

    /* Show controls */
    for (int i = IDC_EUSER; i <= IDC_LPASS; i++) {
        HWND h = GetDlgItem(hWnd, i);
        if (h) ShowWindow(h, SW_SHOW);
    }

    LayoutLoginControls(hWnd);
}

/* Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â
 *  CREATION DES CONTROLES Ã¢â‚¬â€ MAIN
 * Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â */
static void CreateMainControls(HWND hWnd)
{
    for (int i = 0; i < 6; i++) {
        CreateWindow("BUTTON", navLabels[i],
                     WS_CHILD|BS_OWNERDRAW,
                     0, 0, SIDE_W, 46,
                     hWnd, (HMENU)(UINT_PTR)navIDs[i], hInst, NULL);
    }
    CreateWindow("BUTTON","   QUITTER",WS_CHILD|BS_OWNERDRAW,
                 0, 0, SIDE_W, 46,
                 hWnd,(HMENU)IDC_BQUIT,hInst,NULL);

    CreateWindow("LISTBOX","",WS_CHILD|WS_VSCROLL|LBS_NOTIFY|LBS_USETABSTOPS|LBS_NOINTEGRALHEIGHT,
                 0,0,200,120,
                 hWnd,(HMENU)IDC_LBELEC,hInst,NULL);
    CreateWindow("BUTTON","  + Ajouter un electeur",WS_CHILD|BS_OWNERDRAW,
                 0,0,280,40,
                 hWnd,(HMENU)IDC_BADE,hInst,NULL);

    CreateWindow("LISTBOX","",WS_CHILD|WS_VSCROLL|LBS_NOTIFY|LBS_NOINTEGRALHEIGHT,
                 0,0,200,120,
                 hWnd,(HMENU)IDC_LBCAND,hInst,NULL);
    CreateWindow("BUTTON","  + Ajouter un candidat",WS_CHILD|BS_OWNERDRAW,
                 0,0,280,40,
                 hWnd,(HMENU)IDC_BADC,hInst,NULL);

    CreateWindow("BUTTON","  OUVRIR LE VOTE",WS_CHILD|BS_OWNERDRAW,
                 0,0,200,40,
                 hWnd,(HMENU)IDC_BOVOTE,hInst,NULL);
    CreateWindow("BUTTON","  FERMER LE VOTE",WS_CHILD|BS_OWNERDRAW,
                 0,0,200,40,
                 hWnd,(HMENU)IDC_BCVOTE,hInst,NULL);
    CreateWindow("BUTTON","  FERMER LA SESSION",WS_CHILD|BS_OWNERDRAW,
                 0,0,220,40,
                 hWnd,(HMENU)IDC_BSESSION,hInst,NULL);
    CreateWindow("BUTTON","  Generer rapport",WS_CHILD|BS_OWNERDRAW,
                 0,0,180,40,
                 hWnd,(HMENU)IDC_BRPT,hInst,NULL);

    CreateWindow("LISTBOX","",WS_CHILD|WS_VSCROLL|LBS_NOTIFY|LBS_NOINTEGRALHEIGHT,
                 0,0,200,120,
                 hWnd,(HMENU)IDC_LBLOG,hInst,NULL);
    CreateWindow("BUTTON","  LANCER LE SERVEUR RESEAU",WS_CHILD|BS_OWNERDRAW,
                 0,0,300,40,
                 hWnd,(HMENU)IDC_BNET,hInst,NULL);

    CreateWindow("LISTBOX","",WS_CHILD|WS_VSCROLL|LBS_NOTIFY|LBS_NOINTEGRALHEIGHT,
                 0,0,200,120,
                 hWnd,(HMENU)IDC_LBUSERS,hInst,NULL);
    CreateWindow("BUTTON","  + Creer compte",WS_CHILD|BS_OWNERDRAW,
                 0,0,180,40,
                 hWnd,(HMENU)IDC_BADUSER,hInst,NULL);
    CreateWindow("BUTTON","  Reset mdp",WS_CHILD|BS_OWNERDRAW,
                 0,0,150,40,
                 hWnd,(HMENU)IDC_BRSTPWD,hInst,NULL);
    CreateWindow("BUTTON","  Activer/Desactiver",WS_CHILD|BS_OWNERDRAW,
                 0,0,180,40,
                 hWnd,(HMENU)IDC_BACTIV,hInst,NULL);

    /* Apply font to all EDIT and LISTBOX */
    //EnumChildWindows(hWnd, NULL, 0);  /* placeholder */
    SendMessage(GetDlgItem(hWnd, IDC_LBELEC),  LB_SETITEMHEIGHT, 0, 28);
    SendMessage(GetDlgItem(hWnd, IDC_LBCAND),  LB_SETITEMHEIGHT, 0, 28);
    SendMessage(GetDlgItem(hWnd, IDC_LBLOG),   LB_SETITEMHEIGHT, 0, 22);
    SendMessage(GetDlgItem(hWnd, IDC_LBUSERS), LB_SETITEMHEIGHT, 0, 28);
    SendMessage(GetDlgItem(hWnd, IDC_LBELEC),  WM_SETFONT, (WPARAM)hFNorm, TRUE);
    SendMessage(GetDlgItem(hWnd, IDC_LBCAND),  WM_SETFONT, (WPARAM)hFNorm, TRUE);
    SendMessage(GetDlgItem(hWnd, IDC_LBLOG),   WM_SETFONT, (WPARAM)hFMono, TRUE);
    SendMessage(GetDlgItem(hWnd, IDC_LBUSERS), WM_SETFONT, (WPARAM)hFNorm, TRUE);
    LayoutMainControls(hWnd);
}

/* Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â
 *  TRANSITION LOGIN Ã¢â€ â€™ MAIN
 * Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â */
static void TransitionToMain(HWND hWnd)
{
    /* Cache les contrÃƒÂ´les de login */
    for (int i = IDC_EUSER; i <= IDC_LPASS; i++) {
        HWND h = GetDlgItem(hWnd, i);
        if (h) { ShowWindow(h, SW_HIDE); EnableWindow(h, FALSE); }
    }

    /* Affiche les contrÃƒÂ´les principaux */
    for (int i = IDC_NAV0; i <= IDC_BQUIT; i++) {
        HWND h = GetDlgItem(hWnd, i);
        if (h) ShowWindow(h, SW_SHOW);
    }

    screenMode = 1;
    activePanel = 0;
    LayoutControls(hWnd);
    ShowPanel(0);
    SetTimer(hWnd, IDT_REFRESH, 1000, NULL);
    InvalidateRect(hWnd, NULL, TRUE);
}

/* Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â
 *  SHOW PANEL Ã¢â‚¬â€ masque tout, affiche le bon groupe
 * Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â */
static void ShowPanel(int idx)
{
    /* IDs de chaque panel */
    static const int panelIDs[][8] = {
        { 0 },                                                          /* 0: dashboard (dessin) */
        { IDC_LBELEC, IDC_BADE,    0 },                                /* 1: electeurs */
        { IDC_LBCAND, IDC_BADC,    0 },                                /* 2: candidats */
        { IDC_BOVOTE, IDC_BCVOTE, IDC_BSESSION, IDC_BRPT, 0 },        /* 3: vote (dessin+boutons) */
        { IDC_LBLOG,  IDC_BNET,    0 },                                /* 4: reseau */
        { IDC_LBUSERS,IDC_BADUSER, IDC_BRSTPWD, IDC_BACTIV, 0 },      /* 5: comptes */
    };
    static const int allIDs[] = {
        IDC_LBELEC, IDC_BADE,
        IDC_LBCAND, IDC_BADC,
        IDC_BOVOTE, IDC_BCVOTE, IDC_BSESSION, IDC_BRPT,
        IDC_LBLOG,  IDC_BNET,
        IDC_LBUSERS,IDC_BADUSER,IDC_BRSTPWD,IDC_BACTIV,
        0
    };

    /* Masque tout */
    for (int i = 0; allIDs[i]; i++) {
        HWND h = GetDlgItem(hMain, allIDs[i]);
        if (h) ShowWindow(h, SW_HIDE);
    }

    activePanel = idx;

    /* Affiche le bon panel */
    for (int i = 0; panelIDs[idx][i]; i++) {
        HWND h = GetDlgItem(hMain, panelIDs[idx][i]);
        if (h) ShowWindow(h, SW_SHOW);
    }

    /* Rafraichit les listes */
    if (idx == 1) RefreshElecList();
    if (idx == 2) RefreshCandList();
    if (idx == 5) RefreshUserList();

    /* Invalide la zone de contenu pour redessiner barres/dashboard */
    RECT rc = { CONT_X, CONT_Y, WIN_W, WIN_H-STAT_H };
    InvalidateRect(hMain, &rc, TRUE);

    /* Invalide sidebar pour surligner le bon bouton */
    RECT rs = { 0, 0, SIDE_W, WIN_H };
    InvalidateRect(hMain, &rs, TRUE);
}

/* Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â
 *  RAFRAICHISSEMENT DES LISTES
 * Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â */
static void RefreshElecList(void)
{
    HWND hLB = GetDlgItem(hMain, IDC_LBELEC);
    if (!hLB) return;
    SendMessage(hLB, LB_RESETCONTENT, 0, 0);
    for (int i = 0; i < nbElecteurs; i++) {
        char buf[100];
        sprintf(buf, "  ID:%-4d  %-25s  Login:%-15s  Vote:%s",
                electeurs[i].id, electeurs[i].nom, electeurs[i].username,
                electeurs[i].a_vote ? "OUI" : "NON");
        SendMessage(hLB, LB_ADDSTRING, 0, (LPARAM)buf);
    }
}

static void RefreshCandList(void)
{
    HWND hLB = GetDlgItem(hMain, IDC_LBCAND);
    if (!hLB) return;
    SendMessage(hLB, LB_RESETCONTENT, 0, 0);
    for (int i = 0; i < nbCandidats; i++) {
        char buf[100];
        sprintf(buf, "  ID:%-4d  %-25s  Voix: %d",
                candidats[i].id, candidats[i].nom, candidats[i].voix);
        SendMessage(hLB, LB_ADDSTRING, 0, (LPARAM)buf);
    }
}

static void RefreshUserList(void)
{
    HWND hLB = GetDlgItem(hMain, IDC_LBUSERS);
    if (!hLB) return;
    SendMessage(hLB, LB_RESETCONTENT, 0, 0);
    AuthUser *users = NULL; size_t cnt = 0;
    if (auth_list_users(CSV_PATH, &users, &cnt) == AUTH_OK) {
        for (size_t i = 0; i < cnt; i++) {
            char buf[120];
            sprintf(buf, "  %-20s  Role:%-8s  Actif:%s",
                    users[i].username, users[i].role,
                    users[i].active ? "OUI" : "NON");
            SendMessage(hLB, LB_ADDSTRING, 0, (LPARAM)buf);
        }
        auth_free_user_list(users);
    }
}

static void AddLogLine(const char *msg)
{
    HWND hLB = GetDlgItem(hMain, IDC_LBLOG);
    if (!hLB) return;
    char buf[200];
    SYSTEMTIME st; GetLocalTime(&st);
    sprintf(buf, "[%02d:%02d:%02d] %s", st.wHour, st.wMinute, st.wSecond, msg);
    int idx = (int)SendMessage(hLB, LB_ADDSTRING, 0, (LPARAM)buf);
    SendMessage(hLB, LB_SETCURSEL, idx, 0);
}

/* Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â
 *  INPUT DIALOG Ã¢â‚¬â€ gÃƒÂ©nÃƒÂ©rique (popup modal)
 * Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â */
typedef struct {
    const char *labels[5];
    int         isPwd[5];
    int         nFields;
    HWND        hEdits[5];
} InputDlgCtx;
static InputDlgCtx gCtx;

LRESULT CALLBACK InputDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    (void)lp;
    switch(msg) {
        case WM_CREATE: {
            RECT cr; GetClientRect(hDlg, &cr);
            int cw = cr.right, y = 16;
            SendMessage(hDlg, WM_SETFONT, (WPARAM)hFNorm, FALSE);
            for (int i = 0; i < gCtx.nFields; i++) {
                HWND hLbl = CreateWindow("STATIC", gCtx.labels[i],
                    WS_CHILD|WS_VISIBLE|SS_LEFT, 16, y, cw-32, 20,
                    hDlg, NULL, hInst, NULL);
                SendMessage(hLbl, WM_SETFONT, (WPARAM)hFSub, TRUE);
                y += 24;
                DWORD es = WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL;
                if (gCtx.isPwd[i]) es |= ES_PASSWORD;
                gCtx.hEdits[i] = CreateWindow("EDIT","",es,
                    16,y,cw-32,36,hDlg,(HMENU)(UINT_PTR)(900+i),hInst,NULL);
                SendMessage(gCtx.hEdits[i], WM_SETFONT, (WPARAM)hFNorm, TRUE);
                y += 44;
            }
            y += 8;
            HWND hOK  = CreateWindow("BUTTON","OK",WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
                            cw-196,y,90,34,hDlg,(HMENU)IDOK,hInst,NULL);
            HWND hCan = CreateWindow("BUTTON","Annuler",WS_CHILD|WS_VISIBLE,
                            cw-98,y,90,34,hDlg,(HMENU)IDCANCEL,hInst,NULL);
            SendMessage(hOK,  WM_SETFONT, (WPARAM)hFNorm, TRUE);
            SendMessage(hCan, WM_SETFONT, (WPARAM)hFNorm, TRUE);
            return 0;
        }
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLOREDIT: {
            HDC hdc = (HDC)wp;
            SetTextColor(hdc, C_TEXT);
            SetBkColor(hdc, C_CARD);
            return (LRESULT)hBrCard;
        }
        case WM_CTLCOLORBTN: {
            HDC hdc = (HDC)wp;
            SetBkColor(hdc, C_CARD);
            return (LRESULT)hBrCard;
        }
        case WM_ERASEBKGND: {
            RECT r; GetClientRect(hDlg, &r);
            FillRect((HDC)wp, &r, hBrCard);
            return 1;
        }
        case WM_COMMAND: {
            int id = LOWORD(wp);
            if (id == IDOK) {
                for (int i = 0; i < gCtx.nFields; i++)
                    GetWindowText(gCtx.hEdits[i], gDlg.v[i], 127);
                gDlg.ok = TRUE; DestroyWindow(hDlg);
            } else if (id == IDCANCEL) {
                gDlg.ok = FALSE; DestroyWindow(hDlg);
            }
            return 0;
        }
        case WM_KEYDOWN:
            if (wp == VK_ESCAPE) { gDlg.ok = FALSE; DestroyWindow(hDlg); }
            break;
    }
    return DefWindowProc(hDlg, msg, wp, lp);
}

static BOOL ShowInputDialog(HWND hParent, const char *title,
                             int nFields, const char **labels,
                             const int *isPwd, char results[][128])
{
    static BOOL classOK = FALSE;
    if (!classOK) {
        WNDCLASS wc = {0};
        wc.lpfnWndProc   = InputDlgProc;
        wc.hInstance     = hInst;
        wc.lpszClassName = "PivDlg";
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = hBrCard;
        RegisterClass(&wc);
        classOK = TRUE;
    }

    gCtx.nFields = nFields;
    for (int i = 0; i < nFields; i++) {
        gCtx.labels[i] = labels[i];
        gCtx.isPwd[i]  = isPwd[i];
    }
    gDlg.ok = FALSE;

    int dw = 380, dh = 68 + nFields*68 + 56;
    RECT pr; GetWindowRect(hParent, &pr);
    int dx = pr.left + (pr.right-pr.left-dw)/2;
    int dy = pr.top  + (pr.bottom-pr.top-dh)/2;

    HWND hDlg = CreateWindowEx(WS_EX_DLGMODALFRAME|WS_EX_TOPMOST,
        "PivDlg", title,
        WS_POPUP|WS_CAPTION|WS_VISIBLE,
        dx, dy, dw, dh, hParent, NULL, hInst, NULL);
    if (!hDlg) return FALSE;

    EnableWindow(hParent, FALSE);
    MSG msg;
    while (IsWindow(hDlg)) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (!IsDialogMessage(hDlg, &msg)) { TranslateMessage(&msg); DispatchMessage(&msg); }
        } else WaitMessage();
    }
    EnableWindow(hParent, TRUE);
    SetForegroundWindow(hParent);

    if (gDlg.ok && results)
        for (int i = 0; i < nFields; i++)
            strncpy(results[i], gDlg.v[i], 127);

    return gDlg.ok;
}

/* Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â
 *  ACTIONS
 * Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â */
static BOOL DoLogin(HWND hWnd)
{
    char user[65], pass[65];
    GetWindowText(GetDlgItem(hWnd, IDC_EUSER), user, 64);
    GetWindowText(GetDlgItem(hWnd, IDC_EPASS), pass, 64);

    AuthUser *allU = NULL; size_t cnt = 0;
    int adminExists = 0;
    if (auth_list_users(CSV_PATH, &allU, &cnt) == AUTH_OK) {
        for (size_t i = 0; i < cnt; i++)
            if (strcmp(allU[i].role,"admin")==0) { adminExists = 1; break; }
        auth_free_user_list(allU);
    }

    if (!adminExists) {
        /* Premier lancement : crÃƒÂ©e le compte admin */
        const char *lbl[]  = {"Choisir un identifiant admin :", "Choisir un mot de passe :"};
        const int   pwd[]  = {0, 1};
        char res[2][128];
        if (!ShowInputDialog(hWnd, "Premiere utilisation Ã¢â‚¬â€ Creer l'admin", 2, lbl, pwd, res)) return FALSE;
        auth_register_user(CSV_PATH, res[0], res[1], "admin");
        MessageBox(hWnd, "Compte admin cree. Connectez-vous maintenant.", "PIVOTE", MB_OK|MB_ICONINFORMATION);
        return FALSE;
    }

    AuthUser au;
    AuthStatus st = auth_authenticate(CSV_PATH, user, pass, &au);
    if (st == AUTH_OK && strcmp(au.role,"admin")==0) {
        strncpy(adminUser, au.username, AUTH_MAX_USERNAME);
        chargerDonnees();
        return TRUE;
    }

    SetWindowText(GetDlgItem(hWnd, IDC_LERR),
                  " Identifiants incorrects ou role non-admin.");
    InvalidateRect(GetDlgItem(hWnd, IDC_LERR), NULL, TRUE);
    return FALSE;
}

static void DoAddVoter(HWND hWnd)
{
    if (nbElecteurs >= MAX) { MessageBox(hWnd,"Limite atteinte.","",MB_OK); return; }
    const char *lbl[] = {"ID numerique :","Nom complet :","Login (identifiant) :","Mot de passe :"};
    const int   pwd[] = {0,0,0,1};
    char res[4][128];
    if (!ShowInputDialog(hWnd,"Ajouter un electeur",4,lbl,pwd,res)) return;

    int id = atoi(res[0]);
    if (id <= 0) { MessageBox(hWnd,"ID invalide.","Erreur",MB_OK|MB_ICONERROR); return; }
    for (int i=0;i<nbElecteurs;i++)
        if (electeurs[i].id==id) { MessageBox(hWnd,"ID deja utilise.","Erreur",MB_OK|MB_ICONERROR); return; }

    AuthStatus st = auth_register_user(CSV_PATH, res[2], res[3], "votant");
    if (st==AUTH_ERR_EXISTS) { MessageBox(hWnd,"Login deja existant.","Erreur",MB_OK|MB_ICONERROR); return; }
    if (st!=AUTH_OK)          { MessageBox(hWnd,"Erreur creation compte.","Erreur",MB_OK|MB_ICONERROR); return; }

    Electeur e = {0};
    e.id = id;
    strncpy(e.nom, res[1], 49); e.nom[49]='\0';
    strncpy(e.username, res[2], AUTH_MAX_USERNAME); e.username[AUTH_MAX_USERNAME]='\0';
    electeurs[nbElecteurs++] = e;
    sauvegarderDonnees();
    RefreshElecList();
    InvalidateRect(hWnd, NULL, TRUE);
}

static void DoAddCandidat(HWND hWnd)
{
    if (nbCandidats >= MAX) { MessageBox(hWnd,"Limite atteinte.","",MB_OK); return; }
    const char *lbl[] = {"ID numerique :","Nom du candidat :"};
    const int   pwd[] = {0,0};
    char res[2][128];
    if (!ShowInputDialog(hWnd,"Ajouter un candidat",2,lbl,pwd,res)) return;

    int id = atoi(res[0]);
    if (id<=0) { MessageBox(hWnd,"ID invalide.","Erreur",MB_OK|MB_ICONERROR); return; }

    Candidat c = {0};
    c.id = id;
    strncpy(c.nom, res[1], 49); c.nom[49]='\0';
    candidats[nbCandidats++] = c;
    sauvegarderDonnees();
    RefreshCandList();
    InvalidateRect(hWnd, NULL, TRUE);
}

static void DoOpenVote(HWND hWnd)
{
    const char *lbl[] = {"Duree du vote (en minutes) :"};
    const int   pwd[] = {0};
    char res[1][128];
    int minutes, seconds;

    if (voteOuvert) {
        MessageBox(hWnd,"Le vote est deja ouvert.","Information",MB_OK|MB_ICONINFORMATION);
        return;
    }

    if (!ShowInputDialog(hWnd,"Ouvrir le vote",1,lbl,pwd,res)) return;
    minutes = atoi(res[0]);
    if (minutes <= 0) {
        MessageBox(hWnd,"Entrez une duree valide en minutes.","Erreur",MB_OK|MB_ICONERROR);
        return;
    }

    seconds = minutes * 60;
    ouvrirVoteAvecDuree(seconds);
    sauvegarderDonnees();
    RECT rc = {CONT_X, CONT_Y, WIN_W, WIN_H-STAT_H};
    InvalidateRect(hWnd, &rc, TRUE);
    RECT rs = {WIN_W-200, HEAD_H-35, WIN_W, HEAD_H};
    InvalidateRect(hWnd, &rs, TRUE);
    InvalidateRect(hWnd, NULL, TRUE);
    MessageBox(hWnd, "Vote ouvert avec succes.\nChaque votant recevra maintenant ce temps des sa connexion.",
               "PIVOTE", MB_OK|MB_ICONINFORMATION);
}

static void DoCloseVote(HWND hWnd)
{
    if (MessageBox(hWnd,"Fermer le vote definitivement ?","Confirmation",
                   MB_YESNO|MB_ICONQUESTION) != IDYES) return;
    fermerVote();
    sauvegarderDonnees();
    exporterVersExcel();
    InvalidateRect(hWnd, NULL, TRUE);
    MessageBox(hWnd, "Vote ferme. Rapport genere : rapport_final.txt",
               "PIVOTE", MB_OK|MB_ICONINFORMATION);
}

static void DoCloseSession(HWND hWnd)
{
    if (MessageBox(hWnd,
                   "Fermer la session courante, arreter le reseau et expulser tous les votants connectes ?",
                   "Confirmation",
                   MB_YESNO|MB_ICONWARNING) != IDYES) return;

    fermerSessionVote();
    netRunning = FALSE;
    AddLogLine("Session fermee par l'administrateur.");
    InvalidateRect(hWnd, NULL, TRUE);
    MessageBox(hWnd,
               "La session a ete fermee. Le vote est arrete et les votants encore connectes ont ete deconnectes.",
               "PIVOTE",
               MB_OK|MB_ICONINFORMATION);
}

static void DoStartNet(HWND hWnd)
{
    if (serveurReseauActif()) { MessageBox(hWnd,"Serveur reseau deja actif.","",MB_OK); return; }
    if (!demarrerServeurReseauAsync()) {
        MessageBox(hWnd,"Impossible de demarrer le serveur reseau.","Erreur",MB_OK|MB_ICONERROR);
        return;
    }
    netRunning = TRUE;
    AddLogLine("Serveur reseau demarre sur le port 8888.");
    AddLogLine("En attente de connexions clients...");
    MessageBox(hWnd,"Mode reseau actif sur le port 8888.\nLes votes des clients sont maintenant acceptes.","PIVOTE",MB_OK|MB_ICONINFORMATION);
}

static void DoAddUser(HWND hWnd)
{
    const char *lbl[] = {"Identifiant :","Mot de passe :","Role (admin/votant) :"};
    const int   pwd[] = {0,1,0};
    char res[3][128];
    if (!ShowInputDialog(hWnd,"Creer un compte",3,lbl,pwd,res)) return;
    AuthStatus st = auth_register_user(CSV_PATH,res[0],res[1],res[2]);
    if (st==AUTH_OK) MessageBox(hWnd,"Compte cree avec succes.","OK",MB_OK|MB_ICONINFORMATION);
    else if (st==AUTH_ERR_EXISTS) MessageBox(hWnd,"Login deja existant.","Erreur",MB_OK|MB_ICONERROR);
    else MessageBox(hWnd,"Erreur.","Erreur",MB_OK|MB_ICONERROR);
    RefreshUserList();
}

static void DoResetPwd(HWND hWnd)
{
    const char *lbl[] = {"Login de l'utilisateur :","Nouveau mot de passe :"};
    const int   pwd[] = {0,1};
    char res[2][128];
    if (!ShowInputDialog(hWnd,"Reinitialiser un mot de passe",2,lbl,pwd,res)) return;
    AuthStatus st = auth_change_password(CSV_PATH,res[0],NULL,res[1]);
    if (st==AUTH_OK) MessageBox(hWnd,"Mot de passe reinitialise.","OK",MB_OK|MB_ICONINFORMATION);
    else MessageBox(hWnd,"Utilisateur inconnu ou erreur.","Erreur",MB_OK|MB_ICONERROR);
}

static void DoToggleActive(HWND hWnd)
{
    const char *lbl[] = {"Login :", "Activer (1) ou Desactiver (0) :"};
    const int   pwd[] = {0,0};
    char res[2][128];
    if (!ShowInputDialog(hWnd,"Activer / Desactiver un compte",2,lbl,pwd,res)) return;
    int act = atoi(res[1]);
    AuthStatus st = auth_set_active(CSV_PATH, res[0], act);
    if (st==AUTH_OK) MessageBox(hWnd, act?"Compte active.":"Compte desactive.", "OK", MB_OK|MB_ICONINFORMATION);
    else MessageBox(hWnd,"Utilisateur inconnu.","Erreur",MB_OK|MB_ICONERROR);
    RefreshUserList();
}

static void DoReport(HWND hWnd)
{
    genererRapportFinal();
    exporterVersExcel();
    MessageBox(hWnd,"Rapport genere : rapport_final.txt\nCSV mis a jour : resultats_vote.csv",
               "PIVOTE", MB_OK|MB_ICONINFORMATION);
}

/* Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â
 *  DESSIN DES BOUTONS OWNER-DRAW
 * Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â */
static void DrawOwnerButton(DRAWITEMSTRUCT *dis)
{
    HDC   hdc = dis->hDC;
    RECT  r   = dis->rcItem;
    int   id  = dis->CtlID;
    BOOL  pressed = (dis->itemState & ODS_SELECTED) != 0;

    /* Couleur selon l'ID */
    COLORREF bg, fg;
    if (id >= IDC_NAV0 && id <= IDC_NAV5) {
        int navIdx = id - IDC_NAV0;
        if (navIdx == activePanel && screenMode == 1) {
            bg = C_NSEL; fg = C_ACCE;
        } else {
            bg = pressed ? C_NSEL : C_SIDE; fg = C_TEXT;
        }
        FillRect(hdc, &r, CreateSolidBrush(bg));
        /* Barre d'accent gauche si sÃƒÂ©lectionnÃƒÂ© */
        if (navIdx == activePanel && screenMode == 1) {
            DrawFilledRect(hdc, r.left, r.top, 3, r.bottom-r.top, C_ACCE);
        }
        DrawTextAt(hdc, navLabels[navIdx], r.left+12, r.top,
                   r.right-r.left, r.bottom-r.top, fg, hFNav,
                   DT_VCENTER|DT_SINGLELINE|DT_LEFT);
    } else if (id == IDC_BQUIT) {
        bg = pressed ? C_BRED : RGB(60,15,15); fg = C_DANG;
        FillRect(hdc, &r, CreateSolidBrush(bg));
        DrawTextAt(hdc,"   QUITTER",r.left,r.top,r.right-r.left,r.bottom-r.top,
                   fg,hFNav,DT_VCENTER|DT_SINGLELINE|DT_LEFT);
    } else if (id == IDC_BLOGIN) {
        bg = pressed ? C_BPRI : RGB(40,90,190); fg = C_WHITE;
        FillRect(hdc, &r, CreateSolidBrush(bg));
        DrawTextAt(hdc,"  SE CONNECTER",r.left,r.top,r.right-r.left,r.bottom-r.top,
                   fg,hFNav,DT_VCENTER|DT_CENTER|DT_SINGLELINE);
    } else if (id == IDC_BOVOTE || id == IDC_BNET) {
        bg = pressed ? C_BGRN : RGB(30,100,45); fg = C_WHITE;
        FillRect(hdc, &r, CreateSolidBrush(bg));
        char txt[64]; GetWindowText(dis->hwndItem, txt, 63);
        DrawTextAt(hdc,txt,r.left,r.top,r.right-r.left,r.bottom-r.top,
                   fg,hFNav,DT_VCENTER|DT_CENTER|DT_SINGLELINE);
    } else if (id == IDC_BCVOTE || id == IDC_BSESSION) {
        bg = pressed ? C_BRED : RGB(140,30,30); fg = C_WHITE;
        FillRect(hdc, &r, CreateSolidBrush(bg));
        char txt[64]; GetWindowText(dis->hwndItem, txt, 63);
        DrawTextAt(hdc,txt,r.left,r.top,r.right-r.left,r.bottom-r.top,
                   fg,hFNav,DT_VCENTER|DT_CENTER|DT_SINGLELINE);
    } else {
        /* Bouton gÃƒÂ©nÃƒÂ©rique */
        bg = pressed ? C_BPRI : RGB(40,55,80); fg = C_ACCE;
        FillRect(hdc, &r, CreateSolidBrush(bg));
        char txt[64]; GetWindowText(dis->hwndItem, txt, 63);
        DrawTextAt(hdc,txt,r.left,r.top,r.right-r.left,r.bottom-r.top,
                   fg,hFNorm,DT_VCENTER|DT_CENTER|DT_SINGLELINE);
    }
}

/* Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â
 *  PROCEDURE PRINCIPALE
 * Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â */
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch(msg) {
        /* Ã¢â€â‚¬Ã¢â€â‚¬ Creation Ã¢â€â‚¬Ã¢â€â‚¬ */
        case WM_CREATE:
            CreateLoginControls(hWnd);
            CreateMainControls(hWnd);
            /* Masque tous les contrÃƒÂ´les main au dÃƒÂ©part */
            for (int i = IDC_NAV0; i <= IDC_BACTIV; i++) {
                HWND h = GetDlgItem(hWnd, i);
                if (h) ShowWindow(h, SW_HIDE);
            }
            /* Applique la police aux contrÃƒÂ´les login */
            SendMessage(GetDlgItem(hWnd,IDC_LUSER),WM_SETFONT,(WPARAM)hFSub,FALSE);
            SendMessage(GetDlgItem(hWnd,IDC_EUSER),WM_SETFONT,(WPARAM)hFNorm,FALSE);
            SendMessage(GetDlgItem(hWnd,IDC_LPASS),WM_SETFONT,(WPARAM)hFSub,FALSE);
            SendMessage(GetDlgItem(hWnd,IDC_EPASS),WM_SETFONT,(WPARAM)hFNorm,FALSE);
            SendMessage(GetDlgItem(hWnd,IDC_LERR), WM_SETFONT,(WPARAM)hFSub, FALSE);
            LayoutControls(hWnd);
            return 0;

        /* Ã¢â€â‚¬Ã¢â€â‚¬ Effacement arriÃƒÂ¨re-plan Ã¢â€â‚¬Ã¢â€â‚¬ */
        case WM_ERASEBKGND: {
            RECT r; GetClientRect(hWnd, &r);
            FillRect((HDC)wp, &r, hBrBg);
            return 1;
        }

        /* Ã¢â€â‚¬Ã¢â€â‚¬ Dessin Ã¢â€â‚¬Ã¢â€â‚¬ */
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            /* Header */
            RECT rHead = {0, 0, WIN_W, HEAD_H};
            DrawHeader(hdc, &rHead);

            if (screenMode == 0) {
                /* Login Ã¢â‚¬â€ cadre central */
                int cx = WIN_W/2-250, cy = WIN_H/2-210;
                DrawFilledRect(hdc, cx-28, cy-28, 560, 420, C_CARD);
                DrawFilledRect(hdc, cx-28, cy-28, 560, 5, C_ACCE);
                DrawTextAt(hdc, "CONNEXION ADMINISTRATEUR",
                           cx-28, cy-4, 560, 32, C_ACCE, hFTitle,
                           DT_CENTER|DT_SINGLELINE);
                DrawTextAt(hdc, "Entrez vos identifiants pour acceder au panneau de vote.",
                           cx-28, cy+36, 560, 22, C_SUBT, hFSub,
                           DT_CENTER|DT_SINGLELINE);
                DrawFilledRect(hdc, cx-28, cy+72, 560, 1, C_BORD);
            } else {
                /* Sidebar */
                RECT rSide = {0, HEAD_H, SIDE_W, WIN_H-STAT_H};
                DrawSidebar(hdc, &rSide);

                /* Status bar */
                RECT rStat = {0, WIN_H-STAT_H, WIN_W, WIN_H};
                DrawStatusBar(hdc, &rStat);

                /* Panel title */
                const char *panTitles[] = {
                    "TABLEAU DE BORD","GESTION DES ELECTEURS",
                    "GESTION DES CANDIDATS","VOTE & RESULTATS EN DIRECT",
                    "MODE RESEAU","GESTION DES COMPTES"
                };
                DrawTextAt(hdc, panTitles[activePanel],
                           CONT_X+20, CONT_Y+12, CONT_W-40, 24,
                           C_ACCE, hFTitle, DT_LEFT|DT_SINGLELINE);
                DrawFilledRect(hdc, CONT_X+20, CONT_Y+40, CONT_W-40, 1, C_BORD);

                /* Dessin contenu selon panel actif */
                if (activePanel == 0) {
                    RECT rDash = {CONT_X, CONT_Y+50, WIN_W, WIN_H-STAT_H};
                    DrawDashboard(hdc, &rDash);
                } else if (activePanel == 3) {
                    RECT rVote = {CONT_X, CONT_Y+50, WIN_W, WIN_H-STAT_H};
                    DrawResultBars(hdc, &rVote);
                } else if (activePanel == 4) {
                    /* RÃƒÂ©seau : info IP */
                    char nbuf[80];
                    WSADATA wsa; WSAStartup(MAKEWORD(2,2),&wsa);
                    char host[64]; gethostname(host,63);
                    sprintf(nbuf,"Port : 8888   Statut : %s", netRunning?"ACTIF":"Inactif");
                    DrawTextAt(hdc,nbuf,CONT_X+20,CONT_Y+52,CONT_W-40,18,
                               netRunning?C_SUCC:C_SUBT,hFNorm,DT_LEFT|DT_SINGLELINE);
                    WSACleanup();
                }
            }

            EndPaint(hWnd, &ps);
            return 0;
        }

        /* Ã¢â€â‚¬Ã¢â€â‚¬ Couleurs contrÃƒÂ´les Ã¢â€â‚¬Ã¢â€â‚¬ */
        case WM_CTLCOLORSTATIC: {
            HDC hdc = (HDC)wp;
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc,
                (GetDlgCtrlID((HWND)lp) == IDC_LERR) ? C_DANG : C_TEXT);
            SetBkColor(hdc, C_CARD);
            return (LRESULT)hBrCard;
        }
        case WM_CTLCOLOREDIT: {
            HDC hdc = (HDC)wp;
            SetTextColor(hdc, C_TEXT);
            SetBkColor(hdc, C_CARD);
            return (LRESULT)hBrCard;
        }
        case WM_CTLCOLORLISTBOX: {
            HDC hdc = (HDC)wp;
            SetTextColor(hdc, C_TEXT);
            SetBkColor(hdc, C_CARD);
            return (LRESULT)hBrCard;
        }

        /* Ã¢â€â‚¬Ã¢â€â‚¬ Boutons owner-draw Ã¢â€â‚¬Ã¢â€â‚¬ */
        case WM_DRAWITEM:
            DrawOwnerButton((DRAWITEMSTRUCT*)lp);
            return TRUE;

        /* Ã¢â€â‚¬Ã¢â€â‚¬ Commandes Ã¢â€â‚¬Ã¢â€â‚¬ */
        case WM_COMMAND: {
            int id = LOWORD(wp);
            if (screenMode == 0) {
                if (id == IDC_BLOGIN) {
                    if (DoLogin(hWnd)) TransitionToMain(hWnd);
                }
            } else {
                if      (id==IDC_NAV0) ShowPanel(0);
                else if (id==IDC_NAV1) ShowPanel(1);
                else if (id==IDC_NAV2) ShowPanel(2);
                else if (id==IDC_NAV3) ShowPanel(3);
                else if (id==IDC_NAV4) ShowPanel(4);
                else if (id==IDC_NAV5) ShowPanel(5);
                else if (id==IDC_BADE)    DoAddVoter(hWnd);
                else if (id==IDC_BADC)    DoAddCandidat(hWnd);
                else if (id==IDC_BOVOTE)  DoOpenVote(hWnd);
                else if (id==IDC_BCVOTE)  DoCloseVote(hWnd);
                else if (id==IDC_BSESSION) DoCloseSession(hWnd);
                else if (id==IDC_BRPT)    DoReport(hWnd);
                else if (id==IDC_BNET)    DoStartNet(hWnd);
                else if (id==IDC_BADUSER) DoAddUser(hWnd);
                else if (id==IDC_BRSTPWD) DoResetPwd(hWnd);
                else if (id==IDC_BACTIV)  DoToggleActive(hWnd);
                else if (id==IDC_BQUIT) {
                    if (MessageBox(hWnd,"Quitter et reinitialiser ?","PIVOTE",MB_YESNO)==IDYES) {
                        KillTimer(hWnd, IDT_REFRESH);
                        remove(FICHIER_SAUVEGARDE);
                        DestroyWindow(hWnd);
                    }
                }
                /* Invalide sidebar pour redessiner sÃƒÂ©lection */
                RECT rs = {0, HEAD_H, SIDE_W, WIN_H-STAT_H};
                InvalidateRect(hWnd, &rs, TRUE);
            }
            return 0;
        }

        /* Ã¢â€â‚¬Ã¢â€â‚¬ EntrÃƒÂ©e clavier login Ã¢â€â‚¬Ã¢â€â‚¬ */
        case WM_KEYDOWN:
            if (wp == VK_RETURN && screenMode == 0) {
                if (DoLogin(hWnd)) TransitionToMain(hWnd);
            }
            break;

        case WM_SIZE:
            if (wp != SIZE_MINIMIZED) {
                gWinW = LOWORD(lp);
                gWinH = HIWORD(lp);
                LayoutControls(hWnd);
                InvalidateRect(hWnd, NULL, TRUE);
            }
            return 0;

        case WM_GETMINMAXINFO: {
            MINMAXINFO *mmi = (MINMAXINFO*)lp;
            mmi->ptMinTrackSize.x = 1080;
            mmi->ptMinTrackSize.y = 760;
            return 0;
        }

        /* Ã¢â€â‚¬Ã¢â€â‚¬ Timer de rafraÃƒÂ®chissement Ã¢â€â‚¬Ã¢â€â‚¬ */
        case WM_TIMER:
            if (wp == IDT_REFRESH) {
                netRunning = serveurReseauActif();
                verifierExpirationVote();
                RECT r = {CONT_X, CONT_Y, WIN_W, WIN_H-STAT_H};
                InvalidateRect(hWnd, &r, TRUE);
                RECT rs = {0, WIN_H-STAT_H, WIN_W, WIN_H};
                InvalidateRect(hWnd, &rs, TRUE);
                if (activePanel == 1) RefreshElecList();
                if (activePanel == 2) RefreshCandList();
                if (activePanel == 5) RefreshUserList();
            }
            return 0;

        case WM_DESTROY:
            KillTimer(hWnd, IDT_REFRESH);
            arreterServeurReseau();
            sauvegarderDonnees();
            FreeGDI();
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hWnd, msg, wp, lp);
}

/* Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â
 *  WINMAIN
 * Ã¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢ÂÃ¢â€¢Â */
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
{
    (void)hPrev; (void)lpCmd;
    hInst = hI;

    /* Init commune contrÃƒÂ´les Windows */
    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_STANDARD_CLASSES };
    InitCommonControlsEx(&icc);

    /* Auth init */
    auth_init(CSV_PATH);

    /* GDI */
    InitGDI();

    /* Enregistrement classe principale */
    WNDCLASSEX wc = {0};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = MainWndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = "PivoteServeur";
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = hBrBg;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassEx(&wc);

    /* CrÃƒÂ©ation fenÃƒÂªtre centrÃƒÂ©e */
    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);
    hMain = CreateWindowEx(0, "PivoteServeur",
        "PIVOTE Ã¢â‚¬â€ Panneau Administrateur",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME,
        (sw-DEF_WIN_W)/2, (sh-DEF_WIN_H)/2, DEF_WIN_W, DEF_WIN_H,
        NULL, NULL, hInst, NULL);
    ShowWindow(hMain, nShow);
    UpdateWindow(hMain);

    /* Boucle messages */
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
