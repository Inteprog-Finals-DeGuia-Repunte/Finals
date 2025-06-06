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

// RESERVATION CLASS

// Represents a hotel reservation
class Reservation {
public:
    string username;
    string roomType;
    int nights;
    double totalPrice;
    string month;
    Reservation(string user, string type, int n, double price, string m)
        : username(move(user)), roomType(move(type)), nights(n), totalPrice(price), month(move(m)) {}
};


// FILE HANDLING

// CSV file names
const char USERS_FILE[] = "users.csv";
const char RESERVATIONS_FILE[] = "reservations.csv";

// Saves user data to users.csv
void saveUsers(const vector<User>& users) {
    ofstream file(USERS_FILE);
    if (!file.is_open()) {
        cerr << "Error: Could not open " << USERS_FILE << " for writing.\n";
        return;
    }
    for (const auto& user : users) {
        file << user.username << "," << user.password << "," << user.isAdmin << "\n";
    }
    file.close();
}

// Loads user data from users.csv
vector<User> loadUsers() {
    vector<User> users;
    ifstream file(USERS_FILE);
    if (!file.is_open()) return users; // File not found, return empty
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string uname, pwd, adminFlagStr;
        if (getline(ss, uname, ',') && getline(ss, pwd, ',') && getline(ss, adminFlagStr, ',')) {
            if (!uname.empty()) { // Basic check
                users.emplace_back(uname, pwd, (adminFlagStr == "1"));
            }
        }
    }
    file.close();
    return users;
}

// Saves reservation data to reservations.csv
void saveReservations(const vector<Reservation>& reservations) {
    ofstream file(RESERVATIONS_FILE);
    if (!file.is_open()) {
        cerr << "Error: Could not open " << RESERVATIONS_FILE << " for writing.\n";
        return;
    }
    for (const auto& res : reservations) {
        file << res.username << "," << res.roomType << "," << res.nights << ","
             << fixed << setprecision(2) << res.totalPrice << "," << res.month << "\n";
    }
    file.close();
}

// Loads reservation data from reservations.csv
vector<Reservation> loadReservations() {
    vector<Reservation> reservations;
    ifstream file(RESERVATIONS_FILE);
    if (!file.is_open()) return reservations;
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string uname, type, nightsStr, priceStr, month;
        if (getline(ss, uname, ',') && getline(ss, type, ',') &&
            getline(ss, nightsStr, ',') && getline(ss, priceStr, ',') && getline(ss, month, ',')) {
            try {
                if (!uname.empty() && !type.empty() && !nightsStr.empty() && !priceStr.empty() && !month.empty()) {
                    int nights = stoi(nightsStr);
                    double price = stod(priceStr);
                    reservations.emplace_back(uname, type, nights, price, month);
                }
            } catch (const invalid_argument& e) {
                cerr << "Error parsing reservation data: " << e.what() << " in line: \"" << line << "\"\n";
            } catch (const out_of_range& e) {
                cerr << "Error parsing reservation data: " << e.what() << " in line: \"" << line << "\"\n";
            }
        }
    }
    file.close();
    return reservations;
}

// LOGIC FOR RESERVATION

// Checks if a month is peak season (March, April, May, Dec)
bool isPeakSeason(const string& month) {
    static const set<string> peakMonths = {"March", "April", "May", "December"};
    string formattedMonth = month;
    if (!formattedMonth.empty()) {
        formattedMonth[0] = toupper(formattedMonth[0]);
        for (size_t i = 1; i < formattedMonth.length(); ++i) {
            formattedMonth[i] = tolower(formattedMonth[i]);
        }
    }
    return peakMonths.count(formattedMonth);
}

