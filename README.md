# ExpenceTracker

ExpenceTracker is a C++ console application for personal finance management with multi-account support, monthly budgeting, and detailed expense history.

## Features

- **Multi-account:** Each account has its own balance and expense history
- **Monthly budget:** Set and control a monthly budget for each account
- **Expense management:** Add, delete, and update expenses with category and date
- **Transfers:** Transfer money between accounts
- **Unified storage:** All data is saved and loaded from a single file (`accounts.txt`)
- **Detailed account view:** View full transaction history for any account

## Usage

1. **Build the project** in Visual Studio or with CMake.
2. **Run the program** — all data will be loaded automatically from `accounts.txt`.
3. **Use the menu** to:
    - Create accounts
    - Add expenses
    - View detailed account history
    - Transfer money between accounts
4. **All changes are saved automatically.**

## Data Format Example

```
ACCOUNT_BEGIN
ID: 1
INITIAL_BUDGET: 1000.00
MONTHLY_BUDGET: 500.00
EXPENSES_BEGIN
AMOUNT: 100.00 | DESC: "Food" | CAT: Food | DATE: 2024-06-01 12:00:00
AMOUNT: 50.00  | DESC: "Taxi" | CAT: Travels | DATE: 2024-06-02 15:00:00
EXPENSES_END
ACCOUNT_END
```

## License

MIT License (see repository) 