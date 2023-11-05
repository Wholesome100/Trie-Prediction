#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "TriePrediction.h"

TrieNode *buildTrie(char *filename);//Functional prototypes, issues occurred without them
TrieNode *createTrieNode(void);
TrieNode *destroyTrie(TrieNode *root);
TrieNode *getNode(TrieNode *root, char *str);
void insertString(TrieNode *root, char *str);
void mostFrequentWord(TrieNode *root, char *str);
void stripPunctuators(char *str);
void printTrie(TrieNode *root, int useSubtrieFormatting);
void printTrieHelper(TrieNode *root, char *buffer, int k);
void inputHelp(TrieNode *root, char *str);
double difficultyRating(void);
double hoursSpent(void);
void mostFrequentWordHelp(TrieNode *root, char *str, char *buffer, int k, int *strCheck);
void predictiveText(TrieNode *root,char *str,int n);


int main(int argc, char **argv)
{
char *manager=malloc(sizeof(char)*1026);//Declaration for variables
char *holder=malloc(sizeof(char)*1026);
int nWords;

FILE *input;//FILE pointer input
TrieNode *root;//Declare a root for the Trie

root=buildTrie(argv[1]);//Set root equal to the returned Trie from buildTrie with 1st argument
input=fopen(argv[2], "r");//Open the file

while(fscanf(input,"%s",manager) == 1)//Read out of the file as long as there is text
{
    if(strcmp("@", manager)==0)//When @ is encountered:
    {
        fscanf(input, "%s", holder);//Get the prediction string
        fscanf(input, "%d", &nWords);//Get the n words

        printf("%s", holder);//Print string as it is, strip it's punctuators, and pass it to predictiveText
        stripPunctuators(holder);
        predictiveText(root, holder, nWords);
        printf("\n");
    }
    inputHelp(root, manager);//Function for basic input
}

fclose(input);//Close the file, destroy the tree, and free the dynamically allocated strings
destroyTrie(root);
free(manager);
free(holder);

return 0;
}




void predictiveText(TrieNode *root,char *str,int n)
{
TrieNode *temp=NULL;//Declare temp node and set it equal to getNode with root and str
temp=getNode(root, str);

if(n!=0)
{
    if(temp==NULL || temp->subtrie==NULL)//NULL guards/checks, if either is NULL/empty then leave
    {
    return;
    }
    else
    {
    mostFrequentWord(temp, str);//Otherwise call frequentWord with temp and str
    printf(" %s", str);//Print str, it will have the most frequent word copied into it
    predictiveText(root, str, n-1);//Call predictiveText again until n=0 or temp=NULL
    }
}
}




void mostFrequentWord(TrieNode *root, char *str)
{
	int *strCheck = malloc(sizeof(int));//Declaration of variables
	char *holder=malloc(sizeof(char)*1026);
    char buffer[1026];//Buffer was declared this way in PrintTrie

	if(root == NULL || root->subtrie==NULL)//NULL guards/checks for root
    {
		return;
	}

	*strCheck = 0;//Flag value, declared as a pointer so it can be passed by reference to mostFrequentWordHelp

	mostFrequentWordHelp(root->subtrie, holder, buffer, 0, strCheck);//mostFrequentWordHrlp uses a modified version of printTrieHelper

	strcpy(str, holder);//Copy the string in holder into str

	free(holder);//Free malloc'ed variables
	free(strCheck);
}




