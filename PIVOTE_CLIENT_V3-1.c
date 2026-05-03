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
#define _WIN32_WINNT 0x0600
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
#define C_BG       RGB( 7, 11, 21)
#define C_SURFACE  RGB(15, 22, 40)
#define C_CARD     RGB(24, 34, 58)
#define C_PANEL    RGB(29, 41, 71)
#define C_BORDER   RGB(58, 77,121)
#define C_ACCENT   RGB(105,194,255)
#define C_VIOLET   RGB(153,126,255)
#define C_GREEN    RGB( 74,220,132)
#define C_RED      RGB(255,102, 96)
#define C_ORANGE   RGB(255,196, 76)
#define C_TEXT     RGB(240,246,255)
#define C_MUTED    RGB(176,196,228)
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
#define DEF_W_WIN 920
#define DEF_H_WIN 760
#define H_TOP      72
#define W_WIN      (gWinW)
#define H_WIN      (gWinH)

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
#define ID_TIMER_NET 401

/* =========================================================
 * SNAKE
 * ========================================================= */
#define SN_COLS  24
#define SN_ROWS  18
#define SN_CELL  22
#define SN_MAX   (SN_COLS*SN_ROWS)
#define SN_DELAY 220
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
static int        gWinW = DEF_W_WIN;
static int        gWinH = DEF_H_WIN;

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
static char pendingList[BUFFER*2]="";
static time_t voteDeadline=0;
[03/05/2026 09:08] Angelo William: /* Snake state */
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
static void CloseConnection(void);
static void RelayoutPage(void);
static void FormatVoteCountdown(char*,size_t);
static int  GetVoteRemainingSeconds(void);
static void HandleServerShutdown(const char*);
static void NetPollAsync(HWND);

/* =========================================================
 * RESOURCES
 * ========================================================= */
