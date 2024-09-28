int spectr[FFT_SIZE];
float averK = 0.006;
int lowPassSpectr = 40;
int lowPassVolume = 100;

bool colorMusicFlash[3];
bool running_flag[3];
byte thisBright[3];
int freq_min[32];
float freq_max_f;
float colorMusic_f[3], colorMusic_aver[3];

void computeSound() {
  if (mode == 0) 
    filterVolume();

  else {
    ADC_FFT();
    filterFFT();
  }
}

void ADC_FFT() {
  int raw[FFT_SIZE];

  for (int i = 0; i < FFT_SIZE; i++) 
    raw[i] = analogRead(soundRList[portAux]);

  FFT(raw, spectr);
} 

void filterFFT() {
  int spectrF[FFT_SIZE];
  int colorMusic[3] = {0,0,0};

  for (int i = 0; i < FFT_SIZE-3; i++) //Нижний порог звука (подавление шума)
    if (spectr[i+2] < lowPassSpectr) 
      spectrF[i] = 0;

  for (byte i = 1; i < 6; i++)  //выборка нижних частот
    if (spectr[i] > colorMusic[0]) 
      colorMusic[0] = spectr[i];
  
  for (byte i = 6; i < 11; i++) //выборка средних частот
    if (spectr[i] > colorMusic[1]) 
      colorMusic[1] = spectr[i];
  
  for (byte i = 11; i < 32; i++) //выборка высоких частот
    if (spectr[i] > colorMusic[2]) 
      colorMusic[2] = spectr[i];

  int freq_max = 0;
  
  for (byte i = 0; i < 31; i++) { 
    if (spectr[i + 1] > freq_max) freq_max = spectr[i + 1]; //максимальное значение среди всех частот
    if (freq_max < 5) freq_max = 5;
    
    if (freq_min[i] < spectr[i + 1]) freq_min[i] = spectr[i + 1]; //минимальное значение среди всех частот
    if (freq_min[i] > 0) freq_min[i] -= LIGHT_SMOOTH; //плавность вспышки
    else freq_min[i] = 0;
  }

  freq_max_f = freq_max * averK + freq_max_f * (1 - averK); 

  for (byte i = 0; i < 3; i++) { //цикл на фильтрацию и проверку 3-х частот
    colorMusic_aver[i] = colorMusic[i] * averK + colorMusic_aver[i] * (1 - averK);
    colorMusic_f[i] = colorMusic[i] * SmoothFreq + colorMusic_f[i] * (1 - SmoothFreq);
    
    if (colorMusic_f[i] > ((float)colorMusic_aver[i] * MaxCoefFreq)) {
      thisBright[i] = 255; //яркость вспышки
      colorMusicFlash[i] = true; //разрешение на вспышку 1
      running_flag[i] = true; //разрешение на вспышку частоты 2 
    } 
    else colorMusicFlash[i] = false; //запрет на вспышку 1

    if (thisBright[i] >= 0) thisBright[i] -= SMOOTH_STEP; //если яркость вспышки больше 0 то применяется плавность вспышки

    if (thisBright[i] < emptyBright) { //emptyBright - яркость "подложки" (когда нет звука)
      thisBright[i] = emptyBright; //яркость вспышки не может быть меньше чем emptyBright
      running_flag[i] = false; //запрет на вспышку частоты 2
    }
  }
}

float averageLevel = 50;
int maxLevel = 100;
int Rlenght, Llenght;
float RsoundLevel_f, LsoundLevel_f;
int RcurrentLevel, LcurrentLevel;

void filterVolume() {
  #define MAX_COEF 1.8
  int HALF_LED = NUM_LEDS / 2;
  float RsoundLevel = 0, LsoundLevel = 0;

  for (byte i = 0; i < 100; i ++) { //100 измерений для большей точности                     
    RcurrentLevel = analogRead(soundRList[portAux]);
    if (RsoundLevel < RcurrentLevel) //ищем максимум
      RsoundLevel = RcurrentLevel;
  }

  RsoundLevel = map(RsoundLevel, lowPassVolume, 1023, 0, 500); //ограничение по диапазону 0 - 500
  RsoundLevel = constrain(RsoundLevel, 0, 500); //предотвращение выхода за диапазон
  RsoundLevel = pow(RsoundLevel, EXP); //возведение в степень 
  RsoundLevel_f = RsoundLevel * volumeSmooth + RsoundLevel_f * (1 - volumeSmooth);
  LsoundLevel_f = RsoundLevel_f;

  if (emptyBright > 5) {
    for (int i = 0; i < NUM_LEDS; i++)
      leds[i] = CHSV(emptyColor, 255, emptyBright); //тут заливка подложки светодиодов
  }

  if (RsoundLevel_f > 15 && LsoundLevel_f > 15) {
    averageLevel = (float)(RsoundLevel_f + LsoundLevel_f) / 2 * averK + averageLevel * (1 - averK);
    maxLevel = (float)averageLevel * MAX_COEF;
    Rlenght = map(RsoundLevel_f, 0, maxLevel, 0, HALF_LED);
    Llenght = map(LsoundLevel_f, 0, maxLevel, 0, HALF_LED);

    Rlenght = constrain(Rlenght, 0, HALF_LED);
    Llenght = constrain(Llenght, 0, HALF_LED);
  }
}

void updateLowPass() {
  int maxNoiseLevel = 0;                          
  int noiseLevel = 0;

  for (byte i = 0; i < 200; i++) {
    noiseLevel = analogRead(soundRList[portAux]);        
    if (noiseLevel > maxNoiseLevel) maxNoiseLevel = noiseLevel;                                                
  }
  lowPassVolume = maxNoiseLevel + addNoiseLowPassVolume;

  maxNoiseLevel = 0;
  for (byte i = 0; i < 100; i++) {
    ADC_FFT();

    for (byte j = 2; j < 32; j++) 
      if (spectr[j] > maxNoiseLevel
      ) 
        maxNoiseLevel = noiseLevel;
  }
  lowPassSpectr = maxNoiseLevel + addNoiseLowPassSpectr;
}