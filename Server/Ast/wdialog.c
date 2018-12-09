/*
** Astrolog (Version 5.05) File: wdialog.c
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
** Last code change made 6/1/1995.
*/

#include "astrolog.h"


#ifdef WIN
/*
******************************************************************************
** Dialog Utility Functions.
******************************************************************************
*/

/* Set the contents of the given edit control in a dialog to a string. */

void SetEditSz(hdlg, id, sz)
HWND hdlg;
int id;
char *sz;
{
  while (*sz == ' ')    /* Strip off any extra leading spaces. */
    sz++;
  SetEdit(id, sz);
}


/* Set the contents of the given edit control in a dialog to a floating */
/* point value, with at most 'n' significant fractional digits.         */

void SetEditR(hdlg, id, r, n)
HWND hdlg;
int id;
real r;
int n;
{
  char sz[cchSzDef], szT[8], *pch;

  sprintf(szT, "%%.%df", n);
  sprintf(sz, szT, r);
  for (pch = sz; *pch; pch++)
    ;
  while (*(--pch) == '0')          /* Drop off any trailing zero digits. */
    ;
  pch[1 + (*pch == '.')] = chNull; /* Ensure at least one fractional digit. */
  SetEdit(id, sz);
}


/* Set the contents of four combo controls and their dropdowns in a dialog */
/* indicating month, day, year, and time fields to the given values.       */

void SetEditMDYT(hdlg, idMon, idDay, idYea, idTim, mon, day, yea, tim)
HWND hdlg;
int idMon, idDay, idYea, idTim;
int mon, day, yea;
real tim;
{
  char sz[cchSzDef];
  int i;

  ClearCombo(idMon);
  ClearCombo(idDay);
  ClearCombo(idYea);
  ClearCombo(idTim);
  if (!FValidMon(mon))
    mon = 1;
  sprintf(sz, "%c%c%c", chMon3(mon));
  SetEdit(idMon, sz);
  for (i = 1; i <= cSign; i++)
    SetCombo(idMon, szMonth[i]);
  if (!FValidDay(day, mon, yea))
    day = 1;
  SetEditN(idDay, day);
  SetCombo(idDay, "1"); SetCombo(idDay, "15");
  SetEditN(idYea, yea);
  for (i = 1990; i < 2000; i++) {
    sprintf(sz, "%d", i); SetCombo(idYea, sz);
  }
  sprintf(sz, "%s", SzTim(tim));
  SetEditSz(hdlg, idTim, sz);
  SetCombo(idTim, "Midnight");
  SetCombo(idTim, (char *)(us.fEuroTime ? "6:00" : "6:00am"));
  SetCombo(idTim, "Noon");
  SetCombo(idTim, (char *)(us.fEuroTime ? "18:00" : "6:00pm"));
}


/* Set the contents of four combo controls in a dialog indicating daylight, */
/* time zone, longitude, and latitude fields to the given values.           */

void SetEditSZOA(hdlg, idDst, idZon, idLon, idLat, dst, zon, lon, lat)
HWND hdlg;
int idDst, idZon, idLon, idLat;
real dst, zon, lon, lat;
{
  char sz[cchSzDef];
  int i;
  bool fT;

  if (dst == 0.0 || dst == 1.0)
    sprintf(sz, "%s", dst == 0.0 ? "No" : "Yes");
  else
    sprintf(sz, "%.2f", dst);
  SetEdit(idDst, sz);
  SetCombo(idDst, "No"); SetCombo(idDst, "Yes");
  sprintf(sz, "%s", SzZone(-zon));
  SetEdit(idZon, (char *)(sz[0] == '+' ? &sz[1] : sz));
  /* For the time zone dropdown, fill it out with all abbreviations of */
  /* three letters that don't reference daylight or war time.          */
  for (i = 0; i < cZone; i++) {
    if (szZon[i][1] && szZon[i][1] != 'D' && szZon[i][1] != 'W' &&
      szZon[i][2] && szZon[i][2] != 'D')
      SetCombo(idZon, szZon[i]);
  }
  fT = us.fAnsi; us.fAnsi = fFalse;
  sprintf(sz, "%s", SzLocation(lon, lat));
  us.fAnsi = fT;
  sz[7] = chNull;
  SetEditSz(hdlg, idLon, &sz[0]);
  SetCombo(idLon, "122W20"); SetCombo(idLon, "0E00");
  SetEditSz(hdlg, idLat, &sz[8]);
  SetCombo(idLat, "47N36"); SetCombo(idLat, "0S00");
}


/* Set the contents of a combo control in a dialog indicating a color   */
/* field to the given value, and fill its dropdown with the color list. */

void SetEditColor(hdlg, id, ki)
HWND hdlg;
int id;
KI ki;
{
  int i;

  SetEdit(id, szColor[ki]);
  for (i = 0; i < cColor; i++)
    SetCombo(id, szColor[i]);
}


/* Return the contents of a dialog edit control as a floating point value. */

real GetEditR(hdlg, id)
HWND hdlg;
int id;
{
  char sz[cchSzDef];

  GetEdit(id, sz);
  return atof(sz);
}


/* Bring up an error box indicating an illegal value for a dialog field. */

void ErrorEnsure(n, sz)
int n;
char *sz;
{
  char szT[cchSzDef];

  sprintf(szT, "The value %d is not a valid %s setting.", n, sz);
  PrintWarning(szT);
}


/*
******************************************************************************
** Windows Dialogs.
******************************************************************************
*/

/* Bring up the Windows standard file open dialog, and process the        */
/* command file specified if any. This is called from the File Open Chart */
/* and File Open Chart #2 menu commands.                                  */

