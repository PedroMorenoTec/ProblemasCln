# ProblemasCln

Sistema de altas, bajas y cambios para la materia de Pensamiento Computacional Orientado a Objetos.

En este sistema los usuarios se autentican con su correo y contraseña. Tienen la posibilidad de crear posts diciendo cuales son los problemas de la ciudad de Culiacán. Los demás usuarios pueden realizar upvotes para decidir cuáles problemas de la ciudad son de mayor importancia, y esos aparecerán primero en la lista.

## Instalación
Es necesario tener instalado MYSYS2 en Windows: https://www.msys2.org/
Para correr (Windows) solo clona el repositorio y ejecuta:
```bash
./main.exe
```

Para compilar añade la extensión sqlite al compilador dependiendo del sistema operativo de tu computadora.
```bash
g++ -o main main.cpp -l sqlite3 && ./main
```
