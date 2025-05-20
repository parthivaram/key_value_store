#include<iostream>
#include<unordered_map>
#include<fstream>
#include<filesystem>
#include<sstream>
#include<vector>
using namespace std;

bool checklog(const string& filename);
void append(const string& filename, string& content);
// core operations are "set", "get", "delete". Create an unordered map to store the key value pairs - phase 1
// adding persistance to the current implementation:
// check if a text file named log in present in the working directory, if not create and open it append mode.
// on every restart, read the commands in the log file line by line to replay them. This will reslt in regaining the previous state.
// some problem with replay function 
string filename = "log";
string backup = "snapshot";
class kvstore
{
    private:
    unordered_map<string, string> db;
    public:
    void set(string key, string value){
        if(db.find(key) != db.end()){
            auto k = db.find(key);
            k->second = value;
            cout<< "value for the key is updated"<<endl;
        }
        else{
            db.insert({key, value});
            cout<< "New key value pair added..."<<endl;
        }   
    }
    void get(string key){
        if(db.find(key) != db.end()){
            auto res = db.find(key);
            cout<<res->second<<endl;
            cout<<"Record fetched..."<<endl;
        }
        else{
            cout<< "Key not found in the database"<<endl;
        }
        
    }
    void del(string key){ 
        if(db.find(key) != db.end()){
            db.erase(key);
            cout<<"Record is deleted"<<endl;   
        }
        else{
            cout<<"No record found with key "<<key<<" to delete from the database"<<endl;
        }   
    }
    void friend snapshot(string u_filename, kvstore &k);
    void friend replaylog(string u_filename, kvstore &k);
    void friend replaysnap(string u_filename, kvstore &k);
    void friend compact_log(string u_filename, kvstore &k);
};

int main()
{
    kvstore app;
    string command, key, value, line;
    bool persist = checklog(filename);
    bool snap = checklog(backup);
    //ofstream file(filename, ios::app);
    cout<<"Welcome to the command line interface of key value store..."<<endl;
    cout<<endl;
    cout<<"Refer the commands below to use the application..."<<endl;
    cout<<endl;
    cout<<"Store new record --> set [key] [value]; Get a record using key --> get [key]; Delete a record --> delete [key]; quit to exit from the application"<< endl;
    cout<<endl;
    if(persist){
        //replay the set and delete commands in the log to regain the state  
        cout<<"processing..."<<endl;
        cout<<endl;
        if(snap)
        {
        replaysnap(backup, app);
        }
        replaylog(filename,app);
        cout<<"retained previous state"<<endl;
        while(true)
        {
            cout<<"\n";
            getline(cin, line);
            stringstream ss(line);
            ss>>command>>key>>value;
            if( command == "set"){
                //cin>>key>>value;
                app.set(key,value);
                string content = command+" "+key+" "+value;
                append(filename, content);
            }
            else if(command == "get"){
                //cin>>key;
                app.get(key);
                string content = command+" "+key;
                append(filename, content);
            }
            else if(command =="delete"){
                //cin>>key;
                app.del(key);
                string content = command+" "+key;
                append(filename, content);
            }
            else if(command == "quit"){
                cout<< "Leaving command line interface..."<<endl;
                string content = command;
                append(filename, content);
                compact_log(filename, app);
                return 0;
            }
            else if(command == "snapshot"){
                cout<<"Taking a snapshot"<<endl;
                snapshot(backup, app);
                cout<<"snapshot taken... "<<"clearing logs"<<endl; 
            }
            else{
                cout<<"Invalid command"<<endl;
            }
        }
    }
    else
    {
        cout<<"some problem in generating the log file";
    }
    return 0;
}

bool checklog(const string& filename){
    ofstream file(filename, ios::app);
    return file.is_open();
}
void append(const string& filename, string& content){
    ofstream file(filename, ios::app);
    file<<content<<"\n";
}
void snapshot(string u_filename, kvstore &k){
    ofstream file(u_filename, ios::out | ios::trunc); // clear the previous snapshot 
    if(file.is_open()){
        for(auto i:k.db){
            file<<i.first<<" "<<i.second<<endl;
        }
        file.close();
    }  // update with the current state
    else{
        cout<<"error with snapshot file"<<endl;
    }
    // clear the log file
    ofstream file_1(filename, ios::out | ios::trunc);
    if(file_1.is_open()){
        file_1.close();
    }  
}
void replaysnap(string u_filename, kvstore &k){
    string line, line_key, line_val;
    ifstream file(u_filename);
    while(getline(file, line))
    {
        stringstream ss(line);
        ss>>line_key>>line_val;
        k.db.insert({line_key, line_val});
    }
}
void replaylog(string u_filename, kvstore &k){
    string line, line_com,line_key, line_val;
    ifstream file(u_filename);
    while(getline(file, line))
    {
        stringstream ss(line);
        ss>>line_com>>line_key>>line_val;
        k.db.insert({line_key, line_val});
    }
}
void compact_log(string u_filename, kvstore &k){
    vector<string> temp;
    string line, command, key, value;
    ifstream file(u_filename);
    while(getline(file, line)){
        stringstream ss(line);
        ss>>command>>key>>value;
        if(command == "set"){
            temp.push_back(key+" "+ value);
        }
    }
    file.close();
    ofstream file_1(u_filename, ios::out | ios::trunc);
    if(file_1.is_open()){
        for(string i:temp){
            file_1<<"set"<<" "<<i<<endl;
        }
        file_1.close();
    }
    else{
        cout<<"error with log file"<<endl;
    }
}