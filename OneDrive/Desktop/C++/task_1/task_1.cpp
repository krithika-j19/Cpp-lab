#include <iostream>
#include <string>
using namespace std;

class Student {
private:
    int id;
    string name;
    float marks;
    static int count;              
public:
   
    Student(int id, string name, float marks) {
        this->id = id;
        this->name = name;
        this->marks = marks;
        count++;
    }


    Student(const Student &other) {
        id = other.id;
        name = other.name;
        marks = other.marks;
        count++;
    }

    
    ~Student() {
        count--;
    }

    
    void setDetails(int newId, string newName) {
        id = newId;
        name = newName;
    }
    void setDetails(int newId, string newName, float newMarks) {
        id = newId;
        name = newName;
        marks = newMarks;
    }
    void setDetails(float newMarks) {
        marks = newMarks;
    }

    
    int getId() const { return id; }
    string getName() const { return name; }
    float getMarks() const { return marks; }

    void display() const {
        cout << "ID: " << id << ", Name: " << name << ", Marks: " << marks << endl;
    }

    
    static int getCount() {
        return count;
    }

    
    friend void showMarks(const Student &s);

    
    bool operator>(const Student &other) const {
        return marks > other.marks;
    }
};


int Student::count = 0;


void showMarks(const Student &s) {
    cout << "[Friend] " << s.name << " scored " << s.marks << " marks." << endl;
}

int main() {
    Student s1(101, "Aarav", 85.5f);
    Student s2(102, "Diya", 92.0f);
    Student s3(103, "Kabir", 78.25f);

    cout << "--- Student Details ---" << endl;
    s1.display();
    s2.display();
    s3.display();

    cout << "\nTotal students: " << Student::getCount() << endl;

    cout << "\n--- Friend Function ---" << endl;
    showMarks(s1);
    showMarks(s2);

    cout << "\n--- Operator > Comparison ---" << endl;
    if (s1 > s2)
        cout << s1.getName() << " has higher marks than " << s2.getName() << endl;
    else
        cout << s2.getName() << " has higher (or equal) marks than " << s1.getName() << endl;

    if (s2 > s3)
        cout << s2.getName() << " has higher marks than " << s3.getName() << endl;

    cout << "\n--- Overloaded Functions ---" << endl;
    s3.setDetails(88.0f);                    
    s3.display();
    s3.setDetails(104, "Kabir S");           
    s3.display();
    s3.setDetails(105, "Kabir Singh", 90.5f); 
    s3.display();

    {
        Student temp(200, "Temp", 50);
        cout << "\nInside block, total students: " << Student::getCount() << endl;
    }
    cout << "After block, total students: " << Student::getCount() << endl;

    return 0;
}