bool API DlgOpenChart()
{
  CI ciT;

  /* Setup dialog title and settings and get the name of a file from it. */
  if (wi.wCmd == cmdOpenChart)
    ofn.lpstrTitle = "Open Chart";
  else
    ofn.lpstrTitle = "Open Chart #2";
  ofn.lpstrFilter = "All Files (*.*)\0*.*\0Data Files (*.DAT)\0*.DAT\0";
  if (!GetOpenFileName((LPOPENFILENAME)&ofn))
    return fFalse;

  /* Process the given file based on what open command is being run. */
  if (wi.wCmd == cmdOpenChart2)
    ciT = ciCore;
  FInputData(ofn.lpstrFileTitle);
  if (wi.wCmd == cmdOpenChart2) {
    ciTwin = ciCore;
    ciCore = ciT;
  }

  wi.fCast = fTrue;
  return fTrue;
}


/* Bring up the Windows standard file save dialog, get the name of a file   */
/* from the user, and save to it either right away or set variables to      */
/* ensure it will be done later. This is called from all six File Save menu */
/* commands: Save Info, Positions, Text, Bitmap, Picture, and PostScript.   */

bool API DlgSaveChart()
{
  /* Setup dialog title and settings and get the name of a file from it. */
  ofn.lpstrFilter = "All Files (*.*)\0*.*\0Data Files (*.DAT)\0*.DAT\0";
  switch (wi.wCmd) {
  case cmdSaveChart:
    ofn.lpstrTitle = "Save Chart Info";
    break;
  case cmdSavePositions:
    ofn.lpstrTitle = "Save Chart Positions";
    break;
  case cmdSaveText:
    ofn.lpstrTitle = "Save Chart Text";
    ofn.lpstrFilter = "All Files (*.*)\0*.*\0Text Files (*.TXT)\0*.TXT\0";
    break;
  case cmdSaveBitmap:
    ofn.lpstrTitle = "Save Chart Bitmap";
    ofn.lpstrFilter =
      "Windows Bitmap (*.BMP)\0*.BMP\0All Files (*.*)\0*.*\0";
    break;
  case cmdSavePicture:
    ofn.lpstrTitle = "Save Chart Picture";
    ofn.lpstrFilter =
      "Windows Metafile (*.WMF)\0*.WMF\0All Files (*.*)\0*.*\0";
    break;
  case cmdSavePS:
    ofn.lpstrTitle = "Save Chart PostScript";
    ofn.lpstrFilter =
      "PostScript Text (*.EPS)\0*.EPS\0All Files (*.*)\0*.*\0";
    break;
  }
  if (!GetSaveFileName((LPOPENFILENAME)&ofn))
    return fFalse;

  /* Saving chart info or chart position command files can be done right */
  /* away. Saving actual chart output isn't done until the next redraw.  */
  is.szFileOut = gi.szFileOut = ofn.lpstrFile;
  switch (wi.wCmd) {
  case cmdSaveChart:
    us.fWritePos = fFalse;
    return FOutputData();
  case cmdSavePositions:
    us.fWritePos = fTrue;
    return FOutputData();
  case cmdSaveText:
    is.szFileScreen = ofn.lpstrFile;
    us.fGraphics = fFalse;
    wi.fRedraw = fTrue;
    break;
  case cmdSaveBitmap:
    gs.fBitmap = fTrue;
    gs.fMeta = gs.fPS = fFalse;
    us.fGraphics = wi.fRedraw = fTrue;
    break;
  case cmdSavePicture:
    gs.fMeta = fTrue;
    gs.fBitmap = gs.fPS = fFalse;
    us.fGraphics = wi.fRedraw = fTrue;
    break;
  case cmdSavePS:
    gs.fPS = fTrue;
    gs.fBitmap = gs.fMeta = fFalse;
    us.fGraphics = wi.fRedraw = fTrue;
    break;
  }
  return fTrue;
}


/* Processing function for the command switch entry dialog, as brought up */
/* with the Edit Enter Command Line menu command.                         */

bool API DlgCommand(hdlg, message, wParam, lParam)
HWND hdlg;
_int message;
WORD wParam;
LONG lParam;
{
  char sz[cchSzDef];

  switch (message) {
  case WM_INITDIALOG:
    SetFocus(GetDlgItem(hdlg, deCo));
    return fFalse;

  case WM_COMMAND:
    if (wParam == IDOK) {
      GetDlgItemText(hdlg, deCo, sz, cchSzDef);
      FProcessCommandLine(sz);
      wi.fCast = wi.fMenuAll = fTrue;
    }
    if (wParam == IDOK || wParam == IDCANCEL) {
      EndDialog(hdlg, fTrue);
      return fTrue;
    }
    break;
  }
  return fFalse;
}


/* Processing function for the color customization dialog, as brought up */
/* with the View Set Colors menu command.                                */

bool API DlgColor(hdlg, message, wParam, lParam)
HWND hdlg;
_int message;
WORD wParam;
LONG lParam;
{
  char sz[cchSzDef];
  int i, j, k, l;

  switch (message) {
  case WM_INITDIALOG:
    for (i = 0; i < 4; i++)
      SetEditColor(hdlg, dce0 + i, kElemA[i]);
    for (i = 1; i <= cAspect; i++)
      SetEditColor(hdlg, dca1 + i - 1, kAspA[i]);
    for (i = 0; i < cColor; i++) {
      j = ikPalette[i];
      SetEditColor(hdlg, dck0 + i, j <= 0 ? kMainA[-j] : kRainbowA[j]);
    }
    return fTrue;

  case WM_COMMAND:
    if (wParam == IDOK) {
      for (k = 0; k <= 1; k++) {
        for (i = 0; i < 4; i++) {
          GetEdit(dce0 + i, sz);
          l = NParseSz(sz, pmColor);
          if (k)
            kElemA[i] = l;
          else
            EnsureN(l, FValidColor(l), "element color");
        }
        for (i = 1; i <= cAspect; i++) {
          GetEdit(dca1 + i - 1, sz);
          l = NParseSz(sz, pmColor);
          if (k)
            kAspA[i] = l;
          else
            EnsureN(l, FValidColor(l), "aspect color");
        }
        for (i = 0; i < cColor; i++) {
          GetEdit(dck0 + i, sz);
          l = NParseSz(sz, pmColor);
          if (k) {
            j = ikPalette[i];
            if (j <= 0)
              kMainA[-j] = l;
            else
              kRainbowA[j] = l;
          } else
            EnsureN(l, FValidColor(l), "palette color");
        }
      }
      wi.fRedraw = fTrue;
    }
    if (wParam == IDOK || wParam == IDCANCEL) {
      EndDialog(hdlg, fTrue);
      return fTrue;
    }
    break;
  }
  return fFalse;
}


