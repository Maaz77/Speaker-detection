#include <stdio.h>
#include <stdlib.h>
#include "portaudio.h"
#include "math.h"

#include "mfcc.h"
#include "kiss_fft.h"
#include "feature matching.h"
#include "recordaudio.h"


#define M_2PI 6.28318530718

typedef float SAMPLE;


/////////////////////////////////////////////////////////////////////////
void pre_emph(float arr[],float arr1[],int len){
    int i;
    arr1[0]=arr[0];
    for(i=1;i<len;i++){
        arr1[i]=arr[i]- 0.95*arr[i-1];
    }
}
//////////////////////////////////



void hamming(int leng,float window[]){
    int i;
    float arg;
    arg = M_2PI / (leng - 1);
    for (i = 0; i < leng; i++){
        *window++ = 0.54 - 0.46 * cos(i * arg);
    }

}





int main(void){
    
    
    int len_Sample;
    unsigned int coeff;
    double mfcc_result;
    int nSamples = 512;
    int overlap = 200;
    int start = 0;
    float window[512];
    float signals[36000];
    float signal[36000];
    
   
    
    int numSamples = 64000;
    
    float minenergy = 0;
    
    SAMPLE * thesilence = record_voice(0);
   
    for (int i = 0 ; i < numSamples - 1 ; i = i +2 ) minenergy+= thesilence[i]*thesilence[i];
    minenergy/= numSamples/2;
    printf("silence avg energy  = %f ", minenergy);

    SAMPLE * thevoice = record_voice(1) ;
    
    int k;
    int s=0;
    for(k=0;k<numSamples-1;++k){
        if (thevoice[k]==thevoice[k+1]){
            continue;
        }
        
        signals[s]=thevoice[k];
        s++;
    }
    
    
    

    
    int i=0;
    len_Sample=s;
    k=0;
    int j=0;
    int nFrames = (int)( ceil( (float)len_Sample / (nSamples - overlap) ) );
    float segment[nFrames][nSamples];
    double pwr_fft[nSamples];
    float mfcc_coeff[nFrames][13];
    float file_MFCC_coeff[200][13];
    
    
    pre_emph(signals,signal,len_Sample);
    
    ////framing
    int padding = ( (nSamples - overlap) * nFrames ) - len_Sample;
    for( i = len_Sample ; i < padding + len_Sample ; i++ )signal[i] = 0;
    padding += len_Sample;
    for( i = 0 ; i < nFrames ; i++){
        for( j = 0 ; j < nSamples ; j++ ){
            segment[i][j] = signal[start + j];
            start = (nSamples - overlap) * i;
        }
    }
    
    float * frameenergy = (float *) malloc(nFrames * sizeof(float));
    int voiceframesnum = 0 ;

    for (int i =0 ; i < nFrames; i++){
      
         float energy_signal = 0;
        
        for ( int j = 0 ; j < nSamples ; j++){
             energy_signal+= segment[i][j] * segment[i][j];
        }
        energy_signal /= nSamples;
        if (energy_signal < minenergy) frameenergy[i] = 0;
        else {
            frameenergy[i] = 1;
            voiceframesnum++;
        }
    }
    
    printf("number of frames: %d\nnumber of voice frames : %d\n",nFrames, voiceframesnum );
    
    hamming(512,window);
    
    for (j=0;j<nFrames;j++){
        for(i=0;i<512;i++){
            segment[j][i]=segment[j][i]*window[i];
        }
    }
    
    
//    for (i=0;i<nFrames;i++){
//        fftr1(segment[i], 512,1);
//    }
    
 
    kiss_fft_cpx  cx_in[nSamples],  cx_out[nSamples];
    kiss_fft_cfg cfg = kiss_fft_alloc( nSamples ,0 ,0,0 );
    
    float  dft_real_in [nSamples];
    float dft_img_in [nSamples];
    float dft_real_out [nSamples];
    float dft_img_out [nSamples];
    
    int l = 0;
    for(i=0;i<nFrames;i++) {
        
        if(frameenergy[i] == 1) {
              printf("processing Frame %d...\n",i);
            
            
            for(j=0;j<nSamples;j++) {
                cx_in[j].r = segment[i][j];
                cx_in[j].i =0;
                
                dft_real_in[j] = segment[i][j];
                dft_img_in[j] = 0;
            }
            
            kiss_fft( cfg , cx_in , cx_out );
            //fft_double(nSamples,0,dft_real_in,dft_img_in,dft_real_out,dft_img_out);
            for(int w = 0 ; w < nSamples ; w ++) {
                pwr_fft[w] = cx_out[w].r;
                //printf("imag : %f\n",cx_out[w].i);
               
            }
            //printf("\n\n dft_img_out \n\n");
            //printvector(20,dft_img_out);
            //printvector(20,pwr_fft);
            for(coeff = 0; coeff < 9; coeff++) {
                mfcc_result = GetCoefficient(pwr_fft, 16000, 48, nSamples, coeff);
                mfcc_coeff[l][coeff]=mfcc_result;
            }
            //printvector(13,mfcc_coeff[l]);
            
            l++;
        }
    }
    
    
    free(cfg);
    
    printf("a) save this Voice in file. \n");
    
    printf("b) recognize this voice(There should be a voice which already has been saved in 'sample.txt', otherwise first save this voice.) \n");

    int menue ;
    
    menue = getchar();


    if (menue == 'a') {

        FILE *f;
        f= fopen("/Users/maaz/Desktop/sample.txt","w");
        for (int i = 0; i < l;i++) for (int j = 0 ; j < 9 ; j ++) fprintf(f, "%f\n ", mfcc_coeff[i][j]);
        fclose(f);

    }
    
    
    else{
        
        FILE *f1;
        f1= fopen("/Users/maaz/Desktop/sample.txt", "r");
        i=0;
        while(fscanf(f1,"%f",&file_MFCC_coeff[i/9][i%9]) == 1 ) i++;
        
      
        float similarity = bayesian_classifier (i/9, voiceframesnum, 9 , file_MFCC_coeff, mfcc_coeff);
        
        
        printf("\nOutput of bayesian classifier : %f\n", similarity);
        if (similarity > 0.1) printf("\nmatched\n");
        else printf ("\nnot matched\n");
        
        fclose(f1);
    }
        
    
}

