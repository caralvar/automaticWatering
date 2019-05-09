/*
  Estudio de la lectura de los sensores
  Este ejemplo es utilizado para poder caracterizar el funcionamiento de los sensores.
  Programa de riego automatico
  un delay de 1 equivale a que el loop dure 3
  entonces 1 ms = 3 ms
            3333 ms = 10000 ms = 10 s
  el programa mide la humedad cada 10 segundos
  y si esta seco (huemdadSuelo<40) enciende una bomba durante 2 segundos.
  Con el fin de hacer demostracion del funcionamiento.
*/

int pinSensor = 13;
int pinLectura = 12;
int pinBomba = 11;
int humedadMinima = 40;
int segundos = 0;
int humedadSuelo = 100;
int tiempoEspera = 10000;
int tiempoRiego = 2000;
int controlManualyConfig = 0;      // 1-Entra al control manual 0-Sale control manual
int seleccion = 4;
int numeroConfig = -1;
boolean manual = false;
boolean configuracion = false;
boolean configTiempoEspera = false;
boolean configTiempoRiego = false;
boolean configHumedadMinima = false;


// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(pinLectura, OUTPUT);
  pinMode(pinBomba, OUTPUT);
  pinMode(pinSensor, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  //Primero se revisa si el usuario selecciono entrar a otro modo
  //0-Automatico
  //1-Manual
  //2-Configuracion
  //Lectura de la seleccion
  if (Serial.available() > 0) {
    controlManualyConfig = Serial.read();
  } else {
    controlManualyConfig = '0';
  }
  if (controlManualyConfig != '0' && controlManualyConfig != '3' && controlManualyConfig != '4') {
    controlManualyConfig = '0';
  }
  if (controlManualyConfig == '0') {
    riegoAutomatico();
  }
  if (controlManualyConfig == '3') {
    digitalWrite(pinBomba, LOW);
    humedadSuelo = 100;
    segundos = 0;
    //Serial.println("Modo manual");
    //Serial.println("1-Activar bomba");
    //Serial.println("2-Desactivar bomba");
    //Serial.println("0-Volver a modo automatico");
    manual = true;
    while (manual) {
      delay(1);
      segundos++;
      if (segundos % 20 == 0) {
        Serial.print(" ");
      }
      if (segundos > tiempoEspera) {
        lecturaHumedad();
      }
      if (Serial.available() > 0) {
        seleccion = Serial.read();
      } else {
        seleccion = '4';
      }
      if (seleccion == '1') {
        digitalWrite(pinBomba, HIGH);
      }
      if (seleccion == '2') {
        digitalWrite(pinBomba, LOW);
      }
      if (seleccion == '0') {
        digitalWrite(pinBomba, LOW);
        segundos = 0;
        humedadSuelo = 100;
        controlManualyConfig = '0';
        manual = false;
        //Serial.println("Modo automatico");
      }
    }
  }
  if (controlManualyConfig == '4') {
    digitalWrite(pinBomba, LOW);
    humedadSuelo = 100;
    segundos = 0;
    Serial.println("Configuracion:");
    Serial.println("1-Cambiar Tiempo espera");
    Serial.println("2-Cambiar Tiempo riego");
    Serial.println("3-Cambiar humedad minima");
    Serial.println("0-Volver a modo automatico");
    configuracion = true;
    while (configuracion) {
      if (Serial.available() > 0) {
        seleccion = Serial.read();
      } else {
        seleccion = '4';
      }
      if (seleccion == '1') {
        Serial.println("Ingrese un numero entre 10 y 60.");
        configTiempoEspera = true;
        while (configTiempoEspera) {
          if (Serial.available() > 0) {
            numeroConfig = lecturaNumeros();
          } else {
            numeroConfig = 0;
          }
          if (numeroConfig >= 10) {
            tiempoEspera = 1000 * numeroConfig;
            numeroConfig = 0;
            seleccion = '4';
            configTiempoEspera = false;
          }
        }
      }
      if (seleccion == '2') {
        Serial.println("Ingrese un numero entre 2 y 9.");
        configTiempoRiego = true;
        while (configTiempoRiego) {
          if (Serial.available() > 0) {
            numeroConfig = lecturaNumeros();
          } else {
            numeroConfig = 0;
          }
          if (numeroConfig >= 2 && numeroConfig <= 9) {
            tiempoRiego = 1000 * numeroConfig;
            numeroConfig = 0;
            seleccion = '4';
            configTiempoRiego = false;
          }
        }
      }
      if (seleccion == '3') {
        Serial.println("Ingrese un numero entre 10 y 90.");
        configHumedadMinima = true;
        while (configHumedadMinima) {
          if (Serial.available() > 0) {
            numeroConfig = lecturaNumeros();
          } else {
            numeroConfig = 0;
          }
          if (numeroConfig >= 10 && numeroConfig <= 90) {
            humedadMinima = numeroConfig;
            numeroConfig = 0;
            seleccion = '4';
            configHumedadMinima = false;
          }
        }
      }
      if (seleccion == '0') {
        controlManualyConfig = '0';
        configuracion = false;
        //Serial.println("Modo automatico");
      }
    }
  }
}






///////////////////////////////////////////////////////////////
/////////////Proceso de lectura de humedad/////////////////////

void lecturaHumedad() {
  digitalWrite(pinSensor, HIGH);
  digitalWrite(pinLectura, HIGH);
  delay(3);     //Espera de 3 ms para realizar la lectura (necesaria)
  // read the input on analog pin 0:
  humedadSuelo = -0.1398601399 * analogRead(A0) + 127.972028; //lectura y conversion a valores entre 0 y 100
  digitalWrite(pinLectura, LOW);
  digitalWrite(pinSensor, LOW); //Se apaga el sensor inmediatamente despues de realizar la lectura.
  Serial.print(humedadSuelo);
  segundos = 0;
}

////////////////////////////////////////////////////////////////
//////////////Proceso de riego automatico///////////////////////

void riegoAutomatico() {
  //Muestreo cada 10s
  if (segundos % 20 == 0) {
    Serial.print(" ");
  }

  if (segundos  > tiempoEspera) {
    lecturaHumedad();
  }
  if (humedadSuelo < humedadMinima && segundos <= tiempoRiego) {
    digitalWrite(pinBomba, HIGH);
  } else {
    digitalWrite(pinBomba, LOW);
  }
  delay(1);
  segundos++;
}
/////////////////////////////////////////////////
////////Funcion para obtener un numero//////////
///////////de dos digitos en el////////////////
//////////////monitor serial//////////////////
int lecturaNumeros () {
  int decena = (int)Serial.read();
  delay(2);
  int unidad = (int)Serial.read();
  int numero =  10 * (decena - 48) + (unidad - 48);
  Serial.println(numero);
  return numero;
}





