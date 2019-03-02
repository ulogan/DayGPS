/*
** Astrolog (Version 5.05) File: charts3.c
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

#define LOGAN

#include "astrolog.h"


#ifdef LOGAN
#include "astrosee.h"
char *sign[] = { "Ari", "Tau", "Gem", "Can",
                 "Leo", "Vir", "Lib", "Sco",
                 "Sag", "Cap", "Aqu", "Pis" };

char *splanet[] = { "Sun", "Moo", "Mer", "Ven", "Mar",
                    "Jup", "Sat", "Rah", "Ket", "Lag", "Lagc", "Lags" };

tplnData pln[DAYS_IN_MONTH][MINS_IN_DAY][eMaxPla];

void assignPln(tplnData *plan, double pdeg, double ret)
{
    double deg;
    
    deg = Mod(pdeg + rRound/60.0/60.0);
    plan->deg = deg;

    plan->ret = ret;

    plan->rasi = deg / 30.0;
    plan->nak = deg / (360 / 27.0);
    plan->amsa = (int)(deg / (360 / 108.0)) % 12;
}
#endif /* LOGAN */

bool VedicAspect(obj1, asp, obj2)
int obj1, asp, obj2;
{
  if (asp == 1 || asp == 2) {
    if (obj2 <= obj1)
      return fFalse;
  }
  if (obj1 == obj2) 
    return fFalse;
  if ((obj1 == 16 && obj2 == 17) ||
     (obj1 == 17 && obj2 == 16))
    return fFalse;
  switch(obj1) 
  {
    case 1: /* Sun */
    case 2: /* Moon */
    case 3: /* Mercury */
    case 4: /* Venus */
    case 16: /* Rahu */
    case 17: /* Ketu */  /* 1, 7 */
      if (asp == 1 || asp == 2)
        return fTrue;
      break;
    case 5: /* Mars 1, 7, 4, 8 */
      if (asp == 1 || asp == 2 || asp == 3 || asp == 6)
        return fTrue;
      break;
    case 6: /* Jupiter 1, 7, 5, 9 */
      if (asp == 1 || asp == 2 || asp == 4 || asp == 7)
        return fTrue;
      break;
    case 7: /* Sat 1, 7, 3, 10 */
      if (asp == 1 || asp == 2 || asp == 5 || asp == 8)
        return fTrue;
      break;
  }
  return fFalse; /* Asc is 21 */

}

bool StockAspect(obj1, asp, obj2)
int obj1, asp, obj2;
{
  if (asp == 1 || asp == 2) {
    if (obj2 <= obj1)
      return fFalse;
  }
  if (obj1 == obj2) 
    return fFalse;
  if ((obj1 == 16 && obj2 == 17) ||
     (obj1 == 17 && obj2 == 16))
    return fFalse;
  switch(obj1) 
  {
    case 1: /* Sun */
    case 3: /* Mercury */
    case 4: /* Venus */
    case 16: /* Rahu */
    case 17: /* Ketu */ 
      if (asp == 1 || asp == 2)
        return fTrue;
      break;
    case 2: /* Moon */
    case 21: /* Lagna */ 
      /* 1, 7, 5 */
      if (asp == 1 || asp == 2 || asp == 4)
        return fTrue;
      break;
    case 5: /* Mars 1, 7, 4, 8 */
      if (asp == 1 || asp == 2 || asp == 3 || asp == 6)
        return fTrue;
      break;
    case 6: /* Jupiter 1, 7, 5, 9 */
      if (asp == 1 || asp == 2 || asp == 4 || asp == 7)
        return fTrue;
      break;
    case 7: /* Sat 1, 7, 3, 10 */
      if (asp == 1 || asp == 2 || asp == 5 || asp == 8)
        return fTrue;
      break;
  }
  return fFalse; /* Asc is 21 */

}

bool Aspect(obj1, asp, obj2)
int obj1, asp, obj2;
{
    extern int stockAspect;
    if (stockAspect) {
        return(StockAspect(obj1, asp, obj2));
    }

    return(VedicAspect(obj1, asp, obj2));
}

#ifdef LOGAN

int SzLoganZodiac(deg, d, m)
real deg;
int *d;
int *m;
{
  int sign;
  real s;

  /* format the position in degrees/minutes format: and return sign as integer */

  deg = Mod(deg + (is.fSeconds ? rRound/60.0/60.0 : rRound/60.0));
  sign = (int)(deg / 30.0);
  *d = (int)deg - sign*30;
  *m = (int)(RFract(deg)*60.0);

  return sign + 1;
}

int getNaksatra(rdeg)
real rdeg;
{
    int actualmin;
    int rasi;
    int deg;
    int min;
    int i;
    extern int nakdeg[];

    rasi = SzLoganZodiac(rdeg, &deg, &min);

    actualmin = ((deg * 60) + ((rasi - 1) * 30 * 60)) + min;

    for (i = 0; i < 28; i++) {
        if ((actualmin >= nakdeg[i])  && (actualmin < nakdeg[i + 1])) {
            return(i);
        }
    }

    return(0);

}

int getTithi(sdeg, mdeg)
real sdeg;
real mdeg;
{
    int ti = 1;
    float ddeg;

    if (sdeg < mdeg) {
        sdeg += 360.0;
    }

    ddeg = sdeg - mdeg;

    ddeg = 360 - ddeg;

    ti = (int)(ddeg / 12);
    ti++;

    /*
    if (ti >= 15) {
        ti = ti - 15;
    }
    */

    /*
    printf("sdeg %f mdeg %f ddeg %f ti %d\n",
            sdeg, mdeg, ddeg, ti);
            */

    return (ti);
}

int getYogas(sdeg, mdeg)
real sdeg;
real mdeg;
{
    int smin;
    int mmin;
    int rasi;
    int deg;
    int min;
    int i;
    int summin;
    extern int nakdeg[];

    rasi = SzLoganZodiac(sdeg, &deg, &min);
    smin = ((deg * 60) + ((rasi - 1) * 30 * 60)) + min;

    rasi = SzLoganZodiac(mdeg, &deg, &min);
    mmin = ((deg * 60) + ((rasi - 1) * 30 * 60)) + min;

    summin = smin + mmin;

    if (summin > 21600) {
        summin -= 21600;
    }

    for (i = 0; i < 28; i++) {
        if ((summin >= nakdeg[i])  && (summin < nakdeg[i + 1])) {
            return(i);
        }
    }

    return(0);
}

