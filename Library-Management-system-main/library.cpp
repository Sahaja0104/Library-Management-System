#include "library.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstring>
#include <algorithm>
#include <iostream>

// --------- Book -----------

Book::Book(string t, string a, string p, string i, int copies)
    : title(t), author(a), publisher(p), ISBN(i), totalCopies(copies), availableCopies(copies) {}

// --------- Account -----------

void Account::borrowBook(const string& ISBN) {
    borrowedBooks.push_back(ISBN);
}

void Account::returnBook(const string& ISBN) {
    borrowedBooks.erase(remove(borrowedBooks.begin(), borrowedBooks.end(), ISBN), borrowedBooks.end());
}

bool Account::hasBorrowed(const string& ISBN) {
    return find(borrowedBooks.begin(), borrowedBooks.end(), ISBN) != borrowedBooks.end();
}

int Account::checkFine() {
    // For simplicity, return stored fine only
    return fine;
}

void Account::payFine() {
    fine = 0;
}

// --------- User (Student) -----------

Student::Student(int id, string n) : User(id, n, "Student") {}

void Student::borrowBook(const string& ISBN, Book& book) {
    if (book.availableCopies <= 0) {
        std::cout << "All copies already borrowed!\n";
        return;
    }
    if (account.hasBorrowed(ISBN)) {
        std::cout << "You have already borrowed this book.\n";
        return;
    }
    book.availableCopies--;
    account.borrowBook(ISBN);
    std::cout << "Book borrowed successfully for 14 days.\n";
}

void Student::returnBook(const string& ISBN, Book& book) {
    if (!account.hasBorrowed(ISBN)) {
        std::cout << "You did not borrow this book.\n";
        return;
    }
    book.availableCopies++;
    account.returnBook(ISBN);
    std::cout << "Book returned successfully.\n";
}

// --------- User (Faculty) -----------

Faculty::Faculty(int id, string n) : User(id, n, "Faculty") {}

void Faculty::borrowBook(const string& ISBN, Book& book) {
    if (book.availableCopies <= 0) {
        std::cout << "All copies already borrowed!\n";
        return;
    }
    if (account.hasBorrowed(ISBN)) {
        std::cout << "You have already borrowed this book.\n";
        return;
    }
    book.availableCopies--;
    account.borrowBook(ISBN);
    std::cout << "Book borrowed successfully for 28 days.\n";
}

void Faculty::returnBook(const string& ISBN, Book& book) {
    if (!account.hasBorrowed(ISBN)) {
        std::cout << "You did not borrow this book.\n";
        return;
    }
    book.availableCopies++;
    account.returnBook(ISBN);
    std::cout << "Book returned successfully.\n";
}

// --------- Library -----------

Library::Library() : nextUserID(1) {
    loadBooksFromFile();
    loadUsersFromFile();
}

Library::~Library() {
    for (auto& pair : usersByID) {
        delete pair.second;
    }
}

Book* Library::findBook(const string& ISBN) {
    for (auto& book : books) {
        if (book.ISBN == ISBN) return &book;
    }
    return nullptr;
}

void Library::addBook(const string& title, const string& author, const string& publisher, const string& ISBN, int copies) {
    Book* book = findBook(ISBN);
    if (book) {
        book->totalCopies += copies;
        book->availableCopies += copies;
        std::cout << "Added " << copies << " more copies of existing book.\n";
    } else {
        books.push_back(Book(title, author, publisher, ISBN, copies));
        std::cout << "Book added successfully.\n";
    }
    saveBooksToFile();
}

void Library::borrowBook(const string& ISBN, User* user) {
    Book* book = findBook(ISBN);
    if (!book) {
        std::cout << "Book not found.\n";
        return;
    }
    user->borrowBook(ISBN, *book);
    saveBooksToFile();
}

void Library::returnBook(const string& ISBN, User* user) {
    Book* book = findBook(ISBN);
    if (!book) {
        std::cout << "Book not found.\n";
        return;
    }
    user->returnBook(ISBN, *book);
    saveBooksToFile();
}

User* Library::getUserByName(const string& name) {
    if (usersByName.count(name)) {
        return usersByName[name];
    }
    return nullptr;
}

User* Library::createUser(const string& name, const string& role) {
    User* user = nullptr;
    int id = nextUserID++;
    if (role == "Student") {
        user = new Student(id, name);
    } else if (role == "Faculty") {
        user = new Faculty(id, name);
    }
    if (user) {
        usersByID[id] = user;
        usersByName[name] = user;
    }
    return user;
}

