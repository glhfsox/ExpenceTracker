#define _CRT_SECURE_NO_WARNINGS
#define _HAS_STD_BYTE 0
#include <iostream>
#include "Account.h"
#include <fstream>
#include <vector>
#include <tuple>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <limits>
#include <map>
#include <filesystem>

const std::string expensesFilename = "expenses.txt";
std::map<int, Account> accounts;
int nextAccountId = 1;
class FileManager {
public:
    static void saveDataToFile(const std::string& filename, double budget, const std::vector<std::tuple<double, std::string, Account::Category, std::time_t>>& expenses) {
        std::ofstream outFile(filename, std::ios::out | std::ios::trunc);
        if (!outFile) {
            std::cerr << "Error opening file for writing: " << filename << "\n";
            return;
        }

        outFile << "BUDGET:" << std::fixed << std::setprecision(2) << budget << "\n";
        outFile << "EXPENSES:\n";
        
        for (const auto& expense : expenses) {
            std::tm expenseTm;
            #ifdef _WIN32
            localtime_s(&expenseTm, &std::get<3>(expense));
            #else
            localtime_r(&std::get<3>(expense), &expenseTm);
            #endif
            
            outFile << std::fixed << std::setprecision(2) << std::get<0>(expense) << "|" 
                   << std::quoted(std::get<1>(expense)) << "|"  
                   << static_cast<int>(std::get<2>(expense)) << "|"  
                   << std::put_time(&expenseTm, "%Y-%m-%d %H:%M:%S") << "\n";  
        }
        
        outFile.close();
        std::cout << "Data successfully saved to file: " << filename << "\n";
    }
    static std::pair<double, std::vector<std::tuple<double, std::string, Account::Category, std::time_t>>> loadDataFromFile(const std::string& filename) {
        std::ifstream testFile(filename);
        if (!testFile.good()) {
            std::cout << "No existing data file found.\n";
            return { 0.0, {} };
        }
        testFile.close();

        std::ifstream inFile(filename, std::ios::in);
        double budget = 0.0;
        std::vector<std::tuple<double, std::string, Account::Category, std::time_t>> expenses;

        if (!inFile) {
            std::cerr << "Error opening file for reading: " << filename << "\n";
            return { budget, expenses };
        }

        std::string line;
        bool readingExpenses = false;

        while (std::getline(inFile, line)) {
            if (line.empty()) continue;

            if (line == "EXPENSES:") {
                readingExpenses = true;
                continue;
            }

            if (!readingExpenses && line.substr(0, 7) == "BUDGET:") {
                try {
                    budget = std::stod(line.substr(7));
                } catch (const std::exception& e) {
                    std::cerr << "Error parsing budget value: " << e.what() << "\n";
                }
                continue;
            }

            if (readingExpenses) {
                std::istringstream iss(line);
                std::string token;
                std::vector<std::string> tokens;
                
                while (std::getline(iss, token, '|')) {
                    tokens.push_back(token);
                }

                if (tokens.size() == 4) {
                    try {
                        double amount = std::stod(tokens[0]);
                        std::string description = tokens[1];
                        int categoryInt = std::stoi(tokens[2]);
                        
                        std::tm tm = {};
                        std::istringstream ss(tokens[3]);
                        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
                        if (ss.fail()) {
                            std::cerr << "Error parsing date: " << tokens[3] << "\n";
                            continue;
                        }
                        std::time_t timestamp = std::mktime(&tm);

                        expenses.emplace_back(amount, description, static_cast<Account::Category>(categoryInt), timestamp);
                    } catch (const std::exception& e) {
                        std::cerr << "Error parsing expense line: " << line << " - " << e.what() << "\n";
                    }
                }
            }
        }

        inFile.close();
        if (budget > 0.0 || !expenses.empty()) {
            std::cout << "Successfully loaded data from file: " << filename << "\n";
            std::cout << "Budget: " << std::fixed << std::setprecision(2) << budget << "\n";
            std::cout << "Number of expenses: " << expenses.size() << "\n";
        }
        return { budget, expenses };
    }
};

