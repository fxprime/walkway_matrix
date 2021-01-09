#include <Arduino.h> 
#include <ArduinoOTA.h>   
#define FASTLED_ESP8266_RAW_PIN_ORDER

 
#include <FastLED.h>


#define RGT_SENS D6
#define LFT_SENS D0
#define LED_PIN 14 //D5


const char *WIFI_NAME = "Kanuengnit_kiss2G";
const char *WIFI_PW = "3213213213";
 
//
// Mark's xy coordinate mapping code.  See the XYMatrix for more information on it.
//

// Params for width and height
const uint8_t kMatrixWidth = 180;
const uint8_t kMatrixHeight = 10;
#define MAX_DIMENSION ((kMatrixWidth>kMatrixHeight) ? kMatrixWidth : kMatrixHeight)
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
// Param for different pixel layouts
const bool    kMatrixSerpentineLayout = true;


uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;

  if( kMatrixSerpentineLayout == false) {
    i = (y * kMatrixWidth) + x;
  }

  if( kMatrixSerpentineLayout == true) {
    if( y & 0x01) {
      // Odd rows run backwards
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    } else {
      // Even rows run forwards
      i = (y * kMatrixWidth) + x;
    }
  } 

  return i;
}

// The leds
CRGB leds[kMatrixWidth * kMatrixHeight];

 

void setup() {


  pinMode(RGT_SENS, INPUT);
  pinMode(LFT_SENS, INPUT);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);  // GRB ordering is typical
  LEDS.setBrightness(50); 


  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting... ");
  // WiFi.begin(WIFI_NAME, WIFI_PW);
  // while (WiFi.waitForConnectResult() != WL_CONNECTED) {
  //     WiFi.begin(WIFI_NAME, WIFI_PW);
  //     Serial.println("WiFi failed, retrying.");
  // }
  // Serial.println("Booted");
  
  




  // ArduinoOTA.onStart([]() {  
  //  String type;  
  //  if (ArduinoOTA.getCommand() == U_FLASH) {  
  //   type = "sketch";  
  //  } else { // U_SPIFFS  
  //   type = "filesystem";  
  //  }  
   
  //  // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()  
  //  Serial.println("Start updating " + type);  
  // });  
  // ArduinoOTA.onEnd([]() {  
  //  Serial.println("\nEnd");  
  // });  
  // ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {  
  //  Serial.printf("Progress: %u%%\r", (progress / (total / 100)));  
  // });  
  // ArduinoOTA.onError([](ota_error_t error) {  
  //  Serial.printf("Error[%u]: ", error);  
  //  if (error == OTA_AUTH_ERROR) {  
  //   Serial.println("Auth Failed");  
  //  } else if (error == OTA_BEGIN_ERROR) {  
  //   Serial.println("Begin Failed");  
  //  } else if (error == OTA_CONNECT_ERROR) {  
  //   Serial.println("Connect Failed");  
  //  } else if (error == OTA_RECEIVE_ERROR) {  
  //   Serial.println("Receive Failed");  
  //  } else if (error == OTA_END_ERROR) {  
  //   Serial.println("End Failed");  
  //  }  
  // });  
  // ArduinoOTA.begin();  
  // Serial.println("Ready");  
  // Serial.print("IP address: ");  
  // Serial.println(WiFi.localIP());  


}

 
void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(200); } }


