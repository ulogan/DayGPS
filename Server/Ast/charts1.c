/*
** Astrolog (Version 5.05) File: charts1.c
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

#include "astrolog.h"
#include "string.h"

real lret[objMax];

/*
******************************************************************************
** Single Chart Display Routines.
******************************************************************************
*/

/* Print the straight listing of planet and house positions and specified */
/* by the -v switch, along with the element table, etc.                   */
#define LOGAN
#ifdef LOGAN

  char *tithis[] = { "", "S.Pratipat", "S.Dvitiya", "S.Tritiya",
                     "S.Chaturthi", "S.Panchami", "S.Shasthi",
                     "S.Saptami", "S.Ashtami", "S.Navami",
                     "S.Dashami", "S.Ekadashi", "S.Dvadashi",
                     "S.Trayadashi", "S.Chaturdashi",
                    "Purnima", "K.Pratipat", "K.Dvitiya", "K.Tritiya",
                     "K.Chaturthi", "K.Panchami", "K.Shasthi",
                     "K.Saptami", "K.Ashtami", "K.Navami",
                     "K.Dashami", "K.Ekadashi", "K.Dvadashi",
                     "K.Trayadashi", "K.Chaturdashi", "Amavasa"};

  char *naksatras[] = { "Aswini", "Bharani", "Krithikai",
                        "Rohini", "Mrigashir", "Thiruvadu",
                        "Punrpusam", "Pusam", "Ayilyam",
                        "Magham", "Puram", "Uttaram",
                        "Hastam", "Chitrai", "Swati",
                        "Vishakham", "Anusham", "Kettai",
                        "Mulam", "Puradam", "Uttaradam",
                        "Tiruvonam", "Avitam", "Sathayam",
                        "Puratathi", "Utartathi", "Revati" }; 

  int nakdeg[]      = {  0,  800,  1600,  2400,
                         3200,  4000,  4800,
                         5600,  6400,  7200,
                         8000,  8800,  9600,
                        10400, 11200, 12000,
                        12800, 13600, 14400,
                        15200, 16000, 16800,
                        17600, 18400, 19200,
                        20000, 20800, 21600 };

  char *yogas[] = { "Vishkamba", "Preeti", "Aayushmaan",
                    "Saubhaagys", "Sobhana", "Atiganda",
                    "Sukarman", "Dhriti", "Shoola",
                    "Ganda", "Vriddhi", "Dhruva",
                    "Vyaaghaata", "Harshana", "Vajra",
                    "Siddhi", "Vyatipata", "Variyan",
                    "Parigha", "Shiva", "Siddha",
                    "Saadhya", "Shubha", "Sukla",
                    "Brahma", "Indra", "Vaidhriti" };

  char *lord[] = { "Mars", "Venus", "Mercury",
                   "Moon", "Sun", "Mercury",
                   "Venus", "Mars", "Jupiter",
                   "Saturn", "Saturn", "Jupiter" };

  char *plan[] = { "Sun", "Moon", "Mercury",
                   "Venus", "Mars", "Jupiter",
                   "Saturn", "Rahu", "Ketu",
                   "Lag1", "Lag2", "Lag3" };
  enum {
      Sun,
      Moon,
      Mercury,
      Venus,
      Mars,
      Jupiter,
      Saturn,
      Rahu,
      Ketu,
      Lag1,
      Lag2,
      Lag3,
      Pmax
  };

  enum {
      Rasi,
      Amsa,
      Smax
  };

  int pindex[] = { 5, 4, 3,
                   2, 1, 3,
                   4, 5, 6,
                   7, 7, 6 };

  struct {
    char name[4];
    int count;
    int index[8];
    char planet[8][12];
    char mdeg[8][12];
    char deg[8][12];
    int navacount;
    int navaindex[8];
    char navamsam[8][12];
  } chr[12];

  struct {
      int index;
  } ind[Smax][Pmax];

  extern int csv;
  extern int autodst;
  extern int regular;
  extern int secstrue;
  extern int navamsam;
  extern int predictor;

  enum {
      NEUTRAL,
      EXALTED,
      DEBILITATED,
      OWN,
      FRIEND,
      ENEMY
  };

  // For external use
  int eDay;
  int eMon;
  int eYea;
  int eruler;
  int edays;
  int emonths;
  int eyears;

int getSun(int rasi) 
{
        switch(rasi) {
        case 0:
                printf("Exalted ");
                return(EXALTED);
                break;
        case 6:
                printf("Debilitated ");
                return(DEBILITATED);
                break;
        case 4:
                printf("Own ");
                return(OWN);
                break;
        case 5:
                printf("Friend ");
                return(FRIEND);
                break;
        case 7:
                printf("Enemy ");
                return(ENEMY);
                break;
        default:
                break;
        }
        return(NEUTRAL);
}

int getMoon(int rasi) 
{
        switch(rasi) {
        case 1:
                printf("Exalted ");
                return(EXALTED);
                break;
        case 7:
                printf("Debilitated ");
                return(DEBILITATED);
                break;
        case 3:
                printf("Own ");
                return(OWN);
                break;
        case 5:
                printf("Friend ");
                return(FRIEND);
                break;
        case 6:
                printf("Enemy ");
                return(ENEMY);
                break;
        default:
                break;
        }
        return(NEUTRAL);
}

int getMercury(int rasi) 
{
        switch(rasi) {
        case 5:
                printf("Exalted ");
                return(EXALTED);
                break;
        case 11:
                printf("Debilitated ");
                return(DEBILITATED);
                break;
        case 2:
                printf("Own ");
                return(OWN);
                break;
        case 3:
                printf("Friend ");
                return(FRIEND);
                break;
        case 7:
                printf("Enemy ");
                return(ENEMY);
                break;
        default:
                break;
        }
        return(NEUTRAL);
}

int getVenus(int rasi) 
{
        switch(rasi) {
        case 11:
                printf("Exalted ");
                return(EXALTED);
                break;
        case 5:
                printf("Debilitated ");
                return(DEBILITATED);
                break;
        case 2:
        case 6:
                printf("Own ");
                return(OWN);
                break;
        case 8:
                printf("Friend ");
                return(FRIEND);
                break;
        case 7:
                printf("Enemy ");
                return(ENEMY);
                break;
        default:
                break;
        }
        return(NEUTRAL);
}

int getMars(int rasi) 
{
        switch(rasi) {
        case 9:
                printf("Exalted ");
                return(EXALTED);
                break;
        case 3:
                printf("Debilitated ");
                return(DEBILITATED);
                break;
        case 0:
        case 7:
                printf("Own ");
                return(OWN);
                break;
        case 2:
                printf("Friend ");
                return(FRIEND);
                break;
        case 5:
                printf("Enemy ");
                return(ENEMY);
                break;
        default:
                break;
        }
        return(NEUTRAL);
}

int getJupiter(int rasi) 
{
        switch(rasi) {
        case 3:
                printf("Exalted ");
                return(EXALTED);
                break;
        case 9:
                printf("Debilitated ");
                return(DEBILITATED);
                break;
        case 8:
        case 11:
                printf("Own ");
                return(OWN);
                break;
        case 5:
                printf("Friend ");
                return(FRIEND);
                break;
        case 7:
                printf("Enemy ");
                return(ENEMY);
                break;
        default:
                break;
        }
        return(NEUTRAL);
}

int getSaturn(int rasi) 
{
        switch(rasi) {
        case 6:
                printf("Exalted ");
                return(EXALTED);
                break;
        case 0:
                printf("Debilitated ");
                return(DEBILITATED);
                break;
        case 9:
        case 10:
                printf("Own ");
                return(OWN);
                break;
        case 1:
                printf("Friend ");
                return(FRIEND);
                break;
        case 4:
                printf("Enemy ");
                return(ENEMY);
                break;
        default:
                break;
        }
        return(NEUTRAL);
}

int getRahu(int rasi) 
{
        switch(rasi) {
        case 0:
                printf("Exalted ");
                return(EXALTED);
                break;
        case 6:
                printf("Debilitated ");
                return(DEBILITATED);
                break;
        case 4:
                printf("Own ");
                return(OWN);
                break;
        case 5:
                printf("Friend ");
                return(FRIEND);
                break;
        case 7:
                printf("Enemy ");
                return(ENEMY);
                break;
        default:
                break;
        }
        return(NEUTRAL);
}

int getKetu(int rasi) 
{
        switch(rasi) {
        case 0:
                printf("Exalted ");
                return(EXALTED);
                break;
        case 6:
                printf("Debilitated ");
                return(DEBILITATED);
                break;
        case 4:
                printf("Own ");
                return(OWN);
                break;
        case 5:
                printf("Friend ");
                return(FRIEND);
                break;
        case 7:
                printf("Enemy ");
                return(ENEMY);
                break;
        default:
                break;
        }
        return(NEUTRAL);
}

getPlanet(int planet, int position)
{
        switch (planet) {
        case 1: /* Sun */
                getSun(position);
                break;
        case 2: /* Moon */
                getMoon(position);
                break;
        case 3: /* Mercury */
                getMercury(position);
                break;
        case 4: /* Venus */
                getVenus(position);
                break;
        case 5: /* Mars */
                getMars(position);
                break;
        case 6: /* Jupiter */
                getJupiter(position);
                break;
        case 7: /* Saturn */
                getSaturn(position);
                break;
        case 8: /* Rahu */
                getRahu(position);
                break;
        case 9: /* Ketu */
                getKetu(position);
                break;
        default:
                break;
        }
}

predProfession(int asc)
{
            /* For profession */
        int prof = (asc + 9);
        int lo;
        int lh;
        int li;
        int lj;

        if (prof > 12) {
                prof -= 12;
        }
        prof--; /* Index starts at 0 */

        printf("10th Lord %s is in the ", lord[prof]);

        for (li = 0; li < 12; li++) {
                for (lj = 0; lj < chr[li].count; lj++) {
                        if (pindex[prof] == chr[li].index[lj]) {

                                lh = li + 1;
                                lo = lh - asc;
                                if (lo < 0) {
                                        lo += 12;
                                }
                                printf("%d House ", lo + 1);
                                break;
                        }
                }
        }

        getPlanet(pindex[prof], lh - 1);

        for (li = 0; li < 12; li++) {
                for (lj = 0; lj < chr[li].navacount; lj++) {
                        if (pindex[prof] == chr[li].navaindex[lj]) {

                                lh = li;
                                /*
                                lo = lh - asc;
                                if (lo < 0) {
                                        lo += 12;
                                }
                                printf("%d House ", lo + 1);
                                */
                                break;
                        }
                }
        }

        printf("Navamsa ");
        getPlanet(pindex[prof], lh - 1);

        printf("\n");

        printf("Planets in the 10th House: ");
        for (lj = 0; lj < chr[prof].count; lj++) {

                printf("%s ", plan[chr[prof].index[lj]]);

                getPlanet(chr[prof].index[lj], prof);

        }
        printf("\n");
}

#define MIN (60)
#define HOUR (MIN * 60)

