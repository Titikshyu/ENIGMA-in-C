// ENIGMA IN C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//#define DEBUG
#define MAX_MESSAGE_LENGTH 100

//Strut Prototypes
struct Rotor;

//Function Prototypes
void getInput(char*, int*);//bipin
void initialisePlugboard(char*, char*);//abisek
void initialiseRotor(struct Rotor*, char*, char, char, char);//me
void rotorShift(struct Rotor*, int);//me
void enigmaEncode(char*, char*, int , struct Rotor*, struct Rotor*, struct Rotor*, struct Rotor*);//abhisek
char newLetter(char, struct Rotor*, struct Rotor*, struct Rotor*, struct Rotor*);//abhisek
void plugboardEncode(char*,int,char*, char*);//abisek
void userch(char *, char *, char *,char *, char *, char *, char *);//bipin
//Structure to store individual rotor settings
struct Rotor {

	char rotorCurrentPosition;
	char rotorOffset;
	char ringPosition;
	char carryPosition;
	char leftToRight[26];
	char rightToLeft[26];
	
};


int main() {

	char inputMessage[MAX_MESSAGE_LENGTH] = { 0 };
	char outputMessage[MAX_MESSAGE_LENGTH] = { 0 };
	char plugboardConnections[26];
	int messageLength;
	int i = 0;
	char plug[100],rotI[26],rotII[26],rotIII[26],reflect[26];
	int rot1, rot2, rot3, ref;
	struct Rotor rI, rII, rIII, reflector;
	char c1,c2,c3;
	
	printf("Enter plugboard connections(eg : ab cd) : ");
	gets(plug);
	
	for (i = 0; plug[i]!='\0'; i++) {
    if(plug[i] >= 'a' && plug[i] <= 'z') 
	{
        plug[i] = plug[i] - 32;
    }
   }
	//Setup plugboard connections. Use format "AH,JI,FY" 
	//NB with the plugboard when input A is connected to output H, input H is also connected to output A, J=I and I=J, etc. This in not ture for the rotors.
	initialisePlugboard(plug, plugboardConnections);
	
	//Get input text and length
	getInput(inputMessage, &messageLength);

	//Check for a valid message
	if (messageLength == 0) {
		printf("Unexpected character entered. Only A to Z are allowed\n");
		system("PAUSE");
		return -1;
	}
	
	//First pass through Plugboard
	plugboardEncode(inputMessage, messageLength, outputMessage, plugboardConnections);

	//Copy Output back to input ready for next stage
	memcpy(inputMessage, outputMessage, messageLength * sizeof(char));

	userch(rotI,rotII,rotIII,reflect,&c1,&c2,&c3);
	//Initialise the default rotors and reflectors
	//initialiseRotor(rotor pointer, "Rotor Wiring Connections", 'Rotor Offset', 'Ring Position', 'Notch Position');
	
	initialiseRotor(&rI, rotI, 'A',c1, 'Q');
	initialiseRotor(&rII, rotII, 'A',c2, 'E');
	initialiseRotor(&rIII, rotIII, 'A',c3, 'V');
	initialiseRotor(&reflector, reflect, 'A', 'A', 'A');

	//Pass through enigma rotors
	enigmaEncode(inputMessage, outputMessage, messageLength, &rI, &rII, &rIII, &reflector);

	//Copy Output back to input ready for next stage
	memcpy(inputMessage, outputMessage, messageLength * sizeof(char));
	
	//Second pass though plugboard
	plugboardEncode(inputMessage, messageLength, outputMessage, plugboardConnections);
	
	printf("Output Message : ");
	for (i = 0; i < messageLength; i++) {
		printf("%c", outputMessage[i] + 65);
	}
	printf("\n");
	system("PAUSE");
	return 0;
}
void userch(char *rotI, char *rotII, char *rotIII,char *reflect,char *c1, char *c2, char *c3)
{
	int rot1,rot2,rot3,ref,i;
	char ch1,ch2,ch3;
	char rotor[5][26]= { "EKMFLGDQVZNTOWYHXUSPAIBRCJ", "AJDKSIRUXBLHWTMCQGZNPYFVOE", "BDFHJLCPRTXVZNYEIWGAKMUSQO", "ESOVPZJAYQUIRHXLNFTGKDCMWB", "VZBRGITYUPSDNHLXAWMJQOFECK"};
	char reflects[3][26]={ "EJMZALYXVBWFCRQUONTSPIKHGD", "YRUHQSLDPXNGOKMIEBFZCWVJAT", "FVPJIAOYEDRZXWGCTKUQSBNMHL"};
	printf("Enter rotor settings (eg : 1 2 3) : ");
	scanf("%d%d%d",&rot1,&rot2,&rot3);
	rot1--;
	rot2--;
	rot3--;
	for(i=0;i<26;i++){
		rotI[i] = rotor[rot1][i];
	}
	for(i=0;i<26;i++){
		rotII[i] = rotor[rot2][i];
	}
	for(i=0;i<26;i++){
		rotIII[i] = rotor[rot3][i];
	}
	printf("Which refelctor you want to use?\n(A : 1)\n(B : 2)\n(C : 3)\n");
	scanf("%d",&ref);
	ref--;
	printf("\n");
	for(i=0;i<26;i++){
		reflect[i]= reflects[ref][i];
	}
	printf("Enter Ring settings eg:(A B C)\n");
	getchar();
	scanf("%c %c ",&ch1,&ch2);
	scanf("%c",&ch3);
	*c1=toupper(ch1);
	*c2=toupper(ch2);
	*c3=toupper(ch3);
}	

