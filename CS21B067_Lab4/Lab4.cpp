#include <bits/stdc++.h>
#include <chrono>
#include <ctime>

using namespace std;

//--------------------------------------------------------------------------------------------------------------------------------
//Global variables and classes
int pid_counter = 1;
int M=1, V=1, P=1;
list<int> LRU;
ofstream output;
string outputfile;
bool StopExecution = true;

int S2UI(string s)
{
    int x = 0;
    for(int i=0; i<s.length(); i++)
    {
        x *= 10;
        x += s[i]-'0';
    }
    return x;
}

class Page
{
    public:
        vector<int> page;
        uint32_t ASID;
        Page()
        {
            ASID = 0;
            page.resize(P, 0);
        }
        void ResetPage(uint32_t ASIDin = 0, int arg = 0)
        {
            ASID = ASIDin;
            page.resize(P, arg);
        }
        int getContent(int index) const { return page[index]; }
        void setContent( const int content, int index)
        {
            page[index] = content;
        }
        void setPage(const uint32_t ProcASID, int index, const int content = 0)
        {
            ASID = ProcASID;
            page[index] = content;
        }
        void PrintPage()
        {
            cout<<ASID<<" "<<page.size()<<endl;
        }
};

class MainMemory
{
    public:
        vector<Page> Memory;
        int MainMemoryPageNum;
        MainMemory()
        {
            Memory.resize((1024*M)/P);
            MainMemoryPageNum = (1024*M)/P;
        }
};

class VirtualMemory
{
    public:
        vector<Page> Memory;
        int VirtualMemoryPageNum;
        VirtualMemory()
        {
            Memory.resize((1024*V)/P);
            VirtualMemoryPageNum = (1024*V)/P;
        }
};

MainMemory MAIN;
VirtualMemory VIRTUAL;
//--------------------------------------------------------------------------------------------------------------------------------
//File's Commands

class FileCommand //Parent class for all User process commands
{
    public:
        virtual void Run() = 0;
};

class PageTable
{
    public:
        bool mainmemory;
        map<int, int> VPN2PFN; 
        // VPN2PFN : Translates Virtual Page Number to Page Frame Number for every process
        PageTable()
        {
            mainmemory = false;
        }
        PageTable(bool b)
        {
            mainmemory = b;
        }
        void setBool(const bool b)
        {
            mainmemory = b;
        }
        bool getBool() const
        {
            return mainmemory;
        }
};

class AddCommand : public FileCommand //ADD Command of input file
{
    private:
        int x;
        int y;
        int z;
        int sz;
        int pid;
        PageTable *table;
    public:
        AddCommand(int xreg, int yreg, int zreg, int procsz, PageTable *procTable, int inpid) : x(xreg), y(yreg), z(zreg), sz(procsz), table(procTable), pid(inpid) { }
        int showX() const{ return x; }
        int showY() const{ return y; }
        int showZ() const{ return z; }
        void Run()
        {
            if(x>=sz)
            {
                output<<"Invalid Memory Address "<<x<<" specified for process id "<<pid<<endl;
                StopExecution = true;
                return;
            } 
            if(y>=sz)
            {
                output<<"Invalid Memory Address "<<x<<" specified for process id "<<pid<<endl;
                StopExecution = true;
                return;
            }   
            if(z>=sz)
            {
                output<<"Invalid Memory Address "<<x<<" specified for process id "<<pid<<endl;
                StopExecution = true;
                return;
            } 
            int a, b, c;
            a = MAIN.Memory[table->VPN2PFN[x/P]].getContent(x%P);
            b = MAIN.Memory[table->VPN2PFN[y/P]].getContent(y%P);
            c = a+b;
            MAIN.Memory[table->VPN2PFN[z/P]].setContent(c, z%P);
            output<<"Command: add "<<x<<" "<<y<<" "<<z<<"; Result: Value in addr "<<x<<" = "<<a<<", addr "<<y<<" = "<<b<<", addr "<<z<<" = "<<MAIN.Memory[z/P].getContent(z%P)<<endl;
        }    
};

