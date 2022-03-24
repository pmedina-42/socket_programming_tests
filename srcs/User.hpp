#ifndef USER_H
#define USER_H

#include <unistd.h>

using namespace std;

class User {
	private:
		string nickName;
		bool belongs;
		int fd;

	public:
		User(int fd, string nick) : nickName(nick), belongs(false), fd(fd) {}
		~User() {}

		int getFd() { return fd; }
		string getNickName() { return nickName; }
		void setNewNick(string nick) { nickName = nick; }
		void add() { belongs = true; }
		bool stillThere() { return belongs; }
		void leave() {
			belongs = false;
			close(fd);
		}
};

#endif
