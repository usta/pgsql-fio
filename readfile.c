/**
Copyright (c) 2015, Cafer Şimşek
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

* Neither the name of pgsql-fio nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "fio.h"

#define BUFFER_SIZE 1024

Datum fio_readfile(PG_FUNCTION_ARGS) {
    text *v_filename;
    char *filename;
    char *content;
    int totalcount = 0;
    char buffer[BUFFER_SIZE];
    FILE *fd;
    size_t bytecount;
    bytea *result;
    size_t filesize;
    v_filename = PG_GETARG_TEXT_P(0);
    filename = text_to_cstring(v_filename);
    if ((fd = fopen(filename, "r+")) == NULL) {
        elog(ERROR, "cannot open file: %s", filename);
        return 0;
    }
    fseek(fd, 0, SEEK_END);
    filesize = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    content = (char *) palloc(filesize);
    while (true) {
        int readcount = fread(buffer, 1, BUFFER_SIZE, fd);
        elog(NOTICE, "readcount: %d", readcount);
        memcpy(content + totalcount, buffer, readcount);
        totalcount += readcount;
        if (readcount < BUFFER_SIZE) {
            break;
        }
    }
    fclose(fd);
    bytecount = VARHDRSZ + filesize;
    result = (bytea *) palloc(bytecount);
    SET_VARSIZE(result, filesize);
    memcpy(VARDATA(result), content, filesize);
    pfree(content);
    PG_RETURN_BYTEA_P(result);
}
