#include <wiringPi.h>
#include "Onkyo_send_blocking/OnkyoRI.h"
#include <iostream>
#include <vector> 
#include <chrono>
#include <thread>
#include "stdlib.h"
 
using namespace std;

std::vector<std::string> split(std::string str, std::string token){
    std::vector<std::string>result;
    while(str.size()){
        int index = str.find(token);
        if(index!=std::string::npos){
            result.push_back(str.substr(0,index));
            str = str.substr(index+token.size());
            if(str.size()==0)result.push_back(str);
        }else{
            result.push_back(str);
            str = "";
        }
    }
    return result;
}


bool file_exists (char *filename) {
  struct stat   buffer;   
  return (stat (filename, &buffer) == 0);
}



int main(int argc, char** argv) {
    
    const lockfile = ".LOCK";
    int fd;
    if (file_exists(lockfile)){
        throw throw std::runtime_error("Application can not have multiple instances!!");
        return EXIT_FAILURE;
    }
    else{
     fd = open(lockfile, O_CREAT | S_IRUSR | S_IWUSR);
        if (fd == -1) {
            std::runtime_error("Could not create LOCK file.");
            return EXIT_FAILURE;
        }
        else {
            close(fd);
        }
    }

    std::vector<string> args(argv + 1, argv + argc);
    string commands;
    int pin;
    int delay;
    string s, tmp; 
    vector<string> commandsVector; 
    pin = 6; // Used pin (WiringPiSchema)
    delay= 100; // in milliseconds 
    for (auto i = args.begin(); i != args.end(); ++i) {
        if (*i == "-h" || *i == "--help") {
            cout << "Syntax: " << argv[0] << " -p <pin [6]> -c <command(s) [0x20(,0x21)]>" << endl;
            return 0;
        } else if (*i == "-c") {
	    commands = *++i;
	    commandsVector = split(commands, ",");
        } else if (*i == "-p") {
            pin =  std::stoi(*++i, nullptr, 0);
        }
    }


   
 
 cout << "GPIO Pin is " << pin << endl;

 wiringPiSetup();
 OnkyoRI ori(pin);

   for(auto const& commandString: commandsVector) {
	int commandInteger = std::stoi(commandString, nullptr, 0);
	cout << "Sending command: " << commandInteger << endl;
	ori.send(commandInteger);	
	std::this_thread::sleep_for(std::chrono::milliseconds(delay)); 
   }

   std::remove(lockfile)
   return EXIT_SUCCESS;

}

