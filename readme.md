readme

# Introducción al Laboratorio primero de Sistemas Operativo

### Grupo:

Virtualized, nombrado así en honor a la técnica empleada por los sistemas operativos.

### Integrantes:

Conformado por

- Leonardo Torres, (leo.torres@mi.unc.edu.ar)
- Matías Scantamburlo, (matias.scantamburlo@mi.unc.edu.ar)
- Maciel Salguero, (maciel.salguero@mi.unc.edu.ar)

# Modularización

### Idea general:

En este laboratorio se realizará un shell al estilo bash de Linux, el cual debe tener las propiedades características de un shell, que son:

- Ejecución de comandos en modo foreground y background
    
- Re-dirección de entrada y salida, y un pipe entre comandos.
    

### Módulos:

El proyecto está dividido en cuatro partes. Estas son:

1.  Command:
    Contendrá la implementación del abstract data type scommand y pipeline.
    
2.  Execute:
    Contendrá las instrucciones de la propia ejecución de los comandos.
    
3.  Parser:
    Consiste en el parser que interpreta el texto ingresado por teclado.
    
4.  Builtin en donde se encontrarán los comandos? internos del shell.
    

### TAD´s (abstract data type):

- TAD scommand: Consistirá en la siguiente estructura:

!\[scommand\](./img/Captura de pantalla de 2021-09-12 21-19-45.png)

La cual es una terna compuesta por una lista de strings donde irán los argumentos, un string para el redireccionamiento in, y un string para el redireccionamiento out.

Por ejemplo, el comando:

```
wc -w < words.txt > numerOfWords.txt 
```

Se almacenaría en una variable del tipo scommand de la siguiente manera;

En la lista de string irá primero el comando principal wc junto con los argumentos, en este caso -w, por lo que la lista tendría la forma:

```
[“wc”, “-w”] 
```

En el string redir\_in irá el string “words.txt” y en el string redir\_out “numberOfWords.txt”.

Quedando así como resultado:

```
([“wc”, “-w”] ,“words.txt”,“numberOfWords.txt”) 
```

- TAD Pipeline, Consistirá de la siguiente estructura:

!\[pipeline\](./img/Captura de pantalla de 2021-09-12 22-07-01.png)

La cual es una tupla conformada por scmds, que es una lista de scommand, y wait donde está el booleano que indicará si se debe esperar o no.
Por ejemplo, en el comando;

```
ls -lt | grep notes -i | wc -l > notesOnTheList.txt 
```

Cada comando simple se almacenará en una variable del tipo scommand, de esta manera pasarán a formar una lista de ternas. En este caso se vería de la siguiente manera;

```
[([“ls”,”-lt”],””,””),([“grep”,”notes”,”-i”],””,””),[([“wc”,”-l”],””,”notesOnTheList.txt”)]] 
```

Como dijimos anteriormente al ser una tupla,el elemento definido recientemente pasará a ser el primer elemento, mientras que el wait será el segundo. Del modo siguiente;4

```
([([“ls”,”-lt”],””,””),([“grep”,”notes”,”-i”],””,””),[([“wc”,”-l”],””,”notesOnTheList.txt”)]],1) 
```

# Desarrollo

En este laboratorio se trabajó principalmente en 3 archivos:

1.  command.c
2.  execute.c
3.  builtin.c
    A continuación describiremos que se hizo en cada archivo, los problemas que surgieron y cómo se solucionaron.

### command.c:

- Idea general:
    
    Nuestro primer objetivo en esta instancia fue programar los comandos TAD scommand, ya que fueron bases para la implementación de los TAD pipeline y el desarrollo del proyecto en su generalidad.
    Para esto, y antes que nada, debimos familiarizarnos con la estructura de los comandos simples, para posteriormente poder estudiar y comprender, de mejor manera, las librerías externas. Una vez realizado esto, procedimos a usar la librerías de glib-2.0 para las primeras instrucciones, las dichas no nos ocasionaron mayores complicaciones. Seguimos con la implementación de las instrucciones pipelines, estas requirieron un nuevo estudio de la librerías pero similar que los anteriores TAD´s no tuvimos muchas trabas.
    
- Problemas que surgieron:
    

1.  Al principio nos costó poder hacer una buen uso de la librería externa.
    
2.  En la implementación de **scommand\_to\_string()** nos equivocamos al usar **g\_slist\_nth()** para llamar al enésimo argumento del scommand
    
3.  Tuvimos problemas al liberar memoria ya que optamos por la utilización de **g\_slist\_free_full()**
    

- Soluciones que les dimos:

1.  Hacer una buena lectura de cada una de las funciones, como también hacer un seguimiento completo de su estructura.
    
2.  Usar la función **g\_slist\_nth_data()** y evitar la practica de casteos al nodo
    
3.  Como solución decidimos ir liberando memoria de elemento a elemento con un ciclo de while
    

### builtin.c:

- Idea general:
    
    En esta segunda instancia nuestro objetivo fue implementar los comandos internos de nuestro mybash:
    
    1.  **cd()**: Primero definimos el cuerpo de la función **builtin\_is\_cd()** el cual comparará el primer elemento del pipe con el string "cd". Esta función sera utilizada en el **builtin_exec()** Donde, si se cumple el antecedente (**builtin\_is\_cd()**), tomaremos el path, a continuación de "cd", y usando la función **chdir()** nos moveremos al directorio indicado
        
    2.  **exit()**: Definimos la función **builtin\_is\_exit()** la cual tomará el primer comando simple del pipe, y lo comparará con el string "exit"
        
