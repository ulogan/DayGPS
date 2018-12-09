/*
** Astrolog (Version 5.05) File: calc.c
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


/*
******************************************************************************
** House Cusp Calculations.
******************************************************************************
*/

/* This is a subprocedure of ComputeInHouses(). Given a zodiac position,  */
/* return which of the twelve houses it falls in. Remember that a special */
/* check has to be done for the house that spans 0 degrees Aries.         */

int HousePlaceIn(rDeg)
real rDeg;
{
  int i = 0;

  rDeg = Mod(rDeg + 0.5/60.0/60.0);
  do {
    i++;
  } while (!(i >= cSign ||
      (rDeg >= house[i] && rDeg < house[Mod12(i+1)]) ||
      (house[i] > house[Mod12(i+1)] &&
      (rDeg >= house[i] || rDeg < house[Mod12(i+1)]))));
  return i;
}


/* For each object in the chart, determine what house it belongs in. */

void ComputeInHouses()
{
  int i;

  for (i = 1; i <= cObj; i++)
    inhouse[i] = HousePlaceIn(planet[i]);
}


/* This house system is just like the Equal system except that we start */
/* our 12 equal segments from the Midheaven instead of the Ascendant.   */

void HouseEqualMidheaven()
{
  int i;

  for (i = 1; i <= cSign; i++)
    house[i] = Mod(MC-270.0+30.0*(real)(i-1));
}


/* Compute the cusp positions using the Alcabitius house system. */

void HouseAlcabitius()
{
  real rDecl, rSda, rSna, r;
  int i;

  rDecl = RAsin(RSin(OB) * RSinD(Asc));
  r = -RTan(AA) * RTan(rDecl);
  rSda = DFromR(RAcos(r));
  rSna = rDegHalf - rSda;
  house[sLib] = DFromR(RA) - rSna;
  house[sSco] = DFromR(RA) - rSna*2.0/3.0;
  house[sSag] = DFromR(RA) - rSna/3.0;
  house[sCap] = DFromR(RA);
  house[sAqu] = DFromR(RA) + rSda/3.0;
  house[sPis] = DFromR(RA) + rSda*2.0/3.0;
  for (i = sLib; i <= sPis; i++)
    house[i] = Mod(house[i]+is.rSid);
  for (i = sAri; i <= sVir; i++)
    house[i] = Mod(house[i+6]+rDegHalf);
}


/* This is a new house system similar in philosophy to Porphyry houses.   */
/* Instead of just trisecting the difference in each quadrant, we do a    */
/* smooth sinusoidal distribution of the difference around all the cusps. */

void HousePorphyryNeo()
{
  real delta;
  int i;

  delta = (MinDistance(MC, Asc) - rDegQuad)/4.0;
  house[sLib] = Mod(Asc+rDegHalf); house[sCap] = MC;
  house[sAqu] = Mod(house[sCap] + 30.0 + delta   + is.rSid);
  house[sPis] = Mod(house[sAqu] + 30.0 + delta*2 + is.rSid);
  house[sSag] = Mod(house[sCap] - 30.0 + delta   + is.rSid);
  house[sSco] = Mod(house[sSag] - 30.0 + delta*2 + is.rSid);
  for (i = sAri; i < sLib; i++)
    house[i] = Mod(house[i+6]-rDegHalf);
}


/* The "Whole" house system is like the Equal system with 30 degree houses, */
/* where the 1st house starts at zero degrees of the sign of the Ascendant. */

void HouseWhole()
{
  int i;

  for (i = 1; i <= cSign; i++)
    house[i] = Mod((SFromZ(Asc)-1)*30+ZFromS(i));
}


/* In "null" houses, the cusps are always fixed to start at their cor-    */
/* responding sign, i.e. the 1st house is always at 0 degrees Aries, etc. */

void HouseNull()
{
  int i;

  for (i = 1; i <= cSign; i++)
    house[i] = Mod(ZFromS(i));
}


/* Calculate the house cusp positions, using the specified algorithm. */

