#include <iostream>
#include "Server.hpp"
#include <poll.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define NAME_MAX_SZ 10

bool running = true;

using namespace std;

void error(string msg) {
	cout << msg << endl;
	exit(1);
}

/* Recorre el vector<Users *> y a la que pilla uno que sigue perteneciendo al
 * servidor devuelve true para que siga corriendo
 */
bool hasUsers(Server *server) {
	for (size_t i = 0; i != server->getUsers().size(); i++) {
		if (server->getUsers()[i]->stillThere())
			return true;
	}
	return false;
}

int main(int n, char **v) {
	/* ./server [port] */
	if (n != 2)
		error("bad arguments");
	int opt = 1;
	/* Se crea el socket. AF_INET = IPv4, SOCK_STREAM = TCP/IP, el 0 viene con
	 * el protocolo de transmision (si fuese UDP sería otro. Cual? Ni idea) */
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	/* Se crea el servidor que recibe el fd del socket y el puerto en el que
	 * corre para rellenar la estructura */
	Server server(fd, atoi(v[1]));
	/* Bind le asigna esa direccion con los datos que hemos definido al socket creado */
	if (bind(fd, (struct sockaddr *)&server.saddr, sizeof(server.saddr)) == -1)
		error("bind error");
	/* Espera conexiones al socket creado y con un limite de cola (puesto a INT_MAX porque me apetece) */
	if (listen(fd, INT_MAX) == -1) 
		error("listen error");
	while (running) {
		/* poll examina un set de fds y espera que haya habido algun evento en concreto o, en este caso,
		 * que ninguno de los fds de sockets que tiene almacenados haya tenido algun tipo de error */
		if (poll(&server.fds[0], server.fds.size(), -1) == -1)
			error("poll error");
		/* En este caso como configuramos el server con el constructor para que una vez creado espere que
		 * le lleguen datos al socket, entrará en la condición cuando note que le intenta llegar info */
		
		/* Este primer condicional es para recibir nuevos usuarios. server.fds[0] es el
		 * socket que escucha nuevas conexiones, las ya establecidas tienen valores de server.fds[]
		 * mayores que 0. */
		if (server.fds[0].revents == POLLIN) {
			/* Accept espera a que le llegue un intento de conexion desde otro socket al servidor y
			 * crea un nuevo fd que apunta a la conexion. Esta funcion hangea el programa (true bucle). */
			int sock = accept(fd, 0, 0);
			if (!sock)
				error("accept error");
			char name[NAME_MAX_SZ];
			/* Recibe el nickname que le hemos mandado desde el cliente para crear un nuevo usuario */
			recv(sock, &name, sizeof(name), 0);
			/* El usuario se crea con el fd de la nueva conexion y con el nickname recibido */
			User *newUser = new User(sock, name);
			/* belongs=true */
			newUser->add();
			/* Se añade el nuevo usuario al vector del servidor que los almacena */
			server.users.push_back(newUser);
			cout << "new connection: " << newUser->getNickName() << endl;
			/* Nombre a \0 para cuando reciba el siguiente */
			memset(name, '\0', 9);
			/* Se le añade un nuevo fd al servidor que apunta a la nueva conexion
			 * creada y se pone ese fd a la espera de datos */
			server.fds.push_back(pollfd());
			server.fds.back().fd = sock;
			server.fds.back().events = POLLIN;
		
		/* si no hay conexiones nuevas, checkea si hay datos a leer por aprte de los
		 * usuarios almacenados. */
		} else {
			for (vector<pollfd>::iterator it = server.fds.begin(); it != server.fds.end(); it++) {
				/* Si le estan llegando datos de uno de los fds que tiene almacenados lo procesa */
				if ((*it).revents == POLLIN) {
					int index;
					/* Se comprueba de quien es el mensaje que esta llegando para
					 * mandarselo al resto de clientes y que el mensaje venga asignado a la persona */
					for (int i = 0; i != server.getUsers().size(); i++) {
						if (server.getUsers()[i]->getFd() == (*it).fd)
							index = i;
					}
					char input[256];
					recv((*it).fd, &input, sizeof(input), 0);
					string output;
					/* Si el mensaje recibido es exit, pone belongs=false del usuario que lo ha enviado y
					 * el output que recibiran el resto de usuarios será el de "tal usuario ha abandonado el chat"*/
					if (!strcmp(input, "exit")) {
						server.getUsers()[index]->leave();
						output = server.getUsers()[index]->getNickName() + " has left the chat\n";
					/* Si no, el output que reciban el resto de clientes será el mensaje enviado */
					} else {
						output = server.getUsers()[index]->getNickName() + ": " + input + "\n";
					}
					for (int i = 0; i != server.getUsers().size(); i++) {
						/* El mensaje no se envía al usuario que lo ha mandado */
						if (i != index)
							send(server.getUsers()[i]->getFd(), output.c_str(), output.length(), 0);
					}
					/* Pone el input a 0 para recibir el siguiente */
					memset(input, '\0', 255);
				}	
			}
		}
		if (!hasUsers(&server)) { /* A la que el server deje de tener usuarios deja de correr */
			running = false;
		}
	}
}