/* Processing function for the chart info entry dialog, as brought up by  */
/* both the Info Set Chart Info and Info Set Chart #2 Info menu commands. */

bool API DlgInfo(hdlg, message, wParam, lParam)
HWND hdlg;
_int message;
WORD wParam;
LONG lParam;
{
  CI ci;
  char sz[cchSzDef];

  switch (message) {
  case WM_INITDIALOG:
    if (wi.wCmd == cmdSetInfo)
      ci = ciMain;
    else {
      SetWindowText(hdlg, "Enter Chart #2 Info");
      ci = ciTwin;
    }
LInit:
    SetEditMDYT(hdlg, dcInMon, dcInDay, dcInYea, dcInTim,
      ci.mon, ci.day, ci.yea, ci.tim);
    SetEditSZOA(hdlg, dcInDst, dcInZon, dcInLon, dcInLat,
      ci.dst, ci.zon, ci.lon, ci.lat);
    SetEditSz(hdlg, deInNam, ciMain.nam);
    SetEditSz(hdlg, deInLoc, ciMain.loc);
    SetFocus(GetDlgItem(hdlg, dcInMon));
    return fFalse;

  case WM_COMMAND:
    if (wParam == dbInNow || wParam == dbInSet) {
      if (wParam == dbInNow) {
        GetTimeNow(&ci.mon, &ci.day, &ci.yea, &ci.tim, us.zonDef-us.dstDef);
        ci.dst = us.dstDef; ci.zon = us.zonDef;
        ci.lon = us.lonDef; ci.lat = us.latDef;
      } else
        ci = ciSave;
      goto LInit;
    }

    if (wParam == IDOK) {
      GetEdit(dcInMon, sz); ci.mon = NParseSz(sz, pmMon);
      GetEdit(dcInDay, sz); ci.day = NParseSz(sz, pmDay);
      GetEdit(dcInYea, sz); ci.yea = NParseSz(sz, pmYea);
      GetEdit(dcInTim, sz); ci.tim = RParseSz(sz, pmTim);
      GetEdit(dcInDst, sz); ci.dst = RParseSz(sz, pmDst);
      GetEdit(dcInZon, sz); ci.zon = RParseSz(sz, pmZon);
      GetEdit(dcInLon, sz); ci.lon = RParseSz(sz, pmLon);
      GetEdit(dcInLat, sz); ci.lat = RParseSz(sz, pmLat);
      EnsureN(ci.mon, FValidMon(ci.mon), "month");
      EnsureN(ci.yea, FValidYea(ci.yea), "year");
      EnsureN(ci.day, FValidDay(ci.day, ci.mon, ci.yea), "day");
      EnsureR(ci.tim, FValidTim(ci.tim), "time");
      EnsureR(ci.dst, FValidZon(ci.dst), "daylight");
      EnsureR(ci.zon, FValidZon(ci.zon), "zone");
      EnsureR(ci.lon, FValidLon(ci.lon), "longitude");
      EnsureR(ci.lat, FValidLat(ci.lat), "latitude");
      GetEdit(deInNam, sz);
      ci.nam = SzPersist(sz);
      GetEdit(deInLoc, sz);
      ci.loc = SzPersist(sz);
      if (wi.wCmd == cmdSetInfo)
        ciMain = ciCore = ci;
      else
        ciTwin = ci;
      wi.fCast = fTrue;
    }
    if (wParam == IDOK || wParam == IDCANCEL) {
      EndDialog(hdlg, fTrue);
      return fTrue;
    }
    break;
  }
  return fFalse;
}


/* Processing function for the default chart info dialog, as brought up */
/* with the Info Default Chart Info menu command.                       */

bool API DlgDefault(hdlg, message, wParam, lParam)
HWND hdlg;
_int message;
WORD wParam;
LONG lParam;
{
  CI ci;
  char sz[cchSzDef];

  switch (message) {
  case WM_INITDIALOG:
    SetEditSZOA(hdlg, dcDeDst, dcDeZon, dcDeLon, dcDeLat,
      us.dstDef, us.zonDef, us.lonDef, us.latDef);
    SetFocus(GetDlgItem(hdlg, dcDeDst));
    return fFalse;

  case WM_COMMAND:
    if (wParam == IDOK) {
      GetEdit(dcDeDst, sz); ci.dst = RParseSz(sz, pmDst);
      GetEdit(dcDeZon, sz); ci.zon = RParseSz(sz, pmZon);
      GetEdit(dcDeLon, sz); ci.lon = RParseSz(sz, pmLon);
      GetEdit(dcDeLat, sz); ci.lat = RParseSz(sz, pmLat);
      EnsureR(ci.dst, FValidZon(ci.dst), "daylight");
      EnsureR(ci.zon, FValidZon(ci.zon), "zone");
      EnsureR(ci.lon, FValidLon(ci.lon), "longitude");
      EnsureR(ci.lat, FValidLat(ci.lat), "latitude");
      us.dstDef = ci.dst; us.zonDef = ci.zon;
      us.lonDef = ci.lon; us.latDef = ci.lat;
      wi.fCast = fTrue;
    }
    if (wParam == IDOK || wParam == IDCANCEL) {
      EndDialog(hdlg, fTrue);
      return fTrue;
    }
    break;
  }
  return fFalse;
}


/* Processing function for the aspect settings dialog, as brought up with */
/* the Setting Aspect Settings menu command.                              */

