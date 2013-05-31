// Pins 
// ------------------------------------------------------------------------------------

const int PIN_BUTTON_UP         = A2 ; 
const int PIN_BUTTON_RIGHT      = A1 ; 
const int PIN_BUTTON_DOWN       = A0 ; 
const int PIN_BUTTON_LEFT       = A3 ; 
const int PIN_BUTTON_A          = A4 ;
const int PIN_BUTTON_B          = A5 ;

const int PIN_LIMIT_CLAW_MAX    =  5 ; 
const int PIN_LIMIT_CLAW_MIN    =  4 ; 

const int PIN_MOTOR_CLAW_TEETH_IN1  =  6;
const int PIN_MOTOR_CLAW_TEETH_IN2  =  7;
const int PIN_MOTOR_CLAW_CABLE_IN1  =  8;
const int PIN_MOTOR_CLAW_CABLE_IN2  =  9;
const int PIN_MOTOR_TWO_IN1         = 10;
const int PIN_MOTOR_TWO_IN2         = 11;
const int PIN_MOTOR_ONE_IN1         = 12;
const int PIN_MOTOR_ONE_IN2         = 13;


// Constants 
// ------------------------------------------------------------------------------------
const int  MIN_BUTTON_VALUE     = 1000; 
const long TIME_BUTTON_DEBOUNCE = 100;
const int  MAX_CABLE_LENGTH     = 10; 

// ------------------------------------------------------------------------------------

class CMotor {
  private:
    int m_pin_in1; 
    int m_pin_in2;
    
    unsigned char m_motorSpeed; 
    
  public:
    void Init( int pin_in1, int pin_in2 ) {
      // this->m_pin_speed = pin_speed ; 
      this->m_pin_in1   = pin_in1 ;  
      this->m_pin_in2   = pin_in2 ; 
      
      // Set the pin modes. 
      pinMode(this->m_pin_in1,  OUTPUT);
      pinMode(this->m_pin_in2,  OUTPUT);
      
      this->Stop() ; // Start with the motor stopped. 
    }
    
    void Forward() {
      digitalWrite(this->m_pin_in1,  LOW); // DC motor rotates clockwise
      digitalWrite(this->m_pin_in2,  HIGH);
    }

    void Reverse() {
      digitalWrite(this->m_pin_in1,  HIGH); // DC motor rotates counter-clockwise
      digitalWrite(this->m_pin_in2,  LOW);
    }
    
    void Stop() {
      digitalWrite(this->m_pin_in1,  LOW); // DC motor rotates counter-clockwise
      digitalWrite(this->m_pin_in2,  LOW);    
    }
    
}; 

// ------------------------------------------------------------------------------------



class CButton
{
  private:
    int m_pin; 
    boolean m_value ; 
    boolean m_lastButtonState ; 
    long m_lastDebounceTime ; 
    
  public:
    void Init( int pin, int type=INPUT ) {      
      this->m_lastDebounceTime  = 0 ; 
      this->m_lastButtonState   = false ;  
      this->m_value             = false ; 
      
      this->m_pin = pin; 
      pinMode(this->m_pin, INPUT);
    }
    int GetPin() {
      return this->m_pin ; 
    }
    int ReadValue() {
      return digitalRead( this->GetPin() ); 
    }
    
    boolean GetValue() {
      int reading   = this->ReadValue() ; 
      long now      = (long)millis();    // get current time
    
      if( reading != this->m_lastButtonState ) {
        this->m_lastDebounceTime = now ; 
        this->m_lastButtonState  = reading ; 
      }
      
      // debounce the button (Check if a stable, changed state has occured)
      if (now - this->m_lastDebounceTime > TIME_BUTTON_DEBOUNCE  ) {
        this->m_value = reading ;         
      }
      
      /*
      Serial.print( "P[" );
      Serial.print( this->GetPin() );
      Serial.print( "] reading [" );
      Serial.print( reading ); 
      Serial.print( "] value [" );
      Serial.print( m_value );      
      Serial.print( "] lastState [" );
      Serial.print( m_lastButtonState );  
      Serial.print( "] Delta [" );
      Serial.print( now - this->m_lastDebounceTime );  
      Serial.println( "]" );
      */
      
      return this->m_value; 
    }    
}; 


