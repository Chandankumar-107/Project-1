/*
 * Library Management System
 * -------------------------
 * Features:
 *   1. Add a book
 *   2. Display all books
 *   3. Search books (by title or author)
 *   4. Delete a book
 *   5. Issue a book
 *   6. Return a book (with late fine calculation)
 *   7. View issued books
 *
 * Data is saved in binary files (books.dat, issues.dat) so it persists
 * between runs.
 *
 * Compile:  gcc library.c -o library
 * Run:      ./library        (Windows: library.exe)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_BOOKS    500
#define MAX_ISSUES   1000
#define TITLE_LEN    100
#define AUTHOR_LEN   60
#define NAME_LEN     60
#define BOOK_FILE    "books.dat"
#define ISSUE_FILE   "issues.dat"
#define ISSUE_DAYS   14   /* days allowed before fine starts */
#define FINE_PER_DAY 2    /* fine in rupees per extra day    */

typedef struct {
    int  id;
    char title[TITLE_LEN];
    char author[AUTHOR_LEN];
    int  quantity;    /* total copies     */
    int  available;   /* copies on shelf  */
} Book;

typedef struct {
    int    bookId;
    char   student[NAME_LEN];
    time_t issueDate;
    int    returned;  /* 0 = still issued, 1 = returned */
} Issue;

Book  books[MAX_BOOKS];
Issue issues[MAX_ISSUES];
int   bookCount  = 0;
int   issueCount = 0;

/* ---------- input helpers ---------- */

