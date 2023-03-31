#include <stdio.h>
#include <unistd.h>

#define SB_IMPLEMENTATION
#include "sb.h"

int main(void) {

    const char* filename = "test";

    //If file test exists, read that, otherwise create a new string builder
    String_Builder* sb = access(filename, F_OK) == 0 ? sb_create_from_file(filename) : sb_create(0);

    //Appends a couple of lines
    sb_append_line(sb, "Line 1");
    sb_append_line(sb, "Line 2");
    sb_append_line(sb, "Line 3");
    sb_append_line(sb, "Line 4");
    sb_append_line(sb, "Line 5");
    sb_append_line(sb, "Line 6");

    //Insert text
    sb_insert(sb,0,"AAAAAA");

    //Insert a line
    sb_insert_line(sb,0,"Line Insert 1");
    sb_insert_line(sb,2,"Line Insert 2");
    
    //deletes characters between index 0 and 5
    sb_delete(sb,0,5);

    //Delete some lines
    sb_delete_line(sb,0);
    sb_delete_line(sb,0);
    sb_delete_line(sb,0);

    //Can you guess what this does?
    sb_save_to_file(sb,filename);

    //Print the stringbuilder
    printf(SB_Fmt,SB_Arg(sb));

    //destroy the string builder when you are done with it
    sb_destroy(sb);
    
    return 0;
}