void ComputeHouses(housesystem)
int housesystem;
{
  char sz[cchSzDef];

  if (RAbs(AA) > RFromD(rDegQuad-rAxis) && housesystem < 2) {
    sprintf(sz,
      "The %s system of houses is not defined at extreme latitudes.",
      szSystem[housesystem]);
    PrintError(sz);
    Terminate(tcFatal);
  }
  switch (housesystem) {
  case  1: HouseKoch();           break;
  case  2: HouseEqual();          break;
  case  3: HouseCampanus();       break;
  case  4: HouseMeridian();       break;
  case  5: HouseRegiomontanus();  break;
  case  6: HousePorphyry();       break;
  case  7: HouseMorinus();        break;
  case  8: HouseTopocentric();    break;
  case  9: HouseAlcabitius();     break;
  case 10: HouseEqualMidheaven(); break;
  case 11: HousePorphyryNeo();    break;
  case 12: HouseWhole();          break;
  case 13: HouseNull();           break;
  default: HousePlacidus();
  }
}


/*
******************************************************************************
** Star Position Calculations.
******************************************************************************
*/

/* This is used by the chart calculation routine to calculate the positions */
/* of the fixed stars. Since the stars don't move in the sky over time,     */
/* getting their positions is mostly just reading info from an array and    */
/* converting it to the correct reference frame. However, we have to add    */
/* in the correct precession for the tropical zodiac, and sort the final    */
/* index list based on what order the stars are supposed to be printed in.  */

void ComputeStars(SD)
real SD;
{
  int i, j;
  real x, y, z;

  /* Read in star positions. */

  for (i = 1; i <= cStar; i++) {
    x = rStarData[i*6-6]; y = rStarData[i*6-5]; z = rStarData[i*6-4];
    planet[oNorm+i] = RFromD(x*rDegMax/24.0+y*15.0/60.0+z*0.25/60.0);
    x = rStarData[i*6-3]; y = rStarData[i*6-2]; z = rStarData[i*6-1];
    planetalt[oNorm+i] = RFromD(x+y/60.0+z/60.0/60.0);
    /* Convert to ecliptic zodiac coordinates. */
    EquToEcl(&planet[oNorm+i], &planetalt[oNorm+i]);
    planet[oNorm+i] = Mod(DFromR(planet[oNorm+i])+rEpoch2000+SD);
    planetalt[oNorm+i] = DFromR(planetalt[oNorm+i]);
    ret[oNorm+i] = RFromD(rDegMax/26000.0/365.25);
    starname[i] = i;
  }

  /* Sort the index list if -Uz, -Ul, -Un, or -Ub switch in effect. */

  if (us.nStar > 1) for (i = 2; i <= cStar; i++) {
    j = i-1;

    /* Compare star names for -Un switch. */

    if (us.nStar == 'n') while (j > 0 && NCompareSz(
      szObjName[oNorm+starname[j]], szObjName[oNorm+starname[j+1]]) > 0) {
      SwapN(starname[j], starname[j+1]);
      j--;

    /* Compare star brightnesses for -Ub switch. */

    } else if (us.nStar == 'b') while (j > 0 &&
      rStarBright[starname[j]] > rStarBright[starname[j+1]]) {
      SwapN(starname[j], starname[j+1]);
      j--;

    /* Compare star zodiac locations for -Uz switch. */

    } else if (us.nStar == 'z') while (j > 0 &&
      planet[oNorm+starname[j]] > planet[oNorm+starname[j+1]]) {
      SwapN(starname[j], starname[j+1]);
      j--;

    /* Compare star declinations for -Ul switch. */

    } else if (us.nStar == 'l') while (j > 0 &&
      planetalt[oNorm+starname[j]] < planetalt[oNorm+starname[j+1]]) {
      SwapN(starname[j], starname[j+1]);
      j--;
    }
  }
}


/*
******************************************************************************
** Chart Calculation.
******************************************************************************
*/

/* Given a zodiac degree, transform it into its Decan sign, where each    */
/* sign is trisected into the three signs of its element. For example,    */
/* 1 Aries -> 3 Aries, 10 Leo -> 0 Sagittarius, 25 Sagittarius -> 15 Leo. */

