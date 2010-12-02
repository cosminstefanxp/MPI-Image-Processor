/* 
 * File:   img_process.c
 * Author: cosmin
 *
 * Created on November 26, 2010, 1:35 AM
 */
#include <errno.h>
#include <values.h>
#include "img_process.h"
#include "filters.h"

int WIDTH;
int HEIGHT;
int MAX_COLOR;

U8 *fullImage;
U8 **imageStrip;

int rank,numberProcesses;
int stripSize,stripStart,stripEnd;


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
    fullImage=(U8*)malloc(WIDTH*HEIGHT*sizeof(U8*));
    int i,j,temp;

    for(i=0;i<HEIGHT;i++)
        for(j=0;j<WIDTH;j++)
        {
            fscanf(fin,"%d",&temp);
            fullImage[i*WIDTH+j]=temp;
        }
    fclose(fin);
}

/* Scrie fisierul de iesire, folosind datele din fullImage */
void writeData(char* outputFile)
{
    FILE* fout;
    int i,j;

    printf("[Proces %d] Scriere fisier de iesire.\n",rank);

    if((fout=fopen(outputFile,"w"))==NULL)
    {
        perror("Eroare la deschiderea fisierului de iesire");
        exit(1);
    }

    //Scriere header
    fprintf(fout,"%s\n%d %d\n%d\n","P2",WIDTH,HEIGHT,MAX_COLOR);
    //Scriere date
    for(i=0;i<HEIGHT;i++,fprintf(fout,"\n"))
        for(j=0;j<WIDTH;j++)
            fprintf(fout,"%d ",fullImage[i*WIDTH+j]);
    fclose(fout);

    printf("[Proces %d]\tScriere fisier de iesire terminata.\n",rank);
}


//Functia ce realizeaza ajustarea contrastului imaginii:
//If = (b-a) * (Ii -min) / (max-min) + a
void contrast(int a,int b)
{
    int i,j;
    int min=MAX_COLOR+1;
    int max=0;
    int minAbsolut,maxAbsolut;

    printf("[Proces 0] Incepem ajustarea contrastului imaginii.\n");
    //Calculare min/max pe fasie
    for(i=1;i<=stripSize;i++)
        for(j=1;j<=WIDTH;j++)
        {
            if(imageStrip[i][j]>max)
                max=imageStrip[i][j];
            if(imageStrip[i][j]<min)
                min=imageStrip[i][j];
        }
    printf("[Proces %d]\tMin fasie: %d, Max fasie: %d\n",rank,min,max);
    MPI_Allreduce(&min,&minAbsolut,1,MPI_INT,MPI_MIN,MPI_COMM_WORLD);
    MPI_Allreduce(&max,&maxAbsolut,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);

    if(rank==0)
         printf("[Proces %d]\tMin absolut: %d, Max absolut: %d\n",rank,minAbsolut,maxAbsolut);

    //Ajustarea contrastului
    for(i=1;i<=stripSize;i++)
        for(j=1;j<=WIDTH;j++)
            imageStrip[i][j]= (b-a) * (imageStrip[i][j] -minAbsolut) / (maxAbsolut-minAbsolut) + a;

    printf("[Proces %d]\tAjustarea contrastului terminata.\n",rank);
}

/*
 * 
 */
