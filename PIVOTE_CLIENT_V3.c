/**
 * @file PIVOTE_GUI_CLIENT.c
 * @brief Interface graphique Win32 complete -- CLIENT PIVOTE V2
 *        Le mini-jeu Snake est integre directement (page PG_SNAKE).
 *        Apres un vote accepte, le bouton "Jouer au Snake" apparait.
 *        Le serpent se joue avec les fleches ou WASD.
 *        Echap revient a la page de succes. Espace rejoue.
 *
 * Compilation (Code::Blocks / MinGW, C99) :
 *   gcc -std=c99 -Wall PIVOTE_GUI_CLIENT.c -o client_gui.exe -lws2_32 -lgdi32 -mwindows
 *
 * Linker settings :  ws2_32  gdi32
 * Other linker options : -mwindows
 * Fichier requis : client.h  (doit definir PORT et BUFFER)
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "client.h"
#include "snake.h"

/* =========================================================
 * PALETTE
 * ========================================================= */
#define C_BG       RGB( 8, 10, 22)
#define C_SURFACE  RGB(14, 18, 36)
#define C_CARD     RGB(20, 26, 48)
#define C_PANEL    RGB(26, 32, 60)
#define C_BORDER   RGB(40, 52, 88)
#define C_ACCENT   RGB(82,162,255)
#define C_VIOLET   RGB(130,100,255)
#define C_GREEN    RGB(  0,210,110)
#define C_RED      RGB(255, 75, 75)
#define C_ORANGE   RGB(255,180,  0)
#define C_TEXT     RGB(218,230,252)
#define C_MUTED    RGB( 85,115,168)
#define C_WHITE    RGB(255,255,255)
/* Snake */
#define CS_HEAD    RGB(  0,232,140)
#define CS_BODY    RGB(  0,160, 90)
#define CS_APPLE   RGB(255, 72, 72)
#define CS_GRID    RGB( 22, 28, 52)
#define CS_WALL    RGB( 40, 52, 88)
#define CS_SCORE   RGB(255,200,  0)

/* =========================================================
 * DIMENSIONS
 * ========================================================= */
#define W_WIN 620
#define H_WIN 580
#define H_TOP  58

/* =========================================================
 * PAGES
 * ========================================================= */
#define PG_CONNECT 0
#define PG_LOGIN   1
#define PG_VOTE    2
#define PG_CONFIRM 3
#define PG_SUCCESS 4
#define PG_FAIL    5
#define PG_SNAKE   6

/* =========================================================
 * IDS
 * ========================================================= */
#define ID_E1       101
#define ID_E2       102
#define ID_E3       103
#define ID_LIST     200
#define ID_BTN_OK   300
#define ID_BTN_BACK 301
#define ID_BTN_PLAY 302
#define ID_TIMER_SN 400

/* =========================================================
 * SNAKE
 * ========================================================= */
#define SN_COLS  24
#define SN_ROWS  18
#define SN_CELL  22
#define SN_MAX   (SN_COLS*SN_ROWS)
#define SN_DELAY 130
#define SN_OX    ((W_WIN - SN_COLS*SN_CELL)/2)
#define SN_OY    (H_TOP + 62)

typedef struct { int x; int y; } Pt;
typedef enum { DIR_RIGHT=0,DIR_LEFT=1,DIR_UP=2,DIR_DOWN=3 } Dir;

/* =========================================================
 * GLOBALS
 * ========================================================= */
static HINSTANCE  hInst;
static HWND       hwMain;
static int        curPage = PG_CONNECT;

static HFONT  fHuge,fTitle,fBold,fNorm,fSmall,fMono,fSnScore;
static HBRUSH brBg,brSurface,brCard,brPanel,brAcc,brVio,
              brGrn,brRed,brOrange,brBorder;

static SOCKET sock = INVALID_SOCKET;
static char   serverIP[64]="";
static char   username[65]="";
static char   password[65]="";
static int    myIDElec=0;
static int    choixID=-1;
static char   choixNom[64]="";

#define MAX_CANDS 50
typedef struct { int id; char nom[50]; } CandInfo;
static CandInfo cands[MAX_CANDS];
static int      nbCands=0;

static char statusMsg[256]="";

/* Snake state */
static Pt   sn_body[SN_MAX];
static int  sn_len;
static Dir  sn_dir;
static Dir  sn_nextDir;
static Pt   sn_apple;
static int  sn_score;
static BOOL sn_running;
static BOOL sn_over;
static BOOL sn_started;

/* =========================================================
 * PROTOTYPES
 * ========================================================= */
static LRESULT CALLBACK MainProc(HWND,UINT,WPARAM,LPARAM);
static void BuildPage(void);
static void ClearDynamic(void);
static void DrawPage(HDC,RECT*);
static void DrawSnakeBoard(HDC);
static void SnakeInit(void);
static void SnakeTick(HWND);
static void SnakePlaceApple(void);
static BOOL SnakeHitsBody(Pt);
static void SnakeKey(int);

/* =========================================================
 * RESOURCES
 * ========================================================= */
