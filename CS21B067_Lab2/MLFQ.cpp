#include <bits/stdc++.h>
#include <time.h>

using namespace std;
double timer;
double tat;
int processCount;
ifstream input;
ofstream output;

class Process
{
    private:
        int ID;
        int OriginalLevel;
        int FinalLevel;
        double ArrivalTime;
        double BurstTime;
        double CompletionTime;
        double TotalTime;
        double WaitTime;
    public:
        Process(int inpID, int inpOriginalLevel, double inpArrivalTime, double inpBurstTime)
        {
            ID = inpID;
            OriginalLevel = inpOriginalLevel;
            ArrivalTime = inpArrivalTime;
            BurstTime = inpBurstTime;
            FinalLevel = -1;
            CompletionTime = 0.0;
            TotalTime = 0.0;
            WaitTime = 0.0;
        }

        const int getID(){return ID;}
        const int getOriginalLevel(){return OriginalLevel;}
        const int getFinalLevel(){return FinalLevel;}
        const double getArrivalTime(){return ArrivalTime;}
        const double getBurstTime(){return BurstTime;}
        const double getCompletionTime(){return CompletionTime;}
        const double getTAT(){return CompletionTime-ArrivalTime;}
        const double getTotalTime(){return TotalTime;}
        const double getWaitTime(){return WaitTime;}

        void setBurstTime(const double & inp)
        {
            BurstTime = inp; return;
        }
        void setTotalTime(const double & inp)
        {
            TotalTime = inp; return;
        }
        void setCompletionTime(const double & inp)
        {
            CompletionTime = inp; return;
        }
        void setFinalLevel(const int & inp)
        {
            FinalLevel = inp; return;
        }
        void setWaitTime(const double & inp)
        {
            WaitTime = inp; return;
        }
        void PrintProcess()
        {
            output<<"ID: "<<setw(6)<<ID<<"; ";
            output<<"Orig. Level: "<<OriginalLevel<<"; ";
            output<<"Final Level: "<<FinalLevel<<"; ";
            output<<fixed<<setprecision(2)<<"Comp. Time(ms): "<<setw(8)<<CompletionTime<<"; ";
            output<<fixed<<setprecision(2)<<"TAT(ms): "<<setw(8)<<getTAT()<<":\n";
            tat += getTAT();
        }
};

class FCFS;
class SJF;

class CMP 
{ 
    public:
        bool operator()(Process &a, Process &b)
        {
            if(a.getBurstTime()-a.getTotalTime() > b.getBurstTime()-b.getTotalTime())
            {
                return true;
            }
            else if(a.getBurstTime()-a.getTotalTime() == b.getBurstTime()-b.getTotalTime())
            {
                if(a.getID() > b.getID())
                {
                    return true;
                }
            }
            return false;
        }
};

class CMPRR
{
    public:
        bool operator()(pair<double, Process> a, pair<double, Process> b)
        {
            if(a.first==b.first)
            {
                if(a.second.getID() > b.second.getID())
                {
                    return true;
                }
            }
            return a.first > b.first;
        }
};

class RR
{
    private:
        priority_queue<pair<double, Process>, vector<pair<double, Process>>, CMPRR> RoundRobin;
        double TimeSlice;
        double Threshold;
    public:
        RR(double Q, double T)
        {
            TimeSlice = Q;
            Threshold = T;
        }
        bool isEmpty()
        {
            return RoundRobin.empty();
        } 
        void PushProcess(const pair<double, Process> &inp)
        {
            RoundRobin.push(inp);
        }
        void Run(FCFS *Q1, SJF *Q2, SJF *Q3, RR *Q4);
    friend SJF;
    friend FCFS;
};