static HFONT MkFont(int h,int w,const char*f){
    return CreateFont(-abs(h),0,0,0,w,0,0,0,DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
        DEFAULT_PITCH|FF_SWISS,f);
}
static void InitRes(void){
    fHuge    =MkFont(60,FW_BLACK, "Segoe UI");
    fTitle   =MkFont(30,FW_BOLD,  "Segoe UI");
    fBold    =MkFont(20,FW_BOLD,  "Segoe UI");
    fNorm    =MkFont(18,FW_NORMAL,"Segoe UI");
    fSmall   =MkFont(16,FW_NORMAL,"Segoe UI");
    fMono    =MkFont(16,FW_NORMAL,"Consolas");
    fSnScore =MkFont(22,FW_BOLD,  "Segoe UI");
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

static void CloseConnection(void){
    if(sock!=INVALID_SOCKET){
        closesocket(sock);
        WSACleanup();
        sock=INVALID_SOCKET;
    }
}

static int GetVoteRemainingSeconds(void){
    if(voteDeadline<=0)return -1;
    {
        time_t now=time(NULL);
        if(now>=voteDeadline)return 0;
        return (int)difftime(voteDeadline,now);
    }
}

static void FormatVoteCountdown(char*buf,size_t size){
    int rem=GetVoteRemainingSeconds();
    if(rem<0){snprintf(buf,size,"Temps de vote : non defini");return;}
    snprintf(buf,size,"Temps restant : %02d:%02d",rem/60,rem%60);
}

static void HandleServerShutdown(const char*msg){
    char finalMsg[256];
    if(msg&&strstr(msg,"SESSION_CLOSED"))
        strcpy(finalMsg,"La session a ete fermee par l'administrateur.");
    else if(msg&&strstr(msg,"VOTE_TIMEOUT"))
        strcpy(finalMsg,"Le temps attribue a ce votant est ecoule.");
    else if(msg&&strstr(msg,"VOTE_CLOSED"))
        strcpy(finalMsg,"Le vote est maintenant ferme.");
    else
        strcpy(finalMsg,"Connexion au serveur interrompue.");
    CloseConnection();
    curPage=PG_FAIL;
    BuildPage();
    strncpy(statusMsg,finalMsg,sizeof(statusMsg)-1);
    statusMsg[sizeof(statusMsg)-1]='\0';
    InvalidateRect(hwMain,NULL,TRUE);
}
[03/05/2026 09:08] Angelo William: /* =========================================================
 * RESEAU
 * ========================================================= */
static BOOL NetConnect(void){
    WSADATA wd; WSAStartup(MAKEWORD(2,2),&wd);
    sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock==INVALID_SOCKET){strcpy(statusMsg,"Erreur socket.");return FALSE;}
    pendingList[0]='\0';
    voteDeadline=0;
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(serverIP);
    addr.sin_port=htons(PORT);
    if(connect(sock,(struct sockaddr*)&addr,sizeof(addr))<0){
        CloseConnection();
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
    if(n<=0){strcpy(statusMsg,"Erreur reseau.");CloseConnection();return FALSE;}
    resp[n]='\0';
    if(strncmp(resp,"AUTH_OK",7)==0){
        pendingList[0]='\0';
        if(n>7){
            const char*extra=resp+7;
            while(*extra=='\r'*extra=='\n')extra++;
            strncpy(pendingList,extra,sizeof(pendingList)-1);
            pendingList[sizeof(pendingList)-1]='\0';
        }
        statusMsg[0]='\0';
        return TRUE;
    }
    if(strstr(resp,"SESSION_CLOSED"))strcpy(statusMsg,"La session a ete fermee par l'administrateur.");
    else if(strstr(resp,"VOTE_TIMEOUT"))strcpy(statusMsg,"Le temps du vote est ecoule.");
    else if(strstr(resp,"VOTE_CLOSED"))strcpy(statusMsg,"Le vote n'est pas ouvert.");
    else strcpy(statusMsg,"Identifiants incorrects. Reessayez.");
    CloseConnection();
    return FALSE;
}
static void NetReceiveCandidats(void){
    char resp[BUFFER*2];
    char*ctx=NULL;
    char*line=NULL;
    nbCands=0;

    if(pendingList[0]){
        strncpy(resp,pendingList,sizeof(resp)-1);
        resp[sizeof(resp)-1]='\0';
        pendingList[0]='\0';
    }else{
        int n=recv(sock,resp,sizeof(resp)-1,0); if(n<=0)return;
        resp[n]='\0';
    }

    line=strtok_s(resp,"\n",&ctx);
    while(line){
        while(*line=='\r'*line==' ')line++;
        if(strncmp(line,"INFO DEADLINE ",14)==0){
            long long deadline=0;
            if(sscanf(line+14,"%lld",&deadline)==1)voteDeadline=(time_t)deadline;
        }else if(line[0]=='['){
            char *p=line+1;
            int id=0;
            while(*p>='0'&&*p<='9'){id=id*10+(*p-'0');p++;}
            if(*p==']')p++;
            while(*p==' ')p++;
            if(nbCands<MAX_CANDS){
                cands[nbCands].id=id;
                strncpy(cands[nbCands].nom,p,49);
                cands[nbCands].nom[49]='\0';
                nbCands++;
            }
        }
        line=strtok_s(NULL,"\n",&ctx);
    }
}
static BOOL NetVote(int idE,int idC){
    char buf[BUFFER],resp[BUFFER];
    snprintf(buf,sizeof(buf),"VOTE %d %d",idE,idC);
    send(sock,buf,strlen(buf),0);
    {
        int n=recv(sock,resp,BUFFER-1,0);
        if(n<=0){strcpy(statusMsg,"Connexion au serveur interrompue.");CloseConnection();return FALSE;}
        resp[n]='\0';
        CloseConnection();
        if(strcmp(resp,"OK")==0)return TRUE;
        if(strstr(resp,"SESSION_CLOSED"))strcpy(statusMsg,"La session a ete fermee par l'administrateur.");
        else if(strstr(resp,"VOTE_TIMEOUT"))strcpy(statusMsg,"Le temps du vote est ecoule.");
        else if(strstr(resp,"VOTE_CLOSED"))strcpy(statusMsg,"Le vote est maintenant ferme.");
        else strcpy(statusMsg,"Vote refuse (deja vote, ferme, ou ID invalide).");
        return FALSE;
    }
}

static void NetPollAsync(HWND hw){
    if(sock==INVALID_SOCKET)return;
    if(curPage!=PG_VOTE&&curPage!=PG_CONFIRM)return;

    fd_set rfds;
    struct timeval tv;
    FD_ZERO(&rfds);
    FD_SET(sock,&rfds);
    tv.tv_sec=0;
    tv.tv_usec=0;
[03/05/2026 09:08] Angelo William: if(select(0,&rfds,NULL,NULL,&tv)>0&&FD_ISSET(sock,&rfds)){
        char resp[BUFFER];
        int n=recv(sock,resp,BUFFER-1,0);
        if(n<=0){HandleServerShutdown(NULL);return;}
        resp[n]='\0';
        HandleServerShutdown(resp);
        InvalidateRect(hw,NULL,TRUE);
    }
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
[03/05/2026 09:08] Angelo William: /* Pomme */
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
        DestroyWindow(c);
        c=nxt;
    }
    statusMsg[0]='\0';
}
static HWND MkEdit(int id,int x,int y,int w,int h,BOOL pwd){
    HWND e=CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL|(pwd?ES_PASSWORD:0),
        x,y,w,h,hwMain,(HMENU)(INT_PTR)id,hInst,NULL);
    SendMessage(e,WM_SETFONT,(WPARAM)fNorm,TRUE);
    SendMessage(e,EM_SETMARGINS,EC_LEFTMARGIN|EC_RIGHTMARGIN,MAKELPARAM(8,8));
    return e;
}
static HWND MkBtn(int id,const char*txt,int x,int y,int w,int h){
    HWND b=CreateWindowEx(0,"BUTTON",txt,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,
        x,y,w,h,hwMain,(HMENU)(INT_PTR)id,hInst,NULL);
    SendMessage(b,WM_SETFONT,(WPARAM)fBold,TRUE); return b;
}
[03/05/2026 09:08] Angelo William: /* =========================================================
 * BUILD PAGE
 * ========================================================= */
