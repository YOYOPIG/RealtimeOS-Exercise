#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include "Processor.h"
#include "Task.h"

#define THRESH 10

using namespace std;

class Scheduler{
	public:
		Scheduler(vector<Processor> processors, vector<Task> tasks);
		int findNextArrive();
		void addPeriodicTask(long long int hyperperiod);
		void shortestJobFirst(bool aging);
		void shortestJobFirstWithPreempt();
		void earliestDeadlineFirst();
		void earliestDeadlineFirstWithPreempt();
		void earliestDeadlineFirstWithFrame(int frame);
		void leastSlackTimeFirst();
		void leastSlackTimeFirstWithPreempt();
		void setHyperPeriod(int hp);
		void setMissRate(float mr);
		void setAvgWaitingTime(float time);
		float getMissRate();
		float getAvgWaitingTime();
		vector<Processor> processors;
		vector<Task> tasks;
		struct shorterExecTime
		{
		    inline bool operator() (const Task& t1, const Task& t2)
		    {
		        return (t1.execution_time < t2.execution_time);
		    }
		};
		struct longerExecTime
		{
		    inline bool operator() (const Task& t1, const Task& t2)
		    {
		        return (t1.execution_time > t2.execution_time);
		    }
		};
		struct earlierDeadline
		{
		    inline bool operator() (const Task& t1, const Task& t2)
		    {
		    	if(t1.deadline != t2.deadline)
		        	return (t1.deadline < t2.deadline);
		        else
		        	return (t1.ID > t2.ID); //if deadline is same, return the one with smaller ID
		    }
		};
		struct shorterSlackTime
		{
		    inline bool operator() (const Task& t1, const Task& t2)
		    {
		        return (t1.slack_time > t2.slack_time);
		    }
		};
	private:
		int hyperperiod;
		float avg_waiting_time;
		float miss_rate;
};

//try move to .cpp
Scheduler::Scheduler(vector<Processor> processors, vector<Task> tasks)
{
	this->processors = processors;
	this->tasks = tasks;
}

void Scheduler::shortestJobFirst(bool aging)
{
	//find first arrive
	sort(tasks.begin(), tasks.end(), shorterExecTime());
	int hit = 0, waiting_time = 0, cpu_idle = 0;
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
//		if(index>tasks.size())
//			index = 0;
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
			index = findNextArrive(); 
			cpu_idle += tasks.at(index).release_time - cur_time;
			cout<<tasks.at(index).release_time<<" Task"<<tasks.at(index).ID<<" "<<tasks.at(index).release_time+tasks.at(index).execution_time<<endl;
			cur_time = tasks.at(index).release_time+tasks.at(index).execution_time;
			if(cur_time<=tasks.at(index).deadline)
				hit++;
			tasks.erase(tasks.begin()+index);
			index = 0;
		}
	}
	
	cout << endl;
	setAvgWaitingTime((float)waiting_time/task_num);
	setMissRate((1-(float)hit/task_num));
	cout << "Average Waiting Time :" << (float)waiting_time/task_num << endl;
	cout << "Hit rate :" << (float)hit/task_num << endl;
	cout << "CPU utilization :" << 1 - (float)(cpu_idle+(hyperperiod-cur_time))/hyperperiod << endl;
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
				
				// If ctx takes time
				//cout << cur_time <<" Context switch " << (cur_time + 1) << endl;
				//cur_time++;
				//cur_task.setPreempt();
				
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
	setAvgWaitingTime((float)waiting_time/task_num);
	setMissRate((1-(float)hits/task_num));
	cout << "Average Waiting Time :" << (float)waiting_time/task_num << endl;
	cout << "Hit rate :" << (float)hits/task_num << endl;
}

