#include <stdio.h>
#include <unistd.h>

#define SB_IMPLEMENTATION
#include "sb.h"

int main(void) {

    const char* filename = "test";

    //If file test exists, read that, otherwise create a new string builder
    String_Builder* sb = access(filename, F_OK) == 0 ? sb_create_from_file(filename) : sb_create(0);

    //Appends a couple of lines
    sb_append_line(sb,"Hello World!");
    sb_append_line(sb,"This is on a new line");
    sb_append_line(sb,"This is another line");

    //deletes characters between index 0 and 5
    sb_delete(sb,0,5);
    //deletes line 2
    sb_delete_line(sb,2);

    //Can you guess what this does?
    sb_save_to_file(filename,sb);

    //Print the stringbuilder
    printf(SB_Fmt,SB_Arg(sb));

    //destroy the string builder when you are done with it
    sb_destroy(sb);
    
    return 0;
}