void createAccount() {
    double initialBudget;
    std::cout << "Enter initial budget: ";
    while (!(std::cin >> initialBudget) || initialBudget <= 0) {
        std::cout << "Invalid budget. Please enter a number greater than zero: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    accounts[nextAccountId] = Account(initialBudget);
    std::cout << "Account created successfully with ID: " << nextAccountId << "\n";
    nextAccountId++;
}

void listAccounts() {
    if (accounts.empty()) {
        std::cout << "No accounts available.\n";
        return;
    }
    std::cout << "\nAvailable accounts:\n";
    std::cout << std::setw(10) << "ID" << " | " << std::setw(15) << "Balance" << "\n";
    std::cout << std::string(30, '-') << "\n";
    for (const auto& [id, account] : accounts) {
        std::cout << std::setw(10) << id << " | "
            << std::fixed << std::setprecision(2) << std::setw(15)
            << account.getBudget() << "\n";
    }
}

void printMenu() {
    std::cout << "\nChoose an operation:\n"
        << "1. Create new account\n"
        << "2. List all accounts\n"
        << "3. Select account\n"
        << "4. Transfer money\n"
        << "5. Exit\n"
        << "Your choice: ";
}

void printAccountMenu() {
    std::cout << "\nAccount Operations:\n"
        << "1. Add expense\n"
        << "2. Delete expense\n"
        << "3. Update expense\n"
        << "4. View expenses\n"
        << "5. Sum of all expenses\n"
        << "6. Save data to file\n"
        << "7. Reload data from file\n"
        << "8. Set the month budget\n"
        << "9. Deposit money\n"
        << "10. Withdraw money\n"
        << "11. Return to main menu\n"
        << "Your choice: ";
}

void handleAccountOperations(Account& account, int accountId) {
    int choice = 0;
    bool returnToMain = false;
    std::string filename = "account_" + std::to_string(accountId) + ".txt";

    while (!returnToMain) {
        printAccountMenu();
        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore();

        switch (choice) {
        case 1:
            account.addExpense();
            FileManager::saveDataToFile(filename, account.getBudget(), account.getAllExpenses());
            break;
        case 2:
            account.deleteExpense();
            FileManager::saveDataToFile(filename, account.getBudget(), account.getAllExpenses());
            break;
        case 3:
            account.updateExpense();
            FileManager::saveDataToFile(filename, account.getBudget(), account.getAllExpenses());
            break;
        case 4:
            account.viewExpence();
            break;
        case 5:
            account.SumOfAllExpenses();
            break;
        case 6:
            FileManager::saveDataToFile(filename, account.getBudget(), account.getAllExpenses());
            break;
        case 7: {
            auto newData = FileManager::loadDataFromFile(filename);
            if (newData.first > 0.0) {
                account = Account(newData.first, newData.second);
                std::cout << "Data reloaded from file. New budget = " << newData.first << "\n";
            } else {
                std::cout << "No valid data found in file.\n";
            }
            break;
        }
        case 8:
            account.MonthBudget();
            break;
        case 9:
            account.DepositMoney();
            FileManager::saveDataToFile(filename, account.getBudget(), account.getAllExpenses());
            break;
        case 10:
            account.WithdrawMoney();
            FileManager::saveDataToFile(filename, account.getBudget(), account.getAllExpenses());
            break;
        case 11:
            returnToMain = true;
            break;
        default:
            std::cout << "Invalid choice. Please try again.\n";
            break;
        }
    }
}

void transferMoney() {
    if (accounts.size() < 2) {
        std::cout << "Please create at least two accounts before transferring money.\n";
        return;
    }

    listAccounts();
    int senderId, receiverId;
    
    std::cout << "Enter the ID of the sender account: ";
    if (!(std::cin >> senderId)) {
        std::cout << "Invalid input. Please enter a number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    std::cout << "Enter the ID of the receiver account: ";
    if (!(std::cin >> receiverId)) {
        std::cout << "Invalid input. Please enter a number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    if (accounts.find(senderId) == accounts.end() || accounts.find(receiverId) == accounts.end()) {
        std::cout << "Invalid account ID. Please try again.\n";
        return;
    }

    if (senderId == receiverId) {
        std::cout << "Cannot transfer money to the same account.\n";
        return;
    }

    double transferAmount;
    std::cout << "Enter the amount to transfer: ";
    while (!(std::cin >> transferAmount) || transferAmount <= 0) {
        std::cout << "Invalid amount. Please enter a number greater than zero: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    accounts[senderId].SendMoney(accounts[receiverId], transferAmount);

    // Save both accounts after transfer
    FileManager::saveDataToFile("account_" + std::to_string(senderId) + ".txt", 
        accounts[senderId].getBudget(), accounts[senderId].getAllExpenses());
    FileManager::saveDataToFile("account_" + std::to_string(receiverId) + ".txt", 
        accounts[receiverId].getBudget(), accounts[receiverId].getAllExpenses());
}

int main() {
    std::cout << "Welcome to the Multi-Account Expense Tracker system!\n";
    
    int choice = 0;
    bool exitProgram = false;

    while (!exitProgram) {
        printMenu();
        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore();

        switch (choice) {
        case 1:
            createAccount();
            break;
        case 2:
            listAccounts();
            break;
        case 3: {
            if (accounts.empty()) {
                std::cout << "No accounts available. Please create an account first.\n";
                break;
            }
            listAccounts();
            int accountId;
            std::cout << "Enter account ID to select: ";
            if (!(std::cin >> accountId)) {
                std::cout << "Invalid input. Please enter a number.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            }
            if (accounts.find(accountId) != accounts.end()) {
                handleAccountOperations(accounts[accountId], accountId);
            } else {
                std::cout << "Invalid account ID.\n";
            }
            break;
        }
        case 4:
            transferMoney();
            break;
        case 5:
            exitProgram = true;
            break;
        default:
            std::cout << "Invalid choice. Please try again.\n";
            break;
        }
    }

    std::cout << "Thank you for using the Expense Tracker system! Goodbye!\n";
    return 0;
}
