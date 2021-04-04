//Adafruit
#include <Adafruit_SSD1306.h>
#define OLED_Address 0x3C // 0x3C device address of I2C OLED
Adafruit_SSD1306 oled(128, 64);
//SETUP LEDS
int led = 13; 
//SETUP BUZZER
// notes in the melody:
int pin_buzzer = 11;
int melody[] = {
  262, 196, 196, 220, 196, 0, 247, 262
}; 
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};
int pin_aquecedor = 10; //SETUP AQUECEDOR
int pin_bombaDagua = 9; //SETUP BOMBA D'AGUA
int btn_incremento = 2; //SETUP BOTAO INCREMENTO
int btn_decremento = 3; //SETUP BOTAO DECREMENTO
int btn_start = 4; //SETUP BOTAO START
int tempo = 0;
bool start = false;
float temperatura_sensor_user = 0;
float temperatura_sensor_term = 0;

int pin_sensor_temperatura_term = A0; //Entrada termistor
int pin_sensor_temperatura_user = A1; //Entrada temp pelo user
// Parâmetros do termistor
const double beta = 3600.0;
const double r0 = 10000.0;
const double t0 = 273.0 + 25.0;
const double rx = r0 * exp(-beta/t0);
// Numero de amostras na leitura
const int nAmostras = 2;
// Parâmetros do circuito
const double vcc = 5.0;
const double R = 10000.0;
int contagem_last;
float templast_user, templast_term;
//DECLARANDO ENTRADAS E SAIDAS

void setup() {
  	 pinMode(led, OUTPUT);
	 pinMode(pin_buzzer, OUTPUT);
	 pinMode(pin_aquecedor, OUTPUT);
	 pinMode(pin_bombaDagua, OUTPUT);
	 pinMode(btn_incremento, INPUT);
	 pinMode(btn_decremento, INPUT);
	 pinMode(btn_start, INPUT);
	 Serial.begin(9600);
	
	  //inicialização do display oled 
  	  oled.begin(SSD1306_SWITCHCAPVCC, OLED_Address);
  	  oled.clearDisplay();
  	  oled.setTextSize(1);
 	  oled.setTextColor(WHITE);

}

// the loop routine runs over and over again forever:
void loop() {
	//Incrementar o tempo
	if(digitalRead(btn_incremento) == LOW && start == false){
		Serial.println("Clicou no botao de incremento");
		tempo += 10;
		Serial.println(tempo);
	}
	//Decrementar o tempo
	if(digitalRead(btn_decremento) == LOW && start == false){
		Serial.println("Clicou no botao de decremento");
		tempo -= 10;
		Serial.println(tempo);
	}
	//Startar o programa
	if(digitalRead(btn_start) == LOW && start == false){
		start = true;
		temperatura_sensor_user = temperatura_termistor(pin_sensor_temperatura_user);
		Serial.println("Programa starto");
		digitalWrite(pin_aquecedor, HIGH);
		digitalWrite(pin_bombaDagua, HIGH);
		for(int i = 1; i <= tempo; i++){
			temperatura_sensor_term = temperatura_termistor(pin_sensor_temperatura_term);
			if(temperatura_sensor_term >= temperatura_sensor_user){
				digitalWrite(pin_aquecedor, LOW);
			}else{
				digitalWrite(pin_aquecedor, HIGH);
			}
			oledTempo(i);
			serial(i);
			delay(1000); 
		}
		digitalWrite(pin_aquecedor, LOW);
		digitalWrite(pin_bombaDagua, LOW);
		for(int repeticao = 1; repeticao <= 6; repeticao++){
		     digitalWrite(led, HIGH);
	 		//Aguarda intervalo de tempo em milissegundos
			delay(200); 
			//Apaga o led
			digitalWrite(led, LOW);
			buzzer();
		}
		start = false;
	}		
}

//Função da buzzer
void buzzer(){
    // iterate over the notes of the melody:
    for (int thisNote = 0; thisNote < 8; thisNote++) {
    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(pin_buzzer, melody[thisNote], noteDuration);
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(pin_buzzer);
  }
}

//Função de formatação de hora:min
void formatHoraMin(int minutosSetados){
	int hora;
	int minutos;
	char mensagem;
	hora = minutosSetados / 60;
	minutos = minutosSetados % 60;
	//mensagem = hora + ":" + minutos;
	Serial.print(hora);
	Serial.print(":");
	Serial.print(minutos);
}

void serial(int i){
	Serial.print("Tempo Atual: "); //Serial.print(formatHoraMin(i));
	formatHoraMin(i);
	Serial.print("|Tempo Setado: ");
	formatHoraMin(tempo);//Serial.print(formatHoraMin(tempo));
	Serial.print(" |Temp. Setado: ");
	Serial.print(temperatura_sensor_user);
	Serial.print(" |Temp. Medida: ");
	Serial.print(temperatura_sensor_term);
	Serial.print(" |Status Bomba: ");
	statusEquipamento(digitalRead(pin_bombaDagua));
	Serial.print(" |Status Aquecedor: ");
	statusEquipamento(digitalRead(pin_aquecedor));
	Serial.println("");
}

void statusEquipamento(int status){
	if(status == 1){
		Serial.print("Ligado");
	}else{
		Serial.print("Desligado");
	}
}

float temperatura_termistor(int sensor){
  // Le o sensor algumas vezes
  	int soma = 0;
	for (int i = 0; i < nAmostras; i++) {
    		soma += analogRead(sensor);
    		delay (10);
  	}
  	// Determina a resistência do termistor
	 double v = (vcc*soma)/(nAmostras*1024.0);
	 double rt = (vcc*R)/v - R;
	 
	 // Calcula a temperatura
	 double t = beta / log(rt/rx);
	 return t-273.0;
}

void oledTempo(int i){
  oled.setCursor(0,0); 		//seta o cursor no inicio
  oled.setTextColor(BLACK); 	//e apaga o
  oled.print("temp. setada:");			// valor da temp.
  oled.print(templast_user);		// antigo, o templast

  oled.setCursor(0,0);			// seta o cursor no inicio
  oled.setTextColor(WHITE);	// 
  oled.print("temp. setada:");
  oled.print(temperatura_sensor_user);
  templast_user = temperatura_sensor_user;
  oled.display();

  oled.setCursor(0,7); 		//seta o cursor no inicio
  oled.setTextColor(BLACK); 	//e apaga o
  oled.print("temp. atual:");			// valor da temp.
  oled.print(templast_term);		// antigo, o templast

  oled.setCursor(0,7);			// seta o cursor no inicio
  oled.setTextColor(WHITE);	// 
  oled.print("temp. atual:");
  oled.print(temperatura_sensor_term);
  templast_term = temperatura_sensor_term;
  oled.display();

  oled.setCursor(0,20); 		//seta o cursor no inicio
  oled.setTextColor(BLACK); 	//e apaga o
  oled.print("Falta: ");			// valor da temp.
  contagemRegressiva(contagem_last);		// antigo, o templast

  oled.setCursor(0,20);			// seta o cursor no inicio
  oled.setTextColor(WHITE);	// 
  oled.print("Falta: ");
  contagemRegressiva(tempo - i);
  contagem_last = tempo - i;
  oled.display();
}

void contagemRegressiva(int minutosSetados){
	int hora;
	int minutos;
	char mensagem;
	hora = minutosSetados / 60;
	minutos = minutosSetados % 60;
	oled.print(hora);
	oled.print(":");
	oled.print(minutos);
}
