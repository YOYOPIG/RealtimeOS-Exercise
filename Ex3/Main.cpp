#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "Processor.h"
#include "Task.h"
#include "Scheduler.h"
using namespace std;

vector<Processor> processors;
vector<Task> tasks;


/*
Reads input file then prints CPU and task data 
*/
void readFile()
{
	int m, n;
	fstream file;
	file.open("input.txt", ios::in);
	if(!file)     //Check if success
    {
            cerr<<"ERROR : Can't open file!"<<endl;
            exit(1);
    }
    file>>m>>n;
    
    //read cpu data
	for(int i=0;i<m;++i)
	{
		int id, pow;
		file>>id>>pow;
		Processor *cpu = new Processor(id, pow);
		processors.push_back(*cpu);
	}
	
	//read task data
	for(int i=0;i<n;++i)
	{
		int ID, release_time, execution_time, deadline, period, preemption, type;
		file>>ID>>release_time>>execution_time>>deadline>>period>>preemption>>type;
		Task *newtask = new Task(ID, release_time, execution_time, deadline, period, preemption, type);
		tasks.push_back(*newtask);
	}
	for(int i=0;i<m;++i)
	{
		processors.at(i).printData();
	}
	for(int i=0;i<n;++i)
	{
		tasks.at(i).printData();
	}
}

int main()
{
	readFile();
	Scheduler *scheduler = new Scheduler(processors, tasks);
	cout << "\nStarting scheduler...\n" << endl;
	scheduler->shortestJobFirstWithPreempt();
	return 0;
}