void Library::loadBooksFromFile() {
    books.clear();
    std::ifstream file("books.txt");
    if (!file.is_open()) {
        std::cerr << "Could not open books.txt\n";
        return;
    }

    string line;
    while (getline(file, line)) {
        std::stringstream ss(line);
        string title, author, publisher, ISBN, totalStr, availableStr;

        if (!getline(ss, title, ',') ||
            !getline(ss, author, ',') ||
            !getline(ss, publisher, ',') ||
            !getline(ss, ISBN, ',') ||
            !getline(ss, totalStr, ',') ||
            !getline(ss, availableStr)) {
            std::cerr << "⚠️ Skipping malformed line in books.txt: " << line << std::endl;
            continue;
        }

        try {
            int total = std::stoi(totalStr);
            int available = std::stoi(availableStr);
            books.push_back(Book(title, author, publisher, ISBN, total));
            books.back().availableCopies = available;
        } catch (const std::exception& e) {
            std::cerr << "⚠️ Error converting numbers in line: " << line << " (" << e.what() << ")" << std::endl;
            continue;
        }
    }
    file.close();
}

void Library::saveBooksToFile() {
    std::ofstream file("books.txt");
    for (auto& book : books) {
        file << book.title << ","
             << book.author << ","
             << book.publisher << ","
             << book.ISBN << ","
             << book.totalCopies << ","
             << book.availableCopies << "\n";
    }
    file.close();
}

void Library::loadUsersFromFile() {
    usersByID.clear();
    usersByName.clear();
    ifstream file("users.txt");
    if (!file) return;

    string line;
    int maxID = 0;

    // Skip header line if present
    if (getline(file, line)) {
        // Check if the first line contains "UserID" or non-numeric ID
        if (line.find("UserID") != string::npos) {
            // It's a header, so skip processing it
        } else {
            // If no header, process first line as data
            stringstream ss(line);
            string idStr, name, role, fineStr;

            getline(ss, idStr, ',');
            getline(ss, name, ',');
            getline(ss, role, ',');
            getline(ss, fineStr);

            try {
                int id = stoi(idStr);
                maxID = max(maxID, id);

                User* user = nullptr;
                if (role == "Student") user = new Student(id, name);
                else if (role == "Faculty") user = new Faculty(id, name);

                if (user) {
                    user->account.fine = stoi(fineStr);
                    usersByID[id] = user;
                    usersByName[name] = user;
                }
            } catch (...) {
                cerr << "Skipping invalid user line: " << line << endl;
            }
        }
    }

    // Now read rest of the lines
    while (getline(file, line)) {
        stringstream ss(line);
        string idStr, name, role, fineStr;

        getline(ss, idStr, ',');
        getline(ss, name, ',');
        getline(ss, role, ',');
        getline(ss, fineStr);

        try {
            int id = stoi(idStr);
            maxID = max(maxID, id);

            User* user = nullptr;
            if (role == "Student") user = new Student(id, name);
            else if (role == "Faculty") user = new Faculty(id, name);

            if (user) {
                user->account.fine = stoi(fineStr);
                usersByID[id] = user;
                usersByName[name] = user;
            }
        } catch (...) {
            cerr << "Skipping invalid user line: " << line << endl;
        }
    }
    file.close();

    nextUserID = maxID + 1;
}

// void Library::loadUsersFromFile() {
//     usersByID.clear();
//     usersByName.clear();
//     std::ifstream file("users.txt");
//     if (!file.is_open()) return;

//     string line;
//     int maxID = 0;
//     while (getline(file, line)) {
//         std::stringstream ss(line);
//         string idStr, name, role, fineStr;

//         if (!getline(ss, idStr, ',') ||
//             !getline(ss, name, ',') ||
//             !getline(ss, role, ',') ||
//             !getline(ss, fineStr)) {
//             std::cerr << "⚠️ Skipping malformed line in users.txt: " << line << std::endl;
//             continue;
//         }

//         int id = 0;
//         try {
//             id = std::stoi(idStr);
//         } catch (...) {
//             std::cerr << "⚠️ Invalid user ID: " << idStr << std::endl;
//             continue;
//         }

//         maxID = std::max(maxID, id);

//         User* user = nullptr;
//         if (role == "Student") user = new Student(id, name);
//         else if (role == "Faculty") user = new Faculty(id, name);
//         else {
//             std::cerr << "⚠️ Unknown role: " << role << std::endl;
//             continue;
//         }

//         try {
//             user->account.fine = std::stoi(fineStr);
//         } catch (...) {
//             user->account.fine = 0;
//         }

//         usersByID[id] = user;
//         usersByName[name] = user;
//     }
//     file.close();

//     nextUserID = maxID + 1;
// }

void Library::saveUsersToFile() {
    std::ofstream file("users.txt");
    for (auto& pair : usersByID) {
        User* user = pair.second;
        file << user->userID << "," << user->name << "," << user->role << "," << user->account.fine << "\n";
    }
    file.close();
}

// --------- Transaction log -----------

void logTransaction(const std::string& entry) {
    std::ofstream ofs("transactions.txt", std::ios::app);
    time_t now = time(0);
    char* dt = const_cast<char*>(ctime(&now));
    size_t len = strlen(dt);
    if (len > 0) dt[len - 1] = '\0'; // remove newline
    ofs << "[" << dt << "] " << entry << "\n";
    ofs.close();
}