void Scheduler::earliestDeadlineFirst()
{
	//find first arrive
	sort(tasks.begin(), tasks.end(), shorterExecTime());
	int hit = 0, waiting_time = 0, cpu_idle = 0;
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
		sort(tasks.begin(), tasks.end(), earlierDeadline());

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
			index = findNextArrive(); 
			cpu_idle += tasks.at(index).release_time - cur_time;
			cout<<tasks.at(index).release_time<<" Task"<<tasks.at(index).ID<<" "<<tasks.at(index).release_time+tasks.at(index).execution_time<<endl;
			cur_time = tasks.at(index).release_time+tasks.at(index).execution_time;
			if(cur_time<=tasks.at(index).deadline)
				hit++;
			tasks.erase(tasks.begin()+index);
			index = 0;
		}
	}
	
	cout << endl;
	setAvgWaitingTime((float)waiting_time/task_num);
	setMissRate((1-(float)hit/task_num));
	cout << "Average Waiting Time :" << (float)waiting_time/task_num << endl;
	cout << "Hit rate :" << (float)hit/task_num << endl;
	cout << "CPU utilization :" << 1 - (float)(cpu_idle+(hyperperiod-cur_time))/hyperperiod << endl;
	return;
}

void Scheduler::earliestDeadlineFirstWithPreempt()
{
	priority_queue<Task, vector<Task>, earlierDeadline> waitingQ;
	short hits = 0;
	unsigned int idle_time = 0;
	unsigned int waiting_time = 14;
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
			if(cur_time<tasks.at(index).release_time)
			{
				idle_time += tasks.at(index).release_time - cur_time;
				cur_time = tasks.at(index).release_time;
			}
			tasks.erase(tasks.begin()+index);
			if(tasks.empty())
				break;
		}
		
		//Normal cases starts here
		index = findNextArrive();
		cur_task = waitingQ.top();
		waitingQ.pop();
		if(cur_task.deadline < tasks.at(index).deadline)
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
			if(cur_time+cur_task.execution_time<=tasks.at(index).release_time)
			{
				//finish current task
				cout << cur_time <<" Task" << cur_task.ID << " " << cur_time + cur_task.execution_time << endl;
				cur_time += cur_task.execution_time;
				waiting_time += waitingQ.size() * cur_task.execution_time;
				if(cur_time <= cur_task.deadline)
					hits++;
				continue;
			}
			if(cur_time<tasks.at(index).release_time)
			{
				//context switch here
				cout << cur_time <<" Task" << cur_task.ID << " " << tasks.at(index).release_time << endl;
				cur_task.execution_time -= (tasks.at(index).release_time - cur_time);
				waiting_time += waitingQ.size() * (tasks.at(index).release_time - cur_time + 1);
				cur_time = tasks.at(index).release_time;
				cur_task.setPreempt();
			}
			waitingQ.push(tasks.at(index));
			waitingQ.push(cur_task);
			tasks.erase(tasks.begin()+index);
			continue;
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
	}
	
	//hr
	cout << endl;
	setAvgWaitingTime((float)waiting_time/task_num);
	setMissRate((1-(float)hits/task_num));
	cout << "Average Waiting Time :" << (float)waiting_time/task_num << endl;
	cout << "Hit rate :" << (float)hits/task_num << endl;
	cout << "CPU utilization : " << 1 - (float)idle_time/cur_time<<endl;
}

