#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <time.h>

#define STR_LEN     1024

static int verbose = 0;
char file_name[128];

extern char *star_table[36];
extern char *stable[36];
extern char *rtable[36];
extern int table_lookup(int, int);
extern void star_lookup(int, int, int *);

/*
 *
 * Notes
 *
 * Dina Kuta: (couple's health and family happiness is indicated) 3 points or 1 1/2 points 
 *      The Nakshatra of the bridegroom is counted from that of the bride or vice-versa 
 *          then divded by the number 9. If the remainder is 1, 3, 5, 7 then no points are given.
 *          If from one side it is favourable and from the other side unfavourable, then 1 1/2 points 
 *          are given. Count the constellation of the boy from that of the girl and divide the number by 9 
 *
 *      1. Janma - similar thoughts, good mental connection can be harmed due to lack of balance in 
 *                  emotions as both can react similarly to pressures
 *
 *      2. Sampat- means wealthy so you will emotionally support each other the relationship will 
 *                  creative and enriching
 *
 *      3. Vipat - blocked. this is the difficult relationships where two minds do not understand 
 *                  each other and are not able to give emotional support the other wants
 *
 *      4. Kshema - gives happiness and fulfillment
 *
 *      5. Prayatak - creates obstacles, the minds lack harmony and can be obstructive towards each other
 *
 *      6. Sadhana - Spiritual harmony and the ability to gel together on a deeply spiritual level
 *
 *      7. Naidhana - means povery. this creates a poor connection, no fertility of ideas and emotions
 *                  that ties people together 
 *
 *      8. Mitra - means friend. great nakshatra to have as you mentally connect and give friendly 
 *                  vibes to each other 
 *
 *      9. Param mitra - means great friends. This is a great combination as both people feel at 
 *                  ease with each. one picks up what the other says, there is true harmon
 *
 * Gana Kuta: (Temperament and Nature) 6 points
 *
 *      Devas (Sattva) - Ashwini, Mrigshira, Punarvasu, Pushya, Hasta, Swati, Anuradha, Sravana and Revathi.
 *                  They are kind, soft and sensitive. Steady, calm, settled, puritan and followers of dharma. 
 *
 *      Manushya (Rajas) - Bharini, Rohini, Aridra, Poorvaphalguni, Uttaraphalguni, Poorvashada, 
 *          Uttarashada, Poorvabhadrapada and Uttarabhadrapada.
 *                  They are imperfect mostly trying to fulfill their ambitions, they strugle with their 
 *                  weaknesses and moving between fine and harsh natures. Dynamic, active, ambitious, 
 *                  searching for answers inlife. 
 *
 *      Rakshasha (Tamas) - Krittika, Ashlesha, Magha, Chitra, Vishakha, Jyestha, Moola, 
 *          Dhanishta and Shatabisha.
 *                  They are only concerned with their own needs, never caring for others.
 *                  They are highly intelligent, will be self serving, ambitious and agressive
 *
 *      Gana           Deva       Nara       Rakshasa
 *      Deva            6          6            1
 *      Nara            5          6            0
 *      Rakshasa        1          0            6
 *
 * Mahendra: (well-being, children and longevity) 2 points (*, Non scoring)
 *
 *      The nakshatra of the bridegroom, counted from that of the bride, should be 4, 7, 10, 13, 16, 
 *      19, 22 or 25
 *
 * Stree-Deergha: (for long marriage, good companionship and woman's happiness) 2 points (*, Non scoring)
 *
 *      If the nakshatra of the bridegroom, counted from that of the bride, is between 13 to 27, then 2 points
 *      If it is between 8 to 12, one point
 *      If it is between 0 and 7, no points
 *
 * Yoni Kuta: (Sexual compatibility) 4 points
 *
 *    Animal   Male            Female
 * 0. Horse    Ashwini         Shatabhishak
 * 1. Elephant Bharani         Revati
 * 2. Sheep    Pushya          Krittika
 * 3. Serpent  Rohini          Mrigasira
 * 4. Dog      Mula            Ardra
 * 5. Cat      Aslesha         Punarvasu
 * 6. Rat      Magha           Puram
 * 7. Cow      Uttaram         Utartathi
 * 8. Buffalo  Swati           Hasta
 * 9. Tiger    Visakha         Chitta
 *10. Deer     Jyeshta         Anuradha
 *11. Monkey   Puradam         Shravana
 *12. Lion     Puratathi       Avitam   
 *13. Mongoose Uttaradam*
 *  
 * Graha Maitri: (Friendship between Rashis)
 *
 *    Planet      Friends               Neutral                         Enemy
 *    Sun         Jupiter, Mars, Moon   Mercury                         Saturn, Venus
 *    Moon        Sun, Mercury          Mars, Jupiter, Venus, Saturn
 *    Mars        Sun, Jupiter, Moon    Saturn, Venus                   Mercury
 *    Mercury     Sun, Venus            Mars, Jupiter, Saturn           Moon
 *    Jupiter     Sun, Moon , Mars      Saturn                          Mercury, Venus
 *    Venus       Mercury, Saturn       Mars, Jupiter                   Sun, Moon
 *    Saturn      Mercury, Venus        Jupiter                         Sun, Moon, Mars
 *
 *    If the male and female rashis are friends or same then 5 gunas are given.
 *    If one is friendly and the other neutral - 4 gunas.
 *    Both are neutral- 3 gunas.
 *    One friend, other enemy- 2 gunas,
 *    One neutral, other enemy - 0 gunas
 *
 * Rajju Kuta - Invisible connections
 *
 *  Pada - feet (Mercury and Ketu ruled nakshatras)
 *      Ashwini, Ashlesha, Magha, Jyeshta, Mula, Revati. 
 *          This is connected to stability in relationships. If partners share this rajju, this brings about 
 *          a lack of stability, the tendency to drift apart. To travel away from each other.
 *
 *  Kati - waist (Venus and Saturn ruled nakshatras)
 *      Bharani, Pushya, Purva Phalguni, Anuradha, Purva Ashadha, Uttara Bhadrapada.
 *          This is connected to emotional and sexual desires and related to lack of sexual
 *          compatibility. There can be frustration as the mind thinks that their sexual desires 
 *          are not being met.
 *
 *  Nabhi - stomach (Sun and Jupiter ruled nakshatras)
 *      Krittika, Punarvasu, Uttara Phalguni, Vishakha, Uttara Ashadha, Purva Bhadrapada 
 *          Courage, confidence, security are the issues for this rajju. Couple sharing it will feel
 *          anxious, out of control. The desires can become overpowering and they are unable to deal with them
 *
 *  Kantha - neck (Moon and Rahu ruled nakshatras)
 *      Rohini, Ardra, Hasta, Swati, Shravana, Shatabhishak
 *          Communication, shared interest and fears are the issues of this rajju. Couples sharing 
 *          this can lose their communication skills and become aggravated with each other.
 *
 *  Sira - head ((Mars ruled nakshatras)
 *      Dhanishta, Chitra, Mrigasira.
 *          This deals with anger, mental control and motivations. As this deals with the head,
 *          emotions can split the couple apart unless they become extremely sensitive to the 
 *          others feelings. This can lead to unleashing of anger, bitterness and even violence.
 *
 *  Vedha - relationships that cause pain (Affliction)
 *  The following pairs of Nakshatras cause Vedha to each other
 *      Ashwini and Jyeshta
 *      Bharani and Anuradha
 *      Krittika and Vishkha
 *      Rohini and Swati
 *      Ardra and Shravana
 *      Punarvasu and Uttara Ashadha
 *      Pushya and Purva Ashadha
 *      Ashlesha and Mula
 *      Magha and Revati
 *      Purva Phalguni and Uttara Bhadra
 *      Uttara Phalguni and Purva Bhadra
 *      Hasta and Shatbhishak
 *      Mrigasira, Chitra and Dhanishta cause Vedha to each other.
 *
 */

