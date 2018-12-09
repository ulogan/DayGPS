/*
** Astrolog (Version 5.05) File: extern.h
**
** IMPORTANT NOTICE: The graphics database and chart display routines
** used in this program are Copyright (C) 1991-1995 by Walter D. Pullen
** (astara@u.washington.edu). Permission is granted to freely use and
** distribute these routines provided one doesn't sell, restrict, or
** profit from them in any way. Modification is allowed provided these
** notices remain with any altered or edited versions of the program.
**
** The main planetary calculation routines used in this program have
** been Copyrighted and the core of this program is basically a
** conversion to C of the routines created by James Neely as listed in
** Michael Erlewine's 'Manual of Computer Programming for Astrologers',
** available from Matrix Software. The copyright gives us permission to
** use the routines for personal use but not to sell them or profit from
** them in any way.
**
** The PostScript code within the core graphics routines are programmed
** and Copyright (C) 1992-1993 by Brian D. Willoughby
** (brianw@sounds.wa.com). Conditions are identical to those above.
**
** The extended accurate ephemeris databases and formulas are from the
** calculation routines in the program "Placalc" and are programmed and
** Copyright (C) 1989,1991,1993 by Astrodienst AG and Alois Treindl
** (alois@azur.ch). The use of that source code is subject to
** regulations made by Astrodienst Zurich, and the code is not in the
** public domain. This copyright notice must not be changed or removed
** by any user of this program.
**
** Initial programming 8/28,30, 9/10,13,16,20,23, 10/3,6,7, 11/7,10,21/1991.
** X Window graphics initially programmed 10/23-29/1991.
** PostScript graphics initially programmed 11/29-30/1992.
** Last code change made 8/18/1995.
*/

/*
******************************************************************************
** Function Declarations.
******************************************************************************
*/

#ifdef PROTO
#define P(t) t
#else
#define P(t) ()
#endif

/* From astrolog.c */

extern void InitColors P((void));
extern void Action P((void));
extern void InitVariables P((void));
extern bool FProcessCommandLine P((char *));
extern int NParseCommandLine P((char *, char **));
extern int NPromptSwitches P((char *, char *[MAXSWITCHES]));
extern int NProcessSwitchesRare P((int, char **, int, bool, bool, bool));
extern bool FProcessSwitches P((int, char **));


/* From data.c & data2.c */

#define MM ciCore.mon
#define DD ciCore.day
#define YY ciCore.yea
#define TT ciCore.tim
#define ZZ ciCore.zon
#define SS ciCore.dst
#define OO ciCore.lon
#define AA ciCore.lat

#define Mon ciMain.mon
#define Day ciMain.day
#define Yea ciMain.yea
#define Tim ciMain.tim
#define Zon ciMain.zon
#define Dst ciMain.dst
#define Lon ciMain.lon
#define Lat ciMain.lat

#define Mon2 ciTwin.mon
#define Day2 ciTwin.day
#define Yea2 ciTwin.yea
#define Tim2 ciTwin.tim
#define Zon2 ciTwin.zon
#define Dst2 ciTwin.dst
#define Lon2 ciTwin.lon
#define Lat2 ciTwin.lat

#define ignorea(a) (rAspOrb[a] <= 0.0)

extern US NPTR us;
extern IS NPTR is;
extern CI ciCore, ciMain, ciTwin, ciSave;
extern CP cp1, cp2;
extern FILE *S;
extern real MC, Asc, T, RA;

extern real planet[objMax], planetalt[objMax], house[cSign+1], ret[objMax],
  spacex[oNorm+1], spacey[oNorm+1], spacez[oNorm+1], force[objMax];
extern GridInfo FPTR *grid;
extern byte inhouse[objMax];
extern int starname[cStar+1], kObjA[objMax];

extern byte ignore[objMax], ignore2[objMax];
extern real rAspAngle[cAspect+1], rAspOrb[cAspect+1], rObjOrb[oNorm+1],
  rObjAdd[oNorm+1];
