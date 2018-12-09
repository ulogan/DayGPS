/*
** Astrolog (Version 5.05) File: xcharts2.c
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


#ifdef GRAPH
/*
******************************************************************************
** Chart Graphics Utility Procedures.
******************************************************************************
*/

/* Return whether the specified object should be displayed in the current */
/* graphics chart type. For example, don't include the Moon in the solar  */
/* system charts, don't include house cusps in astro-graph, and so on.    */

bool FProper(i)
int i;
{
  bool f;

  if (gi.nMode == gHorizon || fMap || gi.nMode == gGlobe ||
    gi.nMode == gPolar)                       /* Horizon and map charts */
    f = FObject(i);
  else if (gi.nMode == gOrbit)                /* Solar system charts */
    f = FObject(i) && (i != oMoo || (us.fPlacalc && us.objCenter < oMoo));
  else if (gi.nMode == gEphemeris)
    f = FThing(i);                            /* Ephemeris charts */
  else
    f = fTrue;
  return f && !ignore[i];                     /* Check restriction status */
}


/* Adjust an array of zodiac positions so that no two positions are within   */
/* a certain orb of each other. This is used by the wheel drawing chart      */
/* routines in order to make sure that we don't draw any planet glyphs on    */
/* top of each other. We'll later draw the glyphs at the adjusted positions. */

void FillSymbolRing(symbol)
real *symbol;
{
  real orb = DEFORB*256.0/(real)gs.yWin*(real)gi.nScale, k1, k2, temp;
  int i, j, k = 1, l;

  /* Keep adjusting as long as we can still make changes, or until we do 'n' */
  /* rounds. (With many objects, there just may not be enough room for all.) */

  for (l = 0; k && l < us.nDivision*2; l++) {
    k = 0;
    for (i = 1; i <= cObj; i++) if (FProper(i)) {

      /* For each object, determine who is closest on either side. */

      k1 = rLarge; k2 = -rLarge;
      for (j = 1; j <= cObj; j++)
        if (FProper(j) && i != j) {
          temp = symbol[j]-symbol[i];
          if (RAbs(temp) > rDegHalf)
            temp -= rDegMax*RSgn(temp);
          if (temp < k1 && temp > 0.0)
            k1 = temp;
          else if (temp > k2 && temp <= 0.0)
            k2 = temp;
        }

      /* If an object's too close on one side, then we move to the other. */

      if (k2 > -orb && k1 > orb) {
        k = 1; symbol[i] = Mod(symbol[i]+orb*0.51+k2*0.49);
      } else if (k1 < orb && k2 < -orb) {
        k = 1; symbol[i] = Mod(symbol[i]-orb*0.51+k1*0.49);

      /* If we are bracketed by close objects on both sides, then let's move */
      /* to the midpoint, so we are as far away as possible from either one. */

      } else if (k2 > -orb && k1 < orb) {
        k = 1; symbol[i] = Mod(symbol[i]+(k1+k2)*0.5);
      }
    }
  }
}


/* Adjust an array of longitude positions so that no two are within a    */
/* certain orb of each other. This is used by the astro-graph routine to */
/* make sure we don't draw any planet glyphs marking the lines on top of */
/* each other. This is almost identical to the FillSymbolRing() routine  */
/* used by the wheel charts; however, there the glyphs are placed in a   */
/* continuous ring, while here we have the left and right screen edges.  */
/* Also, here we are placing two sets of planets at the same time.       */

void FillSymbolLine(symbol)
real *symbol;
{
  real orb = DEFORB*1.35*(real)gi.nScale, max = rDegMax, k1, k2, temp;
  int i, j, k = 1, l;

  if (gi.nMode != gEphemeris)
    max *= (real)gi.nScale;
  else
    orb *= rDegMax/(real)gs.xWin;

  /* Keep adjusting as long as we can still make changes. */

  for (l = 0; k && l < us.nDivision*2; l++) {
    k = 0;
    for (i = 1; i <= cObj*2; i++)
      if (FProper((i+1)/2) && symbol[i] >= 0.0) {

        /* For each object, determine who is closest to the left and right. */

        k1 = max-symbol[i]; k2 = -symbol[i];
        for (j = 1; j <= cObj*2; j++) {
          if (FProper((j+1)/2) && i != j) {
            temp = symbol[j]-symbol[i];
            if (temp < k1 && temp > 0.0)
              k1 = temp;
            else if (temp > k2 && temp <= 0.0)
              k2 = temp;
          }
        }

        /* If an object's too close on one side, then we move to the other. */

        if (k2 > -orb && k1 > orb) {
          k = 1; symbol[i] = symbol[i]+orb*0.51+k2*0.49;
        } else if (k1 < orb && k2 < -orb) {
          k = 1; symbol[i] = symbol[i]-orb*0.51+k1*0.49;
        } else if (k2 > -orb && k1 < orb) {
          k = 1; symbol[i] = symbol[i]+(k1+k2)*0.5;
        }
      }
  }
}


