#include <ros.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Int32.h>

void messageCb( const std_msgs::Float32& servo_angle);

ros::NodeHandle nh;
ros::Subscriber<std_msgs::Float32> sub("servo", &messageCb );

std_msgs::Float32 str_msg;
ros::Publisher chatter("chatter", &str_msg);
std_msgs::Float32 str_msg2;
ros::Publisher targett("targett", &str_msg2);

float angle = 90;
float ex = 0;

int Relaypin1 = 4;                 // IN1
int Relaypin2 = 5;                 // IN2
int Relaypin3 = 6;                 // IN3
int Relaypin4 = 7;                 // IN4
int resis = A0;
int present_resis;
float target = 512;
char Direction = 0; //  -1:왼쪽, 0:정지, 1:오른쪽.


void messageCb( const std_msgs::Float32& servo_angle) {
  angle = servo_angle.data;
}


void actuator_stop() {
  digitalWrite (Relaypin1, HIGH);
  digitalWrite (Relaypin2, HIGH);
  digitalWrite (Relaypin3, HIGH);
  digitalWrite (Relaypin4, HIGH);
}

void actuator_left() {
  digitalWrite (Relaypin1, HIGH);
  digitalWrite (Relaypin4, HIGH);
  digitalWrite (Relaypin2, LOW);
  digitalWrite (Relaypin3, LOW);
}

void actuator_right() {
  digitalWrite (Relaypin2, HIGH);
  digitalWrite (Relaypin3, HIGH);
  digitalWrite (Relaypin1, LOW);
  digitalWrite (Relaypin4, LOW);
}

void setup() {
  //  Serial.begin(57600);

  nh.initNode();
  nh.subscribe(sub);
  nh.advertise(chatter);
  nh.advertise(targett);
  pinMode(Relaypin1, OUTPUT);
  pinMode(Relaypin2, OUTPUT);
  pinMode(Relaypin3, OUTPUT);
  pinMode(Relaypin4, OUTPUT);
  /*
    actuator_down();
    delay(700);
    actuator_up();
    delay(700);
    actuator_stop();
  */

  actuator_stop();
}

void loop() {

  nh.spinOnce();
  angle = constrain(angle, 30, 150);
  target = map(angle, 30, 150, 20, 970);
  target = constrain(target, 20, 970);
  present_resis = analogRead(A0);

  str_msg.data = present_resis;
  chatter.publish( &str_msg );

  str_msg2.data = target;
  targett.publish( &str_msg2 );

  if (present_resis > (target + 15)) {
    Direction = -1;
    actuator_left();

  } else if (present_resis < (target - 15)) {
    actuator_right();
    Direction = 1;
  } else {

    switch (Direction) {

      case 1:
        delay(30);
        actuator_stop();
        delay(1);
        actuator_left();
        delay(20);
        actuator_stop();
        Direction = 0;
        break;


      case -1:
        delay(30);
        actuator_stop();
        delay(1);
        actuator_right();
        delay(20);
        Direction = 0;
        actuator_stop();
        break;


      case 0:
        actuator_stop();
        break;
    }
  }
  delay(1);
}
