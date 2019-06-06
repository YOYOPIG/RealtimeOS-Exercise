#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <iostream>

class Processor{
	public:
		Processor(int ID, int pow);
		void printData();
		int ID;
		int pow;
};

//try move to .cpp
Processor::Processor(int ID, int pow)
{
	this->ID = ID;
	this->pow = pow;
}

void Processor::printData()
{
	std::cout<<"Processor "<<ID<<" : ability "<<pow<<std::endl;
}

#endif