void mostFrequentWordHelp(TrieNode *root, char *holder, char *buffer, int k, int *strCheck)
{
	static int count;//Static int to keep track of the count throughout the recursive calls

	if (root == NULL)//NULL guard/check for root
    {
        return;
    }

	if (root->count > 0)//When we find the first terminal node:
    {
		if(*strCheck == 0)//If the flag=0, copy the buffer string into holder and set count, this sets up our count int and holder string
        {
			strcpy(holder, buffer);//With the way the code from printTrieHelper works, the first terminal node belongs to the alphabetically greatest string, we don't need to change it unless a word with a higher root->count appears
            count=root->count;
			*strCheck = 1;//Set flag=1 so the statement doesn't execute again until a new word arrives
		}
        else if(root->count > count)//If we find a word with a root->count greater than the one we have stored:
        {
			strcpy(holder, buffer);//Copy the buffer string into the holder
			count=root->count;//Set count equal to the new value
		}

	}


	buffer[k + 1] = '\0';//This is a modified version of the printTrieHelper function

	for (int i = 0; i < 26; i++) {
		buffer[k] = 'a' + i;
		mostFrequentWordHelp(root->children[i], holder, buffer, k + 1, strCheck);//Go through the children nodes like in printTrieHelper via recursive calls
	}
	buffer[k] = '\0';
}




void inputHelp(TrieNode *root, char *str)
{
    TrieNode *temp=NULL;//Declare a temp node

    if(strcmp("!", str)==0)//If str is just "!", print the trie
    {
        printTrie(root, 0);
        return;
    }
    else if(strcmp("@", str)==0)
    {
        //This part is handled in main(), it's here to prevent weird behavior
    }
    else//If str is just a string, print it as it is then pass it to getNode
    {
        printf("%s\n", str);
        stripPunctuators(str);
        temp=getNode(root, str);//Use getNode to get the terminal node and print subtrie

        if(temp==NULL)//If temp is NULL, string isn't in trie
        {
            printf("(INVALID STRING)\n");
        }
        else if(temp->subtrie==NULL)//If subtrie is NULL, string is empty
        {
            printf("(EMPTY)\n");
        }
        else//If valid subtrie present, print it
        {
          printTrie(temp->subtrie, 1);
        }
    }
}




TrieNode *buildTrie(char *filename)
{
FILE *fp;//Declare FILE pointer fp and create a holder for input
char *holder=malloc(sizeof(char)*1026);//Variable declarations
char *current, *currentPunc, *next, *nextPunc;
int len;

fp=fopen(filename, "r");//Open the file

TrieNode *root, *trieHold;
root=createTrieNode();//Make the initial trie node


while(fscanf(fp,"%s",holder) == 1)//While loop to go through the file and insert all strings into trie
{
    stripPunctuators(holder);//Strip punctuators from strings
    insertString(root, holder);//Insert string into trie

}

fclose(fp);//Close the file
free(holder);//Free the holder string

//This is the subtrie component
fp=fopen(filename, "r");//Open the file again

current=malloc(sizeof(char)*1026);//Dynamically allocate strings, we need to keep copies of the current and next word to determine their co-occurrence
currentPunc=malloc(sizeof(char)*1026);//We also need to track the punctuation, words only co-occur if they appear together in the SAME SENTENCE
next=malloc(sizeof(char)*1026);
nextPunc=malloc(sizeof(char)*1026);


if(fscanf(fp, "%s",current)==1)//Set the initial string values for current and current Punc
{
    strcpy(currentPunc,current);//Copy current as it is into currentPunc, and strip it
    stripPunctuators(current);
}


while(fscanf(fp,"%s",next)==1)
{
    strcpy(nextPunc,next);//Same deal as if loop above, but this is the next word
    stripPunctuators(next);

    len=strlen(currentPunc);//Get the length of currentPunc and assign it to len


    if(currentPunc[len-1]=='!' || currentPunc[len-1]=='?' || currentPunc[len-1]=='.')//If any of these are present it indicates a sentence end
    {
        strcpy(currentPunc,nextPunc);//Copy the next strings into the current strings
        strcpy(current,next);
        continue;//Continue since there is no co-occurrence
    }

    trieHold=getNode(root, current);//Hold the value of getNode with root and current string

    if(trieHold->subtrie==NULL)//If subtrie is NULL, create a root node and insert the string
    {
        trieHold->subtrie=createTrieNode();
        insertString(trieHold->subtrie, next);
    }
    else
    {
        insertString(trieHold->subtrie, next);//Otherwise just use insertString as normal
    }

    strcpy(currentPunc, nextPunc);//Copy the next strings into the current strings
    strcpy(current, next);

}

free(current);//Free all malloc'ed variables
free(currentPunc);
free(next);
free(nextPunc);
//free(holder);

fclose(fp);//close the file

return root;//Return the Trie
}