void loop() { 
  // ArduinoOTA.handle();  
  








  // static unsigned long last_step=millis();
  // unsigned long cur_time=millis();

  // static int ir =0;
  // if(cur_time - last_step> 2000) {
  //   last_step = cur_time;
  //   for(int i = ir*180; i < (ir+1)*180; i++) {
  //     leds[i] = CHSV(0,255,255);
  //   } 
  //   ir++;
  //   ir=ir%10;
  // }

  

  // LEDS.show();
  // for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(180); } 









  static unsigned long last_right_st = 0;
  static unsigned long last_left_st = 0;
  static int sensor_code = 0; //0:no detect, 1:left detect, 2:right detect

  


  static unsigned long last_step=millis();
  unsigned long cur_time=millis();

  static int upper_pos = 0; 
  static int lower_pos = 0; 

  // if( cur_time - last_step > 100/kMatrixWidth ) {
  //   last_step = cur_time;

    upper_pos+=1;
    upper_pos%=kMatrixWidth;
    lower_pos=kMatrixWidth-upper_pos;
  // }
   

  bool left_detected = digitalRead(LFT_SENS);
  bool right_detected = digitalRead(RGT_SENS);
 
  //State check
  if(last_left_st ==0 && last_right_st ==0 || last_right_st==last_left_st) {
    sensor_code = 0;
    last_left_st  =  (left_detected ? cur_time : 0);
    last_right_st =  (right_detected ? cur_time : 0);
    Serial.println("Check " + String(left_detected) + ", " + String(right_detected));

  }else if(last_left_st > last_right_st) { 
    last_right_st = 0;
    if(cur_time - last_left_st < 8000) {
      sensor_code = 1;
      Serial.println("Left doing");
      last_left_st  =  (left_detected ? cur_time : last_left_st); //If still detect, update time
    }else{
      last_left_st = 0;
      sensor_code = 0;
      Serial.println("Left finished");

    }
  }else if(last_left_st < last_right_st) {
    last_left_st = 0;
    if(cur_time - last_right_st < 8000) {
      sensor_code = 2;
      Serial.println("Right doing");
      last_right_st  =  (right_detected ? cur_time : last_right_st); //If still detect, update time
    }else{
      last_right_st = 0;
      sensor_code = 0;
      Serial.println("Right finished");
    }
  } 
  Serial.println("Code = " + String(sensor_code) + "-> "+String(last_left_st) + ", " +String(last_right_st));
  
  for(int i = 0; i < kMatrixWidth; i++) {
    if(sensor_code==2) {
      if(i==upper_pos || i==(upper_pos+600)%kMatrixWidth || i==(upper_pos+1200)%kMatrixWidth ) {
        leds[XY( (i)%kMatrixWidth, 0)] = CHSV(0,255,255);
        leds[XY( (i+3)%kMatrixWidth, 1)] = CHSV(0,255,255);
        leds[XY( (i+6)%kMatrixWidth, 2)] = CHSV(0,255,255);
        leds[XY( (i+3)%kMatrixWidth, 3)] = CHSV(0,255,255);
        leds[XY( (i)%kMatrixWidth, 4)] = CHSV(0,255,255);
        
        leds[XY( (i+1)%kMatrixWidth, 0)] = CHSV(0,255,255);
        leds[XY( (i+4)%kMatrixWidth, 1)] = CHSV(0,255,255);
        leds[XY( (i+7)%kMatrixWidth, 2)] = CHSV(0,255,255);
        leds[XY( (i+4)%kMatrixWidth, 3)] = CHSV(0,255,255);
        leds[XY( (i+1)%kMatrixWidth, 4)] = CHSV(0,255,255);
      }  
    }

    if(sensor_code==1) {
      if(i==lower_pos || i==(lower_pos+600)%kMatrixWidth || i==(lower_pos+1200)%kMatrixWidth ) {

        leds[XY( (i+6)%kMatrixWidth, 5)] = CHSV(0,255,255);
        leds[XY( (i+3)%kMatrixWidth, 6)] = CHSV(0,255,255);
        leds[XY( (i)%kMatrixWidth, 7)] = CHSV(0,255,255);
        leds[XY( (i+3)%kMatrixWidth, 8)] = CHSV(0,255,255);
        leds[XY( (i+6)%kMatrixWidth, 9)] = CHSV(0,255,255);

        leds[XY( (i+7)%kMatrixWidth, 5)] = CHSV(0,255,255);
        leds[XY( (i+4)%kMatrixWidth, 6)] = CHSV(0,255,255);
        leds[XY( (i+1)%kMatrixWidth, 7)] = CHSV(0,255,255);
        leds[XY( (i+4)%kMatrixWidth, 8)] = CHSV(0,255,255);
        leds[XY( (i+7)%kMatrixWidth, 9)] = CHSV(0,255,255);
      }
    }
    
    
  }  
  LEDS.show(); 

  fadeall();  
}




























