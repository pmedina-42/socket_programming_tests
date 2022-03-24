#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>

bool running = true;

using namespace std;

void error(string msg) {
	cout << msg << endl;
	exit(1);
}

int main(int n, char **v) {
	/* ./client [port] [username] */
	if (n != 3)
		error("bad arguments");
	int fd = socket(AF_INET, SOCK_STREAM, 0); /* Se crea el socket. AF_INET = IPv4, SOCK_STREAM = TCP/IP, el 0 viene con el protocolo de transmision */
	struct sockaddr_in addr; /* Esta estructura contiene toda la info del socket. Mas info preguntar a pmedina- */
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(v[1]));
	addr.sin_addr.s_addr = INADDR_ANY;
	connect(fd, (struct sockaddr *)&addr, sizeof(addr)); /* Establece una conexion con otro socket que este corriendo en el mismo puerto */
	send(fd, v[2], strlen(v[2]), 0); /* Una vez establecida la conexion le mando al servidor el nickname del cliente que se ha conectado */
	pid_t ppid = getpid(); /* Cojo el pid del padre para matarlo luego desde el hijo a la que reciba "exit" */
	pid_t pid = fork();
	if (pid == -1) {
		error("fork error");
	}
	if (pid == 0) {
		while (running) { /* Mientras el hijo este corriendo va a esperar a recibir un input para enviarselo al servidor*/
			string str;
			getline(cin, str);
			if (!str.compare("exit")) {
				running = false;
			}
			send(fd, str.c_str(), str.length(), 0); /* Le envio al servidor el input que ha metido el cliente. Al ser funcion de c hay que pasar la string a char* */
		}
		kill(ppid, SIGKILL); /* Cuando el cliente sale del chat el hijo mata al padre, aunque imprime cosa fea por pantalla y con hilos no, pero lo quería hacer con procesos */
	} else {
		while (running) { /* Mientras el padre esta corriendo esta pendiente de recibir un mensaje que haya enviado otro cliente al servidor */
			char serveroutput[256];
			recv(fd, &serveroutput, sizeof(serveroutput), 0); /* Si lo recibe, rellena el char[] */
			cout << serveroutput; /* Lo saca por pantalla */
			memset(serveroutput, '\0', 255); /* Y se vacía para cuando llegue el siguiente mensaje */
		}
	}
	close(fd); /* Se cierra el fd del socket */
	return 0;
}
