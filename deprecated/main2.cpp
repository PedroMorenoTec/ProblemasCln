#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <ctime>

using namespace std;

vector<string> split(string s, char target) {
    vector<string> ans;
    s+=target;
    string word = "";
    for(char c: s) {
      if(c==',') {
        ans.push_back(word);
        word="";
      } else {
        word+=c;
      }
    }
    return ans;
}

string join(vector<string> v, char target) {
    string line="";
    for(string s : v) {
      line+=s+target;
    }
    line.pop_back();
    return line;
}

class User {
  public:
    int ID=rand()% 50000000000;
    string FULLNAME;
    string MAIL;
    string PASSWORD; 

    User(string FULLNAME, string MAIL,string PASSWORD) {
      this->FULLNAME=FULLNAME;
      this->MAIL=MAIL;
      this->PASSWORD=PASSWORD; 
    }

    string toString() {
      vector<string> v{to_string(this->ID), this->FULLNAME, this->MAIL, this->PASSWORD};
      return join(v,',');
    }
};

class Post {
  public:
    int ID=rand()% 50000000000;
    int DATE=time(0);
    string TITLE;
    string DESCRIPTION;
    string IMGSRC; 
    int VOTES=0;
    int USERID;

    Post(string TITLE, string DESCRIPTION, string IMGSRC, int USERID) {
      this->TITLE=TITLE;
      this->DESCRIPTION=DESCRIPTION;
      this->IMGSRC=IMGSRC; 
      this->USERID=USERID;
    }

    string toString() {
      vector<string> v{to_string(this->ID), to_string(this->DATE),this->TITLE, this->DESCRIPTION, this->IMGSRC,  to_string(this->VOTES), to_string(this->USERID)};
      return join(v,',');
    }
};

class Db {
  public:
    string path;
    vector<vector<string>> contents;

    Db(string path) {
      this->path=path;
      this->readDb();
    }

    void readDb() {
      this->contents = {};
      ifstream inData(path);
      string line;

      while (getline (inData, line)) {
        this->contents.push_back(split(line, ',')); // Row
      }

      inData.close();
    }

    void printDb() {
      const int WIDTH = 15;
      for (int i = 0; i < this->contents.size(); i++) {
        for (int j = 0; j < this->contents[i].size(); j++){
            string element = this->contents[i][j];

            if(element.length()>WIDTH) {
              element = element.substr(0,WIDTH-3-1) + "...";
            }

            cout<<left<<setw(WIDTH)<<element;
        }
        cout << endl;
      }
    }

    vector<string> getRowByField(string field, string match){
      vector<string> headers = this->contents[0];

      int targetIndex=-1;
      
      for(int i=0; i<headers.size(); i++) {
        if(headers[i]==field) {
          targetIndex=i;
          break;
        }
      }

      for(int j=0; j<this->contents.size(); j++) {
        if(this->contents[j][targetIndex]==match) {
          return this->contents[j];
        }
      }

      return {};
    }

    void insert(string line) {
      ofstream outData(this->path, ios_base::app);

      outData<<line<<endl;
      outData.close();
      this->readDb();
    }

};

int main() {
  Db users("users.txt");
  User ben("uwu", "uwu@1.com", "aadfbw");

  users.insert(ben.toString());
  users.printDb();
} 