real Decan(deg)
real deg;
{
  int sign;
  real unit;

  sign = SFromZ(deg);
  unit = deg - ZFromS(sign);
  sign = Mod12(sign + 4*((int)RFloor(unit/10.0)));
  unit = (unit - RFloor(unit/10.0)*10.0)*3.0;
  return ZFromS(sign)+unit;
}


/* Transform spherical to rectangular coordinates in x, y, z. */

void SphToRec(r, azi, alt, rx, ry, rz)
real r, azi, alt, *rx, *ry, *rz;
{
  real rT;

  *rz = r *RSinD(alt);
  rT  = r *RCosD(alt);
  *rx = rT*RCosD(azi);
  *ry = rT*RSinD(azi);
}


#ifdef PLACALC
/* Compute the positions of the planets at a certain time using the Placalc */
/* accurate formulas and ephemeris. This will superseed the Matrix routine  */
/* values and is only called with the -b switch is in effect. Not all       */
/* objects or modes are available using this, but some additional values    */
/* such as Moon and Node velocities not available without -b are. (This is  */
/* the one place in Astrolog which calls the Placalc package functions.)    */

void ComputePlacalc(t)
real t;
{
  int i;
  real r1, r2, r3, r4;

  /* We can compute the positions of Sun through Pluto, Chiron, the four */
  /* asteroids, Lilith, and the (true or mean) North Node using Placalc. */
  /* The other objects must be done elsewhere.                           */

  for (i = oSun; i <= oLil; i++) {
    if ((ignore[i] && i > oMoo) ||
      (us.fPlacalcAst && FBetween(i, oCer, oVes)))
      continue;
    if (FPlacalcPlanet(i, t*36525.0+2415020.0, us.objCenter != oSun,
      &r1, &r2, &r3, &r4)) {

      /* Note that this can't compute charts with central planets other */
      /* than the Sun or Earth or relative velocities in current state. */

      planet[i]    = Mod(r1 + is.rSid);
      planetalt[i] = r2;
      ret[i]       = RFromD(r3);

      /* Compute x,y,z coordinates from azimuth, altitude, and distance. */

      SphToRec(r4, planet[i], planetalt[i],
        &spacex[i], &spacey[i], &spacez[i]);
    }
  }
}
#endif


/* This is probably the main routine in all of Astrolog. It generates a   */
/* chart, calculating the positions of all the celestial bodies and house */
/* cusps, based on the current chart information, and saves them for use  */
/* by any of the display routines.                                        */

