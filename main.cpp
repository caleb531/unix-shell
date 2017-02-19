#include <iostream>
#include <cstring>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <deque>

using namespace std;

// Parse the given command into the vector for command name and arguments
// Command is saved in the first vector index, and all arguments are after that
void parseCmd(string cmd, vector<char*> &cmdArgs) {
	// A stream to store the command for reading while parsing
	istringstream ss(cmd);
	// A temporary variable for storing the string (non-char*) instance of
	// command argument
	string argStr;

	// stringstream splits the commands using whitespace as delimiter
	while (ss >> argStr) {
		char* cmdArg = new char[argStr.length() + 1];
		strcpy(cmdArg, argStr.c_str());
		cmdArgs.push_back(cmdArg);
	}

	// Push back null to signify end of arguments
	cmdArgs.push_back(NULL);

}

// Execute the given command (or display error if not a real command)
void execCmd(string cmd) {
	vector<char*> cmdArgs;
	parseCmd(cmd, cmdArgs);

	// Convert vector to char** to appease execvp()
	char **cmdArgsArray = &cmdArgs[0];
	int status = execvp(cmdArgsArray[0], cmdArgsArray);

	// execvp() returns -1 if command does not exist
	if (status == -1) {
		cout << cmdArgsArray[0] << ": command not found" << endl;
		exit(1);
	}
	// TODO: add code for deallocating elements of cmdArgs
}

// Wait for the user to enter a one-line command, returning what was entered if
// possible (or erroring out if not possible)
string getEnteredCmd() {
	string cmd;
	getline(cin, cmd);
	if (cin.bad()) {
		cout << "Could not read line; exiting..." << endl;
		exit(1);
	} else {
		return cmd;
	}
}

// Displays the last 10 values of the deque
void displayHistory(deque<string> &history) {
	if (!history.size() > 0) {
		cout << "No commands in history" << endl;
		return;
	}

	for (int i = 0; i < 10; i++) {
		if (i >= history.size()) {
			break;
		}

		cout << history.size() - i << " " <<  history[history.size() - i - 1] << endl;
	}
}

// Changes the passed in cmd string to the proper command in history
// Returns true if the string was changed, false otherwise
bool changeCommand(string &cmd, deque<string> &history) {
	//If trying to access a position outside of the current bounds of the history, return false
	if (!history.size() > 0) {
		cout << "No commands in history" << endl;
		return false;
	}

	// Sets cmd to the last value in history
	if (cmd == "!!") { 
		cmd = history[history.size() - 1];
		return true;
	}

	// Sets cmd to the Nth value in history
	if (cmd[0] == '!') {

		// Represents the Nth command in history to be executed 
		long pos;

		pos = strtol(cmd.substr(1, 2).c_str(), NULL, 10);

		// If strtol couldn't parse the string, return false
		if (pos == 0) {
			cout << "Could not read history position" << endl;
			return false;
		}

		// If the parsed number points the a number greater than the current index, return false
		if (pos > history.size()) {
			cout << "No such command in history" << endl;
			return false;
		}

		// Change cmd to the proper string in history
		cmd = history[pos - 1];
		return true;
	}

	// If the cmd wasn't the proper syntax, then don't change it
	return false;
}

int main() {
	//Deque containing last 10 commands in history
	deque<string> history;

	// Run forever until shell exit
	while (1) {
		cout << "osh> ";
		// Retrieve and execute whatever command was entered (if possible)
		string cmd = getEnteredCmd();
		// Exit the shell if `exit` command was given
		if (cmd == "exit") {
			return 0;
		}
		// If user entered nothing, continue to next line
		if (cmd == "") {
			continue;
		}
		// Display the current history, then continue to next line
		if (cmd == "history") {
			displayHistory(history);
			continue;
		}

		if (cmd == "!!" || cmd[0] == '!') {
			// Attempt to change cmd to the proper cmd in history
			// If unable, continue to next line
			if (!changeCommand(cmd, history)) {
				continue;
			}
		}

		int pid = fork();
		if (pid == 0) {
			// Execute command within process, then exits child
			execCmd(cmd);
		} else {
			// Make parent process wait for child process to finish
			int status;
			wait(&status);
			// Check if child exited properly
			if (status == 0) {
				history.push_back(cmd);
			}
		}
	}
	return 0;
}
