//Author: Mariuspersen <marius_kule@hotmail.com>
//This is free and unencumbered software released into the public domain.
//
//Anyone is free to copy, modify, publish, use, compile, sell, or
//distribute this software, either in source code form or as a compiled
//binary, for any purpose, commercial or non-commercial, and by any
//means.
//
//In jurisdictions that recognize copyright laws, the author or authors
//of this software dedicate any and all copyright interest in the
//software to the public domain. We make this dedication for the benefit
//of the public at large and to the detriment of our heirs and
//successors. We intend this dedication to be an overt act of
//relinquishment in perpetuity of all present and future rights to this
//software under copyright law.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
//OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
//ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//OTHER DEALINGS IN THE SOFTWARE.
//
//For more information, please refer to <http://unlicense.org/>

#ifndef SB_H_
#define SB_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#ifndef SBDEF
#define SBDEF
#endif // SBDEF

// printf macros for StringBuild
#define SB_Fmt "%.*s"
#define SB_Arg(sb) (int) (sb)->length, (sb)->data

typedef struct {
    char* data;
    size_t length;
    size_t capacity;
} String_Builder;


const char* sb_get_string(const String_Builder* sb);
size_t sb_get_length(const String_Builder* sb);
size_t sb_get_capacity(const String_Builder* sb);
bool sb_trim(String_Builder *sb);
bool sb_get_line_index(String_Builder* sb, size_t line_number, size_t* index);
String_Builder *sb_create(size_t initial_capacity);
String_Builder *sb_create_from_file(const char* filename);
String_Builder *sb_create_from_filepointer(FILE* f);
bool sb_save_to_file(String_Builder* sb, const char* filename);
bool sb_expand_capacity(String_Builder* sb, size_t str_length);
bool sb_append(String_Builder *sb, const char *str);
bool sb_append_format(String_Builder* sb, const char* format, ...);
bool sb_append_line(String_Builder *sb, const char *str);
bool sb_insert(String_Builder *sb, size_t start_idx, const char* str);
bool sb_insert_format(String_Builder* sb, size_t start_idx, const char* format, ...);
bool sb_insert_line(String_Builder *sb, size_t line_number, const char* str);
bool sb_insert_line_format(String_Builder* sb, size_t line_number, const char* format, ...);
bool sb_delete(String_Builder *sb, size_t start_idx, size_t end_idx);
bool sb_delete_line(String_Builder* sb, size_t line_number);
void sb_destroy(String_Builder *sb);

#endif  // SB_H_
#ifdef SB_IMPLEMENTATION

SBDEF const char* sb_get_string(const String_Builder* sb) {
    return sb->data;
}

SBDEF size_t sb_get_length(const String_Builder* sb) {
    return sb->length;
}

SBDEF size_t sb_get_capacity(const String_Builder* sb) {
    return sb->capacity;
}

SBDEF bool sb_trim(String_Builder *sb) {
    if (sb == NULL) {
        return false;
    }

    sb->capacity = strlen(sb->data) + 1;

    char* temp_capacity = realloc(sb->data, sb->capacity);

    if (temp_capacity == NULL) {
        return false;
    }

    sb->data = temp_capacity;

    return true;
}

SBDEF bool sb_get_line_index(String_Builder* sb, size_t line_number, size_t* index) {
    if (line_number == 0) {
        *index = 0;
        return true;
    }

    size_t current_line = 0;
    size_t i = 0;
    
    for (i = 0; i < sb->length; i++) {
        if (current_line == line_number) {
            *index = i;
            return true;
        }
        if (sb->data[i] == '\n') {
            current_line++;
        }
    }

    if (current_line == line_number) {
        *index = i;
        return true;
    }

    return false;
}

SBDEF String_Builder *sb_create(size_t initial_capacity) {
    String_Builder* sb = (String_Builder*)malloc(sizeof(String_Builder));

    if (initial_capacity == 0) initial_capacity++;
    
    sb->data = malloc(initial_capacity);
    sb->capacity = initial_capacity;
    sb->data[0] = '\0';
    sb->length = strlen(sb->data);
    return sb;
}

SBDEF String_Builder *sb_create_from_filepointer(FILE* f) {

    if (f == NULL) {
        perror("Error opening file\n");
        return NULL;
    }

    if(fseek(f, 0, SEEK_END) != 0) {
        perror("Error seeking end of file\n");
        fclose(f);
        return NULL;
    } 
    
    size_t filesize = ftell(f);

    if(fseek(f, 0, SEEK_SET)!= 0) {
        perror("Error seeking beginning of file\n");
        fclose(f);
        return NULL;
    }

    String_Builder* sb = sb_create(filesize);

    if (sb == NULL) {
        perror("Error creating string builder\n");
        fclose(f);
        sb_destroy(sb);
        return NULL;
    }


    size_t sizeread = fread(sb->data, 1, filesize, f);
    sb->length = filesize;

    if(sizeread != filesize) {
        perror("Error filesize does not equal sizeread\n");
        fclose(f);
        sb_destroy(sb);
        return NULL;
    }

    sb->data[filesize] = '\0';
    fclose(f);
    return sb;
}