bool API DlgAspect(hdlg, message, wParam, lParam)
HWND hdlg;
_int message;
WORD wParam;
LONG lParam;
{
  int i, j;
  real r;

  switch (message) {
  case WM_INITDIALOG:
    for (i = 1; i <= cAspect; i++) {
      SetCheck(dxa1 - 1 + i, i > us.nAsp);
      SetEditR(hdlg, deo1 - 1 + i, rAspOrb[i], 2);
      SetEditR(hdlg, dea1 - 1 + i, rAspAngle[i], 6);
      SetEditR(hdlg, dei1 - 1 + i, rAspInf[i], 2);
    }
    return fTrue;

  case WM_COMMAND:
    switch (wParam) {
    case dbAs_RA0:
      for (i = 1; i <= cAspect; i++)
        SetCheck(dxa1 - 1 + i, fTrue);
      break;
    case dbAs_RA1:
      for (i = 1; i <= cAspect; i++)
        SetCheck(dxa1 - 1 + i, fFalse);
      break;
    case dbAs_RA:
      for (i = 1; i <= 5; i++)
        SetCheck(dxa1 - 1 + i, !GetCheck(dxa1 - 1 + i));
      break;
    }

    if (wParam == IDOK) {
      for (j = 0; j <= 1; j++) {
        for (i = 1; i <= cAspect; i++) {
          r = GetEditR(hdlg, deo1 - 1 + i);
          if (j)
            rAspOrb[i] = r;
          else
            EnsureR(r, r >= -rDegMax && r <= rDegMax, "orb");
        }
        for (i = 1; i <= cAspect; i++) {
          r = GetEditR(hdlg, dea1 - 1 + i);
          if (j)
            rAspAngle[i] = r;
          else
            EnsureR(r, r >= -rDegMax && r <= rDegMax, "angle");
        }
        for (i = 1; i <= cAspect; i++) {
          r = GetEditR(hdlg, dei1 - 1 + i);
          if (j)
            rAspInf[i] = r;
        }
      }
      for (us.nAsp = cAspect; us.nAsp > 0 && GetCheck(dxa1 - 1 + us.nAsp);
        us.nAsp--)
        ;
      for (i = 1; i <= us.nAsp; i++) {
        if (GetCheck(dxa1 - 1 + i))
          rAspOrb[i] = -rDegHalf;
      }
      wi.fRedraw = fTrue;
    }
    if (wParam == IDOK || wParam == IDCANCEL) {
      EndDialog(hdlg, fTrue);
      return fTrue;
    }
    break;
  }
  return fFalse;
}


/* Processing function for the object settings dialog, as brought up with */
/* the Setting Object Settings menu command.                              */

bool API DlgObject(hdlg, message, wParam, lParam)
HWND hdlg;
_int message;
WORD wParam;
LONG lParam;
{
  int i, j;
  real r;

  switch (message) {
  case WM_INITDIALOG:
    for (i = 1; i <= oCore; i++) {
      SetEditR(hdlg, deo1 - 1 + i, rObjOrb[i], 2);
      SetEditR(hdlg, dea1 - 1 + i, rObjAdd[i], 1);
      SetEditR(hdlg, dei1 - 1 + i, rObjInf[i], 2);
    }
    return fTrue;

  case WM_COMMAND:
    if (wParam == IDOK) {
      for (j = 0; j <= 1; j++) {
        for (i = 1; i <= oCore; i++) {
          r = GetEditR(hdlg, deo1 - 1 + i);
          if (j)
            rObjOrb[i] = r;
          else
            EnsureR(r, r >= -rDegMax && r <= rDegMax, "max orb");
        }
        for (i = 1; i <= oCore; i++) {
          r = GetEditR(hdlg, dea1 - 1 + i);
          if (j)
            rObjAdd[i] = r;
          else
            EnsureR(r, r >= -rDegMax && r <= rDegMax, "orb addition");
        }
        for (i = 1; i <= oCore; i++) {
          r = GetEditR(hdlg, dei1 - 1 + i);
          if (j)
            rObjInf[i] = r;
        }
      }
      wi.fRedraw = fTrue;
    }
    if (wParam == IDOK || wParam == IDCANCEL) {
      EndDialog(hdlg, fTrue);
      return fTrue;
    }
    break;
  }
  return fFalse;
}


/* Processing function for the object restrictions dialog, as invoked with */
/* both the Setting Restrictions and Transit Restrictions menu commands.   */

bool API DlgRestrict(hdlg, message, wParam, lParam)
HWND hdlg;
_int message;
WORD wParam;
LONG lParam;
{
  byte *lpb, *lpb2;
  int i;

  switch (message) {
  case WM_INITDIALOG:
    if (wi.wCmd == cmdRes)
      lpb = ignore;
    else {
      SetWindowText(hdlg, "Transit Object Restrictions");
      lpb = ignore2;
    }
    for (i = 1; i <= oNorm; i++)
      SetCheck(dx1 - 1 + i, lpb[i]);
    return fTrue;

  case WM_COMMAND:
    switch (wParam) {
    case dbRe_R0:
      for (i = 1; i <= oNorm; i++)
        SetCheck(dx1 - 1 + i, fTrue);
      break;
    case dbRe_R1:
      for (i = 1; i <= oNorm; i++)
        SetCheck(dx1 - 1 + i, fFalse);
      break;
    case dbRe_R:
      for (i = oMain+1; i <= oCore; i++)
        SetCheck(dx1 - 1 + i, !GetCheck(dx1 - 1 + i));
      break;
    case dbRe_RC:
      for (i = cuspLo; i <= cuspHi; i++)
        SetCheck(dx1 - 1 + i, !GetCheck(dx1 - 1 + i));
      break;
    case dbRe_Ru:
      for (i = uranLo; i <= uranHi; i++)
        SetCheck(dx1 - 1 + i, !GetCheck(dx1 - 1 + i));
      break;
    case dbRT:
      lpb2 = wi.wCmd == cmdRes ? ignore2 : ignore;
      for (i = 1; i <= oNorm; i++)
        SetCheck(dx1 - 1 + i, lpb2[i]);
      break;
    }

    if (wParam == IDOK) {
      lpb = wi.wCmd == cmdRes ? ignore : ignore2;
      for (i = 1; i <= oNorm; i++)
        lpb[i] = GetCheck(dx1 - 1 + i);
      if (!us.fCusp) {
        for (i = cuspLo; i <= cuspHi; i++)
          if (!lpb[i]) {
            us.fCusp = fTrue;
            WiCheckMenu(cmdResCusp, fTrue);
            break;
          }
      } else {
        for (i = cuspLo; i <= cuspHi; i++)
          if (!lpb[i])
            break;
        if (i > cuspHi) {
          us.fCusp = fFalse;
          WiCheckMenu(cmdResCusp, fFalse);
        }
      }
      if (!us.fUranian) {
        for (i = uranLo; i <= uranHi; i++)
          if (!lpb[i]) {
            us.fUranian = fTrue;
            WiCheckMenu(cmdResUranian, fTrue);
            break;
          }
      } else {
        for (i = uranLo; i <= uranHi; i++)
          if (!lpb[i])
            break;
        if (i > uranHi) {
          us.fUranian = fFalse;
          WiCheckMenu(cmdResUranian, fFalse);
        }
      }
      wi.fCast = fTrue;
    }
    if (wParam == IDOK || wParam == IDCANCEL) {
      EndDialog(hdlg, fTrue);
      return fTrue;
    }
    break;
  }
  return fFalse;
}