// Guides user to make a new reservation
void makeReservation(vector<Reservation>& reservations, const string& currentUser) {
    printHeader("Make a New Reservation");
    printLine("Heads up: Base rates apply, but expect a 20% surcharge during Peak Season (March, April, May, December).");
    cout << "\nAvailable Room Types:\n";
    cout << "1. Standard (PHP 1000/night)\n";
    cout << "2. Deluxe (PHP 2000/night)\n";
    cout << "3. Suite (PHP 3000/night)\n";
    cout << "Enter room type (Standard, Deluxe, or Suite): ";
    string roomType;
    getline(cin, roomType);

    if (!roomType.empty()) { // Format room type for consistency
        roomType[0] = toupper(roomType[0]);
        for (size_t i = 1; i < roomType.length(); ++i) {
            roomType[i] = tolower(roomType[i]);
        }
    }

    unique_ptr<Room> room;
    try {
        room = createRoom(roomType); // Create room based on input
    } catch (const invalid_argument& e) {
        printLine(string("Oops! ") + e.what());
        pauseScreen();
        return;
    }

    int nights = getIntInput("Enter number of nights: ");
    if (nights <= 0) {
        printLine("Nights must be positive. Reservation cancelled.");
        pauseScreen();
        return;
    }

    cout << "Enter reservation month (e.g., January): ";
    string month;
    getline(cin, month);

    if (!isValidMonth(month)) {
        printLine("Invalid month. Reservation cancelled.");
        pauseScreen();
        return;
    }

    bool isCurrentPeakSeason = isPeakSeason(month);
    if (isCurrentPeakSeason) {
        printLine("Just a reminder: " + month + " is Peak Season, so that 20% surcharge is on!");
    }

    double totalPrice = room->calculatePrice(nights, isCurrentPeakSeason);
    reservations.emplace_back(currentUser, roomType, nights, totalPrice, month);
    saveReservations(reservations);
    printLine("Reservation successful! Total Price: PHP " + to_string(totalPrice));
    pauseScreen();
}

// Shows all reservations for the current user in a table
void viewReservations(const vector<Reservation>& reservations, const string& username) {
    printHeader("Your Reservations");
    bool found = false;
    int reservationCount = 0;

    cout << left << setw(5) << "#"
         << setw(15) << "Room Type"
         << setw(10) << "Nights"
         << setw(12) << "Month"
         << setw(20) << "Total Price (PHP)" << "\n";
    cout << string(62, '-') << "\n"; // Table header divider

    for (const auto& res : reservations) {
        if (res.username == username) {
            found = true;
            reservationCount++;
            cout << left << setw(5) << reservationCount
                 << setw(15) << res.roomType
                 << setw(10) << res.nights
                 << setw(12) << res.month
                 << setw(20) << fixed << setprecision(2) << res.totalPrice << "\n";
        }
    }
    if (!found) {
        printLine("No reservations found for your account.");
    }
    pauseScreen();
}

// Allows user to change an existing reservation
void updateReservation(vector<Reservation>& reservations, const string& username) {
    printHeader("Update Reservation");
    vector<int> userReservationIndices;
    cout << "Your current reservations:\n";
    int displayIndex = 1;

    cout << left << setw(5) << "#"
         << setw(15) << "Room Type"
         << setw(10) << "Nights"
         << setw(12) << "Month"
         << setw(20) << "Total Price (PHP)" << "\n";
    cout << string(62, '-') << "\n"; // Table header divider

    for (size_t i = 0; i < reservations.size(); ++i) {
        if (reservations[i].username == username) {
            userReservationIndices.push_back(i);
            cout << left << setw(5) << displayIndex++
                 << setw(15) << reservations[i].roomType
                 << setw(10) << reservations[i].nights
                 << setw(12) << reservations[i].month
                 << setw(20) << fixed << setprecision(2) << reservations[i].totalPrice << "\n";
        }
    }

    if (userReservationIndices.empty()) {
        printLine("No reservations found to update.");
        pauseScreen();
        return;
    }

    int choice = getIntInput("\nEnter the number of the reservation you want to update: ");
    if (choice < 1 || choice > userReservationIndices.size()) {
        printLine("Invalid reservation number. Update cancelled.");
        pauseScreen();
        return;
    }

    int actualIndex = userReservationIndices[choice - 1];
    Reservation& resToUpdate = reservations[actualIndex]; // Get the actual reservation

    cout << "\n--- Selected Reservation Details ---\n"; // Divider
    cout << "  Room Type: " << resToUpdate.roomType << "\n"
         << "  Number of Nights: " << resToUpdate.nights << "\n"
         << "  Month: " << resToUpdate.month << "\n"
         << "  Total Price: PHP " << fixed << setprecision(2) << resToUpdate.totalPrice << "\n";
    cout << "------------------------------------\n\n"; // Divider

    cout << "Enter new number of nights (0 or Enter to keep current): ";
    string newNightsStr;
    getline(cin, newNightsStr);
    int newNights = resToUpdate.nights; // Default to current

    if (!newNightsStr.empty()) {
        try {
            int enteredNights = stoi(newNightsStr);
            if (enteredNights == 0) {
                printLine("Keeping current number of nights.");
            } else if (enteredNights < 0) {
                printLine("Nights can't be negative. Update cancelled.");
                pauseScreen();
                return;
            } else {
                newNights = enteredNights;
            }
        } catch (const invalid_argument&) {
            printLine("Invalid input for nights. Please enter a number. Update cancelled.");
            pauseScreen();
            return;
        } catch (const out_of_range&) {
            printLine("Nights value out of range. Update cancelled.");
            pauseScreen();
            return;
        }
    } else {
        printLine("Keeping current number of nights.");
    }

    cout << "Enter new reservation month (Enter to keep current): ";
    string newMonth;
    getline(cin, newMonth);

    if (newMonth.empty()) {
        newMonth = resToUpdate.month; // Keep current
        printLine("Keeping current reservation month.");
    } else {
        if (!isValidMonth(newMonth)) {
            printLine("Invalid month. Update cancelled.");
            pauseScreen();
            return;
        }
    }

    // Recalculate price with new details
    unique_ptr<Room> room = createRoom(resToUpdate.roomType);
    resToUpdate.nights = newNights;
    resToUpdate.month = newMonth;
    resToUpdate.totalPrice = room->calculatePrice(newNights, isPeakSeason(newMonth));

    printLine("Reservation updated successfully!");
    saveReservations(reservations);
    pauseScreen();
}

