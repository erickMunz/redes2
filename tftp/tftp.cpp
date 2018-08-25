#include <bits/stdc++.h>
using namespace std;
class tftp
{
	public:
	string codigoOP;
    unsigned char *nombreArchivo;
    unsigned char *DATA;

	
	// printname is not defined inside class defination
	string getCodigoOP(unsigned char *trama);
    void setCodigoOP(unsigned char* trama);
    int formatoACK (unsigned char* trama);
	
	// printid is defined inside class defination

};

// Definition of printname using scope resolution operator ::
string tftp::getCodigoOP(unsigned char *trama)
{
    return this.codigoOP;
}

void tftp::setCodigoOP(unsigned char *trama){
    return
}
int main() {
	
	Geeks obj1;
	obj1.geekname = "xyz";
	obj1.id=15;
	
	// call printname()
	obj1.printname();
	cout << endl;
	
	// call printid()
	obj1.printid();
	return 0;
}
