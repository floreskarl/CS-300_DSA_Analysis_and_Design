// NAME:        Karl Flores
// COURSE:      CS-300 – Data Structures & Algorithms
// ASSIGNMENT:  Project Two – Advising Assistance Program
// DATE:        August 2026
//
// -------------------------------------------------------------
// This program loads course information from a CSV file,
// stores the courses in a hash table for fast lookup, and
// maintains a vector for sorted output. The user can:
//
//     1. Load Data Structure
//     2. Print Course List (sorted)
//     3. Print Course Information
//     9. Exit
//
// The program follows the Project Two rubric and matches the
// sample output formatting exactly.
// -------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

const unsigned int DEFAULT_SIZE = 179;

// -------------------------------------------------------------
// Course Structure
// Represents a single course with number, title, and prerequisites
// -------------------------------------------------------------
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// -------------------------------------------------------------
// HashTable
// Linked-list chaining hash table adapted from Assignment 4-2
// -------------------------------------------------------------
class HashTable {

private:

    struct Node {
        Course course;
        unsigned int key;
        Node* next;

        Node() : key(0), next(nullptr) {}
        Node(const Course& c, unsigned int k) : course(c), key(k), next(nullptr) {}
    };

    vector<Node*> table;
    unsigned int tableSize;

    // hash(key):
    // -------------
    // Compress numeric key into table range
    //     index ⇐ key MOD tableSize
    //     return index
    unsigned int hash(unsigned int key) const {
        return key % tableSize;
    }

    // strToKey(str):
    // -------------
    // Convert courseNumber string into numeric key
    //     hash ⇐ 0
    //     for each character ch:
    //         hash ⇐ (hash * 31) + ch
    //     return hash
    unsigned int strToKey(const string& str) const {
        unsigned int hash = 0;
        for (char ch : str) {
            hash = (hash * 31) + ch;
        }
        return hash;
    }

public:

    // HashTable(size):
    // -------------
    // Initialize table with NULL buckets
    HashTable(unsigned int size = DEFAULT_SIZE) : tableSize(size) {
        table.resize(size, nullptr);
    }

    // ~HashTable():
    // -------------
    // Free all nodes in each bucket chain
    ~HashTable() {

        for (unsigned int i = 0; i < tableSize; ++i) {

            // initialize traversal decals
            Node* pCurrent = table[i];

            // walk chain
            while (pCurrent != nullptr) {
                Node* pNext = pCurrent->next;
                delete pCurrent;
                pCurrent = pNext;
            }
        }
    }

    // Insert(course):
    // -------------
    // Insert a course using chaining
    //
    //     key ⇐ strToKey(course.courseNumber)
    //     index ⇐ hash(key)
    //
    //     if bucket empty:
    //         table[index] ⇐ new Node(course)
    //
    //     else walk chain to end and append
    void Insert(const Course& course) {

        unsigned int key = strToKey(course.courseNumber);
        unsigned int index = hash(key);

        if (table[index] == nullptr) {
            table[index] = new Node(course, key);
            return;
        }

        // walk chain
        Node* pCurrent = table[index];
        while (pCurrent->next != nullptr) {
            pCurrent = pCurrent->next;
        }

        // append node
        pCurrent->next = new Node(course, key);
    }

    // Search(courseNumber):
    // -------------
    // Search for the specified courseNumber and return the matching course
    // -------------
    //
    //     // convert courseNumber into numeric key
    //     key ⇐ strToKey(courseNumber)
    //
    //     // compute bucket index
    //     index ⇐ hash(key)
    //
    //     // initialize traversal decals
    //     pCurrent ⇐ table[index]
    //
    //     // walk chain
    //     while (pCurrent ≠ NULL):
    //
    //         if (pCurrent.course.courseNumber = courseNumber):
    //             return pCurrent.course
    //
    //         // shift traversal cursor
    //         pCurrent ⇐ pCurrent.next
    //
    //     // return empty course if not found
    Course Search(const string& courseNumber) const {

        unsigned int key = strToKey(courseNumber);
        unsigned int index = hash(key);

        // initialize traversal decals
        Node* pCurrent = table[index];

        // walk chain
        while (pCurrent != nullptr) {

            if (pCurrent->course.courseNumber == courseNumber) {
                return pCurrent->course;
            }

            // shift traversal cursor
            pCurrent = pCurrent->next;
        }

        // return empty course if not found
        return Course();
    }
};