real CastChart(fDate)
bool fDate;
{
  CI ci;
  real housetemp[cSign+1], Off = 0.0, vtx, j;
  int i, k;

  /* Hack: Time zone +/-24 means to have the time of day be in Local Mean */
  /* Time (LMT). This is done by making the time zone value reflect the   */
  /* logical offset from GMT as indicated by the chart's longitude value. */

  if (RAbs(ZZ) == 24.0)
    ZZ = DegToDec(DecToDeg(OO)/15.0);
  ci = ciCore;

  if (MM == -1) {

    /* Hack: If month is negative, then we know chart was read in through a  */
    /* -o0 position file, so the planet positions are already in the arrays. */

    MC = planet[oMC]; Asc = planet[oAsc];
  } else {
    for (i = 1; i <= cObj; i++) {
      planet[i] = planetalt[i] = 0.0;    /* On ecliptic unless we say so.  */
      ret[i] = 1.0;                      /* Direct until we say otherwise. */
    }
    Off = ProcessInput(fDate);
    ComputeVariables(&vtx);
    if (us.fGeodetic)               /* Check for -G geodetic chart. */
      RA = RFromD(Mod(-OO));
    MC  = CuspMidheaven();          /* Calculate our Ascendant & Midheaven. */
    Asc = CuspAscendant();
    ComputeHouses(us.nHouseSystem); /* Go calculate house cusps. */

    /* Go calculate planet, Moon, and North Node positions. */

    ComputePlanets();
    if (!ignore[oMoo] || !ignore[oNod] || !ignore[oSou] || !ignore[oFor]) {
      ComputeLunar(&planet[oMoo], &planetalt[oMoo],
        &planet[oNod], &planetalt[oNod]);
      ret[oNod] = -1.0;
    }

    /* Compute more accurate ephemeris positions for certain objects. */

#ifdef PLACALC
    if (us.fPlacalc)
      ComputePlacalc(T);
#endif
    if (!us.fPlacalc) {
      planet[oSou] = Mod(planet[oNod]+rDegHalf);
      ret[oSou] = ret[oNod] = RFromD(-0.053);
      ret[oMoo] = RFromD(12.5);
    }

    /* Calculate position of Part of Fortune. */

    j = planet[oMoo]-planet[oSun];
    if (us.nArabicNight < 0)
      neg(j);
    j = RAbs(j) < rDegQuad ? j : j - RSgn(j)*rDegMax;
    planet[oFor] = Mod(j+Asc);

    /* Fill in "planet" positions corresponding to house cusps. */

    planet[oVtx] = vtx; planet[oEP] = CuspEastPoint();
    for (i = 2; i <= cSign; i++)
      planet[cuspLo + i - 1] = house[i];
    planet[oAsc] = Asc; planet[oMC] = MC;
    planet[oDes] = Mod(Asc + rDegHalf); planet[oNad] = Mod(MC + rDegHalf);
    for (i = oFor; i <= cuspHi; i++)
      ret[i] = RFromD(rDegMax);
  }

  /* Go calculate star positions if -U switch in effect. */

  if (us.nStar)
    ComputeStars(us.fSidereal ? 0.0 : -Off);

  /* Transform ecliptic to equatorial coordinates if -sr in effect. */

  if (us.fEquator)
    for (i = 1; i <= cObj; i++) if (!ignore[i]) {
      planet[i]    = RFromD(Tropical(planet[i]));
      planetalt[i] = RFromD(planetalt[i]);
      EclToEqu(&planet[i], &planetalt[i]);
      planet[i]    = DFromR(planet[i]);
      planetalt[i] = DFromR(planetalt[i]);
    }

  /* Now, we may have to modify the base positions we calculated above based */
  /* on what type of chart we are generating.                                */

  if (us.fProgress && us.fSolarArc) { /* Are we doing a -p0 solar arc chart? */
    for (i = 1; i <= cObj; i++)
      planet[i] = Mod(planet[i] + (is.JDp - is.JD) / us.rProgDay);
    for (i = 1; i <= cSign; i++)
      house[i]  = Mod(house[i]  + (is.JDp - is.JD) / us.rProgDay);
    }
  if (us.nHarmonic > 1)             /* Are we doing a -x harmonic chart?     */
    for (i = 1; i <= cObj; i++)
      planet[i] = Mod(planet[i] * (real)us.nHarmonic);
  if (us.objOnAsc) {
    if (us.objOnAsc > 0)            /* Is -1 put on Ascendant in effect?     */
      j = planet[us.objOnAsc]-Asc;
    else                            /* Or -2 put object on Midheaven switch? */
      j = planet[-us.objOnAsc]-MC;
    for (i = 1; i <= cSign; i++)    /* If so, rotate the houses accordingly. */
      house[i] = Mod(house[i]+j);
  }

  /* Check to see if we are -F forcing any objects to be particular values. */

  for (i = 1; i <= cObj; i++)
    if (force[i] != 0.0) {
      planet[i] = force[i]-rDegMax;
      planetalt[i] = ret[i] = 0.0;
    }

  ComputeInHouses();        /* Figure out what house everything falls in. */

  /* If -f domal chart switch in effect, switch planet and house positions. */

  if (us.fFlip) {
    for (i = 1; i <= cObj; i++) {
      k = inhouse[i];
      inhouse[i] = SFromZ(planet[i]);
      planet[i] = ZFromS(k)+MinDistance(house[k], planet[i]) /
        MinDistance(house[k], house[Mod12(k+1)])*30.0;
    }
    for (i = 1; i <= cSign; i++) {
      k = HousePlaceIn(ZFromS(i));
      housetemp[i] = ZFromS(k)+MinDistance(house[k], ZFromS(i)) /
        MinDistance(house[k], house[Mod12(k+1)])*30.0;
    }
    for (i = 1; i <= cSign; i++)
      house[i] = housetemp[i];
  }

  /* If -3 decan chart switch in effect, edit planet positions accordingly. */

  if (us.fDecan) {
    for (i = 1; i <= cObj; i++)
      planet[i] = Decan(planet[i]);
    ComputeInHouses();
  }

  ciCore = ci;
  return T;
}