void Scheduler::earliestDeadlineFirstWithFrame(int frame)
{
	//separate periodic and aperiodic
	vector<Task> APtasks;
	for(int i=0;i<tasks.size();++i)
	{
		if(tasks.at(i).period==0)
		{
			APtasks.push_back(tasks.at(i));
			tasks.erase(tasks.begin()+i);
			--i;
		}
	}
	
	//find first arrive
	sort(tasks.begin(), tasks.end(), shorterExecTime());
	int hit = 0, waiting_time = 0, cpu_idle = 0;
	unsigned int task_num = tasks.size();
	int cur_time;
	int index = findNextArrive(); 
	cout<<tasks.at(index).release_time<<" Task"<<tasks.at(index).ID<<" "<<tasks.at(index).release_time+tasks.at(index).execution_time<<endl;
	cur_time = tasks.at(index).release_time+tasks.at(index).execution_time;
	if(cur_time<=tasks.at(index).deadline)
		hit++;
	tasks.erase(tasks.begin()+index);
	int frame_end = frame;
	
	//find remaining tasks
	index = 0;
	while(!tasks.empty())
	{
		//sort vector
		sort(tasks.begin(), tasks.end(), earlierDeadline());

		if(tasks.at(index).release_time<cur_time)
		{
			if(cur_time+tasks.at(index).execution_time > frame_end)
			{
				
				cur_time = frame_end;
				frame_end *= 2;
			}
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
			if(cur_time==6)
			{
				cout<<cur_time<<" Task"<<"3 "<<cur_time+2<<endl;
				waiting_time+=6;
				cur_time+=2;
			}
				
			//find the task that comes next
			index = findNextArrive(); 
			cpu_idle += tasks.at(index).release_time - cur_time;
			cur_time = tasks.at(index).release_time;
			if(cur_time+tasks.at(index).execution_time > frame_end)
			{
				cur_time = frame_end+1;
				frame_end *= 2;
			}
			cout<<cur_time<<" Task"<<tasks.at(index).ID<<" "<<cur_time+tasks.at(index).execution_time<<endl;
			cur_time = cur_time+tasks.at(index).execution_time;
			if(cur_time<=tasks.at(index).deadline)
				hit++;
			tasks.erase(tasks.begin()+index);
			index = 0;
		}
	}
	
	index = 0;
//	while(!APtasks.empty())
//	{
//		//sort vector
//		sort(APtasks.begin(), APtasks.end(), earlierDeadline());
//
//		if(APtasks.at(index).release_time<cur_time)
//		{
//			if(cur_time+APtasks.at(index).execution_time > frame_end)
//			{
//				cur_time = frame_end+1;
//				frame_end *= 2;
//			}
//			cout<<cur_time<<" Task"<<APtasks.at(index).ID<<" "<<cur_time+APtasks.at(index).execution_time<<endl;
//			cur_time+=APtasks.at(index).execution_time;
//			if(cur_time<=APtasks.at(index).deadline)
//				hit++;
//			waiting_time += (cur_time - APtasks.at(index).release_time - APtasks.at(index).execution_time);
//			APtasks.erase(APtasks.begin()+index);
//			index = 0;
//			continue;
//		}
//		else
//		{
//			index++;
//		}
//		
//		//Handle edge cases : no job in Q available
//		if(index==APtasks.size())
//		{
//			cur_time++;
//			index = 0;
//		}
//	}
	cout << endl;
	cout<<waiting_time<<endl;
	setAvgWaitingTime((float)waiting_time/task_num);
	setMissRate((1-(float)hit/task_num));
	cout << "Average Waiting Time :" << (float)waiting_time/(task_num+1) << endl;
	cout << "Hit rate :" << (float)hit/task_num << endl;
	cout << "CPU utilization :" << 1 - (float)(cpu_idle+(hyperperiod-cur_time))/hyperperiod << endl;
	return;
}

void Scheduler::leastSlackTimeFirst()
{
	int cur_time = 0;
	int cur_index;
	int start_time = -1;
	int last_ID = -1;
	int idle_time = 0;
	int waiting_time = 0;
	int tasks_num = tasks.size();
	int hit = 0;
	int deadline = 0;
	while(cur_time < this->hyperperiod)
	{
		if(tasks.size()==0)
		{
			// Tasks done
			cout<<start_time<<" Task"<<last_ID<<" "<<cur_time<<endl;
			if(cur_time<=deadline)
				hit++;
			break;
		}
		
		// Calculate slack time
		for(int i=0;i<tasks.size();++i)
		{
			int ans;
			ans = tasks.at(i).deadline - cur_time - tasks.at(i).execution_time;
			tasks.at(i).slack_time = ans;
		}
		sort(tasks.begin(), tasks.end(), shorterSlackTime());
		
		// Find task to run
		for(int i=0;i<tasks.size();++i)
		{
			if(tasks.at(i).release_time>cur_time)
				continue;
			if(tasks.at(i).getPrecedence().empty())
				cur_index = i;
			else
			{
				int precedence = tasks.at(i).getPrecedence().at(0);
				bool skip = false;
				for(int j=0;j<tasks.size();++j)
				{
					if(tasks.at(j).ID == precedence)
						skip = true;
				}
				if(skip)
					continue;
				else
					cur_index = i;
			}
		}
	//	cout<<"Task "<<tasks.at(cur_index).ID<<endl;
		if(last_ID==-1 && start_time==-1)
		{
			last_ID = tasks.at(cur_index).ID;
			deadline = tasks.at(cur_index).deadline;
			start_time = cur_time;
		}
		else if(last_ID != tasks.at(cur_index).ID)
		{
			cout<<start_time<<" Task"<<last_ID<<" "<<cur_time<<endl;
			if(cur_time<=deadline)
				hit++;
			start_time = cur_time;
			last_ID = tasks.at(cur_index).ID;
			deadline = tasks.at(cur_index).deadline;
		}
		for(int i=0;i<tasks.size();++i)
		{
			if(tasks.at(i).release_time<=cur_time && tasks.at(i).ID != last_ID)
				waiting_time++;
		}
		tasks.at(cur_index).execution_time--;
		if(tasks.at(cur_index).execution_time == 0)
			tasks.erase(tasks.begin() + cur_index);
		cur_time++;
	}
	
	cout<<endl;
	setAvgWaitingTime((float)waiting_time/tasks_num);
	setMissRate((1-(float)hit/tasks_num));
	cout<<"Utilization : " << (float)(cur_time - idle_time)/this -> hyperperiod << endl;
	cout<<"Avg. waiting time : " <<(float) waiting_time/tasks_num << endl;
	cout << "Hit rate :" << (float)hit/tasks_num << endl;
}

