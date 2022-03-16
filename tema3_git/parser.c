#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmp_header.h"

struct Pixel {
    unsigned char rosu, verde, albastru;
};

struct Punct {
    int coordX, coordY;
};

void citeste_header(bmp_fileheader * im_fd, bmp_infoheader * im_info, FILE * imagine) {
    fread(&im_fd->fileMarker1, sizeof(unsigned char), 1, imagine);
    fread(&im_fd->fileMarker2, sizeof(unsigned char), 1, imagine);
    fread(&im_fd->bfSize, sizeof(unsigned int), 1, imagine);
    fread(&im_fd->unused1, sizeof(unsigned short), 1, imagine);
    fread(&im_fd->unused2, sizeof(unsigned short), 1, imagine);
    fread(&im_fd->imageDataOffset, sizeof(unsigned int), 1, imagine);
    fread(&im_info->biSize, sizeof(unsigned int), 1, imagine);
    fread(&im_info->width, sizeof(signed int), 1, imagine);
    fread(&im_info->height, sizeof(signed int), 1, imagine);
    fread(&im_info->planes, sizeof(unsigned short), 1, imagine);
    fread(&im_info->bitPix, sizeof(unsigned short), 1, imagine);
    fread(&im_info->biCompression, sizeof(unsigned int), 1, imagine);
    fread(&im_info->biSizeImage, sizeof(unsigned int), 1, imagine);
    fread(&im_info->biXPelsPerMeter, sizeof(int), 1, imagine);
    fread(&im_info->biYPelsPerMeter, sizeof(int), 1, imagine);
    fread(&im_info->biClrUsed, sizeof(unsigned int), 1, imagine);
    fread(&im_info->biClrImportant, sizeof(unsigned int), 1, imagine);
}

void deseneaza_punct(int line_width, struct Pixel colors, struct Pixel **mat
, int x, int y, int y_max, int x_max) {
    for (int i = x - line_width / 2; i <= x + line_width / 2; i++) {
        for (int j = y - line_width / 2; j <= y + line_width / 2; j++) {
            if (i < x_max && j < y_max && i >= 0 && j >= 0) mat[i][j] = colors;
        }
    }
}

void deseneaza_linie(int line_width, struct Pixel colors, struct Pixel **mat
, int y_max, int x_max, int x1, int y1, int x2, int y2) {
                deseneaza_punct(line_width, colors, mat, x1, y1, y_max, x_max);
                deseneaza_punct(line_width, colors, mat, x2, y2, y_max, x_max);
                if (x1 == x2) {
                    for (int i = y1; i <= y2; i++) {
                        deseneaza_punct(line_width, colors, mat, x1, i, y_max, x_max);
                    }
                } else if (y1 == y2) {
                    for (int i = x1; i <= x2; i++) {
                        deseneaza_punct(line_width, colors, mat, i, y1, y_max, x_max);
                    }
                } else {
                    if (abs(x1 - x2) < abs(y1 - y2)) {
                        if (y1 < y2) {
                            for (int j = y1; j <= y2; j++) {
                                int i = ((j - y1) * (x2 - x1) + x1 * (y2 - y1)) / (y2 - y1);
                                deseneaza_punct(line_width, colors, mat, i, j, y_max, x_max);
                            }
                        } else {
                            for (int j = y2; j <= y1; j++) {
                                int i = ((j - y1) * (x2 - x1) + x1 * (y2 - y1)) / (y2 - y1);
                                deseneaza_punct(line_width, colors, mat, i, j, y_max, x_max);
                            }
                        }
                    } else {
                        if (x1 < x2) {
                            for (int i = x1; i <= x2; i++) {
                                int j = ((i - x1) * (y2 - y1) + y1 * (x2 - x1)) / (x2 - x1);
                                deseneaza_punct(line_width, colors, mat, i, j, y_max, x_max);
                            }
                        } else {
                            for (int i = x2; i <= x1; i++) {
                                int j = ((i - x1) * (y2 - y1) + y1 * (x2 - x1)) / (x2 - x1);
                                deseneaza_punct(line_width, colors, mat, i, j, y_max, x_max);
                            }
                        }
                    }
                }
}

void umplere(struct Punct p, struct Pixel color, struct Pixel init, struct Pixel **mat, int x_max, int y_max) {
    if (p.coordX >= 0 && p.coordX < x_max && p.coordY >= 0 && p.coordY < y_max) {
        if (mat[p.coordX][p.coordY].rosu == init.rosu && mat[p.coordX][p.coordY].albastru == init.albastru
         && mat[p.coordX][p.coordY].verde == init.verde) {
            mat[p.coordX][p.coordY] = color;
            p.coordX = p.coordX + 1;
            umplere(p, color, init, mat, x_max, y_max);
            p.coordX = p.coordX - 1;
            p.coordY = p.coordY + 1;
            umplere(p, color, init, mat, x_max, y_max);
            p.coordX = p.coordX - 1;
            p.coordY = p.coordY - 1;
            umplere(p, color, init, mat, x_max, y_max);
            p.coordX = p.coordX + 1;
            p.coordY = p.coordY - 1;
            umplere(p, color, init, mat, x_max, y_max);
        }
    }
}