// ------------------------------------------------------------------------------------

class CClaw
{
  private:
  
    // Joystick. 
    CButton m_buttonMoveUp;
    CButton m_buttonMoveRight;
    CButton m_buttonMoveDown;
    CButton m_buttonMoveLeft;
    
    // Buttons 
    CButton m_buttonA;
    CButton m_buttonB;
    
    // Limit switches 
    CButton m_buttonLimitClawMax;
    CButton m_buttonLimitClawMin;
    
    // Motors 
    CMotor m_motorClawTeeth; 
    CMotor m_motorClawCable; 
    CMotor m_motorOne; 
    CMotor m_motorTwo; 

    // Claw movment. 
    void ActionMoveUp(); 
    void ActionMoveDown(); 
    void ActionMoveRight(); 
    void ActionMoveLeft(); 
    
    // Claw Action 
    void ActionClawOpen();
    void ActionClawClose();
    void ActionClawRise();
    void ActionClawDrop();
    
    boolean m_bClawCableState ; 
    boolean m_bClawTeethState;
    
  public:
    void Init( ); 
    void Loop();
    
    void DelayWithUpdate( int time ) ; 
};

void CClaw::Init( ) {
     
  // Joystick 
  this->m_buttonMoveUp.Init    (PIN_BUTTON_UP    );
  this->m_buttonMoveRight.Init (PIN_BUTTON_RIGHT );
  this->m_buttonMoveDown.Init  (PIN_BUTTON_DOWN  );
  this->m_buttonMoveLeft.Init  (PIN_BUTTON_LEFT  );
  
  // Buttons 
  this->m_buttonA.Init         (PIN_BUTTON_A     );
  this->m_buttonB.Init         (PIN_BUTTON_B     );

  // Claw limit switches. 
  this->m_buttonLimitClawMax.Init( PIN_LIMIT_CLAW_MAX );
  this->m_buttonLimitClawMin.Init( PIN_LIMIT_CLAW_MIN );
  
  // Motors 
  this->m_motorClawCable.Init ( PIN_MOTOR_CLAW_CABLE_IN1,  PIN_MOTOR_CLAW_CABLE_IN2 ); 
  this->m_motorClawTeeth.Init ( PIN_MOTOR_CLAW_TEETH_IN1,  PIN_MOTOR_CLAW_TEETH_IN2 ); 
  this->m_motorOne.Init       ( PIN_MOTOR_ONE_IN1,         PIN_MOTOR_ONE_IN2 ); 
  this->m_motorTwo.Init       ( PIN_MOTOR_TWO_IN1,         PIN_MOTOR_TWO_IN2 ); 
  
  this->m_bClawCableState = false; 
  this->m_bClawTeethState = false;
}

void CClaw::DelayWithUpdate( int time ) {
  while(time > 0 ) {
    delay(2);
    time -= 2 ;
    
    // Joystick 
    this->m_buttonMoveUp.GetValue();
    this->m_buttonMoveRight.GetValue();
    this->m_buttonMoveDown.GetValue();
    this->m_buttonMoveLeft.GetValue();
    
    // Buttons 
    this->m_buttonA.GetValue();
    this->m_buttonB.GetValue();
  
    // Claw limit switches. 
    this->m_buttonLimitClawMax.GetValue();
    this->m_buttonLimitClawMin.GetValue(); 
  }
}

