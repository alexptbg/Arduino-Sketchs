const int TriggerPin = 3; // pin del TRIGGER
const int EchoPin = 4; // pin del ECHO
int greenled = 13;
long Duration;
 
void setup(){
pinMode(TriggerPin,OUTPUT); // Pin del TRIGGER lo ponemos en output
pinMode(EchoPin,INPUT); // Pin del ECHO lo ponemos en input
pinMode(greenled,OUTPUT);
Serial.begin(115200); // activamos el puerto serie para ver los resultados

}
 
void loop(){
  
digitalWrite(TriggerPin, LOW); // ponemos el pin TRIGGER en LOW
delayMicroseconds(2); // esperamos 2 milisegundos
digitalWrite(TriggerPin, HIGH); // ponemos el pin TRIGGER en HIGH
delayMicroseconds(10); // lo temos activado durante 10 milisegundos
digitalWrite(TriggerPin, LOW); // ponemos el pin del TRIGGER en LOW
 
Duration = pulseIn(EchoPin,HIGH); // Esperamos a que el pin del ECHO devuelva HIGH, y guardamos el tiempo
// Devuelve el tiempo en milisegundos
long dis = fDistancia(Duration); // Función para calcular la distancia

Serial.print("Distancia = "); // Y la mostramos por el puerto serie
Serial.print(dis,);
Serial.println(" mm");
digitalWrite(greenled, HIGH);
delay(200);
digitalWrite(greenled, LOW);
delay(1000); // Esperamos 1 segundo para hace de nuevo la medición
}
 
long fDistancia(long tiempo) {
  long DistanceCalc;
  DistanceCalc = (tiempo /2.9) / 2;
  //DistanceCalc = (tiempo /29) / 2;
  //DistanceCalc = (tiempo / 74) / 2;
  return DistanceCalc+2;
}
