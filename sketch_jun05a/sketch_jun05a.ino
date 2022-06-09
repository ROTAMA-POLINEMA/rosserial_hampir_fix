#include <ros.h>
#include <std_msgs/Int32.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#define LED_PIN    PD5
#define LED_COUNT  21

Servo kanan, kiri, sayap;
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

ros::NodeHandle nh;
int tes_pesan = 0;

int max_sayap = 60;
int min_sayap = 180;
int count_5 = 0;

void messageCb( const std_msgs::Int32& msg ) {
  tes_pesan = msg.data;
  Serial.println(tes_pesan);
}

ros::Subscriber<std_msgs::Int32> sub("test_msg", &messageCb);

void setup() {
  Serial.begin(9600);
  kanan.attach(A1);
  kiri.attach(A2);
  sayap.attach(A3);
  pinMode(LED_PIN, OUTPUT);
  nh.initNode();
  nh.subscribe(sub);

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  strip.begin();
  strip.show();
  strip.setBrightness(50);
}

//LED_function

void theaterChase(uint32_t color, int wait) {
  for (int a = 0; a < 10; a++) {
    for (int b = 0; b < 3; b++) {
      strip.clear();
      for (int c = b; c < strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color);
      }
      strip.show();
      delay(wait);
    }
  }
}

void rainbow(int wait) {
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
    for (int i = 0; i < strip.numPixels(); i++) {
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show();
    delay(wait);
  }
}

void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;
  for (int a = 0; a < 30; a++) {
    for (int b = 0; b < 3; b++) {
      strip.clear();
      for (int c = b; c < strip.numPixels(); c += 3) {
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue));
        strip.setPixelColor(c, color);
      }
      strip.show();
      delay(wait);
      firstPixelHue += 65536 / 90;
    }
  }
}


//servo_function

//reset atau back to normal position
void normal(int jeda) {
  kanan.write(90);
  kiri.write(90);
  delay(jeda);
}


void pmasker () {
  kanan.write(140);
  kiri.write(90);
  delay(1000);
}


//nganjat
void nganjat(int count) {
  for (int i = 0; i < count; i++) {
    for (int a = 110; a >= 50 ; a -= 1) {
      kanan.write(a);
      kiri.write(180 - a);
      delay(15);
    }
    for (int a = 50; a <= 110; a += 1) {
      kanan.write(a);
      kiri.write(180 - a);
      delay(15);
    }
  }
}

void naikTurun2() {
  for (int a = 110; a >= 50 ; a -= 1) {
    kanan.write(a);
    kiri.write(180 - a);
    delay(15);
  }
  for (int a = 50; a <= 110; a += 1) {
    kanan.write(a);
    kiri.write(180 - a);
    delay(15);
  }
  count_5++;
  if (count_5 == 2) {
    delay(2000);
    count_5 = 0;
  }
}

//lepas masker
void lmasker () {
  kanan.write(90);
  kiri.write(40);
  delay(100);
}


//sayap
void zerofull(int jeda, int count) {
  for (int i = 0; i < count; i++) {
    sayap.write(min_sayap);
    delay(1000);
    sayap.write(max_sayap);
    delay(1000);
  }
  delay(jeda);
}

void sayapBuka() {
  sayap.write(max_sayap);
  delay(10);
}
void sayapTutup() {
  sayap.write(min_sayap);
  delay(10);
}

void lepasBulu() {
  kanan.write(140);
  kiri.write(40);
  delay(10);
}

void pasangBulu() {
  kanan.write(140);
  kiri.write(40);
  delay(10);
}

void servoSubs(int msg) {
  if (msg == 1) pmasker(); // pasang masker
  else if (msg == 2) lmasker(); // lepas masker
  else if (msg == 3) pasangBulu(); // gerakan salam pembuka
  else if (msg == 4) lepasBulu(); // gerakan ngasai
  else if (msg == 5) nganjat(10);
  else if (msg == 6) naikTurun2();
  else normal(1000); // ketika nggak ngapangapain
}

void sayapSub(int msg) {
  if (msg == 10) zerofull(700, 5);
  else if (msg == 11) sayapBuka();
  else if (msg == 12) sayapTutup();
}

// end of servo function

void ledstripSubs(int msg) {
  if (msg == 13) theaterChase(strip.Color(127, 127, 127), 50);
  else if (msg == 14) rainbow(100);
  else if (msg == 15) theaterChaseRainbow(100);
}

void loop() {

  servoSubs(tes_pesan);
  sayapSub(tes_pesan);
  ledstripSubs(tes_pesan);

  nh.spinOnce();
  delay(10);
}