/* Processing function for the star restrictions dialog, as brought up with */
/* the Setting Star Restrictions menu command.                              */

bool API DlgStar(hdlg, message, wParam, lParam)
HWND hdlg;
_int message;
WORD wParam;
LONG lParam;
{
  int i;

  switch (message) {
  case WM_INITDIALOG:
    for (i = 1; i <= cStar; i++)
      SetCheck(dx1 - 1 + i, ignore[oNorm + i]);
    return fTrue;

  case WM_COMMAND:
    switch (wParam) {
    case dbSt_RU0:
      for (i = 1; i <= cStar; i++)
        SetCheck(dx1 - 1 + i, fTrue);
      break;
    case dbSt_RU1:
      for (i = 1; i <= cStar; i++)
        SetCheck(dx1 - 1 + i, fFalse);
      break;
    }

    if (wParam == IDOK) {
      for (i = 1; i <= cStar; i++)
        ignore[oNorm + i] = GetCheck(dx1 - 1 + i);
      if (!us.nStar) {
        for (i = starLo; i <= starHi; i++)
          if (!ignore[i]) {
            us.nStar = fTrue;
            WiCheckMenu(cmdResStar, fTrue);
            break;
          }
      } else {
        for (i = starLo; i <= starHi; i++)
          if (!ignore[i])
            break;
        if (i > starHi) {
          us.nStar = fFalse;
          WiCheckMenu(cmdResStar, fFalse);
        }
      }
      wi.fCast = fTrue;
    }
    if (wParam == IDOK || wParam == IDCANCEL) {
      EndDialog(hdlg, fTrue);
      return fTrue;
    }
    break;
  }
  return fFalse;
}


/* Processing function for the standard settings dialog, as brought up with */
/* the Setting More Settings menu command.                                  */

bool API DlgSetting(hdlg, message, wParam, lParam)
HWND hdlg;
_int message;
WORD wParam;
LONG lParam;
{
  char sz[cchSzDef];
  real r1;
  int n2, n3, n4, n5, n6;

  switch (message) {
  case WM_INITDIALOG:
    SetEditR(hdlg, deSe_s, us.rZodiacOffset, 6);
    SetEditN(deSe_A, us.nAsp);
    SetEditN(deSe_x, us.nHarmonic);
    SetEdit(deSe_h,
      szObjName[us.objCenter <= 1 ? 1-us.objCenter : us.objCenter]);
    SetRadio(us.objOnAsc == 0 ? dr1 : (us.objOnAsc > 0 ? dr2 : dr3),
      dr1, dr3);
    SetEdit(deSe_1, szObjName[us.objOnAsc == 0 ? oSun : abs(us.objOnAsc)]);
    SetEditN(deSe_I, us.nScreenWidth);
    SetCheck(dxSe_b, us.fPlacalc);
    SetRadio(us.nDegForm == 0 ? dr4 : (us.nDegForm == 1 ? dr5 : dr6),
      dr4, dr6);
    SetCheck(dxSe_sr, us.fEquator);
    return fTrue;

  case WM_COMMAND:
    if (wParam == IDOK) {
      r1 = GetEditR(hdlg, deSe_s);
      GetEdit(deSe_A, sz); n2 = NParseSz(sz, pmAspect);
      n3 = GetEditN(deSe_x);
      GetEdit(deSe_h, sz); n4 = NParseSz(sz, pmObject);
      GetEdit(deSe_1, sz); n5 = NParseSz(sz, pmObject);
      n6 = GetEditN(deSe_I);
      EnsureR(r1, FValidOffset(r1), "zodiac offset");
      EnsureN(n2, FValidAspect(n2), "aspect count");
      EnsureN(n3, FValidHarmonic(n3), "harmonic factor");
      EnsureN(n4, FValidCenter(n4), "central planet");
      EnsureN(n5, FItem(n5), "Solar chart planet");
      EnsureN(n6, FValidScreen(n6), "text columns");
      us.rZodiacOffset = r1;
      us.nAsp = n2;
      us.nHarmonic = n3;
      us.objCenter = n4 <= 1 ? 1-n4 : n4;
      us.objOnAsc = GetCheck(dr1) ? 0 : (GetCheck(dr2) ? n5 : -n5);
      us.nScreenWidth = n6;
      us.fPlacalc = GetCheck(dxSe_b);
      us.nDegForm = GetCheck(dr4) ? 0 : (GetCheck(dr5) ? 1 : 2);
      us.fEquator = GetCheck(dxSe_sr);
      WiCheckMenu(cmdHeliocentric, us.objCenter != 1);
      wi.fCast = fTrue;
    }
    if (wParam == IDOK || wParam == IDCANCEL) {
      EndDialog(hdlg, fTrue);
      return fTrue;
    }
    break;
  }
  return fFalse;
}


