#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <set>

// Platform-specific headers for clear screen and masked input
#ifdef _WIN32
#include <conio.h>
#include <cstdlib>
#define CLEAR_COMMAND "cls"
#else
#include <unistd.h>
#include <termios.h>
#define CLEAR_COMMAND "clear"
#endif

using namespace std;

// Forward declarations so functions can see each other
class Reservation;
void makeReservation(vector<Reservation>& reservations, const string& currentUser);
bool isPeakSeason(const string& month);



// UTILITIES


// Clears the console screen
void clearScreen() {
    system(CLEAR_COMMAND);
}

// Pauses the screen, waits for Enter
void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Gets input while masking it (like for passwords)
string getMaskedInput(const string& prompt) {
    cout << prompt;
    string password;
#ifdef _WIN32
    char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b' && !password.empty()) {
            password.pop_back();
            cout << "\b \b";
        } else if (isprint(static_cast<unsigned char>(ch))) {
            password += ch;
            cout << '*';
        }
    }
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    getline(cin, password);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    cout << endl;
    return password;
}

// Gets a number from user, re-prompts if input is junk
int getIntInput(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail()) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
    }
}

// Checks if a string is a valid month (case-insensitive)
bool isValidMonth(const string& month) {
    static const set<string> validMonths = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };
    string formattedMonth = month;
    if (!formattedMonth.empty()) {
        formattedMonth[0] = toupper(formattedMonth[0]);
        for (size_t i = 1; i < formattedMonth.length(); ++i) {
            formattedMonth[i] = tolower(formattedMonth[i]);
        }
    }
    return validMonths.count(formattedMonth);
}

// Prints a fancy header for screens
void printHeader(const string& title) {
    clearScreen();
    cout << string(50, '=') << "\n";
    cout << "\t\t" << title << "\n";
    cout << string(50, '=') << "\n\n";
}

// Prints a message in brackets
void printLine(const string& content) {
    cout << "[ " << content << " ]\n";
}

// Prints a menu option
void printMenuOption(int option, const string& description) {
    cout << "  " << option << ". " << description << "\n";
}


// ROOM CLASSES


// Base class for all room types
class Room {
public:
    virtual double calculatePrice(int nights, bool isPeakSeason) const = 0;
    virtual string getType() const = 0;
    virtual string getDescription() const = 0;
    virtual ~Room() {} // Important for proper cleanup
};

// Standard Room, basic price logic
class StandardRoom : public Room {
public:
    double calculatePrice(int nights, bool isPeakSeason) const override {
        double basePrice = nights * 1000.0;
        return isPeakSeason ? basePrice * 1.2 : basePrice; // 20% surcharge
    }
    string getType() const override { return "Standard"; }
    string getDescription() const override { return "A Standard Room with basic amenities."; }
};

// Deluxe Room, higher base price
class DeluxeRoom : public Room {
public:
    double calculatePrice(int nights, bool isPeakSeason) const override {
        double basePrice = nights * 2000.0;
        return isPeakSeason ? basePrice * 1.2 : basePrice;
    }
    string getType() const override { return "Deluxe"; }
    string getDescription() const override { return "A Deluxe Room with enhanced comfort."; }
};

// Suite Room, premium price
class SuiteRoom : public Room {
public:
    double calculatePrice(int nights, bool isPeakSeason) const override {
        double basePrice = nights * 3000.0;
        return isPeakSeason ? basePrice * 1.2 : basePrice;
    }
    string getType() const override { return "Suite"; }
    string getDescription() const override { return "A Suite Room with luxury amenities."; }
};

// Creates a Room object based on type string
unique_ptr<Room> createRoom(const string& type) {
    if (type == "Standard") return make_unique<StandardRoom>();
    if (type == "Deluxe") return make_unique<DeluxeRoom>();
    if (type == "Suite") return make_unique<SuiteRoom>();
    throw invalid_argument("Invalid room type selected.");
}



//  USER CLASS


// Represents a user (or admin)
class User {
public:
    string username;
    string password;
    bool isAdmin; // True if admin
    User(string uname, string pwd, bool adminFlag)
        : username(move(uname)), password(move(pwd)), isAdmin(adminFlag) {}
};