class SJF
{
    private:
        priority_queue<Process, vector<Process>, CMP> ShortestJobFirst;
        double Threshold;
        int priority;
    public:
        SJF(double T, int id)
        {
            Threshold = T;
            priority = id;
        }
        void PushProcess(Process &inp)
        {
            ShortestJobFirst.push(inp);
        }
        const bool isEmpty()
        {
            return ShortestJobFirst.empty();
        }
        void PushUp2(SJF *Q3)
        {
            queue<Process> temp;
            while(!ShortestJobFirst.empty())
            {
                Process A = ShortestJobFirst.top();
                ShortestJobFirst.pop();
                if(A.getOriginalLevel()==1 && timer-A.getArrivalTime()>=2*Threshold)
                {
                    Q3->PushProcess(A);
                }                
                else if(A.getOriginalLevel()==2 && timer-A.getArrivalTime()>=Threshold)
                {
                    Q3->PushProcess(A);
                }
                else
                {
                    temp.push(A);
                }
            }
            while(!temp.empty())
            {
                ShortestJobFirst.push(temp.front());
                temp.pop();
            }            
        }
        void PushUp3(RR *Q4)
        {
            queue<Process> temp;
            while(!ShortestJobFirst.empty())
            {
                Process A = ShortestJobFirst.top();
                ShortestJobFirst.pop();
                if(A.getOriginalLevel()==1 && timer-A.getArrivalTime()>=3*Threshold)
                {
                    Q4->PushProcess({A.getArrivalTime()+3*Threshold, A});
                }                
                else if(A.getOriginalLevel()==2 && timer-A.getArrivalTime()>=2*Threshold)
                {
                    Q4->PushProcess({A.getArrivalTime()+2*Threshold, A});
                }
                else if(A.getOriginalLevel()==3 && timer-A.getArrivalTime()>=Threshold)
                {
                    Q4->PushProcess({A.getArrivalTime()+Threshold, A});
                }
                else
                {
                    temp.push(A);
                }
            }
            while(!temp.empty())
            {
                ShortestJobFirst.push(temp.front());
                temp.pop();
            }
        }
        void RunQ3(RR *Q4, SJF *Q2)
        {
            PushUp3(Q4);
            if(ShortestJobFirst.empty() || !Q4->isEmpty())
            {
                return;
            }
            Process A = ShortestJobFirst.top();
            ShortestJobFirst.pop();
            timer += A.getBurstTime();
            A.setCompletionTime(timer);
            A.setFinalLevel(3);
            A.setTotalTime(A.getBurstTime());
            A.PrintProcess();
            PushUp3(Q4);     
            return;                            
        }
        void RunQ2(RR *Q4, SJF *Q3, FCFS *Q1)
        {
            PushUp2(Q3);
            Q3->PushUp3(Q4);
            if(ShortestJobFirst.empty() || !Q4->isEmpty() || !Q3->isEmpty())
            {
                return;
            }
            Process A = ShortestJobFirst.top();
            ShortestJobFirst.pop();
            timer += A.getBurstTime();
            A.setCompletionTime(timer);
            A.setFinalLevel(2);
            A.setTotalTime(A.getBurstTime());
            A.PrintProcess();
            PushUp2(Q3);   
            Q3->PushUp3(Q4);  
            return;     
        }
    friend RR;
    friend FCFS;
};

class FCFS
{
    private:
        queue<Process> Order;
        double Threshold;
    public:
        FCFS(double inp)
        {
            Threshold = inp;
        }
        void PushProcess(Process & inp)
        {
            Order.push(inp);
            return;
        }
        const bool isEmpty()
        {
            return Order.empty();
        }
        void PushUp1(SJF *Q2)
        {
            queue<Process> temp;
            while(!Order.empty())
            {
                if(timer-Order.front().getArrivalTime() >= Threshold)
                {
                    Q2->PushProcess(Order.front());
                    Order.pop();
                }
                else
                {
                    temp.push(Order.front());
                    Order.pop();
                }
            }
            while(!temp.empty())
            {
                Order.push(temp.front());
                temp.pop();
            }
            return;
        }
        void Run(RR *Q4, SJF *Q3, SJF *Q2)
        {
            PushUp1(Q2);
            Q2->PushUp2(Q3);
            Q3->PushUp3(Q4);
            if(Order.empty() || !Q2->isEmpty() || !Q3->isEmpty() || !Q4->isEmpty())
            {
                return;
            }
            Process A = Order.front();
            Order.pop();
            timer += A.getBurstTime();
            A.setCompletionTime(timer);
            A.setFinalLevel(1);
            A.setTotalTime(A.getBurstTime());
            A.PrintProcess();
            PushUp1(Q2); 
            Q2->PushUp2(Q3);
            Q3->PushUp3(Q4);
            return;        
        }
    friend RR;
    friend SJF;
};

void RR::Run(FCFS *Q1, SJF *Q2, SJF *Q3, RR *Q4)
{
    double temp = TimeSlice;
    while(temp>0 && !RoundRobin.empty())
    {
        double qarrival = RoundRobin.top().first;
        Process A = RoundRobin.top().second;
        RoundRobin.pop();
        if(A.getBurstTime() - A.getTotalTime() <= temp)
        {
            temp -= A.getBurstTime() - A.getTotalTime();
            timer += A.getBurstTime() - A.getTotalTime();
            A.setCompletionTime(timer);
            A.setFinalLevel(4);
            A.setTotalTime(A.getBurstTime());
            A.PrintProcess();
            Q1->PushUp1(Q2);
            Q2->PushUp2(Q3);
            Q3->PushUp3(Q4);
            return;
        }
        else
        {
            timer += temp;
            A.setTotalTime(A.getTotalTime()+temp);
            temp -= temp;
            Q1->PushUp1(Q2);
            Q2->PushUp2(Q3);
            Q3->PushUp3(Q4);
            RoundRobin.push({timer, A});
            return;
        }
    }
}

