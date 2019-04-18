#include "Processor.h"

Processor::Processor(int ID, int pow)
{
	this->ID = ID;
	this->pow = pow;
}

Processor::printData()
{
	cout<<"Processor "<<ID<<" : ability "<<pow<<endl;
}