extern CONST int ruler2[oNorm+1], exalt[oNorm+1], rules[cSign+1];
extern int ruler1[oNorm+1], kMainA[9], kRainbowA[8], kElemA[4],
  kAspA[cAspect+1];
extern real rObjInf[oNorm+3], rHouseInf[cSign+3], rAspInf[cAspect+1],
  rTransitInf[oNorm+3];

extern CONST char *szAppName, *szSignName[cSign+1], *szSignAbbrev[cSign+1],
  *szSignEnglish[cSign+1], *szHouseTradition[cSign+1], *szSystem[cSystem],
  *szAspectName[cAspect+1], *szAspectGlyph[cAspect+1],
  *szAspectConfig[cAspConfig+1], *szElem[4], *szMode[3], *szMonth[cSign+1],
  *szDay[cWeek], *szZon[cZone], *szDir[4], *szSuffix[cSign+1];
extern CONST real rZon[cZone];
extern char *szObjName[objMax], *szAspectAbbrev[cAspect+1];
extern CONST char *szCnstlName[cCnstl+1], *szCnstlAbbrev[cCnstl+1],
  *szCnstlMeaning[cCnstl+1], *szCnstlGenitive[cCnstl+1];
extern CONST real rStarBright[cStar+1], rStarData[cStar*6];
extern char *szMindPart[oNorm+1], *szDesc[cSign+1], *szDesire[cSign+1],
  *szLifeArea[cSign+1], *szInteract[cAspectInt+1], *szTherefore[cAspectInt+1],
  *szModify[3][cAspectInt];

extern CONST real rObjDist[oVes+1], rObjYear[oVes+1], rObjDiam[oVes+1],
  rObjDay[oPlu+1], rObjMass[oPlu+1], rObjAxis[oPlu+1];
extern CONST byte cSatellite[oPlu+1];
extern CONST AI ai[cPart];

extern CONST byte rErrorCount[oPlu-oJup+1];
extern CONST byte rErrorOffset[oPlu-oJup+1];
extern CONST real rErrorData[72+51+42*3];
extern OE rgoe[oVes-1+cUran];
extern char *szMacro[48];
extern CONST char *szColor[cColor];


/* From general.c */

#define PrintAltitude(deg) PrintSz(SzAltitude(deg))
#define ErrorValR(sz, r) ErrorValN(sz, (int)r)

extern void SwapR P((real *, real *));
extern int CchSz P((CONST char *));
extern int NCompareSz P((CONST char *, CONST char *));
extern void ClearB P((lpbyte, int));
extern void CopyRgb P((byte *, byte *, int));
extern real RSgn P((real));
extern real Angle P((real, real));
extern real Mod P((real));
extern real ModRad P((real));
extern long Dvd P((long, long));
extern int Mod12 P((int));
extern real DecToDeg P((real));
extern real DegToDec P((real));
extern real MinDistance P((real, real));
extern real MinDifference P((real, real));
extern real Midpoint P((real, real));
extern char Dignify P((int, int));
extern int DayInMonth P((int, int));
extern int DaysInMonth P((int, int));
extern int DayOfWeek P((int, int, int));
extern int AddDay P((int, int, int, int));
extern real GetOrb P((int, int, int));
extern void Terminate P((int));
extern void PrintSz P((CONST char *));
extern void PrintCh P((char));
extern void PrintSzScreen P((char *));
extern void PrintNotice P((char *));
extern void PrintWarning P((char *));
extern void PrintError P((char *));
extern void ErrorArgc P((char *));
extern void ErrorValN P((char *, int));
extern void ErrorArgv P((char *));
extern void ErrorSwitch P((char *));
extern void ErrorEphem P((char *, long));
extern void PrintTab P((char, int));
extern void AnsiColor P((int));
extern void PrintZodiac P((real));
extern char *SzZodiac P((real));
extern char *SzAltitude P((real));
extern char *SzDegree P((real));
extern char *SzDate P((int, int, int, int));
extern char *SzTime P((int, int));
extern char *SzTim P((real));
extern char *SzZone P((real));
extern char *SzLocation P((real, real));
extern void GetTimeNow P((int *, int *, int *, real *, real));
extern int NFromAltN P((int));
extern char *ProcessProgname P((char *));
extern char *SzPersist P((char *));
extern lpbyte PAllocate P((long, bool, char *));


