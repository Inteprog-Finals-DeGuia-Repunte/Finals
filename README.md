# Enhanced Hotel Reservation System  

A console-based **Hotel Reservation System** built with C++. This program offers a user-friendly experience for customers to manage their hotel reservations and provides administrators with powerful tools to oversee and maintain the system.  

---

## Features  

### **General Features**  
- **User Authentication**: Secure registration and login system.  
- **Role-Based Access Control**:  
  - **Users**: Book, view, update, and cancel reservations.  
  - **Admins**: Manage user accounts, view all reservations, and generate usage summaries.  
- **Room Options**: Choose between **Standard**, **Deluxe**, and **Suite** rooms with unique pricing.  
- **Peak Season Surcharge**: Automatically applies a 20% surcharge during peak months (**March, April, May, December**).  
- **Data Persistence**: User and reservation data are securely stored in `users.csv` and `reservations.csv`.  

### **User Functionality**  
- **Make a Reservation**: Book rooms for specific months and nights.  
- **View Reservations**: Review all active bookings with detailed information.  
- **Update Reservation**: Modify the number of nights or the reservation month.  
- **Cancel Reservation**: Remove a reservation from the system.  

### **Admin Panel**  
- **View All Reservations**: Access a complete list of all bookings.  
- **Manage Users**: View all registered accounts in the system.  
- **Generate Usage Summaries**: Get detailed metrics such as the total number of users, reservations, and estimated revenue.  

### **Technical Highlights**  
- **Input Validation**: Ensures robust and error-free user interactions.  
- **Cross-Platform Compatibility**: Includes platform-specific support for clearing the screen and masking input.  
- **Default Admin Account**: Automatically created on the first run if no users are found (**username**: `admin`, **password**: `admin123`).  

---

## How to Run  

### Prerequisites  
- A C++ compiler (e.g., dev c++).  

### Steps to Run  

1. **Save the Code**:  
   Save the C++ source code into a file named `main.cpp`.  

2. **Compile and Run**:  
   Press F6 to compile and run

3. **Initial Setup**:
   - The system will automatically create users.csv and reservations.csv if they don't exist.
   - A default admin account is created on the first run if no users are present.

## Team Members
Ernest De Guia
Mark Repunte
