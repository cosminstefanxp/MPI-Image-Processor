/* 
 * File:   img_process.c
 * Author: cosmin
 *
 * Created on November 26, 2010, 1:35 AM
 */
#include <errno.h>
#include <values.h>
#include "img_process.h"

int WIDTH;
int HEIGHT;
int MAX_COLOR;

U8 **image;


/* Citeste datele de intrare.*/
void readData(char* inputFile)
{
    FILE* fin;
    char data[70];

    if((fin=fopen(inputFile,"r"))==NULL)
    {
        perror("Eroare la deschiderea fisierului de intrare");
        exit(1);
    }

    printf("Reading input file HEADER:\n");

    //Reading format
    fscanf(fin,"%s",data);
    while(data[0]=='#') //ignoring comments
    {
        printf("\tFound comment: %s",data);
        fgets(data,70,fin);
        printf("%s",data);
        fscanf(fin,"%s",data);
    }
    if(strcasecmp(data,"P2")!=0)
    {
        printf("Illegal format type of input file. Expected encoding: \"P2\". Found encoding \"%s\"\n",data);
        exit(1);
    }
    else
        printf("\tFile format found for %s: %s\n",inputFile,data);

    //Reading Width
    fscanf(fin,"%s",data);
    while(data[0]=='#') //ignoring comments
    {
        printf("\tFound comment: %s",data);
        fgets(data,70,fin);
        printf("%s",data);
        fscanf(fin,"%s",data);
    }
    sscanf(data,"%d",&WIDTH);
    printf("\tImage width: %d\n",WIDTH);

    //Reading Height
    fscanf(fin,"%s",data);
    while(data[0]=='#') //ignoring comments
    {
        printf("\tFound comment: %s",data);
        fgets(data,70,fin);
        printf("%s",data);
        fscanf(fin,"%s",data);
    }
    sscanf(data,"%d",&HEIGHT);
    printf("\tImage height: %d\n",HEIGHT);


    //Reading MAX_COLOR
    fscanf(fin,"%s",data);
    while(data[0]=='#') //ignoring comments
    {
        printf("\tFound comment: %s",data);
        fgets(data,70,fin);
        printf("%s",data);
        fscanf(fin,"%s",data);
    }
    sscanf(data,"%d",&MAX_COLOR);
    printf("\tImage number of colors: %d\n",MAX_COLOR);

    //Reading data

    printf("Reading input file DATA.\n");
    image=(U8**)malloc(HEIGHT*sizeof(U8*));
    int i,j,temp;
    for(i=0;i<HEIGHT;i++)
        image[i]=(U8*)malloc(WIDTH*sizeof(U8));

    for(i=0;i<HEIGHT;i++)
        for(j=0;j<WIDTH;j++)
        {
            fscanf(fin,"%d",&temp);
            image[i][j]=temp;
        }
}

/*
 * 
 */
int main(int argc, char** argv)
{
    //Initial parameters check
    if(argc<3)
    {
        fprintf(stderr,"\nNumar incorect de parametrii. Utilizare:\n%s contrast/filter/entropy input_file ...\n",argv[0]);
        return EXIT_FAILURE;
    }

    readData(argv[2]);



    return (EXIT_SUCCESS);
}