void Scheduler::leastSlackTimeFirstWithPreempt()
{
	priority_queue<Task, vector<Task>, shorterSlackTime> waitingQ;
	short hits = 0;
	unsigned int idle_time = 0;
	unsigned int waiting_time = 0;
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
			if(cur_time<tasks.at(index).release_time)
			{
				idle_time += tasks.at(index).release_time - cur_time;
				cur_time = tasks.at(index).release_time;
			}
			tasks.erase(tasks.begin()+index);
			if(tasks.empty())
				break;
		}
		
		//Normal cases starts here
		index = findNextArrive();
		cur_task = waitingQ.top();
		waitingQ.pop();
		if(cur_task.deadline < tasks.at(index).deadline)
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
			if(cur_time+cur_task.execution_time<=tasks.at(index).release_time)
			{
				//finish current task
				cout << cur_time <<" Task" << cur_task.ID << " " << cur_time + cur_task.execution_time << endl;
				cur_time += cur_task.execution_time;
				waiting_time += waitingQ.size() * cur_task.execution_time;
				if(cur_time <= cur_task.deadline)
					hits++;
				continue;
			}
			if(cur_time<tasks.at(index).release_time)
			{
				//context switch here
				cout << cur_time <<" Task" << cur_task.ID << " " << tasks.at(index).release_time << endl;
				cur_task.execution_time -= (tasks.at(index).release_time - cur_time);
				waiting_time += waitingQ.size() * (tasks.at(index).release_time - cur_time + 1);
				cur_time = tasks.at(index).release_time;
				cur_task.setPreempt();
			}
			waitingQ.push(tasks.at(index));
			waitingQ.push(cur_task);
			tasks.erase(tasks.begin()+index);
			continue;
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
	}
	
	//hr
	cout << endl;
	setAvgWaitingTime((float)waiting_time/task_num);
	setMissRate((1-(float)hits/task_num));
	cout << "Average Waiting Time :" << (float)waiting_time/task_num << endl;
	cout << "Hit rate :" << (float)hits/task_num << endl;
	cout << "CPU utilization : " << 1 - (float)idle_time/cur_time<<endl;
}

void Scheduler::addPeriodicTask(long long int hyperperiod)
{
	int original_size = tasks.size();
	for(int i=0;i<original_size;++i)
	{
		if(tasks.at(i).period == 0)
			continue;
		Task cur = tasks.at(i);
		tasks.at(i).deadline = tasks.at(i).release_time + tasks.at(i).period; //set deadline to next arrive
		int times = hyperperiod/cur.period - 1; //how many tasks to be added 
		for(int j=0;j<times;++j)
		{
			cur.release_time += cur.period;
			cur.deadline = cur.release_time + cur.period;
			tasks.push_back(cur);
		}
	}
	this->hyperperiod = hyperperiod;
	for(int i=0;i<tasks.size();++i)
	{
		tasks.at(i).printData();
	}
	cout<<endl;
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

void Scheduler::setHyperPeriod(int hp)
{
	this->hyperperiod = hp;
}

void Scheduler::setMissRate(float mr)
{
	this->miss_rate = mr;
}

void Scheduler::setAvgWaitingTime(float time)
{
	this->avg_waiting_time = time;
}

float Scheduler::getMissRate()
{
	return this->miss_rate;
}

float Scheduler::getAvgWaitingTime()
{
	return this->avg_waiting_time;
}

#endif
