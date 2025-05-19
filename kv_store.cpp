#include<iostream>
#include<unordered_map>
#include<fstream>
#include<filesystem>
#include<sstream>
using namespace std;

bool checklog(const string& filename);
void append(const string& filename, string& content);
// core operations are "set", "get", "delete". Create an unordered map to store the key value pairs - phase 1
// adding persistance to the current implementation:
// check if a text file named log in present in the working directory, if not create and open it append mode.
// on every restart, read the commands in the log file line by line to replay them. This will reslt in regaining the previous state.
// some problem with replay function 
string filename = "log";
class kvstore
{
    private:
    unordered_map<string, string> db;
    public:
    void set(string key, string value){
        if(db.find(key) != db.end()){
            cout<< "already a record exists with the given key, delete the existing record to add a different value to this key"<<endl;
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

};

int main()
{
    kvstore app;
    string command, key, value, line, line_com, line_key, line_val;
    bool persist = checklog(filename);
    ifstream file(filename);
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
        while(getline(file, line)){
            stringstream ss(line);
            ss>>line_com>>line_key>>line_val;
            if(line_com == "set"){
                app.set(line_key, line_val);
            }
            else if(line_com == "delete"){
                app.del(line_key);
            }
            else{
            }
        }
        cout<<"retained previous state"<<endl;
        while(true)
        {
            cout<<"\n";
            cin>>command;
            if( command == "set"){
                cin>>key>>value;
                app.set(key,value);
                string content = command+" "+key+" "+value;
                append(filename, content);
            }
            else if(command == "get"){
                cin>>key;
                app.get(key);
                string content = command+" "+key;
                append(filename, content);
            }
            else if(command =="delete"){
                cin>>key;
                app.del(key);
                string content = command+" "+key;
                append(filename, content);
            }
            else if(command == "quit"){
                cout<< "Leaving command line interface..."<<endl;
                string content = command;
                append(filename, content);
                return 0;
                
            }
            else{
                cout<<"Invalid command";
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
void replay(string filename){

}