// #include <ArduinoOTA.h>   
// #define FASTLED_ESP8266_RAW_PIN_ORDER

 
// #include <FastLED.h>

// #define LED_PIN 14 //D5


// const char *WIFI_NAME = "Kanuengnit_kiss2G";
// const char *WIFI_PW = "3213213213";
  
// #define LED_PIN     14
// #define BRIGHTNESS  50
// #define LED_TYPE    WS2812B
// #define COLOR_ORDER GRB

// const uint8_t kMatrixWidth  = 180;
// const uint8_t kMatrixHeight = 10;
// const bool    kMatrixSerpentineLayout = true;


// // This example combines two features of FastLED to produce a remarkable range of
// // effects from a relatively small amount of code.  This example combines FastLED's 
// // color palette lookup functions with FastLED's Perlin/simplex noise generator, and
// // the combination is extremely powerful.
// //
// // You might want to look at the "ColorPalette" and "Noise" examples separately
// // if this example code seems daunting.
// //
// // 
// // The basic setup here is that for each frame, we generate a new array of 
// // 'noise' data, and then map it onto the LED matrix through a color palette.
// //
// // Periodically, the color palette is changed, and new noise-generation parameters
// // are chosen at the same time.  In this example, specific noise-generation
// // values have been selected to match the given color palettes; some are faster, 
// // or slower, or larger, or smaller than others, but there's no reason these 
// // parameters can't be freely mixed-and-matched.
// //
// // In addition, this example includes some fast automatic 'data smoothing' at 
// // lower noise speeds to help produce smoother animations in those cases.
// //
// // The FastLED built-in color palettes (Forest, Clouds, Lava, Ocean, Party) are
// // used, as well as some 'hand-defined' ones, and some proceedurally generated
// // palettes.


// #define NUM_LEDS (kMatrixWidth * kMatrixHeight)
// #define MAX_DIMENSION ((kMatrixWidth>kMatrixHeight) ? kMatrixWidth : kMatrixHeight)

// // The leds
// CRGB leds[kMatrixWidth * kMatrixHeight];

// // The 16 bit version of our coordinates
// static uint16_t x;
// static uint16_t y;
// static uint16_t z;

// // We're using the x/y dimensions to map to the x/y pixels on the matrix.  We'll
// // use the z-axis for "time".  speed determines how fast time moves forward.  Try
// // 1 for a very slow moving effect, or 60 for something that ends up looking like
// // water.
// uint16_t speed = 20; // speed is set dynamically once we've started up

// // Scale determines how far apart the pixels in our noise matrix are.  Try
// // changing these values around to see how it affects the motion of the display.  The
// // higher the value of scale, the more "zoomed out" the noise iwll be.  A value
// // of 1 will be so zoomed in, you'll mostly see solid colors.
// uint16_t scale = 30; // scale is set dynamically once we've started up

// // This is the array that we keep our computed noise values in
// uint8_t noise[MAX_DIMENSION][MAX_DIMENSION];

// CRGBPalette16 currentPalette( PartyColors_p );
// uint8_t       colorLoop = 1;

// void setup() {
//     Serial.begin(115200);
//   Serial.println();
//   Serial.println("Booting... ");
//   WiFi.begin(WIFI_NAME, WIFI_PW);
//   while (WiFi.waitForConnectResult() != WL_CONNECTED) {
//       WiFi.begin(WIFI_NAME, WIFI_PW);
//       Serial.println("WiFi failed, retrying.");
//   }
//   Serial.println("Booted");



//   LEDS.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds,NUM_LEDS);
//   LEDS.setBrightness(BRIGHTNESS);

//   // Initialize our coordinates to some random values
//   x = random16();
//   y = random16();
//   z = random16();




//   ArduinoOTA.onStart([]() {  
//    String type;  
//    if (ArduinoOTA.getCommand() == U_FLASH) {  
//     type = "sketch";  
//    } else { // U_SPIFFS  
//     type = "filesystem";  
//    }  
   