char *mon[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

real getAsc(long clock, int flag)
{
  extern int predasc;
  extern float baseTT;
  extern float baseZZ;
  real Off = 0.0, vtx;
  struct tm ctm0;
  struct tm *ctm;
  char month[28];
  char day[28];
  char year[28];
  char ltm[28];
  char *dst;
  char *lon;
  char *lat;
  int tmp;

  if (predasc) {
        TT = baseTT + flag;
        ZZ = baseZZ - flag;
  } else {
        clock += (HOUR * flag);

        ctm = localtime(&clock);

        strcpy(month, mon[ctm->tm_mon]);
        sprintf(day, "%d", ctm->tm_mday);
        sprintf(year, "%d", (ctm->tm_year + 1900));
        sprintf(ltm, "%02d:%02d:%02d", ctm->tm_hour, ctm->tm_min, ctm->tm_sec);
        MM = NParseSz(month, pmMon);
        DD = NParseSz(day, pmDay);
        YY = NParseSz(year, pmYea);
        TT = RParseSz(ltm, pmTim);
        // printf("MM %d DD %d YY %d Time %f\n", MM, DD, YY, TT);
        if ((flag == 2) || (flag == -1)) {
            dst = "CST";
        } else if (flag == 3) {
            dst = "EST";
        } else if (flag == -3) {
            dst = "PST";
        }
        ZZ = RParseSz(dst, pmZon);
  }

  if ((flag == 2) || (flag == -1)) {
    /* Chicago */
    lon = "87:39W";
    lat = "41:51N";
  } else if (flag == 3) {
    /* New York */
    lon = "74:00W";
    lat = "40:42N";
  } else if (flag == -3) {
    /* Sunnyvale */
    lon = "122:00W";
    lat = "37:23N";
  }

  OO = RParseSz(lon, pmLon);
  AA = RParseSz(lat, pmLat);

  // Process Input
  ProcessInput(1);

  // Compute Variables
  ComputeVariables(&vtx);

  // Return the Ascendant
  return(CuspAscendant());
}

#endif

void ChartListing()
{
  ET et;
  char sz[cchSzDef];
  int i, j, k, fNam, fLoc;
  real rT;

#ifdef LOGAN
  int li;
  int lj;
  char zod[4];
  int deg;
  int min;
  int sec;
  int sunrasi;
  int sundeg;
  int sunmin;
  int moonrasi;
  int moondeg;
  int moonmin;
  int mercrasi;
  int mercdeg;
  int mercmin;
  int venurasi;
  int venudeg;
  int venumin;
  int marsrasi;
  int marsdeg;
  int marsmin;
  int jupirasi;
  int jupideg;
  int jupimin;
  int saturasi;
  int satudeg;
  int satumin;
  int rahurasi;
  int rahudeg;
  int rahumin;
  int keturasi;
  int ketudeg;
  int ketumin;
  int asc1rasi;
  int asc1deg;
  int asc1min;
  int asc;
  int asc2rasi;
  int asc2deg;
  int asc2min;
  int asc3rasi;
  int asc3deg;
  int asc3min;
  int dum;
  int dum1;
  int dum2;
  char ddeg[12];
  char lag1[6];
  char lag2[6];
  char lag3[6];
  extern int matchdata;

  if (matchdata) {
      printf("%f\n", planet[oMoo]);
      return;
  }
#endif

  CreateElemTable(&et);
  fNam = *ciMain.nam > chNull; fLoc = *ciMain.loc > chNull;

  /* Print header showing time and date of the chart being displayed. */

  AnsiColor(kWhite);
#ifndef LOGAN
  sprintf(sz, "%s %s chart ", szAppName, szVersionCore); PrintSz(sz);
  if (Mon == -1)
    PrintSz("(No time or space)\n");
  else if (us.nRel == rcComposite)
    PrintSz("(Composite)\n");
  else {
    sprintf(sz, "for %s%s", ciMain.nam, fNam ? "\n" : ""); PrintSz(sz);
    j = DayOfWeek(Mon, Day, Yea);
    sprintf(sz, "%c%c%c %s %s (%cT %s GMT)", chDay3(j),
      SzDate(Mon, Day, Yea, 3), SzTim(Tim), ChDst(Dst),
      SzZone(Zon)); PrintSz(sz);
    sprintf(sz, "%c%s%s%s\n", fLoc && !fNam ? '\n' : ' ', ciMain.loc,
      fLoc ? " " : "", SzLocation(Lon, Lat)); PrintSz(sz);
  }
#else /* LOGAN */
    if (navamsam) {
        sprintf(sz, "%s - ", ciMain.nam); PrintSz(sz);
        j = DayOfWeek(Mon, Day, Yea);
        sprintf(sz, "%c%c%c %s %s (%cT %s GMT)", chDay3(j),
        SzDate(Mon, Day, Yea, 3), SzTim(Tim), ChDst(Dst),
        SzZone(Zon)); PrintSz(sz);
        sprintf(sz, "%c%s%s%s\n", fLoc && !fNam ? '\n' : ' ', ciMain.loc,
         fLoc ? " " : "", SzLocation(Lon, Lat)); PrintSz(sz);
    }
#endif /* LOGAN */

#ifdef INTERPRET
  if (us.fInterpret) {          /* Print an interpretation if -I in effect. */
    if (us.nRel == rcSynastry)
      InterpretSynastry();      /* Print synastry interpretaion for -r -I.  */
    else
      InterpretLocation();      /* Do normal interpretation for just -v -I. */
    return;
  }
#endif

  AnsiColor(kDkGray);
#ifndef LOGAN
  if (us.fSeconds)
    PrintSz("Body  Location   Ret. Declin.  Rul.      House  Rul. Velocity\n");
  else {
    PrintSz("Body  Locat. Ret. Decl. Rul.      House  Rul. Veloc.    ");
    sprintf(sz, "%s Houses.\n", szSystem[us.nHouseSystem]); PrintSz(sz);
  }
  if (!fNam && !fLoc)
    PrintL();
#else 
  if (navamsam) {
    printf("\n");
  }
#endif

  /* Ok, now print out the location of each object. */

  for (i = 1, j = 1; i <= oNorm; i++, j++) {
    if (us.fSeconds) {
      if (ignore[i])
        continue;
    } else {
      if (i > oCore && (i <= cuspHi || ignore[i]))
        continue;
      while (i <= oCore && j <= oCore && ignore[j])
        j++;
    }
    if (i <= oCore && j > oCore)
      PrintTab(' ', 51);
    else {
      if (i > oCore)
        j = i;
#ifdef LOGAN
      {
        static int flag = 0;
        if (flag == 0)
        {
          flag = 1;
          strcpy(chr[0].name, "Ari");
          strcpy(chr[1].name, "Tau");
          strcpy(chr[2].name, "Gem");
          strcpy(chr[3].name, "Can");
          strcpy(chr[4].name, "Leo");
          strcpy(chr[5].name, "Vir");
          strcpy(chr[6].name, "Lib");
          strcpy(chr[7].name, "Sco");
          strcpy(chr[8].name, "Sag");
          strcpy(chr[9].name, "Cap");
          strcpy(chr[10].name, "Aqu");
          strcpy(chr[11].name, "Pis");
          for (li = 0; li < 12; li++)
          {
            chr[li].count = 0;
            chr[li].navacount = 0;
            for (lj = 0; lj < 8; lj++)
            {
              chr[li].planet[lj][0] = '\0';
              chr[li].navamsam[lj][0] = '\0';
              chr[li].deg[lj][0] = '\0';
              chr[li].mdeg[lj][0] = '\0';
            }
          }
        }
        if (strncmp(szObjName[j], "Ketu", 4) == 0)
          continue;

        sscanf(SzZodiac(planet[j]), "%d%3s%d", &deg, zod, &min);
        strcpy(ddeg, SzDegree(planet[j]));
        if (strlen(ddeg) > 7)
          sscanf(&ddeg[7], "%02d", &sec);
        else
          sec = 0;
        for (li = 0; li < 12; li++)
        {
          if (strcmp(chr[li].name, zod) == 0)
          {
            sprintf(chr[li].planet[chr[li].count], "%-4.4s", szObjName[j]);
            if (strcmp(chr[li].planet[chr[li].count], "Sun ") == 0)
            {
              sunrasi = li + 1;
              sundeg = deg;
              sunmin = min;
              chr[li].index[chr[li].count] = 1;
              ind[Rasi][Sun].index = li;
            }
            else
            if (strcmp(chr[li].planet[chr[li].count], "Moon") == 0)
            {
              moonrasi = li + 1;
              moondeg = deg;
              moonmin = min;
              chr[li].index[chr[li].count] = 2;
              ind[Rasi][Moon].index = li;
            }
            else
            if (strcmp(chr[li].planet[chr[li].count], "Merc") == 0)
            {
              mercrasi = li + 1;
              mercdeg = deg;
              mercmin = min;
              chr[li].index[chr[li].count] = 3;
              ind[Rasi][Mercury].index = li;
            }
            else
            if (strcmp(chr[li].planet[chr[li].count], "Venu") == 0)
            {
              venurasi = li + 1;
              venudeg = deg;
              venumin = min;
              chr[li].index[chr[li].count] = 4;
              ind[Rasi][Venus].index = li;
            }
            else
            if (strcmp(chr[li].planet[chr[li].count], "Mars") == 0)
            {
              marsrasi = li + 1;
              marsdeg = deg;
              marsmin = min;
              chr[li].index[chr[li].count] = 5;
              ind[Rasi][Mars].index = li;
            }
            else
            if (strcmp(chr[li].planet[chr[li].count], "Jupi") == 0)
            {
              jupirasi = li + 1;
              jupideg = deg;
              jupimin = min;
              chr[li].index[chr[li].count] = 6;
              ind[Rasi][Jupiter].index = li;
            }
            if (strcmp(chr[li].planet[chr[li].count], "Satu") == 0)
            {
              saturasi = li + 1;
              satudeg = deg;
              satumin = min;
              chr[li].index[chr[li].count] = 7;
              ind[Rasi][Saturn].index = li;
            }
            else
            if (strcmp(chr[li].planet[chr[li].count], "Lagn") == 0)
            {
              asc1rasi = li + 1;
              asc1deg = deg;
              asc1min = min;
              asc = li + 1;
              if (!regular) {
                sprintf(lag1, "Lag%c", ciMain.loc[0]);
              } else {
                sprintf(lag1, "Lag");
              }
              strcpy(chr[li].planet[chr[li].count], lag1);
              ind[Rasi][Lag1].index = li;
            }
            if (secstrue)
              sprintf(chr[li].deg[chr[li].count], "[%02d:%02d:%02d]", deg, min, sec);
            else
              sprintf(chr[li].deg[chr[li].count], "%02d:%02d", deg, min);
            sprintf(chr[li].mdeg[chr[li].count], "%02d:%02d", deg, min);
            if (strcmp(chr[li].planet[chr[li].count], "Rahu") == 0)
            {
              strcpy(chr[li].planet[chr[li].count], "Rahu   ");
              rahurasi = li + 1;
              rahudeg = deg;
              rahumin = min;
              chr[li].index[chr[li].count] = 8;
              ind[Rasi][Rahu].index = li;
              if (li >= 6)
                lj = li - 6;
              else
                lj = li + 6;
              strcpy(chr[lj].planet[chr[lj].count], "Ketu   ");
              keturasi = lj + 1;
              ketudeg = deg;
              ketumin = min;
              chr[lj].index[chr[lj].count] = 9;
              ind[Rasi][Ketu].index = lj;
              if (secstrue)
                sprintf(chr[lj].deg[chr[lj].count], "[%02d:%02d:%02d]", deg, min, sec);
              else
                sprintf(chr[lj].deg[chr[lj].count], "%02d:%02d", deg, min);
              sprintf(chr[lj].mdeg[chr[lj].count], "%02d:%02d", deg, min);
              chr[lj].count++;
            }
            else if (ret[j] < 0.0)
              strcat(chr[li].planet[chr[li].count], "(R)");
            else if ((j != oAsc) && ((DFromR(fabs(ret[j])) / lret[j]) >= 2.0))
              strcat(chr[li].planet[chr[li].count], "(F)");
            else
              strcat(chr[li].planet[chr[li].count], "   ");
            chr[li].count++;
            break;
          }
        }
      }
#else
      AnsiColor(kObjA[j]);
      sprintf(sz, "%-4.4s: ", szObjName[j]); PrintSz(sz);
      PrintZodiac(planet[j]);
      sprintf(sz, " %c ", ret[j] >= 0.0 ? ' ' : chRet); PrintSz(sz);
      if (j <= cThing || j > cuspHi)
        PrintAltitude(planetalt[j]);
      else
        PrintTab('_', us.fSeconds ? 10 : 7);
      sprintf(sz, " (%c)", Dignify(j, SFromZ(planet[j])));
      PrintSz(FCusp(j) ? "    " : sz);
      k = inhouse[j];
      AnsiColor(kSignA(k));
      sprintf(sz, " [%2d%s house] ", k, szSuffix[k]); PrintSz(sz);
      AnsiColor(kDefault);
      sprintf(sz, "[%c] ", Dignify(j, k)); PrintSz(FCusp(j) ? "    " : sz);
      if ((j != oMoo || us.fPlacalc) &&
        (FObject(j) || ((j == oNod || j == oLil) && us.fPlacalc))) {
        PrintCh((char)(ret[i] < 0.0 ? '-' : '+'));
        rT = DFromR(RAbs(ret[j]));
        sprintf(sz, us.fSeconds ? (rT < 10.0 ? "%9.7f" : "%9.6f") :
          (rT < 10.0 ? "%5.3f" : "%5.2f"), rT); PrintSz(sz);
      } else
        PrintTab('_', us.fSeconds ? 10 : 6);
#endif
    }
#ifndef LOGAN
    if (!us.fSeconds) {

      /* For some lines, we have to append the house cusp positions. */

      if (i <= cSign) {
        PrintSz("  -  ");
        AnsiColor(kSignA(i));
        sprintf(sz, "House cusp %2d: ", i); PrintSz(sz);
        PrintZodiac(house[i]);
      }

      /* For some lines, we have to append the element table information. */

      if (i == cSign+2)
        PrintSz("     Car Fix Mut TOT");
      else if (i > cSign+2 && i < cSign+7) {
        k = i-(cSign+2)-1;
        AnsiColor(kElemA[k]);
        sprintf(sz, "  %c%c%c%3d %3d %3d %3d",
          szElem[k][0], szElem[k][1], szElem[k][2],
          et.coElemMode[k][0], et.coElemMode[k][1], et.coElemMode[k][2],
          et.coElem[k]); PrintSz(sz);
        AnsiColor(kDefault);
      } else if (i == cSign+7) {
        sprintf(sz, "  TOT %2d %3d %3d %3d",
          et.coMode[0], et.coMode[1], et.coMode[2], et.coSum); PrintSz(sz);
      } else if (i == oCore)
        PrintTab(' ', 23);
      else if (i >= uranLo) {
        sprintf(sz, "  Uranian #%d", i-uranLo+1); PrintSz(sz);
      }
      sz[0] = chNull;
      switch (i-cSign-1) {
      case 1: sprintf(sz, "   +:%2d", et.coYang);  break;
      case 2: sprintf(sz, "   -:%2d", et.coYin);   break;
      case 3: sprintf(sz, "   M:%2d", et.coMC);    break;
      case 4: sprintf(sz, "   N:%2d", et.coIC);    break;
      case 5: sprintf(sz, "   A:%2d", et.coAsc);   break;
      case 6: sprintf(sz, "   D:%2d", et.coDes);   break;
      case 7: sprintf(sz,    "<:%2d", et.coLearn); break;
      }
      PrintSz(sz);
    } else {
      PrintSz(" Decan: ");
      is.fSeconds = fFalse;
      PrintZodiac(Decan(planet[i]));
      is.fSeconds = us.fSeconds;
    }
    PrintL();
#endif
  }

#ifdef LOGAN
  if (!regular)
  {
    real rasc;
    struct tm ltm;
    long lclock;
    long clock;
    int flag1 = 0;
    int flag2 = 0;

    time(&clock);
    
    memset(&ltm, 0, sizeof(struct tm));
    ltm.tm_min = (int)(RFract(RAbs(Tim)) * 100.0 + rRound / 60.0);
    ltm.tm_hour = NFloor(Tim);
    ltm.tm_mday = Day;
    ltm.tm_mon = Mon - 1;
    ltm.tm_year = Yea - 1900;
    ltm.tm_isdst = SS;

    lclock = mktime(&ltm);

    if ((Lon == 122.00) && (Lat == 37.23)) { /* Sunnyvale */
        // Asc 2 is Chicago
        // Asc 3 is New York 
        flag1 = 2;
        flag2 = 3;
    } else {
        // Asc 2 is Chicago
        // Asc 3 is Sunnyvale
        flag1 = -1;
        flag2 = -3;
    }

    // printf("Lon %f Lat %f\n", OO, AA);

    // Asc 2 is Chicago
    rasc = getAsc(lclock, flag1);
    sscanf(SzZodiac(rasc), "%d%3s%d", &deg, zod, &min);
    strcpy(ddeg, SzDegree(rasc));
    if (strlen(ddeg) > 7)
      sscanf(&ddeg[7], "%02d", &sec);
    else
      sec = 0;
    for (li = 0; li < 12; li++)
    {
      if (strcmp(chr[li].name, zod) == 0)
      {
        asc2rasi = li + 1;
        asc2deg = deg;
        asc2min = min;
        strcpy(lag2, "LagC");
        sprintf(chr[li].planet[chr[li].count], "%-4.4s", "LagC");
        if (secstrue)
          sprintf(chr[li].deg[chr[li].count], "[%02d:%02d:%02d]", deg, min, sec);
        else
          sprintf(chr[li].deg[chr[li].count], "%02d:%02d", deg, min);
        sprintf(chr[li].mdeg[chr[li].count], "%02d:%02d", deg, min);
        chr[li].count++;
        ind[Rasi][Lag2].index = li;
        break;
      }
    }
 
    rasc = getAsc(lclock, flag2);
    sscanf(SzZodiac(rasc), "%d%3s%d", &deg, zod, &min);
    strcpy(ddeg, SzDegree(rasc));
    if (strlen(ddeg) > 7)
      sscanf(&ddeg[7], "%02d", &sec);
    else
      sec = 0;
    for (li = 0; li < 12; li++)
    {
      if (strcmp(chr[li].name, zod) == 0)
      {
        asc3rasi = li + 1;
        asc3deg = deg;
        asc3min = min;
        if (flag2 == 3) {
            strcpy(lag3, "LagN");
        } else {
            strcpy(lag3, "LagS");
        }
        sprintf(chr[li].planet[chr[li].count], "%-4.4s", lag3);
        if (secstrue)
          sprintf(chr[li].deg[chr[li].count], "[%02d:%02d:%02d]", deg, min, sec);
        else
          sprintf(chr[li].deg[chr[li].count], "%02d:%02d", deg, min);
        sprintf(chr[li].mdeg[chr[li].count], "%02d:%02d", deg, min);
        chr[li].count++;
        ind[Rasi][Lag3].index = li;
        break;
      }
    }
  }

#define SUN     0
#define MOON    1
#define MARS    2
#define RAHU    3
#define JUPITER 4
#define SATURN  5
#define MERCURY 6
#define KETU    7
#define VENUS   8

  {
    int ruler;
    int days;
    int months;
    int years;
    char *dst1;
    char *dst2;
    char *fmt1;
    char *fmt2;

    getRuler("Moon", Moon,  moonrasi, moondeg, moonmin, &ruler, &days, &months, &years, navamsam);

    {
      if (is.szFile != NULL) {
        DasaMain(is.szFile, Day, Mon, Yea, ruler, days, months, years);
      } else {
        extern int exportDesa;
        // For external use
        if (exportDesa) {
          printf("#--- %d %d %d %d %d %d %d\n",
            Day, Mon, Yea, ruler, days, months, years);
        }
      }

      getRuler("Sun", Sun, sunrasi, sundeg, sunmin, NULL, NULL, NULL, NULL, regular && navamsam);
      getRuler("Merc", Mercury, mercrasi, mercdeg, mercmin, NULL, NULL, NULL, NULL, regular && navamsam);
      getRuler("Venu", Venus, venurasi, venudeg, venumin, NULL, NULL, NULL, NULL, regular && navamsam);
      if (regular && navamsam) printf("\n"); 
      getRuler("Mars", Mars, marsrasi, marsdeg, marsmin, NULL, NULL, NULL, NULL, regular && navamsam);
      getRuler("Jupi", Jupiter, jupirasi, jupideg, jupimin, NULL, NULL, NULL, NULL, regular && navamsam);
      getRuler("Satu", Saturn, saturasi, satudeg, satumin, NULL, NULL, NULL, NULL, regular && navamsam);
      if (regular && navamsam) printf("\n"); 
      getRuler("Rahu", Rahu, rahurasi, rahudeg, rahumin, NULL, NULL, NULL, NULL, regular && navamsam);
      getRuler("Ketu", Ketu, keturasi, ketudeg, ketumin, NULL, NULL, NULL, NULL, regular && navamsam);
      getRuler(lag1, Lag1, asc1rasi, asc1deg, asc1min, NULL, NULL, NULL, NULL, regular && navamsam);
      if (regular && navamsam) printf("\n"); 
      if (!regular) {
        getRuler(lag2, Lag2, asc2rasi, asc2deg, asc2min, NULL, NULL, NULL, NULL, regular && navamsam);
        getRuler(lag3, Lag3, asc3rasi, asc3deg, asc3min, NULL, NULL, NULL, NULL, regular && navamsam);
      }
    }
    if (!regular && (csv == 0)) {
      real tret;
      tret = DFromR(fabs(ret[1])) / lret[1];
      printf("%-4.4s%-3.3s %4.2f ", szObjName[1], ret[1] < 0.0 ? "(R)" : tret >= 2.0 ? "(F)" : "   ", tret);
      tret = DFromR(fabs(ret[2])) / lret[2];
      printf("%-4.4s%-3.3s %4.2f ", szObjName[2], ret[2] < 0.0 ? "(R)" : tret >= 2.0 ? "(F)" : "   ", tret);
      tret = DFromR(fabs(ret[3])) / lret[3];
      printf("%-4.4s%-3.3s %4.2f ", szObjName[3], ret[3] < 0.0 ? "(R)" : tret >= 2.0 ? "(F)" : "   ", tret);
      tret = DFromR(fabs(ret[4])) / lret[4];
      printf("%-4.4s%-3.3s %4.2f\n", szObjName[4], ret[4] < 0.0 ? "(R)" : tret >= 2.0 ? "(F)" : "   ", tret);
      tret = DFromR(fabs(ret[5])) / lret[5];
      printf("%-4.4s%-3.3s %4.2f ", szObjName[5], ret[5] < 0.0 ? "(R)" : tret >= 2.0 ? "(F)" : "   ", tret);
      tret = DFromR(fabs(ret[6])) / lret[6];
      printf("%-4.4s%-3.3s %4.2f ", szObjName[6], ret[6] < 0.0 ? "(R)" : tret >= 2.0 ? "(F)" : "   ", tret);
      tret = DFromR(fabs(ret[7])) / lret[7];
      printf("%-4.4s%-3.3s %4.2f ", szObjName[7], ret[7] < 0.0 ? "(R)" : tret >= 2.0 ? "(F)" : "   ", tret);
      tret = DFromR(fabs(ret[16])) / lret[16];
      printf("%-4.4s%-3.3s %4.2f\n", szObjName[16], "   ", tret);
    }
    /*
    if (regular)
    {
      if (secstrue) {
        dst1 = " ----------------------------------------------------------------------- \n";
        dst2 = " -----------------                                     ----------------- \n";
        fmt1 = "|%7s%10s|%7s%10s|%7s%10s|%7s%10s|\n";
        fmt2 = "|%7s%10s|%7s%10s %7s%10s|%7s%10s|\n";
      }
      else {
        dst1 = " --------------------------------------------------- \n";
        dst2 = " ------------                           ------------ \n";
        fmt1 = "|%7s%5s|%7s%5s|%7s%5s|%7s%5s|\n";
        fmt2 = "|%7s%5s|%7s%5s %7s%5s|%7s%5s|\n";
      }
      printf("\n");
      printf("%s", dst1);
      for (lj = 0; lj < 8; lj++)
      {
        printf(fmt1, 
               chr[11].planet[lj], chr[11].deg[lj], 
               chr[0].planet[lj], chr[0].deg[lj],
               chr[1].planet[lj], chr[1].deg[lj],
               chr[2].planet[lj], chr[2].deg[lj]);
      }
      printf("%s", dst1);
      for (lj = 0; lj < 8; lj++)
      {
        printf(fmt2, 
               chr[10].planet[lj], chr[10].deg[lj], 
               "", "",
               "", "",
               chr[3].planet[lj], chr[3].deg[lj]);
      }
      printf("%s", dst2);
      for (lj = 0; lj < 8; lj++)
      {
        printf(fmt2, 
               chr[9].planet[lj], chr[9].deg[lj], 
               "", "",
               "", "",
               chr[4].planet[lj], chr[4].deg[lj]);
      }
      printf("%s", dst1);
      for (lj = 0; lj < 8; lj++)
      {
        printf(fmt1, 
               chr[8].planet[lj], chr[8].deg[lj], 
               chr[7].planet[lj], chr[7].deg[lj],
               chr[6].planet[lj], chr[6].deg[lj],
               chr[5].planet[lj], chr[5].deg[lj]);
      }
      printf("%s", dst1);
      printf("\n"); 
    }
    */

    if (predictor) {
            predProfession(asc);
    }

    if (navamsam)
    {
      dst1 = " ---------------------------------------------------        ----------------------- \n";
      dst2 = " ------------           Rasi            ------------        -----    Amsam    ----- \n";
      printf("\n%s", dst1);
      for (lj = 0; lj < 8; lj++)
      {
        printf("|%-7s%5s|%-7s%5s|%-7s%5s|%-7s%5s|", chr[11].planet[lj], chr[11].mdeg[lj],
                chr[0].planet[lj], chr[0].mdeg[lj],
                  chr[1].planet[lj], chr[1].mdeg[lj],
                chr[2].planet[lj], chr[2].mdeg[lj]);
        printf("      |%-5s|%-5s|%-5s|%-5s|\n", chr[11].navamsam[lj], chr[0].navamsam[lj], 
                  chr[1].navamsam[lj], chr[2].navamsam[lj]);
      }
      printf("%s", dst1);
      for (lj = 0; lj < 8; lj++)
      {
        printf("|%-7s%5s|%-7s%5s %-7s%5s|%-7s%5s|", chr[10].planet[lj], chr[10].mdeg[lj],
                "", "", "", "", 
                  chr[3].planet[lj], chr[3].mdeg[lj]);
        printf("      |%-5s|%-5s %-5s|%-5s|\n", chr[10].navamsam[lj], "", "", 
                  chr[3].navamsam[lj]);
      }
      printf("%s", dst2);
      for (lj = 0; lj < 8; lj++)
      {
        printf("|%-7s%5s|%-7s%5s %-7s%5s|%-7s%5s|", chr[9].planet[lj], chr[9].mdeg[lj],
                "", "", "", "",
               chr[4].planet[lj], chr[4].mdeg[lj]);
        printf("      |%-5s|%-5s %-5s|%-5s|\n", chr[9].navamsam[lj], "", "",
               chr[4].navamsam[lj]);
      }
      printf("%s", dst1);
      for (lj = 0; lj < 8; lj++)
      {
        printf("|%-7s%5s|%-7s%5s|%-7s%5s|%-7s%5s|", chr[8].planet[lj], chr[8].mdeg[lj],
                chr[7].planet[lj], chr[7].mdeg[lj],
                chr[6].planet[lj], chr[6].mdeg[lj],
                chr[5].planet[lj], chr[5].mdeg[lj]);
        printf("      |%-5s|%-5s|%-5s|%-5s|\n", chr[8].navamsam[lj], chr[7].navamsam[lj],
               chr[6].navamsam[lj], chr[5].navamsam[lj]);
      }
      printf("%s", dst1);
      printf("\n");

    } else if (csv) {
        int ki, kj;

        printf("%02d:%02d", NFloor(Tim), (int)(RFract(RAbs(Tim)) * 100.0 + rRound / 60.0));
        for (ki = 0; ki < Smax; ki++) {
            for (kj = 0; kj < Pmax; kj++) {
                printf(",%s", chr[ind[ki][kj].index].name);
            }
        }
        printf("\n");
    }
      if (regular) {
        printf("  Ascendant  Benefic   Malefic   Yogakaraka    Marakas \n\n");
        switch(asc)
        {
          case 1:
            printf("             Mars      Saturn    Sun           Mercury\n");
            printf("  Mesha      Jupiter   Mercury   Moon          Saturn \n");
            printf("             Sun       Venus                          \n");
            break;
          case 2:
            printf("             Saturn    Jupiter   Saturn        Jupiter\n");
            printf("  Vrishabha  Sun       Moon                    Venus  \n");
            printf("             Mercury                                  \n");
            printf("             Venus                                    \n");
            break;
          case 3:
            printf("             Venus     Mars      Venus+        Mars   \n");
            printf("  Midhuna    Mercury   Jupiter   Mercury       Jupiter\n");
            printf("                       Sun                            \n");
            break;
          case 4:
            printf("             Mars      Venus     Mars          Mercury\n");
            printf("  Kataka     Jupiter   Mercury                 Venus  \n");
            printf("             Moon      Saturn                         \n");
            break;
          case 5:
            printf("             Mars      Mercury   Mars          Mercury\n");
            printf("  Simha      Jupiter   Venus                   Venus  \n");
            printf("             Sun       Saturn                         \n");
            break;
          case 6:
            printf("             Mercury   Mars      Venus         Mars   \n");
            printf("  Kanya      Venus     Jupiter   Mercury       Jupiter\n");
            printf("                       Moon                           \n");
            break;
          case 7:
            printf("             Saturn    Jupiter   Saturn        Jupiter\n");
            printf("  Thula      Mercury   Sun       Moon+                \n");
            printf("             Venus     Mars      Mercury              \n");
            break;
          case 8:
            printf("             Sun       Mercury   Sun+          Mercury\n");
            printf("  Vrischika  Moon      Venus     Moon          Venus  \n");
            printf("             Mars      Saturn                         \n");
            printf("             Jupiter                                  \n");
            break;
          case 9:
            printf("             Mars      Venus     Sun+          Venus  \n");
            printf("  Dhanas     Sun       Saturn    Mercury              \n");
            printf("             Jupiter                                  \n");
            break;
          case 10:
            printf("             Venus     Mars      Venus         Mars   \n");
            printf("  Makara     Mercury   Moon                    Jupiter\n");
            printf("             Saturn    Jupiter                        \n");
            break;
          case 11:
            printf("             Venus     Jupiter   Venus         Jupiter\n");
            printf("  Kumbha     Saturn    Moon                           \n");
            printf("             Sun       Mars                           \n");
            printf("             Mercury                                  \n");
            break;
          case 12:
            printf("             Mars      Sun       Mars+         Mercury\n");
            printf("  Meena      Moon      Mercury   Jupiter       Venus  \n");
            printf("             Jupiter   Venus                   Saturn \n");
            printf("                       Saturn                         \n");
            break;
        }
        printf("\n");
      }
  }


#endif

  /* Do another loop to print out the stars in their specified order. */

  /*
  if (us.nStar) for (i = starLo; i <= starHi; i++) if (!ignore[i]) {
    j = oNorm+starname[i-oNorm];
    AnsiColor(kObjA[j]);
    sprintf(sz, "%-4.4s: ", szObjName[j]); PrintSz(sz);
    PrintZodiac(planet[j]);
    PrintSz("   ");
    PrintAltitude(planetalt[j]);
    k = inhouse[j];
    AnsiColor(kSignA(k));
    sprintf(sz, "     [%2d%s house]", k, szSuffix[k]); PrintSz(sz);
    AnsiColor(kDefault);
    sprintf(sz, "     ______%s Star #%2d: %5.2f\n",
      us.fSeconds ? "____" : " ", i-oNorm, rStarBright[j-oNorm]); PrintSz(sz);
  }
  */
}

#ifdef LOGAN

getRuler(plannet, index, rasi, deg, min, ruler, days, months, years, flag)
char *plannet;
int index;
int rasi;
int deg;
int min;
int *ruler;
int *days;
int *months;
int *years;
{
  int actualmin = 0;
  int remainmin = 0;
  int moonflag = 0;
  int padam = 0;
  char star[24];
  int offset = 0;

  if (strcmp(plannet, "Moon") == 0) {
    moonflag = 1;
  }
  actualmin = deg * 60 + min;
  switch(rasi)
  {
    case 1:
      if (moonflag)
        if (flag) printf("Rasi: Mesham, ");
      if (actualmin >= 0 && actualmin < 800)
      { 
        padam = (actualmin / 200) + 1;
        strcpy(star, "Aswini");
        offset = 0;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Ketu ", star, padam);
          remainmin = 800 - actualmin; 
          getBalance(remainmin, years, months, days, 7);
          *ruler = KETU;
        }
      }
      else
      if (actualmin >= 800 && actualmin < 1600)
      {
        padam = ((actualmin - 800) / 200) + 1;
        strcpy(star, "Bharani");
        offset = 4;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Venus ", star, padam);
          remainmin = 1600 - actualmin;
          getBalance(remainmin, years, months, days, 20);
          *ruler = VENUS;
        }
      }
      else
      if (actualmin >= 1600 && actualmin < 1800)
      {
        padam = ((actualmin - 1600) / 200) + 1;
        strcpy(star, "Krithikai");
        offset = 8;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Sun ", star, padam);
          remainmin = 2400 - actualmin;
          getBalance(remainmin, years, months, days, 6);
          *ruler = SUN;
        }
      }
      break;
    case 2:
      if (moonflag)
        if (flag) printf("Rasi: Vrishabham, ");
      if (actualmin >= 0 && actualmin < 600)
      {
        padam = (actualmin / 200) + 2;
        strcpy(star, "Krithikai");
        offset = 8;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Sun ", star, padam);
          remainmin = 600 - actualmin;
          getBalance(remainmin, years, months, days, 6);
          *ruler = SUN;
        }
      }
      else
      if (actualmin >= 600 && actualmin < 1400)
      {
        padam = ((actualmin - 600) / 200) + 1;
        strcpy(star, "Rohini");
        offset = 0;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Moon ", star, padam);
          remainmin = 1400 - actualmin;
          getBalance(remainmin, years, months, days, 10);
          *ruler = MOON;
        }
      }
      else
      if (actualmin >= 1400 && actualmin < 1800)
      {
        padam = ((actualmin - 1400) / 200) + 1;
        strcpy(star, "Mrigashir");
        offset = 4;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Mars ", star, padam);
          remainmin = 2200 - actualmin;
          getBalance(remainmin, years, months, days, 7);
          *ruler = MARS;
        }
      }
      break;
    case 3:
      if (moonflag)
        if (flag) printf("Rasi: Mithunam, ");
      if (actualmin >= 0 && actualmin < 400)
      { 
        padam = (actualmin / 200) + 3;
        strcpy(star, "Mrigashir");
        offset = 4;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Mars ", star, padam);
          remainmin = 400 - actualmin;
          getBalance(remainmin, years, months, days, 7);
          *ruler = MARS;
        }
      }
      else
      if (actualmin >= 400 && actualmin < 1200)
      {
        padam = ((actualmin - 400) / 200) + 1;
        strcpy(star, "Thiruvadu");
        offset = 8;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Rahu ", star, padam);
          remainmin = 1200 - actualmin;
          getBalance(remainmin, years, months, days, 18);
          *ruler = RAHU;
        }
      }
      else
      if (actualmin >= 1200 && actualmin < 1800)
      {
        padam = ((actualmin - 1200) / 200) + 1;
        strcpy(star, "Punrpusam");
        offset = 0;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Jupiter ", star, padam);
          remainmin = 2000 - actualmin;
          getBalance(remainmin, years, months, days, 16);
          *ruler = JUPITER;
        }
      }
      break;
    case 4:
      if (moonflag)
        if (flag) printf("Rasi: Katakam, ");
      if (actualmin >= 0 && actualmin < 200)
      { 
        padam = (actualmin / 200) + 4;
        strcpy(star, "Punrpusam");
        offset = 0;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Jupiter ", star, padam);
          remainmin = 200 - actualmin;
          getBalance(remainmin, years, months, days, 16);
          *ruler = JUPITER;
        }
      }
      else
      if (actualmin >= 200 && actualmin < 1000)
      {
        padam = ((actualmin - 200) / 200) + 1;
        strcpy(star, "Pusam");
        offset = 4;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Saturn ", star, padam);
          remainmin = 1000 - actualmin;
          getBalance(remainmin, years, months, days, 19);
          *ruler = SATURN;
        }
      }
      else
      if (actualmin >= 1000 && actualmin < 1800)
      {
        padam = ((actualmin - 1000) / 200) + 1;
        strcpy(star, "Ayilyam");
        offset = 8;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Mercury ", star, padam);
          remainmin = 1800 - actualmin;
          getBalance(remainmin, years, months, days, 17);
          *ruler = MERCURY;
        }
      }
      break;
    case 5:
      if (moonflag)
        if (flag) printf("Rasi: Simham, ");
      if (actualmin >= 0 && actualmin < 800)
      { 
        padam = (actualmin / 200) + 1;
        strcpy(star, "Magham");
        offset = 0;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Ketu ", star, padam);
          remainmin = 800 - actualmin;
          getBalance(remainmin, years, months, days, 7);
          *ruler = KETU;
        }
      }
      else
      if (actualmin >= 800 && actualmin < 1600)
      {
        padam = ((actualmin - 800) / 200) + 1;
        strcpy(star, "Puram");
        offset = 4;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Venus ", star, padam);
          remainmin = 1600 - actualmin;
          getBalance(remainmin, years, months, days, 20);
          *ruler = VENUS;
        }
      }
      else
      if (actualmin >= 1600 && actualmin < 1800)
      {
        padam = ((actualmin - 1600) / 200) + 1;
        strcpy(star, "Uttaram");
        offset = 8;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Sun ", star, padam);
          remainmin = 2400 - actualmin;
          getBalance(remainmin, years, months, days, 6);
          *ruler = SUN;
        }
      }
      break;
    case 6:
      if (moonflag)
        if (flag) printf("Rasi: Kanya, ");
      if (actualmin >= 0 && actualmin < 600)
      {
        padam = (actualmin / 200) + 2;
        strcpy(star, "Uttaram");
        offset = 8;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Sun ", star, padam);
          remainmin = 600 - actualmin;
          getBalance(remainmin, years, months, days, 6);
          *ruler = SUN;
        }
      }
      else
      if (actualmin >= 600 && actualmin < 1400)
      {
        padam = ((actualmin - 600) / 200) + 1;
        strcpy(star, "Hastam");
        offset = 0;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Moon ", star, padam);
          remainmin = 1400 - actualmin;
          getBalance(remainmin, years, months, days, 10);
          *ruler = MOON;
        }
      }
      else
      if (actualmin >= 1400 && actualmin < 1800)
      {
        padam = ((actualmin - 1400) / 200) + 1;
        strcpy(star, "Chitrai");
        offset = 4;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Mars ", star, padam);
          remainmin = 2200 - actualmin;
          getBalance(remainmin, years, months, days, 7);
          *ruler = MARS;
        }
      }
      break;
    case 7:
      if (moonflag)
        if (flag) printf("Rasi: Tulam, ");
      if (actualmin >= 0 && actualmin < 400)
      { 
        padam = (actualmin / 200) + 3;
        strcpy(star, "Chitrai");
        offset = 4;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Mars ", star, padam);
          remainmin = 400 - actualmin;
          getBalance(remainmin, years, months, days, 7);
          *ruler = MARS;
        }
      }
      else
      if (actualmin >= 400 && actualmin < 1200)
      {
        padam = ((actualmin - 400) / 200) + 1;
        strcpy(star, "Swati");
        offset = 8;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Rahu ", star, padam);
          remainmin = 1200 - actualmin;
          getBalance(remainmin, years, months, days, 18);
          *ruler = RAHU;
        }
      }
      else
      if (actualmin >= 1200 && actualmin < 1800)
      {
        padam = ((actualmin - 1200) / 200) + 1;
        strcpy(star, "Vishakham");
        offset = 0;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Jupiter ", star, padam);
          remainmin = 2000 - actualmin;
          getBalance(remainmin, years, months, days, 16);
          *ruler = JUPITER;
        }
      }
      break;
    case 8:
      if (moonflag)
        if (flag) printf("Rasi: Vrishchikam, ");
      if (actualmin >= 0 && actualmin < 200)
      { 
        padam = (actualmin / 200) + 4;
        strcpy(star, "Vishakham");
        offset = 0;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Jupiter ", star, padam);
          remainmin = 200 - actualmin;
          getBalance(remainmin, years, months, days, 16);
          *ruler = JUPITER;
        }
      }
      else
      if (actualmin >= 200 && actualmin < 1000)
      {
        padam = ((actualmin - 200) / 200) + 1;
        strcpy(star, "Anusham");
        offset = 4;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Saturn ", star, padam);
          remainmin = 1000 - actualmin;
          getBalance(remainmin, years, months, days, 19);
          *ruler = SATURN;
        }
      }
      else
      if (actualmin >= 1000 && actualmin < 1800)
      {
        padam = ((actualmin - 1000) / 200) + 1;
        strcpy(star, "Kettai");
        offset = 8;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Mercury ", star, padam);
          remainmin = 1800 - actualmin;
          getBalance(remainmin, years, months, days, 17);
          *ruler = MERCURY;
        }
      }
      break;
    case 9:
      if (moonflag)
        if (flag) printf("Rasi: Dhanus, ");
      if (actualmin >= 0 && actualmin < 800)
      { 
        padam = (actualmin / 200) + 1;
        strcpy(star, "Mulam");
        offset = 0;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Ketu ", star, padam);
          remainmin = 800 - actualmin;
          getBalance(remainmin, years, months, days, 7);
          *ruler = KETU;
        }
      }
      else
      if (actualmin >= 800 && actualmin < 1600)
      {
        padam = ((actualmin - 800) / 200) + 1;
        strcpy(star, "Puradam");
        offset = 4;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Venus ", star, padam);
          remainmin = 1600 - actualmin;
          getBalance(remainmin, years, months, days, 20);
          *ruler = VENUS;
        }
      }
      else
      if (actualmin >= 1600 && actualmin < 1800)
      {
        padam = ((actualmin - 1600) / 200) + 1;
        strcpy(star, "Uttaradam");
        offset = 8;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Sun ", star, padam);
          remainmin = 2400 - actualmin;
          getBalance(remainmin, years, months, days, 6);
          *ruler = SUN;
        }
      }
      break;
    case 10:
      if (moonflag)
        if (flag) printf("Rasi: Makaram, ");
      if (actualmin >= 0 && actualmin < 600)
      {
        padam = (actualmin / 200) + 2;
        strcpy(star, "Uttaradam");
        offset = 8;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Sun ", star, padam);
          remainmin = 600 - actualmin;
          getBalance(remainmin, years, months, days, 6);
          *ruler = SUN;
        }
      }
      else
      if (actualmin >= 600 && actualmin < 1400)
      {
        padam = ((actualmin - 600) / 200) + 1;
        strcpy(star, "Tiruvonam");
        offset = 0;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Moon ", star, padam);
          remainmin = 1400 - actualmin;
          getBalance(remainmin, years, months, days, 10);
          *ruler = MOON;
        }
      }
      else
      if (actualmin >= 1400 && actualmin < 1800)
      {
        padam = ((actualmin - 1400) / 200) + 1;
        strcpy(star, "Avitam");
        offset = 4;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Mars ", star, padam);
          remainmin = 2200 - actualmin;
          getBalance(remainmin, years, months, days, 7);
          *ruler = MARS;
        }
      }
      break;
    case 11:
      if (moonflag)
        if (flag) printf("Rasi: Kumbham, ");
      if (actualmin >= 0 && actualmin < 400)
      { 
        padam = (actualmin / 200) + 3;
        strcpy(star, "Avitam");
        offset = 4;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Mars ", star, padam);
          remainmin = 400 - actualmin;
          getBalance(remainmin, years, months, days, 7);
          *ruler = MARS;
        }
      }
      else
      if (actualmin >= 400 && actualmin < 1200)
      {
        padam = ((actualmin - 400) / 200) + 1;
        strcpy(star, "Sathayam");
        offset = 8;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Rahu ", star, padam);
          remainmin = 1200 - actualmin;
          getBalance(remainmin, years, months, days, 18);
          *ruler = RAHU;
        }
      }
      else
      if (actualmin >= 1200 && actualmin < 1800)
      {
        padam = ((actualmin - 1200) / 200) + 1;
        strcpy(star, "Puratathi");
        offset = 0;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Jupiter ", star, padam);
          remainmin = 2000 - actualmin;
          getBalance(remainmin, years, months, days, 16);
          *ruler = JUPITER;
        }
      }
      break;
    case 12:
      if (moonflag)
        if (flag) printf("Rasi: Minam, ");
      if (actualmin >= 0 && actualmin < 200)
      { 
        padam = (actualmin / 200) + 4;
        strcpy(star, "Puratathi");
        offset = 0;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Jupiter ", star, padam);
          remainmin = 200 - actualmin;
          getBalance(remainmin, years, months, days, 16);
          *ruler = JUPITER;
        }
      }
      else
      if (actualmin >= 200 && actualmin < 1000)
      {
        padam = ((actualmin - 200) / 200) + 1;
        strcpy(star, "Utartathi");
        offset = 4;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Saturn ", star, padam);
          remainmin = 1000 - actualmin;
          getBalance(remainmin, years, months, days, 19);
          *ruler = SATURN;
        }
      }
      else
      if (actualmin >= 1000 && actualmin < 1800)
      {
        padam = ((actualmin - 1000) / 200) + 1;
        strcpy(star, "Revati");
        offset = 8;
        if (moonflag)
        {
          if (flag) printf("Nakshatram: %s, %d Padam, Mercury ", star, padam);
          remainmin = 1800 - actualmin;
          getBalance(remainmin, years, months, days, 17);
          *ruler = MERCURY;
        }
      }
      break;
  }
  if (moonflag) {
    if (flag) printf("Dasa Balance %dY %dM %dD\n\n", *years, *months, *days);
  } else
    if (flag) printf("%-4s %10s(%d)  ", plannet, star, padam); 

  chr[(padam - 1) + offset].navaindex[chr[(padam - 1) + offset].navacount] = index;
  strcpy(chr[(padam - 1) + offset].navamsam[chr[(padam - 1) + offset].navacount++], plannet);
  ind[Amsa][index].index = ((padam - 1) + offset);
}

