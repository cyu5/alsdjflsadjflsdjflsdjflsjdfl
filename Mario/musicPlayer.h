/* helpful links for basics of music sheet
 * http://www.simplifyingtheory.com/sheet-music-guide/
 * https://github.com/nseidle/AxelF_DoorBell/wiki/How-to-convert-sheet-music-into-an-Arduino-Sketch
 * https://www.princetronics.com/supermariothemesong/
 * http://www.mariopiano.com/mario-sheet-music-overworld-main-theme.html
 */

#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include "pitch.h"


class music_player {

    public:
        //constructor, initialize beatsPerMinute, beatsPerMeasure, and beatsPerNote  
        music_player(byte *speakerPins, byte numSpeakers, unsigned int bpm, byte bpi, byte bpn);
        unsigned int beatsPerMinute;
        byte beatsPerMeasure;
        byte beatsPerNote;
        unsigned long millisPerNote;
        void setBarLine (unsigned int bpm, byte bpi, byte bpn);
        void loadSong (byte speaker, unsigned int *melody, unsigned int *tempo);

    private:
        


    
};







#endif
