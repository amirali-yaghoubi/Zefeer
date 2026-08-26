#include "common/diagnostic.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

void diagnostic_report(DiagnosticContext* dc, const char* format, ...)  // CHANGE THIS LINE
{
    dc->has_error = true;
    
    // Format the message with variable arguments
    va_list args;
    va_start(args, format);
    char msg[2048];
    vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);
    
    const char* diag_msg;

    if(dc->note != NULL){
       if(dc->type == DIAG_ERROR) {
            diag_msg = "[ERROR]: %s: %ld: %d\n%s\n[NOTE]: %s\n";
        } else if(dc->type == DIAG_WARNING) {
            diag_msg = "[WARNING]: %s: %ld: %d\n%s\n[NOTE]: %s\n";
        } else {
            return;
        }
        printf(diag_msg, dc->file_name, dc->line, msg, dc->note);
    } else {
        if(dc->type == DIAG_ERROR) {
            diag_msg = "[ERROR]: %s: %ld: %d\n%s\n";
        } else if(dc->type == DIAG_WARNING) {
            diag_msg = "[WARNING]: %s: %ld: %d\n%s\n";
        } else {
            return;
        }
        printf(diag_msg, dc->file_name, dc->line, dc->col, msg);
    }
}