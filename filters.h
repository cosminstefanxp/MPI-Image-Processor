/* 
 * File:   filters.h
 * Author: cosmin
 *
 * Created on December 2, 2010, 1:09 PM
 */

#ifndef FILTERS_H
#define	FILTERS_H

#ifdef	__cplusplus
extern "C" {
#endif

    //the constants that define the "names" of the filters"
    #define F_IDENTITY_C 0
    #define F_SMOOTH_C 1
    #define F_BLUR_C 2
    #define F_SHARPEN_C 3
    #define F_MEAN_REMOVE_C 4
    #define F_EMBOSS_C 5

    //The factors and values (in close relation with the constants above).
    int F_FACTORS[]={ 1, 9, 16, 3, 1, 1};
    int F_OFFSETS[]={ 0, 0,  0, 0, 0, 127};

    //The convolution matrices
    int F_IDENTITY[]= { 0, 0, 0,\
                        0, 1, 0,\
                        0, 0, 0 };
    int F_SMOOTH[]=   { 1, 1, 1,\
                        1, 1, 1,\
                        1, 1, 1 };
    int F_BLUR[]=     { 1, 2, 1,\
                        2, 4, 2,\
                        1, 2, 1 };
    int F_SHARPEN[]=  { 0,-2, 0,\
                       -2,11,-2,\
                        0,-2, 0 };
    int F_MEAN_REMOVE[]= {-1,-1,-1,\
			  -1, 9,-1,\
			  -1,-1,-1 };
    int F_EMBOSS[]=   {-1, 0,-1,\
                        0, 4, 0,\
                       -1, 0,-1 };



#ifdef	__cplusplus
}
#endif

#endif	/* FILTERS_H */

