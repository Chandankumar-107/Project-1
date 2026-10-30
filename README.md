Library Management System (C Language)

A simple console-based program to manage books in a library. It is made using the C programming language as my 1st semester project.

What does this project do?

It helps a librarian to:

Add new books
See the list of all books
Search a book by title or author
Delete a book
Issue a book to a student
Take a book back (and calculate the fine if it is late)
See which books are currently issued

All the data is saved in files, so it does not get lost when the program is closed.

Technologies Used
Thing	Details
Language	C
Compiler	GCC (any C compiler works)
Header files	stdio.h, stdlib.h, string.h, ctype.h, time.h
Data storage	Binary files (books.dat and issues.dat)
Interface	Command line (text menu)
C Concepts Used

These are the topics from our syllabus that I used in this project:

Structures (struct): to store book details and issue details together
Arrays: to keep many books and issue records in memory
Functions: each feature (add, search, delete, etc.) has its own function
Loops and switch-case: to run the menu again and again
File handling: fopen, fread, fwrite, fclose to save and load data
Strings: strcmp, strlen, fgets for names and titles
Time functions: time() and difftime() to find how many days a book was kept
How to Run
Make sure GCC is installed. To check, type this in the terminal:
   gcc --version
Open the terminal in the folder where library.c is saved.
Compile the program:
   gcc library.c -o library
Run it:
Linux / Mac: ./library
Windows: library.exe
How to Use

When you run the program, you will see this menu:

========== LIBRARY MANAGEMENT SYSTEM ==========
1. Add Book
2. Display All Books
3. Search Book
4. Delete Book
5. Issue Book
6. Return Book
7. View Issued Books
0. Exit

Type the number of the option you want and press Enter.

Example: To add a book, type 1, then enter the Book ID, title, author and number of copies.

Rules in the Program
Every book must have a unique ID.
A student can keep a book for 14 days without any fine.
After 14 days, the fine is Rs. 2 per extra day.
A book cannot be deleted if some of its copies are issued.
The program can store up to 500 books and 1000 issue records.
Files in this Project
File	What it is
library.c	The main source code
README.md	This file
books.dat	Created automatically. Stores all books
issues.dat	Created automatically. Stores issue records
Future Improvements

Things I can add later:

Student registration with student ID
Admin login with password
Sorting books by title
Limit on how many books one student can take
Author

Name: Chandan Kumar , 26BCA10099.