static void BuildPage(void){
    KillTimer(hwMain,ID_TIMER_SN);
    KillTimer(hwMain,ID_TIMER_NET);
    ClearDynamic();
    int cx=(W_WIN-460)/2;
    switch(curPage){
    case PG_CONNECT:
        MkEdit(ID_E1,cx,H_TOP+210,460,46,FALSE);
        SetDlgItemText(hwMain,ID_E1,serverIP);
        MkBtn(ID_BTN_OK,"SE CONNECTER",cx,H_TOP+272,460,54); break;
    case PG_LOGIN:
        MkEdit(ID_E1,cx,H_TOP+180,460,46,FALSE);
        MkEdit(ID_E2,cx,H_TOP+246,460,46,TRUE);
        MkBtn(ID_BTN_OK,"CONNEXION",cx,H_TOP+314,460,54);
        MkBtn(ID_BTN_BACK,"< Retour",28,H_WIN-74,148,44); break;
    case PG_VOTE:{
        int ly=H_TOP+138;
        HWND lw=CreateWindowEx(WS_EX_CLIENTEDGE,"LISTBOX","",
            WS_CHILD|WS_VISIBLE|WS_VSCROLL|LBS_NOTIFY|LBS_NOINTEGRALHEIGHT,
            cx,ly,460,260,hwMain,(HMENU)(INT_PTR)ID_LIST,hInst,NULL);
        SendMessage(lw,WM_SETFONT,(WPARAM)fNorm,TRUE);
        SendMessage(lw,LB_SETITEMHEIGHT,0,26);
        for(int i=0;i<nbCands;i++){
            char buf[80];
            if(cands[i].id==0)sprintf(buf,"  [ 0 ]   Vote blanc");
            else sprintf(buf,"  [ %d ]   %s",cands[i].id,cands[i].nom);
            SendMessage(lw,LB_ADDSTRING,0,(LPARAM)buf);
        }
        HWND lbl=CreateWindowEx(0,"STATIC","Mon ID electeur :",WS_CHILD|WS_VISIBLE,
            cx,ly+280,190,30,hwMain,NULL,hInst,NULL);
        SendMessage(lbl,WM_SETFONT,(WPARAM)fNorm,TRUE);
        MkEdit(ID_E3,cx+198,ly+276,160,44,FALSE);
        MkBtn(ID_BTN_OK,"VALIDER MON CHOIX",cx,ly+338,460,54);
        MkBtn(ID_BTN_BACK,"< Retour",28,H_WIN-74,148,44);
        SetTimer(hwMain,ID_TIMER_NET,1000,NULL); break;}
    case PG_CONFIRM:
        MkBtn(ID_BTN_OK,"CONFIRMER - VOTER",cx,H_WIN-178,460,54);
        MkBtn(ID_BTN_BACK,"< Modifier mon choix",cx,H_WIN-112,460,42);
        SetTimer(hwMain,ID_TIMER_NET,1000,NULL); break;
    case PG_SUCCESS:
        MkBtn(ID_BTN_PLAY,"  Jouer au Snake  ->",cx,H_WIN-126,460,52);
        MkBtn(ID_BTN_BACK,"Fermer l'application",cx,H_WIN-66,460,40); break;
    case PG_FAIL:
        MkBtn(ID_BTN_BACK,"Fermer",cx+150,H_WIN-88,160,46); break;
    case PG_SNAKE:
        SnakeInit(); sn_started=FALSE; break;
    }
    InvalidateRect(hwMain,NULL,TRUE);
}