static HFONT MkFont(int h,int w,const char*f){
    return CreateFont(h,0,0,0,w,0,0,0,DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
        DEFAULT_PITCH|FF_SWISS,f);
}
static void InitRes(void){
    fHuge    =MkFont(48,FW_BLACK, "Segoe UI");
    fTitle   =MkFont(22,FW_BOLD,  "Segoe UI");
    fBold    =MkFont(15,FW_BOLD,  "Segoe UI");
    fNorm    =MkFont(14,FW_NORMAL,"Segoe UI");
    fSmall   =MkFont(12,FW_NORMAL,"Segoe UI");
    fMono    =MkFont(13,FW_NORMAL,"Consolas");
    fSnScore =MkFont(17,FW_BOLD,  "Segoe UI");
    brBg     =CreateSolidBrush(C_BG);
    brSurface=CreateSolidBrush(C_SURFACE);
    brCard   =CreateSolidBrush(C_CARD);
    brPanel  =CreateSolidBrush(C_PANEL);
    brAcc    =CreateSolidBrush(C_ACCENT);
    brVio    =CreateSolidBrush(C_VIOLET);
    brGrn    =CreateSolidBrush(C_GREEN);
    brRed    =CreateSolidBrush(C_RED);
    brOrange =CreateSolidBrush(C_ORANGE);
    brBorder =CreateSolidBrush(C_BORDER);
}
static void FreeRes(void){
    DeleteObject(fHuge);DeleteObject(fTitle);DeleteObject(fBold);
    DeleteObject(fNorm);DeleteObject(fSmall);DeleteObject(fMono);
    DeleteObject(fSnScore);
    DeleteObject(brBg);DeleteObject(brSurface);DeleteObject(brCard);
    DeleteObject(brPanel);DeleteObject(brAcc);DeleteObject(brVio);
    DeleteObject(brGrn);DeleteObject(brRed);DeleteObject(brOrange);
    DeleteObject(brBorder);
}

/* =========================================================
 * RESEAU
 * ========================================================= */
static BOOL NetConnect(void){
    WSADATA wd; WSAStartup(MAKEWORD(2,2),&wd);
    sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock==INVALID_SOCKET){strcpy(statusMsg,"Erreur socket.");return FALSE;}
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(serverIP);
    addr.sin_port=htons(PORT);
    if(connect(sock,(struct sockaddr*)&addr,sizeof(addr))<0){
        closesocket(sock);sock=INVALID_SOCKET;
        strcpy(statusMsg,"Connexion impossible. Verifiez l'IP et le serveur.");
        return FALSE;
    }
    statusMsg[0]='\0'; return TRUE;
}
static BOOL NetAuth(void){
    char buf[BUFFER],resp[BUFFER];
    snprintf(buf,sizeof(buf),"AUTH %s %s",username,password);
    send(sock,buf,strlen(buf),0);
    int n=recv(sock,resp,BUFFER-1,0);
    if(n<=0){strcpy(statusMsg,"Erreur reseau.");return FALSE;}
    resp[n]='\0';
    if(strcmp(resp,"AUTH_OK")==0){statusMsg[0]='\0';return TRUE;}
    strcpy(statusMsg,"Identifiants incorrects. Reessayez.");
    return FALSE;
}
static void NetReceiveCandidats(void){
    char resp[BUFFER]; nbCands=0;
    int n=recv(sock,resp,BUFFER-1,0); if(n<=0)return;
    resp[n]='\0';
    char*p=resp;
    while(*p){
        if(*p=='['){
            p++; int id=0;
            while(*p>='0'&&*p<='9'){id=id*10+(*p-'0');p++;}
            if(*p==']')p++; while(*p==' ')p++;
            char nom[50]=""; int ni=0;
            while(*p&&*p!='\n'&&*p!='\r'&&ni<49)nom[ni++]=*p++;
            nom[ni]='\0';
            while(ni>0&&(nom[ni-1]==' '||nom[ni-1]=='\r'))nom[--ni]='\0';
            if(nom[0]&&nbCands<MAX_CANDS){
                cands[nbCands].id=id;
                strncpy(cands[nbCands].nom,nom,49); nbCands++;
            }
        }
        while(*p&&*p!='\n')p++; if(*p=='\n')p++;
    }
}
static BOOL NetVote(int idE,int idC){
    char buf[BUFFER],resp[BUFFER];
    snprintf(buf,sizeof(buf),"VOTE %d %d",idE,idC);
    send(sock,buf,strlen(buf),0);
    int n=recv(sock,resp,BUFFER-1,0); if(n<=0)return FALSE;
    resp[n]='\0'; return strcmp(resp,"OK")==0;
}

/* =========================================================
 * SNAKE — logique
 * ========================================================= */
static void SnakePlaceApple(void){
    BOOL libre; do{
        libre=TRUE; sn_apple.x=rand()%SN_COLS; sn_apple.y=rand()%SN_ROWS;
        for(int i=0;i<sn_len;i++)
            if(sn_body[i].x==sn_apple.x&&sn_body[i].y==sn_apple.y){libre=FALSE;break;}
    }while(!libre);
}
static BOOL SnakeHitsBody(Pt p){
    for(int i=1;i<sn_len;i++)
        if(sn_body[i].x==p.x&&sn_body[i].y==p.y)return TRUE;
    return FALSE;
}
static void SnakeInit(void){
    srand((unsigned int)time(NULL));
    sn_len=4; sn_dir=DIR_RIGHT; sn_nextDir=DIR_RIGHT;
    sn_score=0; sn_running=TRUE; sn_over=FALSE;
    for(int i=0;i<sn_len;i++){sn_body[i].x=SN_COLS/2-i;sn_body[i].y=SN_ROWS/2;}
    SnakePlaceApple();
}
static void SnakeTick(HWND hw){
    if(!sn_running||sn_over)return;
    sn_dir=sn_nextDir;
    Pt oldTail=sn_body[sn_len-1];
    for(int i=sn_len-1;i>0;i--)sn_body[i]=sn_body[i-1];
    switch(sn_dir){
        case DIR_RIGHT:sn_body[0].x++;break;
        case DIR_LEFT: sn_body[0].x--;break;
        case DIR_UP:   sn_body[0].y--;break;
        case DIR_DOWN: sn_body[0].y++;break;
    }
    if(sn_body[0].x<0||sn_body[0].x>=SN_COLS||
       sn_body[0].y<0||sn_body[0].y>=SN_ROWS||
       SnakeHitsBody(sn_body[0])){
        sn_over=TRUE; sn_running=FALSE;
        KillTimer(hw,ID_TIMER_SN); InvalidateRect(hw,NULL,FALSE); return;
    }
    if(sn_body[0].x==sn_apple.x&&sn_body[0].y==sn_apple.y){
        sn_score+=10;
        if(sn_len<SN_MAX){sn_body[sn_len]=oldTail;sn_len++;}
        SnakePlaceApple();
    }
    InvalidateRect(hw,NULL,FALSE);
}
static void SnakeKey(int vk){
    switch(vk){
        case VK_RIGHT:case 'D':if(sn_dir!=DIR_LEFT) sn_nextDir=DIR_RIGHT;break;
        case VK_LEFT: case 'A':if(sn_dir!=DIR_RIGHT)sn_nextDir=DIR_LEFT; break;
        case VK_UP:   case 'W':if(sn_dir!=DIR_DOWN) sn_nextDir=DIR_UP;   break;
        case VK_DOWN: case 'S':if(sn_dir!=DIR_UP)   sn_nextDir=DIR_DOWN; break;
    }
}

