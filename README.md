Hotel Reservation System
This is a console-based Hotel Reservation System built with C++. It allows users to register, log in, make, view, update, and cancel hotel reservations. Admins have additional functionalities to view all reservations, manage users, and generate system usage summaries.

Features:
  User Authentication: Secure user registration and login system.
  Role-Based Access: Differentiates between regular users and administrators with distinct functionalities.
  Room Types: Offers Standard, Deluxe, and Suite rooms with varying base prices.
  Peak Season Surcharge: Automatically applies a 20% surcharge during peak months (March, April, May, December).
  Reservation Management:
    Make Reservation: Book new rooms for a specified number of nights and month.
    View Reservations: See a detailed list of your current bookings.
    Update Reservation: Modify the number of nights or month for an existing reservation.
    Cancel Reservation: Remove a reservation from the system.
  Admin Panel:
    View All Reservations: Oversee all bookings made in the system.
    View All Registered Users: See a list of all accounts.
    Generate System Usage Summary: Get an overview of total users, reservations, and estimated revenue.
  Data Persistence: User and reservation data are saved to users.csv and reservations.csv files, ensuring data is not lost when the program closes.
  Input Validation: Robust handling of user input to prevent errors.
  Cross-Platform Compatibility: Includes platform-specific headers for clear screen and masked input.

How to Run
To compile and run this program, you'll need a C++ compiler.

Save the Code: Save the provided C++ code into a file named main.cpp.

Compile: Open your terminal or command prompt, navigate to the directory where you saved main.cpp, and compile the code using the following command:

Bash

g++ main.cpp -o hotel_system -std=c++11
g++: The GNU C++ compiler.
main.cpp: Your source code file.
-o hotel_system: Specifies the output executable file name. You can choose any name you like (e.g., hotel_system.exe on Windows).
-std=c++11: Ensures compilation with C++11 standard features (like unique_ptr and emplace_back).
Run: Execute the compiled program:

On Windows:
Bash

.\hotel_system.exe
On Linux/macOS:
Bash

./hotel_system
The system will automatically create users.csv and reservations.csv files if they don't exist. A default admin account (username: admin, password: admin123) will be created upon the first run if no users are found.

Team Members
Ernest De Guia
Mark Repunte
