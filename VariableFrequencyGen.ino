//BEGIN CONFIGURATION
#define FREQ_FLOOR_PIN A0
#define FREQ_CEILING_PIN A1
#define FREQ_A_PIN 6
#define FREQ_B_PIN 7
#define VOL_A_PIN 8
#define VOL_B_PIN 9
#define VOL_MIN 0.1
#define VOL_OUT_PIN 20
#define FREQ_OUT_PIN 21
#define CHANGE_DELAY 5

#define FREQ_COARSENESS 0.5 //How large changes to frequency are, on a scale of 0.0 to 1.0
#define GAIN_COARSENESS 1.0 //How large changes to gain are, on a scale of 0.0 to 1.0

//END CONFIGURATION

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Encoder.h>

AudioSynthWaveformSine waveform;
AudioOutputAnalog dac;
AudioConnection waveformToDac(waveform, dac);
Encoder volumeDial(VOL_A_PIN, VOL_B_PIN);
Encoder freqDial(FREQ_A_PIN, FREQ_B_PIN);
double minFreq = 5;
double maxFreq = 500;
double freq = maxFreq / 2;
int freqSetting = 512;
int gainSetting = 127;
float gain = 0.5;
void setup()
{
  AudioMemory(16);
  dac.analogReference(INTERNAL);
  pinMode(FREQ_FLOOR_PIN, INPUT);
  pinMode(FREQ_CEILING_PIN, INPUT);
  pinMode(FREQ_A_PIN, INPUT_PULLUP);
  pinMode(FREQ_B_PIN, INPUT_PULLUP);
  pinMode(VOL_A_PIN, INPUT_PULLUP);
  pinMode(VOL_B_PIN, INPUT_PULLUP);
  freqDial.write(freqSetting);
  volumeDial.write(gainSetting);
  waveform.frequency(freq);
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void loop()
{
  AudioNoInterrupts();
  minFreq = mapfloat(analogRead(FREQ_FLOOR_PIN), 0.0, 1023.0, 5.0, 249.0);
  maxFreq = mapfloat(analogRead(FREQ_CEILING_PIN), 0.0, 1023.0, 250.0, 500.0);

  int newFreqSetting, newGainSetting;
  newFreqSetting = freqDial.read();
  newGainSetting = volumeDial.read();
  if (newFreqSetting != freqSetting)
  {
    int delta = newFreqSetting - freqSetting;
    freqSetting += delta * FREQ_COARSENESS;
  }

  if (newGainSetting != gainSetting)
  {
    int delta = newGainSetting - gainSetting;
    gainSetting += delta * GAIN_COARSENESS;
  }

  if(freqSetting > 1023) { freqSetting = 1023; freqDial.write(freqSetting); }
  if(freqSetting < 0) { freqSetting = 0; freqDial.write(freqSetting); }
  if(gainSetting > 1023) { gainSetting = 1023; volumeDial.write(gainSetting); }
  if(gainSetting < 0) { gainSetting = 0; volumeDial.write(gainSetting); }
  freq = mapfloat(freqSetting, 0.0, 1023.0, minFreq, maxFreq);
  gain = mapfloat(gainSetting, 0.0, 1023.0, VOL_MIN, 1.0);
  waveform.frequency(freq);
  waveform.amplitude(gain);
  analogWrite(VOL_OUT_PIN, gainSetting / 4);
  analogWrite(FREQ_OUT_PIN, freqSetting / 4);
  AudioInterrupts();
  delay(CHANGE_DELAY);
}

