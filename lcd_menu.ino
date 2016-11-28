#include <Time.h>
#include <TimeLib.h>

// lcd_menu.ino
// include the library code:
#include <LiquidCrystal.h>



#include <String.h>
#include <EEPROM.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
String sys_time = "";
const int lbtn = 8;
const int rbtn = 9;
const int cbtn = 6;
const int leds_pin = 10;
const int ledPin =  13;      // the number of the LED pin
int ledState = LOW;
int leds_state = LOW;
int lbtn_state = 0;
int rbtn_state = 0;
int cbtn_state = 0;
int last_cbtn_state = 0;
int state = 0;
long check_schedule_delay = 0;
byte column_pos = 0;
byte line_pos = 0;
byte digit = 0;
int set_time[6] = {0,0,0,0,0,0};
String set_time_string = "";
int set_PWM[3] = {0,0,0};
String set_PWM_string = "";
int currentPWM = 0;
long last_cbtn_debounce_time = 0;
bool cbtn_pressed = false;

void setup() {
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  Serial.begin(9600);
  // Print a message to the LCD.
  setTime(16,20,0,1,1,2014); //hr,min,sec,day,month,yr  
  pinMode(lbtn, INPUT);
  pinMode(rbtn, INPUT);
  pinMode(cbtn, INPUT);
  pinMode(leds_pin,OUTPUT);
  pinMode(ledPin,OUTPUT);

  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM10); // = 10100001
  TCCR1B = _BV(WGM12) | _BV(CS11) | _BV(CS10); // = 00001010
  OCR1B = 0; // 255 duty B
  //16 bit timer 1 (set up as 8 bit timer with fast pwm)

}

