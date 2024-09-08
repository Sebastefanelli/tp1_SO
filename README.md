# tp1_SO

### Punto 1 
Se implementa una shell básica que permite ejecutar comandos tanto en foreground como
en background. Para evitar procesos zombies, se utiliza la señal SIGCHLD que permite al
proceso padre recolectar los procesos hijos que terminan en background. Además, se
maneja la señal SIGINT (Ctrl+C), permitiendo que cuando se presione este atajo, se espere
a que todos los procesos terminen correctamente antes de salir de la shell.
- Foreground: El padre espera a que el proceso hijo termine antes de continuar.
- Background: El proceso hijo corre en segundo plano, y el padre no espera su
  finalización inmediata.
- Uso de señales para manejar adecuadamente los procesos y evitar zombies.

### Punto 2
Se crea un programa donde se especifica cuántos procesos hijos se deben crear. Estos
procesos son "hermanos", y cada uno entra en un ciclo de espera hasta que recibe la señal
SIGUSR1 o SIGINT, lo que les indica que deben terminar. Cada proceso hijo retorna un
valor distinto al proceso padre, que luego recoge la suma de los retornos de todos los hijos.

- Los procesos hijos están en pausa hasta recibir una señal.
- El proceso padre espera a que cada hijo termine y recoge su valor de retorno.
- Se evita la creación de procesos huérfanos o zombies al asegurarse que el padre
  recoja todos los retornos de los hijos.

Ademas se creo un archivo llamado pids.txt donde se escriben los pids de loa procesos creados para que luego sean recuperados en envia.c con el cual se puden mandar las señales alos procesos creado de manera mas facil ya que se disponen los pids.

### Punto 3
Aquí se resuelve la sincronización de tres hilos (A, B y C) utilizando mutex. El patrón de ejecución es ABAC y se asegura que cada hilo acceda a la sección crítica en el orden
correcto. El programa se detiene después de N iteraciones, las cuales se especifican por
línea de comandos.
- Mutex bloquea y desbloquea secciones críticas para asegurar que los hilos accedan
  en el orden ABAC.
- Se configura un candado (mutex) para cada hilo, y se usa la función
  pthread_mutex_lock() y pthread_mutex_unlock() para controlar el acceso.
- El proceso se detiene después de realizar la cantidad de iteraciones especificadas.

### Punto 4 
Este punto es similar al anterior, pero en lugar de sincronizar hilos, se sincronizan procesos independientes utilizando semáforos. Para esto se crean los siguientes archivos:

- crearsem: crea el conjunto de semaforos.
- initsem: se inicializa cada semaforo con el valor correspondiente.
- versem: te dice los valores de cada semaforo.
- pa, pb, pc: es donde se encuentra el codigo de cada proceso independiente.
- PyV: se encuentran las operaciones para los semaforos que se utilizan en cada proceso.
- inicializar, compilar y ejecutar: son ejecutables que se encargan de hacer lo que dice su nombre con los archivos que se crearon.

Como yo lo pense seria ejecutar compilar primero, luego ininicializar y finalmente ejecutar con parametro el numero de iteraciones (agregue un ejecutar debian, la idea es quelas terminales de pb y pc se habran autamicamente pero no conozco que gestor ni que terminal esta utilizando), en caso que el ejecutar no funcione se debera abrir una terminal por proceso y a cada proceso se le debe pasar el argumento con el numero de iteraciones y funcionara como es esperado.

### Punto 5 
En este punto, se implementa la sincronización entre un proceso padre y sus dos hijos. La
secuencia de ejecución es PadreA-HijoB-PadreA-HijoC. Aquí se resuelve la sincronización
sin utilizar señales, esto se logra utilizando semaforos donde se realizara tantas interaciones (n) como se indique en la linea de comandos, tiene mucha relacion con la logica usada en el punto 3.

### Punto 6
Este ejercicio implementa un programa en el que el proceso padre crea un proceso hijo que
ejecuta un comando. La salida de ese comando es capturada a través de un pipe entre el
proceso padre e hijo. Esto permite que el padre lea la salida y la muestre por pantalla.
- Se usa fork() para crear el proceso hijo.
- Se usa un pipe para comunicar la salida del hijo al padre.

### Punto 7
Aquí se implementa un servidor que recibe conexiones de clientes y ejecuta comandos
Linux que le envía el cliente. Se utiliza sockets TCP/IP para la comunicación, y cada cliente se maneja en un hilo separado para permitir múltiples conexiones concurrentes.
- Sockets TCP/IP permiten la comunicación entre cliente y servidor.
- Pthreads se usan para crear un hilo por cada cliente, lo que permite manejar múltiples
conexiones simultáneas.
- El servidor ejecuta el comando recibido y devuelve la salida al cliente.

### Punto 8
En este caso, se crea un servidor CGI que recibe comandos a través de la query string de
una URL. El servidor ejecuta el comando y devuelve la salida al cliente. Se usa un pipe
para capturar la salida del comando.

- El servidor CGI toma los comandos de la URL, los parsea y luego los ejecuta.
- Se usa un pipe para capturar la salida del comando.
- El servidor devuelve la salida del comando como respuesta al cliente.

Estos puntos cubren el uso de procesos, señales, sincronización con mutex y semáforos,
manejo de sockets, y creación de un servidor CGI, siempre con el objetivo de evitar
procesos huérfanos o zombies y asegurar una correcta sincronización entre procesos o
hilos.
