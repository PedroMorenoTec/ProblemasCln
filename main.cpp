#include <iostream>
#include <sqlite3.h> 
#include <string> 
#include <sstream>
#include <vector>
#include <fstream>

/*
Referencias:
https://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm
https://stackoverflow.com/questions/37956090/how-to-construct-a-stdstring-with-embedded-values-i-e-string-interpolation
https://stackoverflow.com/questions/31146713/sqlite3-exec-callback-function-clarification/31168999
https://stackoverflow.com/questions/24227723/c-cannot-initialize-variable-with-an-rvalue-of-type-void
https://stackoverflow.com/questions/15836253/c-populate-vector-from-sqlite3-callback-function
https://stackoverflow.com/questions/17335816/clear-screen-using-c
https://stackoverflow.com/questions/903221/press-enter-to-continue
https://www.w3schools.com/cpp/cpp_exceptions.asp
https://www.cplusplus.com/reference/stdexcept/invalid_argument/
https://stackoverflow.com/questions/2340281/check-if-a-string-contains-a-string-in-c
https://www.cplusplus.com/reference/cstdio/remove/
https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-14-17-c
*/
using namespace std;

// Crear variables globales, las cuales son la base de datos y el Id del usuario
sqlite3 *db;
int userId = -1;

// Función que es llamada al ser obtenidos los datos (si no hay resultados, esta no se ejecuta), tenemos un puntero que es donde irá la tabla, un count que es la longitud de los resultados, un arreglo de strings data que son los resultados y un arreglo de strings columns que contiene el nombre de los encabezados de los resultados
int callback(void *ptr, int count, char **data, char **columns)
{
  // Creamos una matriz de strings
  vector<vector<string>>* table = static_cast<vector<vector<string>>*>(ptr);
  vector<string> row;

  // Iteramos en los resultados y los vamos añadiendo a la tabla
  for (int i = 0; i < count; i++)
      row.push_back(data[i]);
  table->push_back(row);
  return 0;
}

// Función para correr sql dado un string
vector<vector<string>> runSql(string s) {
  // Convertimos el string que nos pasaron a un apuntador
  const char* sql = s.c_str();
  // Creamos un apuntador para el mensaje de error
  char *errmsg = 0;
  // Creamos una matriz de strings para los resultados
  vector<vector<string>> ans;
  // Ejecutamos el sql y recogemos el status en la variable rc
  int rc = sqlite3_exec(db, sql, callback, &ans, &errmsg);

  // Si algo ocurrió mal, entonces emitimos un error
  if( rc != SQLITE_OK ){
    throw invalid_argument(errmsg);
  } else {
  // Si no, le decimos al usuario que el SQL fue exitoso
    cout<<"SQL successful"<<endl;
  }
  // Devolvemos el resultado
  return ans;
}

// ***********************************************
// Empieza el codigo
// ***********************************************

// Función para salir adecuadamente
void exit() {
  // Cerrar la conexión con la base de datos
  sqlite3_close(db);
  // Dar mensaje de despedida
  cout<<"Adios amigo!"<<endl;
}

// Función para restablecer la base de datos en caso de que exista un error
void reset() {
  // Cerrar la conexión existente
  sqlite3_close(db);

  // Leer archivo database.db
  ifstream f("database.db");
  // Si existe, entonces eliminarlo, si no, lanzar un error
  if(f.good() && remove("database.db")!= 0)
      throw invalid_argument("Error deleting file");
  // Cerrar la conexión con el archivo
  f.close();

  // Crear un nuevo archivo y conectarse a el
  sqlite3_open("database.db", &db);

  // Corrrer sql para crear tabla de usuarios
  runSql("CREATE TABLE USERS (ID INTEGER PRIMARY KEY AUTOINCREMENT, FULLNAME TEXT NOT NULL, MAIL TEXT UNIQUE NOT NULL, PASSWORD TEXT NOT NULL);");
  
  // Corrrer sql para crear tabla de posts
  runSql("CREATE TABLE POSTS (ID INTEGER PRIMARY KEY AUTOINCREMENT, DATE TIMESTAMP DEFAULT CURRENT_TIMESTAMP, TITLE TEXT NOT NULL, DESCRIPTION TEXT NOT NULL, IMGSRC TEXT NOT NULL, VOTES INTEGER DEFAULT 0, USERID INTEGER NOT NULL);");

  // Corrrer sql para insertar un primer usuario
  runSql("INSERT INTO USERS VALUES(NULL,'pedro','pedro@pedro.com','1234');");

  // Corrrer sql para insertar un primer post 
  runSql(" INSERT INTO POSTS(ID,TITLE,DESCRIPTION,IMGSRC,USERID) VALUES(NULL,'My first post','Hello World','img.com',1);");
  
  // Salir del programa
  exit();
}

