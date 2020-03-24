//
//  USB.cpp
//  pALPIDEfs-software
//
//  Created by Carlo Puggioni and Markus Keil on 18/03/14.
//  Copyright (c) 2014 Carlo Puggioni, Markus Keil. All rights reserved.
//

#include "USB.h"
#include "TTestsetup.h"
#include <stdlib.h>
#include <unistd.h>

////////////////////////////////////////////////////////////////////////
//                                                                    //
//                       class TUSBBoard                              //
//                                                                    //
////////////////////////////////////////////////////////////////////////

TUSBBoard::TUSBBoard(libusb_device *ADevice) {
    // open device and get handle;
    fDevice = ADevice;
    if (fDevice) {
        Init             ();
        ConnectUSBDevice ();
        SetDeviceInfo    ();
        CreateEndpoints  ();
    }
}


TUSBBoard::~TUSBBoard() {
    if (fDevice) DisconnectUSBDevice();
}

/*
 * Initialize the values of the object variables.
 */
void TUSBBoard::Init() {
    fBusNum       = 0;
    fDevNum       = 0;
    fVendorId     = 0;
    fProductId    = 0;
    fUsbChipType  = "";
    fNumInterface = 0;
    fNumEndpoints = 0;
}


/*
 * Establish the connection with device and interface.
 * Return:    -1 if the operation failed          1 if it is ok.
 */
int TUSBBoard::ConnectUSBDevice(){
    int err = 0;

	//open the device handle for all future operations
    err = libusb_open(fDevice, &fHandle);
    if (err)
    {
        std::cout << "The device is not opening, check the error code" << std::endl;
        return -1;
    }

    // While claiming the interface, interface 0 is claimed since from our bulkloop firmware we know that.
    err = libusb_claim_interface (fHandle, INTERFACE_NUMBER);
    if (err)
    {
        std::cout << "The device interface " << INTERFACE_NUMBER << " is not getting accessed, HW/connection fault, err = " << err << std::endl;
        return -1;
    }
    return 1;
}


/*
 * Disconnect the interface and the device .
 * Return:    -1 if the operation failed          1 if it is ok.
 */
int TUSBBoard::DisconnectUSBDevice(){
	int err;
    err = libusb_release_interface (fHandle, INTERFACE_NUMBER);
    if(err)
    {
        std::cout << "The device interface is not getting released, if system hangs please disconnect the device" << std::endl;
        return -1;
    }
	libusb_close(fHandle);
	return 1;
}



/*
 * Get the information about device and set it in the variables of object.
 * Parameter: Device USB
 * Return:    -1 if the operation failed          1 if it is ok.
 * fBusNum contains the number of bus.
 * fDevNum contains the address of device in the bus.
 * fIdVendor contains the number of identification of Vendor.
 * fIdProduct contains the number of identification of Product.
 * fChipUsbType contains the values (FX3,FX2 or OTHER)
 * This value is calculated using the values of fIdVendor and fIdProduct.
 * fNumInterface contains the number of interface. (for us must be 1)
 * fNumEndpoints contains the number of endpoints.
 * fEndpoints_vec It is the vector that contains the object of type TUSBEndpoint.
 */
int TUSBBoard::SetDeviceInfo(){
    struct libusb_device_descriptor           desc;
	struct libusb_config_descriptor          *config;



    int r = libusb_get_device_descriptor(fDevice, &desc);
    if (r < 0)
    {
        std::cout << "Failed to get device descriptor for the device" << std::endl;
        return -1;
    }

	fBusNum    = libusb_get_bus_number     (fDevice);
	fDevNum    = libusb_get_device_address (fDevice);
	fVendorId  = desc.idVendor;
	fProductId = desc.idProduct;

	if ((fVendorId == 0x04b4) && (fProductId == 0x00F0 || fProductId == 0x00F1))
	    fUsbChipType="FX3";
	else if ((fVendorId == 0x04b4) && (fProductId == 0x1004))
	    fUsbChipType="FX2";
	else {
        fUsbChipType="OTHER";
        std::cout << "Error: the device selected is not correct" << std::endl;
	    return -1;
	}

	libusb_get_config_descriptor(fDevice, 0, &config);
	fNumInterface = (int) config    -> bNumInterfaces;
	return 1;
}



