
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#include <time.h>

#define STR_LEN     1024

char *cmd = "rasi";

extern int astromain(int targc, char *targv[]);

extern char *naksatras[];

static char *rasi[] = {
    "Mesha",    "Vrishabha",    "Midhuna",
    "Kataka",   "Simha",        "Kanya",
    "Thula",    "Vrischika",    "Dhanas",
    "Makara",   "Kumbha",       "Meena"
};

static char *mon[] = { "",
    "Jan", "Feb", "Mar",
    "Apr", "May", "Jun",
    "Jul", "Aug", "Sep",
    "Oct", "Nov", "Dec"
};

/* array of possible Alexa return values for what a user's day will look like based on moon and rasi positions */
static char *moPred[] = {
    "",
    "Good relations with women and friends, good food",
    "Tiredness and expenditure",
    "Gains of money, gold and good clothes",
    "Accidents or ill health, trouble from relatives, sorrow and expenditure",
    "Indigestion, loss of money, tiresome journeys",
    "Victory over enemies, good health, increase in income and happiness",
    "Friendship and gains, good comforts of bed, conjugal bliss and company of good friends",
    "Trouble losses and expenses, illness, quarrels and worries",
    "Loss of reputation and money, heavy expenses, intimidation and worry",
    "Good name and reputation, gains and fulfillment of desires, happiness",
    "Happiness, good friends, monetary gains, good comforts of bed and increase in respect",
    "Carelessness and losses, misery, difference of opinion and arguments with others",
};

typedef struct sHorDetails {
    char name[STR_LEN];
    char place[STR_LEN];
    char mon[STR_LEN];
    char day[STR_LEN];
    char year[STR_LEN];
    char time[STR_LEN];
    char zone[STR_LEN];
    char offset[STR_LEN];
    char lon[STR_LEN];
    char lat[STR_LEN];
    char email[STR_LEN];
    int old;
    int dst;
    float moondeg;
    int ra;
    int nak;
    char *rasi;
    char *naksatra; 
} tHorDetails;

void
printhd(tHorDetails *hd)
{
    printf("Name        : %s\n", hd->name);
    printf("Place       : %s\n", hd->place);
    printf("Month       : %s\n", hd->mon);
    printf("Date        : %s\n", hd->day);
    printf("Year        : %s\n", hd->year);
    printf("Time        : %s\n", hd->time);
    printf("Zone        : %s\n", hd->zone);
    printf("Offset      : %s\n", hd->offset);
    printf("Long        : %s\n", hd->lon);
    printf("Lat         : %s\n", hd->lat);
    printf("Dst         : %d\n", hd->dst);
    printf("\n");
}

void
horoscope(tHorDetails *hd)
{
    char str[STR_LEN];
    int fd[2];
    int targc = 0;
    char *targv[256];

    pipe(fd);

    if (0) {
        printhd(hd);
    }

    targv[targc++] = cmd;

    if (hd->old) {
        targv[targc++] = "-qa";
    } else {
        targv[targc++] = "-qb";
    }

    targv[targc++] = hd->mon;
    targv[targc++] = hd->day;
    targv[targc++] = hd->year;
    targv[targc++] = hd->time;

    targv[targc++] = hd->zone;

    if (hd->old == 0) {
        targv[targc++] = hd->offset;
    }

    targv[targc++] = hd->lon;
    targv[targc++] = hd->lat;

    targv[targc++] = "-zi";

    targv[targc++] = hd->name;
    targv[targc++] = hd->place;

    targv[targc++] = "-l";

    targv[targc++] = "-Yt";
 
    targv[targc++] = "-s";
    targv[targc++] = "0.872";

    targv[targc++] = "-b0";

    if (hd->dst) {
        targv[targc++] = "-7"; // Auto Daylight Savings
    }

    targv[targc++] = "-R0";

    targv[targc++] = "-R";
    targv[targc++] = "2";

    targv[targc++] = "-A";
    targv[targc++] = "8";

    targv[targc++] = "-!";  // Match Data

    targv[targc] = NULL;

    if (fork() == 0) {

        close(1);
        dup2(fd[1], 1);
        close(fd[0]);
        close(fd[1]);

        astromain(targc, targv);

    } else {

        FILE *pfp;
        char dummy[STR_LEN];

        close(fd[1]);

        pfp = fdopen(fd[0], "r");
        while (fgets(str, STR_LEN, pfp) != NULL) {
            int ra;
            int nak;

            sscanf(str, "%f", &hd->moondeg);

            hd->ra = (hd->moondeg / 30.0);
            hd->nak = (hd->moondeg / (360.0 / 27.0));

            hd->rasi = rasi[hd->ra];
            hd->naksatra = naksatras[hd->nak];
        }

        fclose(pfp);
    }
}

void
strip_blanks(char *name, char *s)
{
    int j;
    int k = 0;
    int len = strlen(s);

    for (j = 0; j < len; j++) {
        if (s[j] == '%') {
            j += 2;
        } else {
            name[k++] = s[j];
        }
    }
    name[k] = '\0';
}

