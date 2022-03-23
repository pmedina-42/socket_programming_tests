#include <iostream>
#include "Server.hpp"
#include <poll.h>

bool running = true;

using namespace std;

void error(string msg) {
	cout << msg << endl;
	exit(1);
}

int main(int n, char **v) {
	/* ./server [port] */
	if (n != 2)
		error("bad arguments");
	int opt = 1;
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	Server server(fd, atoi(v[1]));
	if (bind(fd, (struct sockaddr *)&server.saddr, sizeof(server.saddr)) == -1)
		error("bind error");
	if (listen(fd, INT_MAX) == -1)
		error("listen error");
	while (running) {
		if (poll(&server.fds[0], server.fds.size(), -1) == -1)
			error("poll error");
		if (server.fds[0].revents == POLLIN) {
			int sock = accept(fd, 0, 0);
			if (!sock)
				error("accept error");
			char name[10];
			recv(sock, &name, sizeof(name), 0);
			User *newUser = new User(sock, name);
			server.users.push_back(newUser);
			cout << "new connection: " << newUser->getNickName() << endl;
			memset(name, '\0', 9);
			server.fds.push_back(pollfd());
			server.fds.back().fd = sock;
			server.fds.back().events = POLLIN;
		} else {
			for (vector<pollfd>::iterator it = server.fds.begin(); it != server.fds.end(); it++) {
				if ((*it).revents == POLLIN) {
					int index;
					for (int i = 0; i != server.getUsers().size(); i++) {
						if (server.getUsers()[i]->getFd() == (*it).fd)
							index = i;
					}
					char input[256];
					recv((*it).fd, &input, sizeof(input), 0);
					if (!strcmp(input, "exit")) {
						server.getUsers()[index]->leave();
					}
					/*for (int i = 0; i != server.getUsers().size(); i++) {
						cout << server.getUsers()[i]->getFd() << endl;
						send(server.getUsers()[i]->getFd(), input, strlen(input), 0);
					}*/
					cout << server.getUsers()[index]->getNickName() << ": " << input << endl;
					memset(input, '\0', 255);
				}
			}
		}
	}
}
