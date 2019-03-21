#include "Task.h"

Task::Task(int ID, int release_time, int execution_time, int deadline, int period, int preemption, int type)
{
	this->ID = ID;
	this->release_time = release_time;
	this->execution_time = execution_time;
	this->deadline = deadline;
	this->period = period;
	this->preemption = preemption;
	this->type = type;
}

void Task::printData()
{
	std::cout<<"Task "<<ID<<" : release time "<<release_time<<", execution time "<<execution_time<<", deadline "<<deadline<<", period "<<period<<", preemption "<<preemption<<", type "<<type<<std::endl;
}
	