TrieNode *destroyTrie(TrieNode *root)
{
for(int i=0; i<26; i++)//Go through the 26 child nodes
{
    if(root->children[i]!=NULL)
    {
        destroyTrie(root->children[i]);
    }
}

if(root->subtrie!=NULL)//Delete the subTrie if present
{
    destroyTrie(root->subtrie);
}

free(root);//Free the root node and return

return NULL;
}




TrieNode *createTrieNode(void)
{
TrieNode *node=malloc(sizeof(TrieNode));//Malloc for a TrieNode

node->count=0;//Initialize count to 0

node->subtrie=NULL;//Set subtrie to NULL

for(int i=0;i<26;i++)
{
    node->children[i]=NULL;//Set all children nodes to NULL
}

return node;//Return the created node when finished
}




TrieNode *getNode(TrieNode *root, char *str)
{
int letter;

if(str[0]=='\0')//Once we reach the end of the string, return root
{
    return root;
}

letter=str[0]-'a';//Ascii subtraction gives use the int value of the letter's position

if(root->children[letter]!=NULL)//If there is a valid node at position length, make recursive calls to go through it
{
    return getNode(root->children[letter], str+1);
}
else//If child node is NULL, return NULL
{
    return NULL;
}
}




void insertString(TrieNode *root, char *str)//insertString function provided in class
{
int length, i;
TrieNode *temp;

if(root == NULL || str == NULL)//Null guard/checks for root
{
    return;
}


length = strlen(str);//Set length equal to the length of str

 for(i=0; i<length; i++)//Until i=length:
{
temp = root->children[tolower(str[i]) - 'a'];//Set temp = the child node at position

 if(temp == NULL)//If temp is NULL then createnode at the position, otherwise just increment the root->count variable
temp = root->children[tolower(str[i]) - 'a'] = createTrieNode();

 root = temp;
}

 root->count++;
}







void stripPunctuators(char *str)
{
int n=strlen(str);//Variable declarations
int j=0;
char *temp=malloc(sizeof(char)*strlen(str)+1);

for(int i=0;i<n;i++)
{
    if(isalpha(str[i])==0 && str[i]!=' ')
    {
        //Do nothing, move on to the next character
    }
    else
    {
        temp[j]=tolower(str[i]);//Otherwise make the character lowercase and copy it into temp
        j++;
    }
}
temp[j]='\0';//Make the last character in temp the terminating character

strcpy(str, temp);//Copy the string from temp into str
}






void printTrie(TrieNode *root, int useSubtrieFormatting)//printTrie provided by original project dev
{
	char buffer[1026];

	if (useSubtrieFormatting)
	{
		strcpy(buffer, "- ");
		printTrieHelper(root, buffer, 2);
	}
	else
	{
		strcpy(buffer, "");
		printTrieHelper(root, buffer, 0);
	}
}





void printTrieHelper(TrieNode *root, char *buffer, int k)//printTrieHelper provided by original project dev
{
	int i;

	if (root == NULL)
		return;

	if (root->count > 0)
		printf("%s (%d)\n", buffer, root->count);

	buffer[k + 1] = '\0';

	for (i = 0; i < 26; i++)
	{
		buffer[k] = 'a' + i;

		printTrieHelper(root->children[i], buffer, k + 1);
	}

	buffer[k] = '\0';
}



double difficultyRating(void)
{
 double hard;
 hard=5.0;//This project was very difficult, it took me some time to figure out the necessary helper functions
 return hard;
}



double hoursSpent(void)
{
 double time;
 time=10.0;//I spent a while on this project, estimate around 10 hours
 return time;
}