//    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()  
//    Serial.println("Start updating " + type);  
//   });  
//   ArduinoOTA.onEnd([]() {  
//    Serial.println("\nEnd");  
//   });  
//   ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {  
//    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));  
//   });  
//   ArduinoOTA.onError([](ota_error_t error) {  
//    Serial.printf("Error[%u]: ", error);  
//    if (error == OTA_AUTH_ERROR) {  
//     Serial.println("Auth Failed");  
//    } else if (error == OTA_BEGIN_ERROR) {  
//     Serial.println("Begin Failed");  
//    } else if (error == OTA_CONNECT_ERROR) {  
//     Serial.println("Connect Failed");  
//    } else if (error == OTA_RECEIVE_ERROR) {  
//     Serial.println("Receive Failed");  
//    } else if (error == OTA_END_ERROR) {  
//     Serial.println("End Failed");  
//    }  
//   });  
//   ArduinoOTA.begin();  
//   Serial.println("Ready");  
//   Serial.print("IP address: ");  
//   Serial.println(WiFi.localIP());  

// }


// //
// // Mark's xy coordinate mapping code.  See the XYMatrix for more information on it.
// //
// uint16_t XY( uint8_t x, uint8_t y)
// {
//   uint16_t i;
//   if( kMatrixSerpentineLayout == false) {
//     i = (y * kMatrixWidth) + x;
//   }
//   if( kMatrixSerpentineLayout == true) {
//     if( y & 0x01) {
//       // Odd rows run backwards
//       uint8_t reverseX = (kMatrixWidth - 1) - x;
//       i = (y * kMatrixWidth) + reverseX;
//     } else {
//       // Even rows run forwards
//       i = (y * kMatrixWidth) + x;
//     }
//   }
//   return i;
// }

// // Fill the x/y array of 8-bit noise values using the inoise8 function.
// void fillnoise8() {
//   // If we're runing at a low "speed", some 8-bit artifacts become visible
//   // from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
//   // The amount of data smoothing we're doing depends on "speed".
//   uint8_t dataSmoothing = 0;
//   if( speed < 50) {
//     dataSmoothing = 200 - (speed * 4);
//   }
  
//   for(int i = 0; i < MAX_DIMENSION; i++) {
//     int ioffset = scale * i;
//     for(int j = 0; j < MAX_DIMENSION; j++) {
//       int joffset = scale * j;
      
//       uint8_t data = inoise8(x + ioffset,y + joffset,z);

//       // The range of the inoise8 function is roughly 16-238.
//       // These two operations expand those values out to roughly 0..255
//       // You can comment them out if you want the raw noise data.
//       data = qsub8(data,16);
//       data = qadd8(data,scale8(data,39));

//       if( dataSmoothing ) {
//         uint8_t olddata = noise[i][j];
//         uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
//         data = newdata;
//       }
      
//       noise[i][j] = data;
//     }
//   }
  
//   z += speed;
  
//   // apply slow drift to X and Y, just for visual variation.
//   x += speed / 8;
//   y -= speed / 16;
// }

// void mapNoiseToLEDsUsingPalette()
// {
//   static uint8_t ihue=0;
  
//   for(int i = 0; i < kMatrixWidth; i++) {
//     for(int j = 0; j < kMatrixHeight; j++) {
//       // We use the value at the (i,j) coordinate in the noise
//       // array for our brightness, and the flipped value from (j,i)
//       // for our pixel's index into the color palette.

//       uint8_t index = noise[j][i];
//       uint8_t bri =   noise[i][j];

//       // if this palette is a 'loop', add a slowly-changing base value
//       if( colorLoop) { 
//         index += ihue;
//       }

//       // brighten up, as the color palette itself often contains the 
//       // light/dark dynamic range desired
//       if( bri > 127 ) {
//         bri = 255;
//       } else {
//         bri = dim8_raw( bri * 2);
//       }

//       CRGB color = ColorFromPalette( currentPalette, index, bri);
//       leds[XY(i,j)] = color;
//     }
//   }
  
//   ihue+=1;
// }