class SubCommand : public FileCommand //SUB Command of input file
{
    private:
        int x;
        int y;
        int z;
        int sz;
        int pid;
        PageTable *table;
    public:
        SubCommand(int xreg, int yreg, int zreg, int procsz, PageTable *procTable, int inpid) : x(xreg), y(yreg), z(zreg), sz(procsz), table(procTable), pid(inpid)  { }
        int showX() const{ return x; }
        int showY() const{ return y; }
        int showZ() const{ return z; }   
        void Run()
        {
            if(x>=sz)
            {
                output<<"Invalid Memory Address "<<x<<" specified for process id "<<pid<<endl;
                StopExecution = true;
                return;
            } 
            if(y>=sz)
            {
                output<<"Invalid Memory Address "<<x<<" specified for process id "<<pid<<endl;
                StopExecution = true;
                return;
            }   
            if(z>=sz)
            {
                output<<"Invalid Memory Address "<<x<<" specified for process id "<<pid<<endl;
                StopExecution = true;
                return;
            } 
            int a, b, c;
            a = MAIN.Memory[table->VPN2PFN[x/P]].getContent(x%P);
            b = MAIN.Memory[table->VPN2PFN[y/P]].getContent(y%P);
            c = a-b;
            MAIN.Memory[table->VPN2PFN[z/P]].setContent(c, z%P);
            output<<"Command: sub "<<x<<" "<<y<<" "<<z<<"; Result: Value in addr "<<x<<" = "<<a<<", addr "<<y<<" = "<<b<<", addr "<<z<<" = "<<MAIN.Memory[z/P].getContent(z%P)<<endl;
        }   
};

class PrintRegCommand : public FileCommand //PRINT Command of input file
{
    private:
        int sz;
        int x;
        int pid;
        PageTable *table;
    public:
        PrintRegCommand(int xreg, int procsz, PageTable *procTable, int inpid) : x(xreg), sz(procsz), table(procTable), pid(inpid)  { }
        int showX() const{ return x; } 
        void Run()
        {
            if(x>=sz)
            {
                output<<"Invalid Memory Address "<<x<<" specified for process id "<<pid<<endl;
                StopExecution = true;
                return;
            } 
            int a;
            a = MAIN.Memory[table->VPN2PFN[x/P]].getContent(x%P);
            output<<"Command: print "<<x<<"; Result: Value in addr "<<x<<" = "<<a<<endl;
        }  
};

class LoadRegCommand : public FileCommand //LOAD Command of input file
{
    private:
        int x;
        int a;
        int pid;
        PageTable *table;
        int sz;
    public:
        LoadRegCommand(int xreg, int in, int procsz, PageTable *procTable, int inpid) : x(xreg), a(in), sz(procsz), table(procTable), pid(inpid)  { }
        int showX() const { return x; }
        int showA() const { return a; }
        void Run()
        {
            if(x>=sz)
            {
                output<<"Invalid Memory Address "<<x<<" specified for process id "<<pid<<endl;
                StopExecution = true;
                return;
            } 
            MAIN.Memory[table->VPN2PFN[x/P]].setContent(a, x%P);
            output<<"Command: load "<<a<<", "<<x<<"; Result: Value of "<<a<<" is now stored in addr "<<x<<endl;
        }  
};

class Process{
    private:
        string name;
        int pid;
        int sz;
        vector<FileCommand *> instructions;
    public:
        Process() = default;
        PageTable *table;
        Process(string proc_name, int proc_id, int proc_sz, bool inmain): name(proc_name), pid(proc_id), sz(proc_sz)
        {
            table = new PageTable(inmain);
        }
        void ParseFile()
        {
            ifstream infile;
            infile.open(name+".txt");
            if(!infile.is_open())
            {
                cout<<"Could not open input file\n";
                exit(0);
            }
            string cmd;
            infile>>sz;
            sz = sz<<10;
            while(!infile.eof())
            {
                infile>>cmd;
                FileCommand *newCommand;
                if(cmd=="add")
                {
                    string xs, ys;
                    int x, y, z;
                    infile>>xs>>ys>>z;
                    xs.pop_back();
                    ys.pop_back();
                    x = S2UI(xs);
                    y = S2UI(ys);
                    newCommand = new AddCommand(x, y, z, sz, table, pid);
                }   
                else if(cmd=="sub")
                {
                    string xs, ys;
                    int x, y, z;
                    infile>>xs>>ys>>z;
                    xs.pop_back();
                    ys.pop_back();
                    x = S2UI(xs);
                    y = S2UI(ys);
                    newCommand = new SubCommand(x, y, z, sz, table, pid);
                }   
                else if(cmd=="load")
                {
                    int x;
                    int a;
                    string as;
                    infile>>as>>x;
                    as.pop_back();
                    a = S2UI(as);
                    newCommand = new LoadRegCommand(x, a, sz, table, pid);
                }   
                else if(cmd=="print")
                {
                    int x;
                    infile>>x;
                    newCommand = new PrintRegCommand(x, sz, table, pid);                    
                }     
                else
                {
                    continue;
                }
                instructions.push_back(newCommand);
            }
        }
        void RunProcess()
        {
            StopExecution = false;
            ParseFile();
            for(auto &entry : instructions)
            {
                if(!StopExecution) entry->Run();
            }
            StopExecution = true;
        }
        void setPID(const int pidin)
        {
            pid = pidin;
        }
        void PrintProcess()
        {
            output<<"Name :"<<name<<endl;
            output<<"PID :"<<pid<<endl;
            output<<"Size = "<<sz<<endl;
            output<<"Boolean = "<<table->mainmemory<<endl;   
        }
        int getPID() const
        {
            return pid;
        }
        ~Process()
        {
            for(auto u: instructions)
            {
                free(u);
            }
        }
};
map<int, Process> PROCESSMAP;
//--------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------
//System commands

