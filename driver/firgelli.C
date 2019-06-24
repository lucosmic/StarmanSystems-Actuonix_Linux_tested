#include "firgelli.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int Firgelli::m_debug;


Firgelli::Firgelli()
{
	m_ctx=NULL;
	m_handle=NULL;
	m_interface=0;
	libusb_init(&m_ctx);
}

Firgelli::~Firgelli()
{
	int rval=libusb_release_interface(m_handle,m_interface);
	assert(rval==0);
	
	if(m_handle)
		libusb_close(m_handle);
	libusb_exit(m_ctx);
}

void Firgelli::SetDebug(int m)
{
	libusb_set_debug(m_ctx,m);
	m_debug=m;
}

void Firgelli::Open(int rank)
{
	int rval;
	uint16_t vid=0x04d8;		// Microtech
	uint16_t pid=0xfc5f;		// ??
	
#ifdef SIMPLE
	m_handle=libusb_open_device_with_vid_pid(m_ctx,vid,pid);
	if(m_handle==NULL){
		fprintf(stderr,"firgelli::open device vid=0x%x pid=0x%x not found\n",
			vid,pid);
		exit(1);
	}
#else
	libusb_device **devs;
	int count=libusb_get_device_list(m_ctx,&devs);
	if(m_debug)
		printf("number of usb devices=%d\n",count);
	if(count == 0){
		fprintf(stderr,"no device found\n");
		exit(1);
	}
	if(count < 0){
		fprintf(stderr,"device get error\n");
		exit(1);
	}
	int nfound=0;
	libusb_device *found=NULL;
	for(int i=0; i<count; i++){
		libusb_device *dev=devs[i];
		libusb_device_descriptor ddesc;
		int rval=libusb_get_device_descriptor(dev,&ddesc);
		if(rval<0){
			fprintf(stderr,"device descriptor get error\n");
			exit(1);
		}
		if((ddesc.idVendor == vid) && (ddesc.idProduct == pid)){
			nfound++;
			if (nfound==rank)
				found=dev;
		}
	}
	if(m_debug)
		printf("number of lacs=%d\n",nfound);
	if(found==NULL){
		fprintf(stderr,"device rank=%d not found\n",rank);
		exit(1);
	}
	rval=libusb_open(found,&m_handle);
	assert(rval==0);	
#endif
	rval=libusb_claim_interface(m_handle,m_interface);
	assert(rval==0);
}

#define NBUF	80

void Firgelli::Info()
{
//	printf("version=%d\n",libusb_get_version());

	int rval=libusb_kernel_driver_active(m_handle,m_interface);
	printf("kernel driver active=%d\n",rval);

//	char buf[NBUF];
//	rval=libusb_get_string_descriptor_ascii(m_handle,

	libusb_device *device=libusb_get_device(m_handle);
	int busno=libusb_get_bus_number(device);	
	int address=libusb_get_device_address(device);	
	printf("busno=%d address=%d\n",busno,address);

	libusb_device_descriptor ddesc;
	rval=libusb_get_device_descriptor(device,&ddesc);
	if(rval < 0 ){
		fprintf(stderr,"firgelli: bad descriptor\n");
		exit(1);
	}
	printf("number of configurations=%d\n",ddesc.bNumConfigurations);
	printf("serial number=%d\n",ddesc.iSerialNumber);
	printf("productid=0x%x\n",ddesc.idProduct);

	int c;
	rval=libusb_get_configuration(m_handle,&c);
	assert(rval==0);
	printf("config=%d\n",c);

	libusb_config_descriptor *cdesc;
	rval=libusb_get_config_descriptor(device,0,&cdesc);
	assert(rval==0);
	printf("number of interfaces=%d\n",cdesc->bNumInterfaces);
	struct libusb_interface interface=cdesc->interface[0];
	const struct libusb_interface_descriptor *idex=interface.altsetting;

	int nend=idex->bNumEndpoints;
	printf("nend=%d\n",nend);
	const struct libusb_endpoint_descriptor *ends=idex->endpoint;
	for(int i=0; i<nend; i++){
		int eaddr=ends[i].bEndpointAddress;
		int attr=ends[i].bmAttributes;
		printf("addr=%d attr=0x%x %s\n",eaddr,attr,ControlType(attr));
	}

	libusb_free_config_descriptor(cdesc);
}

int Firgelli::WriteCode(int location, int value)
{
	int transferred=0;
	unsigned int timeout=1000;
	unsigned char buf[3];

	buf[0]=location;
	buf[1]=value%256;	// Low
	buf[2]=value/256;	// High

	unsigned char endpoint=1 | LIBUSB_ENDPOINT_OUT; 

	int rval=libusb_bulk_transfer(
		m_handle,
		endpoint,	
		buf,
		3,
		&transferred,
		timeout);
	if(m_debug){
		printf("location=0x%x value=0x%x endpoint=%d transferred=%d\n",
		location,value,endpoint,transferred);
		printf("buf[0]=0x%x buf[1]=0x%x buf[2]=0x%x\n",
			buf[0], buf[1],buf[2]);
	}
	if(rval ){
		fprintf(stderr,"firgelli: bad write rval=%d\n",rval);
		exit(1);
	}

	endpoint=1 | LIBUSB_ENDPOINT_IN;
	rval=libusb_bulk_transfer(
		m_handle,
		endpoint,	
		buf,
		3,
		&transferred,
		timeout);
	if(m_debug)
		printf("location=0x%x endpoint=%d transferred=%d\n",
			location,endpoint,transferred);

	if(rval ){
		fprintf(stderr,"firgelli: bad read rval=%d \n",
			rval);
		exit(1);
	}
	if(m_debug)
		printf("buf[0]=0x%x buf[1]=0x%x buf[2]=0x%x\n",
		buf[0], buf[1],buf[2]);
	
	return buf[1]+256*buf[2];

}

const char *Firgelli::ControlType(int attr)
{
	attr=attr&0x03;		// mask bits 1 and 2
	switch(attr){
		case LIBUSB_TRANSFER_TYPE_CONTROL:
			return "control";
		case LIBUSB_TRANSFER_TYPE_BULK:
			return "bulk";
	}
	return "unknown";
}
