#ifndef SERVER_H
#define SERVER_H

#include "User.hpp"
#include <vector>
#include <unistd.h>
#include <netinet/in.h>
#include <poll.h>

class User;

class Server {
	public:
		Server(int fd, int port) {
			saddr.sin_family = AF_INET;
			saddr.sin_port = htons(port);
			saddr.sin_addr.s_addr = INADDR_ANY;
			fds.push_back(pollfd());
			fds.back().fd = fd;
			fds.back().events = POLLIN;
		}
		~Server() {
			for (int i = 0; i != fds.size(); i++)
				close(fds[i].fd);
		}

		std::vector<User *> getUsers() { return users; }
		std::vector<pollfd> getFds() { return fds; }
		struct sockaddr_in* getAddr() { return &saddr; }
		
		struct sockaddr_in saddr;
		std::vector<User *> users;
		std::vector<pollfd> fds;
};

#endif
