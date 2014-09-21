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
            //image->data[i] = 0;
            int tmp = image->data[i];
            image->data[i] = image->data[i+1];
            image->data[i+2] = image->data[i+2];
            image->data[i+1] = tmp;
        }
    }
    
//for each pixle
    for (y=0;y<3*image->sizeY;y+=3) {
        for (x=0;x<3*image->sizeX;x+=3){
            int *intensityCount = (int *) malloc( 20);
            int *averageR = (int *) malloc( 20);
            int *averageG = (int *) malloc( 20);
            int *averageB = (int *) malloc( 20);

            int tempX, tempY, finX, finY;

            if(y < 15){
                tempY = 0;
            }
            else{
                tempY = y - 15;
            }
            if(x < 15){
                tempX = 0;
            }
            else{
                tempX = x - 15;
            }
            if(y > (3*image->sizeY - 15)){
                finY = 3*image->sizeY;
            }
            else{
                finY = y + 15;
            }
            if(x > (3*image->sizeX - 15)){
                finX = 3*image->sizeX;
            }
            else{
                finX = x + 15;
            }

            int x2, y2;
            //for each pixle with in radius 5
            //for loop is broken
            for(y2 = tempY; y2 < finY; y2 += 3){
                for(x2 = tempX; x2 < finX; x2 +=3){
                        int i = y2*image->sizeX + x2;
                        int curIntensity = (int)(((double)(image->data[i] + image->data[i + 1] + image->data[i + 2])/3)/255) * 20;
                        intensityCount[curIntensity]++;
                        averageR[curIntensity] += image->data[i];
                        averageG[curIntensity] += image->data[i+1];
                        averageB[curIntensity] += image->data[i+2];
                }
            }

            int k;
            int maxIndex, curMax;
            curMax = 0;
            for(k=0;k<20;++k){
                if(intensityCount[k] > curMax){
                    curMax = intensityCount[k];
                    maxIndex = k;
                }
            }

            int i = y*image->sizeX + x;
            image->data[i] = averageR[maxIndex]/curMax;
            image->data[i + 1] = averageB[maxIndex]/curMax;
            image->data[i + 2] = averageG[maxIndex]/curMax;
            free(intensityCount);
            free(averageB);
            free(averageG);
            free(averageR);
        }
    }


    
    if (ImageSave(outputFileName, image)==-1) return(-1);
    free(image);
    return 0;
}