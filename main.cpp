#include <iostream>
#include <sqlite3.h> 
#include <string> 
#include <sstream>
#include <vector>
#include <fstream>

// Referencias:
//https://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm
//https://stackoverflow.com/questions/37956090/how-to-construct-a-stdstring-with-embedded-values-i-e-string-interpolation
// https://stackoverflow.com/questions/31146713/sqlite3-exec-callback-function-clarification/31168999
//https://stackoverflow.com/questions/24227723/c-cannot-initialize-variable-with-an-rvalue-of-type-void
//https://stackoverflow.com/questions/15836253/c-populate-vector-from-sqlite3-callback-function
//https://stackoverflow.com/questions/17335816/clear-screen-using-c
//https://stackoverflow.com/questions/903221/press-enter-to-continue
//https://www.w3schools.com/cpp/cpp_exceptions.asp
//https://www.cplusplus.com/reference/stdexcept/invalid_argument/
//https://stackoverflow.com/questions/2340281/check-if-a-string-contains-a-string-in-c
//https://www.cplusplus.com/reference/cstdio/remove/
//https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-14-17-c

using namespace std;
sqlite3 *db;
int userId = -1;

//Callback isn't called when there are no results
int callback(void *ptr, int count, char **data, char **columns)
{
   vector<vector<string>>* table = static_cast<vector<vector<string>>*>(ptr);
    vector<string> row;
    for (int i = 0; i < count; i++)
        row.push_back(data[i]);
    table->push_back(row);
  return 0;
}

vector<vector<string>> runSql(string s) {
   const char* sql = s.c_str();
   char *errmsg = 0;
   vector<vector<string>> ans;
   int rc = sqlite3_exec(db, sql, callback, &ans, &errmsg);

   if( rc != SQLITE_OK ){
      throw invalid_argument(errmsg);
   } else {
      cout<<"SQL successful"<<endl;
   }
  return ans;
}

// ***********************************************
// Empieza el codigo
// ***********************************************

void exit() {
  sqlite3_close(db);
  cout<<"Adios amigo!"<<endl;
}

void reset() {
  sqlite3_close(db);

  ifstream f("database.db");
  if(f.good() && remove("database.db")!= 0)
      throw invalid_argument("Error deleting file");
  f.close();

  sqlite3_open("database.db", &db);

  runSql("CREATE TABLE USERS (ID INTEGER PRIMARY KEY AUTOINCREMENT, FULLNAME TEXT NOT NULL, MAIL TEXT UNIQUE NOT NULL, PASSWORD TEXT NOT NULL);");
  runSql("CREATE TABLE POSTS (ID INTEGER PRIMARY KEY AUTOINCREMENT, DATE TIMESTAMP DEFAULT CURRENT_TIMESTAMP, TITLE TEXT NOT NULL, DESCRIPTION TEXT NOT NULL, IMGSRC TEXT NOT NULL, VOTES INTEGER DEFAULT 0, USERID INTEGER NOT NULL);");

  runSql("INSERT INTO USERS VALUES(NULL,'pedro','pedro@pedro.com','1234');");
  runSql(" INSERT INTO POSTS(ID,TITLE,DESCRIPTION,IMGSRC,USERID) VALUES(NULL,'My first post','Hello World','img.com',1);");
  
  exit();
}

void clear() {
  cout << "\033[2J\033[1;1H";
}

void enterToContinue() {
  cout<<"Presione enter para continuar...";
  char temp = 'x';
  while (temp != '\n')
    cin.get(temp);
}

string menuItem(string prompt) {
  cout<<prompt<<": ";
  string x;
  getline(cin,x);
  return x;
}

void mainMenu();
void postsMenu();

void viewPosts() {
  clear();
  stringstream sql;
  sql<<"SELECT POSTS.ID,POSTS.DATE,USERS.FULLNAME,POSTS.TITLE,POSTS.DESCRIPTION,POSTS.IMGSRC,POSTS.VOTES FROM POSTS INNER JOIN USERS ON POSTS.USERID = USERS.ID ORDER BY VOTES DESC;";
    
  vector<vector<string>> posts = runSql(sql.str());
  cout<<endl;
  cout<<"******************************************";
  cout<<endl;
  for(vector<string> post : posts) {
    cout<<"Post ID: "<<post[0]<<endl;
    cout<<"Fecha: "<<post[1]<<endl;
    cout<<"Usuario: "<<post[2]<<endl;
    cout<<"Título: "<<post[3]<<endl;
    cout<<"Descripción: "<<post[4]<<endl;
    cout<<"Imagen: "<<post[5]<<endl;
    cout<<"Votos: "<<post[6]<<endl;
    cout<<endl<<endl;
  }
  
  enterToContinue();
  postsMenu();
}

void createPost() {
    clear();
    cout<<"Crear Post: "<<endl;
    string title = menuItem("Introduzca título");
    string description = menuItem("Introduzca descripción");
    string imgSrc = menuItem("Introduzca link de imagen");

    stringstream sql;
    sql<<"INSERT INTO POSTS(ID,TITLE,DESCRIPTION,IMGSRC,USERID) VALUES(NULL,'";
    sql<<title<<"','";
    sql<<description<<"','";
    sql<<imgSrc<<"','";
    sql<<userId<<"');";
    
    runSql(sql.str());
    postsMenu();
};

