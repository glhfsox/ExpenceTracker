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
    static void saveAllAccountsToFile(const std::string& filename, const std::map<int, Account>& accounts) {
        std::ofstream outFile(filename, std::ios::out | std::ios::trunc);
        if (!outFile) {
            std::cerr << "Error opening file for writing: " << filename << "\n";
            return;
        }
        for (const auto& [id, account] : accounts) {
            outFile << "ACCOUNT_BEGIN\n";
            outFile << "ID: " << id << "\n";
            outFile << "INITIAL_BUDGET: " << std::fixed << std::setprecision(2) << account.getBudget() << "\n";
            outFile << "MONTHLY_BUDGET: " << std::fixed << std::setprecision(2) << account.getMonthlyBudget() << "\n";
            outFile << "EXPENSES_BEGIN\n";
            for (const auto& expense : account.getAllExpenses()) {
            std::tm expenseTm;
            #ifdef _WIN32
            localtime_s(&expenseTm, &std::get<3>(expense));
            #else
            localtime_r(&std::get<3>(expense), &expenseTm);
            #endif
                outFile << "AMOUNT: " << std::fixed << std::setprecision(2) << std::get<0>(expense)
                        << " | DESC: \"" << std::get<1>(expense) << "\""
                        << " | CAT: " << Account::CategoryToString(std::get<2>(expense))
                        << " | DATE: " << std::put_time(&expenseTm, "%Y-%m-%d %H:%M:%S") << "\n";
            }
            outFile << "EXPENSES_END\n";
            outFile << "ACCOUNT_END\n";
        }
        outFile.close();
        std::cout << "All accounts successfully saved to file: " << filename << "\n";
    }

    static void loadAllAccountsFromFile(const std::string& filename, std::map<int, Account>& accounts, int& nextAccountId) {
        accounts.clear();
        std::ifstream inFile(filename);
        if (!inFile) {
            std::cout << "No existing accounts file found.\n";
            nextAccountId = 1;
            return;
        }
        std::string line;
        int id = 0;
        double initialBudget = 0.0;
        double monthlyBudget = 0.0;
        std::vector<std::tuple<double, std::string, Account::Category, std::time_t>> expenses;
        int maxId = 0;
        while (std::getline(inFile, line)) {
            if (line == "ACCOUNT_BEGIN") {
                id = 0;
                initialBudget = 0.0;
                monthlyBudget = 0.0;
                expenses.clear();
            } else if (line.rfind("ID:", 0) == 0) {
                id = std::stoi(line.substr(3));
                if (id > maxId) maxId = id;
            } else if (line.rfind("INITIAL_BUDGET:", 0) == 0) {
                initialBudget = std::stod(line.substr(15));
            } else if (line.rfind("MONTHLY_BUDGET:", 0) == 0) {
                monthlyBudget = std::stod(line.substr(15));
            } else if (line == "EXPENSES_BEGIN") {
                while (std::getline(inFile, line) && line != "EXPENSES_END") { 
                    double amount = 0.0;
                    std::string desc, catStr, dateStr;
                    size_t pos1 = line.find("AMOUNT: ");
                    size_t pos2 = line.find(" | DESC: ");
                    size_t pos3 = line.find(" | CAT: ");
                    size_t pos4 = line.find(" | DATE: ");
                    if (pos1 != std::string::npos && pos2 != std::string::npos && pos3 != std::string::npos && pos4 != std::string::npos) {
                        amount = std::stod(line.substr(pos1 + 8, pos2 - (pos1 + 8)));
                        desc = line.substr(pos2 + 10, pos3 - (pos2 + 10) - 1); 
                        catStr = line.substr(pos3 + 8, pos4 - (pos3 + 8));
                        dateStr = line.substr(pos4 + 8);
                        
                        if (!desc.empty() && desc.front() == '"' && desc.back() == '"')
                            desc = desc.substr(1, desc.size() - 2);
                        Account::Category cat = Account::Category::Other;
                        if (catStr == "Food") cat = Account::Category::Food;
                        else if (catStr == "Clothes") cat = Account::Category::Clothes;
                        else if (catStr == "Travels") cat = Account::Category::Travels;
                        else cat = Account::Category::Other;
                        std::tm tm = {};
                        std::istringstream ss(dateStr);
                        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
                        std::time_t timestamp = std::mktime(&tm);
                        expenses.emplace_back(amount, desc, cat, timestamp);
                    }
                }
            } else if (line == "ACCOUNT_END") {
                Account acc(initialBudget, expenses);
                if (monthlyBudget > 0.0) {
                    acc.setMonthlyBudget(monthlyBudget);
                }
                accounts[id] = acc;
            }
        }
        nextAccountId = maxId + 1;
        std::cout << "All accounts successfully loaded from file: " << filename << "\n";
    }
};