void TUSBBoard::CreateEndpoints() {
	struct       libusb_config_descriptor    *config;
    const struct libusb_interface            *inter;
    const struct libusb_interface_descriptor *interdesc;
    const struct libusb_endpoint_descriptor  *epdesc;
    uint8_t                                   epattrib;
    libusb_transfer_type                      ttype;

	libusb_get_config_descriptor(fDevice, 0, &config);

    inter         = &config         -> interface[0];
	interdesc     = &inter          -> altsetting[0];
	fNumEndpoints = (int) interdesc -> bNumEndpoints;


	for (int k=0; k<fNumEndpoints; k++){
		epdesc   = &interdesc -> endpoint[k];
        epattrib = epdesc     -> bmAttributes;
        ttype    = (libusb_transfer_type) (epattrib & 0x3);
        switch (ttype) {
            case LIBUSB_TRANSFER_TYPE_BULK:
                fEndpoints.push_back(new TUSBEndpointBulk(this, epdesc));
                break;
            case LIBUSB_TRANSFER_TYPE_CONTROL:
                fEndpoints.push_back(new TUSBEndpointControl(this, epdesc));
                break;
            default:
                std::cout << "Unknown Endpoint type" << std::endl;
        }
    }
}


/*
 * Parameter:   IdEndpoint is a Identify number of endpoint.
 *              Data_buf is the buffer of data.
 *              packetSize is the size of the packet.
 * Return:  -1 if the operation failed.
 *           Number of byte transfered if the operation is ok.
 * It used the idEndpoint to access to endpointObject.
 * It call the method DataTransfer defined from TEndpoint class.
 */
//int TUSBBoard::TransferData(int idEndpoint,unsigned char * data_buf, int packetSize, int* error /*=0x0*/){
//	int num_byte_tr=0;
//	num_byte_tr=fEndpoints.at(idEndpoint)->TransferData(data_buf, packetSize, error);
//	return num_byte_tr;
//}


int TUSBBoard::SendData(int idEndpoint,unsigned char * data_buf, int packetSize, int* error /*=0x0*/) const {
  int    num_byte_tr = fEndpoints.at(idEndpoint)->SendData(data_buf, packetSize, error);
  return num_byte_tr;
}


int TUSBBoard::ReceiveData(int idEndpoint,unsigned char * data_buf, int packetSize, int* error /*=0x0*/) const {
  int    num_byte_tr = fEndpoints.at(idEndpoint)->ReceiveData(data_buf, packetSize, error);
  return num_byte_tr;
}


void TUSBBoard::DumpDeviceInfo() const {
    std::cout << "***** Information about USB Device *****" <<std::endl;
    std::cout << "   Number of Bus:          " << fBusNum << std::endl;
    std::cout << "   Address of Device:      " << fDevNum << std::endl;
    std::cout << "   Id Vendor:              " << fVendorId << std::endl;
    std::cout << "   Id Product:             " << fProductId << std::endl;
    std::cout << "   Type of Device:         " << fUsbChipType << std::endl;
    std::cout << "   Number of Interfaces:   " << fNumInterface << std::endl;
    std::cout << "   Number of Endpoints:    " << fNumEndpoints << std::endl;
	if (fUsbChipType == "FX3"){
		for (int i=0; i< fNumEndpoints; i++){
            std::cout << "   Endpoint " << i << ":" << std::endl;
			fEndpoints.at(i)->DumpEndpointInfo();
		}
	}
}

//---------------------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////
//                                                                    //
//                       class TUSBEndpoint                           //
//                                                                    //
////////////////////////////////////////////////////////////////////////


TUSBEndpoint::TUSBEndpoint(TUSBBoard *ABoard, const libusb_endpoint_descriptor *desc) {
    fMyBoard = ABoard;
    fHandle  = fMyBoard->GetHandle();
    Init();
    SetEndpointInfo(desc);
}


/*
 * Initialize the values object variables.
 */
void TUSBEndpoint::Init() {
	fType            = (libusb_transfer_type) -1;
	fDirection       = (libusb_endpoint_direction) -1;
	fEndpointAddress = 0;
	fMaxPacketSize   = 0;
	fTimeout         = 120000; // milliseconds
    //fTimeout         = 0; // milliseconds
}


/*
 * Parameter: Endpoint Descriptor
 * Return:    -1 if the operation failed          1 if it is ok.
 * Used the descriptor of Usb Endpoint to get the information about endpoint
 * and set it in the variables of object.
 * fAddress_char contains the address of endpoint in char format. It used in the method DataTransfer.
 * fAddress contains the address of endpoint in int format. It used to set the type and the direction of endpoint.
 * The values 0x81 and 0x1 can be change with the firmware. It is just a example.
 */
