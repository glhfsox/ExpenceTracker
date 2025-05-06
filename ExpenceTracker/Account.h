#include <tuple> 
#include <string>
#include <vector>
#include <iostream>
#include <limits>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <iomanip>
#include <ctime>

class Account {
public:
    enum class Category { Food = 0, Clothes = 1, Travels = 2, Other = 3 };
    
    
    Account() : balance(0.0), sumOfAllExpences(0.0), category(Category::Other), monthlyBudget(0.0) {}
    
    Account(double initialBalance, 
            const std::vector<std::tuple<double, std::string, Category, std::time_t>>& initialExpenses = {}, 
            double initialSumOfExpenses = 0.0) :
        balance(initialBalance),
        allExpenses(initialExpenses),
        sumOfAllExpences(initialSumOfExpenses),
        category(Category::Other),
        monthlyBudget(0.0) {}

    void addExpense();
    void deleteExpense();
    void updateExpense();
    void viewExpence() const;
    void SumOfAllExpenses() const;
    void setCategory(Category c) { category = c; }
    Category GetCategory() const { return category; }
    void MonthBudget();
    void DepositMoney();
    void WithdrawMoney();
    void SendMoney(Account& receiver, double amount);
    void setMonthlyBudget(double value) { monthlyBudget = value; }

    static std::string CategoryToString(Category c) {
        switch (c) {
        case Category::Food:
            return "Food";
        case Category::Clothes:
            return "Clothes";
        case Category::Travels:
            return "Travels";
        case Category::Other:
            return "Other expenses";
        default:
            return "Unknown expense/expenses";
        }
    }

    const std::vector<std::tuple<double, std::string, Category, std::time_t>>& getAllExpenses() const { 
        return allExpenses; 
    }
    
    void SortByCategory(Category c);
    void viewExpensesByDate(std::time_t startDate, std::time_t endDate) const;
    double getBudget() const { return balance; }
    double getMonthlyBudget() const { return monthlyBudget; }
    double getSumOfAllExpenses() const { return sumOfAllExpences; }

private:
    double balance;
    std::vector<std::tuple<double, std::string, Category, std::time_t>> allExpenses;
    double sumOfAllExpences;
    Category category;
    double monthlyBudget;
};

