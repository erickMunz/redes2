#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
using namespace std;


// operaciones
/*
opcode  operation
            1     Read request (RRQ)
            2     Write request (WRQ)
            3     Data (DATA)
            4     Acknowledgment (ACK)
            5     Error (ERROR)*/

class tftp
{
	public:
	short codigoOP;
    unsigned char *nombreArchivo;
    unsigned char *DATA[512];
	int modo;
	short secuencia;
	unsigned char *mensaje;
	short codigoError;

	
	// printname is not defined inside class defination
	short getCodigoOP();
    void setCodigoOP(unsigned char* trama);
	void ACK(unsigned char* trama);
	void RRQ(unsigned char* trama);
	void WRQ(unsigned char* trama);
	void setDATA(unsigned char* trama);
	void ERROR(unsigned char* trama);
    int formatoACK (unsigned char* trama);
	int formatoRRQ(unsigned char * trama);
	int formatoWRQ(unsigned char * trama);
	int formatoDATA(unsigned char * trama);
	int formatoERROR(unsigned char * trama);


	
	
	// printid is defined inside class defination

};

// Definition of printname using scope resolution operator ::
short tftp::getCodigoOP()
{
    return this->codigoOP;
}

void tftp::setCodigoOP(unsigned char *trama){
    this->codigoOP=trama[0];
	this->codigoOP <<= 8;
	this->codigoOP += trama[1];	
	printf("%d", this->codigoOP);
}

void tftp::ERROR(unsigned char *trama){
	this->codigoError = trama[2];
	this->codigoError <<=8;
	this->codigoError += trama[3];

	strcpy((char* )this->mensaje, (const char *) trama+4);
	printf("%s ", this->mensaje);
}
void tftp::ACK(unsigned char *trama){
	this->secuencia = trama[2];
	this->secuencia <<=8;
	this->secuencia += trama[3];
}

int tftp::formatoACK(unsigned char *trama){
	trama[0] = 0;
	trama[1] = 4;
	
	trama[2] = (this->secuencia & 0xff00) >> 8;
	trama[3] = this->secuencia & 0x00ff;
	
	return 4;
}

int tftp::formatoERROR(unsigned char *trama){

	int tam = strlen((char *) this->mensaje);
	trama[0] = 0;
	trama[1] = 5;
	trama[2] = (this->codigoError& 0xf0) >> 8;
	trama[3] = this->codigoError& 0x0f;
	strcpy((char *)trama+4,(const char *) this->mensaje);
	return 4+tam;
}
void tftp::setDATA(unsigned char * trama){
	int b_size = sizeof(trama);
	this->secuencia = trama [2];
	this->secuencia <<= 8;
	this->secuencia += trama[3];

	//memcpy(data.data, buffer+4, b_size-4);
	for(int  i = 0; i<b_size-4; i++){
		//printf("%hu ", buffer[i+4]);
		this->DATA[i] = (unsigned char*)trama[i+4];
	}
}
/*
int tftp::formatoDATA(unsigned char *trama){

	int tam = sizeof(this->DATA);
	trama[0] = 0;
	trama[1] = 3;
	
	trama[2] = (this->secuencia  >> 8) & 0x00ff;
	trama[3] = this->secuencia & 0x00ff;


	//memcpy(buffer+4, data.data, data_data_len);
	//printf("Filling with %d\n", data_data_len);
	for(int  i = 4; i<tam+4; i++){
		//printf("%hu ", data.data[i-4]);
		unsigned char *dat = this->DATA[i-4];
		trama[i] =  reinterpret_cast<unsigned  char>(dat);
	}
	
	return tam + 4;
}*/
