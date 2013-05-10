
// ------------------------------------------------------------------------------------

const int MAX_BUTTON_COUNT   = 5;
const int MIN_BUTTON_VALUE   = 1024*3; 

const int BUTTON_UP          = 0; 
const int BUTTON_RIGHT       = 1; 
const int BUTTON_DOWN        = 2; 
const int BUTTON_LEFT        = 3; 
const int BUTTON_FIRE        = 4; 


class CClaw
{
  private:
    int m_buttonPins  [ MAX_BUTTON_COUNT ]; 
    int m_buttonValue [ MAX_BUTTON_COUNT ];
    
    void ReadInput(); 
    void ActionMoveUp(); 
    void ActionMoveDown(); 
    void ActionMoveRight(); 
    void ActionMoveLeft(); 
    void ActionFire(); 
    
  public:
    void Init( int up, int right, int down, int left, int fire ); 
    void Loop();    
};

void CClaw::Init( int up, int right, int down, int left, int fire) {
  
  // Recored the pins used for each button 
  this->m_buttonPins[BUTTON_UP]     = up ; 
  this->m_buttonPins[BUTTON_RIGHT]  = right ; 
  this->m_buttonPins[BUTTON_DOWN]   = down ; 
  this->m_buttonPins[BUTTON_LEFT]   = left ; 
  this->m_buttonPins[BUTTON_FIRE]   = fire ; 
  
  // Set the pins as inputs 
  for( int offset = 0 ; offset < MAX_BUTTON_COUNT ; offset++ ) {
    pinMode(this->m_buttonPins[ offset ], INPUT); 
  }
}

void CClaw::ReadInput() {
  for( int offset = 0 ; offset < MAX_BUTTON_COUNT ; offset++ ) {
    m_buttonValue[ offset ] += analogRead( this->m_buttonPins[ offset ] ) ;
  }
}

void CClaw::Loop() {
    this->ReadInput(); 

    // Start the fire 
    // Once the player fires the cran the system is automatic afterwards. 
    if( m_buttonValue[ BUTTON_FIRE ] > MIN_BUTTON_VALUE ) {
      this->ActionFire();
      return; 
    }


    // Move the motors
    if( m_buttonValue[ BUTTON_UP ] > MIN_BUTTON_VALUE ) {
      this->ActionMoveUp();       
    } else if( m_buttonValue[ BUTTON_DOWN ] > MIN_BUTTON_VALUE ) {
      this->ActionMoveDown();       
    }

    // Right left 
    if( m_buttonValue[ BUTTON_RIGHT ] > MIN_BUTTON_VALUE ) {
      this->ActionMoveRight();       
    } else if( m_buttonValue[ BUTTON_LEFT ] > MIN_BUTTON_VALUE ) {
      this->ActionMoveLeft();       
    }
    
    
    // Blead the button values 
    // There is some leakages in to the button values. 
    // To reduce false button presses we blead the values every loops.
    for( int offset = 0 ; offset < MAX_BUTTON_COUNT ; offset++ ) {
      if( m_buttonValue[ offset ] > 0 ) {
        m_buttonValue[ offset ]-- ; 
      }
    }
  
}

void CClaw::ActionMoveUp() {
  // Reset the up button values 
  this->m_buttonValue[ BUTTON_UP ] = 0 ;
  Serial.print( "Up " );   
}
void CClaw::ActionMoveDown() {
  // Reset the up button values 
  this->m_buttonValue[ BUTTON_DOWN ] = 0 ;
  Serial.print( "Down " );   
}
void CClaw::ActionMoveRight() {
  // Reset the up button values 
  this->m_buttonValue[ BUTTON_RIGHT ] = 0 ;
  Serial.print( "Right " );   
}
void CClaw::ActionMoveLeft() {
  // Reset the up button values 
  this->m_buttonValue[ BUTTON_LEFT ] = 0 ;
  Serial.print( "Left " );   
}
void CClaw::ActionFire() {
  // Reset the up button values 
  // this->m_buttonValue[ BUTTON_FIRE ] = 0 ;
  Serial.print( "Fire " );   
}


CClaw gClaw; 
// ------------------------------------------------------------------------------------




void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  
  gClaw.Init( A1, A0, A3, A2, A4 );   
}

void loop() {
  
  
  gClaw.Loop(); 

  // wait 2 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  delay(2);                     
}
