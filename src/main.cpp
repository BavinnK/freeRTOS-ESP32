const int trigPin = 5, echopin = 18;
volatile unsigned long start_time = 0;


QueueHandle_t distanceQueue;

void ultra_sonic_task(void *pvParameters) {


  while (1) {
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}
void IRAM_ATTR echo_ISR() {
  bool echo_stat = digitalRead(echopin);
  
  if (echo_stat==HIGH) {
    start_time = micros();
  }
  else {
   unsigned long pulse_width = (micros()-start_time);
   BaseType_t taskWoken=pdFALSE;

   xQueueSendFromISR(distanceQueue, &pulse_width, &taskWoken);
   if(taskWoken){
    portYIELD_FROM_ISR();
   }
  }
  
  
}
void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echopin, INPUT);
  attachInterrupt(echopin, echo_ISR, CHANGE);
  Serial0.begin(115200);
  distanceQueue=xQueueCreate(10, sizeof(unsigned long));
  xTaskCreatePinnedToCore(ultra_sonic_task, "ultraSonicTask", 4096, NULL, 2, NULL, 0);

}

void loop() {
  char buff [70];
  
  unsigned long rec_distance_raw;
  
  if(xQueueReceive(distanceQueue, &rec_distance_raw, 10)==pdTRUE){
  unsigned long rec_distance_data=rec_distance_raw/58;

  sprintf(buff, "Distance in cm: %d",rec_distance_data);
  Serial0.println(buff);
  delay(100);
  }
  else{
    Serial0.println("ERROR!!");
    delay(100);
  }


}