/*
Get the balance day from degress/min
*/
getBalance(remainmin, years, months, days, numyear)
int remainmin;
int *years;
int *months;
int *days;
int numyear;
{

  float val;

  val = remainmin / 800.0 * numyear;
  *years = (int)val;
  val = val - *years;
  val = val * 12;
  *months = (int)val;
  val = val - *months; 
  *days = (int)(val * 30);

}

#endif /* LOGAN */


/* Print out the aspect and midpoint grid for a chart, as specified with the */
/* -g switch. (Each grid row takes up 4 lines of text.)                      */

void ChartGrid()
{
  char sz[cchSzDef];
  int x, y, r, x1, y1, temp;

#ifdef INTERPRET
  if (us.fInterpret) {    /* Print interpretation instead if -I in effect. */
    InterpretGrid();
    return;
  }
#endif

  for (y1 = 0, y = 1; y <= cObj; y++) if (!ignore[y])
    for (r = 1; r <= 4; r++) {
      for (x1 = 0, x = 1; x <= cObj; x++) if (!ignore[x]) {
        if (y1 > 0 && x1 > 0 && y+r > 2)
          PrintCh((char)(r > 1 ? chV : chC));
        if (r > 1) {
          temp = grid->n[x][y];

          /* Print aspect rows. */

          if (x < y) {
            if (temp)
              AnsiColor(kAspA[temp]);
            if (r == 2)
              PrintSz(temp ? szAspectAbbrev[temp] : "   ");
            else if (!temp)
              PrintSz("   ");
            else {
              if (r == 3) {
                if (grid->v[x][y] < 6000)
                  sprintf(sz, "%c%2d", us.fAppSep ?
                    (grid->v[x][y] < 0 ? 'a' : 's') :
                    (grid->v[x][y] < 0 ? '-' : '+'), abs(grid->v[x][y])/60);
                else
                  sprintf(sz, "%3d", abs(grid->v[x][y])/60);
              } else
                sprintf(sz, "%02d'", abs(grid->v[x][y])%60);
              PrintSz(sz);
            }

          /* Print midpoint rows. */

          } else if (x > y) {
            AnsiColor(kSignA(temp));
            if (r == 2) {
              temp = grid->n[x][y];
              sprintf(sz, "%c%c%c", chSig3(temp));
            } else if (r == 3) {
              sprintf(sz, "%2d%c", grid->v[x][y]/60, chDeg0);
            } else
              sprintf(sz, "%02d'", grid->v[x][y]%60);
            PrintSz(sz);

          /* Print the diagonal of object names. */

          } else {
            AnsiColor(kReverse);
            if (r == 2) {
              AnsiColor(kObjA[y]);
              sprintf(sz, "%c%c%c", chObj3(y));
            } else {
              temp = SFromZ(planet[y]);
              AnsiColor(kSignA(temp));
              if (r == 3)
                sprintf(sz, "%2d%c", (int)planet[y] - (temp-1)*30, chDeg0);
              else
                sprintf(sz, "%c%c%c", chSig3(temp));
            }
            PrintSz(sz);
          }
          AnsiColor(kDefault);
        } else
          if (y1 > 0)
            PrintTab(chH, 3);
        x1++;
      }
      if (y+r > 2)
        PrintL();
      y1++;
    }
}