int getNavamsa(rdeg)
real rdeg;
{
  int actualmin;
  int padam = 0;
  int offset = 0;
  int rasi;
  int deg;
  int min;

  rasi = SzLoganZodiac(rdeg, &deg, &min);

  actualmin = deg * 60 + min;

  switch(rasi)
  {
    case 1:
      if (actualmin >= 0 && actualmin < 800) { 
        padam = (actualmin / 200) + 1;
        offset = 0;
      } else if (actualmin >= 800 && actualmin < 1600) {
        padam = ((actualmin - 800) / 200) + 1;
        offset = 4;
      } else if (actualmin >= 1600 && actualmin < 1800) {
        padam = ((actualmin - 1600) / 200) + 1;
        offset = 8;
      }
      break;
    case 2:
      if (actualmin >= 0 && actualmin < 600) {
        padam = (actualmin / 200) + 2;
        offset = 8;
      } else if (actualmin >= 600 && actualmin < 1400) {
        padam = ((actualmin - 600) / 200) + 1;
        offset = 0;
      } else if (actualmin >= 1400 && actualmin < 1800) {
        padam = ((actualmin - 1400) / 200) + 1;
        offset = 4;
      }
      break;
    case 3:
      if (actualmin >= 0 && actualmin < 400) { 
        padam = (actualmin / 200) + 3;
        offset = 4;
      } else if (actualmin >= 400 && actualmin < 1200) {
        padam = ((actualmin - 400) / 200) + 1;
        offset = 8;
      } else if (actualmin >= 1200 && actualmin < 1800) {
        padam = ((actualmin - 1200) / 200) + 1;
        offset = 0;
      }
      break;
    case 4:
      if (actualmin >= 0 && actualmin < 200) { 
        padam = (actualmin / 200) + 4;
        offset = 0;
      } else if (actualmin >= 200 && actualmin < 1000) {
        padam = ((actualmin - 200) / 200) + 1;
        offset = 4;
      } else if (actualmin >= 1000 && actualmin < 1800) {
        padam = ((actualmin - 1000) / 200) + 1;
        offset = 8;
      }
      break;
    case 5:
      if (actualmin >= 0 && actualmin < 800) { 
        padam = (actualmin / 200) + 1;
        offset = 0;
      } else if (actualmin >= 800 && actualmin < 1600) {
        padam = ((actualmin - 800) / 200) + 1;
        offset = 4;
      }
      else if (actualmin >= 1600 && actualmin < 1800) {
        padam = ((actualmin - 1600) / 200) + 1;
        offset = 8;
      }
      break;
    case 6:
      if (actualmin >= 0 && actualmin < 600) {
        padam = (actualmin / 200) + 2;
        offset = 8;
      } else if (actualmin >= 600 && actualmin < 1400) {
        padam = ((actualmin - 600) / 200) + 1;
        offset = 0;
      } else if (actualmin >= 1400 && actualmin < 1800) {
        padam = ((actualmin - 1400) / 200) + 1;
        offset = 4;
      }
      break;
    case 7:
      if (actualmin >= 0 && actualmin < 400) { 
        padam = (actualmin / 200) + 3;
        offset = 4;
      } else if (actualmin >= 400 && actualmin < 1200) {
        padam = ((actualmin - 400) / 200) + 1;
        offset = 8;
      } else if (actualmin >= 1200 && actualmin < 1800) {
        padam = ((actualmin - 1200) / 200) + 1;
        offset = 0;
      }
      break;
    case 8:
      if (actualmin >= 0 && actualmin < 200) { 
        padam = (actualmin / 200) + 4;
        offset = 0;
      } else if (actualmin >= 200 && actualmin < 1000) {
        padam = ((actualmin - 200) / 200) + 1;
        offset = 4;
      } else if (actualmin >= 1000 && actualmin < 1800) {
        padam = ((actualmin - 1000) / 200) + 1;
        offset = 8;
      }
      break;
    case 9:
      if (actualmin >= 0 && actualmin < 800) { 
        padam = (actualmin / 200) + 1;
        offset = 0;
      } else if (actualmin >= 800 && actualmin < 1600) {
        padam = ((actualmin - 800) / 200) + 1;
        offset = 4;
      } else if (actualmin >= 1600 && actualmin < 1800) {
        padam = ((actualmin - 1600) / 200) + 1;
        offset = 8;
      }
      break;
    case 10:
      if (actualmin >= 0 && actualmin < 600) {
        padam = (actualmin / 200) + 2;
        offset = 8;
      } else if (actualmin >= 600 && actualmin < 1400) {
        padam = ((actualmin - 600) / 200) + 1;
        offset = 0;
      } else if (actualmin >= 1400 && actualmin < 1800) {
        padam = ((actualmin - 1400) / 200) + 1;
        offset = 4;
      }
      break;
    case 11:
      if (actualmin >= 0 && actualmin < 400) { 
        padam = (actualmin / 200) + 3;
        offset = 4;
      } else if (actualmin >= 400 && actualmin < 1200) {
        padam = ((actualmin - 400) / 200) + 1;
        offset = 8;
      } else if (actualmin >= 1200 && actualmin < 1800) {
        padam = ((actualmin - 1200) / 200) + 1;
        offset = 0;
      }
      break;
    case 12:
      if (actualmin >= 0 && actualmin < 200) { 
        padam = (actualmin / 200) + 4;
        offset = 0;
      } else if (actualmin >= 200 && actualmin < 1000) {
        padam = ((actualmin - 200) / 200) + 1;
        offset = 4;
      } else if (actualmin >= 1000 && actualmin < 1800) {
        padam = ((actualmin - 1000) / 200) + 1;
        offset = 8;
      }
      break;
    default:
      printf("Error\n");
      break;
  }

  // Return the house of navamsa
  return(padam + offset);
}

extern int csvdeg;
extern int navasp;
extern int naksatra;
extern int autodst;
extern int tithi;
extern int yoga;
extern int sunraise;
extern int predictor;
extern int yoga;

int
findDst(int curmon, int curday, int curyear, int curtime)
{
    int dstFlag = 0;

    if (curyear <= 2006) {
        if (curmon >= 5 && curmon <= 9) {
            dstFlag = 1;
        } else if (curmon == 4) { /* April */
            int dayofweek = DayOfWeek(4, 1, curyear);
            int firstsunday;

            switch (dayofweek) {
            case 0:
                firstsunday = 1;
                break;
            case 1:
                firstsunday = 7;
                break;
            case 2:
                firstsunday = 6;
                break;
            case 3:
                firstsunday = 5;
                break;
            case 4:
                firstsunday = 4;
                break;
            case 5:
                firstsunday = 3;
                break;
            case 6:
                firstsunday = 2;
                break;
            default:
                break;
            }
            if (curday > firstsunday) {
                dstFlag = 1;
            } else if (curday == firstsunday) {
                if (curtime >= 2) {
                    dstFlag = 1;
                }
            }
        } else if (curmon == 10) { /* October */
            int dayofweek = DayOfWeek(10, 31, curyear);
            int lastsunday;

            switch (dayofweek) {
            case 0:
                lastsunday = 31;
                break;
            case 1:
                lastsunday = 31 - 1;
                break;
            case 2:
                lastsunday = 31 - 2;
                break;
            case 3:
                lastsunday = 31 - 3;
                break;
            case 4:
                lastsunday = 31 - 4;
                break;
            case 5:
                lastsunday = 31 - 5;
                break;
            case 6:
                lastsunday = 31 - 6;
                break;
            default:
                break;
            }

            if (curday < lastsunday) {
                dstFlag = 1;
            } else if (curday == lastsunday) {
                if (curtime < 1) {
                    dstFlag = 1;
                }
            }

        }

    } else { // >= 2007
        if (curmon >= 4 && curmon <= 10) {
            dstFlag = 1;
        } else if (curmon == 3) { /* March */
            int dayofweek = DayOfWeek(3, 1, curyear);
            int secondsunday;

            switch (dayofweek) {
            case 0:
                secondsunday = 1 + 7;
                break;
            case 1:
                secondsunday = 7 + 7;
                break;
            case 2:
                secondsunday = 6 + 7;
                break;
            case 3:
                secondsunday = 5 + 7;
                break;
            case 4:
                secondsunday = 4 + 7;
                break;
            case 5:
                secondsunday = 3 + 7;
                break;
            case 6:
                secondsunday = 2 + 7;
                break;
            default:
                break;
            }

            if (curday > secondsunday) {
                dstFlag = 1;
            } else if (curday == secondsunday) {
                if (curtime >= 2) {
                    dstFlag = 1;
                }
            }

        } else if (curmon == 11) { /* November */
            int dayofweek = DayOfWeek(11, 1, curyear);
            int firstsunday;

            switch (dayofweek) {
            case 0:
                firstsunday = 1;
                break;
            case 1:
                firstsunday = 7;
                break;
            case 2:
                firstsunday = 6;
                break;
            case 3:
                firstsunday = 5;
                break;
            case 4:
                firstsunday = 4;
                break;
            case 5:
                firstsunday = 3;
                break;
            case 6:
                firstsunday = 2;
                break;
            default:
                break;
            }

            if (curday < firstsunday) {
                dstFlag = 1;
            } else if (curday == firstsunday) {
                if (curtime < 1) {
                    dstFlag = 1;
                }
            }

        }

    }

    if (dstFlag) {
        return 1;
    }
    
    return 0;
}

