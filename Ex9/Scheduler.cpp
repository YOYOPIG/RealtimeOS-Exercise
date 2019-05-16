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

void Scheduler::shortestJobFirst(bool aging)
{
	//find first arrive
	sort(tasks.begin(), tasks.end(), shorterExecTime());
	int hit = 0, waiting_time = 0;
	unsigned int task_num = tasks.size();
	int cur_time;
	int index = findNextArrive(); 
	cout<<tasks.at(index).release_time<<" Task"<<tasks.at(index).ID<<" "<<tasks.at(index).release_time+tasks.at(index).execution_time<<endl;
	cur_time = tasks.at(index).release_time+tasks.at(index).execution_time;
	if(cur_time<=tasks.at(index).deadline)
		hit++;
	tasks.erase(tasks.begin()+index);
	
	//find remaining tasks
	index = 0;
	while(!tasks.empty())
	{
		//sort vector
		sort(tasks.begin(), tasks.end(), shorterExecTime());
		
		if(aging)
		{
			//Handle special case : Aging tasks, oldest first
			int oldest_task = findNextArrive();
			if(tasks.at(oldest_task).release_time + THRESH <= cur_time)
			{
				cout<<cur_time<<" Task"<<tasks.at(oldest_task).ID<<" "<<cur_time+tasks.at(oldest_task).execution_time<<endl;
				cur_time += tasks.at(oldest_task).execution_time;
				if(cur_time<=tasks.at(oldest_task).deadline)
					hit++;
				waiting_time += (cur_time - tasks.at(oldest_task).release_time - tasks.at(oldest_task).execution_time);
				tasks.erase(tasks.begin()+oldest_task);
				continue;
			}
		}
		
		//normal SJF
		if(tasks.at(index).release_time<cur_time)
		{
			cout<<cur_time<<" Task"<<tasks.at(index).ID<<" "<<cur_time+tasks.at(index).execution_time<<endl;
			cur_time+=tasks.at(index).execution_time;
			if(cur_time<=tasks.at(index).deadline)
				hit++;
			waiting_time += (cur_time - tasks.at(index).release_time - tasks.at(index).execution_time);
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
			if(cur_time<=tasks.at(index).deadline)
				hit++;
			tasks.erase(tasks.begin()+index);
			index = 0;
		}
	}
	
	cout << endl;
	cout << "Average Waiting Time :" << (float)waiting_time/task_num << endl;
	cout << "Hit rate :" << (float)hit/task_num << endl;
	return;
}

void Scheduler::shortestJobFirstWithPreempt()
{
	priority_queue<Task, vector<Task>, longerExecTime> waitingQ;
	short hits = 0;
	unsigned int waiting_time = 1;
	unsigned int task_num = tasks.size();
	
	//find first arrive
	int cur_time;
	int index = findNextArrive();
	Task cur_task;
	waitingQ.push(tasks.at(index));
	cur_time = tasks.at(index).release_time;
	tasks.erase(tasks.begin()+index);
	
	//while there is still jobs remaining, push them into waiting Q when they arrive
	while(!tasks.empty())
	{
		//Handle edge cases
		if(waitingQ.empty())
		{
			index = findNextArrive();
			waitingQ.push(tasks.at(index));
			cur_time = tasks.at(index).release_time;
			tasks.erase(tasks.begin()+index);
		}
		
		//Normal cases starts here
		index = findNextArrive();
		cur_task = waitingQ.top();
		waitingQ.pop();
		if(cur_time + cur_task.execution_time < tasks.at(index).release_time)
		{
			//finish current task
			cout << cur_time <<" Task" << cur_task.ID << " " << cur_time + cur_task.execution_time << endl;
			cur_time += cur_task.execution_time;
			waiting_time += waitingQ.size() * cur_task.execution_time;
			if(cur_time <= cur_task.deadline)
				hits++;
			continue;
		}
		else
		{
			//maybe preempt?
			if(tasks.at(index).execution_time > cur_task.execution_time - (tasks.at(index).release_time - cur_time))
			{
				//new task takes longer, just push it back (dont finish current here Bcause other task might come in)
				waitingQ.push(tasks.at(index));
				tasks.erase(tasks.begin()+index);
				waitingQ.push(cur_task);
				continue;
			}
			else
			{
				//context switch here
				cout << cur_time <<" Task" << cur_task.ID << " " << tasks.at(index).release_time << endl;
				cur_task.execution_time -= (tasks.at(index).release_time - cur_time);
				waiting_time += waitingQ.size() * (tasks.at(index).release_time - cur_time + 1);
				cur_time = tasks.at(index).release_time;
				cout << cur_time <<" Context switch " << (cur_time + 1) << endl;
				cur_time++;
				cur_task.setPreempt();
				waitingQ.push(tasks.at(index));
				waitingQ.push(cur_task);
				tasks.erase(tasks.begin()+index);
				continue;
			}
		}
	}
	
	//Now all tasks arrived, just clear waitingQ
	while(!waitingQ.empty())
	{
		cur_task = waitingQ.top();
		waitingQ.pop();
		cout << cur_time <<" Task" << cur_task.ID << " " << cur_time + cur_task.execution_time << endl;
		waiting_time += waitingQ.size() * cur_task.execution_time;
		cur_time += cur_task.execution_time;
		if(cur_time <= cur_task.deadline)
			hits++;
		if(!waitingQ.empty() && waitingQ.top().isPreempted())
		{
			cout << cur_time <<" Context switch " << (cur_time + 1) << endl;
			waiting_time += waitingQ.size();
			cur_time++;
		}
	}
	
	//hr
	cout << endl;
	cout << "Average Waiting Time :" << (float)waiting_time/task_num << endl;
	cout << "Hit rate :" << (float)hits/task_num << endl;
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