/* This is a subprocedure of DisplayGrands(). Here we print out one aspect */
/* configuration found by the parent procedure.                            */

void PrintGrand(ac, i1, i2, i3, i4)
char ac;
int i1, i2, i3, i4;
{
  char sz[cchSzDef];
  int asp;

  switch (ac) {
  case acS : asp = aCon; break;
  case acGT: asp = aTri; break;
  case acTS: asp = aOpp; break;
  case acY : asp = aInc; break;
  case acGC: asp = aSqu; break;
  case acC : asp = aSex; break;
  default: ;
  }
  AnsiColor(kAspA[asp]);
  sprintf(sz, "%-11s", szAspectConfig[ac]); PrintSz(sz);
  AnsiColor(kDefault);
  sprintf(sz, " %s ",
    ac == acS || ac == acGT || ac == acGC ? "with" : "from"); PrintSz(sz);
  AnsiColor(kObjA[i1]);
  sprintf(sz, "%c%c%c: ", chObj3(i1)); PrintSz(sz);
  PrintZodiac(planet[i1]);
  sprintf(sz, " %s ", ac == acS || ac == acGT ? "and" : "to "); PrintSz(sz);
  AnsiColor(kObjA[i2]);
  sprintf(sz, "%c%c%c: ", chObj3(i2)); PrintSz(sz);
  PrintZodiac(planet[i2]);
  sprintf(sz, " %s ", ac == acGC || ac == acC ? "to " : "and"); PrintSz(sz);
  AnsiColor(kObjA[i3]);
  sprintf(sz, "%c%c%c: ", chObj3(i3)); PrintSz(sz);
  PrintZodiac(planet[i3]);
  if (ac == acGC || ac == acC) {
    PrintSz(" to ");
    AnsiColor(kObjA[i4]);
    sprintf(sz, "%c%c%c: ", chObj3(i4)); PrintSz(sz);
    PrintZodiac(planet[i4]);
  }
  PrintL();
}


