# Speaker-detection
## Dependancy : portaudio 
## Note: Link portaudio binary while compiling.
## First a speaker pronounce a specific word and save it in the file(sample.txt), thereafter run the program once again and say the word again. 
The program detects whether this speaker is the previous one or not.


Challenges : 
1. Feature extraction: MFCC features are extracted after some preprocessing. 
2. classification: I used the bayesian classifier for detecting the speaker according to MFCC features.
