#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

int hextoi[255];

typedef struct pixel_st {
    int r, g, b;
} PIXEL;

// GENERAL
void print_usage (char *arg) {
    printf("Upotreba: %s [-s SIZE] [-b COLOR] SVG_FILE PPM_FILE\n", arg);
}
FILE *safe_fopen(char *file, char *opt);
void check_len(char *str, int max_len);
void read_cli(int argc, char **argv, int *N, int *M, PIXEL *bg, FILE **svg, FILE **ppm);
void print_pixel(PIXEL p) {
    printf("%d %d %d\n", p.r, p.g, p.b);
}
void read_svg_header(FILE *svg, int *width, int *height);

// OBRADA
PIXEL *create_image(PIXEL color, int width, int height);
PIXEL get_color(char *color);
void draw_line(PIXEL *img, int width, int x1, int y1, int x2, int y2, PIXEL color);
void read_lines(FILE *svg, PIXEL *img, int width, int height, double sx, double sy);
void print_image(FILE *ppm, PIXEL *img, int width, int height);

int main(int argc, char **argv) {
    for (int i = '0'; i <= '9'; ++i) {
        hextoi[i] = i - 48;
    }
    for (int i = 'a'; i <= 'f'; ++i) {
        hextoi[i] = 10 + i - 97;
    }

    FILE *svg = NULL;
    FILE *ppm = NULL;
    int N = 0, M = 0;
    PIXEL bg = {
        .r = 255,
        .g = 255,
        .b = 255
    };
    read_cli(argc, argv, &N, &M, &bg, &svg, &ppm);

    // printf("BG color : ");
    // print_pixel(bg);

    if (svg == NULL || ppm == NULL) {
        printf("Greska pri ucitavanju fajlova.\n");
        exit(EXIT_FAILURE);
    }

    int width = 0, height = 0;
    double scale_x = 1.0, scale_y = 1.0;
    read_svg_header(svg, &width, &height);
    // printf("%d - %d\n", width, height);
    if (N != 0 && M != 0) {
        scale_x = 1.0 * N / width;
        scale_y = 1.0 * M / height;

        width = N;
        height = M;
    }
    // printf("sx = %lf\tsy = %lf\n", scale_x, scale_y);
    PIXEL *img = create_image(bg, width, height);
    // printf("a\n");

    read_lines(svg, img, width, height, scale_x, scale_y);
    // printf("a\n");

    print_image(ppm, img, width, height);

    fclose(svg);
    fclose(ppm);
    free(img);

    return 0;
}

FILE *safe_fopen(char *file, char *opt) {
    FILE *ret = fopen(file, opt);

    if (ret == NULL) {
        printf("Failed to open file %s\n", file);
        exit(EXIT_FAILURE);
    }

    return ret;
}

void check_len(char *str, int max_len) {
    if (strlen(str) > max_len) {
        printf("Unet predugacak string.\n");
        exit(EXIT_FAILURE);
    }
}