void loop() 
{
  if (state == 0)
  {
    display_time();
    cbtn_state = digitalRead(cbtn);
    /*
    cbtn_pressed = debounce_btn (cbtn,cbtn_state,last_cbtn_state,last_cbtn_debounce_time,50);
    if (cbtn_pressed == true)
    {
      state = 1;
      cbtn_pressed = false;
    }
    cbtn_state = LOW;
    delay(150);
    */    
    if (cbtn_state == HIGH)
    {
      state = 1;
    }
    cbtn_state = LOW;
    delay(150);
  }
  switch (state) {
      case 1: // Set Time menu
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("1. Set");
        lcd.setCursor(0,1);
        lcd.print ("Time");
        while (cbtn_state == LOW) //wait for cbrn to be pressed
        {
          cbtn_state = digitalRead(cbtn); //update btn states
          rbtn_state = digitalRead(rbtn);
          lbtn_state = digitalRead(lbtn);
          if(rbtn_state == HIGH)
          {
            //digitalWrite(ledPin, HIGH);
            state = 3;
            delay(150);
            rbtn_state = LOW;            
            cbtn_state = HIGH; // act as button pressed
          } //rbtn pressed
        }//cbtn has been pressed
        cbtn_state = LOW;
        lcd.clear();
        delay(150);
        if (state == 1)
        {
          state = 2;    
        }
        break;
      case 2: // Set Time sub-menu
        lcd.setCursor(0,0);
        set_time_string.concat(set_time[0]);
        set_time_string.concat(set_time[1]);
        set_time_string.concat(":");
        set_time_string.concat(set_time[2]);
        set_time_string.concat(set_time[3]);
        set_time_string.concat(":");
        set_time_string.concat(set_time[4]);
        set_time_string.concat(set_time[5]);
        //lcd.print("  :  :  ");      
        lcd.print (set_time_string);
        set_time_string = "";
        lcd.setCursor(column_pos,line_pos);
        lcd.cursor();
        while(cbtn_state == LOW)
        {
          cbtn_state = digitalRead(cbtn);
          lbtn_state = digitalRead(lbtn);
          rbtn_state = digitalRead(rbtn);
          if (digit > 9) 
          {
            digit = 0;
          }
          if (rbtn_state == HIGH)
          {
            digit ++;
            display_digit(digit);
            rbtn_state = LOW;
            delay(150);  
          }
          if (lbtn_state == HIGH)
          {
            digit --;
            display_digit(digit);
            lbtn_state = LOW;
            delay(150);  
          }
        }//cbtn has been pressed
        switch (column_pos) {
            case 0:
              set_time[0] = digit;
              break;
            case 1:
              set_time[1] = digit;
              break;
            case 3:
              set_time[2] = digit;
              break;
            case 4:
              set_time[3] = digit;
              break;
            case 6:
              set_time[4] = digit;
              break;
            case 7:
              set_time[5] = digit;
              break;
        }//switch
        digit = 0; //reset digit value
        cbtn_state = LOW;
        delay(150);
        column_pos ++;
        lcd.setCursor(column_pos,line_pos);
        if (column_pos > 7)
        {
          set_sys_time(set_time);
          for (int count = 0;count <= 5;count++)
          {
            set_time[count] = 0;
          }
          column_pos = 0;
          state = 0; //return to original state
          lcd.clear();
          lcd.noCursor();
          delay(150);

        }
        break;
      case 3: // 2.Set Schedule menu
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("2. Set");
        lcd.setCursor(0,1);
        lcd.print ("Schedule");
        while (cbtn_state == LOW) //wait for cbrn to be pressed
        {
          cbtn_state = digitalRead(cbtn);
          lbtn_state = digitalRead(lbtn);
          rbtn_state = digitalRead(rbtn);
          if (lbtn_state == HIGH)
          {
            state = 1;
            delay(150);
            lbtn_state = LOW;            
            cbtn_state = HIGH; // act as button pressed
          }
          if (rbtn_state == HIGH)
          {
            state = 6;
            delay(150);
            rbtn_state = LOW;            
            cbtn_state = HIGH; // act as button pressed
          }
        }//cbtn has been pressed
        delay(150);
        cbtn_state = LOW;
        if ((state != 1) && (state != 6))
        {
          state = 4; //change to set-schedule sub_menu
        }
        break;
      case 4: //set-schedule sub_menu (on Time)
        lcd.clear();
        lcd.setCursor(0,0);
        set_time_string.concat(set_time[0]);
        set_time_string.concat(set_time[1]);
        set_time_string.concat(":");
        set_time_string.concat(set_time[2]);
        set_time_string.concat(set_time[3]);
        set_time_string.concat(":");
        set_time_string.concat(set_time[4]);
        set_time_string.concat(set_time[5]);
        //lcd.print("  :  :  ");      
        lcd.print (set_time_string);
        set_time_string = "";
        lcd.setCursor(column_pos,line_pos);
        lcd.cursor();
        while(cbtn_state == LOW)
        {
          cbtn_state = digitalRead(cbtn);
          lbtn_state = digitalRead(lbtn);
          rbtn_state = digitalRead(rbtn);
          if (digit > 9) 
          {
            digit = 0;
          }
          if (rbtn_state == HIGH)
          {
            digit ++;
            display_digit(digit);
            rbtn_state = LOW;
            delay(150);  
          }
          if (lbtn_state == HIGH)
          {
            digit --;
            display_digit(digit);
            lbtn_state = LOW;
            delay(150);  
          }
        }//cbtn has been pressed
        switch (column_pos) {
            case 0:
              set_time[0] = digit;
              break;
            case 1:
              set_time[1] = digit;
              break;
            case 3:
              set_time[2] = digit;
              break;
            case 4:
              set_time[3] = digit;
              break;
            case 6:
              set_time[4] = digit;
              break;
            case 7:
              set_time[5] = digit;
              break;
        }//switch
        digit = 0; //reset digit value
        cbtn_state = LOW;
        delay(150);
        column_pos ++;
        lcd.setCursor(column_pos,line_pos);
        if (column_pos > 7)
        {
          store_schedule(set_time,true);

          for (int count = 0;count <= 5;count++)
          {
            set_time[count] = 0;
          }
          column_pos = 0;
          state = 5; //change to off Time subsub menu
          lcd.clear();
          lcd.noCursor();
          delay(150);

        }
        break;
      case 5: //set-schedule sub_menu (off Time)
        lcd.clear();
        lcd.setCursor(0,1);
        set_time_string.concat(set_time[0]);
        set_time_string.concat(set_time[1]);
        set_time_string.concat(":");
        set_time_string.concat(set_time[2]);
        set_time_string.concat(set_time[3]);
        set_time_string.concat(":");
        set_time_string.concat(set_time[4]);
        set_time_string.concat(set_time[5]);
        //lcd.print("  :  :  ");      
        lcd.print (set_time_string);
        set_time_string = "";
        line_pos = 1;
        lcd.setCursor(column_pos,line_pos);
        lcd.cursor();
        while(cbtn_state == LOW)
        {
          cbtn_state = digitalRead(cbtn);
          lbtn_state = digitalRead(lbtn);
          rbtn_state = digitalRead(rbtn);
          if (digit > 9) 
          {
            digit = 0;
          }
          if (rbtn_state == HIGH)
          {
            digit ++;
            display_digit(digit);
            rbtn_state = LOW;
            delay(150);  
          }
          if (lbtn_state == HIGH)
          {
            digit --;
            display_digit(digit);
            lbtn_state = LOW;
            delay(150);  
          }
        }//cbtn has been pressed
        switch (column_pos) {
            case 0:
              set_time[0] = digit;
              break;
            case 1:
              set_time[1] = digit;
              break;
            case 3:
              set_time[2] = digit;
              break;
            case 4:
              set_time[3] = digit;
              break;
            case 6:
              set_time[4] = digit;
              break;
            case 7:
              set_time[5] = digit;
              break;
        }//switch
        digit = 0; //reset digit value
        cbtn_state = LOW;
        delay(150);
        column_pos ++;
        lcd.setCursor(column_pos,line_pos);
        if (column_pos > 7)
        {
          store_schedule(set_time,false);

          for (int count = 0;count <= 5;count++)
          {
            set_time[count] = 0;
          }
          column_pos = 0;
          line_pos = 0;// also reset line_pos due to off time subsubmenu
          state = 0; //return to original state
          /*
          for (int x = 0; x<12;x++)
          {
            Serial.print(EEPROM.read(x));  
          }
          */
          lcd.clear();
          lcd.noCursor();
          delay(150);

        }
        break;  
      case 6: // 3.Set PWM menu
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("3. Set");
        lcd.setCursor(0,1);
        lcd.print ("PWM");
        while (cbtn_state == LOW) //wait for cbrn to be pressed
        {
          cbtn_state = digitalRead(cbtn); //update btn states
          rbtn_state = digitalRead(rbtn);
          lbtn_state = digitalRead(lbtn);
          if(rbtn_state == HIGH)
          {
            //digitalWrite(ledPin, HIGH);
            state = 8;
            delay(150);
            rbtn_state = LOW;            
            cbtn_state = HIGH; // act as button pressed
          } //rbtn pressed
        }//cbtn has been pressed
        cbtn_state = LOW;
        lcd.clear();
        delay(150);
        if (state == 6)
        {
          state = 7; // should be 7 for PWM submenu
        }
        break;
       case 7: // Set PWM sub-menu
        lcd.setCursor(0,0);
        set_PWM_string.concat(set_PWM[0]);
        set_PWM_string.concat(set_PWM[1]);
        set_PWM_string.concat(set_PWM[2]);
        set_PWM_string.concat("%");
       
        //lcd.print("  :  :  ");      
        lcd.print (set_PWM_string);
        set_PWM_string = "";
        lcd.setCursor(column_pos,line_pos);
        lcd.cursor();
        while(cbtn_state == LOW)
        {
          cbtn_state = digitalRead(cbtn);
          lbtn_state = digitalRead(lbtn);
          rbtn_state = digitalRead(rbtn);
          if (digit > 9) 
          {
            digit = 0;
          }
          if (rbtn_state == HIGH)
          {
            digit ++;
            display_digit(digit);
            rbtn_state = LOW;
            delay(150);  
          }
          if (lbtn_state == HIGH)
          {
            digit --;
            display_digit(digit);
            lbtn_state = LOW;
            delay(150);  
          }
        }//cbtn has been pressed
        switch (column_pos) {
            case 0:
              set_PWM[0] = digit;
              break;
            case 1:
              set_PWM[1] = digit;
              break;
            case 2:
              set_PWM[2] = digit;
              break;
        }//switch
        digit = 0; //reset digit value
        cbtn_state = LOW;
        delay(150);
        column_pos ++;
        lcd.setCursor(column_pos,line_pos);
        if (column_pos > 2)
        {
          currentPWM = set_led_pwm(set_PWM);      
          for (int count = 0;count <= 2;count++)
          {
            set_PWM[count] = 0;
          }
          column_pos = 0;
          state = 0; //return to original state
          lcd.clear();
          lcd.noCursor();
          delay(150);
        }
        break;
      case 8: // 4. Go Back
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("4. Go ");
        lcd.setCursor(0,1);
        lcd.print ("Back");
        while (cbtn_state == LOW) //wait for cbrn to be pressed
        {
          cbtn_state = digitalRead(cbtn);
          lbtn_state = digitalRead(lbtn);
          if (lbtn_state == HIGH)
          {
            state = 6;
            delay(150);
            lbtn_state = LOW;            
            cbtn_state = HIGH; // act as button pressed
          }
        }//cbtn has been pressed
        delay(150);
        cbtn_state = LOW;
        if (state != 6)
        {
          state = 0; //change to base state
        }
        break;  
      default:
        // do something
        int a = 0;
  }
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  //lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  //lcd.print(millis()/1000);
  if((millis()-check_schedule_delay) > 30000)
  {
    check_schedule();
    check_schedule_delay = millis();  
  }
  
}// void loop()