/*
******************************************************************************
** Aspect Calculations.
******************************************************************************
*/

/* Set up the aspect/midpoint grid. Allocate memory for this array, if not */
/* already done. Allocation is only done once, first time this is called.  */

bool FEnsureGrid()
{
  if (grid != NULL)
    return fTrue;
  grid = (GridInfo FPTR *)PAllocate(sizeof(GridInfo), fFalse, "grid");
  return grid != NULL;
}


/* Indicate whether some aspect between two objects should be shown. */

bool FAcceptAspect(obj1, asp, obj2)
int obj1, asp, obj2;
{
  int fSupp;

  if (ignorea(asp))    /* If the aspect restricted, reject immediately. */
    return fFalse;
  if (us.fSmartCusp) {

    /* Allow only conjunctions to the minor house cusps. */

    if ((FMinor(obj1) || FMinor(obj2)) && asp > aCon)
      return fFalse;

    /* Prevent any simultaneous aspects to opposing angle cusps,     */
    /* e.g. if conjunct one, don't be opposite the other; if trine   */
    /* one, don't sextile the other; don't square both at once, etc. */

    fSupp = (asp == aOpp || asp == aSex || asp == aSSx || asp == aSes);
    if ((FAngle(obj1) || FAngle(obj2)) &&
      (fSupp || (asp == aSqu &&
      (obj1 == oDes || obj2 == oDes || obj1 == oNad || obj2 == oNad))))
      return fFalse;

    /* Prevent any simultaneous aspects to the North and South Node. */

    if (fSouthNode) {
      if (((obj1 == oNod || obj2 == oNod) && fSupp) ||
        ((obj1 == oSou || obj2 == oSou) && (fSupp || asp == aSqu)))
        return fFalse;
    }
  }
  return fTrue;
}


/* This is a subprocedure of FCreateGrid() and FCreateGridRelation().   */
/* Given two planets, determine what aspect, if any, is present between */
/* them, and save the aspect name and orb in the specified grid cell.   */

void GetAspect(planet1, planet2, ret1, ret2, i, j)
real *planet1, *planet2, *ret1, *ret2;
int i, j;
{
  int k;
  real l, m;

  grid->v[i][j] = grid->n[i][j] = 0;
  l = MinDistance(planet2[i], planet1[j]);
  for (k = us.nAsp; k >= 1; k--) {
    if (!FAcceptAspect(i, k, j))
      continue;
    m = l-rAspAngle[k];
    if (RAbs(m) < GetOrb(i, j, k)) {
      grid->n[i][j] = k;

      /* If -ga switch in effect, then change the sign of the orb to    */
      /* correspond to whether the aspect is applying or separating.    */
      /* To do this, we check the velocity vectors to see if the        */
      /* planets are moving toward, away, or are overtaking each other. */

      if (us.fAppSep)
        m = RSgn2(ret1[j]-ret2[i])*
          RSgn2(MinDifference(planet2[i], planet1[j]))*RSgn2(m)*RAbs(m);
      grid->v[i][j] = (int)(m*60.0);
    }
  }
}


/* Very similar to GetAspect(), this determines if there is a parallel or */
/* contraparallel aspect between the given two planets, and stores the    */
/* result as above. The settings and orbs for conjunction are used for    */
/* parallel and those for opposition are used for contraparallel.         */

void GetParallel(planet1, planet2, planetalt1, planetalt2, i, j)
real *planet1, *planet2, *planetalt1, *planetalt2;
int i, j;
{
  int k;
  real l, alt1, alt2;

  l = RFromD(planet1[j]); alt1 = RFromD(planetalt1[j]);
  EclToEqu(&l, &alt1); alt1 = DFromR(alt1);
  l = RFromD(planet2[i]); alt2 = RFromD(planetalt2[i]);
  EclToEqu(&l, &alt2); alt2 = DFromR(alt2);
  grid->v[i][j] = grid->n[i][j] = 0;
  for (k = Min(us.nAsp, aOpp); k >= 1; k--) {
    if (!FAcceptAspect(i, k, j))
      continue;
    l = RAbs(k == aCon ? alt1 - alt2 : RAbs(alt1) - RAbs(alt2));
    if (l < GetOrb(i, j, k)) {
      grid->n[i][j] = k;
      grid->v[i][j] = (int)(l*60.0);
    }
  }
}


