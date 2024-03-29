/**
 * CS 433 Programming Assignment 2: UNIX Shell
 * Caleb Evans, Jonathan Tapia
 * February 25, 2017
 * DESCRIPTION
 * This program includes the complete implementation for a simple Unix shell
 * HOW TO RUN
 * Run `make` to compile the program, then run `./unix-shell.out` to execute
 * the compiled program
 * KNOWN ASSUMPTIONS
 * The shell is capable of executing standard POSIX commands (e.g. `ls -a`),
 * executable files (e.g. `./unix-shell.out`), and any other command on the
 * user's PATH (e.g. `mysql`). The shell is also capable of accessing the last
 * 10 executed commands via `history`, `!!`, and `!N` (where N is the numeric ID
 * of the command to run)
 */
#include <iostream>
#include <cstring>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <deque>

using namespace std;

// The maximum number of commands to store in the history
static const int HISTORY_MAX_CMD_COUNT = 10;

// Check if the command should spawn a background process (via a trailing &)
bool cmdIsBgProcess(string cmd) {
	return (cmd[cmd.length() - 1] == '&');
}

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

	// If command is to run as background process via &
	if (cmdIsBgProcess(cmd)) {
		// Do not treat & as an argument to the actual command
		cmdArgs.pop_back();
	}

	// Push back null to signify end of arguments
	cmdArgs.push_back(NULL);

}

// Deallocate memory used for storing each command argument in the given array
void freeCmdArgs(vector<char*> &cmdArgs) {
	for (int i = 0; i < cmdArgs.size(); i++) {
		delete cmdArgs[i];
	}
}

// Execute the given command (or display error if not a real command)
void execCmd(string cmd) {
	vector<char*> cmdArgs;
	parseCmd(cmd, cmdArgs);

	// Convert vector to char** to appease execvp()
	char **cmdArgsArray = &cmdArgs[0];
	int status = execvp(cmdArgsArray[0], cmdArgsArray);
	freeCmdArgs(cmdArgs);

	// execvp() returns -1 if command does not exist
	if (status == -1) {
		cout << cmdArgsArray[0] << ": command not found" << endl;
		exit(1);
	}
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

// Display a list of the most-recently used commands
void displayHistory(deque<string> &history) {
	if (!history.size() > 0) {
		cout << "No commands in history" << endl;
		return;
	}

	// Display no more than the maximum number of commands
	for (int i = 0; i < HISTORY_MAX_CMD_COUNT; i++) {
		// There may be fewer commands in the history; do not display more than
		// what the history contains
		if (i >= history.size()) {
			break;
		}

		cout << (history.size() - i) << " " <<  history[history.size() - i - 1] << endl;
	}
}

// Changes the passed in cmd string to the proper command in history
// Returns true if the string was changed, false otherwise
bool changeCommand(string &cmd, deque<string> &history) {
	// If trying to access a position outside of the current bounds of the
	// history, return false
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


// Wait for the current child process (with the given pid) to finish
void waitForChildProcess(int childPid) {
	// Running wait() within a loop ensures that background processes executed
	// in the past do not affect the synchronous execution of future foreground
	// processes
	int terminatedPid;
	do {
		terminatedPid = wait(NULL);
	} while (terminatedPid > 0 && terminatedPid != childPid);
}

int main() {
	// Deque containing last 10 commands in history; a deque guarantees O(1)
	// insertions/deletions from either end of the sequence, making it an
	// optimal choice for storing command history
	deque<string> history;

	// Run forever until shell exit
	while (true) {
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
			// If child process should be run as a foreground process (i.e. not
			// via &), make parent process wait for child to finish
			if (!cmdIsBgProcess(cmd)) {
				waitForChildProcess(pid);
			}
			history.push_back(cmd);
		}
	}
	return 0;
}
