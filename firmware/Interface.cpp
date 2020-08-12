#include "Interface.h"



void LED_on()
{
    Serial.println("LED on");
    digitalWrite(LEDR1_PIN, HIGH);
}

void LED_off()
{
    Serial.println("LED off");
    digitalWrite(LEDR1_PIN, LOW);
}

void SayHello()
{
    char *arg;
    arg = SCmd.next();    // Get the next argument from the SerialCommand object buffer
    if (arg != NULL)      // As long as it existed, take it
    {
        Serial.print("Hello ");
        Serial.println(arg);
    }
    else {
        Serial.println("Hello, whoever you are");
    }
}


void process_command()
{
    int aNumber;
    char *arg;

    Serial.println("We're in process_command");
    arg = SCmd.next();
    if (arg != NULL)
    {
        aNumber = atoi(arg);  // Converts a char string to an integer
        Serial.print("First argument was: ");
        Serial.println(aNumber);
    }
    else {
        Serial.println("No arguments");
    }

    arg = SCmd.next();
    if (arg != NULL)
    {
        aNumber = atol(arg);
        Serial.print("Second argument was: ");
        Serial.println(aNumber);
    }
    else {
        Serial.println("No second argument");
    }

}

// This gets set as the default handler, and gets called when no other command matches.
void unrecognized()
{
    Serial.println("What?");
}

void interface_init(void) {
    pinMode(LEDR1_PIN, OUTPUT);
    digitalWrite(LEDR1_PIN, 0);

    SCmd.addCommand("ON", LED_on);      // Turns LED on
    SCmd.addCommand("OFF", LED_off);       // Turns LED off
    SCmd.addCommand("HELLO", SayHello);    // Echos the string argument back
    SCmd.addCommand("P", process_command); // Converts two arguments to integers and echos them back
    SCmd.addDefaultHandler(unrecognized);  // Handler for command that isn't matched  (says "What?")
}
