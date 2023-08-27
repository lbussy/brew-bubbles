/* Copyright (C) 2019-2023 Lee C. Bussy (@LBussy)

This file is part of Lee Bussy's Brew Bubbles (brew-bubbles).

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

/* Loosely based on:
https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WebServer/examples/FSBrowser/readme.md
*/

#include "editfs.h"

#include "webserver.h"
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <ArduinoLog.h>

extern const char *fsName;
extern FS *fileSystem ;
extern bool fsOK;
extern ESP8266WebServer webserver;

String unsupportedFiles = String();

File uploadFile;

extern const char FS_INIT_ERROR[] PROGMEM;
extern const char FILE_NOT_FOUND[] PROGMEM;

static const char DIR_ARG_MISSING[] PROGMEM = "Directory argument missing";
static const char BAD_PATH[] PROGMEM = "Bad path";
static const char PATH_ARG_MISSING[] PROGMEM = "Path arg missing";
static const char PATH_FILE_EXISTS[] PROGMEM = "Path file exists";
static const char MKDIR_FAILED[] PROGMEM = "MKDIR failed";
static const char CREATE_FAILED[] PROGMEM = "Create failed";
static const char SRC_FILE_NOT_FOUND[] PROGMEM = "Source file not found";
static const char RENAME_FAILED[] PROGMEM = "Rename failed";
static const char WRITE_FAILED[] PROGMEM = "Write failed";

/*
   Return the FS type, status and size info
*/
void handleStatus()
{
    // erial.println("handleStatus");
    FSInfo fs_info;
    String json;
    json.reserve(128);

    json = "{\"type\":\"";
    json += fsName;
    json += "\", \"isOk\":";
    if (fsOK)
    {
        fileSystem->info(fs_info);
        json += F("\"true\", \"totalBytes\":\"");
        json += fs_info.totalBytes;
        json += F("\", \"usedBytes\":\"");
        json += fs_info.usedBytes;
        json += "\"";
    }
    else
    {
        json += "\"false\"";
    }
    json += F(",\"unsupportedFiles\":\"");
    json += unsupportedFiles;
    json += "\"}";

    webserver.send(200, "application/json", json);
}

/*
   Return the list of files in the directory specified by the "dir"
   query string parameter.  Also demonstrates the use of chunked
   responses.
*/
void handleFileList()
{
    if (!fsOK)
    {
        return replyServerError(FPSTR(FS_INIT_ERROR));
    }

    if (!webserver.hasArg("dir"))
    {
        return replyBadRequest(FPSTR(DIR_ARG_MISSING));
    }

    String path = webserver.arg("dir");
    if (path != "/" && !fileSystem->exists(path))
    {
        return replyBadRequest(FPSTR(BAD_PATH));
    }

    Log.verbose(F("handleFileList: %s" LF), path.c_str());
    Dir dir = fileSystem->openDir(path);
    path.clear();

    // Use HTTP/1.1 Chunked response to avoid building a huge temporary string
    if (!webserver.chunkedResponseModeStart(200, "text/json"))
    {
        webserver.send(505, F("text/html"), F("HTTP1.1 required"));
        return;
    }

    // Use the same string for every line
    String output;
    output.reserve(64);
    while (dir.next())
    {
        if (output.length())
        {
            // Send string from previous iteration as an HTTP chunk
            webserver.sendContent(output);
            output = ',';
        }
        else
        {
            output = '[';
        }

        output += "{\"type\":\"";
        if (dir.isDirectory())
        {
            output += "dir";
        }
        else
        {
            output += F("file\",\"size\":\"");
            output += dir.fileSize();
        }

        output += F("\",\"name\":\"");
        // Always return names without leading "/"
        // TODO: Handle an exlusion list
        if (dir.fileName()[0] == '/')
        {
            output += &(dir.fileName()[1]);
        }
        else
        {
            output += dir.fileName();
        }

        output += "\"}";
    }

    // Send last string
    output += "]";
    webserver.sendContent(output);
    webserver.chunkedResponseFinalize();
}

/*
   As some FS (e.g. LittleFS) delete the parent folder when the last child
   has been removed, return the path of the closest parent still existing.
*/
String lastExistingParent(String path)
{
    while (!path.isEmpty() && !fileSystem->exists(path))
    {
        if (path.lastIndexOf('/') > 0)
        {
            path = path.substring(0, path.lastIndexOf('/'));
        }
        else
        {
            path = String(); // No slash => the top folder does not exist
        }
    }
    // erial.println(String("Last existing parent: ") + path);
    return path;
}