/* Scan the aspect grid of a chart and print out any major configurations, */
/* as specified with the -g0 switch.                                       */

void DisplayGrands()
{
  int cac = 0, i, j, k, l;

  for (i = 1; i <= cObj; i++) if (!ignore[i])
    for (j = 1; j <= cObj; j++) if (j != i && !ignore[j])
      for (k = 1; k <= cObj; k++) if (k != i && k != j && !ignore[k]) {

        /* Is there a Stellium among the current three planets? */

        if (i < j && j < k && grid->n[i][j] == aCon &&
            grid->n[i][k] == aCon && grid->n[j][k] == aCon) {
          cac++;
          PrintGrand(acS, i, j, k, l);

        /* Is there a Grand Trine? */

        } else if (i < j && j < k && grid->n[i][j] == aTri &&
            grid->n[i][k] == aTri && grid->n[j][k] == aTri) {
          cac++;
          PrintGrand(acGT, i, j, k, l);

        /* Is there a T-Square? */

        } else if (j < k && grid->n[j][k] == aOpp &&
            grid->n[Min(i, j)][Max(i, j)] == aSqu &&
            grid->n[Min(i, k)][Max(i, k)] == aSqu) {
          cac++;
          PrintGrand(acTS, i, j, k, l);

        /* Is there a Yod? */

        } else if (j < k && grid->n[j][k] == aSex &&
            grid->n[Min(i, j)][Max(i, j)] == aInc &&
            grid->n[Min(i, k)][Max(i, k)] == aInc) {
          cac++;
          PrintGrand(acY, i, j, k, l);
        }
        for (l = 1; l <= cObj; l++) if (!ignore[l]) {

          /* Is there a Grand Cross among the current four planets? */

          if (i < j && i < k && i < l && j < l && grid->n[i][j] == aSqu &&
              grid->n[Min(j, k)][Max(j, k)] == aSqu &&
              grid->n[Min(k, l)][Max(k, l)] == aSqu &&
              grid->n[i][l] == aSqu &&
              MinDistance(planet[i], planet[k]) > 150.0 &&
              MinDistance(planet[j], planet[l]) > 150.0) {
            cac++;
            PrintGrand(acGC, i, j, k, l);

          /* Is there a Cradle? */

          } else if (i < l && grid->n[Min(i, j)][Max(i, j)] == aSex &&
              grid->n[Min(j, k)][Max(j, k)] == aSex &&
              grid->n[Min(k, l)][Max(k, l)] == aSex &&
              MinDistance(planet[i], planet[l]) > 150.0) {
            cac++;
            PrintGrand(acC, i, j, k, l);
          }
        }
      }
  if (!cac)
    PrintSz("No major configurations in aspect grid.\n");
}


