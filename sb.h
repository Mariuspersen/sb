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
String_Builder *sb_create(size_t initial_capacity);
String_Builder *sb_create_from_file(const char* filename);
bool sb_save_to_file(const char* filename, String_Builder* sb);
void sb_append(String_Builder *sb, const char *str);
void sb_append_format(String_Builder* sb, const char* format, ...);
void sb_append_line(String_Builder *sb, const char *str);
bool sb_delete(String_Builder *sb, size_t start_idx, size_t end_idx);
bool sb_delete_line(String_Builder *sb, size_t line_number);
void sb_destroy(String_Builder *sb);

#endif  // SB_H_
#ifdef SB_IMPLEMENTATION

SBDEF const char* sb_get_string(const String_Builder* sb) {
    return sb->data;
}

SBDEF size_t sb_get_length(const String_Builder* sb) {
    return sb->length;
}

SBDEF String_Builder *sb_create(size_t initial_capacity) {
    String_Builder* sb = malloc(sizeof(String_Builder));

    if (initial_capacity == 0) initial_capacity++;
    
    sb->data = malloc(initial_capacity);
    sb->length = 1;
    sb->capacity = initial_capacity;
    sb->data[0] = '\0';
    return sb;
}

SBDEF String_Builder *sb_create_from_file(const char* filename) {
    FILE* f = fopen(filename, "r");

    if (f == NULL) {
        printf("Error opening file\n");
        return NULL;
    }

    if(fseek(f, 0, SEEK_END) != 0) {
        printf("Error seeking end of file\n");
        fclose(f);
        return NULL;
    }

    size_t filesize = ftell(f);

    if(fseek(f, 0, SEEK_SET)!= 0) {
        printf("Error seeking beginning of file\n");
        fclose(f);
        return NULL;
    }

    String_Builder* sb = sb_create(filesize+1);

    if (sb == NULL) {
        printf("Error creating string builder\n");
        fclose(f);
        sb_destroy(sb);
        return NULL;
    }

    sb->length = filesize+1;

    size_t sizeread = fread(sb->data, 1, filesize, f);

    if(sizeread != filesize) {
        printf("Error filesize does not equal sizeread\n");
        fclose(f);
        sb_destroy(sb);
        return NULL;
    }

    sb->data[filesize] = '\0';
    fclose(f);
    return sb;
}

SBDEF bool sb_save_to_file(const char* filename, String_Builder* sb) {
    FILE* f = fopen(filename, "w");
    if (f == NULL) return false;
    fprintf(f,SB_Fmt,SB_Arg(sb));
    fclose(f);
    return true;
}

SBDEF void sb_check_capacity(String_Builder* sb, size_t str_length) {
    if (sb->length + str_length + 1 > sb->capacity) {
        while (sb->length + str_length + 1 > sb->capacity) {
            sb->capacity *= 2;
        }
        sb->data = realloc(sb->data, sb->capacity);
    }
}

SBDEF void sb_append(String_Builder *sb, const char *str) {
    size_t str_length = strlen(str);
    sb_check_capacity(sb,str_length);
    sb->length += str_length;
    strcat(sb->data, str);
}

SBDEF void sb_append_format(String_Builder* sb, const char* format, ...) {
    va_list args1;
    va_list args2;

    va_start(args1,format);
    va_copy(args2,args1);

    size_t str_length = vsnprintf(NULL,0,format,args2);
    va_end(args2);

    char* temp = malloc(str_length + 1);
    
    vsnprintf(temp, str_length + 1, format, args1);
    va_end(args1);

    sb_append(sb,temp);
    free(temp);
}

SBDEF void sb_append_line(String_Builder *sb, const char *str) {

    if (sb->length < 2) {
        sb_append(sb,str);
        sb_append(sb,"\n");
        return;
    }

    if (sb->data[sb->length - 2] != '\n') {
        sb_append(sb, "\n");
    }

    sb_append(sb,str);
    sb_append(sb,"\n");
}

SBDEF bool sb_insert(String_Builder *sb, size_t start_idx, const char* str) {
    if (start_idx > sb->length) {
        return false;
    }

    size_t num_chars_to_add = strlen(str);
    sb_check_capacity(sb,num_chars_to_add);
    sb->data = realloc(sb->data, sb->capacity);
    sb->length += num_chars_to_add;

    memmove(sb->data + start_idx + num_chars_to_add, sb->data + start_idx, num_chars_to_add);
    memmove(sb->data + start_idx,str,strlen(str));
    sb->data[sb->length] = '\0';
    
    return true;
}

SBDEF bool sb_delete(String_Builder *sb, size_t start_idx, size_t end_idx) {
    if (start_idx > end_idx || end_idx >= sb->length) {
        return false;
    }

    size_t num_chars_to_remove = end_idx - start_idx + 1;
    memmove(sb->data + start_idx, sb->data + end_idx + 1, sb->length - end_idx);
    sb->length -= num_chars_to_remove;
    sb->capacity -= num_chars_to_remove;
    sb->data = realloc(sb->data, sb->capacity);
    sb->data[sb->length] = '\0';

    return true;
}

SBDEF bool sb_delete_line(String_Builder *sb, size_t line_number) {
    if (line_number == 0) {
        return false;
    }

    size_t current_line = 1;
    size_t start_idx = 0;
    size_t i;

    for (i = 0; i < sb->length; i++) {
        if (current_line == line_number) {
            start_idx = i;
            break;
        }
        if (sb->data[i] == '\n') {
            current_line++;
        }
    }

    if (current_line != line_number) {
        return false;
    }

    size_t end_idx = i;
    while (end_idx < sb->length && sb->data[end_idx] != '\n') {
        end_idx++;
    }

    if (end_idx > start_idx && sb->data[end_idx - 1] == '\r') {
        end_idx--;
    }

    return sb_delete(sb, start_idx, end_idx);
}

SBDEF void sb_destroy(String_Builder *sb) {
    free(sb->data);
    free(sb);
}
#endif //SB_IMPLEMENTATION