/* From io.c */

extern FILE *FileOpen P((char *, int));
extern bool FProcessSwitchFile P((char *, FILE *));
extern bool FOutputData P((void));
extern int NParseSz P((char *, int));
extern real RParseSz P((char *, int));
extern void InputString P((char *, char *));
extern int NInputRange P((char *, int, int, int));
extern real RInputRange P((char *, real, real, int));
extern bool FInputData P((char *));


/* From calc.c */

#define RBiorhythm(day, rate) (RSin(((day)/(rate))*rPi2)*100.0)

extern int HousePlaceIn P((real));
extern void ComputeInHouses P((void));
extern void HouseAlcabitius P((void));
extern void HouseEqualMidheaven P((void));
extern void HousePorphyryNeo P((void));
extern void HouseWhole P((void));
extern void HouseNull P((void));
extern void ComputeHouses P((int));
extern void ComputeStars P((real));
extern real Decan P((real));
extern void SphToRec P((real, real, real, real *, real *, real *));
extern void ComputePlacalc P((real));
extern real CastChart P((bool));
extern bool FEnsureGrid P((void));
extern bool FAcceptAspect P((int, int, int));
extern void GetAspect P((real *, real *, real *, real *, int, int));
extern void GetParallel P((real *, real *, real *, real *, int, int));
extern bool FCreateGrid P((bool));
extern bool FCreateGridRelation P((bool));
extern void CreateElemTable P((ET *));


/* From matrix.c */

#define EclToEqu(Z, L) CoorXform(Z, L, RFromD(rAxis))
#define EquToEcl(Z, L) CoorXform(Z, L, RFromD(-rAxis))
#define EquToLocal(Z, L, T) CoorXform(Z, L, T)
#define JulianDayFromTime(t) ((t)*36525.0+2415020.0)
#define IoeFromObj(obj) \
  (obj < oMoo ? 0 : (obj <= cPlanet ? obj-2 : obj-uranLo+cPlanet-1))
#define Tropical(deg) (deg - is.rSid + us.rZodiacOffset)

extern real MC, Asc, RA, OB;

extern long MdyToJulian P((int, int, int));
extern real MdytszToJulian P((int, int, int, real, real, real));
extern void JulianToMdy P((real, int *, int *, int *));
extern real ProcessInput P((bool));
extern void PolToRec P((real, real, real *, real *));
extern void RecToPol P((real, real, real *, real *));
extern real RecToSph P((real, real, real));
extern void CoorXform P((real *, real *, real));
extern void ComputeVariables P((real *));
extern real CuspMidheaven P((void));
extern real CuspAscendant P((void));
extern real CuspEastPoint P((void));
extern real CuspPlacidus P((real, real, bool));
extern void HousePlacidus P((void));
extern void HouseKoch P((void));
extern void HouseEqual P((void));
extern void HouseCampanus P((void));
extern void HouseMeridian P((void));
extern void HouseRegiomontanus P((void));
extern void HousePorphyry P((void));
extern void HouseMorinus P((void));
extern real CuspTopocentric P((real));
extern void HouseTopocentric P((void));
extern real ReadThree P((real, real, real));
extern void RecToSph2 P((real, real, real, real *, real *, real *));
extern void ErrorCorrect P((int, real *, real *, real *));
extern void ProcessPlanet P((int, real));
extern void ComputePlanets P((void));
extern void ComputeLunar P((real *, real *, real *, real *));