void
gethd(char *optarg, tHorDetails *hd, tHorDetails *ht)
{
    int i;
    int imon;
    char *s;
               
    for (i = 0; ((s = strsep((char **)&optarg, "|")) != NULL); i++) {
        switch (i) {
        case 0:
            strip_blanks(hd->name, s);
            break;
        case 1:
            strip_blanks(hd->place, s);
            break;
        case 2:
            imon = atoi(s);
            if (imon > 0 && imon <= 12) {
                strcpy(hd->mon, mon[imon]);
            }
            break;
        case 3:
            strcpy(hd->day, s);
            break;
        case 4:
            strcpy(hd->year, s);
            break;
        case 5:
            strip_blanks(hd->time, s);
            break;
        case 6:
            strcpy(hd->zone, s);
            break;
        case 7:
            strcpy(hd->offset, s);
            break;
        case 8:
            strcpy(hd->lon, s);
            break;
        case 9:
            strcpy(hd->lat, s);
            break;
        case 10:
            strcpy(ht->place, s);
            break;
        case 11:
            strcpy(ht->lon, s);
            break;
        case 12:
            strcpy(ht->lat, s);
            break;
        case 13:
            break;
        }
    }

    if (strcmp(hd->zone, "AD") == 0) {
        hd->dst = 1;
        strcpy(hd->zone, "ST");
    } else if (strcmp(hd->zone, "ST") != 0) {
        hd->dst = 1;
        strcpy(hd->lon, hd->lat);
        strcpy(hd->lat, hd->offset);
        hd->offset[0] = '\0';
    }
}

/* findDiff function updated with correct formula
 * Logic: -if position one is greater than position two,
 *              then the difference is the number of spaces from
 *              position one to the base value of 12 added to the
 *              second position (1 is added due to 0 based indexing) 
 *        -if position two is greater than position one the value is
 *              simply the absolute value of position one minus position 2
 *              (1 is added for same reason of 0 based indexing) 
  Conditions: if position one and position two are equal,
                   the formula will return '13' example of 6 and 6: ((12-6)+6+1),
 *                  however this is a special corner case and the value should be one
 */
int
findDiff(int pos1, int pos2, int base)
{
    int diff;

    if (pos1 >= pos2) {
        diff = base - pos1 + pos2 + 1;

        if (diff == 13) {
            diff = 1;
        }
    } else {
        diff = abs(pos1 - pos2) + 1;
    }

    return diff;
}

void
transit(tHorDetails *hd)
{
    time_t clock;
    struct tm *ctm;

    clock = time(&clock);
    ctm = localtime(&clock);

    strcpy(hd->name, "Transit");
    strcpy(hd->mon, mon[ctm->tm_mon + 1]);
    sprintf(hd->day, "%d", ctm->tm_mday);
    sprintf(hd->year, "%d", ctm->tm_year + 1900);
    sprintf(hd->time, "%d:%d", ctm->tm_hour, ctm->tm_min);
    strcpy(hd->zone, "ST"); 
    sprintf(hd->offset, "8:00");

    horoscope(hd);
}

void
star()
{
    tHorDetails hd, ht;
    FILE *wfp = stdout;
    char str[STR_LEN];
    char *qs;
    char *s;
    int flag = 0;
    int i;
    int j;
    int k;
    int len;

    fprintf(wfp, "Content-Type: application/json;charset=UTF-8\n\n");
    fflush(wfp);

    qs = getenv("QUERY_STRING");

    if (qs == NULL) {
        printf("Invalid QUERY_STRING\n");
        exit(0);
    }

    len = strlen(qs);

    if (0) {
        printf("%d:%s\n", len, qs);
    }

    memset(&hd, 0, sizeof(tHorDetails));

    for (i = 0; ((s = strsep((char **)&qs, "=")) != NULL); i++) {
        if (i == 1) {
            len = strlen(s);

            for (j = 0, k = 0; j < len; j++) {
                if (s[j] == '%' && s[j + 1] == '7' && s[j + 2] == 'C') {
                    str[k++] = '|';
                    j += 2;
                } else if (s[j] == '%' && s[j + 1] == '3' && s[j + 2] == 'A') {
                    str[k++] = ':';
                    j += 2;
                } else if (s[j] == '+') {
                    str[k++] = ' ';
                } else {
                    str[k++] = s[j];
                }
            }

            gethd(str, &hd, &ht);
            if (0) {
                printhd(&hd);
            }
            flag = 1;
            break;
        }
    }
    if (flag) {
        int diff;

        horoscope(&hd);
        printf("Star: %s Rasi: %s %d\n", hd.naksatra, hd.rasi, hd.ra);
        transit(&ht);
        if (1) {
            printhd(&ht);
        }
        printf("Transit Star: %s Rasi: %s %d\n", ht.naksatra, ht.rasi, ht.ra);

        diff = findDiff(hd.ra, ht.ra, 12);

        printf("diff %d %s\n", diff, moPred[diff]);
    }
}

int
main(int argc, char *argv[])
{
    char *base = basename(argv[0]);

    if (strcmp(base, "rasi") == 0) {
        star();
    }
    
    exit(0);
}