/* =========================================================
 * SNAKE — dessin
 * ========================================================= */
static void FillCell(HDC dc,int cx,int cy,COLORREF col,int sh){
    HBRUSH br=CreateSolidBrush(col);
    RECT r={SN_OX+cx*SN_CELL+sh,SN_OY+cy*SN_CELL+sh,
            SN_OX+cx*SN_CELL+SN_CELL-sh,SN_OY+cy*SN_CELL+SN_CELL-sh};
    FillRect(dc,&r,br); DeleteObject(br);
}
static void DrawSnakeBoard(HDC dc){
    /* Fond grille */
    HBRUSH bgr=CreateSolidBrush(CS_GRID);
    RECT rg={SN_OX,SN_OY,SN_OX+SN_COLS*SN_CELL,SN_OY+SN_ROWS*SN_CELL};
    FillRect(dc,&rg,bgr); DeleteObject(bgr);
    /* Lignes grille */
    HPEN pen=CreatePen(PS_SOLID,1,RGB(28,36,64));
    HPEN oldp=(HPEN)SelectObject(dc,pen);
    for(int x=0;x<=SN_COLS;x++){MoveToEx(dc,SN_OX+x*SN_CELL,SN_OY,NULL);LineTo(dc,SN_OX+x*SN_CELL,SN_OY+SN_ROWS*SN_CELL);}
    for(int y=0;y<=SN_ROWS;y++){MoveToEx(dc,SN_OX,SN_OY+y*SN_CELL,NULL);LineTo(dc,SN_OX+SN_COLS*SN_CELL,SN_OY+y*SN_CELL);}
    SelectObject(dc,oldp); DeleteObject(pen);
    /* Bordure */
    HPEN pw=CreatePen(PS_SOLID,2,CS_WALL);
    oldp=(HPEN)SelectObject(dc,pw);
    HBRUSH onb=(HBRUSH)SelectObject(dc,(HBRUSH)GetStockObject(NULL_BRUSH));
    Rectangle(dc,SN_OX-1,SN_OY-1,SN_OX+SN_COLS*SN_CELL+1,SN_OY+SN_ROWS*SN_CELL+1);
    SelectObject(dc,onb); SelectObject(dc,oldp); DeleteObject(pw);

    if(!sn_started){
        SetBkMode(dc,TRANSPARENT); SelectObject(dc,fBold); SetTextColor(dc,C_ACCENT);
        RECT rm={SN_OX,SN_OY+SN_ROWS*SN_CELL/2-14,SN_OX+SN_COLS*SN_CELL,SN_OY+SN_ROWS*SN_CELL/2+14};
        DrawText(dc,"Appuyez sur une fleche pour demarrer",-1,&rm,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
        return;
    }

    /* Pomme */
    FillCell(dc,sn_apple.x,sn_apple.y,CS_APPLE,3);
    HBRUSH bhi=CreateSolidBrush(RGB(255,140,140));
    RECT rhi={SN_OX+sn_apple.x*SN_CELL+5,SN_OY+sn_apple.y*SN_CELL+4,
              SN_OX+sn_apple.x*SN_CELL+9,SN_OY+sn_apple.y*SN_CELL+8};
    FillRect(dc,&rhi,bhi); DeleteObject(bhi);
    /* Corps */
    for(int i=sn_len-1;i>0;i--)FillCell(dc,sn_body[i].x,sn_body[i].y,CS_BODY,3);
    /* Tete */
    FillCell(dc,sn_body[0].x,sn_body[0].y,CS_HEAD,1);
    /* Yeux */
    int ex1=0,ey1=0,ex2=0,ey2=0;
    switch(sn_dir){
        case DIR_RIGHT:ex1=15;ey1=4; ex2=15;ey2=13;break;
        case DIR_LEFT: ex1=4; ey1=4; ex2=4; ey2=13;break;
        case DIR_UP:   ex1=4; ey1=4; ex2=13;ey2=4; break;
        case DIR_DOWN: ex1=4; ey1=15;ex2=13;ey2=15;break;
    }
    HBRUSH bey=CreateSolidBrush(C_BG);
    int bx=SN_OX+sn_body[0].x*SN_CELL, by2=SN_OY+sn_body[0].y*SN_CELL;
    RECT re1={bx+ex1-1,by2+ey1-1,bx+ex1+3,by2+ey1+3};
    RECT re2={bx+ex2-1,by2+ey2-1,bx+ex2+3,by2+ey2+3};
    FillRect(dc,&re1,bey); FillRect(dc,&re2,bey); DeleteObject(bey);

    if(sn_over){
        /* Assombrir */
        HBRUSH bov=CreateSolidBrush(RGB(8,10,22));
        for(int y2=0;y2<SN_ROWS;y2+=2)
            for(int x2=0;x2<SN_COLS;x2+=2){
                RECT rt2={SN_OX+x2*SN_CELL,SN_OY+y2*SN_CELL,
                          SN_OX+x2*SN_CELL+SN_CELL,SN_OY+y2*SN_CELL+SN_CELL};
                FillRect(dc,&rt2,bov);
            }
        DeleteObject(bov);
        /* Boite game over */
        int mx=SN_OX+SN_COLS*SN_CELL/2, my=SN_OY+SN_ROWS*SN_CELL/2;
        HBRUSH bc2=CreateSolidBrush(C_CARD);
        RECT rb2={mx-115,my-58,mx+115,my+62};
        FillRect(dc,&rb2,bc2); DeleteObject(bc2);
        HPEN pb=CreatePen(PS_SOLID,2,C_ACCENT);
        HPEN op=(HPEN)SelectObject(dc,pb);
        HBRUSH onb2=(HBRUSH)SelectObject(dc,(HBRUSH)GetStockObject(NULL_BRUSH));
        Rectangle(dc,mx-115,my-58,mx+115,my+62);
        SelectObject(dc,onb2); SelectObject(dc,op); DeleteObject(pb);
        SetBkMode(dc,TRANSPARENT);
        SelectObject(dc,fTitle); SetTextColor(dc,C_RED);
        RECT rt3={mx-105,my-48,mx+105,my-16}; DrawText(dc,"GAME OVER",-1,&rt3,DT_CENTER|DT_SINGLELINE);
        char sc2[32]; sprintf(sc2,"Score : %d",sn_score);
        SelectObject(dc,fBold); SetTextColor(dc,C_TEXT);
        RECT rs2={mx-105,my-12,mx+105,my+18}; DrawText(dc,sc2,-1,&rs2,DT_CENTER|DT_SINGLELINE);
        SelectObject(dc,fSmall); SetTextColor(dc,C_MUTED);
        RECT rh2={mx-105,my+22,mx+105,my+52};
        DrawText(dc,"Espace = Rejouer    Echap = Retour",-1,&rh2,DT_CENTER|DT_SINGLELINE);
    }
}

/* =========================================================
 * CONTROLES DYNAMIQUES
 * ========================================================= */
static void ClearDynamic(void){
    HWND c=GetTopWindow(hwMain);
    while(c){
        HWND nxt=GetWindow(c,GW_HWNDNEXT);
        if(GetDlgCtrlID(c)>=100)DestroyWindow(c);
        c=nxt;
    }
    statusMsg[0]='\0';
}
static HWND MkEdit(int id,int x,int y,int w,int h,BOOL pwd){
    HWND e=CreateWindowEx(0,"EDIT","",WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|(pwd?ES_PASSWORD:0),
        x,y,w,h,hwMain,(HMENU)(INT_PTR)id,hInst,NULL);
    SendMessage(e,WM_SETFONT,(WPARAM)fNorm,TRUE); return e;
}
static HWND MkBtn(int id,const char*txt,int x,int y,int w,int h){
    HWND b=CreateWindowEx(0,"BUTTON",txt,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,
        x,y,w,h,hwMain,(HMENU)(INT_PTR)id,hInst,NULL);
    SendMessage(b,WM_SETFONT,(WPARAM)fBold,TRUE); return b;
}

/* =========================================================
 * BUILD PAGE
 * ========================================================= */
static void BuildPage(void){
    KillTimer(hwMain,ID_TIMER_SN);
    ClearDynamic();
    int cx=(W_WIN-360)/2;
    switch(curPage){
    case PG_CONNECT:
        MkEdit(ID_E1,cx,250,360,32,FALSE);
        SetDlgItemText(hwMain,ID_E1,serverIP);
        MkBtn(ID_BTN_OK,"SE CONNECTER",cx,298,360,44); break;
    case PG_LOGIN:
        MkEdit(ID_E1,cx,218,360,32,FALSE);
        MkEdit(ID_E2,cx,266,360,32,TRUE);
        MkBtn(ID_BTN_OK,"CONNEXION",cx,314,360,44);
        MkBtn(ID_BTN_BACK,"< Retour",22,H_WIN-54,110,36); break;
    case PG_VOTE:{
        int ly=H_TOP+76;
        HWND lw=CreateWindowEx(WS_EX_CLIENTEDGE,"LISTBOX","",
            WS_CHILD|WS_VISIBLE|WS_VSCROLL|LBS_NOTIFY|LBS_NOINTEGRALHEIGHT,
            cx,ly,360,186,hwMain,(HMENU)(INT_PTR)ID_LIST,hInst,NULL);
        SendMessage(lw,WM_SETFONT,(WPARAM)fNorm,TRUE);
        for(int i=0;i<nbCands;i++){
            char buf[80];
            if(cands[i].id==0)sprintf(buf,"  [ 0 ]   Vote blanc");
            else sprintf(buf,"  [ %d ]   %s",cands[i].id,cands[i].nom);
            SendMessage(lw,LB_ADDSTRING,0,(LPARAM)buf);
        }
        HWND lbl=CreateWindowEx(0,"STATIC","Mon ID electeur :",WS_CHILD|WS_VISIBLE,
            cx,ly+196,158,24,hwMain,NULL,hInst,NULL);
        SendMessage(lbl,WM_SETFONT,(WPARAM)fNorm,TRUE);
        MkEdit(ID_E3,cx+164,ly+194,100,28,FALSE);
        MkBtn(ID_BTN_OK,"VALIDER MON CHOIX",cx,ly+234,360,44);
        MkBtn(ID_BTN_BACK,"< Retour",22,H_WIN-54,110,36); break;}
    case PG_CONFIRM:
        MkBtn(ID_BTN_OK,"CONFIRMER — VOTER",cx,H_WIN-148,360,46);
        MkBtn(ID_BTN_BACK,"< Modifier mon choix",cx,H_WIN-94,360,38); break;
    case PG_SUCCESS:
        MkBtn(ID_BTN_PLAY,"  Jouer au Snake  ->",cx,H_WIN-114,360,44);
        MkBtn(ID_BTN_BACK,"Fermer l'application",cx,H_WIN-62,360,36); break;
    case PG_FAIL:
        MkBtn(ID_BTN_BACK,"Fermer",cx+100,H_WIN-80,160,42); break;
    case PG_SNAKE:
        SnakeInit(); sn_started=FALSE; break;
    }
    InvalidateRect(hwMain,NULL,TRUE);
}

/* =========================================================
 * DRAW PAGE
 * ========================================================= */
static void DrawPage(HDC dc,RECT*rca){
    FillRect(dc,rca,brBg);
    /* Top bar */
    RECT rtop={0,0,W_WIN,H_TOP}; FillRect(dc,&rtop,brPanel);
    HBRUSH ba=CreateSolidBrush(C_ACCENT);
    RECT rl={0,H_TOP-2,W_WIN,H_TOP}; FillRect(dc,&rl,ba); DeleteObject(ba);
    SetBkMode(dc,TRANSPARENT);
    SelectObject(dc,fTitle); SetTextColor(dc,C_ACCENT); TextOut(dc,22,14,"PIVOTE",6);
    SelectObject(dc,fSmall); SetTextColor(dc,C_MUTED);  TextOut(dc,100,20,"Module Votant V2",16);
    /* Pastilles etapes */
    int steps[]={PG_CONNECT,PG_LOGIN,PG_VOTE,PG_CONFIRM};
    int stCur=(curPage>=PG_SUCCESS)?3:curPage;
    for(int i=0;i<4;i++){
        COLORREF col=(i<=stCur)?C_ACCENT:C_BORDER;
        HBRUSH bs=CreateSolidBrush(col);
        RECT rp={W_WIN-74+i*18,H_TOP/2-5,W_WIN-74+i*18+10,H_TOP/2+5};
        FillRect(dc,&rp,bs); DeleteObject(bs);
    }
    int cx=(W_WIN-360)/2;

    if(curPage==PG_CONNECT){
        SelectObject(dc,fHuge); SetTextColor(dc,C_TEXT);
        RECT rh={0,H_TOP+28,W_WIN,H_TOP+94}; DrawText(dc,"Connexion",-1,&rh,DT_CENTER|DT_SINGLELINE);
        SelectObject(dc,fNorm); SetTextColor(dc,C_MUTED);
        RECT rs={0,H_TOP+98,W_WIN,H_TOP+120}; DrawText(dc,"Entrez l'adresse IP du serveur PIVOTE",-1,&rs,DT_CENTER|DT_SINGLELINE);
        SelectObject(dc,fNorm); SetTextColor(dc,C_TEXT); TextOut(dc,cx,222,"Adresse IP du serveur",21);
        if(statusMsg[0]){SelectObject(dc,fSmall);SetTextColor(dc,C_RED);RECT re={0,H_WIN-38,W_WIN,H_WIN-16};DrawText(dc,statusMsg,-1,&re,DT_CENTER|DT_SINGLELINE);}
    }
    else if(curPage==PG_LOGIN){
        SelectObject(dc,fHuge); SetTextColor(dc,C_TEXT);
        RECT rh={0,H_TOP+18,W_WIN,H_TOP+80}; DrawText(dc,"Identification",-1,&rh,DT_CENTER|DT_SINGLELINE);
        char srv[80]; sprintf(srv,"Serveur : %s",serverIP);
        SelectObject(dc,fSmall); SetTextColor(dc,C_MUTED);
        RECT rs={0,H_TOP+80,W_WIN,H_TOP+100}; DrawText(dc,srv,-1,&rs,DT_CENTER|DT_SINGLELINE);
        SelectObject(dc,fNorm); SetTextColor(dc,C_TEXT);
        TextOut(dc,cx,191,"Identifiant",11); TextOut(dc,cx,239,"Mot de passe",12);
        if(statusMsg[0]){SelectObject(dc,fSmall);SetTextColor(dc,C_RED);RECT re={0,H_WIN-38,W_WIN,H_WIN-16};DrawText(dc,statusMsg,-1,&re,DT_CENTER|DT_SINGLELINE);}
    }
    else if(curPage==PG_VOTE){
        SelectObject(dc,fTitle); SetTextColor(dc,C_TEXT);
        RECT rh={0,H_TOP+12,W_WIN,H_TOP+44}; DrawText(dc,"Choisissez votre candidat",-1,&rh,DT_CENTER|DT_SINGLELINE);
        SelectObject(dc,fSmall); SetTextColor(dc,C_MUTED);
        RECT rs={0,H_TOP+46,W_WIN,H_TOP+66}; DrawText(dc,"Selectionnez dans la liste, puis entrez votre ID electeur",-1,&rs,DT_CENTER|DT_SINGLELINE);
        if(statusMsg[0]){SelectObject(dc,fSmall);SetTextColor(dc,C_RED);RECT re={0,H_WIN-38,W_WIN,H_WIN-16};DrawText(dc,statusMsg,-1,&re,DT_CENTER|DT_SINGLELINE);}
    }
    else if(curPage==PG_CONFIRM){
        SelectObject(dc,fHuge); SetTextColor(dc,C_TEXT);
        RECT rh={0,H_TOP+16,W_WIN,H_TOP+80}; DrawText(dc,"Confirmation",-1,&rh,DT_CENTER|DT_SINGLELINE);
        /* Carte */
        int cxc=cx-10,cyc=H_TOP+90;
        HBRUSH bcd=CreateSolidBrush(C_CARD); RECT rc={cxc,cyc,cxc+380,cyc+172}; FillRect(dc,&rc,bcd); DeleteObject(bcd);
        HBRUSH bba=CreateSolidBrush(C_ACCENT); RECT rba={cxc,cyc,cxc+4,cyc+172}; FillRect(dc,&rba,bba); DeleteObject(bba);
        HBRUSH bsp=CreateSolidBrush(C_BORDER);
        RECT rs1={cxc+14,cyc+56,cxc+366,cyc+57}; FillRect(dc,&rs1,bsp);
        RECT rs2={cxc+14,cyc+112,cxc+366,cyc+113}; FillRect(dc,&rs2,bsp); DeleteObject(bsp);
        SelectObject(dc,fSmall); SetTextColor(dc,C_MUTED);
        TextOut(dc,cxc+16,cyc+12,"Votre identifiant",17);
        TextOut(dc,cxc+16,cyc+68,"Candidat choisi",15);
        TextOut(dc,cxc+16,cyc+124,"Type de vote",12);
        char bufE[96]; sprintf(bufE,"%s  (ID : %d)",username,myIDElec);
        char bufC[96]; if(choixID==0)strcpy(bufC,"Vote blanc");else sprintf(bufC,"%s  (ID : %d)",choixNom,choixID);
        SelectObject(dc,fBold); SetTextColor(dc,C_TEXT);
        TextOut(dc,cxc+16,cyc+30,bufE,strlen(bufE));
        TextOut(dc,cxc+16,cyc+86,bufC,strlen(bufC));
        SelectObject(dc,fNorm); SetTextColor(dc,choixID==0?C_MUTED:C_ACCENT);
        TextOut(dc,cxc+16,cyc+140,choixID==0?"Vote Blanc":"Vote Nominatif",choixID==0?10:15);
        SelectObject(dc,fSmall); SetTextColor(dc,C_ORANGE);
        RECT rw={0,H_TOP+278,W_WIN,H_TOP+304}; DrawText(dc,"Ce vote est definitif et ne peut pas etre annule.",-1,&rw,DT_CENTER|DT_SINGLELINE);
    }
    else if(curPage==PG_SUCCESS){
        int icx=W_WIN/2,icy=H_TOP+100;
        HBRUSH bgr=CreateSolidBrush(C_GREEN);
        RECT ri={icx-44,icy-44,icx+44,icy+44}; FillRect(dc,&ri,bgr); DeleteObject(bgr);
        HBRUSH bc=CreateSolidBrush(C_BG);
        RECT co1={icx-44,icy-44,icx-32,icy-32};FillRect(dc,&co1,bc);
        RECT co2={icx+32,icy-44,icx+44,icy-32};FillRect(dc,&co2,bc);
        RECT co3={icx-44,icy+32,icx-32,icy+44};FillRect(dc,&co3,bc);
        RECT co4={icx+32,icy+32,icx+44,icy+44};FillRect(dc,&co4,bc); DeleteObject(bc);
        SelectObject(dc,fTitle); SetTextColor(dc,RGB(0,0,0));
        RECT rck={icx-44,icy-44,icx+44,icy+44}; DrawText(dc,"OK",-1,&rck,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        SelectObject(dc,fTitle); SetTextColor(dc,C_GREEN);
        RECT rt2={0,H_TOP+158,W_WIN,H_TOP+192}; DrawText(dc,"Vote enregistre !",-1,&rt2,DT_CENTER|DT_SINGLELINE);
        SelectObject(dc,fNorm); SetTextColor(dc,C_TEXT);
        RECT rs={0,H_TOP+198,W_WIN,H_TOP+222}; DrawText(dc,"Merci de votre participation.",-1,&rs,DT_CENTER|DT_SINGLELINE);
        SelectObject(dc,fSmall); SetTextColor(dc,C_MUTED);
        RECT rs2={40,H_TOP+226,W_WIN-40,H_TOP+268}; DrawText(dc,"Les resultats seront disponibles quand\nl'administrateur fermera le scrutin.",-1,&rs2,DT_CENTER|DT_WORDBREAK);
        char choixStr[80]; if(choixID==0)strcpy(choixStr,"Vote blanc");else sprintf(choixStr,"Candidat : %s",choixNom);
        SelectObject(dc,fBold); SetTextColor(dc,C_ACCENT);
        SIZE sz; GetTextExtentPoint32(dc,choixStr,strlen(choixStr),&sz);
        int bx2=(W_WIN-sz.cx)/2-14, by3=H_TOP+278;
        HBRUSH bbb=CreateSolidBrush(RGB(16,28,62)); RECT rbdg={bx2,by3,bx2+sz.cx+28,by3+sz.cy+12}; FillRect(dc,&rbdg,bbb); DeleteObject(bbb);
        TextOut(dc,bx2+14,by3+6,choixStr,strlen(choixStr));
        SelectObject(dc,fSmall); SetTextColor(dc,C_MUTED);
        RECT rsnk={0,H_TOP+330,W_WIN,H_TOP+354}; DrawText(dc,"En attendant les resultats, jouez au Snake !",-1,&rsnk,DT_CENTER|DT_SINGLELINE);
    }
    else if(curPage==PG_FAIL){
        int icx=W_WIN/2,icy=H_TOP+100;
        HBRUSH bfr=CreateSolidBrush(C_RED);
        RECT ri={icx-44,icy-44,icx+44,icy+44}; FillRect(dc,&ri,bfr); DeleteObject(bfr);
        HBRUSH bc=CreateSolidBrush(C_BG);
        RECT co1={icx-44,icy-44,icx-32,icy-32};FillRect(dc,&co1,bc);
        RECT co2={icx+32,icy-44,icx+44,icy-32};FillRect(dc,&co2,bc);
        RECT co3={icx-44,icy+32,icx-32,icy+44};FillRect(dc,&co3,bc);
        RECT co4={icx+32,icy+32,icx+44,icy+44};FillRect(dc,&co4,bc); DeleteObject(bc);
        SelectObject(dc,fBold); SetTextColor(dc,C_WHITE);
        RECT rck={icx-44,icy-44,icx+44,icy+44}; DrawText(dc,"X",-1,&rck,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        SelectObject(dc,fTitle); SetTextColor(dc,C_RED);
        RECT rt2={0,H_TOP+158,W_WIN,H_TOP+192}; DrawText(dc,"Vote refuse",-1,&rt2,DT_CENTER|DT_SINGLELINE);
        SelectObject(dc,fNorm); SetTextColor(dc,C_MUTED);
        RECT rs={40,H_TOP+198,W_WIN-40,H_TOP+330};
        DrawText(dc,statusMsg[0]?statusMsg:
            "Raisons possibles :\n - Vous avez deja vote\n - Le vote est ferme\n - Votre ID ne correspond pas a votre login",
            -1,&rs,DT_CENTER|DT_WORDBREAK);
    }
    else if(curPage==PG_SNAKE){
        SelectObject(dc,fTitle); SetTextColor(dc,C_GREEN); TextOut(dc,SN_OX,H_TOP+8,"SNAKE",5);
        SelectObject(dc,fSmall); SetTextColor(dc,C_MUTED); TextOut(dc,SN_OX+68,H_TOP+14,"— En attendant les resultats...",31);
        char sc[32]; sprintf(sc,"Score : %d",sn_score);
        SelectObject(dc,fSnScore); SetTextColor(dc,CS_SCORE);
        SIZE sz; GetTextExtentPoint32(dc,sc,strlen(sc),&sz);
        TextOut(dc,SN_OX+SN_COLS*SN_CELL-sz.cx,H_TOP+10,sc,strlen(sc));
        char ln[32]; sprintf(ln,"Longueur : %d",sn_len);
        SelectObject(dc,fSmall); SetTextColor(dc,C_MUTED);
        TextOut(dc,SN_OX+SN_COLS*SN_CELL-sz.cx,H_TOP+32,ln,strlen(ln));
        DrawSnakeBoard(dc);
        RECT rh={SN_OX,SN_OY+SN_ROWS*SN_CELL+8,SN_OX+SN_COLS*SN_CELL,SN_OY+SN_ROWS*SN_CELL+30};
        SelectObject(dc,fSmall); SetTextColor(dc,C_MUTED);
        DrawText(dc,"Fleches / WASD : bouger     Espace : rejouer     Echap : retour",-1,&rh,DT_CENTER|DT_SINGLELINE);
    }
}

/* =========================================================
 * DRAWITEM
 * ========================================================= */
static void DrawBtn(DRAWITEMSTRUCT*di){
    BOOL pr=di->itemState&ODS_SELECTED;
    COLORREF bg,fg;
    switch(di->CtlID){
    case ID_BTN_OK:
        bg=pr?(curPage==PG_CONFIRM?RGB(0,168,88):RGB(54,128,214)):(curPage==PG_CONFIRM?C_GREEN:C_ACCENT);
        fg=RGB(0,0,0); break;
    case ID_BTN_PLAY:
        bg=pr?RGB(0,168,88):C_GREEN; fg=RGB(0,0,0); break;
    case ID_BTN_BACK:
        bg=pr?C_PANEL:C_CARD; fg=C_MUTED; break;
    default: bg=C_CARD; fg=C_TEXT;
    }
    HBRUSH br=CreateSolidBrush(bg); FillRect(di->hDC,&di->rcItem,br); DeleteObject(br);
    SetBkMode(di->hDC,TRANSPARENT); SetTextColor(di->hDC,fg);
    SelectObject(di->hDC,(di->CtlID==ID_BTN_BACK)?fNorm:fBold);
    char txt[128]; GetWindowText(di->hwndItem,txt,128);
    DrawText(di->hDC,txt,-1,&di->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}

/* =========================================================
 * WINPROC
 * ========================================================= */
static LRESULT CALLBACK MainProc(HWND hw,UINT m,WPARAM wp,LPARAM lp){
    switch(m){
    case WM_CREATE:
        hwMain=hw; InitRes(); BuildPage(); return 0;
    case WM_TIMER:
        if(wp==ID_TIMER_SN)SnakeTick(hw); return 0;
    case WM_KEYDOWN:
        if(curPage==PG_SNAKE){
            if(wp==VK_ESCAPE){KillTimer(hw,ID_TIMER_SN);curPage=PG_SUCCESS;BuildPage();return 0;}
            if(wp==VK_SPACE){
                if(sn_over||!sn_started){SnakeInit();sn_started=TRUE;SetTimer(hw,ID_TIMER_SN,SN_DELAY,NULL);}
                InvalidateRect(hw,NULL,FALSE); return 0;
            }
            if(!sn_started){sn_started=TRUE;SetTimer(hw,ID_TIMER_SN,SN_DELAY,NULL);}
            SnakeKey((int)wp);
        } return 0;
    case WM_PAINT:{
        PAINTSTRUCT ps; HDC dc=BeginPaint(hw,&ps);
        RECT rc; GetClientRect(hw,&rc);
        HDC mdc=CreateCompatibleDC(dc);
        HBITMAP bmp=CreateCompatibleBitmap(dc,rc.right,rc.bottom);
        HBITMAP old=(HBITMAP)SelectObject(mdc,bmp);
        DrawPage(mdc,&rc);
        BitBlt(dc,0,0,rc.right,rc.bottom,mdc,0,0,SRCCOPY);
        SelectObject(mdc,old);DeleteObject(bmp);DeleteDC(mdc);
        EndPaint(hw,&ps); return 0;}
    case WM_ERASEBKGND: return 1;
    case WM_CTLCOLOREDIT:
        SetBkColor((HDC)wp,C_CARD);SetTextColor((HDC)wp,C_TEXT);return(LRESULT)brCard;
    case WM_CTLCOLORSTATIC:
        SetBkMode((HDC)wp,TRANSPARENT);SetTextColor((HDC)wp,C_TEXT);return(LRESULT)brBg;
    case WM_CTLCOLORLISTBOX:
        SetBkColor((HDC)wp,C_CARD);SetTextColor((HDC)wp,C_TEXT);return(LRESULT)brCard;
    case WM_DRAWITEM:
        DrawBtn((DRAWITEMSTRUCT*)lp); return TRUE;
    case WM_COMMAND:{
        int id=LOWORD(wp);
        if(id==ID_BTN_OK){
            if(curPage==PG_CONNECT){
                GetDlgItemText(hw,ID_E1,serverIP,64);
                if(!serverIP[0]){strcpy(statusMsg,"Entrez une adresse IP.");InvalidateRect(hw,NULL,TRUE);return 0;}
                if(NetConnect()){curPage=PG_LOGIN;BuildPage();}else InvalidateRect(hw,NULL,TRUE);
            }
            else if(curPage==PG_LOGIN){
                GetDlgItemText(hw,ID_E1,username,65);GetDlgItemText(hw,ID_E2,password,65);
                if(!username[0]||!password[0]){strcpy(statusMsg,"Remplissez tous les champs.");InvalidateRect(hw,NULL,TRUE);return 0;}
                if(NetAuth()){NetReceiveCandidats();curPage=PG_VOTE;BuildPage();}else InvalidateRect(hw,NULL,TRUE);
            }
            else if(curPage==PG_VOTE){
                int sel=(int)SendDlgItemMessage(hw,ID_LIST,LB_GETCURSEL,0,0);
                char idStr[16];GetDlgItemText(hw,ID_E3,idStr,16);myIDElec=atoi(idStr);
                if(sel==LB_ERR){strcpy(statusMsg,"Selectionnez un candidat.");InvalidateRect(hw,NULL,TRUE);return 0;}
                if(!myIDElec){strcpy(statusMsg,"Entrez votre ID electeur.");InvalidateRect(hw,NULL,TRUE);return 0;}
                choixID=cands[sel].id; strncpy(choixNom,cands[sel].nom,63);choixNom[63]='\0';
                curPage=PG_CONFIRM;BuildPage();
            }
            else if(curPage==PG_CONFIRM){
                if(NetVote(myIDElec,choixID)){curPage=PG_SUCCESS;}
                else{strcpy(statusMsg,"Vote refuse (deja vote, ferme, ou ID invalide).");curPage=PG_FAIL;}
                BuildPage();
            }
        }
        if(id==ID_BTN_PLAY&&curPage==PG_SUCCESS){curPage=PG_SNAKE;BuildPage();SetFocus(hw);}
        if(id==ID_BTN_BACK){
            if(curPage==PG_LOGIN){if(sock!=INVALID_SOCKET){closesocket(sock);WSACleanup();sock=INVALID_SOCKET;}curPage=PG_CONNECT;BuildPage();}
            else if(curPage==PG_VOTE){curPage=PG_LOGIN;BuildPage();}
            else if(curPage==PG_CONFIRM){curPage=PG_VOTE;BuildPage();}
            else if(curPage==PG_SUCCESS||curPage==PG_FAIL)DestroyWindow(hw);
        }
        return 0;}
    case WM_DESTROY:
        KillTimer(hw,ID_TIMER_SN);FreeRes();
        if(sock!=INVALID_SOCKET){closesocket(sock);WSACleanup();}
        PostQuitMessage(0); return 0;
    }
    return DefWindowProc(hw,m,wp,lp);
}

/* =========================================================
 * WINMAIN
 * ========================================================= */
int WINAPI WinMain(HINSTANCE hI,HINSTANCE hP,LPSTR cmd,int show){
    hInst=hI;(void)hP;(void)cmd;
    WNDCLASSEX wc={sizeof(WNDCLASSEX)};
    wc.lpfnWndProc=MainProc;wc.hInstance=hI;wc.lpszClassName="PIVOTEClient";
    wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hCursor=LoadCursor(NULL,IDC_ARROW);wc.hIcon=LoadIcon(NULL,IDI_APPLICATION);
    RegisterClassEx(&wc);
    int sw=GetSystemMetrics(SM_CXSCREEN),sh=GetSystemMetrics(SM_CYSCREEN);
    HWND hw=CreateWindowEx(WS_EX_APPWINDOW,"PIVOTEClient","PIVOTE - Module Votant V2",
        WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
        (sw-W_WIN)/2,(sh-H_WIN)/2,W_WIN,H_WIN,NULL,NULL,hI,NULL);
    ShowWindow(hw,show);UpdateWindow(hw);
    MSG msg;
    while(GetMessage(&msg,NULL,0,0)>0){
        if(curPage==PG_SNAKE){
            /* Intercepter les fleches pour le Snake meme si un child a le focus */
            if(msg.message==WM_KEYDOWN){
                SendMessage(hwMain,WM_KEYDOWN,msg.wParam,msg.lParam);
                continue;
            }
        } else {
            if(msg.message==WM_KEYDOWN&&msg.wParam==VK_RETURN){
                HWND btn=GetDlgItem(hwMain,ID_BTN_OK);
                if(btn)SendMessage(hwMain,WM_COMMAND,MAKEWPARAM(ID_BTN_OK,BN_CLICKED),(LPARAM)btn);
                continue;
            }
        }
        TranslateMessage(&msg);DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
