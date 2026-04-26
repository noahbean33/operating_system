#include <stdio.h>
#include <stdarg.h>


void printstr(const char *str)
{
    fputs(str, stdout); 
}

void printstr_chars(const char *str)
{
    int i=0;
    while(*(str+i) != '\0')
    {
        fputc(*(str+i++), stdout);
    }
}

void formatted_print(const char* format, ...)
{
    va_list list;
    va_start(list, format);
    vfprintf(stdout, format, list);
    va_end(list);
}

int main()
{
    // String to print to the console - WITHOUT printf (!)
    const char *str = "A line of text with %d placeholders: %s\n";

    printstr(str);
    printstr_chars(str);
    formatted_print(str, 2, "elephant");
}
