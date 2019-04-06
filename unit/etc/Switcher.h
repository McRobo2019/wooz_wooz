
#ifndef SWITCHER_H_
#define SWITCHER_H_

class Switcher {

	public:
		explicit Switcher() { status = 0; }
		virtual int isPressed() = 0;
		virtual int on() = 0;
		virtual int off() = 0;
		virtual int waitPressed() = 0;
 		int isOn() { return status; }

	protected:
		int status;
};

#endif // SWITCHER_H_
