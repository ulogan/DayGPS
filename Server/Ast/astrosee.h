/*
 * astrosee.h
 */

#ifndef ASTROSEE_H
#define ASTROSEE_H

// day starts at 1
#define DAYS_IN_MONTH   32

#define MINS_IN_DAY     1440

enum {
    eSun,
    eMoo,
    eMer,
    eVen,
    eMar,
    eJup,
    eSat,
    eRah,
    eKet,
    eLag,
    eLagc,
    eLags,
    eMaxPla
};

enum {
    eAri,
    eTau,
    eGem,
    eCan,
    eLeo,
    eVir,
    eLib,
    eSco,
    eSag,
    eCap,
    eAqu,
    ePis
};

typedef struct {
    double deg;
    double ret;
    int    rasi;
    int    nak;
    int    amsa;
} tplnData;

extern tplnData pln[DAYS_IN_MONTH][MINS_IN_DAY][eMaxPla];
extern tplnData dpln[eMaxPla];

extern char *naksatras[];
extern char *sign[];
extern char *splanet[];
extern char *mon[];

extern void astromain(int, char **);
extern int DayInMonth(int, int);
extern char *SzZodiac(double);

#endif /* ASTROSEE_H */


