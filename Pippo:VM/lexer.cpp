#include "lexer.h"

strings Lexer::lex(std::string s) {
	strings strlst;
	char lexeme[256]; // buffer
	int i = 0;
	int j = 0;
	State state = START;
	int done = 0;
	int len = s.length();
	int balance = 0;
	bool labelFound = false;
	bool pushMode = false;
	bool jumpMode = false;
	bool pushNullMode = false;
	bool creating_function = false;
	bool calling_function = false;

	while(i < len) {
		switch (state) {
			case START:
				if (my_isspace(s[i])) {
					state = SKIP;
				} else if(isgroup(s[i])) {
					if(s[i] == '"') {
						lexeme[j] = s[i];
						j++;
						i++;
					}
					state = READBLOCK;
				} else if(s[i] == '/' && s[i + 1] == '/') {
					i += 2;
					state = COMMENT;
				} else {
					state = READCHAR;
				}
				break;

			case READCHAR:
				if(my_isspace(s[i])) {
					if(labelFound || pushMode || jumpMode || calling_function || creating_function){
						lexeme[j] = s[i];  // Add the space after "PUSH" or after the label
						j++;
						state = SKIP;  // Move to SKIP state to skip further spaces
					} else {
						state = DUMP;
					}
				} else if(s[i] == '\\') {
					i += 2;
				} else if(isgroup(s[i])) {
					if(s[i] == '"') {
						lexeme[j] = s[i];
						j++;
						i++;
					}
					state = READBLOCK;
				} else if(isspecial(s[i])) {
					if (j == 0) {
						lexeme[j] = s[i];
						j++;
						i++;
					}
					state = DUMP;
				} else if (s[i] == '/' && s[i + 1] == '/') {
					i += 2;
					state = COMMENT;
				} else {
					lexeme[j] = s[i];
					j++;
					i++;
					// Check if the lexeme is a label
					if (j > 0 && lexeme[j - 1] == '@'){
						std::cout << "creating_function set to true" << std::endl;
						creating_function = true;
						calling_function = false;
					}
					else if (j >= 3 && strncmp(lexeme + j - 3, "CALL", 3) == 0){
						creating_function = false;
						calling_function = true;  // Set the labelFound flag
					}
					if (j > 0 && lexeme[j - 1] == ':') {
						labelFound = true;  // Set the labelFound flag
					}
					if (j >= 7 && strncmp(lexeme + j - 7, "PUSHNULL", 7) == 0) {
						pushMode = false;
						pushNullMode = true;
					} else if (j >= 4 && strncmp(lexeme + j - 4, "PUSH", 4) == 0) {
						pushNullMode = false;
						pushMode = true;
					}
					else if ((j >= 2 && strncmp(lexeme + j - 2, "JMP", 2) == 0) || (j >= 1 && strncmp(lexeme + j - 1, "JZ", 1) == 0)){
						jumpMode = true;  // Set the pushMode flag
						//std::cout << "jump mode set to true" << std::endl;
					}
				}
				break;


			case READBLOCK:
				if(s[i] == beg_char && s[i] != '"') {
					balance++;
					lexeme[j] = s[i];
					j++;
					i++;
				} else if (s[i] == end_char) {
					balance--;
					lexeme[j] = s[i];
					j++;
					i++;
					if(balance <= 0) {
						state = DUMP;
					}
				} else if (end_char == '"' && s[i] == '\\') {
					// TODO: fix this to actually record the chars
					i += 2;
				} else {
					lexeme[j] = s[i];
					j++;
					i++;
				}
                break;

			case SKIP:
				while(my_isspace(s[i])) {
					i++;  // Skip the space
				}

				if(creating_function){

					while (!my_isspace(s[i]) && i < len) {
                        lexeme[j++] = s[i++];
                    }
                    creating_function = false;  // Reset pushMode after handling the command
				}
				else if(calling_function){
					while (!my_isspace(s[i]) && i < len) {
                        lexeme[j++] = s[i++];
                    }
                    calling_function = false;  // Reset pushMode after handling the command
				}
				else if(labelFound){
					// Add the next value (following the space) to the lexeme

					while(!my_isspace(s[i]) && i < len) {
						lexeme[j] = s[i];
						j++;
						i++;
					}
					lexeme[j] = ' '; // Add space between label and command

					std::cout << std::string(lexeme, j) << std::endl; // Print current lexeme content
					labelFound = false;  // Reset labelFound after handling the value
					
					// Check if the command is a "PUSH"
					if ((std::string(lexeme, j).find("CALL") != std::string::npos)) {
						//std::cout << "push after label" << std::endl;
                        calling_function = true;  // Set the pushMode flag
					}
					if(std::string(lexeme, j).find("PUSHNULL") != std::string::npos){
						pushNullMode = true;
					}
                    else if ((std::string(lexeme, j).find("PUSH") != std::string::npos)) {
						//std::cout << "push after label" << std::endl;
                        pushMode = true;  // Set the pushMode flag
                    } else if((std::string(lexeme, j).find("JMP") != std::string::npos) || (std::string(lexeme, j).find("JZ") != std::string::npos)) {
						//std::cout << "jump after label" << std::endl;
                        jumpMode = true;  // Set the pushMode flag
                    } 
	
				} else if (pushMode) {
                    // Read command after "PUSH"
                    while (!my_isspace(s[i]) && i < len) {
                        lexeme[j++] = s[i++];
                    }
                    pushMode = false;  // Reset pushMode after handling the command
                } else if(jumpMode){
					// Read command after "JMP" or "JZ"
                    while (!my_isspace(s[i]) && i < len) {
                        lexeme[j++] = s[i++];
                    }
                    jumpMode = false;  // Reset pushMode after handling the command
				}
				
				state = READCHAR;
				
				break;

			case DUMP:
				if(j > 0) {
					lexeme[j] = 0;
					strlst.push_back(lexeme);
					j = 0;
				}
				// if (j > 0) {
       			// 	lexeme[j] = 0;
        
				// 	if (creating_function) {
					
				// 		// Peek ahead to check if next token is the function name
				// 		while (i < len && my_isspace(s[i])) i++; // Skip spaces
				// 		int start = i;
				// 		while (i < len && !my_isspace(s[i])) {
				// 			lexeme[j++] = s[i++];
				// 		}
				// 		lexeme[j] = 0;
				// 		creating_function = false; // Reset after appending function name
				// 	}
						
				// 	strlst.push_back(lexeme);
				// 	j = 0;
				// }

				pushMode = false;  // Ensure it's reset after dumping
				pushNullMode = false;
				labelFound = false;
				jumpMode = false;
				calling_function = false;
				creating_function = false;
				state = START;
				break;
				
			case COMMENT:
				if(s[i] != '\n') {
					i++;
				} else {
					state = READCHAR;
				}
                break;

			case END:
				i = len;
                break;
		}
	}
	if(j > 0) {
		lexeme[j] = 0;
		strlst.push_back(lexeme);
	}
	return strlst;
}

// This function allows us to define what a space is
bool Lexer::my_isspace(char c) {
	switch (c) {
		case '\n':
		case '\r':
		case '\t':
		case '\v':
		case ' ':
		case '\f':
			return true;
		default:
			return false;
	}
}

bool Lexer::isgroup(char c) {
	beg_char = c;
	switch (c){
		case '"':
			end_char = '"';
			return true;
		case '(':
			end_char = ')';
			return true;
		case ')':
			return true;
		default:
			return false;
	}
}
bool Lexer::isspecial(char c) {
	switch (c) {
		case '[':
		case ']':
			return true;
		default:
			return false;
	}
}