#ifdef PLACALC
/* From placalc.c & placalc2.c */

extern bool FPlacalcPlanet
  P((int, double, int, real *, real *, real *, real *));
extern double julday P((int, int, int, double, int));
extern void revjul P((double, int, int *, int *, int *, double *));
#endif


/* From charts0.c */

extern void PrintW P((char *, int));
extern void DisplayCredits P((void));
extern void PrintS P((char *));
extern void DisplaySwitches P((void));
extern void DisplaySwitchesRare P((void));
extern void PrintObjects P((void));
extern void PrintAspects P((void));
extern void PrintSigns P((void));
#ifdef CONSTEL
extern char *GetSzGenitive P((char *, char *));
extern void GetSzConstel P((char *, int));
extern void PrintConstellations P((void));
#endif
extern void PrintOrbit P((void));
extern int NCompareSzPart P((int, int));
extern void DisplayArabic P((void));
#ifdef GRAPH
extern void DisplayKeysX P((void));
extern void DisplaySwitchesX P((void));
#ifdef WIN
extern void DisplaySwitchesW P((void));
#endif
#endif /* GRAPH */
extern bool FPrintTables P((void));


/* From charts1.c */

extern void ChartListing P((void));
extern void ChartGrid P((void));
extern void PrintGrand P((char, int, int, int, int));
extern void DisplayGrands P((void));
extern void PrintHouse P((int, int));
extern void PrintWheelCenter P((int));
extern void PrintWheelSlot P((int));
extern void ChartWheel P((void));
extern void ChartAspect P((void));
extern void ChartMidpoint P((void));
extern void ChartHorizon P((void));
extern void ChartOrbit P((void));
extern void ChartAstroGraph P((void));
extern void PrintChart P((bool));


/* From charts2.c */

extern void ChartGridRelation P((void));
extern void ChartAspectRelation P((void));
extern void ChartMidpointRelation P((void));
extern void CastRelation P((bool));
extern void PrintInDay P((int, int, int));
extern void PrintAspect P((int, int, int, int, int, int, int, char));
extern void ChartInDayInfluence P((void));
extern void ChartTransitInfluence P((bool));
extern void EclToHorizon P((real *, real *, real, real, real, real, real));
extern void ChartCalendarMonth P((void));
extern void ChartCalendarYear P((void));
extern void DisplayRelation P((void));


/* From charts3.c */

extern void ChartInDaySearch P((bool));
extern void ChartTransitSearch P((bool));
extern void ChartInDayHorizon P((void));
extern void ChartEphemeris P((void));


/* From intrpret.c */

#ifdef INTERPRET
extern void FieldWord P((char *));
extern void InterpretGeneral P((void));
extern void InterpretAspectGeneral P((void));
extern void InterpretLocation P((void));
extern void InterpretAspect P((int, int));
extern void InterpretGrid P((void));
extern void InterpretMidpoint P((int, int));
extern void InterpretInDay P((int, int, int));
extern void InterpretTransit P((int, int, int));
extern void InterpretSynastry P((void));
extern void InterpretAspectRelation P((int, int));
extern void InterpretGridRelation P((void));
extern void InterpretMidpointRelation P((int, int));
#endif
extern void SortRank P((real *, int *, int));
extern void ChartInfluence P((void));


#ifdef GRAPH
/* From xdata.c */

extern GS NPTR gs;
extern GI NPTR gi;

#ifdef X11
extern XSizeHints hint;
extern char xkey[];
extern char *szColorX[cColor];
extern KV rgbind[cColor];
extern KV fg, bg;
#endif
#ifdef WIN
extern int ikPalette[cColor];
#endif

extern CONST KV rgb[cColor], rgbbmp[cColor];
extern KI kMainB[9], kRainbowB[8], kElemB[4], kAspB[cAspect+1], kObjB[objMax];

extern char szObjectFont[oNorm+1], *szDrawSign[cSign+3],
  *szDrawObject[oNorm+8];
