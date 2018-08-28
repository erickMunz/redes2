#include <bits/stdc++.h>
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
    unsigned char *DATA;
	int modo;
	short secuencia;
	unsigned char *mensaje;
	short codigoError;

	
	// printname is not defined inside class defination
	short getCodigoOP(unsigned char *trama);
    void setCodigoOP(unsigned char* trama);
	void ACK(unsigned char* trama);
	void RRQ(unsigned char* trama);
	void WRQ(unsigned char* trama);
	void DATA(unsigned char* trama);
	void ERROR(unsigned char* trama);
    int formatoACK (unsigned char* trama);
	int formatoRRQ(unsigned char * trama);
	int formatoWRQ(unsigned char * trama);
	int formatoDATA(unsigned char * trama);
	int formatoERROR(unsigned char * trama);


	
	
	// printid is defined inside class defination

};

// Definition of printname using scope resolution operator ::
short tftp::getCodigoOP(unsigned char *trama)
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

	strcpy(this->mensaje, trama+4);
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

	int tam = strlen(this->mensaje);
	trama[0] = 0;
	trama[1] = 5;
	trama[2] = (this->mensaje & 0xf0) >> 8;
	trama[3] = this->mensaje& 0x0f;
	strcpy(trama+4, this->mensaje);
	return 4+tam;
}
int main() {
	
	tftp obj1;

	return 0;
}
