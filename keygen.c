/*******************************************************************
* File: Keygen Program4
* Author: Chelsea Starr
* Date: 11/30/2018
* Desctiption: Creates a key file of specified length using uppcase
* alphabet + " " (27 chars total).
*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

void generate_key(int);


/*******************************************************************
* function: main
* description: convert user input to int. Generate key using function
* does not error check user input for key length
*******************************************************************/
int main(int argc, char* argv[])
{
  int key_len;

  srand(time(NULL));

  // convert to int
  key_len = atoi(argv[1]);

  // call function to generate and print key
  generate_key(key_len);

}


/*******************************************************************
* function: generate_key
* description: send specified number of random chars ("A"-"Z" and " ")
* to stdout
*******************************************************************/
void generate_key(int len)
{
  int i;
  char rand_char;
  int rand_int;

  // loop generation specified number of chars
  for(i = 0; i < len; i ++)
  {
    rand_char = (rand() % 27) + 65;   // ACSII vals "A" to "Z" and "["
    if(rand_char == 91)               // if "[", replace with " "
    {
      rand_char = ' ';
    }
    fprintf(stdout, "%c", rand_char); // print char to stdout
  }
  fprintf(stdout, "\n");              // output to stdout
}
