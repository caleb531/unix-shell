#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The maximum length allowed for any line in the shell
static const int MAX_LINE_LENGTH = 80;

// Execute the given command (or display error if not a real command)
void execCmd(char* cmd) {
	if (strcmp(cmd, "") == 0) {
		// Do nothing if the entered command is empty
		return;
	} else if (strcmp(cmd, "exit") == 0) {
		// Exit the shell if `exit` command was given
		exit(0);
	}
	// Merely print the command if non-empty
	printf("%s\n", cmd);
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
		exit(0);
	}
}

int main() {

	// The list where parsed CLI arguments will be stored
	// TODO: move this somewhere else and do something useful with it
	// char *cmdArgs[(MAX_LINE_LENGTH / 2) + 1];

	// Run forever until shell exit
	while (1) {
		printf("osh> ");
		fflush(stdout);
		// Retrieve and execute whatever command was entered (if possible)
		char* cmd = getEnteredCmd();
		execCmd(cmd);
	}

	return 0;

}
