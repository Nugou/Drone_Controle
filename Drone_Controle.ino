#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <RF24.h>
#include <PS2X_lib.h>

/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
boolean radioNumber = false;

boolean Joy = false;

int delay_tr = 80;

int temp_joy[4] = {128,128,128,128};

unsigned long listenDroneTime = 0;
unsigned long listenDroneTimeChecker = 100;

/*Button Menu*/
int btnStart = 1;
int btnSelect = 2;

/*Button Pad*/
int btnUp = 3;
int btnRight = 4;
int btnDown = 5;
int btnLeft = 6;

/*Button Action*/
int btnTriangle = 7;
int btnCircle = 8;
int btnCross = 9;
int btnSquare = 10;

/*Button L Action*/
int btnR1 = 11;
int btnR2 = 12;
int btnL1 = 13;
int btnL2 = 14;

/*Button Joystick*/
int btnR3 = 15;
int btnL3 = 16;

/*Joystick*/
int btnRx = 17;
int btnRy = 18;
int btnLx = 19;
int btnLy = 20;

#define PS2_DAT       5 //13  //14    
#define PS2_CMD       4 //11  //15
#define PS2_SEL       3 //10  //16
#define PS2_CLK       2 //12  //17

#define pressures   false
#define rumble      false

PS2X ps2x; 

int error = 0;
byte type = 0;
byte vibrate = 0;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7,8);
/**********************************************************/
 
LiquidCrystal_I2C lcd(0x3F,2,1,0,4,5,6,7,3, POSITIVE);

                                                                         
byte addresses[][6] = {"1Node","2Node"};


// Used to control whether this node is sending or receiving
bool role = 0;

void setup(){

	Serial.begin(115200);
	lcd.begin (16,2);
	lcd.setBacklight(HIGH);

	for(int i = 3; i > 0; i--){
		Serial.print("Aguardando ");
		Serial.print(i);
		Serial.println(" segundos!!!");
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("Aguardando ");
		lcd.print(i);
		lcd.setCursor(0,1);
		lcd.print(" segundos!!!");
		delay(1000);
	}
		
	
	error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);

	if(error == 0){
		Serial.print("Found Controller, configured successful ");
		Serial.print("pressures = ");
		if (pressures){
			Serial.println("true ");
		}else{
			Serial.println("false");
			Serial.print("rumble = ");
		}
		if (rumble){
			Serial.println("true)");
		}else{
			Serial.println("false");
			Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
			Serial.println("holding L1 or R1 will print out the analog stick values.");
			Serial.println("Note: Go to www.billporter.info for updates and to report bugs.");
		}
	}else if(error == 1){
		Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
   
	}else if(error == 2){
		Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

	}else if(error == 3){
		Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
	}
  
	type = ps2x.readType(); 
	switch(type) {
		case 0:{
			Serial.print("Unknown Controller type found ");
			break;
		}
		case 1:{
			Serial.print("DualShock Controller found ");
			break;
		}
		case 2:{
			Serial.print("GuitarHero Controller found ");
			break;
		}
		case 3:{
			Serial.print("Wireless Sony DualShock Controller found ");
			break;
		}
  
	}

	// Setup and configure radio

	radio.begin();
	radio.setPALevel(RF24_PA_MIN);
 
	if(radioNumber){
		radio.openWritingPipe(addresses[1]);        // Both radios listen on the same pipes by default, but opposite addresses
		radio.openReadingPipe(1,addresses[0]);      // Open a reading pipe on address 0, pipe 1
	}else{
		radio.openWritingPipe(addresses[0]);
		radio.openReadingPipe(1,addresses[1]);
	}
	//radio.startListening();// Start listening  
	delay(100);
	radio.stopListening();
}