char *helpnak[] = { 
    "Aswini         Aswini              0",
    "Bharani        Bharani             1",      
    "Krithikai      Krittika            2",
    "Rohini         Rohini              3",
    "Mrigashir      Mrigasira           4",
    "Thiruvadu      Ardra               5",
    "Punrpusam      Punarvasu           6",
    "Pusam          Pushya              7",
    "Ayilyam        Ashlesha            8",
    "Magham         Magha               9",
    "Puram          Purva Phalguni      10",
    "Uttaram        Uttara Phalguni     11",
    "Hastam         Hasta               12",
    "Chitrai        Chitra              13",
    "Swati          Swati               14",
    "Vishakham      Vishkha             15",
    "Anusham        Anuradha            16",
    "Kettai         Jyeshta             17",
    "Mulam          Mula                18",
    "Puradam        Purva Ashadha       19",
    "Uttaradam      Uttara Ashadha      20",
    "Tiruvonam      Shravana            21",
    "Avitam         Dhanishta           22",
    "Sathayam       Shatbhishak         23",
    "Puratathi      Purva Bhadra        24",
    "Utartathi      Uttara Bhadra       25",
    "Revati         Revatii             26"
}; 

char *helprasi[] = { 
    "Mesha          Aires                0",
    "Vrishabha      Taurus               1",      
    "Midhuna        Gemini               2",
    "Kataka         Cancer               3",
    "Simha          Leo                  4",
    "Kanya          Virgo                5",
    "Thula          Libra                6",
    "Vrischika      Scorpio              7",
    "Dhanas         Sagittarius          8",
    "Makara         Capriocron           9",
    "Kumbham        Aquarius            10",
    "Meena          Pisces              11",
};

typedef struct sMatchTable {
    int nak;
    int ra;
} tMatchTable;

tMatchTable matchTable[36] = {
    {  0,  0 }, {  1,  0 }, {  2,  0 },
    {  2,  1 }, {  3,  1 }, {  4,  1 },
    {  4,  2 }, {  5,  2 }, {  6,  2 },
    {  6,  3 }, {  7,  3 }, {  8,  3 },
    {  9,  4 }, { 10,  4 }, { 11,  4 },
    { 11,  5 }, { 12,  5 }, { 13,  5 },
    { 13,  6 }, { 14,  6 }, { 15,  6 },
    { 15,  7 }, { 16,  7 }, { 17,  7 },
    { 18,  8 }, { 19,  8 }, { 20,  8 },
    { 20,  9 }, { 21,  9 }, { 22,  9 },
    { 22, 10 }, { 23, 10 }, { 24, 10 },
    { 24, 11 }, { 25, 11 }, { 26, 11 }
};
 
char *cmd = "match";

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
    int disp;
    float moondeg;
    int ra;
    char *rasi;
    char *rasilord;
    int rasidiff;
    int nak;
    char *naksatra; 
    int nakdiff;
    int varna;
    char *cvarna;
    int vashya;
    char *cvashya;
    char *cdina;
    int yoni;
    char *cyoni;
    int gana;
    char *cgana;
    int nadi;
    char *cnadi;
    int rajju;
    char *crajju;
} tHorDetails;

typedef struct sStarList {
    int si;
    int mv;
} __attribute__((packed)) tStarList;

tStarList sl[36];

extern char *naksatras[];

static char *rasi[] = {
    "Mesha",    "Vrishabha",    "Midhuna",
    "Kataka",   "Simha",        "Kanya",
    "Thula",    "Vrischika",    "Dhanas",
    "Makara",   "Kumbha",       "Meena" };

static char *rasilord[] = {
    "Mars",     "Venus",        "Mercury",
    "Moon",     "Sun",          "Mercury",
    "Venus",    "Mars",         "Jupiter",
    "Saturn",   "Saturn",       "Jupiter" };

static char *gyoni[] = { "Male Horse",      "Male Elephant",    "Female Sheep",
                         "Male Serpent",    "Female Serpent",   "Female Dog",
                         "Female Cat",      "Male Sheep",       "Male Cat",
                         "Male Rat",        "Female Rat",       "Male Cow",
                         "Female Buffalo",  "Female Tiger",     "Male Buffalo",
                         "Male Tiger",      "Female Deer",      "Male Deer",
                         "Male Dog",        "Male Monkey",      "Mongoose",
                         "Female Monkey",   "Female Lion",      "Female Horse",
                         "Male Lion",       "Female Cow",       "Female Elephant" }; 

static int giyoni[] = { 0,   1,  2,
                       3,   3,  4,
                       5,   2,  5,
                       6,   6,  7,
                       8,   9,  8,
                       9,  10, 10,
                       4,  11, 13,
                      11,  12,  0,
                      12,   7,  1 }; 

void
usage(char *argv, int c)
{
    printf("%s: [options]\n"
            "-a     Print all, else only top ten is printed\n"
            "-b     Boy's Name (brith details read from astweb.txt, if found)\n"
            "-c     Testing\n"
            "-f     Girl's birth details Name|Place|Mon|dd|yyyy|hh:mm|ST|GMToff|Lon|Lat\n"
            "-g     Girl's Name (brith details read from astweb.txt, if found)\n"
            "-h     This info\n"
            "-l     Loop\n"
            "-m     Boy's birth details Name|Place|Mon|dd|yyyy|hh:mm|ST|GMToff|Lon|Lat\n"
            "-s     Star Table\n"
            "-t     Match Table\n"
            "-v     Verbose\n"
            "-w     Data File [astnew.txt, default is astweb.txt]\n"
            "\n", argv);

    printf("%s -m \"Author|Valasarapakkam Madras|11|15|1967|08:13|ST|-5:30|80:17E|13:40N\" "
                "-f \"Wife|Madras|2|01|1977|19:12|ST|-5:30|80:17E|13:40N\""
                "\n", argv);
    if (c) {
        printf("Invalid argument %c\n", c);
    }
    exit(0);
}

int 
pupper(char *str, char *ustr)
{
    int i;
    int len = strlen(str);

    for (i = 0; i < len; i++) {
        if (islower(str[i])) {
            ustr[i] = toupper(str[i]);
        } else {
            ustr[i] = str[i];
        }
    }
    ustr[i] = '\0';

    return(0);
}

