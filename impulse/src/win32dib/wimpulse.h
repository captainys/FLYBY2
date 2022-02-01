#ifdef __cplusplus
extern "C" {
#endif


void BiWinPlug(HDC dc,long wid,long hei,int bitPerPix);
void BiWinUnplug(void);
HPALETTE BiWinCreatePalette(HDC dc);
LOGPALETTE *BiWinGetLogicalPalette(LOGPALETTE *lp,HDC dc);

#ifdef __cplusplus
}
#endif