/* This is subprocedure of ChartWheel(). Here we print out the location */
/* of a particular house cusp as well as what house cusp number it is.  */

void PrintHouse(i, left)
int i, left;
{
  char sz[cchSzDef];

  if (!left)
    PrintZodiac(house[i]);
  AnsiColor(kSignA(i));
  sprintf(sz, "<%d>", i); PrintSz(sz);
  if (left)
    PrintZodiac(house[i]);
  else
    AnsiColor(kDefault);
}


/* Another subprocedure of ChartWheel(). Print out one of the chart info */
/* rows in the middle of the wheel (which may be blank) given an index.  */

void PrintWheelCenter(irow)
int irow;
{
  char sz[cchSzDef];
  int cch, nT;

  if (*ciMain.nam == chNull && irow >= 2)    /* Don't have blank lines if */
    irow++;                                  /* the name and/or location  */
  if (*ciMain.loc == chNull && irow >= 4)    /* strings are empty.        */
    irow++;
  switch (irow) {
  case 1:
    sprintf(sz, "%s %s chart", szAppName, szVersionCore);
    break;
  case 2:
    sprintf(sz, "%s", ciMain.nam);
    break;
  case 3:
    nT = DayOfWeek(Mon, Day, Yea);
    sprintf(sz, "%c%c%c %s %s", chDay3(nT), SzDate(Mon, Day, Yea, 2),
      SzTim(Tim));
    break;
  case 4:
    sprintf(sz, "%s", ciMain.loc);
    break;
  case 5:
    nT = (int)(RFract(RAbs(Zon))*100.0+rRound);
    sprintf(sz, "%cT %c%02d:%02d, %s", ChDst(Dst),
      Zon > 0.0 ? '-' : '+', (int)RAbs(Zon), nT, SzLocation(Lon, Lat));
    break;
  case 6:
    sprintf(sz, "%s Houses", szSystem[us.nHouseSystem]);
    break;
  case 7:
    sprintf(sz, "Julian Day = %12.4f", JulianDayFromTime(T));
    break;
  default:
    *sz = chNull;
  }
  cch = CchSz(sz);
  nT = WHEELCOLS*2-1 + is.fSeconds*8;
  PrintTab(' ', (nT - cch) / 2);
  PrintSz(sz);
  PrintTab(' ', nT-cch - (nT - cch) / 2);
}


/* Yet another subprocedure of ChartWheel(). Here we print out one line */
/* in a particular house cell (which may be blank).                     */

void PrintWheelSlot(obj)
int obj;
{
  char sz[cchSzDef];

  if (obj) {
    AnsiColor(kObjA[obj]);
    sprintf(sz, " %c%c%c ", chObj3(obj)); PrintSz(sz);
    PrintZodiac(planet[obj]);
    sprintf(sz, "%c ", ret[obj] < 0.0 ? 'r' : ' '); PrintSz(sz);
    PrintTab(' ', WHEELCOLS-15);
  } else                            /* This particular line is blank. */
    PrintTab(' ', WHEELCOLS-1 + is.fSeconds*4);
}


/* Display all the objects in a wheel format on the screen, as specified */
/* with the -w switch. The wheel is divided into the 12 houses and the   */
/* planets are placed accordingly.                                       */

void ChartWheel()
{
  byte wheel[cSign][WHEELROWS];
  int wheelcols, count = 0, i, j, k, l;

  /* If the seconds (-b0) flag is set, we'll print all planet and house    */
  /* locations to the nearest zodiac second instead of just to the minute. */

  wheelcols = WHEELCOLS + is.fSeconds*4;

  for (i = 0; i < cSign; i++)
    for (j = 0; j < us.nWheelRows; j++)    /* Clear out array from the */
      wheel[i][j] = 0;                     /* last time we used it.    */

  /* This section of code places each object in the wheel house array. */

  for (i = 1; i <= cObj && count < us.nWheelRows*12; i++) {
    if (ignore[i] || (FCusp(i) &&
      MinDistance(planet[i], house[i-oAsc+1]) < rRound/60.0))
      continue;

    /* Try to put object in its proper house. If no room, */
    /* then overflow over to the next succeeding house.   */

    for (j = inhouse[i]-1; j < cSign; j = j < cSign ? (j+1)%cSign : j) {

      /* Now try to find the proper place in the house to put the object. */
      /* This is in sorted order, although a check is made for 0 Aries.   */

      if (wheel[j][us.nWheelRows-1] > 0)
        continue;
      l = house[j+1] > house[Mod12(j+2)];
      for (k = 0; wheel[j][k] > 0 && (planet[i] >= planet[wheel[j][k]] ||
         (l && planet[i] < rDegHalf && planet[wheel[j][k]] > rDegHalf)) &&
        !(l && planet[i] > rDegHalf && planet[wheel[j][k]] < rDegHalf); k++)
        ;

      /* Actually insert object in proper place. */

      if (wheel[j][k] <= 0)
        wheel[j][k] = i;
      else {
        for (l = us.nWheelRows-1; l > k; l--)
          wheel[j][l] = wheel[j][l-1];
        wheel[j][k] = i;
      }
      count++;
      j = cSign;
    }
  }

  /* Now, if this is really the -w switch and not -w0, then reverse the */
  /* order of objects in western houses for more intuitive reading.     */

  if (!us.fWheelReverse)
    for (i = 3; i < 9; i++)
      for (j = 0; j < us.nWheelRows/2; j++) {
        k = us.nWheelRows-1-j;
        l = wheel[i][j]; wheel[i][j] = wheel[i][k]; wheel[i][k] = l;
      }

  /* Here we actually print the wheel and the objects in it. */

  PrintCh(chNW); PrintTab(chH, WHEELCOLS-8); PrintHouse(11, fTrue);
  PrintTab(chH, WHEELCOLS-11); PrintHouse(10, fTrue);
  PrintTab(chH, WHEELCOLS-10); PrintHouse(9, fTrue);
  PrintTab(chH, wheelcols-4); PrintCh(chNE); PrintL();
  for (i = 0; i < us.nWheelRows; i++) {
    for (j = 10; j >= 7; j--) {
      PrintCh(chV); PrintWheelSlot(wheel[j][i]);
    }
    PrintCh(chV); PrintL();
  }
  PrintHouse(12, fTrue); PrintTab(chH, WHEELCOLS-11);
  PrintCh(chC); PrintTab(chH, wheelcols-1); PrintCh(chJN);
  PrintTab(chH, wheelcols-1); PrintCh(chC); PrintTab(chH, WHEELCOLS-10);
  PrintHouse(8, fFalse); PrintL();
  for (i = 0; i < us.nWheelRows; i++) {
    PrintCh(chV); PrintWheelSlot(wheel[11][i]); PrintCh(chV);
    PrintWheelCenter(i);
    PrintCh(chV); PrintWheelSlot(wheel[6][i]);
    PrintCh(chV); PrintL();
  }
  PrintHouse(1, fTrue); PrintTab(chH, WHEELCOLS-10);
  PrintCh(chJW); PrintWheelCenter(us.nWheelRows); PrintCh(chJE);
  PrintTab(chH, WHEELCOLS-10); PrintHouse(7, fFalse); PrintL();
  for (i = 0; i < us.nWheelRows; i++) {
    PrintCh(chV); PrintWheelSlot(wheel[0][i]); PrintCh(chV);
    PrintWheelCenter(us.nWheelRows+1 + i);
    PrintCh(chV); PrintWheelSlot(wheel[5][i]);
    PrintCh(chV); PrintL();
  }
  PrintHouse(2, fTrue); PrintTab(chH, WHEELCOLS-10);
  PrintCh(chC); PrintTab(chH, wheelcols-1); PrintCh(chJS);
  PrintTab(chH, wheelcols-1); PrintCh(chC);
  PrintTab(chH, WHEELCOLS-10); PrintHouse(6, fFalse); PrintL();
  for (i = 0; i < us.nWheelRows; i++) {
    for (j = 1; j <= 4; j++) {
      PrintCh(chV); PrintWheelSlot(wheel[j][i]);
    }
    PrintCh(chV); PrintL();
  }
  PrintCh(chSW); PrintTab(chH, wheelcols-4); PrintHouse(3, fFalse);
  PrintTab(chH, WHEELCOLS-10); PrintHouse(4, fFalse);
  PrintTab(chH, WHEELCOLS-10); PrintHouse(5, fFalse);
  PrintTab(chH, WHEELCOLS-7); PrintCh(chSE); PrintL();
}


/* Display all aspects between objects in the chart, one per line, in       */
/* sorted order based on the total "power" of the aspect, as specified with */
/* the -a switch. The same influences used for -I charts are used here.     */

void ChartAspect()
{
  int ca[cAspect + 1], co[objMax];
  char sz[cchSzDef];
  int pcut = 30000, icut, jcut, phi, ihi, jhi, ahi, p, i, j, k, count = 0;
  real ip, jp, rPowSum = 0.0;

  ClearB((lpbyte)ca, (cAspect + 1)*(int)sizeof(int));
  ClearB((lpbyte)co, objMax*(int)sizeof(int));
  loop {
    phi = -1;

    /* Search for the next most powerful aspect in the aspect grid. */

    for (i = 2; i <= cObj; i++) if (!ignore[i])
      for (j = 1; j < i; j++) if (!ignore[j])
        if (k = grid->n[j][i]) {
          ip = i <= oNorm ? rObjInf[i] : 2.5;
          jp = j <= oNorm ? rObjInf[j] : 2.5;
          p = (int)(rAspInf[k]*(ip+jp)/2.0*
            (1.0-RAbs((real)(grid->v[j][i]))/60.0/rAspOrb[k])*1000.0);
          if ((p < pcut || (p == pcut && (i > icut ||
            (i == icut && j > jcut)))) && p > phi) {
            ihi = i; jhi = j; phi = p; ahi = k;
          }
        }
    if (phi < 0)    /* Exit when no less powerful aspect found. */
      break;
    pcut = phi; icut = ihi; jcut = jhi;
    count++;                               /* Display the current aspect.   */
#ifdef INTERPRET
    if (us.fInterpret) {                   /* Interpret it if -I in effect. */
      InterpretAspect(jhi, ihi);
      continue;
    }
#endif
    rPowSum += (real)phi/1000.0;
    ca[ahi]++;
    co[jhi]++; co[ihi]++;
    sprintf(sz, "%3d: ", count); PrintSz(sz);
    PrintAspect(jhi, SFromZ(planet[jhi]), (int)RSgn(ret[jhi]), ahi,
      ihi, SFromZ(planet[ihi]), (int)RSgn(ret[ihi]), 'a');
    k = grid->v[jhi][ihi];
    AnsiColor(k < 0 ? kWhite : kLtGray);
    sprintf(sz, " - orb: %c%d%c%02d'",
      us.fAppSep ? (k < 0 ? 'a' : 's') : (k < 0 ? '-' : '+'),
      abs(k)/60, chDeg1, abs(k)%60); PrintSz(sz);
    AnsiColor(kDkGreen);
    sprintf(sz, " - power:%6.2f\n", (real)phi/1000.0); PrintSz(sz);
    AnsiColor(kDefault);
  }

  /* Now, if the -a0 switch is set, display summary information, the total */
  /* number of aspects of each type, and the # of aspects to each object.  */

  if (!us.fAspSummary) {
    PrintL();
    return;
  }
  AnsiColor(kDkGreen);
  sprintf(sz, "\t\t\t\t\t\t  Sum power:%6.2f\n", rPowSum); PrintSz(sz);
  PrintL();
#ifdef notdef
  k = us.fParallel ? Min(us.nAsp, aOpp) : us.nAsp;
  for (j = 0, i = 1; i <= k; i++) if (!ignorea(i)) {
    if (!(j & 7)) {
      if (j)
        PrintL();
    } else
      PrintSz("   ");
    AnsiColor(kAspA[i]);
    sprintf(sz, "%s:%3d", szAspectAbbrev[i], ca[i]); PrintSz(sz);
    j++;
  }
  PrintL();
  for (j = 0, i = 1; i <= cObj; i++) if (!ignore[i]) {
    if (!(j & 7)) {
      if (j)
        PrintL();
    } else
      PrintSz("   ");
    AnsiColor(kObjA[i]);
    sprintf(sz, "%c%c%c:%3d", chObj3(i), co[i]); PrintSz(sz);
    j++;
  }
  PrintL();
#endif
}


