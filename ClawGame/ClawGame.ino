
// Pins 
// ------------------------------------------------------------------------------------

const int PIN_BUTTON_UP         = A2 ; 
const int PIN_BUTTON_RIGHT      = A1 ; 
const int PIN_BUTTON_DOWN       = A0 ; 
const int PIN_BUTTON_LEFT       = A3 ; 
const int PIN_BUTTON_FIRE       = A4 ;

const int PIN_LIMIT_CLAW_MAX    =  2 ; 
const int PIN_LIMIT_CLAW_MIN    =  3 ; 
const int PIN_LIMIT_AXIS_X      =  4 ; 
const int PIN_LIMIT_AXIS_Y      =  4 ; // ?

const int PIN_MOTOR_CLAW_SPEED  =  5;
const int PIN_MOTOR_CLAW_IN1    =  6;
const int PIN_MOTOR_CLAW_IN2    =  7;

const int PIN_MOTOR_ONE_SPEED  =  8;
const int PIN_MOTOR_ONE_IN1    =  9;
const int PIN_MOTOR_ONE_IN2    = 10;

const int PIN_MOTOR_TWO_SPEED  = 11;
const int PIN_MOTOR_TWO_IN1    = 12;
const int PIN_MOTOR_TWO_IN2    = 13;



// Constants 
// ------------------------------------------------------------------------------------
const int MAX_BUTTON_COUNT   = 20 ;
const int MIN_BUTTON_VALUE   = 1024*1; 


// ------------------------------------------------------------------------------------

class CMotor {
  private:
    int m_pin_speed; 
    int m_pin_in1; 
    int m_pin_in2;
    
    unsigned char m_motorSpeed; 
    
  public:
    void Init( int pin_speed, int pin_in1, int pin_in2 ) {
      this->m_pin_speed = pin_speed ; 
      this->m_pin_in1   = pin_in1 ;  
      this->m_pin_in2   = pin_in2 ; 
      
      // Set the pin modes. 
      pinMode(this->m_pin_in1,  OUTPUT);
      pinMode(this->m_pin_in2,  OUTPUT);
      pinMode(this->m_pin_speed,OUTPUT);
      
      this->m_motorSpeed = 100; // Full speed ahead 
      this->Stop() ; // Start with the motor stopped. 
    }
    
    unsigned char GetSpeed() const {
      return this->m_motorSpeed ; 
    }
    void SetSpeed( const unsigned char motorSpeed ) {
      this->m_motorSpeed = motorSpeed ; 
    }
    
    void Forward() {
      analogWrite(this->m_pin_speed, this->m_motorSpeed );      
      digitalWrite(this->m_pin_in1,  LOW);// DC motor rotates clockwise
      digitalWrite(this->m_pin_in2,  HIGH);
    }

    void Reverse() {
      analogWrite(this->m_pin_speed, this->m_motorSpeed );      
      digitalWrite(this->m_pin_in1,  HIGH);// DC motor rotates counter-clockwise
      digitalWrite(this->m_pin_in2,  LOW);
    }
    
    void Stop() {
      digitalWrite(this->m_pin_in1,  HIGH);// DC motor rotates counter-clockwise
      digitalWrite(this->m_pin_in2,  HIGH);    
    }
    
}; 

// ------------------------------------------------------------------------------------

class CButton
{
  private:
    int m_pin; 
    int m_value;
  
  public:
    void Init( int pin ) {
      this->m_pin = pin; 
      pinMode(this->m_pin, INPUT);
      this->Reset() ; 
    }
    
    void Update() {
      this->m_value += analogRead( this->m_pin ) ;

      // Blead the button values 
      // There is some leakages in to the button values. 
      // To reduce false button presses we blead the values every loops.      
      this->m_value--; 
    }
    
    void Reset() {
      this->m_value = 0 ; 
    }
    
    int GetValue() {
      return this->m_value ; 
    }
};


// ------------------------------------------------------------------------------------

