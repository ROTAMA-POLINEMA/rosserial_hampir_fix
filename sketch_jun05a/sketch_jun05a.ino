#include <ros.h>
#include <std_msgs/Int32.h>
#include <Servo.h>

Servo kanan, kiri, sayap;

ros::NodeHandle nh;
int tes_pesan = 0;

int max_sayap = 50;
int min_sayap = 100;
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
  nh.initNode();
  nh.subscribe(sub);

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
}

//servo_function

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

void sayapStart() {
  sayap.write(110);
  delay(10);
}
// end of sayap

// bulu
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
// end of bulu

void servoSubs(int msg) {
  if (msg == 1) pmasker(); // pasang masker
  else if (msg == 2) lmasker(); // lepas masker
  else if (msg == 3) pasangBulu(); // gerakan pasang bulu
  else if (msg == 4) lepasBulu(); // gerakan lepas bulu
  else if (msg == 5) nganjat(10); // gerakan nganjat
  else if (msg == 6) naikTurun2(); // gerakan naik turun delay
  else normal(1000); // ketika nggak ngapangapain
}

void sayapSub(int msg) {
  if (msg == 7) zerofull(700, 5); // naik turun sayap
  else if (msg == 8) sayapBuka(); // buka sayap
  else if (msg == 9) sayapTutup(); // tutup sayap
  else if (msg == 10)sayapStart(); // sayap pada saat start
}


void loop() {

  servoSubs(tes_pesan);
  sayapSub(tes_pesan);

  nh.spinOnce();
  delay(10);
}
