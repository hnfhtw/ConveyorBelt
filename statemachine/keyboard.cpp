#include <stdio.h>
#include <sysLib.h>
#include <stdLib.h>
#include "keyboard.h"
extern "C"{
	#include "kbd.h"
}
int keyCount;

Keyboard :: Keyboard() {
	printf("Keyboard Konstruktor!\n\r");	
	return;
}

Keyboard :: ~Keyboard() {
	pressedKey = 0;
	return;
}

char Keyboard::getPressedKey( )
{
	char pressedKey = getKey();
	return pressedKey;
}
