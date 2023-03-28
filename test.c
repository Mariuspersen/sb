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
    sb_append_line(sb,"This line gets deleted");
    sb_append_line(sb,"This is on a new line");
    sb_append_line(sb,"This is another line");
    sb_append_format(sb,"Does %s work?\n","this");

    //You can also insert a line between existing lines
    sb_insert_line(sb,2,"This is a inserted line\n");

    //Or you can insert with whatever index you want
    sb_insert(sb,0,"AAA");

    //You can delete characters between two indexes
    sb_delete(sb,0,1);
    
    //Or delete an entire line
    sb_delete_line(sb,3);

    //Can you guess what this does?
    sb_save_to_file(sb,filename);

    //If you are deleting alot and really care about memory usage you can use this to trim off the fat
    sb_trim(sb);

    //Print the stringbuilder
    printf(SB_Fmt,SB_Arg(sb));

    //destroy the string builder when you are done with it
    sb_destroy(sb);
    
    return 0;
}