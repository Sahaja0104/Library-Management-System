#ifndef LIBRARY_H
#define LIBRARY_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class Book {
public:
    string title, author, publisher, ISBN;
    int totalCopies;
    int availableCopies;

    Book() : totalCopies(0), availableCopies(0) {}
    Book(string t, string a, string p, string i, int copies);
};

class Account {
public:
    int fine;
    vector<string> borrowedBooks;

    Account() : fine(0) {}

    void borrowBook(const string& ISBN);
    void returnBook(const string& ISBN);
    bool hasBorrowed(const string& ISBN);
    int checkFine();
    void payFine();
};

class User {
public:
    int userID;
    string name, role;
    Account account;

    User(int id, const string& n, const string& r) : userID(id), name(n), role(r) {}

    virtual void borrowBook(const string& ISBN, Book& book) = 0;
    virtual void returnBook(const string& ISBN, Book& book) = 0;
    virtual ~User() {}
};

class Student : public User {
public:
    Student(int id, string n);
    void borrowBook(const string& ISBN, Book& book) override;
    void returnBook(const string& ISBN, Book& book) override;
};

class Faculty : public User {
public:
    Faculty(int id, string n);
    void borrowBook(const string& ISBN, Book& book) override;
    void returnBook(const string& ISBN, Book& book) override;
};

class Library {
public:
    vector<Book> books;

    unordered_map<int, User*> usersByID;       // userID -> User*
    unordered_map<string, User*> usersByName;  // name -> User*
    int nextUserID;

    Library();
    ~Library();

    Book* findBook(const string& ISBN);
    void addBook(const string& title, const string& author, const string& publisher, const string& ISBN, int copies);
    void borrowBook(const string& ISBN, User* user);
    void returnBook(const string& ISBN, User* user);

    User* getUserByName(const string& name);
    User* createUser(const string& name, const string& role);

    void loadBooksFromFile();
    void saveBooksToFile();
    void loadUsersFromFile();
    void saveUsersToFile();
};

// Utility function to log transactions
void logTransaction(const std::string& entry);

#endif
