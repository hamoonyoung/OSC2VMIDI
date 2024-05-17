#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <ncurses.h>
#include <vector>

#include "UDPServer.h"
#include "VirtualMidiDevice.h"

WINDOW * input, * output;
std::map<std::string, std::vector<std::string>> dataMap;

VirtualMIDIDevice virtualMidiDevice;

enum cli_code {
    error,
    quit,
    setting,
    ip,
    port
};

enum tag_code {
    Tag1,
    Tag2,
    Tag3,
    Tag4,
    Tag5,
    Tag6,
    Tag7,
    Tag8
};

cli_code hashit(const std::string &inString);
tag_code hashTag(const std::string &inString);

static void onDataReceived(char * msg, int msg_size);
std::string trim(const std::string& str);
void printSettingData();
void printIPAddress(std::vector<std::string>);
static void onMidiReceived(std::string deviceName, int channel, int noteNumber, int velocity);

int main (int argc, char* argv[])
{
    juce::ignoreUnused (argc, argv);
    juce::ScopedJuceInitialiser_GUI gui_init; // even if this isn't a GUI app, the Juce message manager needs to be running

    int c;
    char * value_p;
    char * value_f;
    value_p = nullptr;
    value_f = nullptr;
    bool done = FALSE;
    char buffer [1024];

    while ((c = getopt (argc, argv, "p:f:")) != -1) {
        switch (c) {
            case 'p':
                value_p = optarg;
                printf("Port#: %s\n", value_p);
                break;
            case 'f':
                value_f = optarg;
                printf("Setting File: %s\n", value_p);
            default:
                break;
        }
    }
    if (value_p == nullptr) {
        printf("USAGE: -p port#\n");
        return 0;
    }

    std::ifstream fs;
    fs.open(value_f);
    if (!fs.is_open()){
        std::printf ("Error opening file. %s is not existing\n", value_f);
        return 1;
    }

    std::string line;
    while (std::getline(fs, line)){
        std::istringstream iss(line);
        std::string key, value;

        if (std::getline (iss, key, ',')){
            key = trim(key);
        }else{
            continue;
        }

        while (std::getline(iss, value, ',')){
            dataMap[key].push_back (trim (value));
        }
    }

    UDPServer udpServer (value_p);
    udpServer.setCallback(onDataReceived); // Set callback function
    udpServer.start(); // Start receiving data

    virtualMidiDevice.midiInputCallback.setCallback(onMidiReceived);

    initscr();
    cbreak();
    echo();
    input = newwin(1, COLS, LINES - 1, 0);
    output = newwin(LINES - 1, COLS, 0, 0);
    wmove(output, LINES - 2, 0);    /* start at the bottom */
    scrollok(output, TRUE);

    while (!done) {
        mvwprintw(input, 0, 0, "(OSC2VMIDI) ");
        if (wgetnstr(input, buffer, COLS - 4) != OK) {
            break;
        } // not sure what it does, but it requires.....
        werase(input);
        switch (hashit(buffer)){
            case quit:
                wrefresh(output);
                endwin();
                exit (0);
                break;
            case port:
                waddch(output, '\n');
                waddstr (output, "Port: ");
                waddstr (output, value_p);
                break;
            case setting:
                printSettingData();
                break;
            case ip:
                printIPAddress(udpServer.ipAddresses);
                break;
            case error:
            default:
                if (buffer[0] != '\0') {
                    waddch(output, '\n');
                    waddstr (output, "command not found: ");
                    waddstr (output, buffer);
                }
                break;
        }
        wrefresh(output);
        done = (*buffer == 4);  /* quit on control-D */
    }
    endwin();
    return 0;
}

cli_code hashit(const std::string &inString) {
    if (inString == "q") return quit;
    if (inString == "quit") return quit;
    if (inString == "p") return port;
    if (inString == "port") return port;
    if (inString == "setting") return setting;
    if (inString == "s") return setting;
    if (inString == "ip") return ip;
    return error;
}
tag_code hashTag(const std::string &inString){
    if (inString == "Tag1") return Tag1;
    if (inString == "Tag2") return Tag2;
    if (inString == "Tag3") return Tag3;
    if (inString == "Tag4") return Tag4;
    if (inString == "Tag5") return Tag5;
    if (inString == "Tag6") return Tag6;
    if (inString == "Tag7") return Tag7;
    if (inString == "Tag8") return Tag8;
    return Tag1;
}