/* Fill in the aspect grid based on the aspects taking place among the */
/* planets in the present chart. Also fill in the midpoint grid.       */

bool FCreateGrid(fFlip)
bool fFlip;
{
  int i, j, k;
  real l;

  if (!FEnsureGrid())
    return fFalse;
  for (j = 1; j <= cObj; j++) if (!ignore[j])
    for (i = 1; i <= cObj; i++) if (!ignore[i])

      /* The parameter 'flip' determines what half of the grid is filled in */
      /* with the aspects and what half is filled in with the midpoints.    */

      if (fFlip ? i > j : i < j) {
        if (us.fParallel)
          GetParallel(planet, planet, planetalt, planetalt, i, j);
        else
          GetAspect(planet, planet, ret, ret, i, j);
      } else if (fFlip ? i < j : i > j) {
        l = Mod(Midpoint(planet[i], planet[j])); k = (int)l;  /* Calculate */
        grid->n[i][j] = k/30+1;                               /* midpoint. */
        grid->v[i][j] = (int)((l-(real)(k/30)*30.0)*60.0);
      } else {
        grid->n[i][j] = SFromZ(planet[j]);
        grid->v[i][j] = (int)(planet[j]-(real)(grid->n[i][j]-1)*30.0);
      }
  return fTrue;
}


/* This is similar to the previous function; however, this time fill in the */
/* grid based on the aspects (or midpoints if 'acc' set) taking place among */
/* the planets in two different charts, as in the -g -r0 combination.       */

bool FCreateGridRelation(fMidpoint)
bool fMidpoint;
{
  int i, j, k;
  real l;

  if (!FEnsureGrid())
    return fFalse;
  for (j = 1; j <= cObj; j++) if (!ignore[j])
    for (i = 1; i <= cObj; i++) if (!ignore[i])
      if (!fMidpoint) {
        if (us.fParallel)
          GetParallel(cp1.obj, cp2.obj, cp1.alt, cp2.alt, i, j);
        else
          GetAspect(cp1.obj, cp2.obj, cp1.dir, cp2.dir, i, j);
      } else {
        l = Mod(Midpoint(cp2.obj[i], cp1.obj[j])); k = (int)l; /* Calculate */
        grid->n[i][j] = k/30+1;                                /* midpoint. */
        grid->v[i][j] = (int)((l-(real)(k/30)*30.0)*60.0);
      }
  return fTrue;
}


/* Fill out tables based on the number of unrestricted planets in signs by  */
/* element, signs by mode, as well as other values such as the number of    */
/* objects in yang vs. yin signs, in various house hemispheres (north/south */
/* and east/west), and the number in first six signs vs. second six signs.  */
/* This is used by the -v chart listing and the sidebar in graphics charts. */

void CreateElemTable(pet)
ET *pet;
{
  int i, s;

  ClearB((lpbyte)pet, (int)sizeof(ET));
  for (i = 1; i <= cObj; i++) if (!ignore[i]) {
    pet->coSum++;
    s = SFromZ(planet[i]);
    pet->coSign[s-1]++;
    pet->coElemMode[(s-1)&3][(s-1)%3]++;
    pet->coElem[(s-1)&3]++; pet->coMode[(s-1)%3]++;
    pet->coYang += (s & 1);
    pet->coLearn += (s < sLib);
    if (!FCusp(i)) {
      pet->coHemi++;
      s = inhouse[i];
      pet->coHouse[s-1]++;
      pet->coModeH[(s-1)%3]++;
      pet->coMC += (s >= sLib);
      pet->coAsc += (s < sCan || s >= sCap);
    }
  }
  pet->coYin   = pet->coSum  - pet->coYang;
  pet->coShare = pet->coSum  - pet->coLearn;
  pet->coDes   = pet->coHemi - pet->coAsc;
  pet->coIC    = pet->coHemi - pet->coMC;
}

/* calc.c */
