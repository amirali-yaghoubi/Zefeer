#include "common/diagnostic.h"
#include <stdio.h>


void diagnostic_report(DiagnosticContext* dc, const char* msg)
{
    const char* diag_msg;
    if(dc->type == DIAG_ERROR) {
        diag_msg = "[ERROR]: %s: %ld:\n%s\n";
    } else if(dc->type == DIAG_WARNING) {
        diag_msg = "[WARNING]: %s: %ld:\n%s\n";
    } else {
        return;
    }

    printf(diag_msg, dc->file_name, dc->line, msg)
}