void read_cli(int argc, char **argv, int *N, int *M, PIXEL *bg, FILE **svg, FILE **ppm)
{
    int option;
    char *dim;
    char *endptr;
    while ((option = getopt(argc, argv, "s:b:")) != -1) {
        switch (option) {
            case 's':
                dim = strtok(optarg, "x");
                *N = strtol(dim, &endptr, 10);
                if (*endptr != 0) {
                    printf("Nekorektno unete dimenzije.\n");
                    exit(EXIT_FAILURE);
                }

                dim = strtok(NULL, "x");

                if (dim == NULL) {
                    printf("Nekorektno unete dimenzije.\n");
                    exit(EXIT_FAILURE);
                }

                *M = strtol(dim, &endptr, 10);
                if (*endptr != 0) {
                    printf("Nekorektno unete dimenzije.\n");
                    exit(EXIT_FAILURE);
                }

                if (*N == 0 || *M == 0) {
                    printf("Nekorektno unete dimenzije.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'b':
                *bg = get_color(optarg);
                
                break;
        }
    }

    if (argc - optind != 2) {
        printf("Potrebno je uneti ulazni SVG fajl i izlazni PPM fajl.\n");
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    *svg = safe_fopen(argv[optind    ], "r");
    *ppm = safe_fopen(argv[optind + 1], "wb");
}

PIXEL *create_image(PIXEL color, int width, int height) {
    PIXEL *img = (PIXEL *)malloc(width * height * sizeof(PIXEL));

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            img[i * width + j] = color;
        }
    }

    return img;
}

PIXEL get_color(char *color) {
    PIXEL ret;
    if (color[0] == '#') {
        check_len(optarg, 7);

        ret.r = hextoi[color[1]] * 16 + hextoi[color[2]];
        ret.g = hextoi[color[3]] * 16 + hextoi[color[4]];
        ret.b = hextoi[color[5]] * 16 + hextoi[color[6]];
    }
    else {
        check_len(color, 7);

        if (strcmp(color, "black") == 0) {
            ret.r = 0;
            ret.g = 0;
            ret.b = 0;
        }
        else if (strcmp(color, "red") == 0) {
            ret.r = 255;
            ret.g = 0;
            ret.b = 0;
        }
        else if (strcmp(color, "green") == 0) {
            ret.r = 0;
            ret.g = 255;
            ret.b = 0;
        }
        else if (strcmp(color, "blue") == 0) {
            ret.r = 0;
            ret.g = 0;
            ret.b = 255;
        }
        else if (strcmp(color, "cyan") == 0) {
            ret.r = 0;
            ret.g = 255;
            ret.b = 255;
        }
        else if (strcmp(color, "yellow") == 0) {
            ret.r = 255;
            ret.g = 255;
            ret.b = 0;
        }
        else {
            printf("Uneta nepoznata boja.\n");
            exit(EXIT_FAILURE);
        }
    }

    return ret;
}

void read_svg_header(FILE *svg, int *width, int *height) {
    char header[150];
    char *str;

    fgets(header, 150, svg);

    str = strtok(header, " =");
    
    if (strcmp(str, "<svg") != 0) {
        printf("Nekorektno formatiranje SVG slike.\n");
        exit(EXIT_FAILURE);
    }

    while ((str = strtok(NULL, " =")) != NULL) {
        if (strcmp(str, "width") == 0) {
            str = strtok(NULL, " =");
            *width = strtol(str + 1, NULL, 10);
        }
        else if (strcmp(str, "height") == 0) {
            str = strtok(NULL, " =");
            *height = strtol(str + 1, NULL, 10);
        }
    }

    if (*width == 0 || *height == 0) {
        printf("Greska pri citanju sirine i visine SVGa.\n");
        exit(EXIT_FAILURE);
    }
}

// void set_pixel(PIXEL *img, int width, int x, int y, PIXEL color) {
//     img[y * width + x] = color;
// }

void draw_line(PIXEL *img, int width, int x0, int y0, int x1, int y1, PIXEL color) {
    // printf("usao\n");
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        // printf("besk ptlj??\n\n");
        // set_pixel(img, width, x0, y0, color);
        img[y0 * width + x0] = color;
        if (x0 == x1 && y0 == y1)
            break;
        
        e2 = 2 * err;
        if (e2 > dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void read_lines(FILE *svg, PIXEL *img, int width, int height, double sx, double sy) {
    char line[150];
    char *s;
    int x1 = -1, y1 = -1, x2 = -1, y2 = -1;
    int done = 0;
    while (!feof(svg)) {
        fgets(line, 150, svg);
        // printf("Linija:\n%s", line);
        s = strtok(line, " =");
        if (strcmp(s, "</svg>") == 0) {
            done = 1;
            break;
        }
        if (strcmp(s, "<line") != 0) {
            printf("Greska.\n");
            exit(EXIT_FAILURE);
        }
        while ((s = strtok(NULL, " =")) != NULL) {
            // printf("%s\n", s);
            if (strcmp(s, "x1") == 0) {
                s = strtok(NULL, " =");
                x1 = sx * strtol(s + 1, NULL, 10);
            }
            else if (strcmp(s, "y1") == 0) {
                s = strtok(NULL, " =");
                y1 = sy * strtol(s + 1, NULL, 10);
            }
            else if (strcmp(s, "x2") == 0) {
                s = strtok(NULL, " =");
                x2 = sx * strtol(s + 1, NULL, 10);
            }
            else if (strcmp(s, "y2") == 0) {
                s = strtok(NULL, " =");
                y2 = sy * strtol(s + 1, NULL, 10);
            }
            else if (strcmp(s, "stroke") == 0) {
                s = strtok(NULL, " =");
                s[strlen(s) - 1] = '\0';
                // printf("%s\n", s + 1);
                if (x1 == -1 || y1 == -1 || x2 == -1 || y2 == -1) {
                    printf("Pogresno formatirana linija SVGa.\n");
                    exit(EXIT_FAILURE);
                }
                draw_line(img, width, x1, y1, x2, y2, get_color(s + 1));
            }
        }
    }
    if (!done) {
        printf("Slika mora imati zatvarajuci </svg> tag na kraju.\n");
        exit(EXIT_FAILURE);
    }
}

void print_image(FILE *ppm, PIXEL *img, int width, int height) {
    fprintf(ppm, "P6\n");
    fprintf(ppm, "%d %d\n", width, height);
    fprintf(ppm, "255\n");
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            fputc(img[i * width + j].r, ppm);
            fputc(img[i * width + j].g, ppm);
            fputc(img[i * width + j].b, ppm);
        }
    }
}