/* Processing function for the obscure settings dialog, as brought up with */
/* the Setting Obscure Settings menu command.                              */

bool API DlgObscure(hdlg, message, wParam, lParam)
HWND hdlg;
_int message;
WORD wParam;
LONG lParam;
{
  int n1;

  switch (message) {
  case WM_INITDIALOG:
    SetCheck(dxOb_Yn, us.fTrueNode);
    SetCheck(dxOb_Yd, us.fEuroDate);
    SetCheck(dxOb_Yt, us.fEuroTime);
    SetCheck(dxOb_YC, us.fSmartCusp);
    SetCheck(dxOb_Y8, us.fClip80);
    SetCheck(dxOb_Yo, us.fWriteOld);
    SetCheck(dxOb_YR0_s, ignore[0]);
    SetCheck(dxOb_YR0_d, ignore2[0]);
    SetEditN(deOb_YXg, gs.nGridCell);
    SetCheck(dxOb_YXf, gs.fFont);
    SetEditR(hdlg, deOb_YXp0_x, gs.xInch, 2);
    SetEditR(hdlg, deOb_YXp0_y, gs.yInch, 2);
    SetRadio(gs.nOrient == 0 ? dr11 : (gs.nOrient > 0 ? dr9 : dr10),
      dr9, dr11);
    SetRadio(dr1 + (gs.nGlyphs/1000 == 2), dr1, dr2);
    SetRadio(dr3 + ((gs.nGlyphs/100)%10 == 2), dr3, dr4);
    SetRadio(dr5 + ((gs.nGlyphs/10)%10 == 2), dr5, dr6);
    SetRadio(dr7 + (gs.nGlyphs%10 == 2), dr7, dr8);
    return fTrue;

  case WM_COMMAND:
    if (wParam == IDOK) {
      n1 = GetEditN(deOb_YXg);
      EnsureN(n1, FValidGrid(n1), "grid cell");
      us.fTrueNode = GetCheck(dxOb_Yn);
      us.fEuroDate = GetCheck(dxOb_Yd);
      us.fEuroTime = GetCheck(dxOb_Yt);
      us.fSmartCusp = GetCheck(dxOb_YC);
      us.fClip80 = GetCheck(dxOb_Y8);
      us.fWriteOld = GetCheck(dxOb_Yo);
      ignore[0] = GetCheck(dxOb_YR0_s);
      ignore2[0] = GetCheck(dxOb_YR0_d);
      gs.nGridCell = n1;
      gs.fFont = GetCheck(dxOb_YXf);
      gs.xInch = GetEditR(hdlg, deOb_YXp0_x);
      gs.yInch = GetEditR(hdlg, deOb_YXp0_y);
      gs.nOrient = GetCheck(dr11) ? 0 : (GetCheck(dr9) ? 1 : -1);
      gs.nGlyphs = (GetCheck(dr1) ? 1 : 2) * 1000 + (GetCheck(dr3) ? 1 : 2) *
        100 + (GetCheck(dr5) ? 1 : 2) * 10 + (GetCheck(dr7) ? 1 : 2);
      wi.fCast = fTrue;
    }
    if (wParam == IDOK || wParam == IDCANCEL) {
      EndDialog(hdlg, fTrue);
      return fTrue;
    }
    break;
  }
  return fFalse;
}


/* Processing function for the transit chart dialog, as brought up with the */
/* Chart Transits menu command.                                             */

bool API DlgTransit(hdlg, message, wParam, lParam)
HWND hdlg;
_int message;
WORD wParam;
LONG lParam;
{
  char sz[cchSzDef];
  int mon, day, yea, n1, n2;
  real tim;

  switch (message) {
  case WM_INITDIALOG:
    if (us.fInDay)           n1 = 1;
    else if (us.fInDayInf)   n1 = 2;
    else if (us.fTransit)    n1 = 3;
    else if (us.fTransitInf) n1 = 4;
    else                     n1 = 0;
    SetRadio(dr1 + n1, dr1, dr5);
    SetCheck(dxTr_p, is.fProgress);
    SetEditMDYT(hdlg, dcTrMon, dcTrDay, dcTrYea, dcTrTim,
      Mon2, Day2, Yea2, Tim2);
    if (n1 == 1) {
      n2 = is.fProgress || us.fInDayMonth;
      if (n2 == 1 && Mon2 == 0)
        n2 += 1 + (us.nEphemYears > 1);
    } else if (n1 == 3)
      n2 = 1 + (Mon2 <= 0) + (Mon2 < 0);
    else
      n2 = 0;
    SetRadio(dr6 + n2, dr6, dr9);
    SetEditN(deTr_tY, us.nEphemYears);
    SetEditN(deTr_d, us.nDivision);
    SetFocus(GetDlgItem(hdlg, dcTrMon));
    return fFalse;

  case WM_COMMAND:
    if (wParam == dbTr_tn) {
      GetTimeNow(&mon, &day, &yea, &tim, us.zonDef-us.dstDef);
      SetEditMDYT(hdlg, dcTrMon, dcTrDay, dcTrYea, dcTrTim,
        mon, day, yea, tim);
    }

    if (wParam == IDOK) {
      GetEdit(dcTrMon, sz); mon = NParseSz(sz, pmMon);
      GetEdit(dcTrDay, sz); day = NParseSz(sz, pmDay);
      GetEdit(dcTrYea, sz); yea = NParseSz(sz, pmYea);
      GetEdit(dcTrTim, sz); tim = RParseSz(sz, pmTim);
      n1 = GetEditN(deTr_tY);
      n2 = GetEditN(deTr_d);
      EnsureN(mon, FValidMon(mon), "month");
      EnsureN(yea, FValidYea(yea), "year");
      EnsureN(day, FValidDay(day, mon, yea), "day");
      EnsureR(tim, FValidTim(tim), "time");
      EnsureN(n2, FValidDivision(n2), "searching divisions");
      Mon2 = mon; Day2 = day; Yea2 = yea; Tim2 = tim;
      us.nEphemYears = n1;
      us.nDivision = n2;
      is.fProgress = GetCheck(dxTr_p);
      n1 = GetCheck(dr1) ? 0 : (GetCheck(dr2) ? 1 : (GetCheck(dr3) ? 2 :
        (GetCheck(dr4) ? 3 : 4)));
      switch (n1) {
      case 1: wi.nMode = gTraTraHit; break;
      case 2: wi.nMode = gTraTraInf; break;
      case 3: wi.nMode = gTraNatHit; break;
      case 4: wi.nMode = gTraNatInf; break;
      default: wi.nMode = gWheel;
      }
      n2 = GetCheck(dr6) ? 0 : (GetCheck(dr7) ? 1 : (GetCheck(dr8) ? 2 : 3));
      if (n1 == 1) {
        us.fInDayMonth = (is.fProgress || n2 >= 1);
        if (n2 >= 2) {
          Mon2 = 0;
          if (n2 == 2)
            us.nEphemYears = 1;
        }
      } else if (n1 == 3) {
        if (n2 == 2)
          Mon2 = 0;
        else if (n2 == 3) {
          Mon2 = -1; Day2 = us.nEphemYears;
        }
      } else if (n1 == 2) {
        us.fProgress = is.fProgress;
        wi.fCast = fTrue;
      }
      us.fGraphics = fFalse;
      wi.fRedraw = fTrue;
    }
    if (wParam == IDOK || wParam == IDCANCEL) {
      EndDialog(hdlg, fTrue);
      return fTrue;
    }
    break;
  }
  return fFalse;
}


