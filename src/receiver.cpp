#include "receiver.h"

#include <iostream>

using namespace std;
using namespace pd;

void receiver::print(const std::string& message) {
	cout << message << endl;
}

void receiver::receiveBang(const std::string& dest) {
	cout << "CPP: bang " << dest << endl;
}

void receiver::receiveFloat(const std::string& dest, float num) {
	
	if(dest=="MIDI"){
		pitch = num;
	}
	else if(dest=="VELOCITY"){
		velocity = num;
	}
	cout << "CPP: float " << dest << ": " << num << endl;
}