int TUSBEndpoint::SetEndpointInfo(const libusb_endpoint_descriptor *desc) {
    uint8_t parameters;

    fEndpointAddress = desc->bEndpointAddress;
	fMaxPacketSize   = desc->wMaxPacketSize;

    fDirection       = (libusb_endpoint_direction) (fEndpointAddress  & 0x80);
    parameters = desc->bmAttributes;
    fType = (libusb_transfer_type) (parameters & 3);

	return 1;
}


/*
 * Dump the information about endpoint.
 */
void TUSBEndpoint::DumpEndpointInfo () {
    std::cout << "     Type:                 " << fType            << std::endl;
    std::cout << "     Direction:            " << fDirection       << std::endl;
    std::cout << "     Address:              0x" << std::hex << (int)fEndpointAddress << std::dec << std::endl;
    std::cout << "     Maximum Packet Size:  " << fMaxPacketSize   << std::endl;
}


int TUSBEndpoint::TransferData     (unsigned char *data_buf, int packetSize, int* error /*=0x0*/) {
    std::cout << "Transfer Data not yet implemented for this type of endpoint " << std::endl;
    return -1;
}



int TUSBEndpoint::SendData (unsigned char *data_buf, int packetSize, int* error /*=0x0*/) {

    if (fDirection == LIBUSB_ENDPOINT_IN) {
        std::cout << "Warning, trying to write to in-endpoint. Doing nothing ... " << std::endl;
        return -1;
    }
    else {
      int Result = TransferData(data_buf, packetSize, error);
        return Result;
    }

}


int TUSBEndpoint::ReceiveData (unsigned char *data_buf, int packetSize, int* error /*=0x0*/) {
    if (fDirection == LIBUSB_ENDPOINT_OUT) {
        std::cout << "Warning, trying to read from out-endpoint. Doing nothing ... " << std::endl;
        return -1;
    }
    else return TransferData(data_buf, packetSize, error);
}

//---------------------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////
//                                                                    //
//                    class TUSBEndpointBulk                          //
//                                                                    //
////////////////////////////////////////////////////////////////////////


TUSBEndpointBulk::TUSBEndpointBulk(TUSBBoard *ABoard, const libusb_endpoint_descriptor *desc)
: TUSBEndpoint(ABoard, desc) {
    fType = LIBUSB_TRANSFER_TYPE_BULK;
}


int TUSBEndpointBulk::TransferData (unsigned char *data_buf, int packetSize,  int* error /*=0x0*/) {
    int err;
    int num_byte_transferred = 0;

    err = libusb_bulk_transfer(fHandle, fEndpointAddress, data_buf, packetSize, &num_byte_transferred, fTimeout);
    if (error) *error = err;
    if (err==LIBUSB_ERROR_TIMEOUT) {
        return num_byte_transferred;
    }
    if (err){
        std::cout << "Error: The transfer of data with endpoint "<< std::hex << (int)fEndpointAddress  <<" failed with error " << std::dec << err << std::endl;
        TDAQBoard *db = (TDAQBoard *) fMyBoard;
        std::cout << "Board address = " << db->GetBoardAddress() << std::endl;
        if (err == -7) {
            std::cout << "trying to clear halt condition, returns" << libusb_clear_halt(fHandle, fEndpointAddress) << std::endl;
        }
        return -1;
    }
    if (num_byte_transferred != packetSize) {
//        std::cout << "Warning, num_byte_transferred != packetSize. transferred: " << num_byte_transferred << " packetSize: " << packetSize << std::endl;
    }

    return num_byte_transferred;
}

//---------------------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////
//                                                                    //
//                    class TUSBEndpointControl                       //
//                                                                    //
////////////////////////////////////////////////////////////////////////


TUSBEndpointControl::TUSBEndpointControl(TUSBBoard *ABoard, const libusb_endpoint_descriptor *desc)
: TUSBEndpoint(ABoard, desc) {
    fType = LIBUSB_TRANSFER_TYPE_CONTROL;
}



int TUSBEndpointControl::GetConfiguration() {
    uint8_t request_type = LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_STANDARD | LIBUSB_RECIPIENT_DEVICE;
    uint8_t request      = LIBUSB_REQUEST_GET_CONFIGURATION;
    unsigned char data[1];
    int size = libusb_control_transfer(fHandle, request_type, request, 0, 0, data, 1, 1000);
    std::cout << "Received data 0x" << std::hex << (int) data[0] << std::dec << std::endl;
    return size;

}