/* Processing function for the progression settings dialog, as brought up */
/* with the Chart Progressions menu command.                              */

bool API DlgProgress(hdlg, message, wParam, lParam)
HWND hdlg;
_int message;
WORD wParam;
LONG lParam;
{
  char sz[cchSzDef];
  int mon, day, yea;
  real tim, r1;

  switch (message) {
  case WM_INITDIALOG:
    SetCheck(dxPr_p, us.fProgress);
    SetEditMDYT(hdlg, dcPrMon, dcPrDay, dcPrYea, dcPrTim,
      Mon2, Day2, Yea2, Tim2);
    SetRadio(dr1 + us.fSolarArc, dr1, dr2);
    SetEditR(hdlg, dcPr_pd, us.rProgDay, 5);
    SetCombo(dcPr_pd, "365.25");
    SetCombo(dcPr_pd, "27.321661");
    SetCombo(dcPr_pd, "29.530588");
    SetFocus(GetDlgItem(hdlg, dcPrMon));
    return fFalse;

  case WM_COMMAND:
    if (wParam == dbPr_pn) {
      GetTimeNow(&mon, &day, &yea, &tim, us.zonDef-us.dstDef);
      SetEditMDYT(hdlg, dcPrMon, dcPrDay, dcPrYea, dcPrTim,
        mon, day, yea, tim);
    }

    if (wParam == IDOK) {
      GetEdit(dcPrMon, sz); mon = NParseSz(sz, pmMon);
      GetEdit(dcPrDay, sz); day = NParseSz(sz, pmDay);
      GetEdit(dcPrYea, sz); yea = NParseSz(sz, pmYea);
      GetEdit(dcPrTim, sz); tim = RParseSz(sz, pmTim);
      r1 = GetEditR(hdlg, dcPr_pd);
      EnsureN(mon, FValidMon(mon), "month");
      EnsureN(yea, FValidYea(yea), "year");
      EnsureN(day, FValidDay(day, mon, yea), "day");
      EnsureR(tim, FValidTim(tim), "time");
      EnsureR(r1, r1 != 0.0, "degree per day");
      Mon2 = mon; Day2 = day; Yea2 = yea; Tim2 = tim;
      us.rProgDay = r1;
      us.fProgress = GetCheck(dxPr_p);
      us.fSolarArc = GetCheck(dr2);
      is.JDp = MdytszToJulian(Mon2, Day2, Yea2, Tim2, us.dstDef, us.zonDef);
      wi.fCast = fTrue;
    }
    if (wParam == IDOK || wParam == IDCANCEL) {
      EndDialog(hdlg, fTrue);
      return fTrue;
    }
    break;
  }
  return fFalse;
}


/* Processing function for the chart subsettings dialog, as brought up with */
/* the Chart Chart Settings menu command.                                   */

