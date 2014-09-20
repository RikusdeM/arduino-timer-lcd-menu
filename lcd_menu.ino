// lcd_menu.ino
// include the library code:
#include <LiquidCrystal.h>
#include <Time.h>
#include <String.h>
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
String sys_time = "";
const int lbtn = 8;
const int rbtn = 9;
const int cbtn = 6;
const int ubtn = 7;
const int dbtn = 10;
const int ledPin =  13;      // the number of the LED pin
int ledState = LOW;
int ubtn_state = 0;
int dbtn_state = 0;
int lbtn_state = 0;
int rbtn_state = 0;
int cbtn_state = 0;
int last_cbtn_state = 0;
int state = 0;

byte column_pos = 0;
byte line_pos = 0;
byte digit = 0;
int set_time[6] = {0,0,0,0,0,0};
String set_time_string = "";

void setup() {
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  Serial.begin(9600);
  // Print a message to the LCD.
  setTime(16,20,0,1,1,2014); //hr,min,sec,day,month,yr	
  pinMode(lbtn, INPUT);
  pinMode(rbtn, INPUT);
  pinMode(cbtn, INPUT);
  pinMode(ubtn,INPUT);
  pinMode(dbtn,INPUT);
  pinMode(ledPin,OUTPUT);
}

void loop() 
{
  if (state == 0)
  {
    display_time();
    cbtn_state = digitalRead(cbtn);
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
          if (lbtn_state == HIGH)
          {
            state = 1;
            delay(150);
            lbtn_state = LOW;            
            cbtn_state = HIGH; // act as button pressed
          }
        }//cbtn has been pressed
        delay(150);
        cbtn_state = LOW;
        if (state != 1)
        {
          state = 0; //change to set-schedule sub_menu
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
      if (reading != btn_state) //btn is still pressed after 50 ms
      {
        btn_state = reading;

        if (btn_state == HIGH) //btn has been pressed
        {
          return true;
          ledState = !ledState;
        }
        else
        {
          return false;
        }
      }
    }
    digitalWrite(ledPin, ledState);
    last_btn_state = reading;
  }
