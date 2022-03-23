#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

bool running = true;

using namespace std;

void error(string msg) {
	cout << msg << endl;
	exit(1);
}

int main(int n, char **v) {
	// ./client [port] [username]
	if (n != 3)
		error("bad arguments");
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(v[1]));
	addr.sin_addr.s_addr = INADDR_ANY;
	connect(fd, (struct sockaddr *)&addr, sizeof(addr));
	send(fd, v[2], strlen(v[2]), 0);
	while (running) {
		string str;
		getline(cin, str);
		if (!str.compare("exit")) {
			running = false;
		}
		send(fd, str.c_str(), str.length(), 0);
	}
	return 0;
}