class MLFQ
{
    private:
        RR *Q4;
        SJF *Q3;
        SJF *Q2;
        FCFS *Q1;
        double TimeSlice;
        double Threshold;
        vector<Process *> ProcessList;
    public:
        MLFQ(double Q, double T, const vector<Process *> &List){
            TimeSlice = Q;
            Threshold = T;
            Q4 = new RR(TimeSlice, Threshold);
            Q3 = new SJF(Threshold, 3);
            Q2 = new SJF(Threshold, 2);
            Q1 = new FCFS(Threshold);
            ProcessList = List;
        }
        void Schedule( )
        {
            for(auto & ProcessPointer : ProcessList)
            {
                while(timer < ProcessPointer->getArrivalTime())
                {
                    if(!Q4->isEmpty())
                    {
                        Q4->Run(Q1, Q2, Q3, Q4);
                    }
                    else if(Q4->isEmpty() && !Q3->isEmpty() && timer<ProcessPointer->getArrivalTime())
                    {
                        Q3->RunQ3(Q4, Q2);
                    }
                    else if(Q4->isEmpty() && Q3->isEmpty() && !Q2->isEmpty() && timer<ProcessPointer->getArrivalTime())
                    {
                        Q2->RunQ2(Q4, Q3, Q1);
                    }
                    else if(Q4->isEmpty() && Q3->isEmpty() && Q2->isEmpty() && !Q1->isEmpty() && timer<ProcessPointer->getArrivalTime())
                    {
                        Q1->Run(Q4, Q3, Q2);
                    }
                    else if(Q4->isEmpty() && Q3->isEmpty() && Q2->isEmpty() && Q1->isEmpty() && timer<ProcessPointer->getArrivalTime())
                    {
                        timer = ProcessPointer->getArrivalTime();
                    }
                }
                if(timer >= ProcessPointer->getArrivalTime())
                {
                    switch(ProcessPointer->getOriginalLevel())
                    {
                        case 1:
                            Q1->PushProcess(*ProcessPointer);
                            break;
                        case 2:
                            Q2->PushProcess(*ProcessPointer);
                            break;
                        case 3:
                            Q3->PushProcess(*ProcessPointer);
                            break;
                        case 4:
                            Q4->PushProcess({ProcessPointer->getArrivalTime(), *ProcessPointer});
                            break;
                    }
                    continue;
                }
                if(!Q4->isEmpty())
                {
                    Q4->Run(Q1, Q2, Q3, Q4);
                }             
                if(Q4->isEmpty() && !Q3->isEmpty())
                {
                    Q3->RunQ3(Q4, Q2);
                }   
                if(Q4->isEmpty() && Q3->isEmpty() && !Q2->isEmpty())
                {
                    Q2->RunQ2(Q4, Q3, Q1);
                }
                if(Q4->isEmpty() && Q3->isEmpty() && Q2->isEmpty() && !Q1->isEmpty())
                {
                    Q1->Run(Q4, Q3, Q2);
                }
            }
            while(!(Q4->isEmpty() && Q3->isEmpty() && Q2->isEmpty() && Q1->isEmpty()))
            {
                if(!Q4->isEmpty())
                {
                    Q4->Run(Q1, Q2, Q3, Q4);
                }             
                if(Q4->isEmpty() && !Q3->isEmpty())
                {
                    Q3->RunQ3(Q4, Q2);
                }   
                if(Q4->isEmpty() && Q3->isEmpty() && !Q2->isEmpty())
                {
                    Q2->RunQ2(Q4, Q3, Q1);
                }
                if(Q4->isEmpty() && Q3->isEmpty() && Q2->isEmpty() && !Q1->isEmpty())
                {
                    Q1->Run(Q4, Q3, Q2);
                } 
            }
            processCount = ProcessList.size();
            output<<"Mean Turnaround time: ";
            output<<fixed<<setprecision(3)<<(1.0*tat)/processCount;
            output<<" (ms); Throughput: ";
            output<<fixed<<setprecision(3)<<(1000.0*processCount)/timer;
            output<<" processes/sec\n";
            free(Q1);
            free(Q2);
            free(Q3);
            free(Q4);
        }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Compare(Process *A, Process *B)
{
    if(A->getArrivalTime()==B->getArrivalTime())
    {
        if(A->getOriginalLevel()==B->getOriginalLevel())
        {
            return A->getID() < B->getID();
        }
        return A->getOriginalLevel() > B->getOriginalLevel();
    }
    return A->getArrivalTime() < B->getArrivalTime();
}

double StringTodouble(char *s)
{
    double temp = 0.0;
    for(int i=0; i<strlen(s); i++)
    {
        temp *= 10;
        temp += (s[i]-'0');
    }
    return temp;
}

int main(int argc, char *argv[])
{
    if(argc!=5)
    {
        cout<<"Incorrect Number of arguements\n";
        cout<<"Terminating\n";
        exit(0);
    }   
    double Q, T;
    int n;
    Q = StringTodouble(argv[1]);
    T = StringTodouble(argv[2]);
    vector<Process *> List(n);
    int ID, OLevel;
    double AT, BT;
    input = ifstream(argv[3]);
    output = ofstream(argv[4]);
    timer = 0.0;
    tat = 0.0;
    while(!input.eof())
    {
        input>>ID>>OLevel>>AT>>BT;        
        Process *A = new Process(ID, OLevel, AT, BT);
        List.push_back(A);
    }
    sort(List.begin(), List.end(), Compare);
    MLFQ* Answer = new MLFQ(Q, T, List);
    Answer->Schedule();
    for(auto u : List)
    {
        free(u);
    }
    free(Answer);
    input.close();
    output.close();
    cout<<"Success!\n";
    return 0;
}