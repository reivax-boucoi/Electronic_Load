#ifndef SerialCommand_h
#define SerialCommand_h

#include "Arduino.h"


// If you want to use SerialCommand with the hardware serial port only, and want to disable
// SoftwareSerial support, and thus don't have to use "#include <SoftwareSerial.h>" in your
// sketches, then uncomment this define for SERIALCOMMAND_HARDWAREONLY, and comment out the 
// corresponding #undef line.  
//
// You don't have to use SoftwareSerial features if this is not defined, you can still only use 
// the Hardware serial port, just that this way lets you get out of having to include 
// the SoftwareSerial.h header. 
#define SERIALCOMMAND_HARDWAREONLY 1
//#undef SERIALCOMMAND_HARDWAREONLY

#ifdef SERIALCOMMAND_HARDWAREONLY
#warning "Warning: Building SerialCommand without SoftwareSerial Support"
#endif

#ifndef SERIALCOMMAND_HARDWAREONLY 
#include <SoftwareSerial.h>  
#endif

#include <string.h>


#define SERIALCOMMANDBUFFER 16
#define MAXSERIALCOMMANDS    10
#define MAXDELIMETER 2

#define SERIALCOMMANDDEBUG 1
#undef SERIALCOMMANDDEBUG      // Comment this out to run the library in debug mode (verbose messages)

class SerialCommand
{
    public:
        SerialCommand();      // Constructor
        #ifndef SERIALCOMMAND_HARDWAREONLY
        SerialCommand(SoftwareSerial &SoftSer);  // Constructor for using SoftwareSerial objects
        #endif

        void clearBuffer();   // Sets the command buffer to all '\0' (nulls)
        char *next();         // returns pointer to next token found in command buffer (for getting arguments to commands)
        void readSerial();    // Main entry point.  
        void addCommand(const char *, void(*)());   // Add commands to processing dictionary
        void addDefaultHandler(void (*function)());    // A handler to call when no valid command received. 
    
    private:
        char inChar;          // A character read from the serial stream 
        char buffer[SERIALCOMMANDBUFFER];   // Buffer of stored characters while waiting for terminator character
        int  bufPos;                        // Current position in the buffer
        char delim[MAXDELIMETER];           // null-terminated list of character to be used as delimeters for tokenizing (default " ")
        char term;                          // Character that signals end of command (default '\r')
        char *token;                        // Returned token from the command buffer as returned by strtok_r
        char *last;                         // State variable used by strtok_r during processing
        typedef struct _callback {
            char command[SERIALCOMMANDBUFFER];
            void (*function)();
        } SerialCommandCallback;            // Data structure to hold Command/Handler function key-value pairs
        int numCommand;
        SerialCommandCallback CommandList[MAXSERIALCOMMANDS];   // Actual definition for command/handler array
        void (*defaultHandler)();           // Pointer to the default handler function 
        int usingSoftwareSerial;            // Used as boolean to see if we're using SoftwareSerial object or not
        #ifndef SERIALCOMMAND_HARDWAREONLY 
        SoftwareSerial *SoftSerial;         // Pointer to a user-created SoftwareSerial object
        #endif
};

#endif //SerialCommand_h
