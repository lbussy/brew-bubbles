<?php

// Copyright (C) 2019 Lee C. Bussy (@LBussy)

// This file is part of Lee Bussy's Brew Bubbles (Brew-Bubbles).

// Brew Bubbles is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.

// Brew Bubbles is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with Brew Bubbles. If not, see <https://www.gnu.org/licenses/>.

// This demonstrates a minimal endoint for Brew Bubbles, saving the
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