extern CONST char szAspectFont[cAspect+1], *szDrawHouse[cSign+1],
  *szDrawAspect[cAspect+1], *szDrawCh[128-32+1], *szWorldData[62*3],
  *szDrawConstel[cCnstl+1];


/* From xgeneral.c */

#define BBmGet(B, X, Y) ((B)[(long)(Y)*(long)(gi.cbBmpRow) + ((X) >> 1)])
#define FBmGet(B, X, Y) (BBmGet(B, X, Y) >> (((X)&1^1) << 2) & 15)
#define BmSet(B, X, Y, O) BBmGet(B, X, Y) = BBmGet(B, X, Y) & \
  15 << (((X)&1) << 2) | (O) << (((X)&1^1) << 2)

#define PutByte(A) putc((byte) (A), file)
#define PutWord(A) PutByte(BLo(A)); PutByte(BHi(A))
#define PutLong(A) PutWord(WLo(A)); PutWord(WHi(A))

#define DrawEdge(X1, Y1, X2, Y2) DrawBox(X1, Y1, X2, Y2, 1, 1)
#define DrawEdgeAll() DrawEdge(0, 0, gs.xWin-1, gs.yWin-1)
#define DrawLine(X1, Y1, X2, Y2) DrawDash(X1, Y1, X2, Y2, 0)
#define DrawCircle(X, Y, RX, RY) \
  DrawEllipse((X)-(RX), (Y)-(RY), (X)+(RX), (Y)+(RY))

extern void DrawColor P((KI));
extern void DrawPoint P((int, int));
extern void DrawSpot P((int, int));
extern void DrawBlock P((int, int, int, int));
extern void DrawBox P((int, int, int, int, int, int));
extern void DrawClearScreen P((void));
extern void DrawDash P((int, int, int, int, int));
extern void DrawWrap P((int, int, int, int, int, int));
extern void ClipLesser P((int *, int *, int *, int *, int));
extern void ClipGreater P((int *, int *, int *, int *, int));
extern void DrawClip P((int, int, int, int, int, int, int, int, int));
extern void DrawEllipse P((int, int, int, int));
extern void DrawSz P((CONST char *, int, int, int));
extern void DrawSign P((int, int, int));
extern void DrawHouse P((int, int, int));
extern void DrawObject P((int, int, int));
extern void DrawAspect P((int, int, int));
extern int NFromPch P((CONST char **));
extern void DrawTurtle P((CONST char *, int, int));


/* From xdevice.c */

#ifdef META
/* Macros to output the various metafile commands used. */

#define MetaRecord(S, R) MetaLong((long)(S)); MetaWord(R)
#define MetaSelectObject(O) MetaRecord(4, 0x12D); MetaWord(O)
#define MetaDeleteObject(O) MetaRecord(4, 0x1F0); MetaWord(O)
#define MetaSaveDc() MetaRecord(3, 0x01E)
#define MetaRestoreDc() MetaRecord(4, 0x127); MetaWord((word)-1)
#define MetaWindowOrg(X, Y) MetaRecord(5, 0x20B); MetaWord(Y); MetaWord(X)
#define MetaWindowExt(X, Y) MetaRecord(5, 0x20C); MetaWord(Y); MetaWord(X)
#define MetaCreatePen(S, W, C) MetaRecord(8, 0x2FA); MetaWord(S); \
  MetaWord(W); MetaWord(W); MetaLong(C)
#define MetaCreateBrush(S, C) MetaRecord(7, 0x2FC); \
  MetaWord(S); MetaLong(C); MetaWord(0 /* Not used */);
#define MetaCreateFont(S, X, Y, C) MetaRecord(12+(S), 0x2FB); MetaWord(Y); \
  MetaWord(X); MetaWord(0 /* Angle */); MetaWord(0 /* Not used */); \
  MetaWord(400 /* Normal Weight */); MetaWord(0 /* Italic, Underline */); \
  MetaWord(WFromBB(0 /* Strikeout */, C)); \
  MetaWord(WFromBB(4 /* TrueType */, 0 /* Clip */))
