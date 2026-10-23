//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Alex Staples
// Description : CS 300 Project Two, ABCU Advising Assistance Program.
//============================================================================

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// create a hash table with 23 buckets
const unsigned int NUM_BUCKETS = 23;

// one course from the file
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// each bucket holds a list of courses (chaining)
typedef vector<vector<Course>> HashTable;

// removes leading and trailing whitespace, including the \r from Windows files
string trim(const string& text) {
    size_t first = text.find_first_not_of(" \t\r\n");
    if (first == string::npos) {
        return "";
    }
    size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

// turns a course number into a bucket index
unsigned int hashCourse(const string& courseNumber) {
    unsigned int total = 0;
    for (int i = 0; i < courseNumber.length(); i++) {
        total = total * 31 + courseNumber[i];
    }
    return total % NUM_BUCKETS;
}

// adds a course to the end of its bucket
void insertCourse(HashTable& table, const Course& course) {
    table[hashCourse(course.courseNumber)].push_back(course);
}

// returns the matching course, or nullptr if it isn't in the table
const Course* searchCourse(const HashTable& table, const string& courseNumber) {
    const vector<Course>& bucket = table[hashCourse(courseNumber)];
    for (int i = 0; i < bucket.size(); i++) {
        if (bucket[i].courseNumber == courseNumber) {
            return &bucket[i];
        }
    }
    return nullptr;
}

// reads the file into the table and returns how many courses were loaded
int loadCourses(const string& fileName, HashTable& table) {
    table.assign(NUM_BUCKETS, vector<Course>());

    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Error: could not open the file " << fileName << "." << endl;
        return 0;
    }

    vector<string> loadedNumbers;
    string line;
    int lineNumber = 0;

    while (getline(file, line)) {
        ++lineNumber;
        if (trim(line).empty()) {
            continue;
        }

        // split the line on commas and trim each field
        vector<string> fields;
        stringstream lineStream(line);
        string field;
        while (getline(lineStream, field, ',')) {
            fields.push_back(trim(field));
        }

        // every line needs a course number and a title
        if (fields.size() < 2 || fields[0].empty() || fields[1].empty()) {
            cout << "Error: line " << lineNumber << " is missing a course number or title." << endl;
            continue;
        }

        // keep the first copy of a repeated course number
        if (searchCourse(table, fields[0]) != nullptr) {
            cout << "Warning: line " << lineNumber << " repeats " << fields[0] << " and was skipped." << endl;
            continue;
        }

        Course course;
        course.courseNumber = fields[0];
        course.courseTitle = fields[1];

        // anything after the title is a prerequisite, skip blanks from trailing commas
        for (int i = 2; i < fields.size(); i++) {
            if (!fields[i].empty()) {
                course.prerequisites.push_back(fields[i]);
            }
        }

        insertCourse(table, course);
        loadedNumbers.push_back(course.courseNumber);
    }
    file.close();

    if (loadedNumbers.size() == 0) {
        cout << "No courses were loaded." << endl;
        return 0;
    }

    // check prerequisites after loading
    // a missing prerequisite prints a warning, the course is still kept
    for (int i = 0; i < loadedNumbers.size(); i++) {
        const Course* course = searchCourse(table, loadedNumbers[i]);
        for (int j = 0; j < course->prerequisites.size(); j++) {
            if (searchCourse(table, course->prerequisites[j]) == nullptr) {
                cout << "Warning: " << loadedNumbers[i] << " lists " << course->prerequisites[j]
                     << ", which is not in the file." << endl;
            }
        }
    }

    return loadedNumbers.size();
}

// splits the courses around a middle pivot by course number
int partition(vector<Course>& courses, int begin, int end) {
    int low = begin;
    int high = end;

    // middle pivot so an already sorted list doesn't hit the worst case
    string pivot = courses[begin + (end - begin) / 2].courseNumber;

    bool done = false;
    while (!done) {
        while (courses[low].courseNumber < pivot) {
            ++low;
        }
        while (pivot < courses[high].courseNumber) {
            --high;
        }

        if (low >= high) {
            done = true;
        }
        else {
            swap(courses[low], courses[high]);
            ++low;
            --high;
        }
    }
    return high;
}

// sorts the courses by course number
void quickSort(vector<Course>& courses, int begin, int end) {
    if (begin >= end) {
        return;
    }
    int lowEnd = partition(courses, begin, end);
    quickSort(courses, begin, lowEnd);
    quickSort(courses, lowEnd + 1, end);
}

// copies every course into a vector, sorts it, and prints it
void printCourseList(const HashTable& table) {
    vector<Course> sorted;
    for (int i = 0; i < table.size(); i++) {
        for (int j = 0; j < table[i].size(); j++) {
            sorted.push_back(table[i][j]);
        }
    }
    quickSort(sorted, 0, sorted.size() - 1);

    cout << "Here is a sample schedule:" << endl << endl;
    for (int i = 0; i < sorted.size(); i++) {
        cout << sorted[i].courseNumber << ", " << sorted[i].courseTitle << endl;
    }
}

// prints one course's title and its prerequisites' numbers and titles
void printCourse(const HashTable& table, string courseNumber) {
    // convert to uppercase
    courseNumber = trim(courseNumber);
    transform(courseNumber.begin(), courseNumber.end(), courseNumber.begin(), ::toupper);

    const Course* course = searchCourse(table, courseNumber);
    if (course == nullptr) {
        cout << courseNumber << " was not found." << endl;
        return;
    }

    cout << course->courseNumber << ", " << course->courseTitle << endl;

    if (course->prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
        return;
    }

    cout << "Prerequisites:" << endl;
    for (int i = 0; i < course->prerequisites.size(); i++) {
        string prereq = course->prerequisites[i];
        const Course* prereqCourse = searchCourse(table, prereq);
        if (prereqCourse != nullptr) {
            cout << "  " << prereq << ", " << prereqCourse->courseTitle << endl;
        }
        else {
            cout << "  " << prereq << ", not found" << endl;
        }
    }
}

// runs the menu until the user picks 9
int main() {
    HashTable courses;
    bool dataLoaded = false;
    int choice = 0;
    string input;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {
        cout << endl;
        cout << "  1. Load Data Structure." << endl;
        cout << "  2. Print Course List." << endl;
        cout << "  3. Print Course." << endl;
        cout << "  9. Exit" << endl << endl;
        cout << "What would you like to do? ";

        if (!getline(cin, input)) {
            break;
        }
        input = trim(input);

        // every option is one digit, anything else is invalid
        if (input.size() == 1 && isdigit(input[0])) {
            choice = input[0] - '0';
        }
        else {
            choice = -1;
        }

        if (choice == 1) {
            cout << "Enter the course data file name: ";
            getline(cin, input);
            dataLoaded = loadCourses(trim(input), courses) > 0;
        }
        else if ((choice == 2 || choice == 3) && !dataLoaded) {
            cout << "Please load the course data first." << endl;
        }
        else if (choice == 2) {
            printCourseList(courses);
        }
        else if (choice == 3) {
            cout << "What course do you want to know about? ";
            getline(cin, input);
            printCourse(courses, input);
        }
        else if (choice == 9) {
            cout << "Thank you for using the course planner!" << endl;
        }
        else {
            cout << input << " is not a valid option." << endl;
        }
    }

    return 0;
}
