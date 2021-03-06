/* 
 *  Simple code to run airhorn for a regatta start
 *  
 *  (C) 2018
 *  Original Author: John Bayldon
 *  
 *  licensing TBD
 *  
 *  
 *  
 *  
 *  
 *  
 */





enum state_enum {ORANGE,WARNING, PREP, READY_TO_START, AP, WAITING};

const long FIVE = 1000L*5*60;
const long FOUR = 1000L*4*60;
const long ONE = 1000L*1*60;
const long SHORT_HORN = 500L;
const long LONG_HORN = 2000L;
const long SHORT_BEEP = 200L;
const long LONG_BEEP = 500L;
const uint8_t HORN_PIN = 13;
const uint8_t BEEP_PIN = 9;

uint8_t timer_state = WAITING;
bool start_sequence = 0;
long start_time = 0;
long horn_timer = 0;
long beep_timer = 0;
long curr_time = 60000L*20;;
int inByte = 0;

void beep_on();
void beep_off();
void horn_on();
void horn_off();
void update_display();
void interval_beeps();



void setup() {
  // setup serial uses pins 0,1 on teensy LC
  Serial1.begin(9600);
  pinMode(HORN_PIN,OUTPUT);
  start_sequence = 0;
  
}

void loop() {
  //serial messages
  if (Serial1.available()>0){
    inByte = Serial1.read();
    switch (inByte ){
      case  114: //r
        timer_state = WAITING;
        start_sequence = 0;
        break;
       case 112: //p
        break;
       case 115: //s
        start_sequence = 1;
        break;
       case 116:
       if (timer_state == 5){
        start_time = millis() + ONE;
        timer_state = READY_TO_START;
        start_sequence = 1;
        horn_on();
        horn_timer = millis() + SHORT_HORN;
       }
        break;
       case 104:
        horn_on();
        horn_timer = millis()+ SHORT_HORN;
    }
  }
  //actions these work in all states
  if (millis()>horn_timer){
    horn_off();
  }
  if (millis()>beep_timer){
    beep_off();
  }
  if (millis()%1000L<1){
    update_display();
  }


  curr_time = start_time - millis();
  
  // state handling, put code for the state into the handling case:
  switch(timer_state){
    case ORANGE:
    interval_beeps();
      if (curr_time<FIVE){
        timer_state = WARNING;
        horn_on();
        horn_timer = millis()+SHORT_HORN;
      }
      break;
     case WARNING:
     interval_beeps();
      //countdown beeps
      if (curr_time< FOUR +1000L*4){
        if (curr_time%(1000L)<1){
          beep_on();
          beep_timer = millis() + SHORT_BEEP;
        }
      }
      //change state to prep with 4 to go
      if (curr_time<FOUR){
        timer_state = PREP;
        horn_on();
        horn_timer = millis()+SHORT_HORN;
        beep_on();
        beep_timer = millis() + LONG_BEEP;
      }
      break;
     case PREP:
      interval_beeps();
      //countdown beeps
       if (curr_time< ONE+1000L*4){
        if (curr_time%(1000L)<1){
          beep_on();
          beep_timer = millis() + SHORT_BEEP;
        }
      }
      //chnage state with one minute to go
      if (curr_time<ONE){
        timer_state = READY_TO_START;
        horn_on();
        horn_timer = millis()+LONG_HORN;
        beep_on();
        beep_timer = millis()+LONG_BEEP;
      }
      break;
     case READY_TO_START:
        interval_beeps();
        //coundown beeps
        if (curr_time< 1000L*4){
        if (curr_time%(1000L)<1){
          beep_on();
          beep_timer = millis() + SHORT_BEEP;
        }
      }
      //chnage state at start
      if (curr_time<0){
        start_time = millis()+FIVE;
        timer_state = WARNING;
        horn_on();
        horn_timer = millis()+SHORT_HORN;
        beep_on();
        beep_timer = millis()+LONG_BEEP;
      }
      break;
     case AP:
     //not set up yet
      break;
     case WAITING:
      // change case once the start_sequence flag is set
      if (start_sequence){
        start_time = millis() + FIVE + FIVE;
        timer_state = ORANGE;
        horn_on();
        horn_timer = millis() + LONG_HORN;
      }
      break;
      
    
  }


}

void interval_beeps(){
    // timing beeps at 15's and minutes
    //long beep every minute
    if ((start_time-millis())%(1000L*60)<1){
    beep_on();
    beep_timer = millis()+LONG_BEEP;
    }
    //short beep every 15 seconds
    else if ((start_time-millis())%(1000L*15)<1){
    beep_on();
    beep_timer = millis()+SHORT_BEEP;
  }
  
}



void horn_on(){
  digitalWrite(HORN_PIN,HIGH);
}

void horn_off(){
  digitalWrite(HORN_PIN,LOW);
}

void beep_on(){
  tone(BEEP_PIN,440);
}

void beep_off(){
  noTone(BEEP_PIN);
}

void update_display(){
  //for now display send info to bluetooth. modify here for other displays
  long curr_time = start_time-millis();
  String out_string = String(curr_time/1000/60);
  if (out_string.length()<2){
    out_string = String("0" + out_string);
  }
  Serial1.print("*");
  Serial1.print(out_string);
  Serial1.print(":");
  out_string = String(curr_time/1000%60);
  if (out_string.length()<2){
    out_string = String("0" + out_string);
  }
  Serial1.print(out_string);
  Serial1.print(',');
  Serial1.println(timer_state);
}