int
readData(tHorDetails *hd, char *optname)
{
    FILE *rfp;
    char str[STR_LEN];
    char name[STR_LEN];
    char uname[STR_LEN];

    if ((rfp = fopen(file_name, "r")) == NULL) {
        printf("Unable to open file %s for reading\n", file_name);
        return -1;
    }

    pupper(optname, name);

    while (fgets(str, STR_LEN, rfp) != NULL) {
        hd->dst = 0;
        hd->old = 0;

        sscanf(str, "%s %s %s %s %s %s %s %s %s %s %s", 
                hd->name, hd->email, hd->place, hd->mon, hd->day, hd->year, hd->time,
                hd->zone, hd->offset, hd->lon, hd->lat);

        if (strcmp(hd->zone, "AD") == 0) {
            hd->dst = 1;
            strcpy(hd->zone, "ST");
        } else if (strcmp(hd->zone, "ST") != 0) {
            hd->old = 1;
            strcpy(hd->lat, hd->lon);
            strcpy(hd->lon, hd->offset);
            hd->offset[0] = '\0';
        }
        
        pupper(hd->name, uname);

        if (strcmp(uname, name) == 0) {
            return 1;
        }
    }

    printf("Name \"%s\" Not Found\n", name);

    return 0;
}

int
findDiff(int pos1, int pos2, int base)
{
    int diff = pos1 - pos2;

    if (diff < 0) {
        diff += base;
    }

    diff += 1;

    return diff;
}

int
VarnaKuta(tHorDetails *hdf, tHorDetails *hdm)
{
    static int varnamat[4][4] = {
        { 1, 0, 0, 0 },
        { 1, 1, 0, 0 },
        { 1, 1, 1, 0 },
        { 1, 1, 1, 1 }
    };

    return varnamat[hdf->varna][hdm->varna];
}

float
VashyaKuta(tHorDetails *hdf, tHorDetails *hdm)
{
    static float vashyamat[5][5] = {
        { 2.0, 1.0, 1.0, 0.0, 1.0 },
        { 1.0, 2.0, 0.5, 0.0, 1.0 },
        { 1.0, 0.5, 2.0, 1.0, 1.0 },
        { 0.0, 0.0, 1.0, 2.0, 0.0 },
        { 1.0, 0.0, 1.0, 0.0, 2.0 }
    };

    return vashyamat[hdf->vashya][hdm->vashya];
}

float
DinaKuta(tHorDetails *hd)
{
    float rv = 1.5; // Favourable
    int diff = hd->nakdiff;

    diff %= 9;
    switch (diff) {
    case 1:
        if (hd->disp) {
            printf("Janma:\n"
                    "    Similar thoughts, good mental connection can be harmed due to lack of balance\n"
                    "        emotions as both can react similarly to pressures\n");
        }
        hd->cdina = "Janma";
        rv = 0;
        break;
    case 2:
        if (hd->disp) {
            printf("Sampat:\n"
                    "    Wealthy so you will emotionally support each other the relationship will\n"
                    "        be creative and enriching\n");
        }
        hd->cdina = "Sampat";
        break;
    case 3:
        if (hd->disp) {
            printf("Vipat:\n"
                    "    Blocked. This is the difficult relationships where two minds do not understand\n"
                    "        each other and are not able to give emotional support the other wants\n");
        }
        hd->cdina = "Vipat";
        rv = 0;
        break;
    case 4:
        if (hd->disp) {
            printf("Kshema:\n"
                    "    Gives happiness and fulfillment\n");
        }
        hd->cdina = "Kshema";
        break;
    case 5:
        if (hd->disp) {
            printf("Prayatak:\n"
                    "    Creates obstacles, the minds lack harmony and can be obstructive towards each other\n");
        }
        hd->cdina = "Prayatak";
        rv = 0;
        break;
    case 6:
        if (hd->disp) {
            printf("Sadhana:\n"
                    "    Spiritual harmony and the ability to gel together on a deeply spiritual level\n");
        }
        hd->cdina = "Sadhana";
        break;
    case 7:
        if (hd->disp) {
            printf("Naidhana:\n"
                    "    Poverty. This creates a poor connection, no fertility of ideas and emotions\n"
                    "        that ties people together\n");
        }
        hd->cdina = "Naidhana";
        rv = 0;
        break;
    case 8:
        if (hd->disp) {
            printf("Mitra:\n"
                    "    Friend. great nakshatra to have as you mentally connect and give friendly\n"
                    "        vibes to each other\n");
        }
        hd->cdina = "Mitra";
        break;
    case 0:
        if (hd->disp) {
            printf("ParamMitra:\n"
                    "    Great friends. This is a great combination as both people feel at\n"
                    "        ease with each. one picks up what the other says, there is true harmony\n");
        }
        hd->cdina = "ParamMitra";
        break;
    default:
        printf("Dina UNKNWON %d\n", diff);
        hd->cdina = "Unknown";
        break;
    }
    return rv;
}

void
printGanaKuta(tHorDetails *hd)
{
    switch(hd->gana) {
    case 0:
        if (hd->disp) {
            printf("Devas:\n"
                    "    Kind, soft and sensitive. Steady, calm, settled, puritan and followers of dharma\n");
        }
        break;
    case 1:
        if (hd->disp) {
            printf("Manushya:\n"
                    "    Imperfect, mostly trying to fulfill their ambitions, strugle with weaknesses\n"
                    "        and moving between fine and harsh natures. Dynamic, active, ambitious,\n"
                    "        searching for answers inlife\n");
        }
        break;
    case 2:
        if (hd->disp) {
            printf("Rakshasha:\n"
                    "    Only concerned with their own needs, never caring for others.\n"
                    "        Highly intelligent, will be self serving, ambitious and agressive\n");
        }
        break;
    default:
        printf("Gana UNKNWON\n");
        break;
    }
}

int
GanaKuta(tHorDetails *hdf, tHorDetails *hdm)
{
    static int ganamat[3][3] = {
        { 6, 6, 1 },
        { 5, 6, 0 },
        { 1, 0, 6 }
    };

    printGanaKuta(hdf);
    printGanaKuta(hdm);

    return ganamat[hdm->gana][hdf->gana];
}

int
YoniKuta(tHorDetails *hdf, tHorDetails *hdm)
{
    static int yonimat[14][14] = {
        { 4, 2, 2, 3, 2, 2, 2, 1, 0, 1, 3, 3, 2, 1 },
        { 2, 4, 3, 3, 2, 2, 2, 2, 3, 1, 2, 3, 2, 0 },
        { 2, 3, 4, 2, 1, 2, 1, 3, 3, 1, 2, 0, 3, 0 },
        { 3, 3, 2, 4, 2, 1, 1, 1, 1, 2, 2, 2, 0, 2 },
        { 2, 2, 1, 2, 4, 2, 1, 2, 2, 1, 0, 2, 1, 1 },
        { 2, 2, 2, 1, 2, 4, 0, 2, 2, 1, 3, 3, 2, 1 },
        { 2, 2, 1, 1, 1, 0, 4, 2, 2, 2, 2, 2, 1, 2 },
        { 1, 2, 3, 1, 2, 2, 2, 4, 3, 0, 3, 2, 2, 1 },
        { 0, 3, 3, 1, 2, 2, 2, 3, 4, 1, 2, 2, 2, 1 },
        { 1, 1, 1, 2, 1, 1, 2, 0, 1, 4, 1, 1, 2, 1 },
        { 1, 2, 2, 2, 0, 3, 2, 3, 2, 1, 4, 2, 2, 1 },
        { 3, 3, 0, 2, 2, 3, 2, 2, 2, 1, 2, 4, 3, 2 },
        { 2, 2, 3, 0, 1, 2, 1, 2, 2, 2, 2, 3, 4, 2 },
        { 1, 0, 1, 2, 1, 1, 2, 1, 2, 1, 1, 2, 2, 4 }
    };

    return yonimat[hdf->yoni][hdm->yoni];
}