#endif /* LOGAN */

/*
******************************************************************************
** Multiple Chart Scanning Routines.
******************************************************************************
*/

/* Search through a day, and print out the times of exact aspects among the  */
/* planets during that day, as specified with the -d switch, as well as the  */
/* times when a planet changes sign or direction. To do this, we cast charts */
/* for the beginning and end of the day, or a part of a day, and do a linear */
/* equation check to see if anything exciting happens during the interval.   */
/* (This is probably the single most complicated procedure in the program.)  */

void ChartInDaySearch(fProg)
bool fProg;
{
  char sz[cchSzDef];
  char tch[10];
  int source[MAXINDAY], aspect[MAXINDAY], dest[MAXINDAY],
    sign1[MAXINDAY], sign2[MAXINDAY];
  int D1, D2, occurcount, division, div,
    fYear, yea0, yea1, yea2, i, j, k, l, s1, s2;
  real time[MAXINDAY], divsiz, d1, d2, e1, e2, f1, f2, g;
  extern real lret[objMax];
  CI ciT;
#ifdef LOGAN
  int dmin;
#endif /* LOGAN */

  /* If parameter 'fProg' is set, look for changes in a progressed chart. */

  ciT = ciTwin;
  fYear = us.fInDayMonth && (Mon2 == 0);
  division = (fYear || fProg) ? 1 : us.nDivision;
  divsiz = 24.0 / (real)division*60.0;

  /* If -dY in effect, then search through a range of years. */

  yea1 = fProg ? Yea2 : Yea;
  yea2 = fYear ? (yea1 + us.nEphemYears - 1) : yea1;
  for (yea0 = yea1; yea0 <= yea2; yea0++) {

  /* If -dm in effect, then search through the whole month, day by day. */

  if (us.fInDayMonth) {
    D1 = 1;
    if (fYear) {
      Mon2 = 1; D2 = DayInYearHi(yea0);
    } else
      D2 = DayInMonth(fProg ? Mon2 : Mon, yea0);
  } else
    D1 = D2 = Day;

  /* Start searching the day or days in question for exciting stuff. */

  for (Day2 = D1; Day2 <= D2; Day2 = AddDay(Mon, Day2, yea0, 1)) {
    occurcount = 0;

#ifdef LOGAN
    if (0) {
        printf("Mon %d Day2 %d\n", fYear ? Mon2 : Mon, Day2);
    }
    if (autodst) {
        Dst = findDst(fYear ? Mon2 : Mon, Day2, yea0, 0);
    }
#endif /* LOGAN */

    /* Cast chart for beginning of day and store it for future use. */

    SetCI(ciCore, fYear ? Mon2 : Mon, Day2, yea0, 0.0, Dst, Zon, Lon, Lat);
    if (us.fProgress = fProg) {
      is.JDp = MdytszToJulian(Mon2, DD, yea0, 0.0, Dst, Zon);
      ciCore = ciMain;
    }
    CastChart(fTrue);
    for (i = 1; i <= cSign; i++) {
      cp2.cusp[i] = house[i];
      cp2.house[i] = inhouse[i];
    }
#ifdef LOGAN
    /* Get Ketu's position hard coded -Logan */
    planet[17] = Mod(planet[16] + 180);
    ret[17] = ret[16];
#endif /* LOGAN */
    for (i = 1; i <= cObj; i++) {
      cp2.obj[i] = planet[i];
#ifdef LOGAN
      if (csvdeg) {
          // Nothing to do
      } else if (naksatra) {
        if (i != oMoo) {
            continue;
        }
        cp2.navamsa[i] = getNaksatra(planet[i]);
      } else if (tithi) {
        if (i != oMoo) {
            continue;
        }
        cp2.navamsa[i] = getTithi(planet[oSun], planet[oMoo]);
      } else if (yoga) {
        if (i != oMoo) {
            continue;
        }
        cp2.navamsa[i] = getYogas(planet[oSun], planet[oMoo]);
      } else if (navasp) {
        cp2.navamsa[i] = getNavamsa(planet[i]);
      }
#endif /* LOGAN */
      cp2.dir[i] = ret[i];
    }

    /* Now divide the day into segments and search each segment in turn. */
    /* More segments is slower, but has slightly better time accuracy.   */

    for (div = 0; div <= division; div++) {
        real curTime = DegToDec(24.0*(real)div/(real)division);

      /* Cast the chart for the ending time of the present segment. The   */
      /* beginning time chart is copied from the previous end time chart. */

#ifdef LOGAN
      dmin = div;
      if (autodst) {
        Dst = findDst(fYear ? Mon2 : Mon, Day2, yea0, (int)curTime);
      }
#endif /* LOGAN */

      SetCI(ciCore, fYear ? Mon2 : Mon, Day2, yea0, curTime, Dst, Zon, Lon, Lat);
      if (fProg) {
        is.JDp = MdytszToJulian(Mon2, DD+1, yea0, 0.0, Dst, Zon);
        ciCore = ciMain;
      }
      CastChart(fTrue);
      for (i = 1; i <= cSign; i++) {
        cp1.cusp[i] = cp2.cusp[i]; cp1.house[i] = cp2.house[i];
        cp2.cusp[i] = house[i];  cp2.house[i] = inhouse[i];
      }
#ifdef LOGAN
      /* Get Ketu's position hard coded -Logan */
      planet[17] = Mod(planet[16] + 180);
      ret[17] = ret[16];
      if (csvdeg) {
          // ascendant (lagna)
          assignPln(&pln[Day2][dmin][eLag], planet[21], ret[21]);
      }
#endif /* LOGAN */
      for (i = 1; i <= cObj; i++) {
        cp1.obj[i] = cp2.obj[i]; cp1.dir[i] = cp2.dir[i];
        cp2.obj[i] = planet[i];  cp2.dir[i] = ret[i];
#ifdef LOGAN
        cp1.navamsa[i] = cp2.navamsa[i];
        if (csvdeg) {
            if (ignore[i]) {
                continue;
            }
            if (i == 16) {
                assignPln(&pln[Day2][dmin][eRah], planet[16], ret[16]);
            } else if (i == 17) {
                assignPln(&pln[Day2][dmin][eKet], planet[17], ret[17]);
            } else if ((i >= 1) && (i <= 7)) {
                assignPln(&pln[Day2][dmin][i - 1], planet[i], ret[i]);
            }
        } else if (naksatra) {
          if (i != oMoo) {
              continue;
          }
          cp2.navamsa[i] = getNaksatra(planet[i]);
        } else if (tithi) {
            if (i != oMoo) {
                continue;
            }
            cp2.navamsa[i] = getTithi(planet[oSun], planet[oMoo]);
        } else if (yoga) {
            if (i != oMoo) {
                continue;
            }
            cp2.navamsa[i] = getYogas(planet[oSun], planet[oMoo]);
        } else if (navasp) {
            cp2.navamsa[i] = getNavamsa(planet[i]);
        }
#endif /* LOGAN */
      }

#ifdef LOGAN
      if (csvdeg) {
          if ((Lon == 74.00) && (Lat == 40.42)) { /* New York */
              // Get Asc for other 2 lagna
              extern real getAsc(long, int);
              struct tm ltm;
              long lclock;

              memset(&ltm, 0, sizeof(struct tm));
              ltm.tm_min = dmin % 60;
              ltm.tm_hour = dmin / 60;
              ltm.tm_mday = Day2;
              ltm.tm_mon = Mon - 1;
              ltm.tm_year = yea0 - 1900;
              ltm.tm_isdst = SS;

              // printf("MM %3d DD %3d YY %6d Time %d\n", Mon, Day2, yea0, dmin);

              lclock = mktime(&ltm);

              // Asc 2 is Chicago
              assignPln(&pln[Day2][dmin][eLagc], getAsc(lclock, -1), 0);

              // Asc 3 is Sunnyvale
              assignPln(&pln[Day2][dmin][eLags], getAsc(lclock, -3), 0);
          }

          // Nothing more to do
          continue;
      }
#endif /* LOGAN */

      /* Now search through the present segment for anything exciting. */

      for (i = 1; i <= cObj; i++) if (!ignore[i] && (fProg || FThing(i))) {
        s1 = SFromZ(cp1.obj[i])-1;
        s2 = SFromZ(cp2.obj[i])-1;

#ifdef LOGAN
    // Check for the sign change in Navamsa
        if (navasp && !ignore[i] && cp1.navamsa[i] != cp2.navamsa[i] && 
            !ignore[0]) {
          source[occurcount] = i;
          aspect[occurcount] = -11; // Indicate that it is Navamsa
          dest[occurcount] = cp2.navamsa[i];
          time[occurcount] = (24.0*(real)div/(real)division) * 60.0;
          sign1[occurcount] = sign2[occurcount] = cp1.navamsa[i];
          occurcount++;
        }

        if (naksatra && !ignore[i] && cp1.navamsa[i] != cp2.navamsa[i] && 
            !ignore[0]) {
          source[occurcount] = i;
          aspect[occurcount] = -13; // Indicate that it is Nakshatra
          dest[occurcount] = cp2.navamsa[i];
          time[occurcount] = (24.0*(real)div/(real)division) * 60.0;
          sign1[occurcount] = sign2[occurcount] = cp1.navamsa[i];
          occurcount++;
        }

        if (tithi && !ignore[i] && cp1.navamsa[i] != cp2.navamsa[i] && 
            !ignore[0]) {
          source[occurcount] = i;
          aspect[occurcount] = -15; // Indicate Tithi change
          dest[occurcount] = cp2.navamsa[i];
          time[occurcount] = (24.0*(real)div/(real)division) * 60.0;
          sign1[occurcount] = sign2[occurcount] = cp1.navamsa[i];
          occurcount++;
        }

        if (yoga && !ignore[i] && cp1.navamsa[i] != cp2.navamsa[i] && 
            !ignore[0]) {
          source[occurcount] = i;
          aspect[occurcount] = -17; // Indicate yoga change
          dest[occurcount] = cp2.navamsa[i];
          time[occurcount] = (24.0*(real)div/(real)division) * 60.0;
          sign1[occurcount] = sign2[occurcount] = cp1.navamsa[i];
          occurcount++;
        }

        if (tithi) {
            continue;
        }

#endif /* LOGAN */

        /* Does the current planet change into the next or previous sign? */

        if (naksatra == 0 && !ignore[i] && s1 != s2 && !ignore[0]) {
          source[occurcount] = i;
          aspect[occurcount] = aSig;
          dest[occurcount] = s2+1;
          time[occurcount] = MinDistance(cp1.obj[i],
            (real)(cp1.dir[i] >= 0.0 ? s2 : s1) * 30.0) /
            MinDistance(cp1.obj[i], cp2.obj[i])*divsiz + (real)(div-1)*divsiz;
          sign1[occurcount] = sign2[occurcount] = s1+1;
          occurcount++;
        }

        /* Does the current planet go retrograde or direct? */

        if (!ignore[i] && (cp1.dir[i] < 0.0) != (cp2.dir[i] < 0.0) &&
          !ignore2[0]) {
          source[occurcount] = i;
          aspect[occurcount] = aDir;
          dest[occurcount] = cp2.dir[i] < 0.0;
          time[occurcount] = RAbs(cp1.dir[i])/(RAbs(cp1.dir[i])+
            RAbs(cp2.dir[i]))*divsiz + (real)(div-1)*divsiz;
          sign1[occurcount] = sign2[occurcount] = s1+1;
          occurcount++;
        }

        /* Does the current planet go progression or direct? */

        if ((i != oAsc) && ((DFromR(fabs(cp1.dir[i])) / lret[i]) >= 2.0) != 
            ((DFromR(fabs(cp2.dir[i])) / lret[i]) >= 2.0)) {
          source[occurcount] = i;
          aspect[occurcount] = -9;
          dest[occurcount] = (DFromR(fabs(cp2.dir[i])) / lret[i]) >= 2.0;
          time[occurcount] = RAbs(cp1.dir[i])/(RAbs(cp1.dir[i])+
            RAbs(cp2.dir[i]))*divsiz + (real)(div-1)*divsiz;
          sign1[occurcount] = sign2[occurcount] = s1+1;
          occurcount++;
        }

        /* Now search for anything making an aspect to the current planet. */

        for (j = /*i+*/1; j <= cObj; j++) if (!ignore[j] && (fProg || FThing(j)))
          for (k = 1; k <= us.nAsp; k++) {

            if (Aspect(i, k, j)) {
              d1 = cp1.obj[i]; d2 = cp2.obj[i];
              e1 = cp1.obj[j]; e2 = cp2.obj[j];

#ifndef LOGAN
              if (MinDistance(d1, d2) < MinDistance(e1, e2)) {
                SwapR(&d1, &e1);
                SwapR(&d2, &e2);
              }
#endif /* LOGAN */

              /* We are searching each aspect in turn. Let's subtract the  */
              /* size of the aspect from the angular difference, so we can */
              /* then treat it like a conjunction.                         */

#ifdef LOGAN
              d1 = Mod(d1+rAspAngle[k]);
              d2 = Mod(d2+rAspAngle[k]);
#else
              if (MinDistance(e1, Mod(d1-rAspAngle[k])) <
                  MinDistance(e2, Mod(d2+rAspAngle[k]))) {
                e1 = Mod(e1+rAspAngle[k]);
                e2 = Mod(e2+rAspAngle[k]);
              } else {
                e1 = Mod(e1-rAspAngle[k]);
                e2 = Mod(e2-rAspAngle[k]);
              }
#endif

              /* Check to see if the aspect actually occurs during our    */
              /* segment, making sure we take into account if one or both */
              /* planets are retrograde or if they cross the Aries point. */

              f1 = e1-d1;
              if (RAbs(f1) > rDegHalf)
                f1 -= RSgn(f1)*rDegMax;
              f2 = e2-d2;
              if (RAbs(f2) > rDegHalf)
                f2 -= RSgn(f2)*rDegMax;
              if (MinDistance(Midpoint(d1, d2), Midpoint(e1, e2)) < rDegQuad &&
                RSgn(f1) != RSgn(f2)) {
                source[occurcount] = i;
                aspect[occurcount] = k;
                dest[occurcount] = j;

                /* Horray! The aspect occurs sometime during the interval.   */
                /* Now we just have to solve an equation in two variables to */
                /* find out where the "lines" cross, i.e. the aspect's time. */
  
                f1 = d2-d1;
                if (RAbs(f1) > rDegHalf)
                  f1 -= RSgn(f1)*rDegMax;
                f2 = e2-e1;
                if (RAbs(f2) > rDegHalf)
                  f2 -= RSgn(f2)*rDegMax;
                g = (RAbs(d1-e1) > rDegHalf ?
                  (d1-e1)-RSgn(d1-e1)*rDegMax : d1-e1)/(f2-f1);
                time[occurcount] = g*divsiz + (real)(div-1)*divsiz;
                sign1[occurcount] = (int)(Mod(cp1.obj[i]+
                  RSgn(cp2.obj[i]-cp1.obj[i])*
                  (RAbs(cp2.obj[i]-cp1.obj[i]) > rDegHalf ? -1 : 1)*
                  RAbs(g)*MinDistance(cp1.obj[i], cp2.obj[i]))/30.0)+1;
                sign2[occurcount] = (int)(Mod(cp1.obj[j]+
                  RSgn(cp2.obj[j]-cp1.obj[j])*
                  (RAbs(cp2.obj[j]-cp1.obj[j]) > rDegHalf ? -1 : 1)*
                  RAbs(g)*MinDistance(cp1.obj[j], cp2.obj[j]))/30.0)+1;
                occurcount++;
              }
            }
          }
      }
    }

    /* After all the aspects, etc, in the day have been located, sort   */
    /* them by time at which they occur, so we can print them in order. */

    /*
     * Astrosee Web site has a bug with the swap routine -Logan
    for (i = 1; i < occurcount; i++) {
      j = i-1;
      while (j >= 0 && time[j] > time[j+1]) {
        SwapN(source[j], source[j+1]);
        SwapN(aspect[j], aspect[j+1]);
        SwapN(dest[j], dest[j+1]);
        SwapR(&time[j], &time[j+1]);
        SwapN(sign1[j], sign1[j+1]); SwapN(sign2[j], sign2[j+1]);
        j--;
      }
    }
    */

    /* Finally, loop through and display each aspect and when it occurs. */

    for (i = 0; i < occurcount; i++) {
      int lt;
      int printFlag = 0;
      s1 = (int)time[i]/60;
      s2 = (int)time[i]-s1*60;
      lt = ((s1 * 60) + s2);
      j = Day2;
      if (fYear || fProg) {
        l = Mon2;
        while (j > (k = DayInMonth(l, yea0))) {
          j -= k;
          l++;
        }
      }
#ifdef LOGAN

      // If -4 argument is passed then print only
      // Navamsa changes
      if (navasp && (aspect[i] != -11)) {
          continue;
      }

      // If -% argument is passed then print only
      // Yoga changes
      if (yoga && (aspect[i] != -17)) {
          continue;
      }

#define SIX30    ((6 * 60) + 30)
#define ONEPM    (13 * 60)
#define EIGHT30    ((8 * 60) + 30)
#define THREEPM    (15 * 60)
#define NINE30    ((9 * 60) + 30)
#define FOURPM    (16 * 60)
      sprintf(tch, "(%c)", ciMain.loc[0]); 
      printFlag = 0;

      // If -8 argument is pased then print only
      // Full Moon/New Moon aspects
      if (tithi) {
          if ((aspect[i] == aCon) || (aspect[i] == aOpp) ||
                  (aspect[i] == -15)) {
              printFlag = 1;
          } else {
              continue;
          }
      }

      if (us.fEuroTime == 0) {  /* if Yt is not set print everything */
        printFlag = 1;
      }
      else 
      if ((Lon == 122.00) && (Lat == 37.23)) { /* Sunnyvale */
        if ((lt >= SIX30) && (lt <= ONEPM)) {
          printFlag = 1;
        }
      }
      else
      if ((Lon == 87.39) && (Lat == 41.51)) { /* Chicago */
        if ((lt >= EIGHT30) && (lt <= THREEPM)) {
          printFlag = 1;
        }
      }
      else 
      if ((Lon == 74.00) && (Lat == 40.42)) { /* New York */
        if ((lt >= NINE30) && (lt <= FOURPM)) {
          printFlag = 1;
        }
      }
      else {
        printFlag = 1;
      }

      if ((Lon == 74.00) && (Lat == 40.42)) { /* New York */
          s1 = s1 - 3;
      } else if ((Lon == 87.39) && (Lat == 41.51)) { /* Chicago */
          s1 = s1 - 2;
      }

      if (printFlag) {
        int dayofweek = DayOfWeek(Mon, Day2, Yea);

        if (naksatra || tithi || yoga || sunraise) {
          if (sunraise) {
              if (dest[i] == 21) {
                sprintf(sz, "%c%c%c %2d/%2d/%2d ", chDay3(dayofweek), Mon, Day2, Yea); PrintSz(sz);
              }
          } else {
              sprintf(sz, "%c%c%c %2d/%2d/%2d ", chDay3(dayofweek), Mon, Day2, Yea); PrintSz(sz);
          }
        } else {
          if ((predictor) || us.fInDayMonth) {
            sprintf(sz, "%c%c%c %2d/%2d/%2d ", chDay3(dayofweek), Mon, Day2, Yea); PrintSz(sz);
          }
          sprintf(sz, "%2d:%02d %s ", s1, s2, tch); PrintSz(sz);
        }
        PrintAspect(source[i], sign1[i],
          (int)RSgn(cp1.dir[source[i]])+(int)RSgn(cp2.dir[source[i]]),
          aspect[i], dest[i], sign2[i],
          (int)RSgn(cp1.dir[dest[i]])+(int)RSgn(cp2.dir[dest[i]]),
          (char)(fProg ? 'e' : 'd'));
        if (naksatra || tithi || yoga || sunraise) {
          if (sunraise) {
              if (dest[i] == 21) {
                sprintf(sz, "%2d:%02d %s\n", s1 > 12 ? s1 - 12 : s1, s2, s1 >= 12 ? "PM" : "AM"); PrintSz(sz);
              }
          } else {
              sprintf(sz, "%2d:%02d %s\n", s1 > 12 ? s1 - 12 : s1, s2, s1 >= 12 ? "PM" : "AM"); PrintSz(sz);
          }
        } else {
            PrintInDay(source[i], aspect[i], dest[i]);
        }
      } else {
        if ((Lon == 122.00) && (Lat == 37.23)) { /* Sunnyvale */
          if (source[i] != 21 && dest[i] != 21) {
            // sprintf(sz, "%s ", SzDate(fYear || fProg ? l : Mon, j, yea0, fFalse)); PrintSz(sz);
            sprintf(sz, "%2d:%02d %s ", s1, s2, tch); PrintSz(sz);
            PrintAspect(source[i], sign1[i],
              (int)RSgn(cp1.dir[source[i]])+(int)RSgn(cp2.dir[source[i]]),
              aspect[i], dest[i], sign2[i],
              (int)RSgn(cp1.dir[dest[i]])+(int)RSgn(cp2.dir[dest[i]]),
              (char)(fProg ? 'e' : 'd'));
            PrintInDay(source[i], aspect[i], dest[i]);
          }
        }
      }

#endif /* LOGAN */
    }
  }
  }

  /* Recompute original chart placements as we've overwritten them. */

  ciCore = ciMain; ciTwin = ciT;
  CastChart(fTrue);
}