#define MetaBkMode(M) MetaRecord(4, 0x102); MetaWord(M)
#define MetaTextAlign(A) MetaRecord(4, 0x12E); MetaWord(A)
#define MetaTextColor(C) MetaRecord(5, 0x209); MetaLong(C);
#define MetaTextOut(X, Y, S) MetaRecord(7+((S)+1)/2, 0xA32); \
  MetaWord(Y); MetaWord(X); MetaWord(S); MetaWord(0 /* ETO */)
#define MetaRectangle(X1, Y1, X2, Y2) MetaRecord(7, 0x41B); \
  MetaWord(Y2); MetaWord(X2); MetaWord(Y1); MetaWord(X1)
#define MetaEllipse(X1, Y1, X2, Y2) MetaRecord(7, 0x418); \
  MetaWord(Y2); MetaWord(X2); MetaWord(Y1); MetaWord(X1)
#define MetaEscape(S) MetaRecord((S), 0x626); \
  MetaWord(15 /* MFCOMMENT */); MetaWord(((S)-5)*2 /* Bytes in comment */);
#endif /* META */

extern void WriteXBitmap P((FILE *, char *, char));
extern void WriteAscii P((FILE *));
extern void WriteBmp P((FILE *));
extern void BeginFileX P((void));
extern void EndFileX P((void));
extern void PsStrokeForce P((void));
extern void PsStroke P((int));
extern void PsLineCap P((bool));
extern void PsDash P((int));
extern void PsLineWidth P((int));
extern void PsFont P((int));
extern void PsBegin P((void));
extern void PsEnd P((void));
extern void MetaWord P((word));
extern void MetaLong P((long));
extern void MetaSelect P((void));
extern void MetaInit P((void));
extern void WriteMeta P((FILE *));
extern int MouseInit P((int, int));
extern void MouseShow P((bool));
extern bool MouseStatus P((int *, int *, int *));


/* From xcharts0.c */

extern int DrawPrint P((char *, int, int));
extern void DrawInfo P((void));
extern bool FReadWorldData P((char FPTR **, char FPTR **, char FPTR **));
extern bool FGlobeCalc P((real, real, int *, int *, int, int, int, int, int));
extern void DrawLeyLine P((real, real, real, real));
extern void DrawLeyLines P((int));
extern void DrawMap P((bool, bool, int));
extern void DrawChartX P((void));


/* From xcharts1.c */

extern void XChartWheel P((void));
extern void XChartAstroGraph P((void));
extern void XChartGrid P((void));
extern void XChartHorizon P((void));
extern void XChartHorizonSky P((void));
extern void XChartOrbit P((void));
extern void DrawArrow P((int, int, int, int));
extern void XChartDispositor P((void));
extern void XChartCalendar P((void));
extern void XChartEphemeris P((void));


/* From xcharts2.c */

extern bool FProper P((int));
extern void FillSymbolRing P((real *));
extern void FillSymbolLine P((real *));
extern real HousePlaceInX P((real));
extern void XChartWheelRelation P((void));
extern void XChartGridRelation P((void));
extern void XChartBiorhythm P((void));


/* From xscreen.c */

extern void InitColorsX P((void));
#ifdef ISG
extern void BeginX P((void));
extern void AddTime P((int, int));
extern void Animate P((int, int));
extern void CommandLineX P((void));
extern void SquareX P((int *, int *, int));
extern void InteractX P((void));
extern void EndX P((void));
#endif
extern int NProcessSwitchesX P((int, char **, int, bool, bool, bool));
extern int NProcessSwitchesRareX P((int, char **, int));
extern bool FActionX P((void));
#endif /* GRAPH */


#ifdef WIN
/* From wdriver.c & wdialog.c */

extern WI NPTR wi;
extern OPENFILENAME ofn;

