#pWordCount

## Project Overview
'pWordCount' is a pipe-based word count tool designed for Linux enviroments.
It uses parent-child process communication through Unix pipes to count words.

## Features
-Reads a file and sends content to a child process via a pipe.
-Child process counts words and returns the result to the parent.
-Handles large file efficiently.
-Uses Makefiles for easy compilation.
-Support multiple branches for additional features.

## Installation
Clone the repository and compile using 'make':
```sh
git clone https://github.com/GideonPy/pwordcount.git
cd pwordount
make

##Usage
Run the program with a file
./pwordcount input.txt

-Output
Parent: Reading file and sending data to child...
Child: Receiving file content and counting words...
Parent: Total word count = 5

##Development
'sh
git checkout -b new-feature

'sh
git add .
git commit -m "Added new feature"
git push origin new-feature