/* Search through a month, year, or years, and print out the times of exact */
/* transits where planets in the time frame make aspect to the planets in   */
/* some other chart, as specified with the -t switch. To do this, we cast   */
/* charts for the start and end of each month, or within a month, and do an */
/* equation check for aspects to the other base chart during the interval.  */

void ChartTransitSearch(fProg)
bool fProg;
{
  real planet3[objMax], house3[cSign+1], ret3[objMax], time[MAXINDAY];
  char sz[cchSzDef];
  int source[MAXINDAY], aspect[MAXINDAY], dest[MAXINDAY], sign[MAXINDAY],
    isret[MAXINDAY], M1, M2, Y1, Y2, occurcount, div, i, j, k, s1, s2, s3;
  real divsiz, daysiz, d, e1, e2, f1, f2;
  CI ciT;

  ciT = ciTwin;
  for (i = 1; i <= cSign; i++)
    house3[i] = house[i];
  for (i = 1; i <= cObj; i++) {
    planet3[i] = planet[i];
    ret3[i] = ret[i];
  }

  /* Hacks: Searching month number zero means to search the whole year    */
  /* instead, month by month. Searching a negative month means to search  */
  /* multiple years, with the span of the year stored in the "day" field. */

  Y1 = Y2 = Yea2;
  M1 = M2 = Mon2;
  if (Mon2 < 1) {
    M1 = 1; M2 = 12;
    if (Mon2 < 0) {
      if (Day2 < 1) {
        Y1 = Yea2 + Day2 + 1; Y2 = Yea2;
      } else {
        Y1 = Yea2; Y2 = Yea2 + Day2 - 1;
      }
    }
  }

  /* Start searching the year or years in question for any transits. */

  for (Yea2 = Y1; Yea2 <= Y2; Yea2++)

  /* Start searching the month or months in question for any transits. */

  for (Mon2 = M1; Mon2 <= M2; Mon2++) {
    daysiz = (real)DayInMonth(Mon2, Yea2)*24.0*60.0;
    divsiz = daysiz / (real)us.nDivision;

    /* Cast chart for beginning of month and store it for future use. */

    SetCI(ciCore, Mon2, 1, Yea2, 0.0, Dst2, Zon2, Lon2, Lat2);
    if (us.fProgress = fProg) {
      is.JDp = MdytszToJulian(MM, DD, YY, 0.0, Dst2, Zon2);
      ciCore = ciMain;
    }
    for (i = 1; i <= oNorm; i++)
      SwapN(ignore[i], ignore2[i]);
    CastChart(fTrue);
    for (i = 1; i <= oNorm; i++)
      SwapN(ignore[i], ignore2[i]);
    for (i = 1; i <= cSign; i++)
      cp2.cusp[i] = house[i];
    for (i = 1; i <= oCore; i++) {
      cp2.obj[i] = planet[i];
      cp2.dir[i] = ret[i];
    }

    /* Divide our month into segments and then search each segment in turn. */

    for (div = 1; div <= us.nDivision; div++) {
      occurcount = 0;

      /* Cast the chart for the ending time of the present segment, and */
      /* copy the start time chart from the previous end time chart.    */

      d = 1.0 + (daysiz/24.0/60.0)*(real)div/(real)us.nDivision;
      SetCI(ciCore, Mon2, (int)d, Yea2,
        DegToDec(RFract(d)*24.0), Dst2, Zon2, Lon2, Lat2);
      if (fProg) {
        is.JDp = MdytszToJulian(MM, DD, YY, 0.0, Dst2, Zon2);
        ciCore = ciMain;
      }
      for (i = 1; i <= oNorm; i++)
        SwapN(ignore[i], ignore2[i]);
      CastChart(fTrue);
      for (i = 1; i <= oNorm; i++)
        SwapN(ignore[i], ignore2[i]);
      for (i = 1; i <= cSign; i++) {
        cp1.cusp[i] = cp2.cusp[i]; cp2.cusp[i] = house[i];
      }
      for (i = 1; i <= oCore; i++) {
        cp1.obj[i] = cp2.obj[i]; cp1.dir[i] = cp2.dir[i];
        cp2.obj[i] = planet[i];  cp2.dir[i] = ret[i];
      }

      /* Now search through the present segment for any transits. Note that */
      /* stars can be transited, but they can't make transits themselves.   */

      for (i = 1; i <= cObj; i++) if (!ignore[i])
        for (j = 1; j <= oNorm; j++) if (!ignore2[j] && (fProg || FThing(j)))

          /* Between each pair of planets, check if they make any aspects. */

          for (k = 1; k <= us.nAsp; k++) if (FAcceptAspect(i, k, j)) {
            d = planet3[i]; e1 = cp1.obj[j]; e2 = cp2.obj[j];
            if (MinDistance(e1, Mod(d-rAspAngle[k])) <
                MinDistance(e2, Mod(d+rAspAngle[k]))) {
              e1 = Mod(e1+rAspAngle[k]);
              e2 = Mod(e2+rAspAngle[k]);
            } else {
              e1 = Mod(e1-rAspAngle[k]);
              e2 = Mod(e2-rAspAngle[k]);
            }

            /* Check to see if the present aspect actually occurs during the */
            /* segment, making sure we check any Aries point crossings.      */

            f1 = e1-d;
            if (RAbs(f1) > rDegHalf)
              f1 -= RSgn(f1)*rDegMax;
            f2 = e2-d;
            if (RAbs(f2) > rDegHalf)
              f2 -= RSgn(f2)*rDegMax;
            if (MinDistance(d, Midpoint(e1, e2)) < rDegQuad &&
              RSgn(f1) != RSgn(f2) && occurcount < MAXINDAY) {

              /* Ok, we have found a transit. Now determine the time */
              /* and save this transit in our list to be printed.    */

              source[occurcount] = j;
              aspect[occurcount] = k;
              dest[occurcount] = i;
              time[occurcount] = RAbs(f1)/(RAbs(f1)+RAbs(f2))*divsiz +
                (real)(div-1)*divsiz;
              sign[occurcount] = (int)(Mod(
                MinDistance(cp1.obj[j], Mod(d-rAspAngle[k])) <
                MinDistance(cp2.obj[j], Mod(d+rAspAngle[k])) ?
                d-rAspAngle[k] : d+rAspAngle[k])/30.0)+1;
              isret[occurcount] = (int)RSgn(cp1.dir[j]) +
                (int)RSgn(cp2.dir[j]);
              occurcount++;
            }
          }

      /* After all transits located, sort them by time at which they occur. */

      for (i = 1; i < occurcount; i++) {
        j = i-1;
        while (j >= 0 && time[j] > time[j+1]) {
          SwapN(source[j], source[j+1]);
          SwapN(aspect[j], aspect[j+1]);
          SwapN(dest[j], dest[j+1]);
          SwapR(&time[j], &time[j+1]);
          SwapN(sign[j], sign[j+1]);
          SwapN(isret[j], isret[j+1]);
          j--;
        }
      }

      /* Now loop through list and display all the transits. */

      for (i = 0; i < occurcount; i++) {
        s1 = (_int)time[i]/24/60;
        s3 = (_int)time[i]-s1*24*60;
        s2 = s3/60;
        s3 = s3-s2*60;
        SetCI(ciSave, Mon2, s1+1, Yea2, DegToDec((real)
          ((_int)time[i]-s1*24*60) / 60.0), Dst2, Zon2, Lon2, Lat2);
        sprintf(sz, "%s %s ",
          SzDate(Mon2, s1+1, Yea2, fFalse), SzTime(s2, s3)); PrintSz(sz);
        PrintAspect(source[i], sign[i], isret[i], aspect[i],
          dest[i], SFromZ(planet3[dest[i]]), (int)RSgn(ret3[dest[i]]),
          (char)(fProg ? 'u' : 't'));

        /* Check for a Solar, Lunar, or any other return. */

        if (aspect[i] == aCon && source[i] == dest[i]) {
          AnsiColor(kWhite);
          sprintf(sz, " (%s Return)", source[i] == oSun ? "Solar" :
            (source[i] == oMoo ? "Lunar" : szObjName[source[i]]));
          PrintSz(sz);
        }
        PrintL();
#ifdef INTERPRET
        if (us.fInterpret)
          InterpretTransit(source[i], aspect[i], dest[i]);
#endif
        AnsiColor(kDefault);
      }
    }
  }

  /* Recompute original chart placements as we've overwritten them. */

  ciCore = ciMain; ciTwin = ciT;
  CastChart(fTrue);
}


