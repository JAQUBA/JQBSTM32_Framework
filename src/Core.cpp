#include "Core.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>

Scheduler mainTasks;
Scheduler interruptTasks;

// Zmienna śledząca czas systemowy w milisekundach
uint32_t ulMillis = 0;

taskStruct addTaskInterrupt(taskCallback_f functionPointer, uint32_t delay, bool single, Scheduler::taskTime time) {return interruptTasks.addTask(functionPointer, delay, single, time);}
taskStruct addTaskMain(taskCallback_f functionPointer, uint32_t delay, bool single, Scheduler::taskTime time) {return mainTasks.addTask(functionPointer, delay, single, time);}

// Funkcja opóźniająca dla AVR
void delay(volatile uint32_t delay_ms) {
  for(volatile uint32_t i = 0; i < delay_ms; i++) {
    _delay_ms(1);
    wdt_reset(); // Resetowanie watchdog dla dłuższych opóźnień
  }
}

// Funkcja opóźniająca w mikrosekundach dla AVR
void delay_us(volatile uint32_t delay_us) {
  for(volatile uint32_t i = 0; i < delay_us; i++) {
    _delay_us(1);
  }
}

uint32_t millis() {return ulMillis;}

// Konstruktor Core dla ATmega128
Core::Core() {
  // Inicjalizacja timera dla odmierzania czasu
  // Używamy Timer0 do generowania przerwań co 1ms
  TCCR0 = (1<<WGM01);  // CTC mode
  TCCR0 |= (1<<CS01) | (1<<CS00);  // Preskaler 64
  OCR0 = 249;  // 16MHz / 64 / 250 = 1000Hz (1ms)
  TIMSK |= (1<<OCIE0);  // Włącz przerwanie Compare Match
  
  // Dodanie zadania aktualizującego licznik milisekund
  addTaskInterrupt(taskCallback {
    ulMillis++;
  }, 1);
  
  init();
  sei();  // Włącz globalne przerwania
}

Core _core;

int main() {
  setup();
  while (1) {
    mainTasks.execute();
    loop();
  }
}

// Obsługa przerwania Timer0 Compare Match
ISR(TIMER0_COMP_vect) {
  interruptTasks.poll();
  interruptTasks.execute();
  mainTasks.poll();
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Słabe definicje funkcji użytkownika
__weak void init() {}
__weak void setup() {}
__weak void loop() {}