#define TextClearScreen() PatBlt(wi.hdc, 0, 0, wi.cxClient, wi.cyClient, \
  us.fAnsi || !gs.fInverse ? BLACKNESS : WHITENESS);
#define CheckMenu(cmd, f) \
  CheckMenuItem(wi.hmenu, (_int)cmd, f ? MF_CHECKED : MF_UNCHECKED);
#define WiCheckMenu(cmd, f) CheckMenu(cmd, f); wi.fMenu = fTrue
#define WiDoDialog(pfn, dlg) \
  dlgproc = (DLGPROC)MakeProcInstance(pfn, wi.hinst); \
  DialogBox(wi.hinst, MAKEINTRESOURCE(dlg), wi.hwnd, dlgproc); \
  FreeProcInstance((FARPROC)dlgproc)

#define SetCheck(id, f) CheckDlgButton(hdlg, id, f)
#define SetRadio(id, idLo, idHi) CheckRadioButton(hdlg, idLo, idHi, id)
#define SetEdit(id, sz) SetDlgItemText(hdlg, id, (LPCSTR)sz)
#define SetEditN(id, n) SetDlgItemInt(hdlg, id, n, fTrue)
#define SetCombo(id, sz) \
  SendDlgItemMessage(hdlg, id, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)sz)
#define ClearCombo(id) SendDlgItemMessage(hdlg, id, CB_RESETCONTENT, 0, 0);
#define GetCheck(id) IsDlgButtonChecked(hdlg, id)
#define GetEdit(id, sz) GetDlgItemText(hdlg, id, sz, cchSzDef)
#define GetEditN(id) GetDlgItemInt(hdlg, id, NULL, fTrue)
#define EnsureN(n, f, sz) if (!(f)) { ErrorEnsure(n, sz); return fTrue; }
#define EnsureR(r, f, sz) EnsureN((int)r, f, sz)

extern LONG API WndProc P((HWND, WORD, WORD, LONG));
extern int NProcessSwitchesW P((int, char **, int, bool, bool, bool));
extern void ResizeWindowToChart P((void));
extern WORD WCmdFromRc P((int));
extern void SetRel P((int));
extern void ProcessState P((void));
extern int NWmCommand P((WORD));
extern void API RedoMenu P((void));
extern bool API FRedraw P((void));

extern void SetEditSz(HWND, int, char *);
extern void SetEditR(HWND, int, real, int);
extern void SetEditMDYT(HWND, int, int, int, int, int, int, int, real);
extern void SetEditSZOA(HWND, int, int, int, int, real, real, real, real);
extern void SetEditColor(HWND, int, KI);
extern real GetEditR(HWND, int);
extern void ErrorEnsure(int, char *);
extern bool API DlgOpenChart P((void));
extern bool API DlgSaveChart P((void));
extern bool API DlgCommand P((HWND, _int, WORD, LONG));
extern bool API DlgColor P((HWND, _int, WORD, LONG));
extern bool API DlgInfo P((HWND, _int, WORD, LONG));
extern bool API DlgDefault P((HWND, _int, WORD, LONG));
extern bool API DlgAspect P((HWND, _int, WORD, LONG));
extern bool API DlgObject P((HWND, _int, WORD, LONG));
extern bool API DlgRestrict P((HWND, _int, WORD, LONG));
extern bool API DlgStar P((HWND, _int, WORD, LONG));
extern bool API DlgSetting P((HWND, _int, WORD, LONG));
extern bool API DlgObscure P((HWND, _int, WORD, LONG));
extern bool API DlgTransit P((HWND, _int, WORD, LONG));
extern bool API DlgProgress P((HWND, _int, WORD, LONG));
extern bool API DlgChart P((HWND, _int, WORD, LONG));
extern bool API DlgGraphics P((HWND, _int, WORD, LONG));
extern bool API DlgAbout P((HWND, _int, WORD, LONG));
#endif /* WIN */

/* extern.h */