void CClaw::Loop() {
   
    // Cable 
    if( this->m_buttonA.GetValue() ) {
        Serial.println( "m_buttonA" );
        if( this->m_bClawCableState ) {      
            this->ActionClawOpen(); 
            this->m_bClawCableState = !this->m_bClawCableState ; 
        } else {      
            this->ActionClawClose();       
            this->m_bClawCableState = !this->m_bClawCableState ; 
        }     
            
        DelayWithUpdate(1000); 
    }  
    
    // Teeth 
    if( this->m_buttonB.GetValue() ) {
        Serial.println( "m_buttonB" );
        if( this->m_bClawTeethState ) {      
            this->ActionClawDrop();
            this->m_bClawTeethState = !this->m_bClawTeethState ;
        } else {      
            this->ActionClawRise();
            this->m_bClawTeethState = !this->m_bClawTeethState ;
        }        
        DelayWithUpdate(1000); 
    }  
      
    if( this->m_buttonLimitClawMax.GetValue() ) {
        Serial.println( "m_buttonLimitClawMax " );
    }
    if( this->m_buttonLimitClawMin.GetValue() ) {
        Serial.println( "m_buttonLimitClawMin " );
    }  
    

    // Move the motors
    if( this->m_buttonMoveUp.GetValue() ) {
        this->ActionMoveUp();       
    } else if( this->m_buttonMoveDown.GetValue()  ) {
        this->ActionMoveDown();       
    } else if( this->m_buttonMoveRight.GetValue() ) {
        this->ActionMoveRight();       
    } else if( this->m_buttonMoveLeft.GetValue() ) {
        this->ActionMoveLeft();       
    } else {
        this->m_motorOne.Stop();
        this->m_motorTwo.Stop();
    }
}

/**
 *       | Motor1 | Motor2 | 
 * -------------------------
 * Right |   +    |   +    |
 * Left  |   -    |   -    |
 * Up    |   +    |   -    |
 * Down  |   -    |   +    |
 */

void CClaw::ActionMoveUp() {
    // Reset the up button values 
    Serial.println( "Action Up " );   
    this->m_motorOne.Forward();  
    this->m_motorTwo.Reverse();
}
void CClaw::ActionMoveDown() {
    // Reset the up button values 
    Serial.println( "Action Down " );  
    this->m_motorOne.Reverse();
    this->m_motorTwo.Forward();
}
void CClaw::ActionMoveRight() {
    // Reset the up button values 
    Serial.println( "Action Right " ); 
    this->m_motorOne.Forward(); 
    this->m_motorTwo.Forward();   
}
void CClaw::ActionMoveLeft() {
    // Reset the up button values 
    Serial.println( "Action Left " ); 
    this->m_motorOne.Reverse();
    this->m_motorTwo.Reverse();
}



void CClaw::ActionClawOpen() {
    while( ! this->m_buttonLimitClawMax.GetValue() ) {
        this->m_motorClawTeeth.Forward();
        Serial.println( "ActionClawOpen" );   
    }
    this->m_motorClawTeeth.Stop();
    Serial.println( "ActionClawOpen = done" );
}
void CClaw::ActionClawClose(){
    while( ! this->m_buttonLimitClawMin.GetValue() ) {
        this->m_motorClawTeeth.Reverse();
        Serial.println( "ActionClawClose" );
    }
    this->m_motorClawTeeth.Stop();
    Serial.println( "ActionClawClose = done" );
}

void CClaw::ActionClawRise() {  
    for( int i = 0 ; i < MAX_CABLE_LENGTH ; i++ ) {
        this->m_motorClawCable.Forward();
        Serial.println( "ActionClawRise" );
        DelayWithUpdate(500); 
    }  
    this->m_motorClawCable.Stop();
    Serial.println( "ActionClawRise = done" );
}

void CClaw::ActionClawDrop() {
    for( int i = 0 ; i < MAX_CABLE_LENGTH ; i++ ) {
        this->m_motorClawCable.Reverse();
        Serial.println( "ActionClawDrop" );
        DelayWithUpdate(500); 
    }
    this->m_motorClawCable.Stop();
    Serial.println( "ActionClawDrop = done" );
}



// ------------------------------------------------------------------------------------
CClaw gClaw; 
void setup() {
    // initialize serial communications at 9600 bps:
    Serial.begin(9600);   
    gClaw.Init( );   
}

void loop() { 
    gClaw.Loop(); 
    
    // wait 2 milliseconds before the next loop
    // for the analog-to-digital converter to settle
    // after the last reading:
    gClaw.DelayWithUpdate(2);                     
}