static void RelayoutPage(void){
    char savedStatus[256];
    char t1[128]="",t2[128]="",t3[32]="";
    int sel=LB_ERR;

    if(curPage==PG_SNAKE){InvalidateRect(hwMain,NULL,TRUE);return;}

    strcpy(savedStatus,statusMsg);
    if(curPage==PG_CONNECT&&GetDlgItem(hwMain,ID_E1))
        GetDlgItemText(hwMain,ID_E1,t1,(int)sizeof(t1));
    else if(curPage==PG_LOGIN){
        if(GetDlgItem(hwMain,ID_E1))GetDlgItemText(hwMain,ID_E1,t1,(int)sizeof(t1));
        if(GetDlgItem(hwMain,ID_E2))GetDlgItemText(hwMain,ID_E2,t2,(int)sizeof(t2));
    }else if(curPage==PG_VOTE){
        if(GetDlgItem(hwMain,ID_E3))GetDlgItemText(hwMain,ID_E3,t3,(int)sizeof(t3));
        if(GetDlgItem(hwMain,ID_LIST))sel=(int)SendDlgItemMessage(hwMain,ID_LIST,LB_GETCURSEL,0,0);
    }

    BuildPage();
    strcpy(statusMsg,savedStatus);

    if(curPage==PG_CONNECT&&GetDlgItem(hwMain,ID_E1))SetDlgItemText(hwMain,ID_E1,t1);
    else if(curPage==PG_LOGIN){
        if(GetDlgItem(hwMain,ID_E1))SetDlgItemText(hwMain,ID_E1,t1);
        if(GetDlgItem(hwMain,ID_E2))SetDlgItemText(hwMain,ID_E2,t2);
    }else if(curPage==PG_VOTE){
        if(GetDlgItem(hwMain,ID_E3))SetDlgItemText(hwMain,ID_E3,t3);
        if(sel!=LB_ERR&&GetDlgItem(hwMain,ID_LIST))SendDlgItemMessage(hwMain,ID_LIST,LB_SETCURSEL,sel,0);
    }

    InvalidateRect(hwMain,NULL,TRUE);
}
[03/05/2026 09:08] Angelo William: static void DrawPage(HDC dc,RECT*rca){
    FillRect(dc,rca,brBg);
    RECT rtop={0,0,W_WIN,H_TOP}; FillRect(dc,&rtop,brPanel);
    HBRUSH ba=CreateSolidBrush(C_ACCENT);
    RECT rl={0,H_TOP-3,W_WIN,H_TOP}; FillRect(dc,&rl,ba); DeleteObject(ba);
    SetBkMode(dc,TRANSPARENT);
    SelectObject(dc,fTitle); SetTextColor(dc,C_ACCENT); TextOut(dc,28,18,"PIVOTE",6);
    SelectObject(dc,fSmall); SetTextColor(dc,C_MUTED);  TextOut(dc,138,24,"Module Votant V3",17);

    if(voteDeadline>0&&(curPage==PG_VOTE||curPage==PG_CONFIRM)){
        char tbuf[64];
        FormatVoteCountdown(tbuf,sizeof(tbuf));
        SelectObject(dc,fSmall);
        SetTextColor(dc,GetVoteRemainingSeconds()<=60?C_ORANGE:C_TEXT);
        TextOut(dc,W_WIN-250,24,tbuf,strlen(tbuf));
    }

    int stCur=(curPage>=PG_SUCCESS)?3:curPage;
    for(int i=0;i<4;i++){
        COLORREF col=(i<=stCur)?C_ACCENT:C_BORDER;
        HBRUSH bs=CreateSolidBrush(col);
        RECT rp={W_WIN-116+i*24,H_TOP/2-6,W_WIN-116+i*24+14,H_TOP/2+6};
        FillRect(dc,&rp,bs); DeleteObject(bs);
    }
    int cx=(W_WIN-460)/2;

    if(curPage==PG_CONNECT){
        SelectObject(dc,fHuge); SetTextColor(dc,C_TEXT);
        RECT rh={0,H_TOP+40,W_WIN,H_TOP+118}; DrawText(dc,"Connexion",-1,&rh,DT_CENTER|DT_SINGLELINE);
        SelectObject(dc,fNorm); SetTextColor(dc,C_MUTED);
        RECT rs={0,H_TOP+122,W_WIN,H_TOP+152}; DrawText(dc,"Entrez l'adresse IP du serveur PIVOTE",-1,&rs,DT_CENTER|DT_SINGLELINE);
        SelectObject(dc,fNorm); SetTextColor(dc,C_TEXT); TextOut(dc,cx,H_TOP+174,"Adresse IP du serveur",21);
        if(statusMsg[0]){SelectObject(dc,fSmall);SetTextColor(dc,C_RED);RECT re={0,H_WIN-50,W_WIN,H_WIN-18};DrawText(dc,statusMsg,-1,&re,DT_CENTER|DT_SINGLELINE);}
    }
    else if(curPage==PG_LOGIN){
        SelectObject(dc,fHuge); SetTextColor(dc,C_TEXT);
        RECT rh={0,H_TOP+28,W_WIN,H_TOP+106}; DrawText(dc,"Identification",-1,&rh,DT_CENTER|DT_SINGLELINE);
        char srv[80]; sprintf(srv,"Serveur : %s",serverIP);
        SelectObject(dc,fSmall); SetTextColor(dc,C_MUTED);
        RECT rs={0,H_TOP+112,W_WIN,H_TOP+140}; DrawText(dc,srv,-1,&rs,DT_CENTER|DT_SINGLELINE);
        SelectObject(dc,fNorm); SetTextColor(dc,C_TEXT);
        TextOut(dc,cx,H_TOP+146,"Identifiant",11); TextOut(dc,cx,H_TOP+208,"Mot de passe",12);
        if(statusMsg[0]){SelectObject(dc,fSmall);SetTextColor(dc,C_RED);RECT re={0,H_WIN-50,W_WIN,H_WIN-18};DrawText(dc,statusMsg,-1,&re,DT_CENTER|DT_SINGLELINE);}
    }
    else if(curPage==PG_VOTE){
        SelectObject(dc,fTitle); SetTextColor(dc,C_TEXT);
        RECT rh={0,H_TOP+26,W_WIN,H_TOP+66}; DrawText(dc,"Choisissez votre candidat",-1,&rh,DT_CENTER|DT_SINGLELINE);
        SelectObject(dc,fSmall); SetTextColor(dc,C_MUTED);
        RECT rs={0,H_TOP+72,W_WIN,H_TOP+102}; DrawText(dc,"Selectionnez dans la liste, puis entrez votre ID electeur",-1,&rs,DT_CENTER|DT_SINGLELINE);
        if(voteDeadline>0){
            char tbuf[64]; FormatVoteCountdown(tbuf,sizeof(tbuf));
            SetTextColor(dc,GetVoteRemainingSeconds()<=60?C_ORANGE:C_ACCENT);
            RECT rt={0,H_TOP+102,W_WIN,H_TOP+126}; DrawText(dc,tbuf,-1,&rt,DT_CENTER|DT_SINGLELINE);
        }
        if(statusMsg[0]){SelectObject(dc,fSmall);SetTextColor(dc,C_RED);RECT re={0,H_WIN-50,W_WIN,H_WIN-18};DrawText(dc,statusMsg,-1,&re,DT_CENTER|DT_SINGLELINE);}
    }
    else if(curPage==PG_CONFIRM){
        SelectObject(dc,fHuge); SetTextColor(dc,C_TEXT);
        RECT rh={0,H_TOP+24,W_WIN,H_TOP+98}; DrawText(dc,"Confirmation",-1,&rh,DT_CENTER|DT_SINGLELINE);
        int cxc=cx-10,cyc=H_TOP+120;
        HBRUSH bcd=CreateSolidBrush(C_CARD); RECT rc={cxc,cyc,cxc+480,cyc+214}; FillRect(dc,&rc,bcd); DeleteObject(bcd);
        HBRUSH bba=CreateSolidBrush(C_ACCENT); RECT rba={cxc,cyc,cxc+5,cyc+214}; FillRect(dc,&rba,bba); DeleteObject(bba);
        HBRUSH bsp=CreateSolidBrush(C_BORDER);
        RECT rs1={cxc+18,cyc+68,cxc+456,cyc+69}; FillRect(dc,&rs1,bsp);
        RECT rs2={cxc+18,cyc+136,cxc+456,cyc+137}; FillRect(dc,&rs2,bsp); DeleteObject(bsp);
        SelectObject(dc,fSmall); SetTextColor(dc,C_MUTED);
[03/05/2026 09:08] Angelo William: TextOut(dc,cxc+20,cyc+16,"Votre identifiant",17);
        TextOut(dc,cxc+20,cyc+84,"Candidat choisi",15);
        TextOut(dc,cxc+20,cyc+152,"Type de vote",12);
        char bufE[96]; sprintf(bufE,"%s  (ID : %d)",username,myIDElec);
        char bufC[96]; if(choixID==0)strcpy(bufC,"Vote blanc");else sprintf(bufC,"%s  (ID : %d)",choixNom,choixID);
        SelectObject(dc,fBold); SetTextColor(dc,C_TEXT);
        TextOut(dc,cxc+20,cyc+38,bufE,strlen(bufE));
        TextOut(dc,cxc+20,cyc+106,bufC,strlen(bufC));
        SelectObject(dc,fNorm); SetTextColor(dc,choixID==0?C_MUTED:C_ACCENT);
        TextOut(dc,cxc+20,cyc+174,choixID==0?"Vote Blanc":"Vote Nominatif",choixID==0?10:15);
        if(voteDeadline>0){
            char tbuf[64]; FormatVoteCountdown(tbuf,sizeof(tbuf));
            SelectObject(dc,fSmall); SetTextColor(dc,GetVoteRemainingSeconds()<=60?C_ORANGE:C_ACCENT);
            RECT rt={0,H_TOP+348,W_WIN,H_TOP+374}; DrawText(dc,tbuf,-1,&rt,DT_CENTER|DT_SINGLELINE);
        }
        SelectObject(dc,fSmall); SetTextColor(dc,C_ORANGE);
        RECT rw={0,H_TOP+380,W_WIN,H_TOP+408}; DrawText(dc,"Ce vote est definitif et ne peut pas etre annule.",-1,&rw,DT_CENTER|DT_SINGLELINE);
    }
    else if(curPage==PG_SUCCESS){
        int icx=W_WIN/2,icy=H_TOP+126;
        HBRUSH bgr=CreateSolidBrush(C_GREEN);
        RECT ri={icx-52,icy-52,icx+52,icy+52}; FillRect(dc,&ri,bgr); DeleteObject(bgr);
        HBRUSH bc=CreateSolidBrush(C_BG);
        RECT co1={icx-52,icy-52,icx-38,icy-38};FillRect(dc,&co1,bc);
        RECT co2={icx+38,icy-52,icx+52,icy-38};FillRect(dc,&co2,bc);
        RECT co3={icx-52,icy+38,icx-38,icy+52};FillRect(dc,&co3,bc);
        RECT co4={icx+38,icy+38,icx+52,icy+52};FillRect(dc,&co4,bc); DeleteObject(bc);
        SelectObject(dc,fTitle); SetTextColor(dc,RGB(0,0,0));
        RECT rck={icx-52,icy-52,icx+52,icy+52}; DrawText(dc,"OK",-1,&rck,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        SelectObject(dc,fTitle); SetTextColor(dc,C_GREEN);
        RECT rt2={0,H_TOP+198,W_WIN,H_TOP+240}; DrawText(dc,"Vote enregistre !",-1,&rt2,DT_CENTER|DT_SINGLELINE);
        SelectObject(dc,fNorm); SetTextColor(dc,C_TEXT);
        RECT rs={0,H_TOP+246,W_WIN,H_TOP+274}; DrawText(dc,"Merci de votre participation.",-1,&rs,DT_CENTER|DT_SINGLELINE);
        SelectObject(dc,fSmall); SetTextColor(dc,C_MUTED);
        RECT rs2={60,H_TOP+286,W_WIN-60,H_TOP+340}; DrawText(dc,"Les resultats seront disponibles quand\nl'administrateur fermera le scrutin.",-1,&rs2,DT_CENTER|DT_WORDBREAK);
        char choixStr[80]; if(choixID==0)strcpy(choixStr,"Vote blanc");else sprintf(choixStr,"Candidat : %s",choixNom);
        SelectObject(dc,fBold); SetTextColor(dc,C_ACCENT);
        SIZE sz; GetTextExtentPoint32(dc,choixStr,strlen(choixStr),&sz);
        int bx2=(W_WIN-sz.cx)/2-18, by3=H_TOP+364;
        HBRUSH bbb=CreateSolidBrush(RGB(20,38,74)); RECT rbdg={bx2,by3,bx2+sz.cx+36,by3+sz.cy+16}; FillRect(dc,&rbdg,bbb); DeleteObject(bbb);
        TextOut(dc,bx2+18,by3+8,choixStr,strlen(choixStr));
        SelectObject(dc,fSmall); SetTextColor(dc,C_MUTED);
        RECT rsnk={0,H_TOP+442,W_WIN,H_TOP+470}; DrawText(dc,"En attendant les resultats, jouez au Snake !",-1,&rsnk,DT_CENTER|DT_SINGLELINE);
    }
    else if(curPage==PG_FAIL){
        int icx=W_WIN/2,icy=H_TOP+126;
        HBRUSH bfr=CreateSolidBrush(C_RED);
        RECT ri={icx-52,icy-52,icx+52,icy+52}; FillRect(dc,&ri,bfr); DeleteObject(bfr);
        HBRUSH bc=CreateSolidBrush(C_BG);
        RECT co1={icx-52,icy-52,icx-38,icy-38};FillRect(dc,&co1,bc);
        RECT co2={icx+38,icy-52,icx+52,icy-38};FillRect(dc,&co2,bc);
        RECT co3={icx-52,icy+38,icx-38,icy+52};FillRect(dc,&co3,bc);
        RECT co4={icx+38,icy+38,icx+52,icy+52};FillRect(dc,&co4,bc); DeleteObject(bc);
        SelectObject(dc,fBold); SetTextColor(dc,C_WHITE);
        RECT rck={icx-52,icy-52,icx+52,icy+52}; DrawText(dc,"X",-1,&rck,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        SelectObject(dc,fTitle); SetTextColor(dc,C_RED);
        RECT rt2={0,H_TOP+198,W_WIN,H_TOP+240}; DrawText(dc,"Vote refuse",-1,&rt2,DT_CENTER|DT_SINGLELINE);
[03/05/2026 09:08] Angelo William: SelectObject(dc,fNorm); SetTextColor(dc,C_TEXT);
        RECT rs={70,H_TOP+254,W_WIN-70,H_TOP+412};
        DrawText(dc,statusMsg[0]?statusMsg:
            "Raisons possibles :\n - Vous avez deja vote\n - Le vote est ferme\n - Votre ID ne correspond pas a votre login",
            -1,&rs,DT_CENTER|DT_WORDBREAK);
    }
    else if(curPage==PG_SNAKE){
        SelectObject(dc,fTitle); SetTextColor(dc,C_GREEN); TextOut(dc,SN_OX,H_TOP+12,"SNAKE",5);
        SelectObject(dc,fSmall); SetTextColor(dc,C_MUTED); TextOut(dc,SN_OX+92,H_TOP+20,"En attendant les resultats...",28);
        char sc[32]; sprintf(sc,"Score : %d",sn_score);
        SelectObject(dc,fSnScore); SetTextColor(dc,CS_SCORE);
        SIZE sz; GetTextExtentPoint32(dc,sc,strlen(sc),&sz);
        TextOut(dc,SN_OX+SN_COLS*SN_CELL-sz.cx,H_TOP+14,sc,strlen(sc));
        char ln[32]; sprintf(ln,"Longueur : %d",sn_len);
        SelectObject(dc,fSmall); SetTextColor(dc,C_MUTED);
        TextOut(dc,SN_OX+SN_COLS*SN_CELL-sz.cx,H_TOP+42,ln,strlen(ln));
        DrawSnakeBoard(dc);
        RECT rh={SN_OX,SN_OY+SN_ROWS*SN_CELL+10,SN_OX+SN_COLS*SN_CELL,SN_OY+SN_ROWS*SN_CELL+38};
        SelectObject(dc,fSmall); SetTextColor(dc,C_MUTED);
        DrawText(dc,"Fleches / WASD : bouger     Espace : rejouer     Echap : retour",-1,&rh,DT_CENTER|DT_SINGLELINE);
    }
}
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
[03/05/2026 09:08] Angelo William: /* =========================================================
 * WINPROC
 * ========================================================= */
static LRESULT CALLBACK MainProc(HWND hw,UINT m,WPARAM wp,LPARAM lp){
    switch(m){
    case WM_CREATE:
        hwMain=hw; InitRes(); BuildPage(); return 0;
    case WM_TIMER:
        if(wp==ID_TIMER_SN){SnakeTick(hw); return 0;}
        if(wp==ID_TIMER_NET){
            NetPollAsync(hw);
            if((curPage==PG_VOTE||curPage==PG_CONFIRM)&&GetVoteRemainingSeconds()==0){
                HandleServerShutdown("VOTE_TIMEOUT");
                return 0;
            }
            if(curPage==PG_VOTE||curPage==PG_CONFIRM)InvalidateRect(hw,NULL,FALSE);
            return 0;
        }
        return 0;
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
    case WM_SIZE:
        if(wp!=SIZE_MINIMIZED){
            gWinW=LOWORD(lp);
            gWinH=HIWORD(lp);
            RelayoutPage();
        }
        return 0;
    case WM_GETMINMAXINFO:{
        MINMAXINFO* mmi=(MINMAXINFO*)lp;
        mmi->ptMinTrackSize.x=760;
        mmi->ptMinTrackSize.y=640;
        return 0;}
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
        if(HIWORD(wp)==EN_CHANGE||HIWORD(wp)==LBN_SELCHANGE){
            if(statusMsg[0]){statusMsg[0]='\0';InvalidateRect(hw,NULL,TRUE);}
        }
        if(id==ID_BTN_OK){
            if(curPage==PG_CONNECT){
                GetDlgItemText(hw,ID_E1,serverIP,64);
                if(!serverIP[0]){strcpy(statusMsg,"Entrez une adresse IP.");InvalidateRect(hw,NULL,TRUE);return 0;}
                if(NetConnect()){curPage=PG_LOGIN;BuildPage();}else InvalidateRect(hw,NULL,TRUE);
            }
            else if(curPage==PG_LOGIN){
                GetDlgItemText(hw,ID_E1,username,65);GetDlgItemText(hw,ID_E2,password,65);
                if(!username[0]||!password[0]){strcpy(statusMsg,"Remplissez tous les champs.");InvalidateRect(hw,NULL,TRUE);return 0;}
                if(sock==INVALID_SOCKET&& !NetConnect()){InvalidateRect(hw,NULL,TRUE);return 0;}
                if(NetAuth()){NetReceiveCandidats();curPage=PG_VOTE;BuildPage();}else InvalidateRect(hw,NULL,TRUE);
            }
            else if(curPage==PG_VOTE){
                int sel=(int)SendDlgItemMessage(hw,ID_LIST,LB_GETCURSEL,0,0);
                char idStr[16];GetDlgItemText(hw,ID_E3,idStr,16);myIDElec=atoi(idStr);
                if(sel==LB_ERR){strcpy(statusMsg,"Selectionnez un candidat.");InvalidateRect(hw,NULL,TRUE);return 0;}
                if(!myIDElec){strcpy(statusMsg,"Entrez votre ID electeur.");InvalidateRect(hw,NULL,TRUE);return 0;}
                if(GetVoteRemainingSeconds()==0){strcpy(statusMsg,"Le temps du vote est ecoule.");HandleServerShutdown("VOTE_TIMEOUT");return 0;}
[03/05/2026 09:08] Angelo William: choixID=cands[sel].id; strncpy(choixNom,cands[sel].nom,63);choixNom[63]='\0';
                curPage=PG_CONFIRM;BuildPage();
            }
            else if(curPage==PG_CONFIRM){
                if(GetVoteRemainingSeconds()==0){HandleServerShutdown("VOTE_TIMEOUT");return 0;}
                if(NetVote(myIDElec,choixID)){curPage=PG_SUCCESS;}
                else curPage=PG_FAIL;
                BuildPage();
            }
        }
        if(id==ID_BTN_PLAY&&curPage==PG_SUCCESS){curPage=PG_SNAKE;BuildPage();SetFocus(hw);}
        if(id==ID_BTN_BACK){
            if(curPage==PG_LOGIN){CloseConnection();curPage=PG_CONNECT;BuildPage();}
            else if(curPage==PG_VOTE){CloseConnection();curPage=PG_CONNECT;BuildPage();}
            else if(curPage==PG_CONFIRM){curPage=PG_VOTE;BuildPage();}
            else if(curPage==PG_SUCCESS||curPage==PG_FAIL)DestroyWindow(hw);
        }
        return 0;}
    case WM_DESTROY:
        KillTimer(hw,ID_TIMER_SN);KillTimer(hw,ID_TIMER_NET);FreeRes();
        CloseConnection();
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
        WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME,
        (sw-DEF_W_WIN)/2,(sh-DEF_H_WIN)/2,DEF_W_WIN,DEF_H_WIN,NULL,NULL,hI,NULL);
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