// // This function generates a random palette that's a gradient
// // between four different colors.  The first is a dim hue, the second is 
// // a bright hue, the third is a bright pastel, and the last is 
// // another bright hue.  This gives some visual bright/dark variation
// // which is more interesting than just a gradient of different hues.
// void SetupRandomPalette()
// {
//   currentPalette = CRGBPalette16( 
//                       CHSV( random8(), 255, 32), 
//                       CHSV( random8(), 255, 255), 
//                       CHSV( random8(), 128, 255), 
//                       CHSV( random8(), 255, 255)); 
// }

// // This function sets up a palette of black and white stripes,
// // using code.  Since the palette is effectively an array of
// // sixteen CRGB colors, the various fill_* functions can be used
// // to set them up.
// void SetupBlackAndWhiteStripedPalette()
// {
//   // 'black out' all 16 palette entries...
//   fill_solid( currentPalette, 16, CRGB::Black);
//   // and set every fourth one to white.
//   currentPalette[0] = CRGB::White;
//   currentPalette[4] = CRGB::White;
//   currentPalette[8] = CRGB::White;
//   currentPalette[12] = CRGB::White;

// }

// // This function sets up a palette of purple and green stripes.
// void SetupPurpleAndGreenPalette()
// {
//   CRGB purple = CHSV( HUE_PURPLE, 255, 255);
//   CRGB green  = CHSV( HUE_GREEN, 255, 255);
//   CRGB black  = CRGB::Black;
  
//   currentPalette = CRGBPalette16( 
//     green,  green,  black,  black,
//     purple, purple, black,  black,
//     green,  green,  black,  black,
//     purple, purple, black,  black );
// }






// // There are several different palettes of colors demonstrated here.
// //
// // FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// // OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
// //
// // Additionally, you can manually define your own color palettes, or you can write
// // code that creates color palettes on the fly.

// // 1 = 5 sec per palette
// // 2 = 10 sec per palette
// // etc
// #define HOLD_PALETTES_X_TIMES_AS_LONG 1

// void ChangePaletteAndSettingsPeriodically()
// {
//   uint8_t secondHand = ((millis() / 1000) / HOLD_PALETTES_X_TIMES_AS_LONG) % 60;
//   static uint8_t lastSecond = 99;
  
//   if( lastSecond != secondHand) {
//     lastSecond = secondHand;
//     if( secondHand ==  0)  { currentPalette = RainbowColors_p;         speed = 20; scale = 30; colorLoop = 1; }
//     if( secondHand ==  5)  { SetupPurpleAndGreenPalette();             speed = 10; scale = 50; colorLoop = 1; }
//     if( secondHand == 10)  { SetupBlackAndWhiteStripedPalette();       speed = 20; scale = 30; colorLoop = 1; }
//     if( secondHand == 15)  { currentPalette = ForestColors_p;          speed =  8; scale =120; colorLoop = 0; }
//     if( secondHand == 20)  { currentPalette = CloudColors_p;           speed =  4; scale = 30; colorLoop = 0; }
//     if( secondHand == 25)  { currentPalette = LavaColors_p;            speed =  8; scale = 50; colorLoop = 0; }
//     if( secondHand == 30)  { currentPalette = OceanColors_p;           speed = 20; scale = 90; colorLoop = 0; }
//     if( secondHand == 35)  { currentPalette = PartyColors_p;           speed = 20; scale = 30; colorLoop = 1; }
//     if( secondHand == 40)  { SetupRandomPalette();                     speed = 20; scale = 20; colorLoop = 1; }
//     if( secondHand == 45)  { SetupRandomPalette();                     speed = 50; scale = 50; colorLoop = 1; }
//     if( secondHand == 50)  { SetupRandomPalette();                     speed = 90; scale = 90; colorLoop = 1; }
//     if( secondHand == 55)  { currentPalette = RainbowStripeColors_p;   speed = 30; scale = 20; colorLoop = 1; }
//   }
// }


// void loop() {
//     ArduinoOTA.handle();  

//   // Periodically choose a new palette, speed, and scale
//   ChangePaletteAndSettingsPeriodically();

//   // generate noise data
//   fillnoise8();
  
//   // convert the noise data to colors in the LED array
//   // using the current palette
//   mapNoiseToLEDsUsingPalette();

//   LEDS.show();
//   // delay(10);
// }