- Problemas que surgieron:
    

No hubo contratiempos en esta parte del Laboratorio

### execute.c:

- Idea general:

El objetivo central de esta parte era programar la función que iba a ejecutar comandos de la terminal.
Para resolver esto lo primero que se hizo fue diferenciar entre comandos internos y externos, para los internos usamos las funciones programadas en el módulo Builtin.c.

```
A continuación se explicará cómo se implementó la primera versión de esta función, o sea la que estaba preparada solo para 2 comandos simples, en caso que se quiera leer sobre la versión de múltiples pipes avanzar a la parte de puntos estrellas. 
```

Básicamente lo que se hizo fue usar la función **fork()** para crear un proceso para cada comando y mediante la función **pipe()** de C, se redirecciono la salida del programa de la izquierda con la entrada del programa derecho, para hacer la redirección usamos la función **dup2()**. Finalmente ejecutamos los respectivos comandos con la función **execvp()**.

Además de esto se tuvo a la hora implementación se en cuenta:

1.  Cerrar las puntas de escritura/lectura cuando no son usadas
    
2.  Chequear los valores de retorno de las funciones nombradas en el párrafo anterior
    
3.  Esperar la finalización de los respectivos procesos usando la función **waitpid()**
    

Respecto a las redirecciones lo que se hizo fue chequear para cada comando que efectivamente se hiciera una redirección y en caso de ser así además de hacer lo descrito anteriormente, se abría el archivo y utilizando una vez más la función dup se cambiaba la redirección en función si ere de entrada o salida.

- Problemas que surgieron

1.  Uno de los problemas que tuvimos al momento de testear la 1era implementación fue el hecho que el bash funcionaba correctamente para pipes de 2 elementos pero no funcionaba para comandos simples.

- Soluciones que les dimos:

1.  El problema venía de que sin importar si era 1 o 2 comandos lo que recibía el bash, de igual manera se estaba redireccionando la salida, así que lo que se hizo fue implementar una variable booleana la cual responde a la pregunta ¿Solo hay que ejecutar un comando? En caso de que la respuesta sea verdadera, entonces la salida del comando se mandaba a la terminal y terminaba la ejecución, de no ser así se proseguía como estaba antes.

# Puntos estrella

- Multiples Pipes:

La idea central fue crear para una cantidad **n** de comandos, **n** procesos y **n-1** pipes. Para esto creamos una función que se encarga de generar los procesos, tomando como parametros de etrada:

1.  El file descriptor del scommand anterior.
2.  Una variale de tipo booleano que responde a la pregunta ¿es el ultimo comando a ejecutarce?.
3.  El scommand en cuestión
4.  Las puntas de lectura/escritura que se utilizaran para leer y escribir en el "buffer" creado por **pipe()**

Ahora verificaremos si nos encontramos al principio, en medio o al ultimo del pipe, ya que en cada caso se debera realizar un tratamiento diferente

1.  Si estamos al principio; debemos tomar como entrada al Std.Input y devolver el resultado del comando a la punta de escritura, para esto se redireccionará la salida estándar al cuarto file descriptor creado anteriormente.
2.  Si estamos en medio; debemos tomar como entrada lo que nos dejo el anterior comando, para esto leeremos la punta de lectura, redireccionando la entrada estándar al tercer file descriptor y similar al anterior caso se devolverá el resultado del comando a la punta de escritura.
3.  Si estamos al ultimo del pipe, tomaremos por entrada el punto de lectura y devolveremos el resultado al Std.Output

Por ultimo esta función devuelve el PID de los procesos hijos para poder cerrarlos

- Ruta y colores del usuario:

A modo de estetica y comodidad, se agregaron al prompt:

1.  Colores
2.  Nombre del usuario
3.  Una ruta absoluta al directorio acutal

Para logear al usuario se llamo a la función **getlogin()**, mientras que par mostrar el directorio actual se uso **getcwd()** (ambas funciónes importadas de la libreria &lt;unistd.h&gt;). Por ultimo para los colores se implemento un codigo que asigna al user el color verde y a la dirección el color azul.

# Conclusiones

- Se logro entender mucho mejor el funcionamiento de un bash, de como funciona la conección y ejecución de sus procesos.
- Se gano experiencia en la practica de no re-inventar la rueda y esto incluye el hecho de no romper la abstracción de una librreria, sino más bien, hacer una buena lectura sobre su aplicación
- Nos iniciamos en el mundo de git, después de varíos choques, y aprendimos a utilizar los comandos más basico y elementales para el trabajo en conjunto
- Entregamos el proyecto de Lab con tres failures
<img src="../_resources/6f4aaff360604f60bef292b2f29fbfe8.png" alt="31377bc6981c3b632e43ddd849c18aa7.png" width="775" height="54">
Sin embargo, tenemos una idea de lo que podría haber sido:
Estas tres fallas estan relacionadas por como nosotros estamos implementando el multiple-pipeline, ya que necesitamos ir conectando la entrada y salida de cada programa, respectivamente, uno detrás del otro, entonces esto nos imposibilita poder concretar con todos los test´s
No lo decimos del todo seguros pero es muy probablemente que si usáramos solo dos comandos en un pipeline no tendríamos estos failures