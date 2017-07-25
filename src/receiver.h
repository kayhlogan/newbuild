#pragma once

#include "PdBase.hpp"

// custom receiver class
class receiver : public pd::PdReceiver{
	public:
		float pitch;
		float velocity;
		
		// pd message receiver callbacks
		void print(const std::string& message);
		void receiveBang(const std::string& dest);
		void receiveFloat(const std::string& dest, float num);
		int currentPitch() {return pitch;}
		int currentVelocity() {return velocity;}
};