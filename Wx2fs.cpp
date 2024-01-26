// Wx2fs.cpp : This file contains the 'main' function. Program execution begins and ends there.



#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>
#include <time.h>
using namespace std;

#include "SimConnect.h"

int     quit = 0;
HANDLE  hSimConnect = NULL;
//static vector<string> layer_alt = { "30","60","90","120","180","240","300","340","390" }; //This is the measurement altitude
static vector<string> layer_alt = { "45","75","105","150","210","270","320","365","415" }; //This is the "top of the wind layer"



/* Generic functions  */


//Returns a vector with every line of the inputed file
vector<string> parse_file(string path)
{
    vector<string> lines;
    string inFileName = path;
    ifstream inFile;
    inFile.open(inFileName.c_str());
    if (inFile.is_open())
    {
        string line;
        while (getline(inFile, line)) {
            lines.push_back(line);
        }
        inFile.close(); // CLose input file
        return lines; 
    }
    else { //Error message
        cerr << "Can't find input file " << inFileName << endl;
        //exit;
        return lines;
    }

}

//Split given string line at separator
vector <string> parse_line(string line, char delim) {
    vector<string> out;
    string str;
    stringstream ss(line);
    while (getline(ss, str, delim))
    {
        //if (delim == '/')
            //cout << str << '\n';
        out.push_back(str);
    }
    return out;

}


//Returns a unique 4 letter code coresponding to an index
string gen_code(int number) {
    vector<char> dic = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z' };
    vector<char> prefix = { 'I','J','X','Q' };
    vector<int> div = { 46656,1296,36 };
    vector<int> index(4);

    index.at(0) = number / div.at(0);
    index.at(1) = (number - index.at(0) * div.at(0)) / div.at(1);
    index.at(2) = (number - index.at(0) * div.at(0) - index.at(1) * div.at(1)) / div.at(2);
    index.at(3) = number - index.at(0) * div.at(0) - index.at(1) * div.at(1) - index.at(2) * div.at(2);
    // cout << index.at(0) << " " << index.at(1) << " " << index.at(2) << " " << index.at(3) << '\n';
    string out;
    out += prefix.at(index.at(0));
    out += dic.at(index.at(1));
    out += dic.at(index.at(2));
    out += dic.at(index.at(3));

    return out;
}


//Removes a given word from a string
string removeWord(string str, string word)
{

    if (str.find(word) != string::npos) {
        size_t p = -1;


        string tempWord = word + "";//removed space
        while ((p = str.find(word)) != string::npos)
            str.replace(p, tempWord.length(), "");


        tempWord = "" + word;//removed space
        while ((p = str.find(word)) != string::npos)
            str.replace(p, tempWord.length(), "");
    }
    return str;
}


//Splits a vector of strings into multiple vector columns at given separator
vector<vector<string>> split_vector(vector<string> lines,int num_col, char delim)
{
    string str;
    int len = lines.capacity();
    vector<vector<string>> output(num_col,vector<string>(len));
    //vector<string> temp(num_col);
    int k;

    for (int i = 0; i < len; i++) {

        vector<string> line = parse_line(lines.at(i), delim);
        if (int(line.capacity()) < num_col) {
            //cout << to_string(num_col);
            //cout << to_string(line.capacity());
            //cout << to_string(i);
            line = vector<string>(num_col);
        }
           
        for (k = 0; k < num_col; k++) {
            output.at(k).at(i) = line.at(k);
        }
       /* k = 0;
        stringstream ss(lines.at(i));
        while (getline(ss, str, delim))
        {
            output.at(k).at(i) = str;
            //cout << str << "\n";
            k++;
        }*/
        
    }
    return output; 
}



/* APP specific functions   */

//Cleans the raw lines, (removes single : separator)
vector<string> clean_raw_lines(vector<string> lines) {

    int len = lines.capacity();
    int N;
    for (int i = 0; i < len; i++) {
        string line = lines.at(i);
        N = line.length();
        for (int k = 0; k < N; k++) {
            if (line[k] == ':') {
                if (line[k + 1] != ':')
                    line[k] = ' ';
                else
                    k++;
            }
        }
        lines.at(i) = line;

    }
    return lines;
}