bool isValidPID(int pid)
{
    if(pid<=0 || PROCESSMAP.find(pid)==PROCESSMAP.end())
    {
        output<<"Invalid PID "<<pid<<endl;
        return false;
    }
    return true;
}

void UpdateLRU(int pid, bool IntoLRU)
{
    LRU.remove(pid);
    if(IntoLRU)
    {
        LRU.push_back(pid);
    }
    return;
}

void LoadCommand(string inp, vector<string> Files, int num_files)
{
    for(int i=0; i<Files.size(); i++)
    {
        ifstream infile;
        infile.open(Files[i]+".txt", ios::in);
        if(infile.is_open()==false)
        {
            output<<Files[i]<<" could not be loaded - file does not exist"<<endl;
            continue;
        }
        int siz = 0;
        infile>>siz;
        infile.close(); 
        Process thisProcess = Process(Files[i], 0, siz, false);
        siz *= 1024;
        siz /= P; 
        if(MAIN.MainMemoryPageNum >= siz)
        {
            MAIN.MainMemoryPageNum -= siz;
            thisProcess.table->setBool(true);
            thisProcess.setPID(pid_counter++);
            vector<size_t> freeProcessInMM;
            for(size_t i=0; i<MAIN.Memory.size(); i++)
            {
                if(freeProcessInMM.size()>=siz)
                {
                    break;
                }
                else if(MAIN.Memory[i].ASID==0)
                {
                    freeProcessInMM.push_back(i);
                }
            }
            for(int i=0; i<freeProcessInMM.size(); i++)
            {
                thisProcess.table->VPN2PFN.insert({i, freeProcessInMM[i]});
                MAIN.Memory[freeProcessInMM[i]].ResetPage(thisProcess.getPID());
            }
            PROCESSMAP.insert({thisProcess.getPID(), thisProcess});
            output<<Files[i]<<" is loaded in main memory and is assigned process id "<<thisProcess.getPID()<<endl;
        }
        else if(VIRTUAL.VirtualMemoryPageNum >= siz)
        {
            VIRTUAL.VirtualMemoryPageNum -= siz;
            thisProcess.setPID(pid_counter);
            pid_counter++;
            vector<size_t> freeProcessInVM;
            for(size_t i=0; i<VIRTUAL.Memory.size(); i++)
            {
                if(freeProcessInVM.size()>=siz)
                {
                    break;
                }
                else if(VIRTUAL.Memory[i].ASID==0)
                {
                    freeProcessInVM.push_back(i);
                }
            }
            for(int i=0; i<freeProcessInVM.size(); i++)
            {
                thisProcess.table->VPN2PFN.insert({i, freeProcessInVM[i]});
                VIRTUAL.Memory[freeProcessInVM[i]].ResetPage(pid_counter-1);
            }  
            PROCESSMAP.insert({pid_counter-1, thisProcess});   
            output<<Files[i]<<" is loaded in virtual memory and is assigned process id "<<pid_counter-1<<endl;
        }
        else if(VIRTUAL.VirtualMemoryPageNum !=0 || MAIN.MainMemoryPageNum !=0 )
        {
            output<<Files[i]<<" could not be loaded - memory is full"<<endl;
        }
    }
    output<<endl;
    return;
}

