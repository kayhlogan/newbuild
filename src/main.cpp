#include <linux/soundcard.h>

#include <iostream> 
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "PdBase.hpp"
#include "RtAudio.h"
#include "receiver.h"

using namespace std;
using namespace pd;

RtAudio audio;
pd::PdBase lpd;
receiver rec;
float YEEHAW;
unsigned char MS20_MIDI = YEEHAW;

//MIDI VALUES
char* device = "/dev/midi";

int audioCallback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData){

   // Pd magic
   int ticks = nBufferFrames / 64;
   lpd.processFloat(ticks, (float *)inputBuffer, (float*)outputBuffer);

   return 0;
}

void initAudio(){
   unsigned int sampleRate = 44100;
   unsigned int bufferFrames = 128;

   // Init pd
   if(!lpd.init(0, 2, sampleRate)) {
      std::cerr << "Could not init pd" << std::endl;
      exit(1);
   }

   // Receive messages from pd
   lpd.setReceiver(&rec);
   lpd.subscribe("MIDI");
   lpd.subscribe("VELOCITY");

   // send DSP 1 message to pd
   lpd.computeAudio(true);

   // load search path
   lpd.addToSearchPath("~/pd-externals");

   // load the patch
   pd::Patch patch = lpd.openPatch("test.pd", "./pd");
   std::cout << patch << std::endl;

   // Use the RtAudio API to connect to the default audio device.
   if(audio.getDeviceCount()==0){
      std::cout << "There are no available sound devices." << std::endl;
      exit(1);
   }

   RtAudio::StreamParameters parameters;
   parameters.deviceId = audio.getDefaultOutputDevice();
   parameters.nChannels = 2;

   RtAudio::StreamOptions options;
   options.streamName = "LibPD Test";
   options.flags = RTAUDIO_SCHEDULE_REALTIME;
   if ( audio.getCurrentApi() != RtAudio::MACOSX_CORE ) {
      options.flags |= RTAUDIO_MINIMIZE_LATENCY; RTAUDIO_ALSA_USE_DEFAULT; // CoreAudio doesn't seem to like this
   }
   try {
      audio.openStream( &parameters, NULL, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &audioCallback, NULL, &options );
      audio.startStream();
   }
   catch ( RtAudioError& e ) {
      std::cerr << e.getMessage() << std::endl;
      exit(1);
   }
}

int main (int argc, char *argv[]) {
   //Setup audio
   initAudio();

   //OPEN OSS
   int fd = open(device, O_WRONLY, 0);
   if (fd < 0){
      printf("Error: cannot open %s\n", device);
   }
   else if (fd > 0){
      printf("YASS");
   }
   
   	//while(1){
    //  lpd.receiveMessages();
    //YEEHAW = rec.currentPitch();
	//  cout << "MS20: " << YEEHAW;
   	//}
   while(1){
   	unsigned char note_on[3] = {0x90, rec.currentPitch(), rec.currentVelocity()};

	write (fd, note_on, sizeof(note_on));
	usleep(5000);
   }
    
   return 0;
}