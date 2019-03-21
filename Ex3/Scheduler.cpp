#include <iostream>
#include <vector>
#include <algorithm>
#include "Processor.h"
#include "Task.h"
#include "Scheduler.h"

Scheduler::Scheduler(vector<Processor> processors, vector<Task> tasks)
{
	this->processors = processors;
	this->tasks = tasks;
}

void Scheduler::shortestJobFirst()
{
	//find first arrive
	int cur_time;
	int index = findNextArrive(); 
	cout<<tasks.at(index).release_time<<" Task"<<tasks.at(index).ID<<" "<<tasks.at(index).release_time+tasks.at(index).execution_time<<endl;
	cur_time = tasks.at(index).release_time+tasks.at(index).execution_time;
	tasks.erase(tasks.begin()+index);
	
	//find remaining tasks
	index = 0;
	while(!tasks.empty())
	{
		//sort vector
		sort(tasks.begin(), tasks.end(), shorterExecTime());
		if(tasks.at(index).release_time<cur_time)
		{
			cout<<cur_time<<" Task"<<tasks.at(index).ID<<" "<<cur_time+tasks.at(index).execution_time<<endl;
			cur_time+=tasks.at(index).execution_time;
			tasks.erase(tasks.begin()+index);
			index = 0;
			continue;
		}
		else
		{
			index++;
		}
		//Handle edge cases : no job in Q available
		if(index==tasks.size())
		{
			//find the task that comes next
			int index = findNextArrive(); 
			cout<<tasks.at(index).release_time<<" Task"<<tasks.at(index).ID<<" "<<tasks.at(index).release_time+tasks.at(index).execution_time<<endl;
			cur_time = tasks.at(index).release_time+tasks.at(index).execution_time;
			tasks.erase(tasks.begin()+index);
			index = 0;
		}
	}
}

int Scheduler::findNextArrive()
{
	int index=0, min_arrive=tasks.at(0).release_time;
	for(int i=0;i<tasks.size();++i)
	{
		if(tasks.at(i).release_time<min_arrive)
		{
			index = i;
			min_arrive = tasks.at(i).release_time;
		}
	}
	return index;
}
