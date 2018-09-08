#define FUSE_USE_VERSION 26

#include <stdio.h>
#include <fuse.h>
#include <string.h>
#include <errno.h>
#include "csv.h"

typedef struct postalCode {
    int pcode;
    char *neighborhood;
    char *city;
    char *district;
    double latitude;
    double longitude;
} postalCode;

typedef struct district {
    char *districtName;
    int neighborhoodCount;
    postalCode *pstlcd;
} district;

typedef struct city {
    char *cityName;
    int districtCount;
    district *dstct;
} city;

typedef struct names {
    int cityCount;
    city *cities;
} names;


names *nms;

typedef struct codes {
    int plateCodeCount;
    char *plateCode[82];
    postalCode *pstlcd;

} codes;

codes *cds;

static const char *names_path = "/NAMES";
static const char *codes_path = "/CODES";
static const char *csv_path = "/postal-codes.csv";
static char *csv_text;

postalCode *postalCodes;

static int postal_getattr(const char *path, struct stat *stbuf) {
    int res = 0;

    char dividedPath[5][50];
    memset(dividedPath, 0, sizeof(dividedPath));

    char temp[strlen(path)];
    memset(temp, 0, sizeof(path));
    strcpy(temp, path);


    char *p = strtok(temp, "/");
    int i = 0;


    if (strstr(temp, "."))
        goto out;


    while (p != NULL) {
        strcpy(dividedPath[i++], p);
        p = strtok(NULL, "/");
    }
    out:

    memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 4;
        stbuf->st_size = 4096;
        stbuf->st_uid = 1000;
        stbuf->st_gid = 1000;
        stbuf->st_atime = time(NULL);
        stbuf->st_mtime = time(NULL);
    } else if (strcmp(path, csv_path) == 0) {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = strlen(csv_text);
        stbuf->st_uid = 1000;
        stbuf->st_gid = 1000;
        stbuf->st_atime = time(NULL);
        stbuf->st_mtime = time(NULL);
    } else if (strcmp(path, names_path) == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = nms[0].cityCount + 2;
        stbuf->st_size = 4096;
        stbuf->st_uid = 1000;
        stbuf->st_gid = 1000;
        stbuf->st_atime = time(NULL);
        stbuf->st_mtime = time(NULL);
    } else if (strcmp(path, codes_path) == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = cds[0].plateCodeCount + 2;
        stbuf->st_size = 4096;
        stbuf->st_uid = 1000;
        stbuf->st_gid = 1000;
        stbuf->st_atime = time(NULL);
        stbuf->st_mtime = time(NULL);
    } else if ((strcmp(dividedPath[0], "NAMES") == 0) && ((strcmp(dividedPath[3], "\0")) != 0) &&
               (strcmp(dividedPath[4], "\0") == 0)) {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = (sizeof(char) * 200);
        stbuf->st_uid = 1000;
        stbuf->st_gid = 1000;
        stbuf->st_atime = time(NULL);
        stbuf->st_mtime = time(NULL);
    } else if ((strcmp(dividedPath[0], "CODES") == 0) && ((strcmp(dividedPath[2], "\0")) != 0)) {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = (sizeof(char) * 200);
        stbuf->st_uid = 1000;
        stbuf->st_gid = 1000;
        stbuf->st_atime = time(NULL);
        stbuf->st_mtime = time(NULL);
    } else {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        stbuf->st_size = 4096;
        stbuf->st_uid = 1000;
        stbuf->st_gid = 1000;
        stbuf->st_atime = time(NULL);
        stbuf->st_mtime = time(NULL);
    }

    return res;
}