void KillCommand(int pid)
{
    if(!isValidPID(pid))
    {
        output<<"Kill Invalid PID "<<pid<<endl;
        return;
    }
    Process thisProcess = PROCESSMAP[pid];
    for(auto &entry : thisProcess.table->VPN2PFN)
    {
        if(thisProcess.table->mainmemory)
        {
            MAIN.Memory[entry.second].ResetPage();   
            MAIN.MainMemoryPageNum++;         
        }
        else
        {
            VIRTUAL.Memory[entry.second].ResetPage();
            VIRTUAL.VirtualMemoryPageNum++;
        }
    }
    PROCESSMAP.erase(pid);
    UpdateLRU(pid, 0);
    output<<endl;
    return;
}

void ListPrCommand()
{
    set<int> ProcessInMain, ProcessInVirtual;
    for(size_t i=0; i < MAIN.Memory.size(); i++)
    {
        ProcessInMain.insert(MAIN.Memory[i].ASID);
    }
    for(size_t i=0; i < VIRTUAL.Memory.size(); i++)
    {
        ProcessInVirtual.insert(VIRTUAL.Memory[i].ASID);
    }
    output<<"ListPr called:"<<endl;
    for(auto &entry : ProcessInMain)
    {
        if(entry != 0)
        {
            output<<"\tIn Main Memory "<<entry<<endl;
        }
    }
    for(auto &entry : ProcessInVirtual)
    {
        if(entry != 0)
        {
            output<<"\tIn Virtual Memory "<<entry<<endl;
        }
    }
    output<<endl;
    return;
}

void PTECommand(int pid, string ofile, bool PrintTime = true)
{  
    ofstream putout;
    if(ofile==outputfile)
    {
        output.close();
    }
    putout.open(ofile+".txt", ios::app);
    if(PrintTime)
    {
        auto time = chrono::system_clock::now();
        auto time1 = chrono::system_clock::to_time_t(time);
        putout<<"PTE: PID = "<<pid<<" "<<ctime(&time1);
    }
    if(PROCESSMAP.find(pid)==PROCESSMAP.end())
    {
        output<<"Invalid PID PTECommand"<<endl;
        return;
    }
    Process thisProcess = PROCESSMAP[pid];
    if(thisProcess.table->mainmemory)
    {
        putout<<"PID : "<<pid<<" in Main Memory\n";
    }
    else
    {
        putout<<"PID : "<<pid<<" in Virtual Memory\n";
    }
    for(auto &Translation : thisProcess.table->VPN2PFN)
    {
        putout<<'\t'<<"Logical Page Number "<<Translation.first<<" Translates to Physical Page Number "<<Translation.second<<endl;
    }
    putout<<'\n';
    putout.close();
    if(ofile==outputfile)
    {
        output.open(outputfile, ios::app);
    }
    output<<endl;
    return;
}

void PTEAllCommand(string ofile)
{
    ofstream putout;
    if(ofile==outputfile)
    {
        output.close();
    }
    putout.open(ofile+".txt", ios::app);
    auto time = chrono::system_clock::now();
    auto time1 = chrono::system_clock::to_time_t(time);
    putout<<"PTEAll: ";
    putout<<ctime(&time1);
    putout.close();
    if(ofile==outputfile)
    {
        output.open(outputfile, ios::app);
    }
    for(auto Proc : PROCESSMAP)
    {
        int pid = Proc.first;
        PTECommand(pid, ofile, false);
    }
    output<<endl;
    return;
}

void SwapOutCommand(int pid)
{
    set<int> ProcessInMain;
    for(size_t i=0; i < MAIN.Memory.size(); i++)
    {
        ProcessInMain.insert(MAIN.Memory[i].ASID);
    }
    if(!isValidPID(pid))
    {
        output<<"SwapOut Invalid PID "<<pid<<endl;
        return;
    }        
    if(ProcessInMain.count(pid)==0)
    {
        output<<"SwapOut Invalid Input not in Main Memory. PID: "<<pid<<endl;
        return;
    }
    Process thisProcess = PROCESSMAP[pid];
    if(VIRTUAL.VirtualMemoryPageNum < thisProcess.table->VPN2PFN.size())
    {
        output<<"Insufficient Space for pid: "<<pid<<endl;
        output<<"Killing process"<<endl;
        KillCommand(pid);
        return;
    }
    vector<size_t> freeProcessInVM;
    for(size_t i=0; i<VIRTUAL.Memory.size(); i++)
    {
        if(freeProcessInVM.size()>=thisProcess.table->VPN2PFN.size())
        {
            break;
        }
        else if(VIRTUAL.Memory[i].ASID==0)
        {
            freeProcessInVM.push_back(i);
        }
    }
    int i = 0;
    thisProcess.table->setBool(false);
    for(auto &entry : thisProcess.table->VPN2PFN)
    {
        VIRTUAL.Memory[freeProcessInVM[i]] = MAIN.Memory[entry.second];
        MAIN.Memory[entry.second].ResetPage();
        entry.second = freeProcessInVM[i++];     
    }
    VIRTUAL.VirtualMemoryPageNum -= freeProcessInVM.size();
    MAIN.MainMemoryPageNum += freeProcessInVM.size();
    UpdateLRU(pid, 0);
    output<<endl;
    return;
}

