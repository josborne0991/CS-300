// CS 300 Project Two - ABCU Advising Assistance Program
// Uses a binary search tree to store and display course information.

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

struct Course {
    string number;
    string title;
    vector<string> prerequisites;
};

string trim(const string& value) {
    size_t first = value.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

string upper(string value) {
    transform(value.begin(), value.end(), value.begin(),
        [](unsigned char c) { return static_cast<char>(toupper(c)); });
    return value;
}

class CourseTree {
private:
    struct Node {
        Course course;
        unique_ptr<Node> left;
        unique_ptr<Node> right;
        explicit Node(const Course& c) : course(c) {}
    };
    unique_ptr<Node> root;
    size_t count = 0;

    static void insertNode(unique_ptr<Node>& node, const Course& course, bool& added) {
        if (!node) {
            node = make_unique<Node>(course);
            added = true;
        }
        else if (course.number < node->course.number) {
            insertNode(node->left, course, added);
        }
        else if (course.number > node->course.number) {
            insertNode(node->right, course, added);
        }
        else {
            node->course = course; // Replace duplicate course number.
        }
    }

    static void printInOrder(const Node* node) {
        if (!node) return;
        printInOrder(node->left.get());
        cout << node->course.number << ", " << node->course.title << '\n';
        printInOrder(node->right.get());
    }

public:
    void insert(const Course& course) {
        bool added = false;
        insertNode(root, course, added);
        if (added) ++count;
    }

    const Course* find(const string& number) const {
        const Node* current = root.get();
        while (current) {
            if (number == current->course.number) return &current->course;
            current = (number < current->course.number)
                ? current->left.get() : current->right.get();
        }
        return nullptr;
    }

    void printSorted() const { printInOrder(root.get()); }
    bool empty() const { return count == 0; }
    size_t size() const { return count; }
};

bool loadCourses(const string& filename, CourseTree& courses) {
    ifstream input(filename);
    if (!input) {
        cout << "Error: Could not open file '" << filename << "'.\n";
        return false;
    }

    CourseTree newCourses; // Preserve existing data if loading fails.
    string line;
    size_t lineNumber = 0;
    while (getline(input, line)) {
        ++lineNumber;
        if (trim(line).empty()) continue;
        stringstream stream(line);
        vector<string> fields;
        string field;
        while (getline(stream, field, ',')) fields.push_back(trim(field));
        if (fields.size() < 2 || fields[0].empty() || fields[1].empty()) {
            cout << "Error: Invalid course data on line " << lineNumber << ".\n";
            return false;
        }
        Course course;
        course.number = upper(fields[0]);
        course.title = fields[1];
        for (size_t i = 2; i < fields.size(); ++i) {
            if (!fields[i].empty()) course.prerequisites.push_back(upper(fields[i]));
        }
        newCourses.insert(course);
    }
    if (newCourses.empty()) {
        cout << "Error: The file contains no courses.\n";
        return false;
    }
    courses = move(newCourses);
    cout << "Successfully loaded " << courses.size() << " courses.\n";
    return true;
}

void printCourseInformation(const CourseTree& courses, const string& number) {
    const Course* course = courses.find(upper(trim(number)));
    if (!course) {
        cout << "Error: Course not found.\n";
        return;
    }
    cout << course->number << ", " << course->title << '\n';
    if (course->prerequisites.empty()) {
        cout << "Prerequisites: None\n";
        return;
    }
    cout << "Prerequisites:\n";
    for (const string& prereqNumber : course->prerequisites) {
        const Course* prereq = courses.find(prereqNumber);
        cout << "  " << prereqNumber;
        if (prereq) cout << ", " << prereq->title;
        else cout << " (course details unavailable)";
        cout << '\n';
    }
}

int main() {
    CourseTree courses;
    cout << "Welcome to the ABCU Advising Assistance Program.\n";
    string choice;
    while (true) {
        cout << "\n1. Load Data Structure\n"
            << "2. Print Course List\n"
            << "3. Print Course\n"
            << "9. Exit\n"
            << "What would you like to do? ";
        if (!getline(cin, choice)) break;
        choice = trim(choice);

        if (choice == "1") {
            cout << "Enter the course data filename: ";
            string filename;
            if (!getline(cin, filename)) break;
            loadCourses(trim(filename), courses);
        }
        else if (choice == "2") {
            if (courses.empty()) cout << "Please load the course data first (option 1).\n";
            else {
                cout << "\nHere is a sample schedule:\n";
                courses.printSorted();
            }
        }
        else if (choice == "3") {
            if (courses.empty()) {
                cout << "Please load the course data first (option 1).\n";
            }
            else {
                cout << "What course do you want to know about? ";
                string number;
                if (!getline(cin, number)) break;
                printCourseInformation(courses, number);
            }
        }
        else if (choice == "9") {
            cout << "Thank you for using the ABCU Advising Assistance Program!\n";
            break;
        }
        else {
            cout << "Invalid option. Please enter 1, 2, 3, or 9.\n";
        }
    }
    return 0;
}