void display_time()
{
  lcd.clear();
  lcd.setCursor(0,1);
  sys_time = "";
  sys_time.concat(hour());
  sys_time.concat(":");
  sys_time.concat(minute());
  sys_time.concat(":");
  sys_time.concat(second());
  lcd.print(sys_time);
}

void set_sys_time (int time_array[6])
{
  int hrs = 0;
  int mns = 0;
  int sec = 0;
  if(time_array[0] <= 2)
  {
    hrs = 10*time_array[0];  
    hrs = hrs + time_array[1];  
  }
  if ((time_array[2] <= 5) && (time_array[4] <= 5))
  {
    mns = 10*time_array[2];
    mns = mns + time_array[3];
    sec = 10*time_array[4];
    sec = sec + time_array[5];
  }
  
  setTime(hrs,mns,sec,1,1,2014); //hr,min,sec,day,month,yr  
  
}
int set_led_pwm(int duty_array[3])
{
  int totalPerc = 0;
  for (int x = 0; x < sizeof(duty_array); x++)
  {
    switch(x){
      case 0:
        totalPerc = totalPerc + 100*duty_array[x];
        break;        
      case 1:
         totalPerc = totalPerc + 10*duty_array[x];
         break;
      case 2:
        totalPerc = totalPerc + duty_array[x];
        break;
    }
  }
  return totalPerc;
}

