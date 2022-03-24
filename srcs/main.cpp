#include <iostream>
#include "Server.hpp"
#include <poll.h>

bool running = true;

using namespace std;

void error(string msg) {
	cout << msg << endl;
	exit(1);
}

bool hasUsers(Server *server) { /* Recorre el vector<Users *> y a la que pilla uno que sigue perteneciendo al servidor devuelve true para que siga corriendo */
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
	int fd = socket(AF_INET, SOCK_STREAM, 0); /* Se crea el socket. AF_INET = IPv4, SOCK_STREAM = TCP/IP, el 0 viene con el protocolo de transmision (si fuese UDP sería otro. Cual? Ni idea) */
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	Server server(fd, atoi(v[1])); /* Se crea el servidor que recibe el fd del socket y el puerto en el que corre para rellenar la estructura */
	if (bind(fd, (struct sockaddr *)&server.saddr, sizeof(server.saddr)) == -1)
		error("bind error");
	if (listen(fd, INT_MAX) == -1)
		error("listen error");
	while (running) {
		if (poll(&server.fds[0], server.fds.size(), -1) == -1) /* poll examina un set de fds y espera que haya habido algun evento en concreto o, en este caso, que ninguno de los fds de sockets que tiene almacenados haya tenido algun tipo de error */
			error("poll error");
		if (server.fds[0].revents == POLLIN) { /* En este caso como configuramos el server con el constructor para que una vez creado espere que le lleguen datos al socket, entrará en la condición */
			int sock = accept(fd, 0, 0); /* Accept espera a que le llegue un intento de conexion desde otro socket al servidor y crea un nuevo fd que apunta a la conexion */
			if (!sock)
				error("accept error");
			char name[10];
			recv(sock, &name, sizeof(name), 0); /* Recibe el nickname que le hemos mandado desde el cliente para crear un nuevo usuario */
			User *newUser = new User(sock, name); /* El usuario se crea con el fd de la nueva conexion y con el nickname recibido */
			newUser->add(); /* belongs=true */
			server.users.push_back(newUser); /* Se añade el nuevo usuario al vector del servidor que los almacena */
			cout << "new connection: " << newUser->getNickName() << endl;
			memset(name, '\0', 9); /* Nombre a \0 para cuando reciba el siguiente */
			server.fds.push_back(pollfd()); /* Se le añade un nuevo fd al servidor que apunta a la nueva conexion creada y se pone ese fd a la espera de datos */
			server.fds.back().fd = sock;
			server.fds.back().events = POLLIN;
		} else {
			for (vector<pollfd>::iterator it = server.fds.begin(); it != server.fds.end(); it++) {
				if ((*it).revents == POLLIN) { /* Si le estan llegando datos de uno de los fds que tiene almacenados lo procesa */
					int index;
					for (int i = 0; i != server.getUsers().size(); i++) { /* Se comprueba de quien es el mensaje que esta llegando para mandarselo al resto de clientes y que el mensaje venga asignado a la persona */
						if (server.getUsers()[i]->getFd() == (*it).fd)
							index = i;
					}
					char input[256];
					recv((*it).fd, &input, sizeof(input), 0);
					string output;
					if (!strcmp(input, "exit")) { /* Si el mensaje recibido es exit, pone belongs=false del usuario que lo ha enviado y el output que recibiran el resto de usuarios será el de "tal usuario ha abandonado el chat"*/
						server.getUsers()[index]->leave();
						output = server.getUsers()[index]->getNickName() + " has left the chat\n";
					} else { /* Si no, el output que reciban el resto de clientes será el mensaje enviado */
						output = server.getUsers()[index]->getNickName() + ": " + input + "\n";
					}
					for (int i = 0; i != server.getUsers().size(); i++) {
						if (i != index) /* El mensaje no se envía al usuario que lo ha mandado */
							send(server.getUsers()[i]->getFd(), output.c_str(), output.length(), 0);
					}
					memset(input, '\0', 255); /* Pone el input a 0 para recibir el siguiente */
				}
			}
		}
		if (!hasUsers(&server)) { /* A la que el server deje de tener usuarios deja de correr */
			running = false;
		}
	}
}
