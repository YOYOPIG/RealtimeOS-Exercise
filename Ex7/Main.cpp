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

void readFile();
int gcd(int a, int b);
long long int findlcm(vector<Task> arr);

int main()
{
	readFile();
	long long int hyperperiod = findlcm(tasks);
	Scheduler *scheduler = new Scheduler(processors, tasks);
	scheduler->addPeriodicTask(hyperperiod);
	cout << "\nStarting scheduler...\n" << endl;
	scheduler->leastSlackTimeFirst();
	return 0;
}


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
	file>>m;
	for(int i=0;i<m;++i)
	{
		int precedence_task, tar_task;
		file>>precedence_task>>tar_task;
		for(int j=0;j<tasks.size();++j)
		{
			if(tasks.at(j).ID==tar_task)
				tasks.at(j).setPrecedence(precedence_task);
		}
	}
	
	cout<<endl;
}

int gcd(int a, int b) 
{ 
    if (b == 0) 
        return a; 
    return gcd(b, a % b); 
} 
  
// Returns LCM of array elements 
long long int findlcm(vector<Task> arr) 
{ 
    // Initialize result 
    long long int ans = arr[0].period; 
  
    // ans contains LCM of arr[0], ..arr[i] 
    // after i'th iteration, 
    for (int i = 1; i < arr.size(); i++) 
    {
    	if(arr[i].period == 0)
    		continue;
        ans = (((arr[i].period * ans)) / 
                (gcd(arr[i].period, ans))); 
	}
  	cout<<"ans is "<<ans<<endl;
    return ans; 
} 
