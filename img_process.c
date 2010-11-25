/* 
 * File:   img_process.c
 * Author: cosmin
 *
 * Created on November 26, 2010, 1:35 AM
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * 
 */
int main(int argc, char** argv)
{
    if(argc<3)
    {
        fprintf(stderr,"Numar incorect de parametrii. Utilizare:\n%s contrast/filter/entropy input_file ...\n",argv[0]);
    }

    return (EXIT_SUCCESS);
}