/* Display locations of all midpoints between objects in the chart, */
/* one per line, in sorted zodiac order from zero Aries onward, as  */
/* specified with the -m switch.                                    */

void ChartMidpoint()
{
  int cs[cSign + 1];
  char sz[cchSzDef];
  int mcut = -1, icut, jcut, mlo, ilo, jlo, m, i, j, count = 0;
  long lSpanSum = 0;

  ClearB((lpbyte)cs, (cSign + 1)*(int)sizeof(int));
  is.fSeconds = fFalse;
  loop {
    mlo = 21600;

    /* Search for the next closest midpoint farther down in the zodiac. */

    for (i = 1; i < cObj; i++) if (!ignore[i])
      for (j = i+1; j <= cObj; j++) if (!ignore[j]) {
        m = (grid->n[j][i]-1)*30*60 + grid->v[j][i];
        if ((m > mcut || (m == mcut && (i > icut ||
          (i == icut && j > jcut)))) && m < mlo) {
          ilo = i; jlo = j; mlo = m;
        }
      }
    if (mlo >= 21600)    /* Exit when no midpoint farther in zodiac found. */
      break;
    mcut = mlo; icut = ilo; jcut = jlo;
    count++;                               /* Display the current midpoint. */
#ifdef INTERPRET
    if (us.fInterpret) {                   /* Interpret it if -I in effect. */
      InterpretMidpoint(ilo, jlo);
      continue;
    }
#endif
    cs[mlo/60/30+1]++;
    sprintf(sz, "%4d: ", count); PrintSz(sz);
    PrintZodiac((real)mlo/60.0);
    PrintCh(' ');
    PrintAspect(ilo, SFromZ(planet[ilo]), (int)RSgn(ret[ilo]), 0,
      jlo, SFromZ(planet[jlo]), (int)RSgn(ret[jlo]), 'm');
    AnsiColor(kDefault);
    m = (int)(MinDistance(planet[ilo], planet[jlo])*60.0);
    lSpanSum += m;
    sprintf(sz, "-%4d%c%02d' degree span.\n", m/60, chDeg1, m%60);
    PrintSz(sz);
  }
  is.fSeconds = us.fSeconds;

  /* Now, if the -m0 switch is set, display summary information as well, */
  /* including the total number of midpoints in each sign.               */

  if (!us.fMidSummary)
    return;
  PrintL();
  m = (int)(lSpanSum/count);
  sprintf(sz, "Average span:%4d%c%02d'\n", m/60, chDeg1, m%60); PrintSz(sz);
  for (i = 1; i <= cSign; i++) {
    if (i == sLib)
      PrintL();
    else if (i != sAri)
      PrintSz("   ");
    AnsiColor(kSignA(i));
    sprintf(sz, "%c%c%c:%3d", chSig3(i), cs[i]); PrintSz(sz);
  }
  PrintL();
}


/* Display locations of the objects on the screen with respect to the local */
/* horizon, as specified with the -Z switch.                                */

void ChartHorizon()
{
  char sz[cchSzDef], szFormat[cchSzDef];
  real lon, lat, sx, sy, vx, vy,
    lonz[objMax], latz[objMax], azi[objMax], alt[objMax];
  int fPrime, i, j, k, tot;

  /* Set up some initial variables. */

  fPrime = us.fPrimeVert;
  lon = RFromD(Mod(Lon)); lat = RFromD(Lat);
  tot = us.nStar ? cObj : oNorm;

  /* First find zenith location on Earth of each object. */

  for (i = 1; i <= tot; i++) if (!ignore[i] || i == oMC) {
    lonz[i] = RFromD(Tropical(planet[i])); latz[i] = RFromD(planetalt[i]);
    EclToEqu(&lonz[i], &latz[i]);
  }

  /* Then, convert this to local horizon altitude and azimuth. */

  for (i = 1; i <= tot; i++) if (!ignore[i] && i != oMC) {
    lonz[i] = RFromD(Mod(DFromR(lonz[oMC]-lonz[i]+lon)));
    lonz[i] = RFromD(Mod(DFromR(lonz[i]-lon+rPiHalf)));
    EquToLocal(&lonz[i], &latz[i], rPiHalf-lat);
    azi[i] = rDegMax-DFromR(lonz[i]); alt[i] = DFromR(latz[i]);
  }

  /* If the -Z0 switch flag is in effect, convert from altitude/azimuth  */
  /* coordinates to prime vertical coordinates that we'll print instead. */

  if (fPrime) {
    for (i = 1; i <= tot; i++) if (!ignore[i]) {
      azi[i] = RFromD(azi[i]); alt[i] = RFromD(alt[i]);
      CoorXform(&azi[i], &alt[i], rPiHalf);
      azi[i] = DFromR(azi[i]); alt[i] = DFromR(alt[i]);
    }
  }

  /* Now, actually print the location of each object. */

  sprintf(szFormat, is.fSeconds ? " " : "");
  sprintf(sz, "Body %s%sAltitude%s %s%sAzimuth%s%s  Azi. Vector%s    ",
    szFormat, szFormat, szFormat, szFormat, szFormat, szFormat, szFormat,
    szFormat); PrintSz(sz);
  sprintf(sz, "%s Vector%s%s    Moon Vector\n\n",
    us.objCenter ? "Sun" : "Earth", szFormat, szFormat); PrintSz(sz);
  for (k = 1; k <= tot; k++) {
    i = k <= oNorm ? k : oNorm+starname[k-oNorm];
    if (ignore[i] || !FThing(i))
      continue;
    AnsiColor(kObjA[i]);
    sprintf(sz, "%-4.4s: ", szObjName[i]); PrintSz(sz);
    PrintAltitude(alt[i]);

    /* Determine directional vector based on azimuth. */

    sprintf(sz, " %s", SzDegree(azi[i])); PrintSz(sz);
    sx = RCos(RFromD(azi[i])); sy = RSin(RFromD(azi[i]));
    if (RAbs(sx) < RAbs(sy)) {
      vx = RAbs(sx / sy); vy = 1.0;
    } else {
      vy = RAbs(sy / sx); vx = 1.0;
    }
    sprintf(sz, is.fSeconds ? " (%.3f%c" : " (%.2f%c", vy,
      sy < 0.0 ? (fPrime ? 'u' : 's') : (fPrime ? 'd' : 'n')); PrintSz(sz);
    sprintf(sz, is.fSeconds ? " %.2f%c)" : " %.2f%c)", vx,
      sx > 0.0 ? 'e' : 'w'); PrintSz(sz);

    /* Determine distance vector of current object from Sun and Moon. */

    vx = azi[1]-azi[i]; vy = azi[2]-azi[i];
    j = 1 + is.fSeconds;
    sprintf(szFormat, " [%%%d.%df%%%d.%df] [%%%d.%df%%%d.%df]",
      j+5, j, j+5, j, j+5, j, j+5, j);
    sprintf(sz, szFormat,
      RAbs(vx) < rDegHalf ? vx : RSgn(vx)*(rDegMax-RAbs(vx)), alt[1]-alt[i],
      RAbs(vy) < rDegHalf ? vy : RSgn(vy)*(rDegMax-RAbs(vy)), alt[2]-alt[i]);
    PrintSz(sz);
    if (!is.fSeconds && i >= uranLo) {
      if (i <= uranHi)
        sprintf(sz, "  Uranian #%d", i-uranLo+1);
      else
        sprintf(sz, "  Star #%2d", i-starLo+1);
      PrintSz(sz);
    }
    PrintL();
  }
  AnsiColor(kDefault);
}


/* Display x,y,z locations of each body (in AU) with respect to the Sun */
/* (or whatever the specified center planet is), as in the -S switch.   */
/* These values were already determined when calculating the planet     */
/* positions themselves, so this procedure is basically just a loop.    */

void ChartOrbit()
{
  char sz[cchSzDef], szFormat[cchSzDef];
  real x, y, z;
  int i;

  sprintf(szFormat, is.fSeconds ? " " : "");
  sprintf(sz, "Body%s    Angle%s%s%s%s    ",
    szFormat, szFormat, szFormat, szFormat, szFormat);
  PrintSz(sz);
  sprintf(sz,
    "%sX axis%s%s%s    %sY axis%s%s%s    %sZ axis%s%s%s    %sLength\n",
    szFormat, szFormat, szFormat, szFormat, szFormat, szFormat, szFormat,
    szFormat, szFormat, szFormat, szFormat, szFormat, szFormat);
  PrintSz(sz);
  for (i = 0; i <= oNorm; i++) {
    if (ignore[i] || (!FThing(i) ||
      ((i == oMoo || i == oNod || i == oSou) && !us.fPlacalc)))
      continue;
    AnsiColor(kObjA[i]);
    sprintf(sz, "%c%c%c%c: ", chObj3(i),
      szObjName[i][3] ? szObjName[i][3] : ' '); PrintSz(sz);
    x = spacex[i]; y = spacey[i]; z = spacez[i];
    sprintf(sz, is.fSeconds ? "[%11.7f] [%11.7f] [%11.7f] [%11.7f] [%11.7f]" :
      "[%7.3f] [%7.3f] [%7.3f] [%7.3f] [%7.3f]",
      planet[i], x, y, z, RSqr(x*x+y*y+z*z)); PrintSz(sz);
    if (!is.fSeconds && i >= uranLo) {
      sprintf(sz, "  Uranian #%d", i-uranLo+1); PrintSz(sz);
    }
    PrintL();
  }
  AnsiColor(kDefault);
}


/* Print the locations of the astro-graph lines on the Earth as specified */
/* with the -L switch. This includes Midheaven and Nadir lines, zenith    */
/* positions, and locations of Ascendant and Descendant lines.            */

