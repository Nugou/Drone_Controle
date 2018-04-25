
/*
 * ========================================================================================================================================================================================
 * #Declaração de biblioteca e define
 */
 
#include <SPI.h>
#include <RF24.h>
#include <PS2X_lib.h>

#define PS2_DAT       5 //13  //14    
#define PS2_CMD       4 //11  //15
#define PS2_SEL       3 //10  //16
#define PS2_CLK       2 //12  //17

#define pressures   false
#define rumble      false

/*
 * ========================================================================================================================================================================================
 * #Fim da declaração de biblioteca e define
 */


/*
 * ========================================================================================================================================================================================
 * #Declarando variaveis globais
 */

boolean radioNumber = false;
boolean Joy = false;
boolean role = false;

int delay_tr = 80;
int btn_code = 0;
int temp_joy[4] = {128,128,128,128};
int error = 0;
int nrf_power = 3;

unsigned long listenDroneTime = 0;
unsigned long listenDroneTimeChecker = 100;

byte type = 0;
byte vibrate = 0;
byte addresses[][6] = {"1Node","2Node"};


/*
 * ========================================================================================================================================================================================
 * #Fim de declarando variaveis globais
 */



/*
 * ========================================================================================================================================================================================
 * #Declarando objetos
 */

RF24 radio(7,8);
PS2X ps2x;

/*
 * ========================================================================================================================================================================================
 * #Fim de Declarando objetos
 */


/*
 * ========================================================================================================================================================================================
 * #Declarando estruturas
 */


struct stDataDrone{
	int stPotMotor[4] = {0, 0, 0, 0};
	int stBactery = 0;
 	double stHeight = 0;
 	double stGyZ = 0;
 	double stGpsLat = 0;
	double stGpsLon = 0;
};
typedef struct stDataDrone typeDataRf;
typeDataRf traDataRf;

/*
 * ========================================================================================================================================================================================
 * #Fmi de declarando estruturas
 */    


/*
 * ========================================================================================================================================================================================
 * #Configurações Iniciais
 */

void setup(){

	Serial.begin(115200);
	SPI.begin();		
	Serial.print("Conectando controle");
	while(true){
		if(ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble) == 0){
			break;
		}
		Serial.print(".");
		//delay(100);
	}
	Serial.println("Conexão concluida");

	// Setup and configure radio

	radio.begin();
	if(nrf_power == 1){radio.setPALevel(RF24_PA_LOW);}
	else if(nrf_power == 2){radio.setPALevel(RF24_PA_MIN);}
	else if(nrf_power == 3){radio.setPALevel(RF24_PA_HIGH);}
	else{radio.setPALevel(RF24_PA_MAX);}
 
	if(radioNumber){
		radio.openWritingPipe(addresses[1]);       
		radio.openReadingPipe(1,addresses[0]);      
	}else{
		radio.openWritingPipe(addresses[0]);
		radio.openReadingPipe(1,addresses[1]);
	} 
	delay(100);

	radio.stopListening();
}

/*
 * ========================================================================================================================================================================================
 * #Fim das configurações iniciais
 */


/*
 * ========================================================================================================================================================================================
 * #Inicio do Loop Arduino
 */