// trim(s):
// -------------
// Remove leading/trailing whitespace and carriage returns
string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// loadCourses(filename, courseVector, courseTable):
// -------------
// Load course data from CSV file into vector and hash table
// -------------
//
//     open file
//     while getline(line):
//         split line by commas
//         trim tokens
//         create Course object
//         push prerequisites
//         push into vector
//         insert into hash table
void loadCourses(const string& filename,
                 vector<Course>& courseVector,
                 HashTable& courseTable) {

    ifstream file(filename);
    string line;

    while (getline(file, line)) {

        // tokenize line
        stringstream ss(line);
        string token;
        vector<string> tokens;

        while (getline(ss, token, ',')) {
            tokens.push_back(trim(token));
        }

        if (tokens.size() < 2) {
            continue;
        }

        // build course object
        Course c;
        c.courseNumber = tokens[0];
        c.courseTitle  = tokens[1];

        // walk remaining tokens for prerequisites
        for (unsigned int i = 2; i < tokens.size(); ++i) {
            if (!tokens[i].empty()) {
                c.prerequisites.push_back(tokens[i]);
            }
        }

        courseVector.push_back(c);
        courseTable.Insert(c);
    }
}

// printCourseList(courseVector):
// -------------
// Print sorted course list
void printCourseList(const vector<Course>& courseVector) {

    cout << "Here is a sample schedule:\n" << endl;

    // walk each course
    for (const Course& c : courseVector) {
        cout << c.courseNumber << ", " << c.courseTitle << endl;
    }
}

// printCourseInfo(courseTable):
// -------------
// Prompt user and print course details
void printCourseInfo(const HashTable& courseTable) {

    cout << "\nWhat course do you want to know about? ";
    string target;
    cin >> target;

    Course c = courseTable.Search(target);

    if (c.courseNumber.empty()) {
        cout << "\nCourse not found.\n" << endl;
        return;
    }

    cout << "\n" << c.courseNumber << ", " << c.courseTitle << endl;

    if (c.prerequisites.empty()) {
        cout << "Prerequisites: None\n" << endl;
    }
    else {
        cout << "Prerequisites: ";

        // walk prerequisites
        for (unsigned int i = 0; i < c.prerequisites.size(); ++i) {
            cout << c.prerequisites[i];
            if (i < c.prerequisites.size() - 1) {
                cout << ", ";
            }
        }

        cout << "\n" << endl;
    }
}

// main():
// -------------
// Display menu and handle user input
int main() {

    vector<Course> courseVector;
    HashTable courseTable;

    string filename = "CS 300 ABCU_Advising_Program_Input.csv";

    cout << "Welcome to the course planner.\n" << endl;

    int choice = 0;

    while (choice != 9) {

        cout << "1. Load Data Structure.\n";
        cout << "2. Print Course List.\n";
        cout << "3. Print Course.\n";
        cout << "9. Exit\n" << endl;

        cout << "What would you like to do? ";
        cin >> choice;

        if (choice == 1) {

            courseVector.clear();
            loadCourses(filename, courseVector, courseTable);

            // sort course list
            sort(courseVector.begin(), courseVector.end(),
                [](const Course& a, const Course& b) {
                    return a.courseNumber < b.courseNumber;
                });

            cout << endl;
        }
        else if (choice == 2) {

            cout << endl;
            printCourseList(courseVector);
        }
        else if (choice == 3) {

            printCourseInfo(courseTable);
        }
        else if (choice == 9) {

            cout << "Thank you for using the course planner!" << endl;
        }
        else {

            cout << choice << " is not a valid option.\n" << endl;
        }
    }

    return 0;
}
