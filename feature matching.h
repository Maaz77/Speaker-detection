//
//  feature matching.h
//  portaudio
//
//  Created by MacBook Pro on 7/1/17.
//  Copyright © 2017 MacBook Pro. All rights reserved.
//

#ifndef feature_matching_h
#define feature_matching_h

#include <stdio.h>

float bayesian_classifier(int frame1,int frame2, int coeff,float source[frame1][coeff],float sample [coeff][frame2]);

#endif /* feature_matching_h */