void loop() {

	if(error == 1) 
		return; 

	ps2x.read_gamepad(false, vibrate); 
    
    if(ps2x.ButtonPressed(PSB_START)){         
		btn_code = 1;
		Serial.println("Botão 'Start' pressionado");
		trData(btn_code);
	}
	if(ps2x.ButtonPressed(PSB_SELECT)){
		btn_code = 2;
		Serial.println("Botão 'Select' pressionado"); 
		Joy = !Joy;     
		trData(btn_code);
	}

/***************************************************************/
	
	if(ps2x.ButtonPressed(PSB_PAD_UP)) {      
		btn_code = 3;
		Serial.println("Botão 'Up' pressionado");
		trData(btn_code);
	}
	if(ps2x.ButtonPressed(PSB_PAD_RIGHT)){
		btn_code = 4;
		Serial.println("Botão 'Right' pressionado");
		trData(btn_code);
	}
	if(ps2x.ButtonPressed(PSB_PAD_DOWN)){
		btn_code = 5;
		Serial.println("Botão 'Down' pressionado");
		trData(btn_code);
	}   
	if(ps2x.ButtonPressed(PSB_PAD_LEFT)){
		btn_code = 6;
		Serial.println("Botão 'Left' pressionado");
		trData(btn_code);
	}
	
/**************************************************************/

	if(ps2x.ButtonPressed(PSB_TRIANGLE)){
		btn_code = 7;
		Serial.println("Botão 'Triangle' pressionado");
		trData(btn_code);
	}
		
	if(ps2x.ButtonPressed(PSB_CIRCLE)){
		btn_code = 8;
		Serial.println("Botão 'Circle' pressionado");
		trData(btn_code);
	}
		
	if(ps2x.ButtonPressed(PSB_CROSS)){
		btn_code = 9;
		Serial.println("Botão 'Cross' pressionado");
		trData(btn_code);
	}
		
	if(ps2x.ButtonPressed(PSB_SQUARE)){
		btn_code = 10;
		Serial.println("Botão 'Square' pressionado");
		trData(btn_code);
	}  

/************************************************************/
		
	if(ps2x.ButtonPressed(PSB_R1)){
		btn_code = 11;
		Serial.println("Botão 'R1' pressionado");
		trData(btn_code);
	}
	
	if(ps2x.ButtonPressed(PSB_R2)){
		btn_code = 12;
		Serial.println("Botão 'R2' pressionado");
		trData(btn_code);
	}
	
	if(ps2x.ButtonPressed(PSB_L1)){
		btn_code = 13;
		Serial.println("Botão 'L1' pressionado");
		trData(btn_code);
	}
	
	if(ps2x.ButtonPressed(PSB_L2)){
		btn_code = 14;
		Serial.println("Botão 'L2' pressionado");
		trData(btn_code);
	}
	
/**********************************************************/
	
	if(ps2x.ButtonPressed(PSB_R3)){
		btn_code = 15;
		Serial.println("Botão 'R3' pressionado");
		trData(btn_code);
	}
	if(ps2x.ButtonPressed(PSB_L3)){
		btn_code = 16;
		Serial.println("Botão 'L3' pressionado");
		trData(btn_code);
	}
	
		
/***********************************************************/

	if(Joy) { //print stick values if either is TRUE
		Serial.print("Stick Values:");
		
		btn_code = 20;
		int Ly = (btn_code*1000) + ps2x.Analog(PSS_LY);
		Serial.print(ps2x.Analog(PSS_LY), DEC); //Left stick, Y axis. Other options: LX, RY, RX  
		Serial.print(",");

		
		btn_code = 19;
		int Lx = (btn_code*1000) + ps2x.Analog(PSS_LX);
		Serial.print(ps2x.Analog(PSS_LX), DEC); 
		Serial.print(",");

		
		btn_code = 18;
		int Ry = (btn_code*1000) + ps2x.Analog(PSS_RY);
		Serial.print(ps2x.Analog(PSS_RY), DEC); 
		Serial.print(",");

		
		btn_code = 17;
		int Rx = (btn_code*1000) + ps2x.Analog(PSS_RX);
		Serial.println(ps2x.Analog(PSS_RX), DEC); 

		if(temp_joy[0] != Ly){
			trData(Ly);
			temp_joy[0] = Ly;
		}
		if(temp_joy[1] != Lx){
			trData(Lx);
			temp_joy[1] = Lx;
		}
		if(temp_joy[2] != Ry){
			trData(Ry);
			temp_joy[2] = Ry;
		}
		if(temp_joy[3] != Rx){
			trData(Rx);
			temp_joy[3] = Rx;
		}
	}     

	if(millis() >= listenDroneTime){
		listenDroneTime += listenDroneTimeChecker;
		listenDrone();
		//Serial.println("Recebendo dados");
	}

	if(Serial.available()){
		String msg = "";
		while(Serial.available()){
			msg += (char) Serial.read();	
		}
		Serial.println(msg);
	}
	delay(30);
}

/*
 * ========================================================================================================================================================================================
 * #Fim do Loop Arduino
 */


/*
 * ========================================================================================================================================================================================
 * #Recebimento de Dados do Drone
 */
 
void listenDrone(){
	radio.startListening();delay(30);
	if(radio.available()){
		while (radio.available()) {                                   
			radio.read( &traDataRf, sizeof(typeDataRf));  delay(20);   
			//radio.read( &traDataStatusRf, sizeof(typeDataStatusRf));   delay(20);  
			//radio.read( &traDataGyRf, sizeof(typeDataGyRf));     delay(20);
			//radio.read( &traDataGpsRf, sizeof(typeDataGpsRf));   delay(20);          
			
		}
		//Serial.print("Recebido: ");
		//Serial.print("X: ");Serial.print(recDataRf.stGpsLatitude,6);
		//Serial.print("  Y: ");Serial.print(recDataRf.stGpsLongitude,6);
		//Serial.println();
	}
	radio.stopListening();
	delay(30);
	/*
	String logD = 
	(String)traDataRf.stPotMotor[0] + ":" + 
	(String)traDataRf.stPotMotor[1] + ":" + 
	(String)traDataRf.stPotMotor[2] + ":" + 
	(String)traDataRf.stPotMotor[3] + ":" +
	(String)traDataRf.stBactery + ":" +
	(String)traDataRf.stHeight + ":" +
	(String)traDataRf.stGyZ + ":" +
	(String)traDataRf.stGpsLatitude + ":" +
	(String)traDataRf.stGpsLongitude + 
	"\n";
	
	for (int i = 0; i < logD.length(); i++){
		Serial.write(logD[i]);   
	} */

	Serial.print(traDataRf.stPotMotor[0]);Serial.print(":");
	Serial.print(traDataRf.stPotMotor[1]);Serial.print(":");
	Serial.print(traDataRf.stPotMotor[2]);Serial.print(":");
	Serial.print(traDataRf.stPotMotor[3]);Serial.print(":");
	Serial.print(traDataRf.stBactery);Serial.print(":");
	Serial.print(traDataRf.stHeight);Serial.print(":");
	Serial.print(traDataRf.stGyZ);Serial.print(":");
	Serial.print(traDataRf.stGpsLat,6);Serial.print(":");
	Serial.print(traDataRf.stGpsLon,6);
	Serial.println();
}

/*
 * ========================================================================================================================================================================================
 * #Fim do Recebimento dos dados do Drone
 */


/*
 * ========================================================================================================================================================================================
 * #Envia mensagem para o drone
 */

void trData(int btValue){
	while(!radio.write(&btValue, sizeof(int)));
	delay(delay_tr);
}

/*
 * ========================================================================================================================================================================================
 * #Fim de envio de mensagem para o drone
 */
