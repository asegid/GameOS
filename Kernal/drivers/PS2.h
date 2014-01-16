#define PS2_READ_FROM_BYTE_0 0x20
#define PS2_WRITE_TO_BYTE_0 0x60
#define TEST_PS2_CONTROLLER 0xAA
#define PS2_CONTROLLER_TEST_PASSED 0x55
#include "asmio.h"
#define PS2_CONTROLLER_TEST_FAILED 0xFC
#define PS2_ENABLE_FIRST_PORT 0xAE
#define PS2_ENABLE_SECOND_PORT 0xA8
#define PS2_DISABLE_FIRST_PORT 0xAD
#define PS2_DISABLE_SECOND_PORT 0xA7
const uint16_t PS2_CONTROLLER_STATE = 0x64;
const uint16_t PS2_CONTROLLER_DATA = 0x60;
uint8_t PS2_PORT_COUNT = 0;
void (*writestring)(const char * data);
inline uint8_t PS2_READ_FROM_OFFSET(uint8_t offset){
	uint8_t result = offset + 0x21;
	if(result > 0x3f){
		result = 0x3f;
	}
	return result;
}
inline uint8_t PS2_WRITE_FROM_OFFSET(uint8_t offset){
	uint8_t result = offset + 0x61;
	if(result > 0x7f){
		result = 0x7f;
	}
}
void PS2_CONTROLLER_SELF_TEST(){
	//clear the buffer to prevent incorrect test data
	inportb(PS2_CONTROLLER_DATA);
	(*writestring)("Performing PS/2 controller self test");
	outportb(PS2_CONTROLLER_STATE, TEST_PS2_CONTROLLER);
	uint8_t result = 0;
	while(!(inportb(PS2_CONTROLLER_STATE) & 0b00000001)){
		(*writestring)(".");
	}
	(*writestring)("\n");
	result = inportb(PS2_CONTROLLER_DATA);
	if(result == PS2_CONTROLLER_TEST_PASSED){
		(*writestring)("The PS/2 controller appears to be working");
	}else if(result == PS2_CONTROLLER_TEST_PASSED){
		(*writestring)("The PS/2 controller does not appear to be working");
		(*writestring)("Will now halt...");
		hang();
	}else{
		(*writestring)("The PS/2 controller did not return a valid code");
		(*writestring)("Will now halt...");
		hang();
	}
	(*writestring)("\n");
}
void PS2_CONTROLLER_INIT(){
	PS2_CONTROLLER_SELF_TEST();
	outportb(PS2_CONTROLLER_STATE, PS2_READ_FROM_BYTE_0);
	uint8_t configuration_byte = inportb(PS2_CONTROLLER_DATA);
	if(configuration_byte & 0b00100000){
		PS2_PORT_COUNT = 2;
		(*writestring)("There are two PS/2 ports on this machine\n");
	}else{
		(*writestring)("There is one PS/2 port on this machine\n");
		PS2_PORT_COUNT = 1;
	}
	configuration_byte = configuration_byte & 0b10111100;
	outportb(PS2_CONTROLLER_STATE, PS2_WRITE_TO_BYTE_0);
	outportb(PS2_CONTROLLER_DATA, configuration_byte);
	inportb(PS2_CONTROLLER_DATA);
	outportb(PS2_CONTROLLER_STATE, PS2_ENABLE_FIRST_PORT);
	(*writestring)("PS/2 port one (assumed keyboard) has been enabled\n");
	if(PS2_PORT_COUNT == 2){
		outportb(PS2_CONTROLLER_STATE, PS2_ENABLE_SECOND_PORT);
		(*writestring)("PS/2 port two (assumed mouse) has been enabled\n");
	}
}