int
RasiKuta(tHorDetails *hdf)
{
    int rv = 0;

    switch (hdf->rasidiff) {
    case 1: case 7: case 8:  case 9: case 10: case 11: case 12:
        rv = 7;
        break;
    }

    return rv;
}

int
NadiKuta(tHorDetails *hdf, tHorDetails *hdm)
{
    int rv = 8;

    if (hdf->nadi == hdm->nadi) {
        rv = 0;
    }

    return rv;
}

int
GrahaKuta(tHorDetails *hdf, tHorDetails *hdm)
{
    static int grahamat[12][12] = {
      //  Ma,  V,   Me,  Mo,  S,   Me,  V,   Ma,  J,   Sa,  Sa,  J
        { 5,   3,   0,   4,   5,   0,   3,   5,   5,   0,   0,   5 }, // Mars
        { 3,   5,   5,   4,   0,   5,   5,   3,   0,   5,   5,   0 }, // Venus
        { 0,   5,   5,   2,   4,   5,   5,   0,   0,   4,   4,   0 }, // Mercury
        { 4,   0,   2,   5,   5,   2,   0,   4,   4,   0,   0,   4 }, // Moon
        { 5,   0,   4,   5,   5,   4,   0,   5,   5,   0,   0,   5 }, // Sun
        { 0,   5,   5,   2,   4,   5,   5,   0,   0,   4,   4,   0 }, // Mercury
        { 3,   5,   5,   4,   0,   5,   5,   3,   0,   5,   5,   0 }, // Venus
        { 5,   3,   0,   4,   5,   0,   3,   5,   5,   0,   0,   5 }, // Mars
        { 5,   0,   0,   4,   5,   0,   0,   5,   5,   3,   3,   5 }, // Jupiter
        { 0,   5,   4,   0,   0,   4,   5,   0,   3,   5,   5,   3 }, // Saturn
        { 0,   5,   4,   0,   0,   4,   5,   0,   3,   5,   5,   3 }, // Saturn
        { 5,   0,   0,   4,   5,   0,   0,   5,   5,   3,   3,   5 }, // Jupiter
    };

    return grahamat[hdf->ra][hdm->ra];
}

int
Mahendra(tHorDetails *hdf)
{
    int rv = 0;

    switch (hdf->nakdiff) {
    case 4: case 7: case 10: case 13: case 16: case 19: case 22: case 25:
        rv = 2;
        break;
    default:
        break;
    }

    return(rv);
}

int
StreeDeergha(tHorDetails *hdf)
{
    int rv = 0;

    if (hdf->nakdiff >= 13 && hdf->nakdiff <= 27) {
        rv = 2;
    } else if (hdf->nakdiff >= 8 && hdf->nakdiff <= 12) {
        rv = 1;
    } 

    return(rv);
}

int
Rajju(tHorDetails *hdf, tHorDetails *hdm)
{
    int rv = 0;

    if (hdf->rajju != hdm->rajju) {
        rv = 4;
    }

    return(rv);
}

int
Vedha(tHorDetails *hdf, tHorDetails *hdm)
{
    int rv = 0;
    int nakm = hdm->nak;
    int nakf = hdf->nak;

    if ((nakf == 0 && nakm == 17) ||
            (nakf == 17 && nakm == 0)) {
        rv = 1;
    } else if ((nakf == 1 && nakm == 16) ||
            (nakf == 16 && nakm == 1)) {
        rv = 1;
    } else if ((nakf == 2 && nakm == 15) ||
            (nakf == 15 && nakm == 2)) {
        rv = 1;
    } else if ((nakf == 3 && nakm == 14) ||
            (nakf == 14 && nakm == 3)) {
        rv = 1;
    } else if ((nakf == 5 && nakm == 21) ||
            (nakf == 21 && nakm == 5)) {
        rv = 1;
    } else if ((nakf == 6 && nakm == 20) ||
            (nakf == 20 && nakm == 6)) {
        rv = 1;
    } else if ((nakf == 7 && nakm == 19) ||
            (nakf == 19 && nakm == 7)) {
        rv = 1;
    } else if ((nakf == 8 && nakm == 18) ||
            (nakf == 18 && nakm == 8)) {
        rv = 1;
    } else if ((nakf == 9 && nakm == 26) ||
            (nakf == 26 && nakm == 9)) {
        rv = 1;
    } else if ((nakf == 10 && nakm == 25) ||
            (nakf == 25 && nakm == 10)) {
        rv = 1;
    } else if ((nakf == 11 && nakm == 24) ||
            (nakf == 24 && nakm == 11)) {
        rv = 1;
    } else if ((nakf == 12 && nakm == 23) ||
            (nakf == 25 && nakm == 10)) {
        rv = 1;
    } else if ((nakf == 4 && nakm == 13) ||
            (nakf == 13 && nakm == 4)) {
        rv = 1;
    } else if ((nakf == 4 && nakm == 22) ||
            (nakf == 22 && nakm == 4)) {
        rv = 1;
    } else if ((nakf == 13 && nakm == 22) ||
            (nakf == 22 && nakm == 13)) {
        rv = 1;
    }

    return(rv);
}

void
AssignVarna(tHorDetails *hd)
{
    switch(hd->ra) {
    case 3: case 7: case 11:
        hd->varna = 0;
        hd->cvarna = "Brahmin";
        break;
    case 0: case 4: case 8:
        hd->varna = 1;
        hd->cvarna = "Kshatriya";
        break;
    case 1: case 5: case 9:
        hd->varna = 2;
        hd->cvarna = "Vaishya";
        break;
    case 2: case 6: case 10:
        hd->varna = 3;
        hd->cvarna = "Shudra";
        break;
    default:
        hd->varna = -1;
        hd->cvarna = "Unknown";
        break;
    }
}

void
AssignVashya(tHorDetails *hd)
{
    switch(hd->ra) {
    case 0: case 1: case 8:
        hd->vashya = 0;
        hd->cvashya = "Chatus";
        break;
    case 2: case 5: case 6:
        hd->vashya = 1;
        hd->cvashya = "Nara";
        break;
    case 3: case 9: case 10: case 11:
        hd->vashya = 2;
        hd->cvashya = "Jala";
        break;
    case 4:
        hd->vashya = 3;
        hd->cvashya = "Bana";
        break;
    case 7:
        hd->vashya = 4;
        hd->cvashya = "Keeta";
        break;
    default:
        hd->vashya = -1;
        hd->cvashya = "Unknown";
        break;
    }
}

void
AssignYoni(tHorDetails *hd)
{
    hd->yoni = giyoni[hd->nak];
    hd->cyoni = gyoni[hd->nak];
}
 
void
AssignGana(tHorDetails *hd)
{
    switch(hd->nak) {
    case 0: case 4: case 6: case 7: case 12: case 14: case 16: case 21: case 26:
        hd->gana = 0;
        hd->cgana = "Devas";
        break;
    case 1: case 3: case 5: case 10: case 11: case 19: case 20: case 24: case 25:
        hd->gana = 1;
        hd->cgana = "Manushas";
        break;
    case 2: case 8: case 9: case 13: case 15: case 17: case 18: case 22: case 23:
        hd->gana = 2;
        hd->cgana = "Rakshashas";
        break;
    default:
        hd->gana = -1;
        hd->cgana = "Unknown";
        break;
    }
}