void initialisePlugboard(char *desiredPlugConnections, char *plugboardConnections) {
	
	int i = 0;
	char x;
	//Initialise all connections to be a normal passthrough. E.g. A to A, B to B, C to C, etc.
	//A is stored as 0, B is stored as 1, Z is stored as 25, etc.
	for ( x = 0; x < 26; x++){
		plugboardConnections[x] = x;
	}
	
	//Plug Settings are in format "AH,JI,FY" 
	//This means A is connected to H and H is connected to A, J=I and I=J, etc
	while (desiredPlugConnections[i] != '\0') {

		if (desiredPlugConnections[i] == ' ') {
			i++;
		}
		else {

			plugboardConnections[desiredPlugConnections[i] - 65] = desiredPlugConnections[i + 1] - 65;
			plugboardConnections[desiredPlugConnections[i + 1] - 65] = desiredPlugConnections[i] - 65;
			i += 2;
		}		
	}
	
	#ifdef DEBUG
	printf("Plugboard Settings \n");
	for (char x = 0; x < 26; x++){
		printf("In: %c \t Out: %c\n\r", x + 65, plugboardConnections[x] + 65);
	}
	#endif
	
	return;
}

void plugboardEncode(char *inputMessage, int messageLength, char *outputMessage, char *plugboardConnections ) {

	int i = 0;

	for (i = 0; i < messageLength; i++) {
		outputMessage[i] = plugboardConnections[inputMessage[i]];
	}

	return;
}

void getInput(char* inputMessage, int* messageLength) {
	
	int i = 0;

	printf("Enter message: ");
	
	//Get user message. Max size is set by MAX_MESSAGE_LENGTH
	fgets(inputMessage, MAX_MESSAGE_LENGTH, stdin);

	//Go through all the entered chars and format them correctly
	while (inputMessage[i] != '\n' && i < MAX_MESSAGE_LENGTH) {

		//Replace spaces with X
		if (inputMessage[i] == ' ') inputMessage[i] = 'X' - 65;

		//Convert lower case letters to upper case and shift A-Z range to between 0 and 25 where A=0, B=1, C=2, etc
		if (inputMessage[i] >= 97 && inputMessage[i] <= 122) inputMessage[i] -= 97;

		//Shift to A-Z range to between 0 and 25 where A = 0, B = 1, C = 2, etc
		if (inputMessage[i] >= 65 && inputMessage[i] <= 90) inputMessage[i] -= 65;

		//Test to make sure all chars are now only in range A-Z. If they are not then quit
		if (inputMessage[i] < 0 || inputMessage[i] > 25) {
			*messageLength = 0;
			return;
		}

		i++;
	}

	*messageLength = i;

	//ifdef gareko code disregard hunxa tyo portion ko kasari kaam gareko xa bujhnz if def debug hatauda hunxa 
	#ifdef DEBUG
	printf("Formatted Input: ");
	for (i = 0; i < *messageLength; i++) {
		printf("%c", inputMessage[i] + 65);
	}	
	printf("\tLength: %d\n", *messageLength);
	#endif

	return;
}