/* Given a zodiac degree, adjust it if need be to account for the expanding */
/* and compacting of parts the zodiac that happen when we display a graphic */
/* wheel chart such that all the houses appear the same size.               */

real HousePlaceInX(deg)
real deg;
{
  int in;

  if (gi.nMode == gWheel)    /* We only adjust for the -w -X combination. */
    return deg;
  in = HousePlaceIn(deg);
  return Mod(ZFromS(in)+MinDistance(house[in], deg)/
    MinDistance(house[in], house[Mod12(in+1)])*30.0);
}


/*
******************************************************************************
** Multiple Chart Graphics Routines.
******************************************************************************
*/

/* Draw another wheel chart; however, this time we have two rings of planets */
/* because we are doing a relationship chart between two sets of data. This  */
/* chart is obtained when the -r0 is combined with the -X switch.            */

void XChartWheelRelation()
{
  real xsign[cSign+1], xhouse1[cSign+1], xplanet1[objMax], xplanet2[objMax],
    symbol[objMax];
  int cx, cy, i, j;
  real asc, unitx, unity, px, py, temp;

  /* Set up variables and temporarily automatically decrease the horizontal */
  /* chart size to leave room for the sidebar if that mode is in effect.    */

  if (gs.fText && !us.fVelocity)
    gs.xWin -= xSideT;
  cx = gs.xWin/2 - 1; cy = gs.yWin/2 - 1;
  unitx = (real)cx; unity = (real)cy;
  asc = gs.objLeft ? cp1.obj[abs(gs.objLeft)]+90*(gs.objLeft < 0) :
    cp1.cusp[1];

  /* Fill out arrays with the degree of each object, cusp, and sign glyph. */

  if (gi.nMode == gWheel) {
    for (i = 1; i <= cSign; i++)
      xhouse1[i] = PZ(cp1.cusp[i]);
  } else {
    asc -= cp1.cusp[1];
    for (i = 1; i <= cSign; i++)
      xhouse1[i] = PZ(ZFromS(i));
  }
  for (i = 1; i <= cSign; i++)
    xsign[i] = PZ(HousePlaceInX(ZFromS(i)));
  for (i = 1; i <= cObj; i++)
    xplanet1[i] = PZ(HousePlaceInX(cp1.obj[i]));
  for (i = 1; i <= cObj; i++)
    xplanet2[i] = PZ(HousePlaceInX(cp2.obj[i]));

  /* Draw the horizon and meridian lines across whole chart, and draw the */
  /* zodiac and house rings, exactly like before. We are drawing only the */
  /* houses of one of the two charts in the relationship, however.        */

  DrawColor(gi.kiLite);
  DrawDash(cx+POINT1(unitx, 0.99, PX(xhouse1[sAri])),
           cy+POINT1(unity, 0.99, PY(xhouse1[sAri])),
           cx+POINT1(unitx, 0.99, PX(xhouse1[sLib])),
           cy+POINT1(unity, 0.99, PY(xhouse1[sLib])), !gs.fColor);
  DrawDash(cx+POINT1(unitx, 0.99, PX(xhouse1[sCap])),
           cy+POINT1(unity, 0.99, PY(xhouse1[sCap])),
           cx+POINT1(unitx, 0.99, PX(xhouse1[sCan])),
           cy+POINT1(unity, 0.99, PY(xhouse1[sCan])), !gs.fColor);
  for (i = 0; i < nDegMax; i += 5-(gs.fColor || gs.fPS || gs.fMeta)*4) {
    temp = PZ(HousePlaceInX((real)i));
    px = PX(temp); py = PY(temp);
    DrawColor(i%5 ? gi.kiGray : gi.kiOn);
    DrawDash(cx+POINT1(unitx, 0.78, px), cy+POINT1(unity, 0.78, py),
      cx+POINT2(unitx, 0.82, px), cy+POINT2(unity, 0.82, py),
      ((gs.fPS || gs.fMeta) && i%5)*2);
  }

  DrawColor(gi.kiOn);
  DrawCircle(cx, cy, (int)(unitx*0.95+rRound), (int)(unity*0.95+rRound));
  DrawCircle(cx, cy, (int)(unitx*0.82+rRound), (int)(unity*0.82+rRound));
  DrawCircle(cx, cy, (int)(unitx*0.78+rRound), (int)(unity*0.78+rRound));
  DrawCircle(cx, cy, (int)(unitx*0.70+rRound), (int)(unity*0.70+rRound));

  for (i = 1; i <= cSign; i++) {
    temp = xsign[i];
    DrawColor(gi.kiOn);
    DrawLine(cx+POINT2(unitx, 0.95, PX(temp)),
      cy+POINT2(unity, 0.95, PY(temp)),
      cx+POINT1(unitx, 0.82, PX(temp)),
      cy+POINT1(unity, 0.82, PY(temp)));
    DrawLine(cx+POINT2(unitx, 0.78, PX(xhouse1[i])),
      cy+POINT2(unity, 0.78, PY(xhouse1[i])),
      cx+POINT1(unitx, 0.70, PX(xhouse1[i])),
      cy+POINT1(unity, 0.70, PY(xhouse1[i])));
    if (gs.fColor && i%3 != 1) {
      DrawColor(gi.kiGray);
      DrawDash(cx, cy, cx+POINT1(unitx, 0.70, PX(xhouse1[i])),
        cy+POINT1(unity, 0.70, PY(xhouse1[i])), 1);
    }
    temp = Midpoint(temp, xsign[Mod12(i+1)]);
    DrawColor(kSignB(i));
    DrawSign(i, cx+POINT1(unitx, 0.885, PX(temp)),
      cy+POINT1(unity, 0.885, PY(temp)));
    temp = Midpoint(xhouse1[i], xhouse1[Mod12(i+1)]);
    DrawHouse(i, cx+POINT1(unitx, 0.74, PX(temp)),
      cy+POINT1(unity, 0.74, PY(temp)));
  }

  /* Draw the outer ring of planets (based on the planets in the chart     */
  /* which the houses do not reflect - the houses belong to the inner ring */
  /* below). Draw each glyph, a line from it to its actual position point  */
  /* in the outer ring, and then draw another line from this point to a    */
  /* another dot at the same position in the inner ring as well.           */

  for (i = 1; i <= cObj; i++)
    symbol[i] = xplanet2[i];
  FillSymbolRing(symbol);
  for (i = cObj; i >= 1; i--) if (FProper2(i)) {
    if (gs.fLabel) {
      temp = symbol[i];
      DrawColor(cp2.dir[i] < 0.0 ? gi.kiGray : gi.kiOn);
      DrawDash(cx+POINT1(unitx, 0.58, PX(xplanet2[i])),
        cy+POINT1(unity, 0.58, PY(xplanet2[i])),
        cx+POINT2(unitx, 0.61, PX(temp)),
        cy+POINT2(unity, 0.61, PY(temp)),
        (cp2.dir[i] < 0.0 ? 1 : 0) - gs.fColor);
      DrawObject(i, cx+POINT1(unitx, 0.65, PX(temp)),
        cy+POINT1(unity, 0.65, PY(temp)));
    }
    DrawColor(kObjB[i]);
    DrawPoint(cx+POINT1(unitx, 0.56, PX(xplanet2[i])),
      cy+POINT1(unity, 0.56, PY(xplanet2[i])));
    DrawPoint(cx+POINT1(unitx, 0.43, PX(xplanet2[i])),
      cy+POINT1(unity, 0.43, PY(xplanet2[i])));
    DrawColor(cp2.dir[i] < 0.0 ? gi.kiGray : gi.kiOn);
    DrawDash(cx+POINT1(unitx, 0.45, PX(xplanet2[i])),
      cy+POINT1(unity, 0.45, PY(xplanet2[i])),
      cx+POINT2(unitx, 0.54, PX(xplanet2[i])),
      cy+POINT2(unity, 0.54, PY(xplanet2[i])), 2-gs.fColor);
  }

  /* Now draw the inner ring of planets. If it weren't for the outer ring,  */
  /* this would be just like the standard non-relationship wheel chart with */
  /* only one set of planets. Again, draw glyph, and a line to true point.  */

  for (i = 1; i <= cObj; i++)
    symbol[i] = xplanet1[i];
  FillSymbolRing(symbol);
  for (i = 1; i <= cObj; i++) if (FProper(i)) {
    if (gs.fLabel) {
      temp = symbol[i];
      DrawColor(cp1.dir[i] < 0.0 ? gi.kiGray : gi.kiOn);
      DrawDash(cx+POINT1(unitx, 0.45, PX(xplanet1[i])),
        cy+POINT1(unity, 0.45, PY(xplanet1[i])),
        cx+POINT2(unitx, 0.48, PX(temp)),
        cy+POINT2(unity, 0.48, PY(temp)),
        (cp1.dir[i] < 0.0 ? 1 : 0) - gs.fColor);
      DrawObject(i, cx+POINT1(unitx, 0.52, PX(temp)),
        cy+POINT1(unity, 0.52, PY(temp)));
    } else
      DrawColor(kObjB[i]);
    DrawPoint(cx+POINT1(unitx, 0.43, PX(xplanet1[i])),
      cy+POINT1(unity, 0.43, PY(xplanet1[i])));
  }

  /* Draw lines connecting planets between the two charts that have aspects. */

  if (!gs.fAlt) {                      /* Don't draw aspects in bonus mode. */
    if (!FCreateGridRelation(fFalse))
      return;
    for (j = cObj; j >= 1; j--)
      for (i = cObj; i >= 1; i--)
        if (grid->n[i][j] && FProper2(i) && FProper(j)) {
          DrawColor(kAspB[grid->n[i][j]]);
          DrawDash(cx+POINT1(unitx, 0.41, PX(xplanet1[j])),
            cy+POINT1(unity, 0.41, PY(xplanet1[j])),
            cx+POINT1(unitx, 0.41, PX(xplanet2[i])),
            cy+POINT1(unity, 0.41, PY(xplanet2[i])),
            abs(grid->v[i][j]/60/2));
        }
  }

  /* Go draw sidebar with chart information and positions if need be. */

  DrawInfo();
}


