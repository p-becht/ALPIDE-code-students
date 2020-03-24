//
//  USB.h
//  pALPIDEfs-software
//
//  Created by Carlo Puggioni and Markus Keil on 18/03/14.
//  Copyright (c) 2014 Carlo Puggioni, Markus Keil. All rights reserved.
//

#ifndef __pALPIDEfs_software__USB__
#define __pALPIDEfs_software__USB__

#include <iostream>
#include <string>
#include <vector>
//#include "libusb.h"
#include <libusb-1.0/libusb.h>

class TUSBEndpoint;


//enum  TEndPointType      {Control, Interrupt, Bulk, Iso};
//enum  TEndpointDirection {In, Out};
class TUSBBoard;


class TUSBEndpoint {
private:
protected:
    TUSBBoard            *fMyBoard;
    libusb_device_handle *fHandle;
    libusb_transfer_type  fType;
    libusb_endpoint_direction    fDirection;
    unsigned char         fEndpointAddress; // Address of endpoint
    unsigned int          fMaxPacketSize;   // Maximum Packet Size
    unsigned int          fTimeout;

    int                   SetEndpointInfo (const libusb_endpoint_descriptor *desc);
    void                  Init            ();
    virtual int           TransferData    (unsigned char *data_buf, int packetSize, int* error=0x0);
public:
    TUSBEndpoint (TUSBBoard *ABoard, const libusb_endpoint_descriptor *desc);
    int                SendData         (unsigned char *data_buf, int packetSize, int* error=0x0);
    int                ReceiveData      (unsigned char *data_buf, int packetSize, int* error=0x0);
    void               DumpEndpointInfo ();
	libusb_transfer_type      GetType          () {return fType;};            // Return Type of endpoint.
	libusb_endpoint_direction GetDirection     () {return fDirection;};       // Return Direction of endpoint.
	unsigned int       GetAddress       () {return fEndpointAddress;}; // Return address of endpoint.
	unsigned int       GetMaxPacketSize () {return fMaxPacketSize;};   // Return Maximum packet size.
    bool               IsBulk           () {return fType == LIBUSB_TRANSFER_TYPE_BULK;};
    bool               IsControl        () {return fType == LIBUSB_TRANSFER_TYPE_CONTROL;};
    bool               IsIso            () {return fType == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS;};
    bool               IsInterrupt      () {return fType == LIBUSB_TRANSFER_TYPE_INTERRUPT;};
};



class TUSBBoard {
private:
    libusb_device        *fDevice;
    libusb_device_handle *fHandle;
    int                   fBusNum;        // Bus Number
    int                   fDevNum;        // Device Number
    uint16_t              fVendorId;      // Id Vendor
    uint16_t              fProductId;     // Id Product
    std::string           fUsbChipType;   // FX3 or FX2 or OTHER
    int                   fNumInterface;  // Number of Interface
    int                   fNumEndpoints;  // Number of Endpoints.
    void                  Init                ();
    int                   ConnectUSBDevice    ();
    int                   DisconnectUSBDevice ();
    int                   SetDeviceInfo       ();
    void                  CreateEndpoints     ();
    int                   GetBusNumber()      {return fBusNum;};       // Return Bus Number.
    int                   GetDevNumber()      {return fDevNum;};       // Return Device Number.
    uint16_t              GetIdVendor()       {return fVendorId;};     // Return Id Vendor.
    uint16_t              GetIdProduct()      {return fProductId;};    // Return Id Product.
    std::string           GetChipType()       {return fUsbChipType;};  // Return type of device (FX3, FX2, OTHER)
    int                   GetNumInterface()   {return fNumInterface;}; // Return number of Interface
    int                   GetNumEndpoints()   {return fNumEndpoints;}; // Return number of Endpoints

    // GetEndpointType Return Type of endpoint (CONTROL, INTERRUPT, BULK, ISOCHRONUS).
	libusb_transfer_type      GetEndpointType(int id) {return fEndpoints.at(id)->GetType();};
	//GetEndpointDirection Return Direction of endpoint(IN, OUT).
	libusb_endpoint_direction GetEndpointDirection(int id)         {return fEndpoints.at(id)->GetDirection();};
	//GetEndpointAddress Return address of endpoint.
	unsigned int GetEndpointAddress(int id)      {return fEndpoints.at(id)->GetAddress();};
	//GetEndpointMaxPacketSize Return Maximum packet size.
	unsigned int GetEndpointMaxPacketSize(int id){return fEndpoints.at(id)->GetMaxPacketSize();};

protected:
    std::vector <TUSBEndpoint *> fEndpoints; // Vector that contains the endpoints object.
public:
    TUSBBoard             (libusb_device *ADevice);
    ~TUSBBoard            ();
    libusb_device_handle *GetHandle   ()      {return fHandle;};
    TUSBEndpoint         *GetEndpoint (int i) {return fEndpoints.at(i);};
    void                  DumpDeviceInfo      () const;
    //int                   TransferData        (int idEndpoint, unsigned char * data_buf, int packetSize, int* error=0x0);
    int                   SendData            (int idEndpoint, unsigned char * data_buf, int packetSize, int* error=0x0) const;
    int                   ReceiveData         (int idEndpoint, unsigned char * data_buf, int packetSize, int* error=0x0) const;
};




class TUSBEndpointBulk : public virtual TUSBEndpoint {
private:
protected:
    int TransferData (unsigned char *data_buf, int packetSize, int* error=0x0);
public:
    TUSBEndpointBulk (TUSBBoard *ABoard, const libusb_endpoint_descriptor *desc);
};



class TUSBEndpointControl : public virtual TUSBEndpoint {
private:
protected:
public:
    TUSBEndpointControl (TUSBBoard *ABoard, const libusb_endpoint_descriptor *desc);
    int GetConfiguration();
};




#endif /* defined(__pALPIDEfs_software__USB__) */
