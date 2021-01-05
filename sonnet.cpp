#include <iostream>
#include <cstring>
#include <cctype>
#include <cassert>
#include <map>
#include <string>
#include <fstream>

using namespace std;

#include "sonnet.h"

/* PRE-SUPPLIED HELPER FUNCTIONS START HERE */

/* NOTE: It is much more important to understand the interface to and
   the "black-box" operation of these functions (in terms of inputs and
   outputs) than it is to understand the details of their inner working. */

/* get_word(...) retrieves a word from the input string input_line
   based on its word number. If the word number is valid, the function
   places an uppercase version of the word in the output parameter
   output_word, and the function returns true. Otherwise the function
   returns false. */

bool get_word(const char *input_line, int word_number, char *output_word) {
  char *output_start = output_word;
  int words = 0;

  if (word_number < 1) {
    *output_word = '\0';
    return false;
  }
  
  do {
    while (*input_line && !isalnum(*input_line))
      input_line++;

    if (*input_line == '\0')
      break;

    output_word = output_start;
    while (*input_line && (isalnum(*input_line) || *input_line=='\'')) {
      *output_word = toupper(*input_line);
      output_word++;
      input_line++;
    }
    *output_word = '\0';

    if (++words == word_number)
      return true;

  } while (*input_line);

  *output_start = '\0';
  return false;
}

/* char rhyming_letter(const char *ending) generates the rhyme scheme
   letter (starting with 'a') that corresponds to a given line ending
   (specified as the parameter). The function remembers its state
   between calls using an internal lookup table, such that subsequents
   calls with different endings will generate new letters.  The state
   can be reset (e.g. to start issuing rhyme scheme letters for a new
   poem) by calling rhyming_letter(RESET). */

char rhyming_letter(const char *ending) {

  // the next rhyming letter to be issued (persists between calls)
  static char next = 'a';
  // the table which maps endings to letters (persists between calls)
  static map<string, char> lookup;

  // providing a way to reset the table between poems
  if (ending == RESET) {
    lookup.clear();
    next = 'a';
    return '\0';
  }

  string end(ending);

  // if the ending doesn't exist, add it, and issue a new letter
  if (lookup.count(end) == 0) {
    lookup[end]=next;
    assert(next <= 'z');
    return next++;
  }

  // otherwise return the letter corresponding to the existing ending
  return lookup[end];
}

/* START WRITING YOUR FUNCTION BODIES HERE */

// I) MAIN FUNCTIONS

int count_words(const char* input_line){

  int word_count = (input_line[0] != '\0') ? 1 : 0;
  for (int i = 0; input_line[i] != '\0'; i++){
    if(input_line[i-1]!=' ' && input_line[i]==' ')
      word_count++;
    else if(input_line[i]=='-')
      word_count++;
  }

  return word_count;

}

bool find_phonetic_ending(const char *word, char *phonetic_ending){

  clean_char_array(phonetic_ending);
  char phonemes[512];
  if(!word_does_have_phonemes(word,phonemes))
    return false;
  
  int number_of_phonemes = count_words(phonemes);
  char *phonemes_list[number_of_phonemes];
  string_to_word_array(phonemes, phonemes_list, number_of_phonemes);
  
  for (int i=number_of_phonemes-1; i >= 0; i--){
    
    strcat(phonetic_ending,phonemes_list[i]);
    if(word_has_vowel(phonemes_list[i])){
      break;
    }

  }

  return true;

}

bool find_rhyme_scheme(const char *filename, char *scheme){

  // Reseting
  clean_char_array(scheme);
  rhyming_letter(RESET);

  // Auxiliar variables
  int number_of_words = 0;
  int index = 0;
  char phonetic_ending[512];
  char sonnet_line[512]; // Array of chars with a line of the sonnet
  char last_word_in_line[512];
  std::ifstream in;

  // Get phonetic endings for each line of sonnet

  in.open(filename);
  if (!in)
      return false;

  while(!in.eof()){
    in.getline(sonnet_line,512);
    number_of_words = count_words(sonnet_line);
    get_word(sonnet_line,number_of_words,last_word_in_line);
    if(!find_phonetic_ending(last_word_in_line,phonetic_ending))
      return false;
    scheme[index] = rhyming_letter(phonetic_ending);
    index++;
  }
  scheme[index] = '\0';
  in.close();
  return true;
}

char* identify_sonnet(const char *filename){

  char *output = new char[512];
  char scheme[512];
  char Shakespearean[] = "ababcdcdefefgg";
  char Petrarchan[] = "abbaabbacdcdcd";
  char Spenserian[] = "ababbcbccdcdee";

  find_rhyme_scheme(filename,scheme);
  
  if(strcmp(scheme,Shakespearean) == 0)
    strcpy(output,"Shakespearean");
  else if(strcmp(scheme,Petrarchan) == 0)
    strcpy(output,"Petrarchan");  
  else if(strcmp(scheme,Spenserian) == 0)
    strcpy(output,"Spenserian");  
  else    
    strcpy(output,"Unknown");

  return output;

}

// AUXILIAR FUNCTIONS

bool word_does_have_phonemes(const char *word, char *phonemes){

  std::ifstream in;
  char reader[512];
  in.open("dictionary.txt");
  if (!in)
    return false;
  while(!in.eof()){
    in >> reader;
    if(strcmp(reader,word)==0){
      in >> ws; // skip initial white spaces
      in.getline(phonemes,512); // get phonemes of word
      return true;
    }
  }
  in.close();
  return false;

}

bool word_has_vowel(const char *word){
  char x;
  for(int i=0; word[i]!='\0'; i++){
    x = word[i];
    if (x == 'a' || x == 'e' || x == 'i' || x == 'o' || x == 'u' || 
        x == 'A' || x == 'E' || x == 'I' || x == 'O' || x == 'U')
        return true;
  }
  return false;
}

void string_to_word_array(const char *string, char *words[], 
  int number_of_words){
  
  int word_index = 0;
  int letter_index = 0;
  
  for(int i=0; i < number_of_words; i++){
    words[i] = new char[512];
  }

  for(int i=0; string[i] != '\0'; i++){
    // if '0' or '\0' is found, assign '\0', change for next word
    if(string[i]==' '||string[i]=='\0'){
        words[word_index][letter_index]='\0';
        word_index++;      //for next word
        letter_index=0;    //for next word, start at index 0
    }
    else{
      words[word_index][letter_index]=string[i];
      letter_index++;
    }
  }

  words[word_index+1] = NULL;
}

void clean_char_array(char *array){
  for(int i=0; array[i]!='\0'; i++){
    array[i] = '\0';
  }
}