//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Christopher Bryan
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : CS-300 Project Two
// Instructor  : Chao Ling
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm> // for std::transform
#include <cctype>    // for std::toupper

using namespace std;

/*
 * Struct to hold course information.
 *   number: "CSCI200"
 *   title: "Data Structures"
 *   prerequisites: {"CSCI101"}
 */
struct Course {
    string number;
    string title;
    vector<string> prerequisites;
};

// Node to be used in the Binary Search Tree (BST).
struct Node {
    Course course;
    Node* left;
    Node* right;

    Node(const Course& c) : course(c), left(nullptr), right(nullptr) {}
};

// BST wrapper class for storing and searching courses.("CSCI100", "CSCI200", etc.).
class CourseBST {
private:
    Node* root;

    // Helper to destroy the tree
    void destroyTree(Node* node) {
        if (node == nullptr) {
            return;
        }
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

    // Helper to insert a course into the BST
    Node* insert(Node* node, const Course& course) {
        if (node == nullptr) {
            return new Node(course);
        }

        if (course.number < node->course.number) {
            node->left = insert(node->left, course);
        } else if (course.number > node->course.number) {
            node->right = insert(node->right, course);
        } else {
            // If duplicate course number: ignore
        }
        return node;
    }

    // Helper for in-order traversal to print courses sorted by course number
    void inorder(Node* node) const {
        if (node == nullptr) {
            return;
        }
        inorder(node->left);
        cout << node->course.number << ", " << node->course.title << endl;
        inorder(node->right);
    }

    // Helper to search for a course by number
    Node* search(Node* node, const string& courseNumber) const {
        if (node == nullptr) {
            return nullptr;
        }

        if (courseNumber == node->course.number) {
            return node;
        } else if (courseNumber < node->course.number) {
            return search(node->left, courseNumber);
        } else {
            return search(node->right, courseNumber);
        }
    }

public:
    CourseBST() : root(nullptr) {}

    ~CourseBST() {
        destroyTree(root);
    }

    // Removes all nodes from the tree
    void clear() {
        destroyTree(root);
        root = nullptr;
    }

    // Public insert
    void insert(const Course& course) {
        root = insert(root, course);
    }

    // Print all courses in sorted order
    void printInOrder() const {
        if (root == nullptr) {
            cout << "No courses loaded." << endl;
            return;
        }
        inorder(root);
    }

    // Find and return a pointer to the Node containing the course
    const Node* find(const string& courseNumber) const {
        return search(root, courseNumber);
    }

    bool isEmpty() const {
        return root == nullptr;
    }
};

// Trim the leading and trailing whitespace from the string.
string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) {
        return "";
    }
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// Converts the string to uppercase to handle case-insensitive course number input ("csci400" → "CSCI400").
string toUpperCopy(string s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c){ return static_cast<char>(toupper(c)); });
    return s;
}

// Split a CSV line into tokens by comma.
vector<string> splitCSV(const string& line) {
    vector<string> tokens;
    string token;
    stringstream ss(line);

    while (getline(ss, token, ',')) {
        tokens.push_back(token);
    }
    return tokens;
}

// Load the courses from a CSV file and place into the provided BST. Returns true if loading succeeds, false otherwise.
bool loadCoursesFromFile(const string& filename, CourseBST& bst) {
    ifstream infile(filename);
    if (!infile.is_open()) {
        cout << "Error: Could not open file '" << filename << "'." << endl;
        return false;
    }

    // Clear the existing tree before loading new data
    bst.clear();

    string line;
    while (getline(infile, line)) {
        line = trim(line);
        if (line.empty()) {
            continue; // skip empty lines
        }

        vector<string> tokens = splitCSV(line);
        if (tokens.size() < 2) {
            cout << "Warning: Invalid line (needs at least course number and title): "
                 << line << endl;
            continue;
        }

        Course course;
        course.number = toUpperCopy(trim(tokens[0]));
        course.title  = trim(tokens[1]);

        // Remaining tokens (if any) are prerequisite course numbers
        for (size_t i = 2; i < tokens.size(); ++i) {
            string prereqCode = toUpperCopy(trim(tokens[i]));
            if (!prereqCode.empty()) {
                course.prerequisites.push_back(prereqCode);
            }
        }

        bst.insert(course);
    }

    infile.close();
    cout << "Data loaded successfully from '" << filename << "'." << endl;
    return true;
}