void readLine(const char *prompt, char *buf, int size) {
    printf("%s", prompt);
    if (fgets(buf, size, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    buf[strcspn(buf, "\n")] = '\0';
}

int readInt(const char *prompt) {
    char line[50];
    int value;
    while (1) {
        printf("%s", prompt);
        if (fgets(line, sizeof(line), stdin) == NULL) return 0;
        if (sscanf(line, "%d", &value) == 1) return value;
        printf("Invalid input. Please enter a number.\n");
    }
}

/* case-insensitive substring search */
int containsIgnoreCase(const char *text, const char *pattern) {
    int n = strlen(text), m = strlen(pattern);
    if (m == 0) return 1;
    for (int i = 0; i + m <= n; i++) {
        int j;
        for (j = 0; j < m; j++) {
            if (tolower((unsigned char)text[i + j]) !=
                tolower((unsigned char)pattern[j]))
                break;
        }
        if (j == m) return 1;
    }
    return 0;
}

/* ---------- file handling ---------- */

void loadData(void) {
    FILE *fp = fopen(BOOK_FILE, "rb");
    if (fp) {
        fread(&bookCount, sizeof(int), 1, fp);
        if (bookCount < 0 || bookCount > MAX_BOOKS) bookCount = 0;
        fread(books, sizeof(Book), bookCount, fp);
        fclose(fp);
    }
    fp = fopen(ISSUE_FILE, "rb");
    if (fp) {
        fread(&issueCount, sizeof(int), 1, fp);
        if (issueCount < 0 || issueCount > MAX_ISSUES) issueCount = 0;
        fread(issues, sizeof(Issue), issueCount, fp);
        fclose(fp);
    }
}

void saveData(void) {
    FILE *fp = fopen(BOOK_FILE, "wb");
    if (!fp) {
        printf("Error: could not save books.\n");
    } else {
        fwrite(&bookCount, sizeof(int), 1, fp);
        fwrite(books, sizeof(Book), bookCount, fp);
        fclose(fp);
    }
    fp = fopen(ISSUE_FILE, "wb");
    if (!fp) {
        printf("Error: could not save issue records.\n");
    } else {
        fwrite(&issueCount, sizeof(int), 1, fp);
        fwrite(issues, sizeof(Issue), issueCount, fp);
        fclose(fp);
    }
}

/* ---------- lookup ---------- */

int findBookIndex(int id) {
    for (int i = 0; i < bookCount; i++)
        if (books[i].id == id) return i;
    return -1;
}

/* ---------- features ---------- */

void addBook(void) {
    if (bookCount >= MAX_BOOKS) {
        printf("Library is full!\n");
        return;
    }
    Book b;
    b.id = readInt("Enter Book ID: ");
    if (findBookIndex(b.id) != -1) {
        printf("A book with this ID already exists.\n");
        return;
    }
    readLine("Enter Title: ", b.title, TITLE_LEN);
    readLine("Enter Author: ", b.author, AUTHOR_LEN);
    b.quantity = readInt("Enter Number of Copies: ");
    if (b.quantity <= 0) {
        printf("Copies must be at least 1.\n");
        return;
    }
    b.available = b.quantity;

    books[bookCount++] = b;
    saveData();
    printf("Book added successfully!\n");
}

void displayBooks(void) {
    if (bookCount == 0) {
        printf("No books in the library yet.\n");
        return;
    }
    printf("\n%-6s %-30s %-20s %-6s %-9s\n",
           "ID", "Title", "Author", "Total", "Available");
    printf("-----------------------------------------------------------------------\n");
    for (int i = 0; i < bookCount; i++) {
        printf("%-6d %-30.30s %-20.20s %-6d %-9d\n",
               books[i].id, books[i].title, books[i].author,
               books[i].quantity, books[i].available);
    }
}

void searchBook(void) {
    char keyword[TITLE_LEN];
    readLine("Enter title or author to search: ", keyword, TITLE_LEN);

    int found = 0;
    for (int i = 0; i < bookCount; i++) {
        if (containsIgnoreCase(books[i].title, keyword) ||
            containsIgnoreCase(books[i].author, keyword)) {
            if (!found) {
                printf("\n%-6s %-30s %-20s %-9s\n",
                       "ID", "Title", "Author", "Available");
                printf("--------------------------------------------------------------\n");
            }
            printf("%-6d %-30.30s %-20.20s %-9d\n",
                   books[i].id, books[i].title, books[i].author,
                   books[i].available);
            found = 1;
        }
    }
    if (!found) printf("No matching books found.\n");
}

void deleteBook(void) {
    int id = readInt("Enter Book ID to delete: ");
    int idx = findBookIndex(id);
    if (idx == -1) {
        printf("Book not found.\n");
        return;
    }
    if (books[idx].available != books[idx].quantity) {
        printf("Cannot delete: some copies are currently issued.\n");
        return;
    }
    for (int i = idx; i < bookCount - 1; i++)
        books[i] = books[i + 1];
    bookCount--;
    saveData();
    printf("Book deleted.\n");
}

void issueBook(void) {
    if (issueCount >= MAX_ISSUES) {
        printf("Issue record is full!\n");
        return;
    }
    int id = readInt("Enter Book ID to issue: ");
    int idx = findBookIndex(id);
    if (idx == -1) {
        printf("Book not found.\n");
        return;
    }
    if (books[idx].available == 0) {
        printf("Sorry, no copies available right now.\n");
        return;
    }
    Issue is;
    is.bookId = id;
    readLine("Enter Student Name: ", is.student, NAME_LEN);
    if (strlen(is.student) == 0) {
        printf("Name cannot be empty.\n");
        return;
    }
    is.issueDate = time(NULL);
    is.returned = 0;

    issues[issueCount++] = is;
    books[idx].available--;
    saveData();
    printf("Book issued to %s. Please return within %d days.\n",
           is.student, ISSUE_DAYS);
}

void returnBook(void) {
    int id = readInt("Enter Book ID to return: ");
    char name[NAME_LEN];
    readLine("Enter Student Name: ", name, NAME_LEN);

    for (int i = 0; i < issueCount; i++) {
        if (issues[i].bookId == id && !issues[i].returned &&
            strcmp(issues[i].student, name) == 0) {

            int days = (int)(difftime(time(NULL), issues[i].issueDate) / 86400);
            int fine = 0;
            if (days > ISSUE_DAYS)
                fine = (days - ISSUE_DAYS) * FINE_PER_DAY;

            issues[i].returned = 1;
            int idx = findBookIndex(id);
            if (idx != -1) books[idx].available++;
            saveData();

            printf("Book returned after %d day(s).\n", days);
            if (fine > 0)
                printf("Late fine: Rs. %d\n", fine);
            else
                printf("No fine. Thank you!\n");
            return;
        }
    }
    printf("No matching issue record found.\n");
}

void viewIssued(void) {
    int found = 0;
    for (int i = 0; i < issueCount; i++) {
        if (!issues[i].returned) {
            if (!found) {
                printf("\n%-6s %-30s %-20s %-12s\n",
                       "ID", "Title", "Student", "Issued On");
                printf("---------------------------------------------------------------------\n");
            }
            int idx = findBookIndex(issues[i].bookId);
            char date[20];
            strftime(date, sizeof(date), "%d-%m-%Y", localtime(&issues[i].issueDate));
            printf("%-6d %-30.30s %-20.20s %-12s\n",
                   issues[i].bookId,
                   idx != -1 ? books[idx].title : "(deleted)",
                   issues[i].student, date);
            found = 1;
        }
    }
    if (!found) printf("No books are currently issued.\n");
}

/* ---------- main ---------- */

int main(void) {
    loadData();
    int choice;

    do {
        printf("\n========== LIBRARY MANAGEMENT SYSTEM ==========\n");
        printf("1. Add Book\n");
        printf("2. Display All Books\n");
        printf("3. Search Book\n");
        printf("4. Delete Book\n");
        printf("5. Issue Book\n");
        printf("6. Return Book\n");
        printf("7. View Issued Books\n");
        printf("0. Exit\n");
        choice = readInt("Enter your choice: ");

        switch (choice) {
            case 1: addBook();      break;
            case 2: displayBooks(); break;
            case 3: searchBook();   break;
            case 4: deleteBook();   break;
            case 5: issueBook();    break;
            case 6: returnBook();   break;
            case 7: viewIssued();   break;
            case 0: printf("Goodbye!\n"); break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 0);

    return 0;
}