bool API DlgChart(hdlg, message, wParam, lParam)
HWND hdlg;
_int message;
WORD wParam;
LONG lParam;
{
  int n1, n2, n3;
  bool f;

  switch (message) {
  case WM_INITDIALOG:
    SetCheck(dxCh_v0, us.fVelocity);
    SetEditN(deCh_w, us.nWheelRows);
    SetCheck(dxCh_w0, us.fWheelReverse);
    SetCheck(dxCh_g0, us.fGridConfig);
    SetCheck(dxCh_a0, us.fAspSummary);
    SetCheck(dxCh_m0, us.fMidSummary);
    SetCheck(dxCh_Z0, us.fPrimeVert);
    SetCheck(dxCh_j0, us.fInfluenceSign);
    SetEditN(deCh_L, us.nAstroGraphStep);
    SetCheck(dxCh_L0, us.fLatitudeCross);
    SetCheck(dxCh_Ky, us.fCalendarYear);
    SetEditN(deCh_P, us.nArabicParts);
    SetCheck(dxCh_P0, us.fArabicFlip);
    switch (us.nStar) {
    case 'z': n1 = dr2; break;
    case 'l': n1 = dr3; break;
    case 'n': n1 = dr4; break;
    case 'b': n1 = dr5; break;
    default:  n1 = dr1;
    }
    SetRadio(n1, dr1, dr5);
    switch (us.nArabic) {
    case 'z': n2 = dr7; break;
    case 'n': n2 = dr8; break;
    case 'f': n2 = dr9; break;
    default:  n2 = dr6;
    }
    SetRadio(n2, dr6, dr9);
    return fTrue;

  case WM_COMMAND:
    if (wParam == IDOK) {
      n1 = GetEditN(deCh_w);
      n2 = GetEditN(deCh_L);
      n3 = GetEditN(deCh_P);
      EnsureN(n1, FValidWheel(n1), "wheel row");
      EnsureN(n2, FValidAstrograph(n2), "astro-graph step");
      EnsureN(n3, FValidPart(n3), "Arabic part");
      f = GetCheck(dxCh_v0);
      if (us.fVelocity != f) {
        us.fVelocity = f;
        WiCheckMenu(cmdGraphicsSidebar, !f);
      }
      us.nWheelRows = n1;
      us.fWheelReverse = GetCheck(dxCh_w0);
      us.fGridConfig = GetCheck(dxCh_g0);
      us.fAspSummary = GetCheck(dxCh_a0);
      us.fMidSummary = GetCheck(dxCh_m0);
      us.fPrimeVert = GetCheck(dxCh_Z0);
      us.fInfluenceSign = GetCheck(dxCh_j0);
      us.nAstroGraphStep = n2;
      us.fLatitudeCross = GetCheck(dxCh_L0);
      us.fCalendarYear = GetCheck(dxCh_Ky);
      us.nArabicParts = n3;
      us.fArabicFlip = GetCheck(dxCh_P0);
      if (us.nStar)
        us.nStar = GetCheck(dr2) ? 'z' : (GetCheck(dr3) ? 'l' :
          (GetCheck(dr4) ? 'n' : (GetCheck(dr5) ? 'b' : fTrue)));
      if (us.nArabic)
        us.nArabic = GetCheck(dr7) ? 'z' : (GetCheck(dr8) ? 'n' :
          (GetCheck(dr9) ? 'f' : fTrue));
      wi.fCast = fTrue;
    }
    if (wParam == IDOK || wParam == IDCANCEL) {
      EndDialog(hdlg, fTrue);
      return fTrue;
    }
    break;
  }
  return fFalse;
}


/* Processing function for the graphic settings dialog, as brought up with */
/* the Graphics Graphics Settings menu command.                            */

bool API DlgGraphics(hdlg, message, wParam, lParam)
HWND hdlg;
_int message;
WORD wParam;
LONG lParam;
{
  char sz[cchSzDef];
  int n1, n2, n3, n5, n6;
  real r4;

  switch (message) {
  case WM_INITDIALOG:
    SetEditN(deGr_Xw_x, gs.xWin);
    SetEditN(deGr_Xw_y, gs.yWin);
    SetEditN(deGr_XW, gs.nRot);
    SetEditR(hdlg, deGr_XG, gs.rTilt, 2);
    SetCheck(dxGr_XW0, gs.fMollewide);
    SetEditN(deGr_WN, wi.nTimerDelay);
    SetRadio(gs.objLeft > 0 ? dr2 : (gs.objLeft < 0 ? dr3 : dr1), dr1, dr3);
    SetEdit(deGr_X1, szObjName[gs.objLeft == 0 ? oSun : abs(gs.objLeft)]);
    return fTrue;

  case WM_COMMAND:
    if (wParam == IDOK) {
      n1 = GetEditN(deGr_Xw_x);
      n2 = GetEditN(deGr_Xw_y);
      n3 = GetEditN(deGr_XW);
      r4 = GetEditR(hdlg, deGr_XG);
      n5 = GetEditN(deGr_WN);
      GetEdit(deGr_X1, sz); n6 = NParseSz(sz, pmObject);
      EnsureN(n1, FValidGraphx(n1), "horizontal size");
      EnsureN(n2, FValidGraphy(n2), "vertical size");
      EnsureN(n3, FValidRotation(n3), "horizontal rotation");
      EnsureR(r4, FValidTilt(r4), "vertical tilt");
      EnsureN(n5, FValidTimer(n5), "animation delay");
      EnsureN(n5, FItem(n6), "rotation planet");
      if (gs.xWin != n1 || gs.yWin != n2) {
        gs.xWin = n1; gs.yWin = n2;
        if (wi.fWindowChart)
          ResizeWindowToChart();
      }
      gs.nRot = n3; gs.rTilt = r4;
      if (wi.nTimerDelay != (UINT)n5) {
        wi.nTimerDelay = n5;
        if (wi.nTimer != 0)
          KillTimer(wi.hwnd, 1);
        wi.nTimer = SetTimer(wi.hwnd, 1, wi.nTimerDelay, NULL);
      }
      gs.objLeft = GetCheck(dr1) ? 0 : (GetCheck(dr2) ? n6 : -n6);
      gs.fMollewide = GetCheck(dxGr_XW0);
      us.fGraphics = wi.fRedraw = fTrue;
    }
    if (wParam == IDOK || wParam == IDCANCEL) {
      EndDialog(hdlg, fTrue);
      return fTrue;
    }
    break;
  }
  return fFalse;
}


/* Processing function for the about dialog, showing copyrights and  */
/* credits, as brought up with the Help About Astrolog menu command. */

bool API DlgAbout(hdlg, message, wParam, lParam)
HWND hdlg;
_int message;
WORD wParam;
LONG lParam;
{
  switch (message) {
  case WM_INITDIALOG:
    return fTrue;

  case WM_COMMAND:
    if (wParam == IDOK || wParam == IDCANCEL) {
      EndDialog(hdlg, fTrue);
      return fTrue;
    }
    break;
  }
  return fFalse;
}
#endif /* WIN */

/* wdialog.c */