/* Display a list of planetary rising times relative to the local horizon */
/* for the day indicated in the chart information, as specified with the  */
/* -Zd switch. For the day, the time each planet rises (transits horizon  */
/* in East half of sky), sets (transits horizon in West), reaches its     */
/* zenith point (transits meridian in South half of sky), and nadirs      */
/* transits meridian in North), is displayed.                             */

void ChartInDayHorizon()
{
  char sz[cchSzDef];
  int source[MAXINDAY], type[MAXINDAY], sign[MAXINDAY],
    fRet[MAXINDAY], occurcount, division, div, s1, s2, i, j, fT;
  real time[MAXINDAY], rgalt1[objMax], rgalt2[objMax], azialt[MAXINDAY],
    azi1, azi2, alt1, alt2, lon, lat, mc1, mc2, xA, yA, xV, yV, d, k;
  CI ciT;

  fT = us.fSidereal; us.fSidereal = fFalse;
  lon = RFromD(Mod(Lon)); lat = RFromD(Lat);
  division = us.nDivision * 4;
  occurcount = 0;

  ciT = ciTwin; ciCore = ciMain; ciCore.tim = 0.0;
  CastChart(fTrue);
  mc2 = RFromD(planet[oMC]); k = RFromD(planetalt[oMC]);
  EclToEqu(&mc2, &k);
  for (i = 1; i <= cSign; i++) {
    cp2.cusp[i] = house[i];
    cp2.house[i] = inhouse[i];
  }
  for (i = 1; i <= cObj; i++) {
    cp2.obj[i] = planet[i];
    rgalt2[i] = planetalt[i];
    cp2.dir[i] = ret[i];
  }

  /* Loop thorough the day, dividing it into a certain number of segments. */
  /* For each segment we get the planet positions at its endpoints.        */

  for (div = 1; div <= division; div++) {
    ciCore = ciMain; ciCore.tim = DegToDec(24.0*(real)div/(real)division);
    CastChart(fTrue);
    mc1 = mc2;
    mc2 = RFromD(planet[oMC]); k = RFromD(planetalt[oMC]);
    EclToEqu(&mc2, &k);
    for (i = 1; i <= cSign; i++) {
      cp1.cusp[i] = cp2.cusp[i]; cp1.house[i] = cp2.house[i];
      cp2.cusp[i] = house[i];    cp2.house[i] = inhouse[i];
    }
    for (i = 1; i <= cObj; i++) {
      cp1.obj[i] = cp2.obj[i]; cp2.obj[i] = planet[i];
      rgalt1[i] = rgalt2[i]; rgalt2[i] = planetalt[i];
      cp1.dir[i] = cp2.dir[i]; cp2.dir[i] = ret[i];
    }

    /* For our segment, check to see if each planet during it rises, sets, */
    /* reaches its zenith, or reaches its nadir.                           */

    for (i = 1; i <= cObj; i++) if (!ignore[i] && FThing(i)) {
      EclToHorizon(&azi1, &alt1, cp1.obj[i], rgalt1[i], lon, lat, mc1);
      EclToHorizon(&azi2, &alt2, cp2.obj[i], rgalt2[i], lon, lat, mc2);
      j = 0;

      /* Check for transits to the horizon. */
      if ((alt1 > 0.0) != (alt2 > 0.0)) {
        d = RAbs(alt1)/(RAbs(alt1)+RAbs(alt2));
        k = Mod(azi1 + d*MinDifference(azi1, azi2));
        j = 1 + 2*(MinDistance(k, rDegHalf) < rDegQuad);

      /* Check for transits to the meridian. */
      } else if (RSgn(MinDifference(azi1, rDegQuad)) !=
        RSgn(MinDifference(azi2, rDegQuad))) {
        j = 2 + 2*(MinDistance(azi1, rDegQuad) < rDegQuad);
        d = RAbs(azi1 - (j > 2 ? rDegQuad : 270.0))/MinDistance(azi1, azi2);
        k = alt1 + d*(alt2-alt1);
      }
      if (j && occurcount < MAXINDAY) {
        source[occurcount] = i;
        type[occurcount] = j;
        time[occurcount] = 24.0*((real)(div-1)+d)/(real)division*60.0;
        sign[occurcount] = (int)Mod(cp1.obj[i] +
          d*MinDifference(cp1.obj[i], cp2.obj[i]))/30 + 1;
        fRet[occurcount] = (int)RSgn(cp1.dir[i]) + (int)RSgn(cp2.dir[i]);
        azialt[occurcount] = k;
        ciSave = ciMain;
        ciSave.tim = DegToDec(time[occurcount] / 60.0);
        occurcount++;
      }
    }
  }

  /* Sort each event in order of time when it happens during the day. */

  for (i = 1; i < occurcount; i++) {
    j = i-1;
    while (j >= 0 && time[j] > time[j+1]) {
      SwapN(source[j], source[j+1]);
      SwapN(type[j], type[j+1]);
      SwapR(&time[j], &time[j+1]);
      SwapN(sign[j], sign[j+1]);
      SwapN(fRet[j], fRet[j+1]);
      SwapR(&azialt[j], &azialt[j+1]);
      j--;
    }
  }

  /* Finally display the list showing each event and when it occurs. */

  for (i = 0; i < occurcount; i++) {
    ciSave = ciMain;
    ciSave.tim = DegToDec(time[i] / 60.0);
    j = DayOfWeek(Mon, Day, Yea);
    AnsiColor(kRainbowA[j + 1]);
    sprintf(sz, "(%c%c%c) ", chDay3(j)); PrintSz(sz);
    AnsiColor(kDefault);
    s1 = (int)time[i]/60;
    s2 = (int)time[i]-s1*60;
    sprintf(sz, "%s %s ", SzDate(Mon, Day, Yea, fFalse), SzTime(s1, s2));
    PrintSz(sz);
    AnsiColor(kObjA[source[i]]);
    sprintf(sz, "%7.7s ", szObjName[source[i]]); PrintSz(sz);
    AnsiColor(kSignA(sign[i]));
    sprintf(sz, "%c%c%c%c%c ",
      fRet[i] > 0 ? '(' : (fRet[i] < 0 ? '[' : '<'), chSig3(sign[i]),
      fRet[i] > 0 ? ')' : (fRet[i] < 0 ? ']' : '>')); PrintSz(sz);
    AnsiColor(kElemA[type[i]-1]);
    if (type[i] == 1)
      PrintSz("rises  ");
    else if (type[i] == 2)
      PrintSz("zeniths");
    else if (type[i] == 3)
      PrintSz("sets   ");
    else
      PrintSz("nadirs ");
    AnsiColor(kDefault);
    PrintSz(" at ");
    if (type[i] & 1) {
      j = (int)(RFract(azialt[i])*60.0);
      sprintf(sz, "%3d%c%02d'", (int)azialt[i], chDeg1, j); PrintSz(sz);

      /* For rising and setting events, we'll also display a direction  */
      /* vector to make the 360 degree azimuth value thought of easier. */

      xA = RCosD(azialt[i]); yA = RSinD(azialt[i]);
      if (RAbs(xA) < RAbs(yA)) {
        xV = RAbs(xA / yA); yV = 1.0;
      } else {
        yV = RAbs(yA / xA); xV = 1.0;
      }
      sprintf(sz, " (%.2f%c %.2f%c)",
        yV, yA < 0.0 ? 's' : 'n', xV, xA > 0.0 ? 'e' : 'w'); PrintSz(sz);
    } else
      PrintAltitude(azialt[i]);
    PrintL();
  }

  /* Recompute original chart placements as we've overwritten them. */

  ciCore = ciMain; ciTwin = ciT;
  us.fSidereal = fT;
  CastChart(fTrue);
}


