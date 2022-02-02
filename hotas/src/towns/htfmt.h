#include <impulse.h>

/* Public */
int HtFmtInitAllGuiParts(void);
int HtFmtWindowFunc(int kobj,int messId,int argc,EVENT *pev);
void HtFmtInitFdlg(void);
void HtFmtTermFdlg(void);


/* Local */
int HtFmtYesNo(char *msg[3],char *yes,char *no);
void HtFmtMessageBox(char *msg[3],char *btn);

int HtFmtLineInput(char *str,char *msg,char *def);

int HtFmtInputNumbers(real n[3],char *tit[3],char *msg,char *yes,char *no);
int HtFmtInputInteger(int n[3],char *tit[3],char *msg,char *yes,char *no);

int HtFmtFileDialog(char *fn,char *msg,char *df,char *ext);
int HtFmtColorDialog(BICOLOR *col,BICOLOR *def,char *msg);

void HtFmtCentering(int kobj);
void HtFmtRepaintWindow(int WinId);
void HtFmtRepaintWindowUserArea(int WinId);
void HtFmtExecAlertObj(int alertObj,int basobj);


int HtFmtListBoxSingle
        (int *ret,char *val[],char *mes,char *yes,char *no);
int HtFmtListBoxPl
        (int *nRet,int ret[],char *val[],char *mes,char *yes,char *no);
