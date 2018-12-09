/*
# 
# @(#)desa.c    1.5 97/05/08 14:17:43
# 
# File:                 desa.c     
# Last modified:        05/08/97 14:17:43
# 
*/

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>

#define THURSDAY    4   /* for reformation */
#define SATURDAY    6   /* 1 Jan 1 was a Saturday */

#define FIRST_MISSING_DAY   639787    /* 3 Sep 1752 */
#define NUMBER_MISSING_DAYS   11    /* 11 day correction */

#define SUN         0
#define MOON        1
#define MARS        2
#define RAHU        3
#define JUPITER     4
#define SATURN      5
#define MERCURY     6
#define KETU        7
#define VENUS       8

long Dasa[9];

char Rasi[13][128];
char Navamsa[13][128];

char *Pla[12] = {
  "SUN", "MOON", "MARS", "RAHU", "JUPITER", 
  "SATURN", "MERCURY", "KETHU", "VENUS",
  "LAGNA", "VAKRA", "NIL"
};

struct PerRelation {
  int Friend[9];
  int Neutral[9];
  int Enemy[9];
}PerPlanet[9];

struct ActRelation {
  int IntFriend[9];
  int Friend[9];
  int Neutral[9];
  int Enemy[9];
  int BitEnemy[9];
}ActPlanet[9];