void SwapInCommand(int pid)
{
    set<int> ProcessInMain;
    for(size_t i=0; i < MAIN.Memory.size(); i++)
    {
        ProcessInMain.insert(MAIN.Memory[i].ASID);
    }
    if(!isValidPID(pid))
    {
        output<<"Invalid pid in SwapInCommand "<<pid<<endl;
        return;
    }  
    if(ProcessInMain.count(pid)!=0)
    {
        output<<"SwapIn Invalid: Input already in Main Memory. PID: "<<pid<<endl;
        return;
    }
    Process thisProcess = PROCESSMAP[pid];  
    if(thisProcess.table->mainmemory)
    {
        output<<"Process PID: "<<pid<<" is already in Main Memory."<<endl;
        return;
    }
    if(thisProcess.table->VPN2PFN.size() >= MAIN.Memory.size())
    {
        output<<"Process PID: "<<pid<<" has size more than Main Memoery."<<endl;
        return;
    }
    vector<Page> buffer(thisProcess.table->VPN2PFN.size());
    int i = 0;
    for(auto &entry : thisProcess.table->VPN2PFN)
    {
        buffer[i] = VIRTUAL.Memory[entry.second];
        VIRTUAL.Memory[entry.second].ResetPage();
        entry.second = i++;    
    }
    int totalsz = MAIN.MainMemoryPageNum;
    while(!LRU.empty() && totalsz < thisProcess.table->VPN2PFN.size())
    {
        totalsz += PROCESSMAP[LRU.front()].table->VPN2PFN.size();
        SwapOutCommand(LRU.front());
        LRU.remove(*(LRU.begin()));      
    }
    vector<size_t> freeProcessInMM;
    for(size_t i=0; i<MAIN.Memory.size(); i++)
    {
        if(freeProcessInMM.size()>=thisProcess.table->VPN2PFN.size())
        {
            break;
        }
        else if(MAIN.Memory[i].ASID==0)
        {
            freeProcessInMM.push_back(i);
        }
    }
    i = 0;
    thisProcess.table->setBool(true);
    for(auto &entry : thisProcess.table->VPN2PFN)
    {
        MAIN.Memory[freeProcessInMM[i]] = buffer[entry.second];
        entry.second = freeProcessInMM[i++];     
    }
    MAIN.MainMemoryPageNum -= freeProcessInMM.size();
    VIRTUAL.VirtualMemoryPageNum += freeProcessInMM.size();
    output<<endl;
    return;
}


void RunCommand(int pid)
{
    if(!isValidPID(pid))
    {
        output<<"Invalid pid in Run PID: "<<pid<<endl;
        return;
    }        
    Process thisProcess = PROCESSMAP[pid];
    if(thisProcess.table->mainmemory==false)
    {
        SwapInCommand(pid);
    }
    thisProcess.RunProcess();
    UpdateLRU(pid, 1);
    output<<endl;
    return;
}

void PrintCommand(int memloc, int length)
{
    int basePage = memloc/P;
    int offset = memloc%P;
    int cnt = 0;
    while(cnt<length)
    {
        output<<"Value of "<<(basePage*P + offset)<<": "<<MAIN.Memory[basePage].getContent(offset)<<endl;
        cnt++;
        offset++;
        basePage += offset/P;
        offset %= P;
    }
    output<<endl;
    return;
}

void ExitCommand()
{
    cout<<"exit"<<endl;
    output.close();
    exit(0);
}

//--------------------------------------------------------------------------------------------------------------------------------