void
AssignNadi(tHorDetails *hd)
{
    switch(hd->nak) {
    case 0: case 5: case 6: case 11: case 12: case 17: case 18: case 23: case 24:
        hd->nadi = 0;
        hd->cnadi = "Vata";
        break;
    case 1: case 4: case 7: case 10: case 13: case 16: case 19: case 22: case 25:
        hd->nadi = 1;
        hd->cnadi = "Pitha";
        break;
    case 2: case 3: case 8: case 9: case 14: case 15: case 20: case 21: case 26:
        hd->nadi = 2;
        hd->cnadi = "Sleshma";
        break;
    default:
        hd->nadi = -1;
        hd->cnadi = "Unknown";
        break;
    }
}

void
AssignRajju(tHorDetails *hd)
{
    switch(hd->nak) {
    case 0:  case 8: case 9: case 17: case 18: case 26:
        hd->rajju = 0;
        hd->crajju = "Pada";
        break;
    case 1: case 7: case 10: case 16: case 19: case 25:
        hd->rajju = 1;
        hd->crajju = "Waist";
        break;
    case 2: case 6: case 11: case 15: case 20: case 24:
        hd->rajju = 2;
        hd->crajju = "Nabhi";
        break;
    case 3: case 5: case 12: case 14: case 21: case 23:
        hd->rajju = 3;
        hd->crajju = "Kantha";
        break;
    case 4: case 13: case 22:
        hd->rajju = 4;
        hd->crajju = "Head";
        break;
    default:
        hd->rajju = -1;
        hd->crajju = "Unknown";
        break;
    }
}

void
printhd(tHorDetails *hd, int flag)
{
    if (flag == 0) {
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
    } else {
        printf("{star:\"Name\",match:\"%s\"},", hd->name);
        printf("{star:\"Place\",match:\"%s\"},", hd->place);
        printf("{star:\"Month\",match:\"%s\"},", hd->mon);
        printf("{star:\"Date\",match:\"%s\"},", hd->day);
        printf("{star:\"Year\",match:\"%s\"},", hd->year);
        printf("{star:\"Time\",match:\"%s\"},", hd->time);
        printf("{star:\"Zone\",match:\"%s\"},", hd->zone);
        printf("{star:\"Offset\",match:\"%s\"},", hd->offset);
        printf("{star:\"Long\",match:\"%s\"},", hd->lon);
        printf("{star:\"Lat\",match:\"%s\"},", hd->lat);
        printf("{star:\"Dst\",match:\"%d\"},", hd->dst);
    }
}


void
getValues(tHorDetails *hd)
{
    hd->rasi = rasi[hd->ra];
    hd->rasilord = rasilord[hd->ra];
    hd->naksatra = naksatras[hd->nak];

    AssignVarna(hd);

    AssignVashya(hd);

    AssignGana(hd);

    AssignYoni(hd);

    AssignNadi(hd);

    AssignRajju(hd);
}