// Función para limpiar la pantalla
void clear() {
  cout << "\033[2J\033[1;1H";
}

// Función para esperar a que el usuario presione enter como confirmación
void enterToContinue() {
  cout<<"Presione enter para continuar...";
  char temp = 'x';
  // Mientras no presione enter esperamos
  while (temp != '\n')
    cin.get(temp);
}

// Función que despliega un menú con un prompt y devuelve la línea introducida por el usuario en un string
string menuItem(string prompt) {
  cout<<prompt<<": ";
  string x;
  getline(cin,x);
  return x;
}

// Declaraciones del menú principal y el menú de posts
void mainMenu();
void postsMenu();

// Función para ver posts
void viewPosts() {
  // Limpiar la pantalla
  clear();
  // Crear sql con inner join para traer los posts con el nombre del usuario y ordenados por votos descendientes
  stringstream sql;
  sql<<"SELECT POSTS.ID,POSTS.DATE,USERS.FULLNAME,POSTS.TITLE,POSTS.DESCRIPTION,POSTS.IMGSRC,POSTS.VOTES FROM POSTS INNER JOIN USERS ON POSTS.USERID = USERS.ID ORDER BY VOTES DESC;";
  
  // Almacenar el resultado del sql en una matriz de strings
  vector<vector<string>> posts = runSql(sql.str());
  cout<<endl;
  // Decorador
  cout<<"******************************************";
  cout<<endl;
  // Iteramos en cada post y lo mostramos al usuario
  for(vector<string> post : posts) {
    cout<<"Post ID: "<<post[0]<<endl;
    cout<<"Fecha: "<<post[1]<<endl;
    cout<<"Usuario: "<<post[2]<<endl;
    cout<<"Titulo: "<<post[3]<<endl;
    cout<<"Descripcion: "<<post[4]<<endl;
    cout<<"Imagen: "<<post[5]<<endl;
    cout<<"Votos: "<<post[6]<<endl;
    cout<<endl<<endl;
  }
  
  // Esperamos a que el usuario presione enter para continuar
  enterToContinue();
  // Dirigir al menú de posts
  postsMenu();
}

void createPost() {
  // Limpiar la pantalla
  clear();

  // Recoger datos como el título, la descripción y la imgSrc del post
  cout<<"Crear Post: "<<endl;
  string title = menuItem("Introduzca titulo");
  string description = menuItem("Introduzca descripcion");
  string imgSrc = menuItem("Introduzca link de imagen");

  // Crear sql para insertar el post en la tabla
  stringstream sql;
  sql<<"INSERT INTO POSTS(ID,TITLE,DESCRIPTION,IMGSRC,USERID) VALUES(NULL,'";
  sql<<title<<"','";
  sql<<description<<"','";
  sql<<imgSrc<<"','";
  sql<<userId<<"');";
  
  runSql(sql.str());
  // Redirigir al menú de posts
  postsMenu();
};

