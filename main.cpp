#include <wiringPi.h>
#include "Onkyo_send_blocking/OnkyoRI.h"
#include <iostream>
#include <vector> 

int main(int argc, char** argv) {

using namespace std;

    std::vector<string> args(argv + 1, argv + argc);
    int command;
    int pin;
    command = 0;
    pin = 18;

    // Loop over command-line args
    // (Actually I usually use an ordinary integer loop variable and compare
    // args[i] instead of *i -- don't tell anyone! ;)
    for (auto i = args.begin(); i != args.end(); ++i) {
        if (*i == "-h" || *i == "--help") {
            cout << "Syntax: onkyopiri -c <command> -p <pin>" << endl;
            return 0;
        } else if (*i == "-c") {
            command =  std::stoi(*++i, nullptr, 0);
        } else if (*i == "-p") {
            pin =  std::stoi(*++i, nullptr, 0);
        }
    }

   if(command == 0){
	cout << "No command specified, use -h for help" << endl;
	return 0;   
   }

   

   wiringPiSetup();

  cout << " Pin is " << pin << " command is:" << command << endl;

 OnkyoRI ori(pin);

 ori.send(command);
// ori.send(0x1AE);
}
