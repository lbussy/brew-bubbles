<?php

// Copyright (C) 2019 Lee C. Bussy (@LBussy)

// This file is part of Lee Bussy's Brew Bubbles (brew-bubbles).

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// This demonstrates a minimal endpoint for Brew Bubbles, saving the
// json to a local file.  The argument on file_put_contents defines
// it's behavior:
//      FILE_USE_INCLUDE_PATH -     Will check the include path for
//                                  existence of the target file
//      FILE_APPEND -               Appends rather than overwrites
//                                  the target file
//      LOCK_EX -                   Locks the file exclusively during
//                                  the process

$file = "test.txt";
$args = "LOCK_EX | FILE_APPEND"; // Separate multiples with pipe
$json = file_get_contents('php://input');
$json .= "\n"; // Add a line return for easy reviewing

//Open the File Stream
$handle = fopen($file, "a");

//Lock File, error if unable to lock
if(flock($handle, LOCK_EX)) {
    fwrite($handle, $json);     //Write the $data into file
    flock($handle, LOCK_UN);    //Unlock File
    // 200 = Ok
    header('X-PHP-Response-Code: 200', true, 200);
} else {
    // 500 = Internal Server Error
    header('X-PHP-Response-Code: 500', true, 500);
}
//Close Stream

?>