void rotorShift(struct Rotor* rotor, int shiftSize) {

	char tempArray[26];
	int i = 0;
	int j = 0;

	//Create a temp copy of the rotor connections so that is can be shifted
	memcpy(&tempArray, &rotor->leftToRight, 26 * sizeof(char));//memcpy le copy gardinxa hai text

	for (i = 0; i < 26; i++) {

		//Corrects the index when the shift wraps over the array
		if (i + shiftSize < 0) {
			j = (i + shiftSize) + 26;
		}
		else if (i + shiftSize > 25) {
			j = (i + shiftSize) - 26;
		}
		else  j = i + shiftSize;

		rotor->leftToRight[i] = tempArray[j];

		//Adjust for at output of rotor

		if (rotor->leftToRight[i] - shiftSize > 25) {
			rotor->leftToRight[i] = (rotor->leftToRight[i] - shiftSize) - 26;
		}
		else if (rotor->leftToRight[i] - shiftSize < 0) {
			rotor->leftToRight[i] = (rotor->leftToRight[i] - shiftSize) + 26;
		}
		else rotor->leftToRight[i] -= shiftSize;

	}

	//Convert from the Left to Right array to the Right to Left array
	for (i = 0; i < 26; i++) {
		rotor->rightToLeft[rotor->leftToRight[i]] = i;
	}

	//Update the current position variable in the rotor struct
	if (rotor->rotorCurrentPosition + shiftSize > 25) {
		rotor->rotorCurrentPosition = (rotor->rotorCurrentPosition + shiftSize) - 26;
	}
	else if (rotor->rotorCurrentPosition + shiftSize < 0) {
		rotor->rotorCurrentPosition = (rotor->rotorCurrentPosition + shiftSize) + 26;
	}
	else rotor->rotorCurrentPosition += shiftSize;

}

void initialiseRotor(struct Rotor* rotor, char* rotorConnections, char rotorOffset, char ringPosition, char carryPosition) {

	//Fill in initial rotor struct with the provided parameters

	int i = 0;
	char initialOffset = 0;

	//Assign the rotor connections. A is stored as 0, B is stored as 1, Z is stored as 25, etc.
	//Connections should be provided as a string. Eg. "BGJKYU...." would map A to B, B to G, C to J, D to K, E to Y, and F to U etc.
	for (i = 0; i < 26; i++){
		rotor->leftToRight[i] = rotorConnections[i] - 65; //Minus 65 converts from ASCII to a number from 0-25
	}
	
	//Convert from the Left to Right array to the Right to Left array
	for (i = 0; i < 26; i++) {
		rotor->rightToLeft[rotor->leftToRight[i]] = i;
	}

	//Set the offset, ring, and carry settings
	//These are all stored as 0-25 instead of A to Z for easier calcs during operation
	rotor->rotorOffset = rotorOffset - 65;
	rotor->ringPosition = ringPosition - 65;
	rotor->carryPosition = carryPosition - 65;
	rotor->rotorCurrentPosition = 0;


	//Calculate initial offset from the ring and offset settings
	if ((rotorOffset - ringPosition) < 0)  {
		initialOffset = (rotorOffset - ringPosition) + 26;
	}
	else   initialOffset = (rotorOffset - ringPosition);

	//If there is an offset apply it to the rotor
	if (initialOffset != 0) rotorShift(rotor, initialOffset);
	
	//Correct the rotor position variable as it will be wrong the first time due to the ring setting offset
	rotor->rotorCurrentPosition = rotorOffset - 65;

	#ifdef DEBUG
	printf("Rotor settings\n");
	for (char i = 0; i < 26; i++){
		printf("In: %c\tL2R: %c\tR2L: %c\n\r", i + 65, rotor->leftToRight[i] + 65, rotor->rightToLeft[i] + 65);
	}
	#endif

	
	return;
}

//P1 = Position 1 which is the left most rotor. P3 = Position 3 which is the right most rotor
char newLetter(char letter, struct Rotor* P1, struct Rotor* P2, struct Rotor* P3, struct Rotor* reflector) {

	char firstPass, reflected, secondPass;
	char carryPos;

	//Always shift the first rotor
	rotorShift(P3, 1);

	//Catch case where carry position + 1 would overflow. i.e where the carry notch is at Z
	if (P3->carryPosition + 1 > 25) carryPos = 0;
	else carryPos = P3->carryPosition + 1;
	
	//Shift the middle rotor if carry notch engaged
	if (P3->rotorCurrentPosition == carryPos) {
		
		rotorShift(P2, 1);
	}
	//Double step condition - Shift both middle and left most rotor
	else if (P2->rotorCurrentPosition == P2->carryPosition) {
		rotorShift(P2, 1);
		rotorShift(P1, 1);
	}
		
	firstPass = P1->leftToRight[P2->leftToRight[P3->leftToRight[letter]]];
	reflected = reflector->leftToRight[firstPass];
	secondPass = P3->rightToLeft[P2->rightToLeft[P1->rightToLeft[reflected]]];

	return secondPass;

}

void enigmaEncode(char* inputMessage, char* outputMessage, int messageLength, struct Rotor* P1, struct Rotor* P2, struct Rotor* P3, struct Rotor* reflector) {
	
	int i = 0;

	//Pass each input letter through the rotors and store in the output array
	for (i = 0; i < messageLength; i++) {

		outputMessage[i] = newLetter(inputMessage[i], P1, P2, P3, reflector);
	}

	return;
}


