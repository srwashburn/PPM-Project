#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int width;
int height;

typedef struct Pixel {

    unsigned char r, g, b;

 }   Pixel;



Pixel* readP3(char *fname){

    FILE *f;
    f = fopen(fname, "r");


    if(fgetc(f) != 'P'){
        fprintf(stderr, "Error: This is not a PPM file!");
        exit(1);
    }
    if(fgetc(f) != '3'){
        fprintf(stderr, "Error: This file is not in P3 format");
        exit(1);
    }

    unsigned char c;
    int specs[3]; //array to hold file data parsed from header: specs[0] = height, specs[1] = width, specs[2] = max color value.
    int i = 0;
    while(i < 3){
        c = fgetc(f);
        while(c != ' ' && c != '\n'){
            if(c == '#'){ //filter out comments
                while(c != '\n'){
                      c = fgetc(f);
                }
            }else{
                char *intbuff;
                intbuff = malloc(sizeof(char) * 3);
                int j = 0;
                while(c != ' ' && c != '\n'){
                    intbuff[j] = c;
                    j++;
                    c = fgetc(f);
                }
                specs[i] = atoi(intbuff);
                i++;
            }
        }
    }

    if(specs[2] != 255){
        fprintf(stderr, "Error: The image is not 8-bit per channel.");
        exit(1);
    }

    width = specs[0];
    height = specs[1];

    Pixel *buffer;
    buffer = malloc(sizeof(Pixel)*width*height);

    int pixelcount = 0;
    while(!feof(f)){
        int i = 0;
        int* rgb;
        rgb = malloc(sizeof(int)*3);
        while(i < 3){
            c = fgetc(f);
            int j = 0;
            char *intbuff;
            intbuff = malloc(sizeof(char) * 3);
            while(c != ' ' && c != '\n' && !feof(f)){
                if(isdigit(c) == 0){
                    fprintf(stderr, "Unexpected character encountered in image data!");
                    exit(1);
                }
                intbuff[j] = c;
                c = fgetc(f);
                j++;
            }
            rgb[i] = atoi(intbuff);
            i++;
        }

        Pixel p;
        p.r = rgb[0];
        p.g = rgb[1];
        p.b = rgb[2];

        buffer[pixelcount] = p;
        pixelcount++;

    }

    fclose(f);

    return buffer;

}

Pixel* readP6(char* fname){
    Pixel* buffer;

    FILE *f;
    f = fopen(fname, "rb");


    if(fgetc(f) != 'P'){
        fprintf(stderr, "Error: This is not a PPM file!");
        exit(1);
    }
    if(fgetc(f) != '6'){
        fprintf(stderr, "Error: This file is not in P6 format!");
        exit(1);
    }

    int specs[3]; //array holding file data parsed from header: specs[0] = height, specs[1] = width, specs[2] = max color value.
    int i = 0;
    while(i < 3){
        unsigned char c = fgetc(f);
        while(c != ' ' && c != '\n'){
            if(c == '#'){ //filter out comments
                while(c != '\n'){
                      c = fgetc(f);
                }
            }else{
                char *intbuff;
                intbuff = malloc(sizeof(char) * 4);
                int j = 0;
                while(c != ' ' && c != '\n'){
                    intbuff[j] = c;
                    j++;
                    c = fgetc(f);
                }
                specs[i] = atoi(intbuff);
                i++;
            }
        }
    }

    if(specs[2] != 255){
        fprintf(stderr, "Error: The image is not 8-bit per channel.");
        exit(1);
    }

    width = specs[0];
    height = specs[1];

    buffer = malloc(sizeof(Pixel)*width*height);

    int j = 0;
    while(j < width*height){
        unsigned char * rgb;
        rgb = malloc(sizeof(unsigned char)*4);
        fread(rgb, 1, 3, f);
        Pixel p;
        p.r = rgb[0];
        p.g = rgb[1];
        p.b = rgb[2];
        buffer[j] = p;
        j++;
    }

    fclose(f);

    return buffer;





}

int writeP3(char* fname, Pixel* buffer){
    FILE* fh;
    fh = fopen(fname, "w");

    fprintf(fh, "P3\n");
    fprintf(fh, "%d ", width);
    fprintf(fh, "%d\n", height);
    fprintf(fh, "%d\n", 255);

    int i;
    for(i = 0; i < width*height; i++){
        fprintf(fh, "%d\n", buffer[i].r);
        fprintf(fh, "%d\n", buffer[i].g);
        fprintf(fh, "%d\n", buffer[i].b);
    };

    fclose(fh);


}

int writeP6(char* fname, Pixel* buffer){
    FILE* fh;
    fh = fopen(fname, "wb");

    fprintf(fh, "P6\n");
    fprintf(fh, "%d ", width);
    fprintf(fh, "%d\n", height);
    fprintf(fh, "%d\n", 255);

    int i;
    for(i = 0; i < width*height; i++){
        unsigned char* rgb;
        rgb = malloc(sizeof(unsigned char)*64);
        rgb[0] = buffer[i].r;
        rgb[1] = buffer[i].g;
        rgb[2] = buffer[i].b;
        fwrite(rgb, 1, 3, fh);

    };

    fclose(fh);


}

int main(int argc, char *argv[]){


    if(argc != 4){
        fprintf(stderr, "Incorrect usage of arguments: 6/3 input.ppm output.ppm");
        return 1;
    }

    if(strcmp(argv[1], "3") != 0 && strcmp(argv[1], "6") != 0){
        fprintf(stderr, "First argument should be 3 or 6");
        return 1;
    }



    if(strcmp(argv[1], "3")==0){
        FILE * fh;
        fh = fopen(argv[2], "r");
        unsigned char c = fgetc(fh);
        if(c != 'P' && c != 'p'){
            fprintf(stderr, "Input file not PPM file!");
            return 1;
        }
        c = fgetc(fh);
        if(c == '3'){
            writeP3(argv[3], readP3(argv[2]));
             fclose(fh);
        }else if(c == '6'){
            writeP3(argv[3], readP6(argv[2]));
             fclose(fh);
        }else{
            fprintf(stderr, "Input file not in P3 or P6 format!");
            return 1;
        }

    }else if(strcmp(argv[1],"6")==0){
        FILE * fh;
        fh = fopen(argv[2], "r");
        unsigned char c = fgetc(fh);
        if(c != 'P' && c != 'p'){
            fprintf(stderr, "Input file not PPM file!");
            return 1;
        }
        c = fgetc(fh);
        if(c == '3'){
            writeP6(argv[3], readP3(argv[2]));
             fclose(fh);
        }else if(c == '6'){
            writeP6(argv[3], readP6(argv[2]));
             fclose(fh);
        }else{
            fprintf(stderr, "Input file not in P3 or P6 format!");
            return 1;
        }
    }else{
        fprintf(stderr, "Incorrect input format: First argument must be 3 or 6.");
        return 1;
    }

    printf("Success!");
    return 0;



    /*
    Pixel* buffer = readP3("P3test.ppm");

    writeP3("testout3.ppm", buffer);

    */

}