class CClaw
{
  private:
    CButton m_buttonMoveUp;
    CButton m_buttonMoveRight;
    CButton m_buttonMoveDown;
    CButton m_buttonMoveLeft;
    CButton m_buttonMoveFire;
    
    void ReadInput(); 
    void ActionMoveUp(); 
    void ActionMoveDown(); 
    void ActionMoveRight(); 
    void ActionMoveLeft(); 
    void ActionFire(); 
    
    CMotor m_claw; 
    CMotor m_motorOne; 
    CMotor m_motorTwo; 
    
  public:
    void Init( ); 
    void Loop();    
};

void CClaw::Init( ) {
  
  // Recored the pins used for each button 
  this->m_buttonMoveUp.Init    (PIN_BUTTON_UP    );
  this->m_buttonMoveRight.Init (PIN_BUTTON_RIGHT );
  this->m_buttonMoveDown.Init  (PIN_BUTTON_DOWN  );
  this->m_buttonMoveLeft.Init  (PIN_BUTTON_LEFT  );
  this->m_buttonMoveFire.Init  (PIN_BUTTON_FIRE  );
  
  this->m_claw.Init     (PIN_MOTOR_CLAW_SPEED, PIN_MOTOR_CLAW_IN1, PIN_MOTOR_CLAW_IN2 ); 
  this->m_motorOne.Init (PIN_MOTOR_ONE_SPEED,  PIN_MOTOR_ONE_IN1,  PIN_MOTOR_ONE_IN2 ); 
  this->m_motorTwo.Init (PIN_MOTOR_TWO_SPEED,  PIN_MOTOR_TWO_IN1,  PIN_MOTOR_TWO_IN2 ); 
  
}

void CClaw::ReadInput() {
  
  m_buttonMoveUp.Update();
  m_buttonMoveRight.Update();
  m_buttonMoveDown.Update();
  m_buttonMoveLeft.Update();
  m_buttonMoveFire.Update();
    
}

void CClaw::Loop() {
    this->ReadInput(); 

    // Start the fire 
    // Once the player fires the cran the system is automatic afterwards. 
    if( m_buttonMoveFire.GetValue() > MIN_BUTTON_VALUE ) {
      this->ActionFire();
      return; 
    }


    // Move the motors
    if( m_buttonMoveUp.GetValue() > MIN_BUTTON_VALUE ) {
      this->ActionMoveUp();       
    } else if( m_buttonMoveDown.GetValue() > MIN_BUTTON_VALUE ) {
      this->ActionMoveDown();       
    } else if( m_buttonMoveRight.GetValue() > MIN_BUTTON_VALUE ) {
      this->ActionMoveRight();       
    } else if( m_buttonMoveLeft.GetValue() > MIN_BUTTON_VALUE ) {
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
  this->m_buttonMoveUp.Reset() ;
  Serial.print( "Up " );   
  
  this->m_motorOne.Forward();  
  this->m_motorTwo.Reverse();
}
void CClaw::ActionMoveDown() {
  // Reset the up button values 
  this->m_buttonMoveDown.Reset() ;
  Serial.print( "Down " );   
  
  this->m_motorOne.Reverse();
  this->m_motorTwo.Forward();
}
void CClaw::ActionMoveRight() {
  // Reset the up button values 
  this->m_buttonMoveRight.Reset() ;
  Serial.print( "Right " );   
  
  this->m_motorOne.Forward(); 
  this->m_motorTwo.Forward();   
}
void CClaw::ActionMoveLeft() {
  // Reset the up button values 
  this->m_buttonMoveLeft.Reset() ;
  Serial.print( "Left " );   
  
  this->m_motorOne.Reverse();
  this->m_motorTwo.Reverse();
}
void CClaw::ActionFire() {
  // Reset the up button values 
  m_buttonMoveFire.Reset() ;
  Serial.print( "Fire " );   
  
  this->m_motorOne.Stop();
  this->m_motorTwo.Stop();
  
}


CClaw gClaw; 
// ------------------------------------------------------------------------------------



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
  delay(2);                     
}
