#ifdef __cplusplus
extern "C" {
#endif


enum {
	SK_ERR,
	SK_OK
};

enum {
	SK_NO,
	SK_YES
};


int SkRelativePath(char *dst,char *src,char *org);


int SkArguments(int *ac,char *av[],int mxac,char *src);
int SkCommandNumber(int *cmd,char *in,char *lst[]);


void SkSetPrintFunc(int (*f)(char *));
void SkResetPrintFunc(void);
int SkPrintf(char *fmt,...);


void SkPutExt(char *fname,char *ext);
unsigned long SkFileSize(char *fname);
int SkFileExist(char *fn);
int SkSeparatePathFile(char *pth,char *fil,char *org);


void SkStringHead(char **line);
void SkStringTail(char *l);
void SkCapitalize(char l[]);
void SkUncapitalize(char l[]);


#ifdef __cplusplus
}
#endif
