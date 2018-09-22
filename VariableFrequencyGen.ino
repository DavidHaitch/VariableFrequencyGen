//BEGIN CONFIGURATION
#define FREQ_A_PIN 6
#define FREQ_B_PIN 7
#define VOL_A_PIN 8
#define VOL_B_PIN 9
#define VOL_MIN 0.1
#define VOL_OUT_PIN 20
#define FREQ_OUT_PIN 21
#define CHANGE_DELAY 10
#define FREQ_MIN 0.1
#define FREQ_MAX 500.0

#define FREQ_INCREMENT 0.05 //How large changes to frequency are, in Hz
#define GAIN_INCREMENT 0.0001 //How large changes to gain are, on a scale of 0.0 to 1.0

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
int freqSetting = 0;
int gainSetting = 0;
float gain = 0.5;
void setup()
{
  AudioMemory(16);
  dac.analogReference(INTERNAL);
  freqDial.write(freqSetting);
  volumeDial.write(gainSetting);
  waveform.frequency(freq);
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int lastFreqSetting, lastGainSetting;
void loop()
{
  AudioNoInterrupts();
  
  freqSetting = freqDial.read();
  gainSetting = volumeDial.read();
  if (lastFreqSetting != freqSetting)
  {
    int delta = freqSetting - lastFreqSetting;
    freq += delta * FREQ_INCREMENT;
  }

  lastFreqSetting = freqSetting;
  
  if (lastGainSetting != gainSetting)
  {
    int delta = gainSetting - lastGainSetting;
    gain += delta * GAIN_INCREMENT;
  }

  lastGainSetting = gainSetting;
  
  if(freq > FREQ_MAX) freq = FREQ_MAX;
  if(freq < FREQ_MIN) freq = FREQ_MIN;
  if(gain > 1.0) gain = 1.0;
  if(gain < 0.0) gain = 0.0;
  Serial.print(freq);
  Serial.print("\t");
  Serial.println(gain);
  waveform.frequency(freq);
  waveform.amplitude(gain);
  analogWrite(VOL_OUT_PIN, mapfloat(gain, 0.0, 1.0, 0.0, 255.0));
  analogWrite(FREQ_OUT_PIN, mapfloat(freq, FREQ_MIN, FREQ_MAX, 0.0, 255.0));
  AudioInterrupts();
  delay(CHANGE_DELAY);
}

