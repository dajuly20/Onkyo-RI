#include <wiringPi.h>
#include "Onkyo_send_blocking/OnkyoRI.h"
#include <iostream>
#include <vector>
#include <thread>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

using namespace std;
const string lockfile = "onkyoricli.LOCK";

std::vector<std::string> split(std::string str, std::string token)
{
    std::vector<std::string> result;
    while (str.size())
    {
        int index = str.find(token);
        if (index != std::string::npos)
        {
            result.push_back(str.substr(0, index));
            str = str.substr(index + token.size());
            if (str.size() == 0)
                result.push_back(str);
        }
        else
        {
            result.push_back(str);
            str = "";
        }
    }
    return result;
}

bool file_exists(string filename)
{
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

int printHelp(string location)
{
    system("gpio readall");

    cout << "\033[1;31mUSE WIRING PI NUMBERS\033[0m\n"
         << endl;
    cout << "See WiringPi PINs: https://pinout.xyz/pinout/wiringpi" << endl
         << "OR: run gpio readall" << endl
         << "OR: view readme.MD / open img/wiringPi.jpeg" << endl
         << endl
         << endl;
    cout << "Syntax: " << location << " -p <WiringPi-Pin [6]> -c <command(s) [0x20(,0x1A0)]>" << endl;

    return EXIT_SUCCESS;
}

int aquireLock(string lockfile)
{
    int fd;
    if (file_exists(lockfile))
    {
        throw std::runtime_error("Seems like the program is running already. Try again in a moment. If this is permanent it might be a DEADLOCK. To Remove Lock: rm  " + lockfile);
        return EXIT_FAILURE;
    }
    else
    {

        if (mknod(lockfile.c_str(), S_IFREG | 0666, 0) != 0)
        {
            std::runtime_error("Could not create LOCK file.");
            return EXIT_FAILURE;
        }
        else
        {
            close(fd);
        }
    }
    return EXIT_SUCCESS;
}

void releaseLock()
{
    std::remove(lockfile.c_str());
    cout << lockfile << " was removed!" << endl;
}

static void sigintHandler(int sig)
{
    releaseLock();
    cout << "Caught SIGINT for cleanup" << endl;
    exit(1);
}

int main(int argc, char **argv)
{
    atexit(releaseLock);
    signal(SIGINT, sigintHandler);
    std::vector<string> args(argv + 1, argv + argc);
    string commands;
    int pin;
    int delay;
    string s, tmp;
    vector<string> commandsVector;
    pin = 6;     // Used pin (WiringPiSchema)
    delay = 100; // in milliseconds
    // cout << "Args:" << argc << endl;
    if (argc == 1)
        return printHelp(argv[0]); // When there are 0 argumentsx
    for (auto i = args.begin(); i != args.end(); ++i)
    {
        if (*i == "-c")
        {
            //cout << *i << " end is " << args.end() <<  endl;
            ++i;
            if(i != args.end()){
                commands = *i;
                commandsVector = split(commands, ",");
            }
        }
        else if (*i == "-p")
        {
            pin = std::stoi(*++i, nullptr, 0);
        }
        else
        {
            return printHelp(argv[0]);
        }
    }


    if(commandsVector.size() == 0){
        cout << "No command(s) specified" << endl;
        return EXIT_FAILURE;
    } else {
        cout << commandsVector.size() << " gross" << endl;
    }

    if (aquireLock(lockfile) == EXIT_FAILURE)
        return EXIT_FAILURE;
    cout << "Using WiringPi Pin: " << pin << endl;

    wiringPiSetup();
    OnkyoRI ori(pin);

    for (auto const &commandString : commandsVector)
    {
        int commandInteger = std::stoi(commandString, nullptr, 0);
        cout << "Sending command: " << commandInteger << endl;
        ori.send(commandInteger);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }

    return EXIT_SUCCESS;
}
