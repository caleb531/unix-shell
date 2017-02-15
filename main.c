#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// The maximum length allowed for any line in the shell
static const int MAX_LINE_LENGTH = 80;

// Parse the given command into the given pointers for command name and command
// arguments
void parseCmd(char* cmd, char** cmdArgs) {
	// Split the command using a single space as a delimeter
	char* cmdArg = (char*)malloc(MAX_LINE_LENGTH * sizeof(char));
	cmdArg = strtok(cmd, " ");
	int i = 0;
	// Assume the arguments comprise every term after the first term
	while (cmdArg != NULL) {
		cmdArgs[i] = (char*)malloc((strlen(cmdArg) + 1) * sizeof(char));
		strcpy(cmdArgs[i], cmdArg);
		cmdArg = strtok(NULL, " ");
		i += 1;
	}
	// Add terminating null pointer to signify end of array
	cmdArgs[i] = NULL;
}

// Execute the given command (or display error if not a real command)
void execCmd(char* cmd) {
	char* cmdArgs[(MAX_LINE_LENGTH / 2) + 1];
	parseCmd(cmd, cmdArgs);
	int status = execvp(cmdArgs[0], cmdArgs);
	// execvp() returns -1 if command does not exist
	if (status == -1) {
		printf("invalid command; try again\n");
		exit(1);
	}
	// TODO: add code for deallocating elements of cmdArgs
}

// Wait for the user to enter a one-line command, returning what was entered if
// possible (or erroring out if not possible)
char* getEnteredCmd() {
	char *cmd = (char*)malloc(MAX_LINE_LENGTH * sizeof(char));
	size_t bufferSize;
	size_t cmdLength = getline(&cmd, &bufferSize, stdin);
	if (cmdLength != -1) {
		// Remove trailing newline from entered command (for conveniece)
		cmd[cmdLength - 1] = '\0';
		cmdLength--;
		return cmd;
	} else {
		// Exit if an error occurred while reading the line
		printf("could not read line; exiting...");
		free(cmd);
		exit(1);
	}
}

int main() {

	// Run forever until shell exit
	while (1) {
		printf("osh> ");
		fflush(stdout);
		// Retrieve and execute whatever command was entered (if possible)
		char* cmd = getEnteredCmd();
		// Exit the shell if `exit` command was given
		if (strcmp(cmd, "exit") == 0) {
			return 0;
		}
		// If user entered nothing, continue to next line
		if (strcmp(cmd, "") == 0) {
			continue;
		}
		int pid = fork();
		if (pid == 0) {
			// Execute command within process, then exit child
			execCmd(cmd);
			return 0;
		} else {
			// Make parent process wait for child process to finish
			wait(NULL);
		}
		free(cmd);
	}

	return 0;

}