static void onDataReceived(char * msg, int msg_size) {
    int OSCAddressEnd = 0;
    int OSCTagEnd = 0;
    for (int k = 0; k < msg_size; k++){
        if ((msg[k] == ',') && (OSCAddressEnd == 0 )){
            OSCAddressEnd = k-1;
        }
        if ((OSCAddressEnd > 0) && (OSCTagEnd == 0)){
            if (msg[k] == '\0') {
                OSCTagEnd = k + (3 - (k % 4));
            }
        }
    }
    waddch(output, '\n');   /* result from wgetnstr has no newline */
    char * OSCAddress;
    OSCAddress = (char *) malloc(static_cast<size_t>(OSCAddressEnd + 2));
    memcpy(OSCAddress, msg,(static_cast<size_t>(OSCAddressEnd + 1)));
    waddstr(output, OSCAddress);
    char * OSCTag;
    OSCTag = (char *) malloc(static_cast<size_t>(OSCTagEnd - OSCAddressEnd + 1));
    memcpy(OSCTag, msg+(OSCAddressEnd + 1),(static_cast<size_t>(OSCTagEnd - OSCAddressEnd)));
    waddstr(output, OSCTag);
    wrefresh(output);

    std::vector<int> convertedValues;
    for (int k=1; k < (OSCTagEnd - OSCAddressEnd); k++){
        char temp_tag = *(OSCTag + k);
        if (temp_tag != '\0') {
            if(temp_tag == 'f'){
                uint32_t i_temp0;
                std::memcpy (&i_temp0, (msg+ OSCTagEnd + 1 + (sizeof(float) * (static_cast<unsigned long>(k - 1)))), sizeof(float));
                uint32_t i_temp1 = ntohl(i_temp0);
                float f_temp=0.0;
                std::memcpy(&f_temp, &i_temp1, 4);
                std::string temp_f = std::to_string (f_temp);
                waddch(output, ' ');
                waddstr(output, temp_f.c_str());
                int i_temp2;
                i_temp2 = static_cast<int>(static_cast<int>(f_temp * 128));
                convertedValues.push_back(i_temp2);
            }
            if(temp_tag == 'i'){
                uint32_t i_temp0;
                std::memcpy (&i_temp0, (msg+ OSCTagEnd + 1 + (sizeof(float) * (static_cast<unsigned long>(k - 1)))), sizeof(float));
                uint32_t i_temp1 = ntohl(i_temp0);
                int32_t i_temp3=0;
                std::memcpy(&i_temp3, &i_temp1, 4);
                std::string temp_i = std::to_string (i_temp3);
                waddch(output, ' ');
                waddstr(output, temp_i.c_str());
                convertedValues.push_back(i_temp3);
            }
            wrefresh(output);
        }
    }

    if ( dataMap.find(OSCAddress) != dataMap.end()) {
        waddch(output, '|');
        for (auto i: dataMap.at(OSCAddress)) {
            waddstr(output, i.c_str());
            waddch(output, ' ');
        }
        waddch(output, '|');
        for (auto j: convertedValues){
            waddstr(output, std::to_string(j).c_str());
            waddch(output, ' ');
        }
        waddch(output, '|');
        waddstr(output, dataMap.at(OSCAddress)[1].c_str());
        waddch(output, ' ');
        waddstr(output, std::to_string(convertedValues[hashTag(dataMap.at(OSCAddress)[2])]).c_str());
        waddch(output, ' ');
        waddstr(output, std::to_string(convertedValues[hashTag(dataMap.at(OSCAddress)[3])]).c_str());
        waddch(output, ' ');
        waddstr(output, std::to_string(convertedValues[hashTag(dataMap.at(OSCAddress)[4])]).c_str());
        waddch(output, ' ');

        virtualMidiDevice.midiSender.sendMidiNoteMessage(std::stoi(dataMap.at(OSCAddress)[1]), convertedValues[hashTag(dataMap.at(OSCAddress)[2])], convertedValues[hashTag(dataMap.at(OSCAddress)[3])]);

        //virtualMidiDevice.midiSender.sendMidiCCMessage(std::stoi(dataMap.at(OSCAddress)[1]), convertedValues[hashTag(dataMap.at(OSCAddress)[2])], convertedValues[hashTag(dataMap.at(OSCAddress)[3])]);
    }
    wrefresh(output);

}

static void onMidiReceived(std::string deviceName, int channel, int noteNumber, int velocity) {
    waddch(output, '\n');
    waddstr(output, deviceName.c_str());
    waddch(output, ':');
    waddstr(output, std::to_string(channel).c_str());
    waddch(output, ':');
    waddstr(output, std::to_string(noteNumber).c_str());
    waddch(output, ':');
    waddstr(output, std::to_string(velocity).c_str());
    wrefresh(output);
}

// Function to trim whitespace from a string
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

void printSettingData(){
    for (const auto& pair : dataMap) {
        waddch(output, '\n');
        waddstr(output, pair.first.c_str());
        //std::cout << pair.second << std::endl;
        for (auto i: pair.second) {
            waddch(output, ',');
            waddstr(output, i.c_str());
        }
    }
    wrefresh(output);
}

void printIPAddress(std::vector<std::string> ipAddresses){
    for (auto i: ipAddresses){
        waddch(output, '\n');
        waddstr(output, i.c_str());
    }
}