// Allows user to cancel a reservation
void cancelReservation(vector<Reservation>& reservations, const string& username) {
    printHeader("Cancel Reservation");
    vector<int> userReservationIndices;
    cout << "Your current reservations:\n";
    int displayIndex = 1;

    cout << left << setw(5) << "#"
         << setw(15) << "Room Type"
         << setw(10) << "Nights"
         << setw(12) << "Month"
         << setw(20) << "Total Price (PHP)" << "\n";
    cout << string(62, '-') << "\n"; // Table header divider

    for (size_t i = 0; i < reservations.size(); ++i) {
        if (reservations[i].username == username) {
            userReservationIndices.push_back(i);
            cout << left << setw(5) << displayIndex++
                 << setw(15) << reservations[i].roomType
                 << setw(10) << reservations[i].nights
                 << setw(12) << reservations[i].month
                 << setw(20) << fixed << setprecision(2) << reservations[i].totalPrice << "\n";
        }
    }

    if (userReservationIndices.empty()) {
        printLine("No reservations found to cancel.");
        pauseScreen();
        return;
    }

    int choice = getIntInput("\nEnter the number of the reservation you want to cancel: ");

    if (choice < 1 || choice > userReservationIndices.size()) {
        printLine("Invalid reservation number. Cancellation aborted.");
        pauseScreen();
        return;
    }

    int actualIndexToRemove = userReservationIndices[choice - 1]; // Get actual index
    reservations.erase(reservations.begin() + actualIndexToRemove); // Remove it!

    saveReservations(reservations);
    printLine("Reservation cancelled successfully.");
    pauseScreen();
}

// MAIN PROGRAM