static int postal_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                          off_t offset, struct fuse_file_info *fi) {
    (void) offset;
    (void) fi;
    struct stat stFile;
    struct stat stFolder;

    char dividedPath[5][50];
    memset(dividedPath, 0, sizeof(dividedPath));

    char temp[strlen(path)];
    memset(temp, 0, sizeof(path));
    strcpy(temp, path);


    char *p = strtok(temp, "/");
    int i = 0;


    if (strstr(temp, "."))
        goto out;


    while (p != NULL) {
        strcpy(dividedPath[i++], p);
        p = strtok(NULL, "/");
    }

    out:

    stFolder.st_uid = 1000;
    stFolder.st_gid = 1000;
    stFolder.st_mode = S_IFDIR | 0755;
    stFolder.st_atime = time(NULL);
    stFolder.st_mtime = time(NULL);

    stFile.st_uid = 1000;
    stFile.st_gid = 1000;
    stFile.st_mode = S_IFREG | 0755;
    stFile.st_atime = time(NULL);
    stFile.st_mtime = time(NULL);

    if (strcmp(path, "/") == 0) {

        filler(buf, ".", &stFolder, 0);
        filler(buf, "..", &stFolder, 0);
        filler(buf, names_path + 1, &stFolder, 0);
        filler(buf, codes_path + 1, &stFolder, 0);
        filler(buf, csv_path + 1, &stFile, 0);


    } else if (strcmp(path, names_path) == 0) {//Fill all cities
        int i = 0;
        filler(buf, ".", &stFolder, 0);
        filler(buf, "..", &stFolder, 0);
        for (i; i < nms[0].cityCount; i++) {
            filler(buf, nms[0].cities[i].cityName, &stFolder, 0);
        }

    } else if (strcmp(path, codes_path) == 0) {//Fill all codes
        int i = 0;

        filler(buf, ".", &stFolder, 0);
        filler(buf, "..", &stFolder, 0);
        for (i; i < cds[0].plateCodeCount; i++) {
            filler(buf, cds[0].plateCode[i], &stFolder, 0);
        }
    } else if ((strcmp(dividedPath[0], "NAMES") == 0) && (strcmp(dividedPath[1], "\0") != 0) &&
               (strcmp(dividedPath[2], "\0") == 0)) {//Pushing districts
        filler(buf, ".", &stFolder, 0);
        filler(buf, "..", &stFolder, 0);
        int i = 0, j = 0;
        for (j; j < nms[0].cityCount; j++) {
            if (strcmp(nms[0].cities[j].cityName, dividedPath[1]) == 0) {
                break;
            }
        }
        for (i; i < nms[0].cities[j].districtCount; i++) {
            filler(buf, nms[0].cities[j].dstct[i].districtName, &stFolder, 0);
        }
    } else if ((strcmp(dividedPath[0], "NAMES") == 0) && ((strcmp(dividedPath[2], "\0")) != 0) &&
               (strcmp(dividedPath[3], "\0") == 0)) {//Pushing Neighborhoods
        filler(buf, ".", &stFolder, 0);
        filler(buf, "..", &stFolder, 0);
        int i = 0, j = 0, k = 0;
        for (j; j < nms[0].cityCount; j++) {
            if (strcmp(nms[0].cities[j].cityName, dividedPath[1]) == 0) {
                for (k; k < nms[0].cities[j].districtCount; k++) {
                    if (strcmp(nms[0].cities[j].dstct[k].districtName, dividedPath[2]) == 0) {
                        break;
                    }
                }
                break;
            }
        }
        for (i; i < nms[0].cities[j].dstct[k].neighborhoodCount - 2; i++) {
            stFile.st_uid = 1000;
            stFile.st_gid = 1000;
            stFile.st_mode = S_IFREG | 0755;
            stFile.st_atime = time(NULL); // The last "a"ccess of the file/directory is right now
            stFile.st_mtime = time(NULL); // The last "m"odification of the file/directory is right now
            filler(buf, nms[0].cities[j].dstct[k].pstlcd[i].neighborhood, &stFile, 0);
        }
    } else if ((strcmp(dividedPath[0], "CODES") == 0) && ((strcmp(dividedPath[1], "\0")) != 0) &&
               (strcmp(dividedPath[2], "\0") == 0)) {//Pushing platecodes
        filler(buf, ".", &stFolder, 0);
        filler(buf, "..", &stFolder, 0);
        int j = 0, i = 0, k = 1;
        for (j; j < cds[0].plateCodeCount; j++) {
            if (strcmp(cds[0].plateCode[j], dividedPath[1]) == 0) {
                break;
            }
        }

        for (i; i < nms[0].cities[j].districtCount; i++) {
            for (k; k < nms[0].cities[j].dstct[i].neighborhoodCount - 1; k++) {
                char *temp = malloc(sizeof(char) * 10);
                sprintf(temp, "%d", nms[0].cities[j].dstct[i].pstlcd[k].pcode);
                filler(buf, temp, &stFile, 0);
            }

        }

    }
    return 0;
}

