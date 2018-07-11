//BEGIN CONFIGURATION
#define FREQ_FLOOR_PIN A0
#define FREQ_CEILING_PIN A1
#define FREQ_DOWN_PIN 6
#define FREQ_UP_PIN 7
#define VOL_DOWN_PIN 8
#define VOL_UP_PIN 9
#define VOL_MIN 100 // on a scale from 0 to 1,000. The default value of 100 means that the volume can never go below 10%.
#define VOL_OUT_PIN 20
#define FREQ_OUT_PIN 21
#define CHANGE_DELAY 50

#define VOL_STEPS 10

//END CONFIGURATION

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

AudioSynthWaveformSine waveform;
AudioOutputAnalog dac;
AudioConnection waveformToDac(waveform, dac);
int minFreq = 5;
int maxFreq = 500;
double freq = maxFreq / 2;
int freqSetting = 512;
int gainSetting = 127;
int gainStep = 1023 / VOL_STEPS;
float gain = 0.5;
void setup()
{
  AudioMemory(16);
  dac.analogReference(INTERNAL);
  pinMode(FREQ_FLOOR_PIN, INPUT);
  pinMode(FREQ_CEILING_PIN, INPUT);
  pinMode(FREQ_DOWN_PIN, INPUT_PULLUP);
  pinMode(FREQ_UP_PIN, INPUT_PULLUP);
  pinMode(VOL_DOWN_PIN, INPUT_PULLUP);
  pinMode(VOL_UP_PIN, INPUT_PULLUP);
  waveform.frequency(freq);
}

void loop()
{
  AudioNoInterrupts();
  minFreq = map(analogRead(FREQ_FLOOR_PIN), 0, 1023, 5, 249);
  maxFreq = map(analogRead(FREQ_CEILING_PIN), 0, 1023, 250, 500);
  if(!digitalRead(FREQ_DOWN_PIN))
  {
    freqSetting--;  
  }
  
  if(!digitalRead(FREQ_UP_PIN))
  {
    freqSetting++;  
  }
  
  if(!digitalRead(VOL_DOWN_PIN))
  {
    gainSetting -= gainStep;  
  }
  
  if(!digitalRead(VOL_UP_PIN))
  {
    gainSetting += gainStep;  
  }

  if(freqSetting > 1023) freqSetting = 1023;
  if(freqSetting < 0) freqSetting = 0;
  if(gainSetting > 1023) gainSetting = 1023;
  if(gainSetting < 0) gainSetting = 0;
  freq = map(freqSetting, 0, 1023, minFreq, maxFreq);
  gain = map(gainSetting, 0,1023, VOL_MIN, 1000) / 1000.0;
  waveform.frequency(freq);
  waveform.amplitude(gain);
  analogWrite(VOL_OUT_PIN, gainSetting / 4);
  analogWrite(FREQ_OUT_PIN, freqSetting / 4);
  AudioInterrupts();
  delay(CHANGE_DELAY);
}

