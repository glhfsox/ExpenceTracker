#include "Account.h"


void Account::MonthBudget() {
    std::cout << "Please enter your monthly budget: ";
    double inputBudget;
    while (!(std::cin >> inputBudget) || inputBudget > balance || inputBudget < 0) {
        std::cout << "Invalid monthly budget, try again: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    monthlyBudget = inputBudget;
    std::cout << "Your monthly budget was successfully set to: " << std::fixed << std::setprecision(2) << monthlyBudget << "\n";
}

void Account::SortByCategory(Category c) {
    std::sort(allExpenses.begin(), allExpenses.end(),
        [c](const auto& a, const auto& b) {
            return std::get<2>(a) < std::get<2>(b);
        });
    std::cout << "Expenses sorted by category\n";
}

void Account::SumOfAllExpenses() const {
    if (allExpenses.empty()) {
        std::cout << "You have no expenses yet.\n";
        return;
    }

    double total = std::accumulate(allExpenses.begin(), allExpenses.end(), 0.0,
        [](double sum, const std::tuple<double, std::string, Category, std::time_t>& expense) {
            return sum + std::get<0>(expense);
        });

    std::cout << "Total expenses: " << std::fixed << std::setprecision(2) << total << "\n";
    std::cout << "Remaining balance: " << std::fixed << std::setprecision(2) << balance << "\n";
}

void Account::updateExpense() {
    if (allExpenses.empty()) {
        std::cout << "No expenses to update.\n";
        return;
    }

    std::string targetDescription;
    std::cout << "Enter the description of the expense you want to update: ";
    std::cin.ignore();
    std::getline(std::cin, targetDescription);

    auto it = std::find_if(allExpenses.begin(), allExpenses.end(),
        [&](const std::tuple<double, std::string, Category, std::time_t>& expense) {
            return std::get<1>(expense) == targetDescription;
        });

    if (it == allExpenses.end()) {
        std::cout << "Expense with such description not found.\n";
        return;
    }

    std::cout << "What would you like to update? (1 - amount, 2 - category): ";
    int choice;
    std::cin >> choice;

    if (choice == 1) {
        double newAmount;
        std::cout << "Enter new amount: ";
        while (!(std::cin >> newAmount) || newAmount < 0 || newAmount > balance + std::get<0>(*it)) {
            std::cout << "Invalid amount. Try again: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        balance += std::get<0>(*it) - newAmount;
        std::get<0>(*it) = newAmount;
        std::cout << "Amount successfully updated.\n";
    }
    else if (choice == 2) {
        int newCategoryChoice;
        std::cout << "Enter new category:\n"
            << " 0) Food\n"
            << " 1) Clothes\n"
            << " 2) Travels\n"
            << " 3) Other\n"
            << "Choice: ";
        while (!(std::cin >> newCategoryChoice) || newCategoryChoice < 0 || newCategoryChoice > 3) {
            std::cout << "Invalid choice. Try again: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::get<2>(*it) = static_cast<Category>(newCategoryChoice);
        std::cout << "Category successfully updated.\n";
    }
    else {
        std::cout << "Invalid choice.\n";
    }
}

void Account::viewExpence() const {
    if (allExpenses.empty()) {
        std::cout << "No expenses.\n";
        return;
    }
    std::cout << "\nYour expenses:\n";
    std::cout << std::setw(10) << "Amount" << " | "
              << std::setw(30) << "Description" << " | "
              << std::setw(15) << "Category" << " | "
              << std::setw(20) << "Date" << "\n";
    std::cout << std::string(80, '-') << "\n";

    for (const auto& expense : allExpenses) {
        std::tm expenseTm;
        #ifdef _WIN32
        localtime_s(&expenseTm, &std::get<3>(expense));
        #else
        localtime_r(&std::get<3>(expense), &expenseTm);
        #endif

        std::cout << std::fixed << std::setprecision(2)
                  << std::setw(10) << std::get<0>(expense) << " | "
                  << std::setw(30) << std::get<1>(expense) << " | "
                  << std::setw(15) << CategoryToString(std::get<2>(expense)) << " | "
                  << std::put_time(&expenseTm, "%Y-%m-%d %H:%M:%S") << "\n";
    }
    std::cout << std::string(80, '-') << "\n";
    std::cout << "Total expenses: " << std::fixed << std::setprecision(2) 
              << std::accumulate(allExpenses.begin(), allExpenses.end(), 0.0,
                  [](double sum, const auto& expense) { return sum + std::get<0>(expense); })
              << "\n";
    std::cout << "Remaining balance: " << std::fixed << std::setprecision(2) << balance << "\n";
}

void Account::deleteExpense() {
    if (allExpenses.empty()) {
        std::cout << "No expenses to delete.\n";
        return;
    }

    std::cout << "Enter the expense description to delete: ";
    std::string targetDescription;
    std::cin.ignore();
    std::getline(std::cin, targetDescription);

    auto it = std::find_if(allExpenses.begin(), allExpenses.end(),
        [&](const std::tuple<double, std::string, Category, std::time_t>& expense) {
            return std::get<1>(expense) == targetDescription;
        });

    if (it != allExpenses.end()) {
        balance += std::get<0>(*it);
        allExpenses.erase(it);
        std::cout << "Expense successfully deleted.\n";
        std::cout << "New balance: " << std::fixed << std::setprecision(2) << balance << "\n";
    }
    else {
        std::cout << "Expense with such description not found.\n";
    }
}

void Account::addExpense() {
    double expenseAmount;
    std::string description;
    int categoryChoice;

   
    if (monthlyBudget > 0) {
       
        std::time_t now = std::time(nullptr);
        std::tm currentTm;
        #ifdef _WIN32
        localtime_s(&currentTm, &now);
        #else
        localtime_r(&now, &currentTm);
        #endif

        double currentMonthExpenses = 0.0;
        for (const auto& expense : allExpenses) {
            std::tm expenseTm;
            #ifdef _WIN32
            localtime_s(&expenseTm, &std::get<3>(expense));
            #else
            localtime_r(&std::get<3>(expense), &expenseTm);
            #endif

            if (expenseTm.tm_year == currentTm.tm_year && expenseTm.tm_mon == currentTm.tm_mon) {
                currentMonthExpenses += std::get<0>(expense);
            }
        }

        if (currentMonthExpenses >= monthlyBudget * 0.8) {
            std::cout << "\nWARNING: You have already spent " << std::fixed << std::setprecision(2) 
                      << (currentMonthExpenses / monthlyBudget * 100) 
                      << "% of your monthly budget!\n";
            std::cout << "Current monthly expenses: " << currentMonthExpenses << "\n";
            std::cout << "Monthly budget: " << monthlyBudget << "\n";
            std::cout << "Are you sure you want to add another expense? (y/n): ";
            char confirm;
            std::cin >> confirm;
            if (confirm != 'y' && confirm != 'Y') {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    std::cout << "Enter expense amount: ";
    while (!(std::cin >> expenseAmount) || expenseAmount < 0 || expenseAmount > balance) {
        std::cout << "Invalid amount. Try again: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::cin.ignore();
    std::cout << "Enter expense description: ";
    std::getline(std::cin, description);

    std::cout << "Select category:\n"
        << " 0) Food\n"
        << " 1) Clothes\n"
        << " 2) Travels\n"
        << " 3) Other\n"
        << "Choice: ";
    while (!(std::cin >> categoryChoice) || categoryChoice < 0 || categoryChoice > 3) {
        std::cout << "Invalid choice. Try again: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    Category category = static_cast<Category>(categoryChoice);
    std::time_t now = std::time(nullptr);
    allExpenses.emplace_back(expenseAmount, description, category, now);
    balance -= expenseAmount;

std::cout << "Expense added successfully.\n";
std::cout << "Remaining balance: " << std::fixed << std::setprecision(2) << balance << "\n";


if (monthlyBudget > 0) {
    std::tm currentTm;
#ifdef _WIN32
    localtime_s(&currentTm, &now);
#else
    localtime_r(&now, &currentTm);
#endif

    double totalMonthExpenses = 0.0;
    for (const auto& expense : allExpenses) {
        std::tm expenseTm;
#ifdef _WIN32
        localtime_s(&expenseTm, &std::get<3>(expense));
#else
        localtime_r(&std::get<3>(expense), &expenseTm);
#endif

        if (expenseTm.tm_year == currentTm.tm_year && expenseTm.tm_mon == currentTm.tm_mon) {
            totalMonthExpenses += std::get<0>(expense);
        }
    }

    if (totalMonthExpenses > monthlyBudget) {
        std::cout << "\nWARNING: You have exceeded your monthly budget!\n";
        std::cout << "Monthly expenses: " << totalMonthExpenses << "\n";
        std::cout << "Monthly budget: " << monthlyBudget << "\n";
        std::cout << "Exceeded by: " << (totalMonthExpenses - monthlyBudget) << "\n";
    }
}
}

void Account::viewExpensesByDate(std::time_t startDate, std::time_t endDate) const {
    if (allExpenses.empty()) {
        std::cout << "No expenses yet.\n";
        return;
    }

    std::tm startTm, endTm;
#ifdef _WIN32
    localtime_s(&startTm, &startDate);
    localtime_s(&endTm, &endDate);
#else
    localtime_r(&startDate, &startTm);
    localtime_r(&endDate, &endTm);
#endif

    std::cout << "Expenses from "
        << std::put_time(&startTm, "%Y-%m-%d") << " to "
        << std::put_time(&endTm, "%Y-%m-%d") << ":\n";

    std::cout << std::setw(10) << "Amount" << " | "
        << std::setw(30) << "Description" << " | "
        << std::setw(15) << "Category" << " | "
        << std::setw(20) << "Date" << "\n";
    std::cout << std::string(80, '-') << "\n";

    double total = 0.0;
    for (const auto& expense : allExpenses) {
        std::time_t expenseTime = std::get<3>(expense);
        if (expenseTime >= startDate && expenseTime <= endDate) {
            std::tm expenseTm;
#ifdef _WIN32
            localtime_s(&expenseTm, &expenseTime);
#else
            localtime_r(&expenseTime, &expenseTm);
#endif

            std::cout << std::fixed << std::setprecision(2)
                << std::setw(10) << std::get<0>(expense) << " | "
                << std::setw(30) << std::get<1>(expense) << " | "
                << std::setw(15) << CategoryToString(std::get<2>(expense)) << " | "
                << std::put_time(&expenseTm, "%Y-%m-%d %H:%M:%S") << "\n";

            total += std::get<0>(expense);
        }
    }
    std::cout << std::string(80, '-') << "\n";
    std::cout << "Total expenses for period: " << std::fixed << std::setprecision(2) << total << "\n";
}

void Account::DepositMoney() {
    double deposit;
    std::cout << "Please enter the amount of money you want to deposit: ";
    while (!(std::cin >> deposit) || deposit < 0) {
        std::cout << "Invalid amount. Please try again.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    balance += deposit;
    std::cout << "Your balance was updated. Now it is " << std::fixed << std::setprecision(2) << balance << " dollars\n";
}

void Account::WithdrawMoney() {
    double withdraw;
    std::cout << "Please enter the amount of money you want to withdraw: ";
    while (!(std::cin >> withdraw) || withdraw < 0 || withdraw > balance) {
        std::cout << "Invalid amount or insufficient funds. Please try again.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    balance -= withdraw;
    std::cout << "Your balance was updated. Now it is " << std::fixed << std::setprecision(2) << balance << " dollars\n";
}

void Account::SendMoney(Account& receiver, double amount) {
    
    if (amount <= 0) {
        std::cout << "Error: Transfer amount must be greater than zero.\n";
        return;
    }

    
    if (amount > balance) {
        std::cout << "Error: Insufficient funds for transfer.\n";
        std::cout << "Your balance: " << std::fixed << std::setprecision(2) << balance << "\n";
        std::cout << "Transfer amount: " << std::fixed << std::setprecision(2) << amount << "\n";
        return;
    }

    
    if (this == &receiver) {
        std::cout << "Error: Cannot transfer money to the same account.\n";
        return;
    }

    
    balance -= amount;
    receiver.balance += amount;

    
    std::time_t now = std::time(nullptr);
    std::string description = "Transfer to another account";
    allExpenses.emplace_back(amount, description, Category::Other, now);

    
    std::cout << "Transfer completed successfully!\n";
    std::cout << "Amount transferred: " << std::fixed << std::setprecision(2) << amount << "\n";
    std::cout << "Your new balance: " << std::fixed << std::setprecision(2) << balance << "\n";
    std::cout << "Receiver's new balance: " << std::fixed << std::setprecision(2) << receiver.balance << "\n";
}
