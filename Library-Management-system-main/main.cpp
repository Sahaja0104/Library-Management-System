#include "library.h"
#include <limits>
#include <cctype>
#include <algorithm>

bool isNumericISBN(const string& isbn) {
    return all_of(isbn.begin(), isbn.end(), ::isdigit);
}

int main() {
    Library library;

    string userType, name;
    cout << "Are you a Student or Faculty? ";
    cin >> userType;

    for (auto& c : userType) c = tolower(c);
    if (userType == "student") userType = "Student";
    else if (userType == "faculty") userType = "Faculty";
    else {
        cout << "Invalid user type!\n";
        return 1;
    }

    cout << "Enter your name: ";
    cin >> ws;
    getline(cin, name);

    User* user = library.getUserByName(name);
    if (!user) {
        user = library.createUser(name, userType);
        cout << "New user created with ID: " << user->userID << "\n";
    } else {
        cout << "Welcome back, " << user->name << " (ID: " << user->userID << ")\n";
    }

    int choice;
    while (true) {
        cout << "\n1. Add Book\n2. Borrow Book\n3. Return Book\n4. Check Fine\n5. Pay Fine\n6. Exit\n";
        cout << "Enter your choice: ";
        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 1) {
            string title, author, publisher, ISBN;
            int copies;
            cout << "Enter Title: "; getline(cin, title);
            cout << "Enter Author: "; getline(cin, author);
            cout << "Enter Publisher: "; getline(cin, publisher);
            cout << "Enter ISBN: "; getline(cin, ISBN);
            if (!isNumericISBN(ISBN)) {
                cout << "Invalid ISBN! Only digits allowed.\n";
                continue;
            }
            cout << "Enter number of copies: ";
            cin >> copies; cin.ignore();

            library.addBook(title, author, publisher, ISBN, copies);
        }
        else if (choice == 2) {
            string ISBN;
            cout << "ISBN to borrow: ";
            getline(cin, ISBN);
            if (!isNumericISBN(ISBN)) {
                cout << "Invalid ISBN! Only digits allowed.\n";
                continue;
            }
            library.borrowBook(ISBN, user);
            logTransaction(user->name + " borrowed book ISBN: " + ISBN);
        }
        else if (choice == 3) {
            string ISBN;
            cout << "ISBN to return: ";
            getline(cin, ISBN);
            if (!isNumericISBN(ISBN)) {
                cout << "Invalid ISBN! Only digits allowed.\n";
                continue;
            }
            library.returnBook(ISBN, user);
            logTransaction(user->name + " returned book ISBN: " + ISBN);
        }
        else if (choice == 4) {
            cout << "Outstanding fine: Rs. " << user->account.checkFine() << "\n";
        }
        else if (choice == 5) {
            int fine = user->account.checkFine();
            if (fine == 0) {
                cout << "No outstanding fine.\n";
            } else {
                cout << "Paying fine Rs. " << fine << ". Confirm? (y/n): ";
                string confirm;
                cin >> confirm; cin.ignore();
                if (confirm == "y" || confirm == "Y") {
                    user->account.payFine();
                    cout << "Fine paid.\n";
                    logTransaction(user->name + " paid fine Rs. " + to_string(fine));
                }
            }
        }
        else if (choice == 6) {
            cout << "Exiting...\n";
            break;
        }
        else {
            cout << "Invalid choice!\n";
        }
    }

    library.saveUsersToFile();
    library.saveBooksToFile();

    return 0;
}