void deletePost() {
  // Limpiar la pantalla
  clear();

  // Recoger el Id del post a borrar
  cout<<"Eliminar Post: "<<endl;
  string postId = menuItem("Introduzca el id del post que desea eliminar o 'b' para regresar");

  // Si la tecla no es 'b' entonces borrar el post
  if(postId!="b") {
    // Crear sql para verificar que el id del post existe
    stringstream sql;
    sql<<"SELECT * FROM POSTS WHERE ID=";
    sql<<postId<<";";
    
    // Usar try-catch por si ocurre un error
    try{
      // Intentar correr el sql
      vector<vector<string>> result = runSql(sql.str());

      // Si devuelve un vector vacío entonces emitir un error
      if(result.size() == 0) 
        throw invalid_argument("no such column");
      
      // Si la ejecución continúa, entonces borrar el post de la base de datos
      stringstream sql;
      sql<<"DELETE FROM POSTS WHERE ID=";
      sql<<postId<<";";
      runSql(sql.str());

      cout<<"Borrado adecuadamente"<<endl;
    } catch(invalid_argument error){
      // Obtener el mensaje de error
      string msg = error.what();

      // Si el mensaje de error contiene la palabra "no such column" o "syntax error" entonces decirle al usuario que el id es inválido, si no, se trata de otro error y debemos emitirlo
      if (msg.find("no such column") != string::npos || msg.find("syntax error") != string::npos) 
        cout<<"Id invalido"<<endl;
      else
        throw invalid_argument(msg);
    }
  }

  // Esperar a que el usuario presione enter
  enterToContinue();
  // Redirigir al menú de posts
  postsMenu();
}

void votePost() {
  // Limpiar la pantalla
  clear();

  // Recoger el Id del post que se va a votar
  cout<<"Votar Post: "<<endl;
  string postId = menuItem("Introduzca el id del post que desea votar o 'b' para regresar");
  
  // Si la tecla no es 'b' entonces votar el post
  if(postId!="b") {
    // Crear sql para verificar que el post existe
    stringstream sql;
    sql<<"SELECT * FROM POSTS WHERE ID=";
    sql<<postId<<";";
    
    // Usar try-catch por si ocurre un error
    try{
      // Almacenar el resultado en una matriz
      vector<vector<string>> result = runSql(sql.str());

      // Si la matriz está vacía entonces emitir un error
      if(result.size() == 0) 
        throw invalid_argument("no such column");
      
      // Si la ejecución continúa entonces añadir 1 voto al post
      stringstream sql;
      sql<<"UPDATE POSTS SET VOTES = VOTES + 1 WHERE ID=";
      sql<<postId<<";";
      runSql(sql.str());

      cout<<"Ha votado adecuadamente"<<endl;
    } catch(invalid_argument error){
      // Obtener el mensaje del error
      string msg = error.what();

      // Si el mensaje contiene la palabra "no such column" o "syntax error" entonces decirle al usuario que el Id es inválido, si no, emitir un error 
      if (msg.find("no such column") != string::npos || msg.find("syntax error") != string::npos)
        cout<<"Id invalido"<<endl;
      else
        throw invalid_argument(msg);
    }
  }

  // Esperar a que el usuario presione enter para continuar
  enterToContinue();

  // Redirigir al menú de posts
  postsMenu();
}

void deleteAccount() {
  // Limpiar la pantalla
  clear();
  cout<<"Eliminar Mi Cuenta: "<<endl;

  // Sql para eliminar el usuario de la tabla
  stringstream sql;
  sql<<"DELETE FROM USERS WHERE ID=";
  sql<<userId<<";";

  runSql(sql.str());
  
  // Sql para eliminar los posts del usuario de la tabla
  sql.str("");
  sql<<"DELETE FROM POSTS WHERE USERID=";
  sql<<userId<<";";

  runSql(sql.str());

  // Resetear el userId global
  userId=-1;
  // Mensaje de despedida
  cout<<"Lamentamos que se vaya :("<<endl;  
  // Esperar a que el usuario presione enter
  enterToContinue();
  // Redirigir al menú principal
  mainMenu();
}