/* Draw an aspect (or midpoint) grid in the window, between the planets in  */
/* two different charts, with the planets labeled at the top and side. This */
/* chart is done when the -g switch is combined with the -r0 and -X switch. */
/* Like above, the chart always has a (definable) fixed number of cells.    */

void XChartGridRelation()
{
  char sz[cchSzDef];
  int unit, siz, x, y, i, j, k, l;
  KI c;

  unit = CELLSIZE*gi.nScale; siz = (gs.nGridCell+1)*unit;
  if (!FCreateGridRelation(gs.fAlt != us.fGridConfig))
    return;
  for (y = 0, j = -1; y <= gs.nGridCell; y++) {
    do {
      j++;
    } while (ignore[j] && j <= cObj);
    DrawColor(gi.kiGray);
    DrawDash(0, (y+1)*unit, siz, (y+1)*unit, !gs.fColor);
    DrawDash((y+1)*unit, 0, (y+1)*unit, siz, !gs.fColor);
    DrawColor(gi.kiLite);
    DrawEdge(0, y*unit, unit, (y+1)*unit);
    DrawEdge(y*unit, 0, (y+1)*unit, unit);
    if (j <= cObj) for (x = 0, i = -1; x <= gs.nGridCell; x++) {
      do {
        i++;
      } while (ignore[i] && i <= cObj);

      /* Again, we are looping through each cell in each row and column. */

      if (i <= cObj) {
        gi.xTurtle = x*unit+unit/2;
        gi.yTurtle = y*unit+unit/2 -
          (gi.nScale/gi.nScaleT > 2 ? 5*gi.nScaleT : 0);
        k = grid->n[i][j];

        /* If current cell is on top row or left hand column, draw glyph */
        /* of planet owning the particular row or column in question.    */

        if (y == 0 || x == 0) {
          if (x+y > 0)
            DrawObject(j == 0 ? i : j, gi.xTurtle, gi.yTurtle);
        } else {

        /* Otherwise, draw glyph of aspect in effect, or glyph of */
        /* sign of midpoint, between the two planets in question. */

          if (gs.fAlt == us.fGridConfig) {
            if (k) {
              DrawColor(c = kAspB[k]);
              DrawAspect(k, gi.xTurtle, gi.yTurtle);
            }
          } else {
            DrawColor(c = kSignB(grid->n[i][j]));
            DrawSign(grid->n[i][j], gi.xTurtle, gi.yTurtle);
          }
        }

        /* Again, when scale size is 300+, print some text in current cell: */

        if (gi.nScale/gi.nScaleT > 2 && gs.fLabel) {

          /* For top and left edges, print sign and degree of the planet. */

          if (y == 0 || x == 0) {
            if (x+y > 0) {
              k = SFromZ(y == 0 ? cp2.obj[i] : cp1.obj[j]);
              l = (int)((y == 0 ? cp2.obj[i] : cp1.obj[j])-ZFromS(k));
              c = kSignB(k);
              sprintf(sz, "%c%c%c %02d", chSig3(k), l);

              /* For extreme upper left corner, print some little arrows */
              /* pointing out chart1's planets and chart2's planets.     */

            } else {
              c = gi.kiLite;
              sprintf(sz, "1v 2->");
            }
          } else {
            k = abs(grid->v[i][j]);

            /* For aspect cells, print the orb in degrees and minutes. */

            if (gs.fAlt == us.fGridConfig) {
              if (grid->n[i][j])
                sprintf(sz, "%c%d %02d'", k != grid->v[i][j] ?
                  (us.fAppSep ? 'a' : '-') : (us.fAppSep ? 's' : '+'),
                  k/60, k%60);
              else
                sprintf(sz, "");

            /* For midpoint cells, print degree and minute. */

            } else
              sprintf(sz, "%2d %02d'", k/60, k%60);
          }
          DrawColor(c);
          DrawSz(sz, x*unit+unit/2, (y+1)*unit-3*gi.nScaleT, dtBottom);
        }
      }
    }
  }
}