SBDEF String_Builder *sb_create_from_file(const char* filename) {
    FILE* f = fopen(filename, "r");
    String_Builder* sb = sb_create_from_filepointer(f);
    fclose(f);
    return sb;
}

SBDEF bool sb_save_to_file(String_Builder* sb, const char* filename) {
    FILE* f = fopen(filename, "w");
    if (f == NULL) return false;
    fprintf(f,SB_Fmt,SB_Arg(sb));
    fclose(f);
    return true;
}

SBDEF bool sb_expand_capacity(String_Builder* sb, size_t str_length) {
    if (sb->length + str_length + 1 > sb->capacity) {
        while (sb->length + str_length + 1 > sb->capacity) {
            sb->capacity *= 2;
        }
        sb->data = realloc(sb->data, sb->capacity);
    }
    if (sb->data)
    {
        return true;
    }
    else {
        perror("Unable to realloc data");
        return false;
    }
    
}

SBDEF bool sb_append(String_Builder *sb, const char *str) {
    size_t str_length = strlen(str);
    if(!sb_expand_capacity(sb,str_length)) {
        return false;
    }
    sb->length += str_length;
    strcat(sb->data, str);
    return true;
}

#define FORMAT_STRING(format, result) \
    do { \
        va_list args1; \
        va_list args2; \
        \
        va_start(args1, (format)); \
        va_copy(args2, args1); \
        \
        size_t str_length = vsnprintf(NULL, 0, (format), args2); \
        va_end(args2); \
        \
        (result) = malloc((str_length + 1)); \
        \
        vsnprintf((result), str_length + 1, (format), args1); \
        va_end(args1); \
    } while (0)

SBDEF bool sb_append_format(String_Builder* sb, const char* format, ...) {
    char* temp = NULL;
    FORMAT_STRING(format,temp);
    if(!sb_append(sb,temp)) {
        return false;
    }
    free(temp);
    return true;
}

SBDEF bool sb_append_line(String_Builder *sb, const char *str) {

    if (sb->length < 1) {
        if(!sb_append(sb,str)) return false;
        if(!sb_append(sb,"\n")) return false;
        return true;
    }

    if (sb->data[sb->length - 1] != '\n') {
        if(!sb_append(sb, "\n")) return false;
    }

    if(sb_append(sb,str)) return false;
    if(sb_append(sb,"\n")) return false;

    return true;
}

SBDEF bool sb_insert(String_Builder *sb, size_t start_idx, const char* str) {
    if (start_idx > sb->length) {
        return false;
    }

    size_t num_chars_to_add = strlen(str);
    if(!sb_expand_capacity(sb,num_chars_to_add)) return false;
    sb->length += num_chars_to_add;

    memmove(sb->data + start_idx + num_chars_to_add, sb->data + start_idx, sb->length - num_chars_to_add - start_idx);
    memmove(sb->data + start_idx, str, num_chars_to_add);
    sb->data[sb->length] = '\0';
    
    return true;
}

SBDEF bool sb_insert_format(String_Builder* sb, size_t start_idx, const char* format, ...) {
    char* temp = NULL;
    FORMAT_STRING(format,temp);
    bool return_val = sb_insert(sb,start_idx,temp);
    free(temp);
    return return_val;
}

SBDEF bool sb_insert_line(String_Builder *sb, size_t line_number, const char* str) {
    size_t start_idx = 0;

    if(!sb_get_line_index(sb,line_number,&start_idx)) return false;
    if(!sb_insert(sb,start_idx,str)) return false;
    if(!sb_insert(sb,start_idx + strlen(str),"\n")) return false;;

    return true;
}

SBDEF bool sb_insert_line_format(String_Builder* sb, size_t line_number, const char* format, ...) {
    char* temp = NULL;
    FORMAT_STRING(format,temp);
    bool return_val = sb_insert_line(sb,line_number,temp);
    free(temp);
    return return_val;
}

SBDEF bool sb_delete(String_Builder *sb, size_t start_idx, size_t end_idx) {
    if (start_idx > end_idx || end_idx > sb->length) {
        return false;
    }

    size_t num_chars_to_remove = end_idx - start_idx;
    memmove(sb->data + start_idx, sb->data + end_idx, sb->length - end_idx);
    sb->length -= num_chars_to_remove;
    sb->data[sb->length] = '\0';
    return sb_trim(sb);
}

SBDEF bool sb_delete_line(String_Builder* sb, size_t line_number) {
    size_t start_idx = 0;
    size_t end_idx = 0;

    if(sb_get_line_index(sb,line_number,&start_idx)==false) return false;
    for (end_idx = start_idx;end_idx < sb->length && sb->data[end_idx] != '\n';  end_idx++);
    if (end_idx > start_idx && sb->data[end_idx - 1] == '\r') end_idx--;
    return sb_delete(sb, start_idx, sb->data[end_idx] != '\0' ? end_idx + 1 : end_idx);
}

SBDEF void sb_destroy(String_Builder *sb) {
    if(sb && sb->data) free(sb->data);
    if(sb) free(sb);
}
#endif //SB_IMPLEMENTATION