int
horoscope(tHorDetails *hd)
{
    char str[STR_LEN];
    int fd[2];
    int targc = 0;
    char *targv[256];

    pipe(fd);

    if (verbose) {
        printhd(hd, 0);
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
            getValues(hd);
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
 
char *
gethd(char *optarg, tHorDetails *hdm, tHorDetails *hdf)
{
    int i;
    int imon;
    char *s;
    static char *mon[] = { "",
            "Jan", "Feb", "Mar",
            "Apr", "May", "Jun",
            "Jul", "Aug", "Sep",
            "Oct", "Nov", "Dec"
        };
                
    for (i = 0; ((s = strsep((char **)&optarg, "|")) != NULL); i++) {
        switch (i) {
        case 0:
            strip_blanks(hdm->name, s);
            break;
        case 1:
            strip_blanks(hdm->place, s);
            break;
        case 2:
            imon = atoi(s);
            if (imon > 0 && imon <= 12) {
                strcpy(hdm->mon, mon[imon]);
            }
            break;
        case 3:
            strcpy(hdm->day, s);
            break;
        case 4:
            strcpy(hdm->year, s);
            break;
        case 5:
            strip_blanks(hdm->time, s);
            break;
        case 6:
            strcpy(hdm->zone, s);
            break;
        case 7:
            strcpy(hdm->offset, s);
            break;
        case 8:
            strcpy(hdm->lon, s);
            break;
        case 9:
            strcpy(hdm->lat, s);
            break;
        case 10:
            break;
        case 11:
            if (hdf) strip_blanks(hdf->name, s);
            break;
        case 12:
            if (hdf) strip_blanks(hdf->place, s);
            break;
        case 13:
            imon = atoi(s);
            if (imon > 0 && imon <= 12) {
                if (hdf) strcpy(hdf->mon, mon[imon]);
            }
            break;
        case 14:
            if (hdf) strcpy(hdf->day, s);
            break;
        case 15:
            if (hdf) strcpy(hdf->year, s);
            break;
        case 16:
            if (hdf) strip_blanks(hdf->time, s);
            break;
        case 17:
            if (hdf) strcpy(hdf->zone, s);
            break;
        case 18:
            if (hdf) strcpy(hdf->offset, s);
            break;
        case 19:
            if (hdf) strcpy(hdf->lon, s);
            break;
        case 20:
            if (hdf) strcpy(hdf->lat, s);
            break;
        case 21:
            break;
        default:
            break;
        }
    }

    if (strcmp(hdm->zone, "AD") == 0) {
        hdm->dst = 1;
        strcpy(hdm->zone, "ST");
    } else if (strcmp(hdm->zone, "ST") != 0) {
        hdm->dst = 1;
        strcpy(hdm->lon, hdm->lat);
        strcpy(hdm->lat, hdm->offset);
        hdm->offset[0] = '\0';
    }

    if (hdf) {
        if (strcmp(hdf->zone, "AD") == 0) {
            hdf->dst = 1;
            strcpy(hdf->zone, "ST");
        } else if (strcmp(hdf->zone, "ST") != 0) {
            hdf->dst = 1;
            strcpy(hdf->lon, hdf->lat);
            strcpy(hdf->lat, hdf->offset);
            hdf->offset[0] = '\0';
        }
    }
}

void
web2String(char *c1, char *c2, char *c3)
{
    printf("{c1:\"%s\",c2:\"%s\"},", c1, c2, c3);
}

void
webString(char *c1, char *c2, char *c3)
{
    printf("{c1:\"%s\",c2:\"%s\",c3:\"%s\"},", c1, c2, c3);
}

void
webInt(char *c1, int c2, int c3)
{
    printf("{c1:\"%s\",c2:\"%d\",c3:\"%d\"},", c1, c2, c3);
}

void
webKuta(char *c1, float c2, float c3)
{
    printf("{c1:\"%s\",c2:\"%.2f out of %.2f\",c3:\"\"},", c1, c2, c3);
}

void
printHeader(tHorDetails *hdm, tHorDetails *hdf, int web)
{
    char mdate[STR_LEN];
    char fdate[STR_LEN];

    sprintf(mdate, "%s, %s %s - %s", hdm->mon, hdm->day, hdm->year, hdm->time);
    sprintf(fdate, "%s, %s %s - %s", hdf->mon, hdf->day, hdf->year, hdf->time);

    if (web) {
        webString("", "Boy", "Girl");
        webString("Name", hdm->name, hdf->name);
        webString("Date and Time of Birth", mdate, fdate);
        webString("Rashi", hdm->rasi, hdf->rasi);
        webString("Rasi Lord", hdm->rasilord, hdf->rasilord);
        webInt("Rasi Difference", hdm->rasidiff, hdf->rasidiff);
        webString("Nakshatra (Star)", hdm->naksatra, hdf->naksatra);
        webInt("Star Difference", hdm->nakdiff, hdf->nakdiff);
        webString("Varna", hdm->cvarna, hdf->cvarna);
        webString("Vashya", hdm->cvashya, hdf->cvashya);
        webString("Dina", hdm->cdina, hdf->cdina);
        webString("Yoni", hdm->cyoni, hdf->cyoni);
        webString("Gana", hdm->cgana, hdf->cgana);
        webString("Nadi", hdm->cnadi, hdf->cnadi);
        webString("", "", "-");
    } else {
        printf("\n%-12s%-12s%-24s%-12s%-24s\n\n", "", "", "Boy", "", "Girl");
        printf("%-12s%-12s%-24s%-12s%-24s\n", "Name", "", hdm->name, "", hdf->name);
        printf("%-12s%-12s%-24s%-12s%-24s\n", "Date", "", mdate, "", fdate);
        printf("%-12s%-12s%-24s%-12s%-24s\n", "Rasi", "", hdm->rasi, "", hdf->rasi);
        printf("%-12s%-12s%-24s%-12s%-24s\n", "RasiLord", "", hdm->rasilord, "", hdf->rasilord);
        printf("%-12s%-12s%-24d%-12s%-24d\n", "RasiDiff", "", hdm->rasidiff, "", hdf->rasidiff);
        printf("%-12s%-12s%-24s%-12s%-24s\n", "Star", "", hdm->naksatra, "", hdf->naksatra);
        printf("%-12s%-12s%-24d%-12s%-24d\n", "StarDiff", "", hdm->nakdiff, "", hdf->nakdiff);
        printf("%-12s%-12s%-24s%-12s%-24s\n", "Varna", "", hdm->cvarna, "", hdf->cvarna);
        printf("%-12s%-12s%-24s%-12s%-24s\n", "Vashya", "", hdm->cvashya, "", hdf->cvashya);
        printf("%-12s%-12s%-24s%-12s%-24s\n", "Dina", "", hdm->cdina, "", hdf->cdina);
        printf("%-12s%-12s%-24s%-12s%-24s\n", "Yoni", "", hdm->cyoni, "", hdf->cyoni);
        printf("%-12s%-12s%-24s%-12s%-24s\n", "Gana", "", hdm->cgana, "", hdf->cgana);
        printf("%-12s%-12s%-24s%-12s%-24s\n", "Nadi", "", hdm->cnadi, "", hdf->cnadi);
        printf("\n");
    }
}

float
find_match(tHorDetails *hdm, tHorDetails *hdf, int web)
{
    float pvarna = 0;
    float pvashya = 0;
    float pdina = 0;
    float pyoni = 0;
    float pgraha = 0;
    float pgana = 0;
    float prasi = 0;
    float pnadi = 0;
    float points = 0;
    int rv;
    char *pstr;

    hdm->rasidiff = findDiff(hdf->ra, hdm->ra, 12);
    hdf->rasidiff = findDiff(hdm->ra, hdf->ra, 12);

    hdm->nakdiff = findDiff(hdf->nak, hdm->nak, 27);
    hdf->nakdiff = findDiff(hdm->nak, hdf->nak, 27);

    // 1 point
    pvarna = VarnaKuta(hdf, hdm);

    // 2 points
    pvashya = VashyaKuta(hdf, hdm);

    // 1.5 points
    pdina = DinaKuta(hdm);

    // 1.5 points
    pdina += DinaKuta(hdf);

    printf("\n");

    // 4 points
    pyoni = YoniKuta(hdf, hdm);

    // 5 points
    pgraha = GrahaKuta(hdf, hdm);

    // 6 points
    pgana = GanaKuta(hdf, hdm);

    printf("\n");

    // 7 points
    prasi = RasiKuta(hdf);

    // 8 points
    pnadi = NadiKuta(hdf, hdm);

    // Printer initial information
    if (web) {
        printHeader(hdm, hdf, 1);
    } else {
        printHeader(hdm, hdf, 0);
    }

    if (web) {
        webKuta("Spiritual Compatibility (Varna)", pvarna, 1.0);
        webKuta("Ability to influence each other (Vashya)", pvashya, 2.0);
        webKuta("Health & Happiness (Dina)", pdina, 3.0);
        webKuta("Sexual compatibility (Yoni)", pyoni, 4.0);
        webKuta("Psychological compatibility (Graha)", pgraha, 5.0);
        webKuta("Attitude to life or Temperament (Gana)", pgana, 6.0);
        webKuta("Harmony in marriage (Rasi)", prasi, 7.0);
        webKuta("Health after marriage (Nadi)", pnadi, 8.0);
        webString("", "", "-");
     } else if (hdm->disp) {
        printf("%-12s %.2f (%.2f) \"%s\"\n", "Varna", pvarna, 1.0, "Spiritual Compatibility");
        printf("%-12s %.2f (%.2f) \"%s\"\n", "Vashya", pvashya, 2.0,
                                                    "Ability to influence each other");
        printf("%-12s %.2f (%.2f) \"%s\"\n", "Dina", pdina, 3.0, "Health & Happiness");
        printf("%-12s %.2f (%.2f) \"%s\"\n", "Yoni", pyoni, 4.0, "Sexual compatibility");
        printf("%-12s %.2f (%.2f) \"%s\"\n", "Graha", pgraha, 5.0, "Psychological compatibility");
        printf("%-12s %.2f (%.2f) \"%s\"\n", "Gana", pgana, 6.0, "Attitude to life or Temperament");
        printf("%-12s %.2f (%.2f) \"%s\"\n", "Rasi", prasi, 7.0, "Harmony in marriage");
        printf("%-12s %.2f (%.2f) \"%s\"\n", "Nadi", pnadi, 8.0, "Health after marriage");
        printf("\n");
    }

    if (Mahendra(hdf)) {
        pstr = "    in agreement";
    } else {
        pstr = "not in agreement";
    }

    if (web) {
        webString("Wealth, longevity, progeny (Mahendra)", pstr, "");
    } else if (hdm->disp) {
        printf("Mahendra     %-17s \"%s\"\n", pstr, "Wealth, longevity, progeny");
    }

    rv = StreeDeergha(hdf);
    if (rv) {
        pstr = "    in agreement";
    } else {
        pstr = "not in agreement";
    }

    if (web) {
        webString("Woman's happiness after marriage (StreeDeergha)", pstr, "");
    } else if (hdm->disp) {
        printf("StreeDeergha %-17s \"%s\"\n", pstr, "Woman's happiness after marriage");
    }

    if (Rajju(hdf, hdm)) {
        pstr = "    in agreement";
    } else {
        pstr = "not in agreement";
    }

    if (web) {
        webString("Bonding between the couple (Rajju)", pstr, "");
    } else if (hdm->disp) {
        printf("Rajju        %-17s \"%s\"\n", pstr, "Bonding between the couple");
    }

    if (Vedha(hdf, hdm)) {
        pstr = "not in agreement";
    } else {
        pstr = "    in agreement";
    }

    if (web) {
        webString("Affliction or Pain (Vedha)", pstr, "");
    } else if (hdm->disp) {
        printf("Vedha        %-17s \"%s\"\n", pstr, "Affliction or Pain");
    }

    points = pvarna + pvashya + pdina + pyoni + pgraha + pgana + prasi + pnadi;

    return points;
}

void
testing(tHorDetails *hdm, tHorDetails *hdf)
{
    strcpy(hdm->name, "Logan");
    strcpy(hdm->email, "loganathanu@yahoo.com");
    strcpy(hdm->place, "Madras");
    strcpy(hdm->mon, "11");
    strcpy(hdm->day, "15");
    strcpy(hdm->year, "1967");
    strcpy(hdm->time, "8:13");
    strcpy(hdm->zone, "ST");
    strcpy(hdm->offset, "-5:30");
    strcpy(hdm->lon, "80:17E");
    strcpy(hdm->lat, "13:40N");
    strcpy(hdf->name, "Meena");
    strcpy(hdf->email, "loganathanu@yahoo.com");
    strcpy(hdf->place, "Madras");
    strcpy(hdf->mon, "2");
    strcpy(hdf->day, "1");
    strcpy(hdf->year, "1977");
    strcpy(hdf->time, "19:12");
    strcpy(hdf->zone, "ST");
    strcpy(hdf->offset, "-5:30");
    strcpy(hdf->lon, "80:17E");
    strcpy(hdf->lat, "13:40N");
}

int
comp(const void *elem1, const void *elem2)
{
    tStarList *lsh = (tStarList *)elem1;
    tStarList *rsh = (tStarList *)elem2;

    if (lsh->mv < rsh->mv) return 1;
    if (lsh->mv > rsh->mv) return -1;

    return 0;
}

void
starloop(tHorDetails *hdm, tHorDetails *hdf)
{
    int i, j;
    float percent, points;

    testing(hdm, hdf);

    for (i = 0; i < 36; i++) {
        hdm->nak = matchTable[i].nak; 
        hdm->ra = matchTable[i].ra; 
        getValues(hdm);
        for (j = 0; j < 36; j++) {
            hdf->nak = matchTable[j].nak; 
            hdf->ra = matchTable[j].ra; 
            getValues(hdf);

            points = find_match(hdm, hdf, 0);

            percent = (points * 100.0) / 36.0;

            printf("\nTotal Points %.2f (36.0), %.2f%%\n\n", points, percent);
        }
    }
}

void
match(int argc, char *argv[])
{
    tHorDetails hdm;
    tHorDetails hdf;
    int i, j;
    int c;
    int si;
    int bsi;
    int gsi;
    int topten = 1;
    int mflag = 0;
    int table = 0;
    int lstable = 0;
    int loop = 0;
    int sdata[36];
    float points = 0;
    float percent = 0;

    memset(&hdm, 0, sizeof(tHorDetails));
    memset(&hdf, 0, sizeof(tHorDetails));

    if (argc == 1) {
        usage(argv[0], 0);
    }

    strcpy(file_name, "/var/www/cgi-bin/astweb.txt");

    while ((c = getopt(argc, argv, "ab:cf:g:hlm:stvw")) != EOF) {
        switch (c) {
        case 'a':
            topten = 0;
            break;
        case 'b':
            if (optarg && optarg[0] != '-') {
                if (readData(&hdm, optarg) <= 0) {
                    exit(0);
                }
            } else {
                usage(argv[0], c);
            }
            mflag = 1;
            break;
        case 'c':
            testing(&hdm, &hdf);
            break;
        case 'f':
            if (optarg && optarg[0] != '-') {
                gethd(optarg, &hdf, NULL);
            } else {
                usage(argv[0], c);
            }
            break;
        case 'g':
            if (optarg && optarg[0] != '-') {
                if (readData(&hdf, optarg) <= 0) {
                    printf("%s\n", optarg);
                    exit(0);
                }
            } else {
                usage(argv[0], c);
            }
            break;
        case 'h':
            usage(argv[0], 0);
            break;
        case 'l':
            loop = 1;
            break;
        case 'm':
            if (optarg && optarg[0] != '-') {
                gethd(optarg, &hdm, NULL);
            } else {
                usage(argv[0], c);
            }
            break;
        case 's':
            lstable = 1;
            break;
        case 't':
            table = 1;
            break;
        case 'v':
            verbose = 1;
            break;
        case 'w':
            strcpy(file_name, "/var/www/cgi-bin/astnew.txt");
            break;
        default:
            usage(argv[0], c);
            break;
        }
    }

    if (loop) {
        starloop(&hdm, &hdf);
    } else if (lstable) {
        if (mflag) {
            if (hdm.name[0] == '\0') {
                printf("Boy's name is missing\n");
                exit(0);
            }
            horoscope(&hdm);
            si = (int)(hdm.moondeg / 10.0);
            printf("\n%s: Moon %8.2f\n%s\n\n", hdm.name, hdm.moondeg, star_table[si]);
        } else {
            if (hdf.name[0] == '\0') {
                printf("Girl's name is missing\n");
                exit(0);
            }
            horoscope(&hdf);
            si = (int)(hdf.moondeg / 10.0);
            printf("\n%s: Moon %8.2f\n%s\n\n", hdf.name, hdf.moondeg, star_table[si]);
        }

        star_lookup(si, mflag, sdata);

        for (i = 0; i < 36; i++) {
            sl[i].si = i;
            sl[i].mv = sdata[i];
        }
        qsort(sl, 36, sizeof(tStarList), comp);
        j = 0;
        for (i = 0; i < 36; i++) {
            if ((sl[i].si / 3) == (si / 3)) continue;
            printf("%-25s %6d\n", star_table[sl[i].si], sl[i].mv);
            if (topten && (++j == 10)) break;
        }
    } else {
        if ((hdm.name[0] == '\0') || (hdf.name[0] == '\0')) {
            usage(argv[0], 0);
        }

        horoscope(&hdm);
        horoscope(&hdf);

        bsi = (int)(hdm.moondeg / 10.0);
        gsi = (int)(hdf.moondeg / 10.0);

        printf("\n%s: Moon %6.2f\n%s\n\n", hdm.name, hdm.moondeg, star_table[bsi]);
        printf("%s: Moon %6.2f\n%s\n\n", hdf.name, hdf.moondeg, star_table[gsi]);

        if (table) {
            points = table_lookup((int)(hdm.moondeg / 10.0), (int)(hdf.moondeg / 10.0));
        } else {
            hdm.disp = hdf.disp = 1;
            points = find_match(&hdm, &hdf, 0);
        }

        percent = (points * 100.0) / 36.0;

        printf("\n\n\nTotal Points %.2f (36.0), %.2f%%\n\n", points, percent);

        if (percent >= 50) {
            printf("\t50%% or more, can be considered for marriage\n\n");
        } else {
            printf("\tBelow 50%%, better Luck next time\n\n");
        }

        printf("For more details: %s\n", "https://www.jodilogik.com/wordpress/index.php/horoscope-matching-marriage/");
    }

    printf("\n");
}

void
starcb(int si, int mflag)
{
    int i, j;
    int sameRasi = 1;
    int sdata[36];

    printf("{star:\"%s\",rasi:\"%s\",match:\"%s\"},", stable[si], rtable[si], 
            mflag ? "Boy" : "Girl");

    star_lookup(si, mflag, sdata);

    for (i = 0; i < 36; i++) {
        sl[i].si = i;
        sl[i].mv = sdata[i];
    }
    qsort(sl, 36, sizeof(tStarList), comp);
    j = 0;
    for (i = 0; i < 36; i++) {
        if (sameRasi) {
            if ((sl[i].si / 3) == (si / 3)) continue;
        }
        printf("{star:\"%s\",rasi:\"%s\",match:\"%.0f%%\"},", stable[sl[i].si], rtable[sl[i].si],
                ((sl[i].mv * 100.0) / 36));
        if (++j == 10) break;
    }
}

void
remove_blanks(char *str)
{
    int len = strlen(str);
    int i;
    int j;

    for (i = 0, j = 0; i <= len; i++) {
        if (str[i] != ' ') {
            str[j++] = str[i];
        }
    }
}

#define AST_WEB_FILE    "astweb.txt"
#define AST_STAR_FILE   "aststar.txt"
#define AST_MATCH_FILE  "astmatch.txt"

void
store_data(tHorDetails *hdm, tHorDetails *hdf, char *fname)
{
    FILE *fp;
    long clock;
    struct tm *ti;
    char *raddr;

    if ((fp = fopen(fname, "a")) == NULL) {
        fprintf(stdout, "Unable to open file %s\n", fname);
        fflush(stdout);
        exit(0);
    }

    remove_blanks(hdm->name);
    remove_blanks(hdm->place);

    if (hdf) {
        remove_blanks(hdf->name);
        remove_blanks(hdf->place);
    }

    if (hdm->email[0] == '\0') {
        strcpy(hdm->email, "loganathanu@yahoo.com");
    }

    raddr = getenv("REMOTE_ADDR");

    if (raddr == NULL) {
        raddr = "localhost";
    }

    clock = time(&clock);
    ti = localtime(&clock);

    fprintf(fp, "%-30s %-30s %-30s %-3s %-2s %-4s   %-10s %6s %14s %9s %9s ",
            hdm->name, hdm->email, hdm->place, hdm->mon, hdm->day,
            hdm->year, hdm->time, hdm->dst ? "AD" : hdm->zone,
            hdm->offset, hdm->lon, hdm->lat);

    if (hdf) {
        fprintf(fp, "\n%30s %-30s %-30s %-3s %-2s %-4s   %-10s %6s %14s %9s %9s ",
                "", hdf->name, hdf->place, hdf->mon, hdf->day,
                hdf->year, hdf->time, hdf->dst ? "AD" : hdf->zone,
                hdf->offset, hdf->lon, hdf->lat);
    }
    
    fprintf(fp, "%20s   %02d/%02d/%4d-%02d:%02d:%02d\n",
            raddr, ti->tm_mon + 1, ti->tm_mday, ti->tm_year + 1900,
            ti->tm_hour, ti->tm_min, ti->tm_sec);

    fclose(fp);
}

void
star()
{
    tHorDetails hd;
    FILE *wfp = stdout;
    char str[STR_LEN];
    char *tqs, *qs;
    char *s;
    int flag = 0;
    int i;
    int j;
    int k;
    int len;
    int si;

    fprintf(wfp, "Content-Type: application/json;charset=UTF-8\n\n");
    fflush(wfp);

    tqs = getenv("QUERY_STRING");

    if (tqs == NULL) {
        printf("Invalid QUERY_STRING\n");
        exit(0);
    }

    len = strlen(tqs);

    for (i = 0; i < len; i++) {
        if (tqs[i] == '&') {
            qs = &tqs[i + 1];
            break;
        }
    }

    if (0) {
        printf("%d:%s\n%s\n", len, tqs, qs);
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

            gethd(str, &hd, NULL);
            if (0) {
                printf("{starcb([");
                printhd(&hd, 1);
                printf("])}\n");
            }
            flag = 1;
            break;
        }
    }

    if (flag) {
        horoscope(&hd);
        si = (int)(hd.moondeg / 10.0);
        printf("{starcb([");
        starcb(si, 1);
        printf("{star:\"\",rasi:\"\",match:\"\"},");
        starcb(si, 0);
        printf("])}\n");
        store_data(&hd, NULL, AST_WEB_FILE);
        store_data(&hd, NULL, AST_STAR_FILE);
    }
}
    
void
wmatch()
{
    tHorDetails hdm;
    tHorDetails hdf;
    FILE *wfp = stdout;
    char str[STR_LEN];
    char *tqs, *qs;
    char *s;
    int flag = 0;
    int i;
    int j;
    int k;
    int len;
    int bsi, gsi;
    float points = 0;
    float percent = 0;

    fprintf(wfp, "Content-Type: application/json;charset=UTF-8\n\n");
    fflush(wfp);

    tqs = getenv("QUERY_STRING");

    if (tqs == NULL) {
        printf("Invalid QUERY_STRING\n");
        exit(0);
    }

    len = strlen(tqs);

    for (i = 0; i < len; i++) {
        if (tqs[i] == '&') {
            qs = &tqs[i + 1];
            break;
        }
    }

    if (0) {
        printf("len %d\ntqs %s\nqs %s\n", len, tqs, qs);
    }

    memset(&hdm, 0, sizeof(tHorDetails));
    memset(&hdf, 0, sizeof(tHorDetails));

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

            gethd(str, &hdm, &hdf);
            if (0) {
                printf("{matchcb([");
                printhd(&hdm, 1);
                printhd(&hdf, 1);
                printf("])}\n");
            }
            flag = 1;
            break;
        }
    }

    if (flag) {
        horoscope(&hdm);
        store_data(&hdm, NULL, AST_WEB_FILE);
        store_data(&hdf, NULL, AST_WEB_FILE);
        horoscope(&hdf);
        bsi = (int)(hdm.moondeg / 10.0);
        gsi = (int)(hdf.moondeg / 10.0);
        printf("{matchcb([");
        points = find_match(&hdm, &hdf, 1);
        percent = (points * 100.0) / 36.0;
        webString("", "", "-");
        printf("{c1:\"%s\",c2:\"%.2f out of 36.0 (%6.2f%%)\",c3:\"\"},",
                "Total Score", points, percent);
        webString("", "", "-");
        if (percent >= 50) {
            printf("{c1:\"Recommendation\",c2:\"Can be considered for marriage\",c3:\"\"},");
        } else {
            printf("{c1:\"Recommendation\",c2:\"Better Luck next time\",c3:\"\"},");
        }
        webString("", "", "-");
        if (0) {
            webString("For more details visit", "",
                    "https://www.jodilogik.com/wordpress/index.php/horoscope-matching-marriage/");
        }
        webString("", "", "-");
        printf("])}\n");
        store_data(&hdm, &hdf, AST_MATCH_FILE);
    }
}

int
main(int argc, char *argv[])
{
    char *base = basename(argv[0]);

    if (strcmp(base, "star") == 0) {
        star();
    } else if (strcmp(base, "wmatch") == 0) {
        wmatch();
    } else {
        match(argc, argv);
    }
    exit(0);
}