#ifdef BIORHYTHM
/* Draw a graphic biorhythm chart on the screen, as is done when the -rb    */
/* switch is combined with -X. This is technically a relationship chart in  */
/* that biorhythm status is determined by a natal chart time at another     */
/* later time. For the day in question, and for two weeks before and after, */
/* the Physical, Emotional, and Mental percentages are plotted.             */

void XChartBiorhythm()
{
  char sz[6], *c;
  real jd, r, a;
  int x1, x2, xs, cx, y1, y2, ys, cy, i, j, k, x, y, x0, y0;

  k = xFont*6*gi.nScaleT;
  x1 = k; x2 = gs.xWin-k; xs = x2-x1; cx = (x1+x2)/2;
  k = CELLSIZE;
  y1 = k; y2 = gs.yWin-k; ys = y2-y1; cy = (y1+y2)/2;

  /* Create a dotted day/percentage grid to graph on. */
  DrawColor(gi.kiGray);
  DrawDash(x1, cy, x2, cy, 1);
  DrawDash(cx, y1, cx, y2, 1);
  for (j = -BIODAYS+1; j <= BIODAYS-1; j++) {
    x = x1 + NMultDiv(xs, j+BIODAYS, BIODAYS*2);
    for (k = -90; k <= 90; k += 10) {
      y = y1 + NMultDiv(ys, 100+k, 200);
      DrawPoint(x, y);
    }
  }

  /* Now actually draw the three biorhythm curves. */
  for (i = 1; i <= 3; i++) {
    jd = RFloor(is.JD + rRound);
    switch (i) {
    case 1: r = brPhy; c = "PHYS"; j = eFir; break;
    case 2: r = brEmo; c = "EMOT"; j = eWat; break;
    case 3: r = brInt; c = "INTE"; j = eEar; break;
    }
    DrawColor(kElemB[j]);
    for (jd -= (real)BIODAYS, j = -BIODAYS; j <= BIODAYS; j++, jd += 1.0) {
      a = RBiorhythm(jd, r);
      x = x1 + NMultDiv(xs, j+BIODAYS, BIODAYS*2);
      y = y1 + (int)((real)ys * (100.0-a) / 200.0);
      if (j > -BIODAYS)
        DrawLine(x0, y0, x, y);
      else
        DrawSz(c, x1/2, y+2*gi.nScaleT, dtCent);
      x0 = x; y0 = y;
    }
  }

  DrawColor(gi.kiLite);
  /* Label biorhythm percentages along right vertical axis. */
  for (k = -100; k <= 100; k += 10) {
    sprintf(sz, "%c%3d%%", k < 0 ? '-' : '+', abs(k));
    y = y1 + NMultDiv(ys, 100-k, 200);
    DrawSz(sz, (x2+gs.xWin)/2, y+2*gi.nScaleT, dtCent);
  }
  /* Label days on top horizontal axis. */
  for (j = -BIODAYS+2; j < BIODAYS; j += 2) {
    x = x1 + NMultDiv(xs, j+BIODAYS, BIODAYS*2);
    sprintf(sz, "%c%d", j < 0 ? '-' : '+', abs(j));
    DrawSz(sz, x, y1-2*gi.nScaleT, dtBottom);
  }
  DrawEdge(x1, y1, x2, y2);
}
#endif /* BIORHYTHM */
#endif /* GRAPH */

/* xcharts2.c */
