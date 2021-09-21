#include "mbed.h"
#include "LM75B.h"
#include "C12832_lcd.h"

//declare global variables
Serial pc(USBTX, USBRX);

LM75B t_sensor(p28,p27);
C12832_LCD lcd;
AnalogIn p1(p20);
AnalogIn p2(p19);
DigitalOut RedLED(p23);
DigitalOut BlueLED(p25);
InterruptIn button(p14);

//global variable used for states
int butpress = 0;

//interrupt method for managing states
void state(){
    if (butpress < 2){
        butpress++;
        wait(0.3);
    }else{
        butpress = 0;
        wait(0.3);
    }
}


int main() {
    //variables used for serial communication
    int putty_char;
    putty_char = '0';
    //variables used for finding board temperature
    double temp_C;
    double temp_F;
    //variable used to controll high and low limits of device
    float highC;
    float lowC;
    float highF;
    float lowF;
    //variables for reading petentiometer values 
    float p1_val;
    float p2_val;
    //interrupt button
    button.rise(&state);
    lcd.cls();
    
    while(1) {
        //variables used for temperature calculations
        temp_C = (float)t_sensor.temp();
        temp_F = (1.8*temp_C)+32;
        p1_val = p1.read();
        p2_val = p2.read();
        highC = (p1_val*60) + 20.0;
        lowC = (p2_val*50) - 15.0;
        
        //method to control lcd display using serial communication
        if(pc.readable()==1){
            putty_char = pc.getc();
            //pc.putc(putty_char); //Commented out line used for debugging
            
            if(putty_char == 'c'||putty_char == 'C'){
                butpress = 0;
                pc.printf("Board temperature = %.2f C\r\n", temp_C);
                pc.printf("Display is in Celsius\r\n");
                pc.printf("\r\n");
            }else if(putty_char == 'f'||putty_char == 'F'){
                butpress = 1;
                pc.printf("Board temperature = %.2f F\r\n", temp_F);
                pc.printf("Display is in Fahrenheit\r\n");
                pc.printf("\r\n");
            }else if(putty_char == 'b' || putty_char == 'B'){
                butpress = 2;
                pc.printf("*** Display blanked ***\r\n");
                pc.printf("\r\n");
            }
        }
        
        //if statement for controlling states
        if(butpress == 0){ //celcius state
            //clear lcd and display current temp and high & low limits
            lcd.cls();
            lcd.locate(0,0);
            lcd.printf("Current Temperature %.1f C", temp_C);
            
            lcd.locate(0,9);
            lcd.printf("High Limit (20 to 80) %.0f C", highC);
            
            lcd.locate(0,18);
            lcd.printf("Low Limit (-15 to 35) %.0f C ", lowC);
            
            //if the current temperature is larger than the high limit turn on red light
            if(temp_C >= highC){
                RedLED = 0;
            }else{
                RedLED = 1;
            }
            
            //if the current temperature is smaller than the low limit turn on blue light
            if(temp_C <= lowC){
                BlueLED = 0;
            }else{
                BlueLED = 1;
            }
        }else if(butpress == 1){//fahrenheit state
            //same as celcius display, but all values converted to fahrenheit
            lcd.cls();
            lcd.locate(0,0);
            lcd.printf("Current Temperature %.1f F", temp_F);
            
            lcd.locate(0,9);
            highF = (1.8*highC)+32;
            lcd.printf("High Limit (68 to 176) %.0f F", highF);
            
            lcd.locate(0,18);
            lowF = (1.8*lowC)+32;;
            lcd.printf("Low Limit (5 to 95) %.0f F", lowF);
            
            if(temp_F >= highF){
                RedLED = 0;
            }else{
                RedLED = 1;
            }
            
            if(temp_F <= lowF){
                BlueLED = 0;
            }else{
                BlueLED = 1;
            }
        }else if(butpress ==2){  //blank state
            lcd.cls();
        }
            
        wait(0.1);
    }
}