int main() {
    vector<User> users = loadUsers();
    vector<Reservation> reservations = loadReservations();

    // Create default admin if no users exist (first run)
    if (users.empty()) {
        users.emplace_back("admin", "admin123", true);
        saveUsers(users);
        printLine("Default admin user created (username: admin, password: admin123).");
        pauseScreen();
    }

    string currentUser;
    bool isLoggedIn = false;
    bool isAdmin = false;

    while (true) {
        if (!isLoggedIn) {
            // Main menu for login/registration
            printHeader("HOTEL SYSTEM - Main Menu");
            printMenuOption(1, "Login");
            printMenuOption(2, "Register");
            printMenuOption(3, "Exit");
            int choice = getIntInput("\nChoice: ");

            if (choice == 1) { // Login
                printHeader("Login");
                cout << "Username: ";
                getline(cin, currentUser);
                string password = getMaskedInput("Password: ");

                auto it = find_if(users.begin(), users.end(), [&](const User& user) {
                    return user.username == currentUser && user.password == password;
                });

                if (it != users.end()) {
                    isLoggedIn = true;
                    isAdmin = it->isAdmin;
                    printLine("Login successful!");
                    pauseScreen();
                } else {
                    printLine("Invalid username or password.");
                    pauseScreen();
                }
            } else if (choice == 2) { // Register
                printHeader("Register New Account");
                string username;
                cout << "Choose username: ";
                getline(cin, username);

                // Check if username exists
                if (any_of(users.begin(), users.end(), [&](const User& u){ return u.username == username; })) {
                    printLine("Username '" + username + "' already taken. Pick another one!");
                    pauseScreen();
                    continue;
                }

                string password = getMaskedInput("Choose password: ");
                users.emplace_back(username, password, false); // New users are regular users
                saveUsers(users);
                printLine("Registration successful. You can log in now.");
                pauseScreen();
            } else if (choice == 3) { // Exit
                printLine("Exiting Hotel System. See ya!");
                break;
            } else {
                printLine("Invalid choice. Try again, human.");
                pauseScreen();
            }
        } else { // User is logged in
            if (!isAdmin) { // Regular User Menu
                printHeader("USER MENU - Logged in as: " + currentUser);
                printMenuOption(1, "Make Reservation");
                printMenuOption(2, "View Reservations");
                printMenuOption(3, "Update Reservation");
                printMenuOption(4, "Cancel Reservation");
                printMenuOption(5, "Logout");
                int userChoice = getIntInput("\nChoice: ");

                if (userChoice == 1) {
                    makeReservation(reservations, currentUser);
                } else if (userChoice == 2) {
                    viewReservations(reservations, currentUser);
                } else if (userChoice == 3) {
                    updateReservation(reservations, currentUser);
                } else if (userChoice == 4) {
                    cancelReservation(reservations, currentUser);
                } else if (userChoice == 5) {
                    isLoggedIn = false;
                    printLine("Logged out.");
                    pauseScreen();
                } else {
                    printLine("Invalid choice. Please try again.");
                    pauseScreen();
                }
            } else { // Admin Menu
                printHeader("ADMIN MENU - Logged in as: " + currentUser);
                printMenuOption(1, "View All Reservations");
                printMenuOption(2, "View All Registered Users");
                printMenuOption(3, "Generate System Usage Summary");
                printMenuOption(4, "Logout");
                int adminChoice = getIntInput("\nChoice: ");

                if (adminChoice == 1) { // View All Reservations
                    printHeader("All Reservations in System");
                    if (reservations.empty()) {
                        printLine("No reservations found in the system.");
                    } else {
                        // Display all reservations in a neat table
                        cout << left << setw(18) << "Username"
                             << setw(15) << "Room Type"
                             << setw(10) << "Nights"
                             << setw(12) << "Month"
                             << setw(15) << "Total Price (PHP)" << "\n";
                        cout << string(70, '-') << "\n";
                        for (const auto& res : reservations) {
                            cout << left << setw(18) << res.username
                                 << setw(15) << res.roomType
                                 << setw(10) << res.nights
                                 << setw(12) << res.month
                                 << setw(15) << fixed << setprecision(2) << res.totalPrice << "\n";
                        }
                    }
                    pauseScreen();
                } else if (adminChoice == 2) { // View All Registered Users
                    printHeader("All Registered Users");
                    if (users.empty()) {
                        printLine("No users registered in the system.");
                    } else {
                        // Display all users in a table
                        cout << left << setw(25) << "Username"
                             << setw(10) << "Admin" << "\n";
                        cout << string(35, '-') << "\n";
                        for (const auto& user : users) {
                            cout << left << setw(25) << user.username
                                 << setw(10) << (user.isAdmin ? "Yes" : "No") << "\n";
                        }
                    }
                    pauseScreen();
                } else if (adminChoice == 3) { // Generate System Usage Summary
                    printHeader("System Usage Summary");
                    cout << "Total Registered Users: " << users.size() << "\n";
                    cout << "Total Reservations Made: " << reservations.size() << "\n";
                    double totalRevenue = 0.0;
                    for (const auto& res : reservations) {
                        totalRevenue += res.totalPrice;
                    }
                    cout << "Total Estimated Revenue: PHP " << fixed << setprecision(2) << totalRevenue << "\n";
                    pauseScreen();
                } else if (adminChoice == 4) {
                    isLoggedIn = false;
                    isAdmin = false;
                    printLine("Logged out.");
                    pauseScreen();
                } else {
                    printLine("Invalid choice. Try again.");
                    pauseScreen();
                }
            }
        }
    }

    return 0;
}