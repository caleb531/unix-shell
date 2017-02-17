#include <iostream>
#include <cstring>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>

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

int main() {
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
				// TODO: Add command to history here
			}
		}
	}
	return 0;
}