int main(int argc, char** argv)
{
    char* outputFileName;
    //Initial parameters check
    if(argc<3)
    {
        fprintf(stderr,"\nNumar incorect de parametrii. Utilizare:\n%s contrast/filter/entropy input_file ...\n",argv[0]);
        return EXIT_FAILURE;
    }

    /*****************VERIFICARE PARAMETRII*****************************/
    if(strcasecmp(argv[1],"contrast")==0)
    {
        if(argc!=6)
        {
            fprintf (stderr,"\nNumar incorect de parametrii. Utilizare:\n%s contrast input_file a b output_file\n",argv[0]);
            return EXIT_FAILURE;
        }
        outputFileName=argv[5];
    }

    if(strcasecmp(argv[1],"filter")==0)
    {
        if(argc!=5)
        {
            fprintf (stderr,"\nNumar incorect de parametrii. Utilizare:\n%s filter input_file smooth/blur/sharpen/mean_removal/emboss output_file\n",argv[0]);
            return EXIT_FAILURE;
        }
        outputFileName=argv[4];
    }

    if(strcasecmp(argv[1],"entropy")==0)
    {
        if(argc!=7)
        {
            fprintf (stderr,"\nNumar incorect de parametrii. Utilizare:\n%s entropy input_file a b c output_file\n",argv[0]);
            return EXIT_FAILURE;
        }
        outputFileName=argv[6];
    }


    /*********************** MPI STARTING FROM HERE ************************/

    MPI_Status status;
    MPI_Request *requests;
    MPI_Status *statuses;
    int rc;
    int i,j;
    int bufferInt[20];

    
    // initializare MPI
    rc = MPI_Init(&argc,&argv);
    if (rc != MPI_SUCCESS)
    {
        fprintf (stderr,"Error starting MPI program. Terminating...\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numberProcesses);

    //Citire imagine si initializari
    if(rank==MASTER)
    {
        readData(argv[2]);
        requests=(MPI_Request*)malloc(sizeof(MPI_Request)*numberProcesses);
        statuses=(MPI_Status*)malloc(sizeof(MPI_Status)*numberProcesses);
    }

/*
    if(rank==MASTER)
    {
    printf("[Proces %d] S-au citit datele:\n",rank);
    for(i=0;i<HEIGHT;i++,printf("\n"))
        for(j=0;j<WIDTH;j++)
            printf("%3d ",fullImage[i*WIDTH+j]);
    }
*/

    /************************ COMUNICATIE INITIALA ****************************/
    //transmitere dimensiuni imagine la procese
    bufferInt[0]=WIDTH;
    bufferInt[1]=HEIGHT;
    bufferInt[2]=MAX_COLOR;
    MPI_Bcast (&bufferInt,3,MPI_INT,MASTER,MPI_COMM_WORLD);

    if(rank!=MASTER)
    {
        WIDTH=bufferInt[0];
        HEIGHT=bufferInt[1];
        MAX_COLOR=bufferInt[2];        
    }

    //Calculare dimensiune fasii
    stripSize =  HEIGHT/ numberProcesses;
    stripStart = rank*stripSize;
    stripEnd = rank*stripSize+stripSize-1;
    if (rank == numberProcesses-1 && HEIGHT % numberProcesses != 0)
    {
	stripSize = HEIGHT - stripSize * (numberProcesses-1);
        stripEnd=HEIGHT-1;
    }

    printf("\n[Proces %d] Dimensiuni imagine: %d X %d => fasie de dimensiune: %d (%d..%d)\n",rank,WIDTH,HEIGHT,stripSize,stripStart,stripEnd);


    //Alocare spatiu cu verificare incadrare in memorie
    /* Se va folosi matricea imageStrip, in care randul 0 reprezinta randul din imagine anterior acestei fasii,
     * iar linia stripSize+1 va reprezenta randul din imagine de dupa aceasta fasie. Aceste randuri vor fi folosite
     * in taskurile 2 si 3. De asemenea, randurile au dimensiune WIDTH+2, datele fiind intre indicii 1 si WIDTH,
     * pentru a usura procedura in cadrul taskului 2 (marginile au valoarea 0).*/
    imageStrip=(U8**)malloc((stripSize+2)*sizeof(U8*));
    if (imageStrip == NULL) {
        fprintf(stderr,"[ERROR] Process %d could not allocate any more memory.\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    for(i=0;i<=stripSize+1;i++)
    {
        imageStrip[i]=(U8*)calloc((WIDTH + 2),sizeof(U8));
        if (imageStrip[i] == NULL) {
            fprintf(stderr,"[ERROR] Process %d could not allocate any more memory for strip row %d.\n", rank,i);
            MPI_Abort(MPI_COMM_WORLD, 2);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    
    //transmiterea datelor. Procesul MASTER trimite informatiile. Se trimit toate liniile din matrice, liniarizat
    int pozCerere=0;
    if(rank==MASTER)
    {
        int sendStripeSize=HEIGHT/ numberProcesses;
        int initialStripSize=sendStripeSize;
        for(i=0;i<numberProcesses;i++)
            if(i!=MASTER)
            {
                if(i==numberProcesses-1)
                    sendStripeSize = HEIGHT - sendStripeSize * (numberProcesses-1);
                printf("[MASTER] Trimitem fasia de dimensiune %d, incepand la pozitia %d, la procesul %d.\n",sendStripeSize, i*initialStripSize*WIDTH,i);
                MPI_Isend (fullImage + i*initialStripSize*WIDTH,sendStripeSize*WIDTH,MPI_UNSIGNED_CHAR,i,i,MPI_COMM_WORLD,&requests[pozCerere++]);
                //MPI_Send (fullImage + i*initialStripSize*WIDTH,sendStripeSize*WIDTH,MPI_UNSIGNED_CHAR,i,i,MPI_COMM_WORLD);
            }
        //Punem si datele procesului MASTER in imageStrip
        U8* bufferChar=fullImage+MASTER*initialStripSize*WIDTH;
        for(i=0;i<stripSize;i++)
            for(j=0;j<WIDTH;j++)
            {
                //printf("MASTER: Punem in %d,%d ce e la %d: %d\n",i+1,j+1,i*WIDTH+j,bufferChar[i*WIDTH+j]);
                imageStrip[i+1][j+1]=bufferChar[i*WIDTH+j];
            }

        printf("[MASTER] Asteptam transmiterea.\n");
        MPI_Waitall(numberProcesses-1,requests,statuses);
    }
    else
    {
        //Se primesc liniile fasiei liniarizat
        U8* bufferChar=(U8*)malloc(sizeof(U8)*WIDTH*stripSize);
        printf("[Proces %d] Incepem primirea datelor\n",rank);
        MPI_Recv(bufferChar,stripSize*WIDTH,MPI_UNSIGNED_CHAR,MASTER,rank,MPI_COMM_WORLD,&status);
        printf("[Proces %d] \tDate primite, incepem deliniarizarea.\n",rank);

        //Se deliniarizeaza datele. Se exclud ultima si prima linie/coloana, care raman 0
        for(i=0;i<stripSize;i++)
            for(j=0;j<WIDTH;j++)
            {
                //printf("[Proces %d] Primit (%d,%d): %d\n",rank,(i+1),j+1,bufferChar[i*WIDTH+j]);
                imageStrip[i+1][j+1]=bufferChar[i*WIDTH+j];
            }
    }

/*
    printf("[Proces %d] \tS-a primit fasia:\n",rank);
    for(i=1;i<=stripSize;i++,printf("\n"))
        for(j=1;j<=WIDTH;j++)
            printf("%3d ",imageStrip[i][j]);
*/

    /********************* END COMUNICATIE INITIALA ***************************/
   
    /**************************** PROCESARE ***********************************/

    //Ajustare contrast
    if(strcasecmp(argv[1],"contrast")==0)
    {
        int a,b;
        sscanf(argv[3],"%d",&a);
        sscanf(argv[4],"%d",&b);
        contrast(a,b);
    }


    /************************** END PROCESARE *********************************/

    //Colectare informatii
    if(rank != MASTER )
    {
        printf("[Proces %d] Procesare completa. Trimitem fasia spre procesul master.\n",rank);
        //Copiem datele intr-un buffer, pentru a fi transmise simultan
        U8* buffer=(U8*) malloc (stripSize*WIDTH*sizeof(U8));
        for(i=1;i<=stripSize;i++)
            for(j=1;j<=WIDTH;j++)
                buffer[(i-1)*WIDTH + (j-1)]=imageStrip[i][j];
        //Trimitem datele
        MPI_Send(buffer,stripSize*WIDTH,MPI_UNSIGNED_CHAR,MASTER,rank,MPI_COMM_WORLD);
        //eliberam memoria
        free(buffer);
    }
    //MASTER PROCESS
    else
    {
        printf("[MASTER] Procesare completa. Primim fasiile de la restul proceselor.\n");
        int sendStripeSize=HEIGHT/ numberProcesses;
        int initialStripSize=sendStripeSize;
        pozCerere=0;
        //Copiem datele de la fiecare proces in locatia corespunzatoare din fullImage
        for(i=0;i<numberProcesses;i++)
            if(i!=MASTER)
            {
                printf("[MASTER] \tPrimim datele de la procesul %d.\n",i);
                if(i==numberProcesses-1)
                    sendStripeSize = HEIGHT - sendStripeSize * (numberProcesses-1);
                MPI_Irecv (fullImage + i*initialStripSize*WIDTH,sendStripeSize*WIDTH,MPI_UNSIGNED_CHAR,i,i,MPI_COMM_WORLD,&requests[pozCerere++]);                
            }
        //Punem si datele procesului MASTER in imageStrip
        for(i=1;i<=stripSize;i++)
            for(j=1;j<=WIDTH;j++)
            {
                fullImage[rank*WIDTH + (i-1)*WIDTH + (j-1)]=imageStrip[i][j];
            }

        printf("[MASTER]\tAsteptam primirea datelor.\n");
        MPI_Waitall(numberProcesses-1,requests,statuses);
    }

    MPI_Finalize();

    //Scrierea finala a imaginii
    if(rank==MASTER)
        writeData(outputFileName);



    return (EXIT_SUCCESS);
}