static int postal_read(const char *path, char *buf, size_t size, off_t offset,
                       struct fuse_file_info *fi) {
    size_t len;
    (void) fi;

    char dividedPath[5][50];
    memset(dividedPath, 0, sizeof(dividedPath));

    char temp[strlen(path)];
    memset(temp, 0, sizeof(path));
    strcpy(temp, path);


    char *p = strtok(temp, "/");
    int i = 0;


    if (strstr(path, "."))
        goto out;


    while (p != NULL) {
        strcpy(dividedPath[i++], p);
        p = strtok(NULL, "/");
    }


    out:

    if (strcmp(path, csv_path) == 0) {

        len = strlen(csv_text);
        if (offset < len) {
            if (offset + size > len)
                size = len - offset;
            memcpy(buf, csv_text + offset, size);
        } else
            size = 0;
    } else if ((strcmp(dividedPath[0], "NAMES") == 0) && (strcmp(dividedPath[3], "\0") != 0)) {
        int j = 0;
        int k = 0;
        int l = 0;

        for (j; j < nms[0].cityCount; j++) {
            if (strcmp(nms[0].cities[j].cityName, dividedPath[1]) == 0) {
                for (k; k < nms[0].cities[j].districtCount; k++) {
                    if (strcmp(nms[0].cities[j].dstct[k].districtName, dividedPath[2]) == 0) {
                        for (l; l < nms[0].cities[j].dstct[k].neighborhoodCount; l++) {
                            if (strcmp(nms[0].cities[j].dstct[k].pstlcd[l].neighborhood, dividedPath[3]) == 0) {
                                char text[200];
                                memset(text, 0, sizeof(text));
                                char lat[20];
                                sprintf(lat, "%f", nms[0].cities[j].dstct[k].pstlcd[l].latitude);
                                char lng[20];
                                sprintf(lng, "%f", nms[0].cities[j].dstct[k].pstlcd[l].longitude);
                                char pcd[6];
                                sprintf(pcd, "%d", nms[0].cities[j].dstct[k].pstlcd[l].pcode);

                                strcat(text, "code: ");
                                strcat(text, pcd);
                                strcat(text, "\nneighborhood: ");
                                strcat(text, nms[0].cities[j].dstct[k].pstlcd[l].neighborhood);
                                strcat(text, "\ncity: ");
                                strcat(text, nms[0].cities[j].dstct[k].pstlcd[l].city);
                                strcat(text, "\ndistrict: ");
                                strcat(text, nms[0].cities[j].dstct[k].pstlcd[l].district);
                                strcat(text, "\nlatitude: ");
                                strcat(text, lat);
                                strcat(text, "\nlongitude: ");
                                strcat(text, lng);
                                len = strlen(text);
                                if (offset < len) {
                                    if (offset + size > len)
                                        size = len - offset;
                                    memcpy(buf, text, size);
                                    //free(text);
                                } else {
                                    size = 0;
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }
    } else if ((strcmp(dividedPath[0], "CODES") == 0) && (strcmp(dividedPath[2], "\0") != 0)) {
        int j = 0;
        int k = 0;
        int l = 0;
        for (j; j < 36307; j++) {
            if (atoi(dividedPath[2]) == postalCodes[j].pcode) {
                char text[200];
                memset(text, 0, sizeof(text));
                char lat[20];
                sprintf(lat, "%f", postalCodes[j].latitude);
                char lng[20];
                sprintf(lng, "%f", postalCodes[j].longitude);
                char pcd[6];
                sprintf(pcd, "%d", postalCodes[j].pcode);

                strcat(text, "code: ");
                strcat(text, pcd);
                strcat(text, "\nneighborhood: ");
                strcat(text, postalCodes[j].neighborhood);
                strcat(text, "\ncity: ");
                strcat(text, postalCodes[j].city);
                strcat(text, "\ndistrict: ");
                strcat(text, postalCodes[j].district);
                strcat(text, "\nlatitude: ");
                strcat(text, lat);
                strcat(text, "\nlongitude: ");
                strcat(text, lng);
                len = strlen(text);
                if (offset < len) {
                    if (offset + size > len)
                        size = len - offset;
                    memcpy(buf, text, size);
                    //free(text);
                } else {
                    size = 0;
                }
            }
        }
    }

    return size;
}

static int postal_open(const char *path, struct fuse_file_info *fi) {
    return 0;
}

static int postal_rename(const char *from, const char *to) {
    (void) from;
    (void) to;

    int res;

    res = rename(from, to);
    if (res == -1)
        return -errno;

    return 0;
}

static struct fuse_operations postal_oper = {
        .getattr    = postal_getattr,
        .readdir    = postal_readdir,
        .open       = postal_open,
        .read       = postal_read,
        .rename     = postal_rename,

        .readlink    = NULL,
        .mknod       = NULL,
        .mkdir       = NULL,
        .symlink     = NULL,
        .unlink      = NULL,
        .rmdir       = NULL,
        .link        = NULL,
        .chmod       = NULL,
        .chown       = NULL,
        .truncate    = NULL,
        .utime       = NULL,
        .write       = NULL,
        .statfs      = NULL,
        .release     = NULL,
        .fsync       = NULL,
        .access      = NULL,

        /* Extended attributes support for userland interaction */
        .setxattr    = NULL,
        .getxattr    = NULL,
        .listxattr   = NULL,
        .removexattr = NULL,
};

int main(int argc, char *argv[]) {

    nms = malloc(sizeof(names));
    cds = malloc(sizeof(codes));
    FILE *f = fopen("postal-codes.csv", "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    csv_text = malloc(fsize + 1);
    fread(csv_text, fsize, 1, f);
    fclose(f);
    char *temp;
    temp = malloc(fsize + 1);
    strcpy(temp, csv_text);
    postalCodes = malloc(36307 * sizeof(postalCode));
    int i = 0;
    printf("fsize = %ld\n", fsize);
    for (i; i < 36307; i++) {
        postalCodes[i].pcode = atoi(strsep(&temp, "\t"));
        postalCodes[i].neighborhood = strsep(&temp, "\t");
        postalCodes[i].city = strsep(&temp, "\t");
        postalCodes[i].district = strsep(&temp, "\t");
        postalCodes[i].latitude = atof(strsep(&temp, "\t"));
        postalCodes[i].longitude = atof(strsep(&temp, "\n"));
    }

    ///////////////////////////////////////////////////////////
    //Calculating cities.
    int j = 0;
    char *tempCity;
    char *tempDistrict;
    tempDistrict = malloc(50 * sizeof(char *));
    tempCity = malloc(30 * sizeof(char *));


    ///////////////////////////////////////////////////////////////////
    /*counting cities, districts, neighborhoods.*/
//counting city number.
    int cityCounter = 1;
    int districtCounter = 1;
    strcpy(tempCity, postalCodes[0].city);
    strcpy(tempDistrict, postalCodes[0].district);
    for (j; j < 36307; j++) {
        if (strcmp(tempCity, postalCodes[j].city) != 0) {
            strcpy(tempCity, postalCodes[j].city);
            cityCounter++;
        }
    }
    nms[0].cityCount = cityCounter;
    nms[0].cities = malloc(cityCounter * sizeof(city));

//counting district and neighborhood number
    cityCounter = 1;
    int neighborhoodCounter = 1;
    int tempArray[45];
    memset(tempArray, 0, sizeof(tempArray));
    j = 0;
    strcpy(tempCity, postalCodes[0].city);
    strcpy(tempDistrict, postalCodes[0].district);
    for (j; j < 36307; j++) {
        if (strcmp(tempCity, postalCodes[j].city) == 0) {//same city
            if (strcmp(tempDistrict, postalCodes[j].district) != 0) {// same city different district
                strcpy(tempDistrict, postalCodes[j].district);
                tempArray[districtCounter - 1] = neighborhoodCounter;
                districtCounter++;
                neighborhoodCounter = 1;
            } else {
                neighborhoodCounter++;
            }
        } else {//different city
            strcpy(tempCity, postalCodes[j].city);
            nms[0].cities[cityCounter - 1].districtCount = districtCounter;
            tempArray[districtCounter - 1] = neighborhoodCounter;
            nms[0].cities[cityCounter - 1].dstct = malloc(districtCounter * sizeof(district));
            neighborhoodCounter = 1;
            int k = 0;
            for (k; k < districtCounter; k++) {
                nms[0].cities[cityCounter - 1].dstct[k].neighborhoodCount = tempArray[k];
                nms[0].cities[cityCounter - 1].dstct[k].pstlcd = malloc(sizeof(postalCode) * tempArray[k]);
            }
            memset(tempArray, 0, sizeof(tempArray));
            districtCounter = 0;

            cityCounter++;
        }
    }
    nms[0].cities[cityCounter - 1].dstct = malloc(districtCounter * sizeof(district));
    tempArray[districtCounter - 1] = neighborhoodCounter - 1;
    int k = 0;
    for (k; tempArray[k] != 0; k++) {
        nms[0].cities[cityCounter - 1].dstct[k].neighborhoodCount = tempArray[k];
        nms[0].cities[cityCounter - 1].dstct[k].pstlcd = malloc(sizeof(postalCode) * tempArray[k]);
    }
    nms[0].cities[cityCounter - 1].districtCount = districtCounter;

//////////////////////////////////////////////////////////////////////////
    //pushing city and district names to the struct.
    j = 0;
    i = 0;

    strcpy(tempDistrict, postalCodes[0].district);
    strcpy(tempCity, postalCodes[0].city);
    nms[0].cities[0].cityName = malloc(30 * sizeof(char *));
    strcpy(nms[0].cities[0].cityName, tempCity);
    districtCounter = 0;
    nms[0].cities[0].dstct[0].districtName = malloc(50 * sizeof(char *));
    strcpy(nms[0].cities[0].dstct[0].districtName, tempDistrict);
    districtCounter++;
    int l = 0;
    for (j; j < 36307; j++) {//number of entries will be counted later. solve issue hardcoding.

        if (strcmp(tempCity, postalCodes[j].city) == 0) {//same city
            if (strcmp(tempDistrict, postalCodes[j].district) != 0) {//different district
                strcpy(tempDistrict, postalCodes[j].district);
                nms[0].cities[i].dstct[districtCounter].districtName = malloc(50 * sizeof(char *));
                strcpy(nms[0].cities[i].dstct[districtCounter].districtName, tempDistrict);
                districtCounter++;
                l = 0;
            } else {
                nms[0].cities[i].dstct[districtCounter - 1].pstlcd[l] = postalCodes[j];
                l++;
            }
        } else {//different city
            i++;
            nms[0].cities[i].cityName = malloc(30 * sizeof(char *));
            //nms[0].cities[i].dstct = (district *) malloc(nms[0].cities[i].districtCount * sizeof(district));
            strcpy(tempCity, postalCodes[j].city);
            strcpy(nms[0].cities[i].cityName, tempCity);
            districtCounter = 0;
        }
    }
    nms[0].cities[i].cityName = malloc(30 * sizeof(char *));
    strcpy(nms[0].cities[i].cityName, tempCity);

//////////////////////////////////////////////////////////////////////////////////////
//pushing platecodes to the struct

    cds[0].plateCodeCount = nms[0].cityCount;
    cds[0].pstlcd = malloc(sizeof(postalCodes) * cds[0].plateCodeCount);

    int n = 0;
    //int p = 0;

    for (n; n < nms[0].cityCount; n++) {
        cds[0].plateCode[n] = malloc(sizeof(char) * 3);
        sprintf(cds[0].plateCode[n], "%d", nms[0].cities[n].dstct[0].pstlcd[0].pcode / 1000);
    }

//////////////////////////////////////////////////////////////////////////////////////
    //Printing cities
    /*i = 0;
    j = 0;
    k = 0;
    for (i; i < nms[0].cityCount; i++) {
        printf("CityName = %s DistrictCount = %d\n", nms[0].cities[i].cityName, nms[0].cities[i].districtCount);
        for (j; j < nms[0].cities[i].districtCount; j++) {
            printf("\t%d - DistrictName = %s - NeighborhoodCount = %i\n", j,
                   nms[0].cities[i].dstct[j].districtName, nms[0].cities[i].dstct[j].neighborhoodCount);
        }
        j = 0;
    }*/
///////////////////////////////////////////////////////////////////////////////////////////////////////



    char *path = "/NAMES/Adana";

    free(tempCity);
    free(tempDistrict);
    return fuse_main(argc, argv, &postal_oper, NULL);
}