void display_digit(int digit)
{
  switch (digit) {
      case 1:
        lcd.print("1");
        lcd.setCursor(column_pos,line_pos);
        break;
      case 2:
        // do something
        lcd.print("2");
        lcd.setCursor(column_pos,line_pos);
        break;
      case 3:
        lcd.print("3");
        lcd.setCursor(column_pos,line_pos);
        break;
      case 4:
        lcd.print("4");
        lcd.setCursor(column_pos,line_pos);
        break;
      case 5:
        lcd.print("5");
        lcd.setCursor(column_pos,line_pos);
        break;
      case 6:
        lcd.print("6");
        lcd.setCursor(column_pos,line_pos);
        break;
      case 7:
        lcd.print("7");
        lcd.setCursor(column_pos,line_pos);
        break;
      case 8:
        lcd.print("8");
        lcd.setCursor(column_pos,line_pos);
        break;
      case 9:
        lcd.print("9");
        lcd.setCursor(column_pos,line_pos);
        break;
      default:
        lcd.print("0");
        lcd.setCursor(column_pos,line_pos);
  }
}

bool debounce_btn (int btn,int btn_state,int last_btn_state,long last_debounce_time,int btn_delay)
{
  int reading = digitalRead(btn);
  if (reading != last_btn_state)
  {
    last_debounce_time = millis();
  }
  if ((millis() - last_debounce_time) > btn_delay) //btn has finished occilating
  {
    if ((reading == HIGH) && (last_btn_state == HIGH)) //btn has been pressed
    {
      return true;
      ledState = !ledState;
    }
    else
    {
      return false;
    }
    
  }
  digitalWrite(ledPin, ledState);
  last_btn_state = reading;
}

void store_schedule (int time_array[6],bool on_time) // time and day of week
{
  if (on_time == true)
  {
    for (int add_range = 0; add_range < 6;  add_range ++)
    {
      EEPROM.write(add_range,time_array[add_range]);    
    }  
  }
  else  // off_time
  {
    for (int add_range = 6; add_range < 12;  add_range ++)
    {
      EEPROM.write(add_range,time_array[(add_range-6)]);    
    }  
  }
}//store_schedule

void check_schedule ()
{
  int scd_hour = (EEPROM.read(0) * 10) + EEPROM.read(1);
  int scd_min = (EEPROM.read(2) * 10) + EEPROM.read(3);
  int scd_hour_off = (EEPROM.read(6) * 10) + EEPROM.read(7);
  int scd_min_off = (EEPROM.read(8) * 10) + EEPROM.read(9);
  /*
  Serial.print("scd_hour : ");
  Serial.print(scd_hour);
  Serial.println();
  Serial.print("scd_min : ");
  Serial.print(scd_min);
  Serial.println();
  Serial.print("hour : ");
  Serial.print(hour());
  Serial.println();
  Serial.print("mins : ");
  Serial.print(minute());
  Serial.println();
  */
  if ((scd_hour == hour()) && (scd_min == minute())) // "on" scheduled time has been reached
  {
    leds_state = HIGH;
    //Serial.println("leds ON");
  }
  if ((leds_state == HIGH) && (scd_hour_off == hour()) && (scd_min_off == minute())) // "off" scheduled time has been reached and leds where allready "on"
  {
    leds_state = LOW;
    //Serial.println("leds OFF");
  }
//  digitalWrite(leds_pin, leds_state);
  if(leds_state == LOW){
      OCR1B = 0;
      digitalWrite(leds_pin, LOW);
  }
  else{
   digitalWrite(leds_pin, HIGH);
    OCR1B = currentPWM;
  }
 Serial.print(currentPWM);
}//check schedule