// Print detailed information about the course: course number, title, prerequisite numbers and titles (if any)
void printCourseInformation(const CourseBST& bst, const string& courseNumber) {
    string key = toUpperCopy(trim(courseNumber));
    const Node* node = bst.find(key);

    if (node == nullptr) {
        cout << "Course " << key << " not found." << endl;
        return;
    }

    const Course& mainCourse = node->course;

    // Main course
    cout << mainCourse.number << ", " << mainCourse.title << endl;

    // If there are no prerequisites
    if (mainCourse.prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
        return;
    }

    // Print prerequisite list: number and title (if the course exists in BST)
    cout << "Prerequisites: ";
    for (size_t i = 0; i < mainCourse.prerequisites.size(); ++i) {
        const string& prereqNum = mainCourse.prerequisites[i];
        const Node* prereqNode = bst.find(prereqNum);

        if (prereqNode != nullptr) {
            cout << prereqNode->course.number << ", "
                 << prereqNode->course.title;
        } else {
            // If no prereq details found, just print the code
            cout << prereqNum;
        }

        if (i + 1 < mainCourse.prerequisites.size()) {
            cout << " | ";
        }
    }
    cout << endl;
}

// Safely read an integer menu choice from user input. If the input is invalid, it returns -1.
int readMenuChoice() {
    string input;
    cout << "What would you like to do? ";
    getline(cin, input);

    // Trim any whitespace
    input = trim(input);
    if (input.empty()) {
        return -1;
    }

    // Checks that all characters are digits
    for (char c : input) {
        if (!isdigit(static_cast<unsigned char>(c))) {
            return -1;
        }
    }

    try {
        return stoi(input);
    } catch (...) {
        return -1;
    }
}

// Main driver: shows menu and processes user input.
int main() {
    CourseBST bst;
    bool dataLoaded = false;

    cout << "Welcome to the course planner." << endl;

    int choice = 0;
    while (choice != 9) {
        cout << "1. Load Data Structure." << endl;
        cout << "2. Print Course List." << endl;
        cout << "3. Print Course." << endl;
        cout << "9. Exit" << endl;

        choice = readMenuChoice();

        switch (choice) {
        case 1: {
            cout << "Enter the file name to load (e.g., CS 300 ABCU_Advising_Program_Input.csv): ";
            string filename;
            getline(cin, filename);
            filename = trim(filename);

            if (filename.empty()) {
                cout << "File name cannot be empty." << endl;
                break;
            }

            if (loadCoursesFromFile(filename, bst)) {
                dataLoaded = true;
            } else {
                dataLoaded = false;
            }
            break;
        }

        case 2:
            if (!dataLoaded) {
                cout << "Please load the data structure first (option 1)." << endl;
            } else {
                cout << "Here is a sample schedule:" << endl;
                bst.printInOrder();
            }
            break;

        case 3:
            if (!dataLoaded) {
                cout << "Please load the data structure first (option 1)." << endl;
            } else {
                cout << "What course do you want to know about? ";
                string courseNum;
                getline(cin, courseNum);
                printCourseInformation(bst, courseNum);
            }
            break;

        case 9:
            cout << "Thank you for using the course planner!" << endl;
            break;

        default:
            cout << choice << " is not a valid option." << endl;
            break;
        }

        // Add a small spacing line between iterations
        if (choice != 9) {
            cout << endl;
        }
    }

    return 0;
}

