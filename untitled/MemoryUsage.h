#pragma once

#include <ios>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sstream>

using namespace std;

//////////////////////////////////////////////////////////////////////////////
//
// process_mem_usage(double &, double &) - takes two doubles by reference,
// attempts to read the system-dependent data for a process' virtual memory
// size and resident set size, and return the results in KB.
//
// On failure, returns 0.0, 0.0


/*
* convert from an integer to a string
*/
string intToStr(int i) {
        stringstream ss;
        ss << i;
        return ss.str();
}

/*
* convert from a strong to an integer
*/
unsigned int strToInt(string s) {
        unsigned int i;
        istringstream myStream(s);

        if (myStream>>i) {
                return i;
        } else {
             cout << "String " << s << " is not a number." << endl;
             return atoi(s.c_str());
        }
        return i;
}
/*
* measure the consumed memory
*/
void disp_mem_usage() {

        string pid = intToStr(unsigned(getpid()));
        string outfile = "tmp_" + pid + ".txt";
        string command = "pmap " + pid + " | grep -i Total | awk '{print $2}' > " + outfile;
        system(command.c_str());
	
        string mem_str;
        ifstream ifs(outfile.c_str());
        std::getline(ifs, mem_str);
        ifs.close();

        mem_str = mem_str.substr(0, mem_str.size()-1);
        float mem = (float)strToInt(mem_str);

	command = "rm " + outfile;
        system(command.c_str());

     //   return mem/1024;
	 cout << "Memory: " << mem/1024 << " MB" << endl;

     //   return 0;
}
float getCurrentMemoryUsage() {

        string pid = intToStr(unsigned(getpid()));
        string outfile = "tmp_" + pid + ".txt";
        string command = "pmap " + pid + " | grep -i Total | awk '{print $2}' > " + outfile;
        system(command.c_str());

        string mem_str;
        ifstream ifs(outfile.c_str());
        std::getline(ifs, mem_str);
        ifs.close();

        mem_str = mem_str.substr(0, mem_str.size()-1);
        float mem = (float)strToInt(mem_str);

	command = "rm " + outfile;
        system(command.c_str());

        return mem/1024;
     //   cout << "Memory: " << mem/1024 << " MB" << endl;

        return 0;
}

/*
void process_mem_usage(double& vm_usage, double& resident_set)
{

   vm_usage     = 0.0;
   resident_set = 0.0;

   // 'file' stat seems to give the most reliable results
   //
   ifstream stat_stream("/proc/self/stat",ios_base::in);//read file

   // dummy vars for leading entries in stat that we don't care about
   //虚拟变量
   string pid, comm, state, ppid, pgrp, session, tty_nr;
   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   string utime, stime, cutime, cstime, priority, nice;
   string O, itrealvalue, starttime;

   // the two fields we want
   //
   unsigned long vsize;
   long rss;

   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
               >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
               >> utime >> stime >> cutime >> cstime >> priority >> nice
               >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

   stat_stream.close();
   //sysconf:通过相关选项，来获取编译或者运行时的一些系统参数值
   long page_size_kb = 1024;//sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
   vm_usage     = vsize / 1024.0;
   resident_set = rss * page_size_kb;
}
void disp_mem_usage(string msg){
    double vm, rss;

    process_mem_usage(vm, rss);
    vm/=1024;
    rss/=1024;
    //cout<< "Memory Usage:" << msg << " vm:" << vm << " MB  rss:" << rss << " MB"<<endl;
    cout<< "Memory Usage:" << msg << " " << rss << " MB"<<endl;
}*/