/*
   Handle the creation/rename of a new file
   Operation      | req.responseText
   ---------------+--------------------------------------------------------------
   Create file    | parent of created file
   Create folder  | parent of created folder
   Rename file    | parent of source file
   Move file      | parent of source file, or remaining ancestor
   Rename folder  | parent of source folder
   Move folder    | parent of source folder, or remaining ancestor
*/
void handleFileCreate()
{
    if (!fsOK)
    {
        return replyServerError(FPSTR(FS_INIT_ERROR));
    }

    String path = webserver.arg("path");
    if (path.isEmpty())
    {
        return replyBadRequest(FPSTR(PATH_ARG_MISSING));
    }

    if (path == "/")
    {
        return replyBadRequest(FPSTR(BAD_PATH));
    }
    if (fileSystem->exists(path))
    {
        return replyBadRequest(FPSTR(PATH_FILE_EXISTS));
    }

    String src = webserver.arg("src");
    if (src.isEmpty())
    {
        // No source specified: creation
        Log.verbose(F("handleFileCreate: %s" LF), path.c_str());
        if (path.endsWith("/"))
        {
            // Create a folder
            path.remove(path.length() - 1);
            if (!fileSystem->mkdir(path))
            {
                return replyServerError(FPSTR(MKDIR_FAILED));
            }
        }
        else
        {
            // Create a file
            File file = fileSystem->open(path, "w");
            if (file)
            {
                file.write((const char *)0);
                file.close();
            }
            else
            {
                return replyServerError(FPSTR(CREATE_FAILED));
            }
        }
        if (path.lastIndexOf('/') > -1)
        {
            path = path.substring(0, path.lastIndexOf('/'));
        }
        replyOKWithMsg(path);
    }
    else
    {
        // Source specified: rename
        if (src == "/")
        {
            return replyBadRequest("BAD SRC");
        }
        if (!fileSystem->exists(src))
        {
            return replyBadRequest(FPSTR(SRC_FILE_NOT_FOUND));
        }

        Log.verbose(F("handleFileCreate: %s from %s" LF), path.c_str(), src.c_str());


        if (path.endsWith("/"))
        {
            path.remove(path.length() - 1);
        }
        if (src.endsWith("/"))
        {
            src.remove(src.length() - 1);
        }
        if (!fileSystem->rename(src, path))
        {
            return replyServerError(FPSTR(RENAME_FAILED));
        }
        replyOKWithMsg(lastExistingParent(src));
    }
}

/*
   Delete the file or folder designed by the given path.
   If it's a file, delete it.
   If it's a folder, delete all nested contents first then the folder itself

   IMPORTANT NOTE: using recursion is generally not recommended on embedded
   devices and can lead to crashes (stack overflow errors). This use is just
   for demonstration purpose, and FSBrowser might crash in case of deepl
   nested filesystems.  Please don't do this on a production system.
*/
void deleteRecursive(String path)
{
    File file = fileSystem->open(path, "r");
    bool isDir = file.isDirectory();
    file.close();

    // If it's a plain file, delete it
    if (!isDir)
    {
        fileSystem->remove(path);
        return;
    }

    // Otherwise delete its contents first
    Dir dir = fileSystem->openDir(path);

    while (dir.next())
    {
        deleteRecursive(path + '/' + dir.fileName());
    }

    // Then delete the folder itself
    fileSystem->rmdir(path);
}

/*
   Handle a file deletion request
   Operation      | req.responseText
   ---------------+------------------------------------------------
   Delete file    | parent of deleted file, or remaining ancestor
   Delete folder  | parent of deleted folder, or remaining ancestor
*/
void handleFileDelete()
{
    if (!fsOK)
    {
        return replyServerError(FPSTR(FS_INIT_ERROR));
    }

    String path = webserver.arg(0);
    if (path.isEmpty() || path == "/")
    {
        return replyBadRequest(FPSTR(BAD_PATH));
    }

    Log.verbose(F("handleFileDelete: %s" LF), path.c_str());
    if (!fileSystem->exists(path))
    {
        return replyNotFound(FPSTR(FILE_NOT_FOUND));
    }
    deleteRecursive(path);

    replyOKWithMsg(lastExistingParent(path));
}

/*
   Handle a file upload request
*/
void handleFileUpload()
{
    if (!fsOK)
    {
        return replyServerError(FPSTR(FS_INIT_ERROR));
    }
    if (webserver.uri() != "/edit/")
    {
        return;
    }
    HTTPUpload &upload = webserver.upload();
    if (upload.status == UPLOAD_FILE_START)
    {
        String filename = upload.filename;
        // Make sure paths always start with "/"
        if (!filename.startsWith("/"))
        {
            filename = "/" + filename;
        }
        Log.verbose(F("handleFileUpload Name: %s" LF), filename.c_str());
        uploadFile = fileSystem->open(filename, "w");
        if (!uploadFile)
        {
            return replyServerError(FPSTR(CREATE_FAILED));
        }
        Log.verbose(F("Upload: START, filename: %s" LF), filename.c_str());
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
        if (uploadFile)
        {
            size_t bytesWritten = uploadFile.write(upload.buf, upload.currentSize);
            if (bytesWritten != upload.currentSize)
            {
                return replyServerError(FPSTR(WRITE_FAILED));
            }
        }
        Log.verbose(F("Upload: WRITE, Bytes: %d" LF), upload.currentSize);
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
        if (uploadFile)
        {
            uploadFile.close();
        }
        Log.verbose(F("Upload: END, Size: %s" LF), upload.totalSize);
    }
}

/*
   This specific handler returns the index.htm (or a gzipped version)
   from the /edit folder. If the file is not present but the flag
   INCLUDE_FALLBACK_INDEX_HTM has been set, falls back to the version
   embedded in the program code.  Otherwise, fails with a 404 page
   with debug information
*/
void handleGetEdit()
{
    if (handleFileRead(F("/edit.htm")))
    {
        return;
    }
    replyNotFound(FPSTR(FILE_NOT_FOUND));
}

////////////////////////////////
// EDIT SERVER INIT

void editPagesInit()
{
    // Filesystem status
    webserver.on("/status/", HTTP_GET, handleStatus);

    // List directory
    webserver.on("/list/", HTTP_GET, handleFileList);

    // Load editor
    webserver.on("/edit/", HTTP_GET, handleGetEdit);

    // Create file
    webserver.on("/edit/", HTTP_PUT, handleFileCreate);

    // Delete file
    webserver.on("/edit/", HTTP_DELETE, handleFileDelete);

    // Upload file
    // - first callback is called after the request has ended with all parsed arguments
    // - second callback handles file upload at that location
    webserver.on("/edit/", HTTP_POST, replyOK, handleFileUpload);

    Log.notice(F("Open: http://%s.local/edit/ to view the filesystem." LF), WiFi.hostname().c_str());
}
