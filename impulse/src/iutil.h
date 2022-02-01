int BiPushStringFunc(void);
int BiPopStringFunc(void);

void BiArguments(int *ac,char *av[],int mxac,char *src);
int BiCommandNumber(int *cmd,char *in,char *lst[]);
int BiSeparatePathFile(char *pth,char *fil,char *org);

void BiSetFp(FILE *fp);
void BiSetStringList(char *(*lst));
char *BiGetStringFunc(char *str,int x);

void BiInitGetStringFuncEx(void);
char *BiGetStringFuncEx(char str[],int nStr);

void BiCapitalizeString(char str[]);
