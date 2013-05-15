
// Pins 
// ------------------------------------------------------------------------------------

const int PIN_BUTTON_UP         = A2 ; 
const int PIN_BUTTON_RIGHT      = A1 ; 
const int PIN_BUTTON_DOWN       = A0 ; 
const int PIN_BUTTON_LEFT       = A3 ; 
const int PIN_BUTTON_FIRE       = A4 ;

const int PIN_LIMIT_CLAW_MAX    =  2 ; 
const int PIN_LIMIT_CLAW_MIN    =  3 ; 

const int PIN_LIMIT_NORTH       = 4 ; 
const int PIN_LIMIT_EAST        = 5 ; 
const int PIN_LIMIT_SOUTH       = 6 ; 
const int PIN_LIMIT_WEST        = 7 ; 

const int PIN_MOTOR_CLAW_IN1    = 8;
const int PIN_MOTOR_CLAW_IN2    = 9;

const int PIN_MOTOR_ONE_IN1    = 10;
const int PIN_MOTOR_ONE_IN2    = 11;

const int PIN_MOTOR_TWO_IN1    = 12;
const int PIN_MOTOR_TWO_IN2    = 13;



// Constants 
// ------------------------------------------------------------------------------------
const int MIN_BUTTON_VALUE   = 500; 

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

class CAButton
{
  private:
    int m_pin; 
    
  public:
    void Init( int pin, int type=INPUT ) {
      this->m_pin   = pin; 
      pinMode(this->m_pin, INPUT);
    }
    
    bool GetValue() {
      return ( analogRead( this->m_pin ) > MIN_BUTTON_VALUE ) ;
    }    
};

class CDButton
{
  private:
    int m_pin; 
    
  public:
    void Init( int pin, int type=INPUT ) {
      this->m_pin   = pin; 
      pinMode(this->m_pin, INPUT);
    }
    
    bool GetValue() {
      return digitalRead( this->m_pin ) ;
    }
    
};



// ------------------------------------------------------------------------------------

class CClaw
{
  private:
    CAButton m_buttonMoveUp;
    CAButton m_buttonMoveRight;
    CAButton m_buttonMoveDown;
    CAButton m_buttonMoveLeft;
    CAButton m_buttonMoveFire;
    
    CDButton m_buttonLimitClawMax;
    CDButton m_buttonLimitClawMin;
    CDButton m_buttonLimitNorth;
    CDButton m_buttonLimitEast;
    CDButton m_buttonLimitSouth;
    CDButton m_buttonLimitWest;
    
    CMotor m_motorCable; 
    CMotor m_motorClaw; 
    CMotor m_motorOne; 
    CMotor m_motorTwo; 

    void ActionMoveUp(); 
    void ActionMoveDown(); 
    void ActionMoveRight(); 
    void ActionMoveLeft(); 
    void ActionFire(); 
    
  public:
    void Init( ); 
    void Loop();    
};

void CClaw::Init( ) {
  
  // Joystick 
  this->m_buttonMoveUp.Init    (PIN_BUTTON_UP    );
  this->m_buttonMoveRight.Init (PIN_BUTTON_RIGHT );
  this->m_buttonMoveDown.Init  (PIN_BUTTON_DOWN  );
  this->m_buttonMoveLeft.Init  (PIN_BUTTON_LEFT  );
  this->m_buttonMoveFire.Init  (PIN_BUTTON_FIRE  );
  
  // Cube limit switches 
  this->m_buttonLimitNorth.Init (PIN_LIMIT_NORTH );
  this->m_buttonLimitEast.Init  (PIN_LIMIT_EAST );
  this->m_buttonLimitSouth.Init (PIN_LIMIT_SOUTH );
  this->m_buttonLimitWest.Init  (PIN_LIMIT_WEST );
  
  // Claw 
  this->m_buttonLimitClawMax.Init( PIN_LIMIT_CLAW_MAX );
  this->m_buttonLimitClawMin.Init( PIN_LIMIT_CLAW_MIN );
  
  // Motors 
  this->m_motorClaw.Init  ( PIN_MOTOR_CLAW_IN1, PIN_MOTOR_CLAW_IN2 ); 
  this->m_motorCable.Init ( PIN_MOTOR_CLAW_IN1, PIN_MOTOR_CLAW_IN2 ); 
  this->m_motorOne.Init   ( PIN_MOTOR_ONE_IN1,  PIN_MOTOR_ONE_IN2 ); 
  this->m_motorTwo.Init   ( PIN_MOTOR_TWO_IN1,  PIN_MOTOR_TWO_IN2 ); 
  
}


void CClaw::Loop() {

    // Start the fire 
    // Once the player fires the craw the system is automatic afterwards. 
    if( this->m_buttonMoveFire.GetValue() ) {
      this->ActionFire();
      return; 
    }
    
    // Limit switches 
    if( this->m_buttonLimitNorth.GetValue() ) {
      Serial.println( "Limit North " ); 
    } else if( this->m_buttonLimitSouth.GetValue() ) {
      Serial.println( "Limit South " ); 
    } else if( this->m_buttonLimitEast.GetValue() ) {
      Serial.println( "Limit East " ); 
    } else if( this->m_buttonLimitWest.GetValue() ) {
      Serial.println( "Limit West " ); 
    }

    // Move the motors
    if( this->m_buttonMoveUp.GetValue() && ! this->m_buttonLimitNorth.GetValue() ) {
      this->ActionMoveUp();       
    } else if( this->m_buttonMoveDown.GetValue() && ! this->m_buttonLimitSouth.GetValue() ) {
      this->ActionMoveDown();       
    } else if( this->m_buttonMoveRight.GetValue() && ! this->m_buttonLimitEast.GetValue() ) {
      this->ActionMoveRight();       
    } else if( this->m_buttonMoveLeft.GetValue() && ! this->m_buttonLimitWest.GetValue() ) {
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
void CClaw::ActionFire() {
  // Reset the up button values 
  Serial.println( "Action Fire " );   
  
  
  // Drop the claw in to the 
  Serial.println( "Drop the claw" ); 
  for( int i = 0 ; i < 10 ; i++ ) {
    this->m_motorCable.Forward();
  }

  // Close the claw. until the limit switch has been research. 
  Serial.println( "Close the claw." ); 
  while( ! this->m_buttonLimitClawMin.GetValue() ) { 
    this->m_motorClaw.Forward();
  }
  
  // Rise the claw   
  Serial.println( "Rise the claw." );   
  for( int i = 0 ; i < 10 ; i++ ) {
    this->m_motorCable.Reverse();
  }
  
  // Move the claw back to the starting pos. 
  Serial.println( "Move the claw to the right." );  
  while( this->m_buttonLimitEast.GetValue() ) {
    this->ActionMoveRight();  
  }
  Serial.println( "Move the claw to the south" );  
  while( this->m_buttonLimitSouth.GetValue() ) {
    this->ActionMoveDown();
  }

  // Drop the claw
  Serial.println( "Open the claw" );  
  while( ! this->m_buttonLimitClawMax.GetValue() ) { 
    this->m_motorClaw.Reverse();
  }
  
  Serial.println( "All done." );  
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
