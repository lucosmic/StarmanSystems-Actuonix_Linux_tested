#ifndef _FIRGELLI_H
#define _FIRGELLI_H

#include <libusb-1.0/libusb.h>

class Firgelli{
public:
	Firgelli();
	~Firgelli();
	void SetDebug(int m);
	void Open(int rank);
	int WriteCode(int location, int value);
	void Info();

private:
	int m_interface;
	libusb_device_handle *m_handle;
	struct libusb_context *m_ctx;
	static const char *ControlType(int attr);
	static int m_debug;
};
#endif