void Execute(string infile, string outfile)
{
    ifstream input;
    input.open(infile);
    if(!input.is_open())
    {
        cout<<"Could not open input file\n";
        exit(0);
    }
    output.open(outfile);
    outputfile = outfile;
    if(!output.is_open())
    {
        cout<<"Could not open output file\n";
        exit(0);
    }
    string line, cmd, temp;
    while(!input.eof())
    {
        getline(input, line);
        line += '\n';
        int i = 0;
        cmd = "";
        temp = "";
        while(line[i]!=' ' && line[i]!='\n')
        {
            cmd += line[i];
            i++;            
        }
        if(cmd=="load")
        {
            vector<string> Files;
            string lin = "";
            for(int i=0; i<line.size(); i++)
            {
                if(line[i]!='\n')
                {
                    lin += line[i];
                }
            }
            string temp;
            stringstream ss(lin);
            while(getline(ss, temp, ' '))
            {
                Files.push_back(temp);                
            }
            reverse(Files.begin(), Files.end());
            Files.pop_back();
            reverse(Files.begin(), Files.end());
            LoadCommand(line, Files, Files.size());
        }
        else if(cmd == "run")
        {
            i++;
            while(line[i]!='\n')
            {
                if(line[i]!=' ')
                {
                    temp += line[i];
                }
                i++;
            }
            RunCommand(S2UI(temp));
        }
        else if(cmd=="kill")
        {
            i++;
            while(i<line.length() && line[i]!='\n')
            {
                if(line[i]!=' ')
                {
                    temp += line[i];
                }
                i++;
            }
            KillCommand(S2UI(temp));
        }
        else if(cmd=="listpr")
        {
            ListPrCommand();
        }
        else if(cmd=="pte")
        {
            i++;
            int pid = 0;
            while(line[i]!=' ')
            {
                temp += line[i];
                i++;
            }
            pid = S2UI(temp);
            temp = "";
            while(line[i]!='\n')
            {
                if(line[i]!=' ')
                {
                    temp += line[i];
                }
                i++;
            }
            PTECommand(pid, temp);
        }
        else if(cmd=="pteall")
        {
            temp = "";
            while(line[i]!='\n')
            {
                if(line[i]!=' ')
                {
                    temp += line[i];
                }
                i++;
            }
            PTEAllCommand(temp);
        }
        else if(cmd=="swapout")
        {
            temp = "";
            while(line[i]!='\n')
            {
                if(line[i]!=' ')
                {
                    temp += line[i];
                }
                i++;
            }
            SwapOutCommand(S2UI(temp));
        }
        else if(cmd=="swapin")
        {
            temp = "";
            while(line[i]!='\n')
            {
                if(line[i]!=' ')
                {
                    temp += line[i];
                }
                i++;
            }
            SwapInCommand(S2UI(temp));
        }
        else if(cmd=="print")
        {
            i++;
            temp = "";
            while(line[i]!=' ')
            {
                temp += line[i];
                i++;
            }
            string x = temp;
            temp = "";
            i++;
            while(line[i]!='\n')
            {
                if(line[i]!=' ')
                {
                    temp += line[i];
                }
                i++;
            }
            // cout<<x<<" "<<temp<<endl;
            PrintCommand(S2UI(x), S2UI(temp));
        }
        else if(cmd=="exit")
        {
            input.close();
            ExitCommand();
            exit(0);
        }
        else
        {
            input.close();
            cout<<"Incorrect Command : "<<cmd<<" detected.\n";
            cout<<"EXITING\n";
            exit(1);
        }
    }
    return;
}

int main(int argc, char * argv[])
{
    string infile;
    string outfile;
    for(int i=0; i<argc; i++)
    {
        if(strcmp(argv[i], "-M")==0)
        {
            M = S2UI(argv[i+1]);
        }
        else if(strcmp(argv[i], "-V")==0)
        {
            V = S2UI(argv[i+1]);
        }
        else if(strcmp(argv[i], "-P")==0)
        {
            P = S2UI(argv[i+1]);
        }
        else if(strcmp(argv[i], "-i")==0)
        {
            infile = argv[i+1];
        }
        else if(strcmp(argv[i], "-o")==0)
        {
            outfile = argv[i+1];
        }
    }
    MAIN.Memory.resize((1024*M)/P);
    VIRTUAL.Memory.resize((1024*V)/P);
    MAIN.MainMemoryPageNum = (1024*M)/P;
    VIRTUAL.VirtualMemoryPageNum = (1024*V)/P;
    Execute(infile, outfile);
    return 0;
}