//Change the format of the wind
vector<string> format_winds(vector<string> winds_raw,int n_layers) {
    
    int len = winds_raw.capacity();
    //cout << len << "\n";
    vector<string> out(len);
    vector<string> layer(n_layers);
    vector<vector<string>> winds = split_vector(winds_raw, n_layers, '/');
    //cout << winds.capacity() << "\n";
    for (int i = 0; i < len; i++) {
        layer.clear();
        out.at(i) = "@@@ ";
        for (int k = 0; k < n_layers; k++) {
            layer = parse_line(winds.at(k).at(i), ',');
            
            if (layer.capacity() != 3) {
               // cout << layer.capacity() << "\n";
                //cout << i << "\n";
                break;
            }
            out.at(i) = out.at(i)+layer_alt.at(k)+" " + layer.at(2) + " " + layer.at(0) + " " + layer.at(1) + "|";

        }
        //cout << out.at(i) << "\n";
    }
    return out;

}


//Cleans metar data
vector<string> format_metar(vector<string> metars) {
    int len = metars.capacity();
    for (int i = 0; i < len; i++) {
        metars.at(i) = removeWord(metars.at(i), "AUTO ");
        metars.at(i) = removeWord(metars.at(i), "$");
        metars.at(i) = removeWord(metars.at(i), " //////");
        metars.at(i) = removeWord(metars.at(i), "//// ");
        metars.at(i) = removeWord(metars.at(i), " ///");
       metars.at(i) = removeWord(metars.at(i), " //");
        metars.at(i) = removeWord(metars.at(i), " TCU");
        metars.at(i) = removeWord(metars.at(i), " CB");
        metars.at(i) = removeWord(metars.at(i), " NCD");
        metars.at(i) = removeWord(metars.at(i), " NSC");
        metars.at(i) = removeWord(metars.at(i), " COR");
        
        string s = "RMK";
        size_t p = -1;
        if ((p = metars.at(i).find(s)) != string::npos) {
            metars.at(i).erase(p, string::npos);
        }
    }

    return metars;
}


//Injects the data into the sim
bool inject_weather(vector<vector<string>> stations, vector<vector<string>> current_wx) {
    HRESULT hr = NULL;
    if (SUCCEEDED(SimConnect_Open(&hSimConnect, "Weather Station", NULL, 0, 0, 0)))
    {
        printf("Connected to Flight Simulator!\n");
        SimConnect_WeatherSetModeCustom(hSimConnect);

        DWORD delay_weather = 0;
        int len = stations.at(0).capacity();
        
        
        for (int i = 0; i < len; i++) {
            
            string id = stations.at(0).at(i);
            string metar = current_wx.at(2).at(i);
            
            if (id[0] == '$') {
                id = gen_code(i);
                metar = id;
            }
            if (metar != "*" ){//&& i != 6140 && i != 14326 && i != 12959 && i != 12205 && i != 11155 && i != 12194 && i != 12468) {
                //cout << metar<<"\n";
                hr = SimConnect_WeatherCreateStation(hSimConnect, 1, id.c_str(), id.c_str(), stof(stations.at(1).at(i)), stof(stations.at(2).at(i)), 0.0F);
                string  str = metar + " " + current_wx.at(6).at(i);
             
                hr = SimConnect_WeatherSetObservation(hSimConnect, 0, str.c_str());
            }

            
        }
        printf("All stations updated! \nPlease keep this window open...\n");
        
        SimConnect_WeatherSetModeCustom(hSimConnect);
        return 1;
      

    }
    else {
        printf("Couldn't connect to SIM !\n");
        return 0;
    }
        
}