static int days_in_month[2][13] = {
  {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

char *month_names[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

char *week_names[7] = {
  "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

/* leap year -- account for gregorian reformation in 1752 */
#define leap_year(yr) \
  ((yr) <= 1752 ? !((yr) % 4) : \
  !((yr) % 4) && ((yr) % 100) || !((yr) % 400))

/* number of centuries since 1700, not inclusive */
#define centuries_since_1700(yr) \
  ((yr) > 1700 ? (yr) / 100 - 17 : 0)

/* number of centuries since 1700 whose modulo of 400 is 0 */
#define quad_centuries_since_1700(yr) \
  ((yr) > 1600 ? ((yr) - 1600) / 400 : 0)

/* number of leap years between year 1 and this year, not inclusive */
#define leap_years_since_year_1(yr) \
  ((yr) / 4 - centuries_since_1700(yr) + quad_centuries_since_1700(yr))

static int b_days_passed;
static int pb_days_passed;
static int a_days_passed;
static int d_days_passed;
static int pa_days_passed;
static int gflag;
static int gflag1;
static int lflag;
static int lflag1;
static int adj;
static int adj1;
static int req;
static int req1;
int antarams;
int balance;
int html;

DasaMain(char *name, int day, int month, int year, int ruler, int b_day, int b_month, int b_year)
{
    int year_day;

    InitDasa();
    year_day = DayInYear(day, month, year);
    balance = ((b_year * 12 * 30) + (b_month * 30) + b_day);
    antarams = 0;
    PrintDasa(name, year, year_day, ruler);
    antarams = 1;
    PrintDasa(name, year, year_day, ruler);
}

#ifdef DESA_MAIN
main(int argc, char *argv[])
{
    int year_day;
    int day;
    int month;
    int year;
    int b_day;
    int b_month;
    int b_year;
    int desa;

    if (argc == 1) {
        fprintf(stderr, "Usage %s filename\n", argv[0]);
        exit(1);
    }

    ReadFile(argv[1], &day, &month, &year, &desa, &b_day, &b_month, &b_year);
    DasaMain(argv[1], day, month, year, desa, b_day, b_month, b_year); 
    RelMain(argv[1]); 
}
#endif

ReadFile(char *fname, int *day, int *month, int *year, int *desa, 
        int *b_day, int *b_month, int *b_year)
{
    int i;
    char file_name[128];
    FILE *fp1;

    sprintf(file_name, "Data/%s", fname);
    if ((fp1 = fopen(file_name, "r")) == NULL) {
        fprintf(stderr, "Unable to open file %s for reading\n", file_name);
        exit(1);
    }
    fscanf(fp1, "%d", day);
    fscanf(fp1, "%d", month);
    fscanf(fp1, "%d", year);
    fscanf(fp1, "%d", desa);
    fscanf(fp1, "%d", b_year);
    fscanf(fp1, "%d", b_month);
    fscanf(fp1, "%d", b_day);
    for (i = 1; i < 13; i++) {
        fscanf(fp1, "%s", Rasi[i]);
    }
    for (i = 1; i < 13; i++) {
        fscanf(fp1, "%s", Navamsa[i]);
    }
    fclose(fp1);
}

RelMain(char *fname)
{
    char file_name[128];
    FILE *fp1;

    sprintf(file_name, "Data/%s.rel", fname);
    if ((fp1 = fopen(file_name, "w")) == NULL) {
        fprintf(stderr, "Unable to open file %s for writing\n", file_name);
        exit(1);
    }
    IntPerRelation();
    PrintRelation(fp1, 1, Rasi);
    PrintRelation(fp1, 2, Navamsa);
    fclose(fp1);
}

InitDasa(void)
{
    Dasa[SUN] = 6;
    Dasa[MOON] = 10;
    Dasa[MARS] = 7;
    Dasa[RAHU] = 18;
    Dasa[JUPITER] = 16;
    Dasa[SATURN] = 19;
    Dasa[MERCURY] = 17;
    Dasa[KETU] = 7;
    Dasa[VENUS] = 20;
}

PrintDasa(char *fname, int year, int dob, int planet)
{
    char name[128];
    char file_name[128];
    int t_planet;
    int t_bhuktis;
    int t_antarams;
    int i;
    int j;
    int k;
    int mod;
    int div;
    int b_days;
    int b_months;
    int b_years;
    int b_temp;
    int b_total;
    int a_days;
    int a_months;
    int a_years;
    int a_temp;
    int a_total;
    int temp;
    long tm;
    char *tmp;
    FILE *fp1;

    time(&tm);
    if (antarams == 0) {
        tmp = "Bhu";
    } else {
        tmp = "Ant";
    }
    if (strcmp(fname, "html") == 0) {
        html = 1;
        fp1 = stdout;
    } else {
        sprintf(file_name, "Data/Dasa/%s%s", basename(fname), tmp);
        if ((fp1 = fopen(file_name, "w")) == NULL) {
            fprintf(stderr, "Unable to open file %s for writing\n", file_name);
            exit(1);
        }
        fprintf(fp1, "For %s Executed on %s\n", basename(fname), ctime(&tm));
    }
    if (antarams ==  0) {
        if (html) {
            fprintf(fp1, "%-20s -  %2s %2s %2s", "Dasa/Bhukthi", "Yr", "Mo", "Da");
        } else {
            fprintf(fp1, "%-28s -  %2s %2s %2s", "Dasa/Bhukthi", "Yr", "Mo", "Da");
        }
    } else {
        if (html) {
            fprintf(fp1, "%-20s -  %2s %2s %2s", "Dasa/Bhukthi/Antaram", 
                    "Yr", "Mo", "Da");
        } else {
            fprintf(fp1, "%-28s -  %2s %2s %2s", "Dasa/Bhukthi/Antaram", 
                    "Yr", "Mo", "Da");
        }
    }
    if (html) {
        fprintf(fp1, " %-15s   \n", "Starting From");
    } else {
        fprintf(fp1, "         %-15s   \n", "Starting From");
    }
    t_planet = 0;
    b_days_passed = 0;
    pb_days_passed = 0;
    gflag = 0;
    gflag1 = 0;
    for (i = planet; t_planet < 9; t_planet++) {

        a_days_passed = 0;
        d_days_passed = 0;
        pa_days_passed = 0;
        lflag1 = 0;

        PrintName(fp1, 1, i, name, 0, 0, Dasa[i], year, dob, 1);

        t_bhuktis = 0;
        b_total = 0;
        temp = Dasa[i] * 12 * 30;

        for (j = i; t_bhuktis < 9; t_bhuktis++) {

            a_days_passed = 0;
            pa_days_passed = 0;
            lflag = 0;
            b_temp = temp * Dasa[j] / 120;
            DasaYearDayToDate(b_temp, &b_days, &b_months, &b_years);
            if (i == planet) {
                if ((gflag != 1) && ((b_total + b_temp) < (temp - balance))) {
                    b_total += b_temp;
                } else {
                    if (gflag == 0) {
                        gflag = 1;
                        lflag1 = 1;
                        adj = ((b_total + b_temp) - (temp - balance));
                        req = (temp - balance) - b_total;
                        PrintName(fp1, 2, j, name, b_days, b_months, b_years, 
                                year, dob, 2);
                    } else {
                        PrintName(fp1, 2, j, name, b_days, b_months, b_years, 
                                year, dob, 0);
                    }
                }
            } else {
                PrintName(fp1, 2, j, name, b_days, b_months, b_years, 
                        year, dob, 0);
            }
            if (gflag == 1) {
                if (antarams) {
                    t_antarams = 0;
                    a_total = 0;
                    for (k = j; t_antarams < 9; t_antarams++) {
                        a_temp = b_temp * Dasa[k] / 120;
                        DasaYearDayToDate(a_temp, &a_days, &a_months, &a_years);
                        if (i == planet) {
                            if ((gflag1 != 1) && ((a_total + a_temp) < (b_temp - adj))) {
                                a_total += a_temp;
                            } else {
                                if (gflag1 == 0) {
                                    gflag1 = 1;
                                    lflag = 1;
                                    adj1 = ((a_total + a_temp) - (b_temp - adj));
                                    req1 = (b_temp - adj) - a_total;
                                    PrintName(fp1, 3, k, name, a_days, a_months,
                                            a_years, year, dob, 3);
                                } else {
                                    PrintName(fp1, 3, k, name, a_days, a_months, 
                                            a_years, year, dob, 0);
                                }
                            }
                        } else {
                            PrintName(fp1, 3, k, name, a_days, a_months, a_years, 
                                    year, dob, 0);
                        }
                        pa_days_passed = a_days_passed;
                        if (k == 8) {
                            k = 0;
                        } else {
                            k++;
                        }
                    }
                    fprintf(fp1, "\n");
                }
                pb_days_passed = b_days_passed;
            }
            if (j == 8) {
                j = 0;
            } else {
                j++;
            }
        }
        if (i == 8) {
            i = 0;
        } else {
            i++;
        }
    }
    fclose(fp1);
}

PrintName(FILE *fp1, int flag, int planet, char *name, int days, int months, int years, int born_year, int dob, int flag1)
{
    char str[128];
    int dasa_day;
    int dasa_month;
    int dasa_year;
    int week_day;
    int days_in_year;
    int temp;
    int logan;

    switch(planet) {
    case SUN:
        strcpy(str, "Sun's");
        break;
    case MOON:
        strcpy(str, "Moon's");
        break;
    case MARS:
        strcpy(str, "Mars's");
        break;
    case RAHU:
        strcpy(str, "Rahu's");
        break;
    case JUPITER:
        strcpy(str, "Jupiter's");
        break;
    case SATURN:
        strcpy(str, "Saturn's");
        break;
    case MERCURY:
        strcpy(str, "Mercury's");
        break;
    case KETU:
        strcpy(str, "Ketu's");
        break;
    case VENUS:
        strcpy(str, "Venus's");
        break;
    }

    days_in_year = ((years * 12 * 30) + (months * 30) + (days));

    if (flag == 1) {
        sprintf(name, "%s Dasa", str);
    } else if (flag == 2) {
        sprintf(name, " %s Bhuktis", str);
        b_days_passed += days_in_year;
        d_days_passed += days_in_year;
    } else if (flag == 3) {
        sprintf(name, "  %s Antarams", str);
        a_days_passed += days_in_year;
    }

    if (flag1 == 0) {
        if (pb_days_passed == 0)
            YearDayToDate(born_year, dob + pa_days_passed - req1, 
                    &dasa_day, &dasa_month, &dasa_year);
        else
            YearDayToDate(born_year, dob + pb_days_passed + pa_days_passed - req, 
                    &dasa_day, &dasa_month, &dasa_year);
    } else if (flag1 == 1) {
        if (gflag == 0) {
            YearDayToDate(born_year, dob, &dasa_day, &dasa_month, &dasa_year);
            DasaYearDayToDate(balance, &days, &months, &years);
        } else {
            YearDayToDate(born_year, dob + pb_days_passed + pa_days_passed - req, 
                    &dasa_day, &dasa_month, &dasa_year);
        }
    } else if (flag1 == 2) {
        YearDayToDate(born_year, dob, &dasa_day, &dasa_month, &dasa_year);
        DasaYearDayToDate(adj, &days, &months, &years);
    } else if (flag1 == 3) {
        YearDayToDate(born_year, dob, &dasa_day, &dasa_month, &dasa_year);
        DasaYearDayToDate(adj1, &days, &months, &years);
    }
#ifdef DEBUG
    fprintf(stderr, "dob = %d, pb_days_passed = %d, pa_days_passed = %d,"
                    "adj = %d, adj1 = %d, req = %d, req1 = %d\n", 
                dob, pb_days_passed, pa_days_passed, adj, adj1, req, req1);
#endif
    if (antarams == 1) {
        if (lflag == 1) {
            logan = a_days_passed - req1;
        } else {
            logan = a_days_passed;
        }
    } else {
        if (lflag1 == 1) {
            logan = d_days_passed - req;
        } else {
            logan = d_days_passed;
        }
    }
    week_day = DayInWeek(dasa_day, dasa_month, dasa_year);
    if (flag == 1) {
        fprintf(fp1, "\n");
    }
    if (html) {
        fprintf(fp1, "%-20s - %2d %2d %2d", name, years, months, days);
        fprintf(fp1, "   %3s %2d %2d\n", month_names[dasa_month - 1], 
                dasa_day, dasa_year);
    } else {
        fprintf(fp1, "%-28s -  %2d %2d %2d %5d", name, years, months, days,
                ((years * 360) + (months * 30) + days));
        fprintf(fp1, "   %3s %2d %2d  %-5s %5d\n", month_names[dasa_month - 1], 
                dasa_day, dasa_year, week_names[week_day], logan);
    }
}

/*
 * DayInYear --
 *  return the 1 based day number within the year
 */
DayInYear(int day, int month, int year)
{
    int i, leap;

    leap = leap_year(year);
    for (i = 1; i < month; i++) {
        day += days_in_month[leap][i];
    }
    return (day);
}

/*
 * Dasa YearDayToDate --
 *
 */
DasaYearDayToDate(int year_day, int *days, int *months, int *years)
{
    int temp;

    *days = year_day % 30;
    temp = year_day / 30;
    *months = temp % 12;
    *years = temp / 12;
    temp = ((*years * 12 * 30) + (*months * 30) + *days);
    if (temp != year_day) {
        fprintf(stderr, " *** Temp = %d, year_day = %d\n", temp, year_day);
    }
}

/*
 * YearDayToDate --
 *  return the 1 based day number within the year
 */
YearDayToDate(int year_year, int year_day, int *day, int *month, int *year)
{
    int i, leap;
    int days;

    while (1) {
        leap = leap_year(year_year);
        days = 0;
        for (i = 1; i < 13; i++) {
            days += days_in_month[leap][i];
            if (year_day <= days) {
                days = days_in_month[leap][i] - (days - year_day);
                break;
            }
        }
        if (i == 13) {
            year_year++;
            year_day = year_day - days;
        } else {
            break;
        }
    }
    *day = days;
    *month = i;
    *year = year_year;
    return (1);
}

/*
 * DayInWeek
 *  return the 0 based day number for any date from 1 Jan. 1 to
 *  31 Dec. 9999.  Assumes the Gregorian reformation eliminates
 *  3 Sep. 1752 through 13 Sep. 1752.  Returns Thursday for all
 *  missing days.
 */
DayInWeek(int day, int month, int year)
{
    long temp;

    temp = (long)(year - 1) * 365 + leap_years_since_year_1(year - 1) 
        + DayInYear(day, month, year);

    if (temp < FIRST_MISSING_DAY) {
        return ((temp - 1 + SATURDAY) % 7);
    }

    if (temp >= (FIRST_MISSING_DAY + NUMBER_MISSING_DAYS)) {
        return (((temp - 1 + SATURDAY) - NUMBER_MISSING_DAYS) % 7);
    }

    return (THURSDAY);
}

PrintRelation(FILE *fp1, int rasi, char tRasi[13][128])
{
    int i;
    int j;
    int ind1;
    int ind2;
    int diff;

    if (rasi == 1) {
        fprintf(fp1, "\nRasi Distance\n\n");
    } else {
        fprintf(fp1, "\nNavamsa Distance\n\n");
    }

    ConvertUpper(tRasi);

    for (i = 0; i < 9; i++) {
        if (i == RAHU || i == KETU) {
            continue;
        }
        ind1 = RasiIndex(Pla[i], tRasi);
        if (ind1 == -1) {
            continue;
        }
        for (j = 0; j < 9; j++) {
            if (j == RAHU || j == KETU) {
                continue;
            }
            ind2 = RasiIndex(Pla[j], tRasi);
            if (ind2 == -1) {
                continue;
            }

            diff = distance(ind1, ind2);
            fprintf(fp1, "%-8s - %-8s (%2d, %2d) Dis: %2d - ",
                    Pla[i], Pla[j], ind1, ind2, diff);

            if (IsFriend(i, j) == 1) {
                if (diff == 2 || diff == 3 || diff == 4 || diff == 10 ||
                        diff == 11 || diff == 12) {
                    fprintf(fp1, "Int Friend (Friend + Friend)\n");
                } else {
                    fprintf(fp1, "Netural (Friend + Enemy)\n");
                }
            } else if (IsNeutral(i, j) == 1) {
                if (diff == 2 || diff == 3 || diff == 4 || diff == 10 || 
                        diff == 11 || diff == 12) {
                    fprintf(fp1, "Friend (Netural + Friend)\n");
                } else {
                    fprintf(fp1, "Enemy (Netural + Enemy)\n");
                }
            } else if (IsEnemy(i, j) == 1) {
                if (diff == 2 || diff == 3 || diff == 4 || diff == 10 || 
                        diff == 11 || diff == 12) {
                    fprintf(fp1, "Netural (Enemy + Friend)\n");
                } else {
                    fprintf(fp1, "Bitter Enemy (Enemy + Enemy)\n");
                }
            } else {
                fprintf(fp1, "Default Enemy\n");
            }
        }
    }
}

RasiIndex(char *planet, char tRasi[13][128])
{
    int i;

    for (i = 1; i < 13; i++) {
        if (strstr(tRasi[i], planet) != NULL)
            return(i);
    }
    return(-1);
}

ConvertUpper(char tRasi[13][128])
{
    int i;

    for (i = 1; i < 13; i++) {
        upper(tRasi[i]);
    }
}

upper(char *str)
{
    int i;

    i = 0;
    while (str[i]) {
        if (islower(str[i])) {
            str[i] = toupper(str[i]);
        }
        i++;
    }
}

IntPerRelation(void)
{
    int i;
    int j;

    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            PerPlanet[i].Friend[j] = -1;
            PerPlanet[i].Neutral[j] = -1;
            PerPlanet[i].Enemy[j] = -1;
        }
    }

    /* For SUN */
    PerPlanet[SUN].Friend[MOON] = 1;
    PerPlanet[SUN].Friend[MARS] = 1;
    PerPlanet[SUN].Friend[JUPITER] = 1;

    PerPlanet[SUN].Neutral[MERCURY] = 1;

    PerPlanet[SUN].Enemy[SATURN] = 1;
    PerPlanet[SUN].Enemy[VENUS] = 1;

    /* For MOON */
    PerPlanet[MOON].Friend[SUN] = 1;
    PerPlanet[MOON].Friend[MERCURY] = 1;

    PerPlanet[MOON].Neutral[MARS] = 1;
    PerPlanet[MOON].Neutral[JUPITER] = 1;
    PerPlanet[MOON].Neutral[SATURN] = 1;
    PerPlanet[MOON].Neutral[VENUS] = 1;

    /* For MARS */
    PerPlanet[MARS].Friend[JUPITER] = 1;
    PerPlanet[MARS].Friend[MOON] = 1;
    PerPlanet[MARS].Friend[SUN] = 1;

    PerPlanet[MARS].Neutral[SATURN] = 1;
    PerPlanet[MARS].Neutral[VENUS] = 1;

    PerPlanet[MARS].Enemy[MERCURY] = 1;

    /* For MERCURY */
    PerPlanet[MERCURY].Friend[SUN] = 1;
    PerPlanet[MERCURY].Friend[VENUS] = 1;

    PerPlanet[MERCURY].Neutral[SATURN] = 1;
    PerPlanet[MERCURY].Neutral[MARS] = 1;
    PerPlanet[MERCURY].Neutral[JUPITER] = 1;

    PerPlanet[MERCURY].Enemy[MOON] = 1;

    /* For JUPITER */
    PerPlanet[JUPITER].Friend[SUN] = 1;
    PerPlanet[JUPITER].Friend[MOON] = 1;
    PerPlanet[JUPITER].Friend[MARS] = 1;

    PerPlanet[JUPITER].Neutral[SATURN] = 1;

    PerPlanet[JUPITER].Enemy[MERCURY] = 1;
    PerPlanet[JUPITER].Enemy[VENUS] = 1;

    /* For VENUS */
    PerPlanet[VENUS].Friend[MERCURY] = 1;
    PerPlanet[VENUS].Friend[SATURN] = 1;

    PerPlanet[VENUS].Neutral[MARS] = 1;
    PerPlanet[VENUS].Neutral[JUPITER] = 1;

    PerPlanet[VENUS].Enemy[MOON] = 1;
    PerPlanet[VENUS].Enemy[SUN] = 1;

    /* For SATURN */
    PerPlanet[SATURN].Friend[VENUS] = 1;
    PerPlanet[SATURN].Friend[MERCURY] = 1;

    PerPlanet[SATURN].Neutral[JUPITER] = 1;

    PerPlanet[SATURN].Enemy[MARS] = 1;
    PerPlanet[SATURN].Enemy[MOON] = 1;
    PerPlanet[SATURN].Enemy[SUN] = 1;
}

IsFriend(int i, int j)
{
    if (PerPlanet[i].Friend[j] == 1) {
        return(1);
    } else {
        return(-1);
    }
}

IsNeutral(int i, int j)
{
    if (PerPlanet[i].Neutral[j] == 1) {
        return(1);
    } else {
        return(-1);
    }
}

IsEnemy(int i, int j)
{
    if (PerPlanet[i].Enemy[j] == 1) {
        return(1);
    } else {
        return(-1);
    }
}

distance(int ind1, int ind2)
{
    int i;
    int temp;

    temp = ind1;
    for (i = 0; i < 12; i++) {
        if (temp == ind2) {
            return(i + 1);
        }
        temp++;
        if (temp == 13)
            temp = 1;
    }
}
