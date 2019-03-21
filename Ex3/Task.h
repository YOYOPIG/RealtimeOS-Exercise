#ifndef TASK_H
#define TASK_H

class Task{
	public:
		Task();
		Task(int ID, int release_time, int execution_time, int deadline, int period, int preemption, int type);
		void printData();
		void setPreempt();
		bool isPreempted() const;
		int ID;
		int release_time;
		int execution_time;
		int deadline;
		int period;
		int preemption;
		int type;
	private:
		bool preempted;
};

//try move to .cpp
Task::Task()
{
	this->ID = 0;
	this->release_time = 0;
	this->execution_time = 0;
	this->deadline = 0;
	this->period = 0;
	this->preemption = 0;
	this->type = 0;
	this->preempted = false;
}

Task::Task(int ID, int release_time, int execution_time, int deadline, int period, int preemption, int type)
{
	this->ID = ID;
	this->release_time = release_time;
	this->execution_time = execution_time;
	this->deadline = deadline;
	this->period = period;
	this->preemption = preemption;
	this->type = type;
	this->preempted = false;
}

void Task::printData()
{
	std::cout<<"Task "<<ID<<" : release time "<<release_time<<", execution time "<<execution_time<<", deadline "<<deadline<<", period "<<period<<", preemption "<<preemption<<", type "<<type<<std::endl;
}

void Task::setPreempt()
{
	this->preempted = true;
}

bool Task::isPreempted() const
{
	return preempted;
}
		

#endif