int main() {
    struct Pixel **mat = NULL, colors;
    bmp_fileheader im_fd;
    bmp_infoheader im_info;
    int line_width = 1;
    #define DIM 1000
    char cuv[DIM];
    while (1) {
        scanf("%s", cuv);
        if (strcmp(cuv, "save") == 0) {
            scanf("%s", cuv);
            FILE *test = fopen(cuv, "wb");
            fwrite(&im_fd, sizeof(bmp_fileheader), 1, test);
            fwrite(&im_info, sizeof(bmp_infoheader), 1, test);
            int padding = im_info.width % 4;
            for (int i=0; i < im_info.height; i++) {
                for (int j=0; j < im_info.width; j++) {
                fwrite(&mat[i][j], 3, 1, test);
                }
                for (int k = 1; k <= padding; k++) {
                    char c = '\0';
                    fwrite(&c, sizeof(char), 1, test);
                }
            }
            for (int i=0; i < im_info.height; i++) free(mat[i]);
            free(mat);
            fclose(test);
        } else if (strcmp(cuv, "edit") == 0) {
            scanf("%s", cuv);
            FILE *imagine = fopen(cuv, "rb");
            citeste_header(&im_fd, &im_info, imagine);
            int padding = im_info.width % 4;
            mat = malloc(im_info.height * sizeof(struct Pixel*));
            for (int i = 0; i < im_info.height; i++) {
                mat[i] = malloc(im_info.width * sizeof(struct Pixel));
            }
            for (int i=0; i < im_info.height; i++) {
                for (int j=0; j < im_info.width; j++) {
                fread(&mat[i][j], 3, 1, imagine);
                }
                fseek(imagine, padding, 1);
            }
            fclose(imagine);
        } else if (strcmp(cuv, "insert") == 0) {
            int x = 0, y = 0;
            scanf("%s", cuv);
            scanf("%d", &x);
            scanf("%d", &y);
            bmp_fileheader test_fd;
            bmp_infoheader test_info;
            FILE *imagine = fopen(cuv, "rb");
            citeste_header(&test_fd, &test_info, imagine);
            int padding = test_info.width % 4;
            for (int i=y; i < test_info.height+y; i++) {
                for (int j=x; j < test_info.width+x; j++) {
                    if (i >= im_info.height || j >= im_info.width) {
                        fseek(imagine, 3, 1);
                    } else {
                        fread(&mat[i][j], 3, 1, imagine);
                    }
                    fseek(imagine, padding, 1);
                }
            }
            fclose(imagine);
        } else if (strcmp(cuv, "set") == 0) {
            scanf("%s", cuv);
            if (strcmp(cuv, "draw_color") == 0) {
                int r = 0, g = 0, b = 0;
                scanf("%d", &b);
                scanf("%d", &g);
                scanf("%d", &r);
                colors.rosu = r;
                colors.verde = g;
                colors.albastru = b;
            } else if (strcmp(cuv, "line_width") == 0) {
                scanf("%d", &line_width);
            }
        } else if (strcmp(cuv, "draw") == 0) {
            scanf("%s", cuv);
            if (strcmp(cuv, "line") == 0) {
                int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
                scanf("%d%d%d%d", &y1, &x1, &y2, &x2);
                deseneaza_linie(line_width, colors, mat, im_info.width, im_info.height, x1, y1, x2, y2);
            } else if (strcmp(cuv, "rectangle") == 0) {
                int x1 = 0, y1 = 0, ww = 0, hh = 0;
                scanf("%d%d%d%d", &y1, &x1, &hh, &ww);
                deseneaza_linie(line_width, colors, mat, im_info.width, im_info.height, x1, y1, x1, y1+hh);
                deseneaza_linie(line_width, colors, mat, im_info.width, im_info.height, x1, y1, x1+ww, y1);
                deseneaza_linie(line_width, colors, mat, im_info.width, im_info.height, x1+ww, y1, x1+ww, y1+hh);
                deseneaza_linie(line_width, colors, mat, im_info.width, im_info.height, x1, y1+hh, x1+ww, y1+hh);
            } else if (strcmp(cuv, "triangle") == 0) {
                int x1 = 0, y1 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0;
                scanf("%d%d%d%d%d%d", &y1, &x1, &y2, &x2, &y3, &x3);
                deseneaza_linie(line_width, colors, mat, im_info.width, im_info.height, x1, y1, x2, y2);
                deseneaza_linie(line_width, colors, mat, im_info.width, im_info.height, x2, y2, x3, y3);
                deseneaza_linie(line_width, colors, mat, im_info.width, im_info.height, x3, y3, x1, y1);
            }
        } else if (strcmp(cuv, "fill") == 0) {
            int x = 0, y = 0;
            struct Punct punct;
            scanf("%d%d", &y, &x);
            punct.coordX = x;
            punct.coordY = y;
            umplere(punct, colors, mat[x][y], mat, im_info.height, im_info.width);
        } else if (strcmp(cuv, "quit") == 0) {
            break;
        }
    }
}