void deletePost() {
  clear();
  cout<<"Eliminar Post: "<<endl;
  string postId = menuItem("Introduzca el id del post que desea eliminar o 'b' para regresar");
  if(postId!="b") {
    stringstream sql;
    sql<<"SELECT * FROM POSTS WHERE ID=";
    sql<<postId<<";";
    
    try{
    vector<vector<string>> result = runSql(sql.str());
    if(result.size() == 0) 
      throw invalid_argument("no such column");
    
    stringstream sql;
    sql<<"DELETE FROM POSTS WHERE ID=";
    sql<<postId<<";";
    runSql(sql.str());

    cout<<"Borrado adecuadamente"<<endl;
    } catch(invalid_argument error){
      string msg = error.what();
      if (msg.find("no such column") != string::npos) 
        cout<<"Id inválido"<<endl;
    }
  }
  enterToContinue();
  postsMenu();
}

void votePost() {
  clear();
  cout<<"Votar Post: "<<endl;
  string postId = menuItem("Introduzca el id del post que desea votar o 'b' para regresar");
  if(postId!="b") {
    stringstream sql;
    sql<<"SELECT * FROM POSTS WHERE ID=";
    sql<<postId<<";";
    
    try{
    vector<vector<string>> result = runSql(sql.str());
    if(result.size() == 0) 
      throw invalid_argument("no such column");
    
    stringstream sql;
    sql<<"UPDATE POSTS SET VOTES = VOTES + 1 WHERE ID=";
    sql<<postId<<";";
    runSql(sql.str());

    cout<<"Ha votado adecuadamente"<<endl;
    } catch(invalid_argument error){
      string msg = error.what();
      if (msg.find("no such column") != string::npos) 
        cout<<"Id inválido"<<endl;
    }
  }
  enterToContinue();
  postsMenu();
}

void deleteAccount() {
  clear();
  cout<<"Eliminar Mi Cuenta: "<<endl;

  stringstream sql;
  sql<<"DELETE FROM USERS WHERE ID=";
  sql<<userId<<";";

  runSql(sql.str());
  
  sql.str("");
  sql<<"DELETE FROM POSTS WHERE USERID=";
  sql<<userId<<";";

  runSql(sql.str());

  userId=-1;
  cout<<"Lamentamos que se vaya :("<<endl;  
  enterToContinue();
  mainMenu();
}

void postsMenu() {
  clear();
  cout<<"==Bienvenido=="<<endl;
  cout<<"[1] Ver Posts"<<endl; 
  cout<<"[2] Crear Post"<<endl;
  cout<<"[3] Eliminar Post"<<endl;
  cout<<"[4] Votar Post"<<endl;
  cout<<"[5] Eliminar Mi Cuenta"<<endl;
  cout<<"[6] Salir"<<endl;
  string o = menuItem("Selección");
  if(o=="1") {
    viewPosts();
  } else if(o=="2") {
    createPost();
  } else if(o=="3") {
    deletePost();
  } else if(o=="4") {
    votePost();
  } else if(o=="5") {
    deleteAccount();
  } else if(o=="6") {
    exit();
  } else {
    postsMenu();
  }
}

void registerMenu() {
  clear();
  cout<<"Register: "<<endl;
  string fullName = menuItem("Introduzca nombre completo");
  string mail = menuItem("Introduzca su correo electrónico");
  string password = menuItem("Introduzca su contraseña");
  string passwordConf = menuItem("Introduzca su contraseña de nuevo");

  if(password != passwordConf) {
    cout<<endl<<"Contraseñas no coinciden"<<endl;
    enterToContinue();
    mainMenu();
  } else {
    stringstream sql;
    sql<<"INSERT INTO USERS VALUES(NULL,'";
    sql<<fullName<<"','";
    sql<<mail<<"','";
    sql<<password<<"');";

    try {
      runSql(sql.str());
      cout<<endl<<"Registrado correctamente!"<<endl;
    } catch(invalid_argument error) {
      string msg = error.what();
      
      if (msg.find("UNIQUE") == string::npos) 
        throw invalid_argument(error);

      cout<<endl<<"El correo ya está registrado"<<endl;
    }
    enterToContinue();
    mainMenu();
  }
}

void loginMenu() {
  clear();
  cout<<"Login: "<<endl;
  string mail = menuItem("Introduzca su correo electrónico");
  string password = menuItem("Introduzca su contraseña");

  stringstream sql;
  sql<<"SELECT ID FROM USERS WHERE ";
  sql<<"MAIL='"<<mail<<"' AND ";
  sql<<"PASSWORD='"<<password<<"';";

  vector<vector<string>> result = runSql(sql.str());

  if(result.size() == 0) {
    cout<<endl<<"Usuario o contraseña incorrecta"<<endl;
    enterToContinue();
    mainMenu();
  } else {
    userId = result[0][0].at(0) - '0';
    postsMenu();
  }
}

void mainMenu() {
  clear();
    cout<<"Menú Principal"<<endl;
    cout<<"[1] Login"<<endl; 
    cout<<"[2] Registrarse"<<endl;
    cout<<"[3] Reestablecer Base De Datos"<<endl;
    cout<<"[4] Salir"<<endl;
    string o = menuItem("Selección");
    if(o=="1") {
      loginMenu();
    } else if(o=="2") {
      registerMenu();
    } else if (o=="3") {
      reset();
    } else if (o=="4") {
      exit();
    } else {
      mainMenu();
    }
}

int main() {
  sqlite3_open("database.db", &db);

  mainMenu();
  return 0;
}