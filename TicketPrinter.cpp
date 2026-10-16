#include "TicketPrinter.h"
#include "Customer.h"
#include "Show.h"
#include "Logger.h"
#include <iostream>
#include <unistd.h>     // OS Concept: fork(), getpid()
#include <sys/wait.h>   // OS Concept: waitpid()

using namespace std;

void TicketPrinter::printTicket(Customer *customer, vector<Booking>& systemBookings) {
    // OS Concept: Forking a child process to handle ticket rendering/printing
    // This isolates the printing workload from the main application thread.
    pid_t pid = fork();

    if (pid == 0) {
        // ── Child Process ────────────────────────────────────────────────
        bool hasTickets = false;
        for (auto& booking : systemBookings) {
            if (booking.getCustomerId() == customer->getCustId()) {
                hasTickets = true;
                
                cout << "\n[Child Process PID " << getpid() << "] ================== TICKET ==================" << endl;
                cout << "Booking ID : BK" << booking.getBookingId() << endl; 
                cout << "Movie      : " << booking.getShow()->getMovieName() << endl;
                cout << "Screen     : Screen-" << booking.getShow()->getScreenId() 
                     << "    " << booking.getShow()->getStartTime() << ":00" << endl;
                
                cout << "Seats      : ";
                vector<int> seats = booking.getSeatIds();
                for (size_t i = 0; i < seats.size(); i++) {
                    cout << seats[i];
                    if (i != seats.size() - 1) {
                        cout << ", ";
                    }
                }
                cout << endl;

                cout << "Amount     : Rs." << booking.getTotalAmount() 
                     << "        Status: " << (booking.getBookingStatus() ? "CONFIRMED" : "CANCELLED") << endl;
                     
                cout << "===========================================================" << endl << endl;
            }
        }

        if (!hasTickets) {
            cout << "\n[Child Process PID " << getpid() << "] No booking history found for this account." << endl;
        }

        // Child process finishes its job and exits cleanly
        _exit(0);
    } else if (pid > 0) {
        // ── Parent Process ───────────────────────────────────────────────
        // Wait for child process to finish ticket rendering (reaping zombie process)
        int status = 0;
        waitpid(pid, &status, 0);
        Logger::getInstance()->log("SYSTEM", "TicketPrinter child process (PID " + to_string(pid) + ") completed.");
    } else {
        // Fork failed fallback: print directly in parent process
        cerr << "[TicketPrinter] Fork failed. Falling back to inline printing.\n";
        bool hasTickets = false;
        for (auto& booking : systemBookings) {
            if (booking.getCustomerId() == customer->getCustId()) {
                hasTickets = true;
                cout << "================== TICKET ==================" << endl;
                cout << "Booking ID : BK" << booking.getBookingId() << endl; 
                cout << "Movie      : " << booking.getShow()->getMovieName() << endl;
                cout << "Screen     : Screen-" << booking.getShow()->getScreenId() 
                     << "    " << booking.getShow()->getStartTime() << ":00" << endl;
                cout << "Seats      : ";
                vector<int> seats = booking.getSeatIds();
                for (size_t i = 0; i < seats.size(); i++) {
                    cout << seats[i];
                    if (i != seats.size() - 1) {
                        cout << ", ";
                    }
                }
                cout << endl;
                cout << "Amount     : Rs." << booking.getTotalAmount() 
                     << "        Status: " << (booking.getBookingStatus() ? "CONFIRMED" : "CANCELLED") << endl;
                cout << "============================================" << endl << endl;
            }
        }
        if (!hasTickets) {
            cout << "No booking history found for this account." << endl;
        }
    }
}