void ChartAstroGraph()
{
  CrossInfo FPTR *c;
  char sz[cchSzDef];
  real planet1[objMax], planet2[objMax], mc[objMax], ic[objMax],
    as[objMax], ds[objMax], as1[objMax], ds1[objMax],
    lo = Lon, longm, w, x, y, z, ad, oa, am, od, dm;
  int cCross = 0, tot = cObj, i, j, k, l, m, n;

  if (us.fLatitudeCross)
    {
    if ((c = (CrossInfo FPTR *)
      PAllocate(sizeof(CrossInfo), fFalse, "crossing table")) == NULL)
      return;
    }

#ifdef MATRIX
  for (i = 1; i <= cObj; i++) if (!ignore[i]) {
    planet1[i] = RFromD(Tropical(planet[i]));
    planet2[i] = RFromD(planetalt[i]);     /* Calculate zenith location on */
    EclToEqu(&planet1[i], &planet2[i]);    /* Earth of each object.        */
  }

  /* Print header. */

  PrintSz("Object :");
  for (j = 0, i = 1; i <= cObj; i++)
    if (!ignore[i] && FThing(i)) {
      AnsiColor(kObjA[i]);
      sprintf(sz, " %c%c%c", chObj3(i)); PrintSz(sz);
      j++;
    }
  AnsiColor(kDefault);
  PrintSz("\n------ :");
  for (i = 1; i <= tot; i++)
    if (!ignore[i] && FThing(i))
      PrintSz(" ###");

  /* Print the longitude locations of the Midheaven lines. */

  PrintSz("\nMidheav: ");
  if (lo < 0.0)
    lo += rDegMax;
  for (i = 1; i <= tot; i++)
    if (!ignore[i] && FThing(i)) {
    AnsiColor(kObjA[i]);
    x = RFromD(MC)-planet1[i];
    if (x < 0.0)
      x += 2.0*rPi;
    if (x > rPi)
      x -= 2.0*rPi;
    z = lo+DFromR(x);
    if (z > rDegHalf)
      z -= rDegMax;
    mc[i] = z;
    sprintf(sz, "%3.0f%c", RAbs(z), z < 0.0 ? 'e' : 'w'); PrintSz(sz);
  }
  AnsiColor(kDefault);

  /* The Nadir lines are just always 180 degrees away from the Midheaven. */

  PrintSz("\nNadir  : ");
  for (i = 1; i <= tot; i++)
    if (!ignore[i] && FThing(i)) {
    AnsiColor(kObjA[i]);
    z = mc[i] + rDegHalf;
    if (z > rDegHalf)
      z -= rDegMax;
    ic[i] = z;
    sprintf(sz, "%3.0f%c", RAbs(z), z < 0.0 ? 'e' : 'w'); PrintSz(sz);
  }
  AnsiColor(kDefault);

  /* Print the Zenith latitude locations. */

  PrintSz("\nZenith : ");
  for (i = 1; i <= tot; i++)
    if (!ignore[i] && FThing(i)) {
      AnsiColor(kObjA[i]);
      y = DFromR(planet2[i]);
      sprintf(sz, "%3.0f%c", RAbs(y), y < 0.0 ? 's' : 'n'); PrintSz(sz);
      as[i] = ds[i] = as1[i] = ds1[i] = rLarge;
    }
  PrintL2();

  /* Now print the locations of Ascendant and Descendant lines. Since these */
  /* are curvy, we loop through the latitudes, and for each object at each  */
  /* latitude, print the longitude location of the line in question.        */

  longm = RFromD(Mod(MC+lo));
  for (j = 80; j >= -80; j -= us.nAstroGraphStep) {
    AnsiColor(kDefault);
    sprintf(sz, "Asc@%2d%c: ", j >= 0 ? j : -j, j < 0 ? 's' : 'n');
    PrintSz(sz);
    for (i = 1; i <= tot; i++)
      if (!ignore[i] && FThing(i)) {
      AnsiColor(kObjA[i]);
      ad = RTan(planet2[i])*RTan(RFromD(j));
      if (ad*ad > 1.0) {
        PrintSz(" -- ");
        as1[i] = ds1[i] = cp2.dir[i] = rLarge;
      } else {
        ad = RAsin(ad);
        oa = planet1[i]-ad;
        if (oa < 0.0)
          oa += 2.0*rPi;
        am = oa-rPiHalf;
        if (am < 0.0)
          am += 2.0*rPi;
        z = longm-am;
        if (z < 0.0)
          z += 2.0*rPi;
        if (z > rPi)
          z -= 2.0*rPi;
        as1[i] = as[i];
        as[i] = z = DFromR(z);
        cp2.dir[i] = ad;
        sprintf(sz, "%3.0f%c", RAbs(z), z < 0.0 ? 'e' : 'w'); PrintSz(sz);
      }
    }

    /* Again, the Descendant position is related to the Ascendant's,  */
    /* being a mirror image, so it can be calculated somewhat easier. */

    AnsiColor(kDefault);
    sprintf(sz, "\nDsc@%2d%c: ", j >= 0 ? j : -j, j < 0 ? 's' : 'n');
    PrintSz(sz);
    for (i = 1; i <= tot; i++)
      if (!ignore[i] && FThing(i)) {
      AnsiColor(kObjA[i]);
      ad = cp2.dir[i];
      if (ad == rLarge)
        PrintSz(" -- ");
      else {
        od = planet1[i]+ad;
        dm = od+rPiHalf;
        z = longm-dm;
        if (z < 0.0)
          z += 2.0*rPi;
        if (z > rPi)
          z -= 2.0*rPi;
        ds1[i] = ds[i];
        ds[i] = z = DFromR(z);
        sprintf(sz, "%3.0f%c", RAbs(z), z < 0.0 ? 'e' : 'w'); PrintSz(sz);
      }
    }
    PrintL();
#endif /* MATRIX */

    /* Now, if the -L0 switch is in effect, then take these line positions, */
    /* which we saved in an array above as we were printing them, and       */
    /* calculate and print the latitude crossings.                          */

    if (us.fLatitudeCross)
      for (l = 1; l <= cObj; l++) if (!ignore[l] && FThing(l))
        for (k = 1; k <= cObj; k++) {
          if (ignore[k] || !FThing(k))
            continue;
          for (n = 0; n <= 1; n++) {
            x = n ? ds1[l] : as1[l];
            y = n ? ds[l] : as[l];
            for (m = 0; m <= 1; m++) {

            /* Check if Ascendant/Descendant cross Midheaven/Nadir. */

            z = m ? ic[k] : mc[k];
            if (cCross < MAXCROSS &&
              RAbs(x-y) < rDegHalf && RSgn(z-x) != RSgn(z-y)) {
              c->obj1[cCross] = n ? -l : l;
              c->obj2[cCross] = m ? -k : k;
              c->lat[cCross] = (real)j+5.0*RAbs(z-y)/RAbs(x-y);
              c->lon[cCross] = z;
              cCross++;
            }

            /* Check if Ascendant/Descendant cross another Asc/Des. */

            w = m ? ds1[k] : as1[k];
            z = m ? ds[k] : as[k];
            if (cCross < MAXCROSS && k > l &&
                RAbs(x-y)+RAbs(w-z) < rDegHalf && RSgn(w-x) != RSgn(z-y)) {
              c->obj1[cCross] = n ? -l : l;
              c->obj2[cCross] = 100+(m ? -k : k);
              c->lat[cCross] = (real)j+5.0*
                RAbs(y-z)/(RAbs(x-w)+RAbs(y-z));
              c->lon[cCross] = Min(x, y)+RAbs(x-y)*
                RAbs(y-z)/(RAbs(x-w)+RAbs(y-z));
              cCross++;
            }
          }
        }
    }
  }
  if (!us.fLatitudeCross)
    return;
  PrintL();

  /* Now, print out all the latitude crossings we found.  */
  /* First, we sort them in order of decreasing latitude. */

  for (i = 1; i < cCross; i++) {
    j = i-1;
    while (j >= 0 && c->lat[j] < c->lat[j+1]) {
      SwapN(c->obj1[j], c->obj1[j+1]); SwapN(c->obj2[j], c->obj2[j+1]);
      SwapR(&c->lat[j], &c->lat[j+1]); SwapR(&c->lon[j], &c->lon[j+1]);
      j--;
    }
  }
  for (i = 1; i < cCross; i++) {
    j = abs(c->obj1[i]);
    AnsiColor(kObjA[j]);
    sprintf(sz, "%c%c%c ", chObj3(j)); PrintSz(sz);
    AnsiColor(kElemA[c->obj1[i] > 0 ? eFir : eAir]);
    PrintSz(c->obj1[i] > 0 ? "Ascendant " : "Descendant");
    AnsiColor(kWhite);
    PrintSz(" crosses ");
    j = abs(c->obj2[i] - (c->obj2[i] < 50 ? 0 : 100));
    AnsiColor(kObjA[j]);
    sprintf(sz, "%c%c%c ", chObj3(j)); PrintSz(sz);
    AnsiColor(kElemA[c->obj2[i] < 50 ?
      (c->obj2[i] > 0 ? eEar : eWat) : (c->obj2[i] > 100 ? eFir : eAir)]);
    sprintf(sz, "%s ", c->obj2[i] < 50 ? (c->obj2[i] > 0 ? "Midheaven " :
      "Nadir     ") : (c->obj2[i] > 100 ? "Ascendant " : "Descendant"));
    PrintSz(sz);
    AnsiColor(kDefault);
    sprintf(sz, "at %s%c,", SzDegree(c->lon[i]),
      c->lon[i] < 0.0 ? 'E' : 'W'); PrintSz(sz);
    j = (int)(RFract(RAbs(c->lat[i]))*60.0);
    sprintf(sz, "%s%c\n", SzDegree(c->lat[i]),
      c->lat[i] < 0.0 ? 'S' : 'N'); PrintSz(sz);
  }
  DeallocateFar(c);
  if (!cCross) {
    AnsiColor(kDefault);
    PrintSz("No latitude crossings.\n");
  }
}

/* Another important procedure: Display any of the types of (text) charts    */
/* that the user specified they wanted, by calling the appropriate routines. */

void PrintChart(fProg)
bool fProg;
{
  int fCall = fFalse;

#ifdef LOGAN
  /*
   * Average velocity got from the web site
   * http://nssdc.gsfc.nasa.gov/planetary/factsheet/planet_table_ratio.html
   */

  lret[1] = 1; /* 360/365.25 - Sun */
  lret[2] = 13.176339; /* Moon */
  lret[3] = 1.61; /* Merc */
  lret[4] = 1.18; /* Venus */
  lret[5] = 0.5240327; /* 360/686.98 - Mars */
  lret[6] = 0.0830911; /* 360/4332.589 - Jupiter */
  lret[7] = 0.0334596; /* 360/10759.22 - Saturn */
  lret[16] = 0.0531; /* Rahu */
  lret[17] = 0.0531; /* Ketu */
#endif /* LOGAN */

  if (us.fListing) {
    if (is.fMult)
      PrintL2();
    if (us.nRel < rcDifference)
      ChartListing();
    else

      /* If the -rb or -rd relationship charts are in effect, then instead  */
      /* of doing the standard -v chart, print either of these chart types. */

      DisplayRelation();
    is.fMult = fTrue;
  }
  if (us.fWheel) {
    if (is.fMult)
      PrintL2();
    ChartWheel();
    is.fMult = fTrue;
  }
  if (us.fGrid) {
    if (is.fMult)
      PrintL2();
    if (us.nRel > rcDual) {
      fCall = us.fSmartCusp; us.fSmartCusp = fFalse;
      if (!FCreateGrid(fFalse))
        return;
      us.fSmartCusp = fCall;
      not(fCall);
      ChartGrid();
      if (us.fGridConfig) {    /* If -g0 switch in effect, then  */
        PrintL();              /* display aspect configurations. */
        if (!fCall)
          FCreateGrid(fFalse);
        DisplayGrands();
      }
    } else {

      /* Do a relationship aspect grid between two charts if -r0 in effect. */

      fCall = us.fSmartCusp; us.fSmartCusp = fFalse;
      if (!FCreateGridRelation(us.fGridConfig))
        return;
      us.fSmartCusp = fCall;
      ChartGridRelation();
    }
    is.fMult = fTrue;
  }
  if (us.fAspList) {
    if (is.fMult)
      PrintL2();
    if (us.nRel > rcDual) {
      if (!fCall) {
        fCall = fTrue;
        if (!FCreateGrid(fFalse))
          return;
      }
      ChartAspect();
    } else {
      if (!FCreateGridRelation(fFalse))
        return;
      ChartAspectRelation();
    }
    is.fMult = fTrue;
  }
  if (us.fMidpoint) {
    if (is.fMult)
      PrintL2();
    if (us.nRel > rcDual) {
      if (!fCall) {
        if (!FCreateGrid(fFalse))
          return;
      }
      ChartMidpoint();
    } else {
      if (!FCreateGridRelation(fTrue))
        return;
      ChartMidpointRelation();
    }
    is.fMult = fTrue;
  }
  if (us.fHorizon) {
    if (is.fMult)
      PrintL2();
    if (us.fHorizonSearch)
      ChartInDayHorizon();
    else
      ChartHorizon();
    is.fMult = fTrue;
  }
  if (us.fOrbit) {
    if (is.fMult)
      PrintL2();
    ChartOrbit();
    is.fMult = fTrue;
  }
  if (us.fInfluence) {
    if (is.fMult)
      PrintL2();
    ChartInfluence();
    is.fMult = fTrue;
  }
  if (us.fAstroGraph) {
    if (is.fMult)
      PrintL2();
    ChartAstroGraph();
    is.fMult = fTrue;
  }
  if (us.fCalendar) {
    if (is.fMult)
      PrintL2();
    if (us.fCalendarYear)
      ChartCalendarYear();
    else
      ChartCalendarMonth();
    is.fMult = fTrue;
  }
  if (us.fInDay) {
    if (is.fMult)
      PrintL2();
    ChartInDaySearch(fProg);
    is.fMult = fTrue;
  }
  if (us.fInDayInf) {
    if (is.fMult)
      PrintL2();
    ChartInDayInfluence();
    is.fMult = fTrue;
  }
  if (us.fEphemeris) {
    if (is.fMult)
      PrintL2();
    ChartEphemeris();
    is.fMult = fTrue;
  }
  if (us.fTransit) {
    if (is.fMult)
      PrintL2();
    ChartTransitSearch(fProg);
    is.fMult = fTrue;
  }
  if (us.fTransitInf) {
    if (is.fMult)
      PrintL2();
    ChartTransitInfluence(fProg);
    is.fMult = fTrue;
  }
#ifdef ARABIC
  if (us.nArabic) {
    if (is.fMult)
      PrintL2();
    DisplayArabic();
    is.fMult = fTrue;
  }
#endif

  if (!is.fMult) {          /* Assume the -v chart if user */
    us.fListing = fTrue;    /* didn't indicate anything.   */
    PrintChart(fProg);
    is.fMult = fTrue;
  }
}

/* charts1.c */
