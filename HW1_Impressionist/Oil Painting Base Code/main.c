#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

//Print info about how to use the app
void usage(){
    printf( "Usage: progName <infile> <outfile> [parallel] [threshold] [filter]\n" );
    printf( "<infile> = Input bmp file\n" );
    printf( "<outfile> = Output bmp file\n" );
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        usage();
        return 1;
    }
    
    char *inputFileName = argv[1];
    char *outputFileName = argv[2];
    
    Image *image = (Image *) malloc(sizeof(Image));
    if (image == NULL) {
        fprintf(stderr, "Out of memory\n");
        return(-1);
    }
    
    if (ImageLoad(inputFileName, image)==-1) {
        printf ("Can't read the image\n");
        return -1;
    }
    
    struct timeval startTimeOfDay, finishTimeOfDay;
    gettimeofday (&startTimeOfDay, NULL);
    
    printf("Apply Filter\n");
    int x,y;
    for (y=0;y<3*image->sizeY;y+=3) {
        for (x=0;x<3*image->sizeX;x+=3) {
            int i = y*image->sizeX + x;
//            image->data[i] = 0;
            int tmp = image->data[i];
            image->data[i] = image->data[i+2];
            image->data[i+1] = image->data[i+1];
            image->data[i+1] = tmp;
        }
    }
    
    if (ImageSave(outputFileName, image)==-1) return(-1);
    free(image);
    return 0;
}