void loop() {

	if(error == 1) //skip loop if no controller found
		return; 

	ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed
    
    if(ps2x.ButtonPressed(PSB_START)){         //will be TRUE as long as button is pressed
		Serial.println("Botão 'Start' pressionado");
		radio.write(&btnStart, sizeof(int));
	}
	if(ps2x.ButtonPressed(PSB_SELECT)){
		Serial.println("Botão 'Select' pressionado"); 
		Joy = !Joy;     
		radio.write(&btnSelect, sizeof(int));
	}

/***************************************************************/
	
	if(ps2x.ButtonPressed(PSB_PAD_UP)) {      //will be TRUE as long as button is pressed
		Serial.println("Botão 'Up' pressionado");
		radio.write(&btnUp, sizeof(int));
	}
	if(ps2x.ButtonPressed(PSB_PAD_RIGHT)){
		Serial.println("Botão 'Right' pressionado");
		radio.write(&btnRight, sizeof(int));
	}
	if(ps2x.ButtonPressed(PSB_PAD_LEFT)){
		Serial.println("Botão 'Left' pressionado");
		radio.write(&btnLeft, sizeof(int));
	}
	if(ps2x.ButtonPressed(PSB_PAD_DOWN)){
		Serial.println("Botão 'Down' pressionado");
		radio.write(&btnDown, sizeof(int));
	}   

/**************************************************************/
	if(ps2x.ButtonPressed(PSB_L3)){
		Serial.println("Botão 'L3' pressionado");
		radio.write(&btnL3, sizeof(int));
	}
		
	if(ps2x.ButtonPressed(PSB_R3)){
		Serial.println("Botão 'R3' pressionado");
		radio.write(&btnR3, sizeof(int));
	}

/**********************************************************/
			
	if(ps2x.ButtonPressed(PSB_L1)){
		Serial.println("Botão 'L1' pressionado");
		radio.write(&btnL1, sizeof(int));
	}
		
	if(ps2x.ButtonPressed(PSB_R1)){
		Serial.println("Botão 'R1' pressionado");
		radio.write(&btnR1, sizeof(int));
	}

	if(ps2x.ButtonPressed(PSB_L2)){
		Serial.println("Botão 'L2' pressionado");
		radio.write(&btnL2, sizeof(int));
	}
		
	if(ps2x.ButtonPressed(PSB_R2)){
		Serial.println("Botão 'R2' pressionado");
		radio.write(&btnR2, sizeof(int));
	}
		
/***********************************************************/
	
	if(ps2x.ButtonPressed(PSB_TRIANGLE)){
		Serial.println("Botão 'Triangle' pressionado");
		radio.write(&btnTriangle, sizeof(int)); 
	}
		
	if(ps2x.ButtonPressed(PSB_CIRCLE)){
		Serial.println("Botão 'Circle' pressionado");
		radio.write(&btnCircle, sizeof(int));
	}
		
	if(ps2x.ButtonPressed(PSB_CROSS)){
		Serial.println("Botão 'Cross' pressionado");
		radio.write(&btnCross, sizeof(int));
		//listenDrone();
	}
		
	if(ps2x.ButtonPressed(PSB_SQUARE)){
		Serial.println("Botão 'Square' pressionado");
		radio.write(&btnSquare, sizeof(int)); 
		//listenDrone(); 
	}  

/************************************************************/

	if(Joy) { //print stick values if either is TRUE
		Serial.print("Stick Values:");
		int Ly = (btnLy*1000) + ps2x.Analog(PSS_LY);
		Serial.print(ps2x.Analog(PSS_LY), DEC); //Left stick, Y axis. Other options: LX, RY, RX  
		Serial.print(",");
		int Lx = (btnLx*1000) + ps2x.Analog(PSS_LX);
		Serial.print(ps2x.Analog(PSS_LX), DEC); 
		Serial.print(",");
		int Ry = (btnRy*1000) + ps2x.Analog(PSS_RY);
		Serial.print(ps2x.Analog(PSS_RY), DEC); 
		Serial.print(",");
		int Rx = (btnRx*1000) + ps2x.Analog(PSS_RX);
		Serial.println(ps2x.Analog(PSS_RX), DEC); 

		if(temp_joy[0] != Ly){
			radio.write(&Ly, sizeof(int));delay(delay_tr);
			temp_joy[0] = Ly;
		}
		if(temp_joy[1] != Lx){
			radio.write(&Lx, sizeof(int));delay(delay_tr);
			temp_joy[1] = Lx;
		}
		if(temp_joy[2] != Ry){
			radio.write(&Ry, sizeof(int));delay(delay_tr);
			temp_joy[2] = Ry;
		}
		if(temp_joy[3] != Rx){
			radio.write(&Rx, sizeof(int));delay(delay_tr);
			temp_joy[3] = Rx;
		}
	}     

	if(millis() >= listenDroneTime){
		listenDroneTime += listenDroneTimeChecker;
		listenDrone();
	}
}


/*Escuta o que o drone enviou de dados para o controle*/
void listenDrone(){
	int got_time = 0;
	int timeReset = 0;
	radio.startListening();
	while(true){
		if(radio.available()){
			while (radio.available()) {                                   // While there is data ready
				radio.read( &got_time, sizeof(int) );             // Get the payload
			}
		}else{
			timeReset++;
		}
		if(got_time != 0 || timeReset == 100){
			if(got_time != 0){
				//Serial.print("Numero recebido ");
				//Serial.println(got_time);
				//Serial.println(" milisegundos");
				analizeAnswer(got_time);
			}
			break;     
		}  
	}
	radio.stopListening();
}

/*Analiza a resposta que chega do drone*/
void analizeAnswer(int answer){
	lcd.clear();
}