/* Print out an ephemeris - the positions of the planets (at the time in the */
/* current chart) each day during a specified month, as done with the -E     */
/* switch. Display the ephemeris for the whole year if -Ey is in effect.     */

void ChartEphemeris()
{
  char sz[cchSzDef];
  int yea, yea1, yea2, mon, mon1, mon2, daysiz, i, j, s, d, m;

  /* If -Ey is in effect, then loop through all months in the whole year. */

  if (us.nEphemYears) {
    yea1 = Yea; yea2 = yea1 + us.nEphemYears - 1; mon1 = 1; mon2 = 12;
  } else {
    yea1 = yea2 = Yea; mon1 = mon2 = Mon;
  }

  /* Loop through the year or years in question. */

  for (yea = yea1; yea <= yea2; yea++)

  /* Loop through the month or months in question, printing each ephemeris. */

  for (mon = mon1; mon <= mon2; mon++) {
    daysiz = DayInMonth(mon, yea);
    PrintSz(us.fEuroDate ? "Dy/Mo/Yr" : "Mo/Dy/Yr");
    for (j = 1; j <= cObj; j++) {
      if (!ignore[j] && FThing(j)) {
        sprintf(sz, "  %s%c%c%c%c%s ", is.fSeconds ? "  " : "", chObj3(j),
          szObjName[j][3] != 0 ? szObjName[j][3] : ' ',
          is.fSeconds ? "   " : ""); PrintSz(sz);
      }
    }
    PrintL();
    for (i = 1; i <= daysiz; i = AddDay(mon, i, yea, 1)) {

      /* Loop through each day in the month, casting a chart for that day. */

      SetCI(ciCore, mon, i, yea, Tim, Dst, Zon, Lon, Lat);
      CastChart(fTrue);
      PrintSz(SzDate(mon, i, yea, -1));
      PrintCh(' ');
      for (j = 1; j <= cObj; j++)
        if (!ignore[j] && FThing(j)) {
          if (is.fSeconds)
            PrintZodiac(planet[j]);
          else {
            AnsiColor(kObjA[j]);
            s = SFromZ(planet[j]);
            d = (int)planet[j] - (s-1)*30;
            m = (int)(RFract(planet[j])*60.0);
            sprintf(sz, "%2d%s%02d", d, szSignAbbrev[s], m); PrintSz(sz);
          }
          PrintCh((char)(ret[j] >= 0.0 ? ' ' : '.'));
        }
      PrintL();
      AnsiColor(kDefault);
    }
    if (mon < mon2 || yea < yea2)
      PrintL();
  }
}

/* charts3.c */
