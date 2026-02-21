#include "audio.hpp"
#include "utils.hpp"
#include "time.hpp"

void play_sound(uint32_t nFrequence) {
 	uint32_t Div;
 	uint8_t tmp;
 
 	Div = 1193180 / nFrequence;
 	outb(0x43, 0xb6);
 	outb(0x42, (uint8_t) (Div) );
 	outb(0x42, (uint8_t) (Div >> 8));
 
 	tmp = inb(0x61);
  	if (tmp != (tmp | 3)) {
 		outb(0x61, tmp | 3);
    }
}
 
void stop_sound() {     
 	outb(0x61, inb(0x61) & 0xFC);
}

void play_note(uint32_t freq, uint32_t duration) {
    play_sound(freq);
    sleep(duration);
    stop_sound();
}

#define NOTE_E5  659
#define NOTE_D5  587
#define NOTE_FS4 370
#define NOTE_GS4 415
#define NOTE_CS5 554
#define NOTE_B4  493
#define NOTE_D4  293
#define NOTE_E4  329
#define NOTE_A4  440
#define NOTE_CS4 277

void play_music() {
    play_note(NOTE_E5, 100);
    play_note(NOTE_D5, 100);
    play_note(NOTE_FS4, 250);
    play_note(NOTE_GS4, 250);
    
    play_note(NOTE_CS5, 100);
    play_note(NOTE_B4, 100);
    play_note(NOTE_D4, 250);
    play_note(NOTE_E4, 250);
    
    play_note(NOTE_B4, 100);
    play_note(NOTE_A4, 100);
    play_note(NOTE_CS4, 250);
    play_note(NOTE_E4, 250);
    
    play_note(NOTE_A4, 550); 
}