void postsMenu() {
  // Limpiar la pantalla
  clear();

  // Mostrar menú de posts
  cout<<"==Bienvenido=="<<endl;
  cout<<"[1] Ver Posts"<<endl; 
  cout<<"[2] Crear Post"<<endl;
  cout<<"[3] Eliminar Post"<<endl;
  cout<<"[4] Votar Post"<<endl;
  cout<<"[5] Eliminar Mi Cuenta"<<endl;
  cout<<"[6] Salir"<<endl;
  string o = menuItem("Seleccion");
  // Redirigir a las pantallas correspondientes, si lo introducido no coincide, entonces volver a mostrar este menú
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
  // Limpiar la pantalla
  clear();

  // Recoger el nombre completo, el correo y la contraseña 2 veces
  cout<<"Register: "<<endl;
  string fullName = menuItem("Introduzca nombre completo");
  string mail = menuItem("Introduzca su correo electronico");
  string password = menuItem("Introduzca su contrasena");
  string passwordConf = menuItem("Introduzca su contrasena de nuevo");

  // Si las contraseñas son distintas entonces decirle al usuario que no se puede
  if(password != passwordConf) {
    cout<<endl<<"Contrasenas no coinciden"<<endl;
    // Esperar a que presione enter para continuar
    enterToContinue();
    // Redirigir al menú principal
    mainMenu();
  } else {
    // Si las contraseñas coinciden, entonces crear un sql para insertar al usuario en la tabla
    stringstream sql;
    sql<<"INSERT INTO USERS VALUES(NULL,'";
    sql<<fullName<<"','";
    sql<<mail<<"','";
    sql<<password<<"');";

    try {
      // Intentar correr el sql
      runSql(sql.str());
      // Si todo va bien, decirle al usuario que ha sido registrado
      cout<<endl<<"Registrado correctamente!"<<endl;
    } catch(invalid_argument error) {
      // Obtener el mensaje del error
      string msg = error.what();
      
      // Si el mensaje de error NO contiene la palabra "UNIQUE" entonces emitir un error
      if (msg.find("UNIQUE") == string::npos) 
        throw invalid_argument(error);

      // Si la ejecución continúa entonces decirle al usuario que el correo ya está registrado
      cout<<endl<<"El correo ya esta registrado"<<endl;
    }

    // Esperar a que el usuario presione enter
    enterToContinue();
    // Redirigir al menú principal
    mainMenu();
  }
}

void loginMenu() {
  // Limpiar la pantalla
  clear();

  // Recoger el correo y contraseña del usuario
  cout<<"Login: "<<endl;
  string mail = menuItem("Introduzca su correo electronico");
  string password = menuItem("Introduzca su contrasena");

  // Crear sql para obtener el usuario de la base de datos
  stringstream sql;
  sql<<"SELECT ID FROM USERS WHERE ";
  sql<<"MAIL='"<<mail<<"' AND ";
  sql<<"PASSWORD='"<<password<<"';";

  // Almacenar el resultado en una matriz
  vector<vector<string>> result = runSql(sql.str());

  // Si la matriz está vacía, entonces decirle al usuario que algo introdujo mal
  if(result.size() == 0) {
    cout<<endl<<"Usuario o contrasena incorrecta"<<endl;
    // Esperar a que presione enter
    enterToContinue();
    // Redirigir al menú principal
    mainMenu();
  } else {
    // Si la matriz no está vacía, entonces tomar el UserId como caracter y guardarlo en esa variable global
    userId = result[0][0].at(0) - '0';
    // Redirigir al menú de posts
    postsMenu();
  }
}

void mainMenu() {
  // Limpiar la pantalla
  clear();
  // Mostrar menú principal
  cout<<"Menu Principal"<<endl;
  cout<<"[1] Login"<<endl; 
  cout<<"[2] Registrarse"<<endl;
  cout<<"[3] Reestablecer Base De Datos"<<endl;
  cout<<"[4] Salir"<<endl;
  string o = menuItem("Seleccion");
  // Redirigir a las pantallas correspondientes, si introdujo algo mal, volverle a mostrar el menú
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
  // Abrir la base de datos
  sqlite3_open("database.db", &db);

  // Mostrar menú principal
  mainMenu();
  return 0;
}