//Just a test function to inject data in the sim and see how it behave
void inject_test() {
    HRESULT hr = NULL;


    if (SUCCEEDED(SimConnect_Open(&hSimConnect, "Weather Station", NULL, 0, 0, 0)))
    {
        DWORD delay_weather = 0;
        printf("\nConnected to simulator!");                           
        
        //LSGG 220855Z 10006G13KT 10SM OVC032 M01/M04 A3020 RMK AO28 @@@ 65 25 270 20 | 196 5 090 125 | 340 -50 180 190
            
        for (int i = 0; i < 10; i++) {
            hr = SimConnect_WeatherCreateStation(hSimConnect, 1, ("D00"+std::to_string(i)).c_str(), ("D00" + std::to_string(i)).c_str(), 46.5F + float(i)/10, 6.6F, 0.0F);
        }
        for (int i = 0; i < 10; i++) {

            string  str = "D00"+std::to_string(i)+" 220855Z 10006G13KT 10SM OVC032 0"+std::to_string(i)+"/M04 Q1000 RMK AO28 @@@ 65 25 270 20 | 196 5 090 125 | 340 -50 180 190";
            if(i==10646)
                printf(str.c_str());
            hr = SimConnect_WeatherSetObservation(hSimConnect, 0, str.c_str());

        }if (hr == E_FAIL)
                printf("fail!");
            else if(hr == S_OK)
                printf("success !");

            SimConnect_WeatherSetModeCustom(hSimConnect);
            //SimConnect_WeatherSetModeGlobal(hSimConnect);

            printf("Sim weather updated, please keep window open...\n");
            while (1);
     
    }   
    else {
        printf("Not connected to SIM !\n");
        

    }
        
}

//Start autosave at requested interval
void start_timer(int delay) {
    std::chrono::minutes minutes(delay);
    HRESULT hr = NULL;
    int k = 0;
    time_t my_time = time(NULL);
    while (true) {
        std::this_thread::sleep_for(minutes);
      
       
        string file = "./wx2fs_save_" + to_string(k % 3);
        string name = "Wx2fs_save_" + std::format("{:%R}",
            std::chrono::zoned_time{ std::chrono::current_zone(),
                                    std::chrono::system_clock::now() });
        string descr = "Saved : " + std::format("{:%F %R}",
            std::chrono::zoned_time{ std::chrono::current_zone(),
                                    std::chrono::system_clock::now() });
       
        hr = SimConnect_FlightSave(hSimConnect,file.c_str(), name.c_str(), descr.c_str(), 1);
       if (hr == S_OK)
           cout << "Saved : "<< std::format("{:%R}",
               std::chrono::zoned_time{ std::chrono::current_zone(),
                                       std::chrono::system_clock::now() }) << "\n";
       else
           cout << "Error !?";
       k++;

    }
}



int main()
{

    std::cout << "OPEN THIS SCRIPT BEFORE LOADING THE FLIGHT\n";
    std::cout << "Importing data from output folder...\n";

    //Reads and cleans file data
    vector<string> stations_raw = parse_file("./output/wx_station_list.txt");
    //cout << stations_raw.at(3000) <<"\n";
    vector<string> current_wx_raw = parse_file("./output/current_wx_snapshot.txt");
    //cout << current_wx_raw.at(10260) << "\n";
    current_wx_raw = clean_raw_lines(current_wx_raw);
    //cout << current_wx_raw.at(10260) << "\n";

    //Refactor data
    vector<vector<string>> stations = split_vector(stations_raw,4, ',');
    vector<vector<string>> current_wx = split_vector(current_wx_raw,7, ':');
    //cout << stations.at(2).at(3000) << "\n";
    //cout << current_wx.at(6).at(3000) << "\n";
    
    
    //Clean and format data
    current_wx.at(6) = format_winds(current_wx.at(6),9);
    //cout << current_wx.at(6).at(3000) << "\n";
    current_wx.at(2) = format_metar(current_wx.at(2));
    //cout << current_wx.at(2).at(11773) << "\n";
    
    
    //Injects the weather
    bool success = inject_weather(stations,current_wx);
    //inject_test();
    if (success)
    {
        cout << "Enter an autosave interval in minutes (optionnal) :";

        int delay;
        cin >> delay;
        if (delay < 1)
            delay = 1;
        cout << "The flight will now be saved every " << delay << " minutes...\n";
        start_timer(delay);
    }
    else
        while (1);

 
}

