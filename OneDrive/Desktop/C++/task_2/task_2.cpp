// Student Result Management System
// OOP Using C++ - Task 2 (Weeks 7-13)
// Concepts: inheritance, virtual functions (runtime polymorphism), exception handling,
//           file handling, templates, STL vector, STL algorithms, lambda expressions

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

const int NUM_SUBJECTS = 5;
const int PASS_MARK = 40;
const std::string FILE_NAME = "students.txt";

// ---------- Custom exception for invalid marks ----------
class InvalidMarksException : public std::runtime_error {
public:
    explicit InvalidMarksException(const std::string &msg) : std::runtime_error(msg) {}
};

// ---------- Base class ----------
class Student {
protected:
    int rollNo;
    std::string name;

public:
    Student(int r = 0, const std::string &n = "") : rollNo(r), name(n) {}
    virtual ~Student() = default;

    int getRollNo() const { return rollNo; }
    const std::string &getName() const { return name; }

    // Virtual function -> runtime polymorphism
    virtual void display() const {
        std::cout << "Roll No : " << rollNo << "\nName    : " << name << "\n";
    }

    virtual std::string toFileString() const {
        return std::to_string(rollNo) + "," + name;
    }
};

// ---------- Derived class ----------
class Result : public Student {
private:
    std::vector<int> marks;

    static void validate(const std::vector<int> &m) {
        if (static_cast<int>(m.size()) != NUM_SUBJECTS)
            throw InvalidMarksException("Exactly " + std::to_string(NUM_SUBJECTS) +
                                        " subject marks are required.");
        for (int x : m)
            if (x < 0 || x > 100)
                throw InvalidMarksException("Invalid marks (" + std::to_string(x) +
                                            "). Marks must be between 0 and 100.");
    }

public:
    Result(int r, const std::string &n, const std::vector<int> &m) : Student(r, n), marks(m) {
        validate(marks);  // throws if invalid
    }

    int getTotal() const { return std::accumulate(marks.begin(), marks.end(), 0); }

    double getPercentage() const { return getTotal() * 100.0 / (NUM_SUBJECTS * 100); }

    std::string getResult() const {
        bool allPass = std::all_of(marks.begin(), marks.end(),
                                   [](int x) { return x >= PASS_MARK; });
        return allPass ? "PASS" : "FAIL";
    }

    void display() const override {
        Student::display();
        std::cout << "Marks   : ";
        for (int x : marks) std::cout << x << " ";
        std::cout << "\nTotal   : " << getTotal() << " / " << NUM_SUBJECTS * 100
                  << "\nPercent : " << std::fixed << std::setprecision(2) << getPercentage() << "%"
                  << "\nResult  : " << getResult() << "\n";
        std::cout << "-----------------------------------\n";
    }

    std::string toFileString() const override {
        std::string s = Student::toFileString();
        for (int x : marks) s += "," + std::to_string(x);
        return s;
    }
};

// ---------- Template function ----------
template <typename T>
void displayAll(const std::vector<std::shared_ptr<T>> &list) {
    if (list.empty()) {
        std::cout << "No records to display.\n";
        return;
    }
    for (const auto &s : list) s->display();  // polymorphic call
}

// ---------- File handling ----------
void saveToFile(const std::vector<std::shared_ptr<Student>> &records) {
    std::ofstream out(FILE_NAME);
    if (!out) throw std::runtime_error("Unable to open file for writing.");
    for (const auto &r : records) out << r->toFileString() << "\n";
    std::cout << "Saved " << records.size() << " record(s) to " << FILE_NAME << "\n";
}

std::vector<std::shared_ptr<Student>> loadFromFile() {
    std::ifstream in(FILE_NAME);
    if (!in) throw std::runtime_error("File '" + FILE_NAME + "' not found. Save records first.");

    std::vector<std::shared_ptr<Student>> records;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> parts;
        while (std::getline(ss, token, ',')) parts.push_back(token);
        if (parts.size() != static_cast<size_t>(2 + NUM_SUBJECTS)) continue;  // skip bad line

        try {
            std::vector<int> m;
            for (int i = 2; i < 2 + NUM_SUBJECTS; i++) m.push_back(std::stoi(parts[i]));
            records.push_back(std::make_shared<Result>(std::stoi(parts[0]), parts[1], m));
        } catch (const std::exception &e) {
            std::cout << "Skipping corrupted line: " << e.what() << "\n";
        }
    }
    return records;
}

// ---------- Input helper ----------
int readInt(const std::string &prompt) {
    int v;
    while (true) {
        std::cout << prompt;
        if (std::cin >> v) return v;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Please enter a valid number.\n";
    }
}

void addStudent(std::vector<std::shared_ptr<Student>> &records) {
    int roll = readInt("Enter Roll No: ");
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string name;
    std::cout << "Enter Name: ";
    std::getline(std::cin, name);

    std::vector<int> marks;
    for (int i = 1; i <= NUM_SUBJECTS; i++)
        marks.push_back(readInt("Enter marks for subject " + std::to_string(i) + ": "));

    try {
        records.push_back(std::make_shared<Result>(roll, name, marks));
        std::cout << "Student added successfully.\n";
    } catch (const InvalidMarksException &e) {
        std::cout << "Error: " << e.what() << " Record not added.\n";
    }
}

void showAboveMark(const std::vector<std::shared_ptr<Student>> &records) {
    int threshold = readInt("Enter mark threshold (total marks): ");

    std::vector<std::shared_ptr<Student>> filtered;
    // STL algorithm + lambda expression
    std::copy_if(records.begin(), records.end(), std::back_inserter(filtered),
                 [threshold](const std::shared_ptr<Student> &s) {
                     auto r = std::dynamic_pointer_cast<Result>(s);
                     return r && r->getTotal() > threshold;
                 });

    std::cout << "\nStudents scoring above " << threshold << " (" << filtered.size() << " found):\n";
    displayAll(filtered);
}

void showSortedByTotal(std::vector<std::shared_ptr<Student>> records) {
    std::sort(records.begin(), records.end(),
              [](const std::shared_ptr<Student> &a, const std::shared_ptr<Student> &b) {
                  return std::dynamic_pointer_cast<Result>(a)->getTotal() >
                         std::dynamic_pointer_cast<Result>(b)->getTotal();
              });
    std::cout << "\nRecords sorted by total marks (highest first):\n";
    displayAll(records);
}

int main() {
    std::vector<std::shared_ptr<Student>> records;
    int choice;

    do {
        std::cout << "\n===== Student Result Management System =====\n"
                  << "1. Add student record\n"
                  << "2. Display all records\n"
                  << "3. Display students scoring above a mark\n"
                  << "4. Display records sorted by total\n"
                  << "5. Save records to file\n"
                  << "6. Load records from file\n"
                  << "0. Exit\n";
        choice = readInt("Enter choice: ");

        try {
            switch (choice) {
                case 1: addStudent(records); break;
                case 2: std::cout << "\n"; displayAll(records); break;
                case 3: showAboveMark(records); break;
                case 4: showSortedByTotal(records); break;
                case 5: saveToFile(records); break;
                case 6:
                    records = loadFromFile();
                    std::cout << "Loaded " << records.size() << " record(s).\n";
                    break;
                case 0: std::cout << "Goodbye!\n"; break;
                default: std::cout << "Invalid choice.\n";
            }
        } catch (const std::exception &e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    } while (choice != 0);

    return 0;
}