void createAccount()     {
    double initialBudget;
    std::cout << "Enter initial budget: ";
    while (!(std::cin >> initialBudget) || initialBudget <= 0) {
        std::cout << "Invalid budget. Please enter a number greater than zero: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    accounts[nextAccountId] = Account(initialBudget);
    FileManager::saveAllAccountsToFile("accounts.txt", accounts);
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

void viewAccountDetails(int accountId) {
    if (accounts.find(accountId) == accounts.end()) {
        std::cout << "Account not found.\n";
        return;
    }
    const Account& acc = accounts.at(accountId);
    std::cout << "Account ID: " << accountId << "\n";
    std::cout << "Balance: " << std::fixed << std::setprecision(2) << acc.getBudget() << "\n";
    std::cout << "Monthly budget: " << std::fixed << std::setprecision(2) << acc.getMonthlyBudget() << "\n";
    std::cout << "Expenses:\n";
    std::cout << std::setw(10) << "Amount" << " | "
              << std::setw(30) << "Description" << " | "
              << std::setw(15) << "Category" << " | "
              << std::setw(20) << "Date" << "\n";
    std::cout << std::string(80, '-') << "\n";
    for (const auto& expense : acc.getAllExpenses()) {
        std::tm expenseTm;
        #ifdef _WIN32
        localtime_s(&expenseTm, &std::get<3>(expense));
        #else
        localtime_r(&std::get<3>(expense), &expenseTm);
        #endif
        std::cout << std::fixed << std::setprecision(2)
                  << std::setw(10) << std::get<0>(expense) << " | "
                  << std::setw(30) << std::get<1>(expense) << " | "
                  << std::setw(15) << Account::CategoryToString(std::get<2>(expense)) << " | "
                  << std::put_time(&expenseTm, "%Y-%m-%d %H:%M:%S") << "\n";
    }
    std::cout << std::string(80, '-') << "\n";
}

void printMenu() {
    std::cout << "\nChoose an operation:\n"
        << "1. Create new account\n"
        << "2. List all accounts\n"
        << "3. Select account\n"
        << "4. Transfer money\n"
        << "5. Exit\n"
        << "6. View account details\n"
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
            FileManager::saveAllAccountsToFile("accounts.txt", accounts);
            break;
        case 2:
            account.deleteExpense();
            FileManager::saveAllAccountsToFile("accounts.txt", accounts);
            break;
        case 3:
            account.updateExpense();
            FileManager::saveAllAccountsToFile("accounts.txt", accounts);
            break;
        case 4:
            account.viewExpence();
            break;
        case 5:
            account.SumOfAllExpenses();
            break;
        case 6:
            FileManager::saveAllAccountsToFile("accounts.txt", accounts);
            break;
        case 7: {
            FileManager::loadAllAccountsFromFile("accounts.txt", accounts, nextAccountId);
            if (accounts.find(accountId) != accounts.end()) {
                account = accounts[accountId];
                std::cout << "Data reloaded from file. New budget = " << account.getBudget() << "\n";
            } else {
                std::cout << "No valid data found in file.\n";
            }
            break;
        }
        case 8:
            account.MonthBudget();
            FileManager::saveAllAccountsToFile("accounts.txt", accounts);
            break;
        case 9:
            account.DepositMoney();
            FileManager::saveAllAccountsToFile("accounts.txt", accounts);
            break;
        case 10:
            account.WithdrawMoney();
            FileManager::saveAllAccountsToFile("accounts.txt", accounts);
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
    FileManager::saveAllAccountsToFile("accounts.txt", accounts);
}

int main() {
    std::cout << "Welcome to the Multi-Account Expense Tracker system!\n";
    FileManager::loadAllAccountsFromFile("accounts.txt", accounts, nextAccountId);
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
        case 6: {
            int accountId;
            std::cout << "Enter account ID to view details: ";
            std::cin >> accountId;
            viewAccountDetails(accountId);
            break;
        }
        default:
            std::cout << "Invalid choice. Please try again.\n";
            break;
        }
    }
    std::cout << "Thank you for using the Expense Tracker system made by drunk Warsaw University of Technology student! Goodbye!\n";
    FileManager::saveAllAccountsToFile("accounts.txt", accounts);
    return 0;
}
