/*******************************
 * Diamond Inheritance Problem *
 *******************************/
//
// This program addresses the diamond inheritance problem by
// handling a university's list of members
//
// Inheritance Scheme of the project:
//
//    IOInterface                         std::exception         University
//        |                                     |
//     Person      StaffInterface          AlreadySet
//     /    \       /
//  Stud    UnivStaff
//     \    /
//  StudentTeacher
//


#include <iostream>
#include <iomanip>
#include <cstring>
#include <string>
#include <list>
#include <tuple>
#include <vector>
#include <ctime>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <conio.h>
#include <chrono>
#include <thread>

// define colours for the console
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RED     "\x1b[91m"
#define ANSI_COLOR_RESET   "\x1b[0m"

using namespace std;

class AlreadySet : public exception{
public:
    const char* what() const noexcept override{
        return "Field is set already.";
    }
};

class StaffInterface{
    // must have functions for Staff members
public:
    virtual void setSalary(int) = 0;
    virtual void setDepartment(string) = 0;
    virtual void setRank(string) = 0;
};

class IOInterface{
public:
    virtual ostream& print(ostream&) const = 0;
    virtual istream& read(istream&) = 0;
};

ostream& operator<<(ostream& out, const IOInterface& obj) {
    return obj.print(out);
}

istream& operator>>(istream& in, IOInterface& obj){
    return obj.read(in);
}

// Person -------------------------------------
class Person : public IOInterface{
    string firstName;
    string lastName;
    tm dateOfBirth;
    const string SSN;
    char gender;

public:
    Person();
    Person(string, string, tm, string, char);
    Person(const Person&);
    Person& operator=(const Person&);

    virtual ostream& print(ostream&) const override;
    virtual istream& read(istream&) override;

    // utils
    virtual Person* clone() const;
    unsigned int calculateAge() const;
    static string generateSSN();

    // getters
    string getFullName() const;
    string getFirstName() const;
    string getLastName() const;
    string getDateOfBirth() const;
    string getSSN() const;
    char getGender() const;

    // setters
    void setFirstName(string);
    void setLastName(string);
    void setDateOfBirth(string);
    void setSSN(string);
    void setGender(char);

    virtual ~Person();
};

Person::Person() : firstName("N/A"), lastName("N/A"), dateOfBirth({0}), SSN(Person::generateSSN()), gender('-'){}

Person::Person(string firstName, string lastName, tm dateOfBirth, string SSN, char gender):
                firstName(firstName), lastName(lastName), dateOfBirth(dateOfBirth), 
                SSN(SSN.empty() ? generateSSN() : SSN), gender(toupper(gender)){}

Person::Person(const Person& obj):Person(obj.firstName, obj.lastName, obj.dateOfBirth, obj.SSN, obj.gender){}

Person& Person::operator=(const Person& obj){
    if (this == &obj)
        return *this;
    
    this -> firstName = obj.firstName;
    this -> lastName = obj.lastName;
    this -> gender = obj.gender;

    return *this;
}

ostream& Person::print(ostream& out) const{
    // print a border around the person's information (ID ASCII art)
    out << ANSI_COLOR_GREEN
        << "|" <<  string(38, '*') << "|\n"
        << "| " << ANSI_COLOR_RED << setw(40/2) << right << "ID" << setw(40/2 + 1) << ANSI_COLOR_GREEN << " |\n"
        << "|" <<  string(38, '*') << "|\n"
        << ANSI_COLOR_RESET
        << ANSI_COLOR_GREEN << "| " << ANSI_COLOR_RESET << left << setw(40 - 3) << "First Name: " + this -> firstName << ANSI_COLOR_GREEN << "|\n"
        << ANSI_COLOR_GREEN << "| " << ANSI_COLOR_RESET << left << setw(40 - 3) << "Last Name: " + this -> lastName << ANSI_COLOR_GREEN << "|\n"
        << ANSI_COLOR_GREEN << "| " << ANSI_COLOR_RESET << left << setw(40 - 3) << "Date of Birth: " + this -> getDateOfBirth() << ANSI_COLOR_GREEN << "|\n"
        << ANSI_COLOR_GREEN << "| " << ANSI_COLOR_RESET << left << setw(40 - 3) << "SSN: " + this -> SSN << ANSI_COLOR_GREEN << "|\n"
        << ANSI_COLOR_GREEN << "| " << ANSI_COLOR_RESET << left << setw(40 - 3) << "Gender: " + string(1, this -> gender) << ANSI_COLOR_GREEN << "|\n"
        << ANSI_COLOR_GREEN << "|" <<  right << setw(40) << "|\n"
        << "|" <<  string(38, '*') << "|" << ANSI_COLOR_RESET << endl << endl;

    return out;
}

istream& Person::read(istream& in){
    cout << "First name: ";
    getline(in, firstName);
    cout << "Last name: ";
    getline(in, lastName);
    cout << "Date of birth (DD/MM/YYYY): ";
    string aux;
    in >> aux;
    this -> setDateOfBirth(aux);
    cout << "Gender: ";
    in >> this -> gender;
    in.ignore();

    return in;
}

Person* Person::clone() const{
    return new Person(*this);
}

// utils
unsigned int Person::calculateAge() const{
    // current date
    time_t now = time(0);
    tm* timeinfo = localtime(&now);

    int currentYear = timeinfo -> tm_year + 1900;
    int currentMonth = timeinfo -> tm_mon + 1;
    int currentDay = timeinfo -> tm_mday;

    // calculate age
    return currentYear - dateOfBirth.tm_year - (currentMonth < dateOfBirth.tm_mon || (currentMonth == dateOfBirth.tm_mon && currentDay < dateOfBirth.tm_mday));
}

// getters
string Person::generateSSN(){
    return to_string(rand() % 900000 + 100000); // proof of concept generator
}

string Person::getFullName() const{
    return firstName + " " + lastName;
}

string Person::getFirstName() const{
    return firstName;
}

string Person::getLastName() const{
    return lastName;
}

string Person::getDateOfBirth() const{
    return to_string(dateOfBirth.tm_mday) + "/" + to_string(dateOfBirth.tm_mon) + "/" + to_string(dateOfBirth.tm_year);
}

string Person::getSSN() const{
    return SSN;
}

char Person::getGender() const{
    return gender;
}

// setters
void Person::setFirstName(string firstName){
    this -> firstName = firstName;
}

void Person::setLastName(string lastName){
    this -> lastName = lastName;
}

void Person::setDateOfBirth(string dateOfBirth){
    if (this -> dateOfBirth.tm_mday != 0)   // can't modify someone's birth date
        throw AlreadySet();

    // check if dateOfBirth is of format dd/mm/yyyy
    if (dateOfBirth.length() != 10)
        throw invalid_argument("Date of birth must be in format dd/mm/yyyy.");
    if (dateOfBirth[2] != '/' || dateOfBirth[5] != '/')
        throw invalid_argument("Date of birth must be in format dd/mm/yyyy.");
    if (stoi(dateOfBirth.substr(0, 2)) < 1 || stoi(dateOfBirth.substr(0, 2)) > 31)
        throw invalid_argument("Day must be between 1 and 31.");
    if (stoi(dateOfBirth.substr(3, 2)) < 1 || stoi(dateOfBirth.substr(3, 2)) > 12)
        throw invalid_argument("Month must be between 1 and 12.");
    
    // current year
    time_t now = time(0);
    tm* timeinfo = localtime(&now);
    int currentYear = timeinfo->tm_year + 1900;

    if (stoi(dateOfBirth.substr(6, 4)) < 1900 || stoi(dateOfBirth.substr(6, 4)) > currentYear)
        throw invalid_argument("Year must be between 1900 and 2024.");

    this -> dateOfBirth.tm_mday = stoi(dateOfBirth.substr(0, 2));
    this -> dateOfBirth.tm_mon = stoi(dateOfBirth.substr(3, 2));
    this -> dateOfBirth.tm_year = stoi(dateOfBirth.substr(6, 4));
}

void Person::setGender(char gender){
    this -> gender = toupper(gender);
}

Person::~Person(){}

// Student --------------------------------------
class Student : virtual public Person{
    const string studentID;
    string major;
    char levelOfStudy;
    unordered_map<string, tuple<double, int>> grades;      // course: (grade, weight)
    double GPA;

    static int studentIDs;

public:
    Student();
    Student(string, string, tm, string, char, string, string, char, unordered_map<string, tuple<double, int>>, double);
    Student(const Student&);
    Student& operator=(const Student&);

    virtual ostream& print(ostream&) const override;
    virtual istream& read(istream&) override;
    virtual Student* clone() const override;

    // utils
    void addGrade(pair<string, tuple<double, int>> grade);
    void updateGPA();
    static string generateStudentID();

    // getters
    string getStudentID() const;
    string getMajor() const;
    string getLevelOfStudy() const;
    const unordered_map<string, tuple<double, int>>& getGrades() const;
    double getGPA() const;

    // setters
    void setMajor(string);
    void setLevelOfStudy(string);
    void setGPA(double);

    virtual ~Student();
};

int Student::studentIDs = 100;

Student::Student() : Person(), studentID(Student::generateStudentID()), major("N/A"), levelOfStudy(0), GPA(0){}

Student::Student(string firstName, string lastName, 
                 tm dateOfBirth, string SSN, 
                 char gender, string studentID,
                 string major, char levelOfStudy, 
                 unordered_map<string, tuple<double, int>> grades, double GPA): 
                    Person(firstName, lastName, dateOfBirth, SSN, gender), 
                    studentID(studentID.empty() ? Student::generateStudentID() : studentID), 
                    major(major), 
                    levelOfStudy(levelOfStudy), 
                    grades(grades), 
                    GPA(GPA){}

Student::Student(const Student& obj):Person(obj), 
                                    studentID(obj.studentID), 
                                    major(obj.major), 
                                    levelOfStudy(obj.levelOfStudy), 
                                    grades(obj.grades), 
                                    GPA(obj.GPA){}

Student& Student::operator=(const Student& obj){
    if (this == &obj)
        return *this;
    
    Person::operator=(obj);
    this -> major = obj.major;
    this -> levelOfStudy = obj.levelOfStudy;
    this -> GPA = obj.GPA;

    return *this;
}

ostream& Student::print(ostream& out) const{
    // print a border around the student's information (ID ASCII art)
    int width = 50;
    out << ANSI_COLOR_GREEN
        << "|" <<  string(width - 2, '*') << "|\n"
        << "| " << ANSI_COLOR_RED << setw(width/2 + 2) << right << "Student ID" << setw(width/2 - 1) << ANSI_COLOR_GREEN << " |\n"
        << "|" <<  string(width - 2, '*') << "|\n"
        << ANSI_COLOR_RESET
        << ANSI_COLOR_GREEN << "| " << ANSI_COLOR_RESET << left << setw(width - 3) << "First Name: " + this -> getFirstName() << ANSI_COLOR_GREEN << "|\n"
        << ANSI_COLOR_GREEN << "| " << ANSI_COLOR_RESET << left << setw(width - 3) << "Last Name: " + this -> getLastName() << ANSI_COLOR_GREEN << "|\n"
        << ANSI_COLOR_GREEN << "| " << ANSI_COLOR_RESET << left << setw(width - 3) << "Date of Birth: " + this -> getDateOfBirth() << ANSI_COLOR_GREEN << "|\n"
        << ANSI_COLOR_GREEN << "| " << ANSI_COLOR_RESET << left << setw(width - 3) << "Student ID: " + this -> studentID << ANSI_COLOR_GREEN << "|\n"
        << ANSI_COLOR_GREEN << "| " << ANSI_COLOR_RESET << left << setw(width - 3) << "Major: " + this -> major << ANSI_COLOR_GREEN << "|\n"
        << ANSI_COLOR_GREEN << "| " << ANSI_COLOR_RESET << left << setw(width - 3) << "Level of Study: " + this -> getLevelOfStudy() << ANSI_COLOR_GREEN << "|\n"
        << ANSI_COLOR_GREEN << "|" <<  right << setw(width) << "|\n"
        << "|" <<  string(width - 2, '*') << "|" << ANSI_COLOR_RESET << endl << endl;

    return out;
}

istream& Student::read(istream& in){
    string aux, major;
    vector<tuple<double, int>> grades;
    Person::read(in);

    cout << "Major: ";
    getline(in, aux);
    setMajor(aux);
    cout << "Level of study(Bachelor's, Master's, PhD): ";
    in >> aux;
    setLevelOfStudy(aux);
    cout << "Introduce any grades? (Y/n): ";
    in >> aux;
    while (aux == "Y" || aux == "y"){
        pair<string, tuple<double, int>> grade;
        cout << "Exam name: ";
        in.ignore();
        getline(in, grade.first);
        cout << "Grade (1-10): ";
        in >> get<0>(grade.second);
        cout << "Weight: ";
        in >> get<1>(grade.second);
        addGrade(grade);
        cout << "Add another grade? (Y/n): ";
        in >> aux;
        this -> updateGPA();
    }

    in.ignore();
    return in;
}

Student* Student::clone() const{
    return new Student(*this);
}

// utils
void Student::addGrade(pair<string, tuple<double, int>> grade){
    if (get<0>(grade.second) < 0 || get<0>(grade.second) > 10)
        throw invalid_argument("Grade must be between 0 and 10");

    get<0>(grade.second) = double(int(get<0>(grade.second) * 100)) / 100;

    (this -> grades)[grade.first] = grade.second;

    this -> updateGPA();
}

void Student::updateGPA(){
    double sum = 0;
    int weight = 0;
    for(auto it : this -> grades) {
        sum += get<0>(it.second) * get<1>(it.second);
        weight += get<1>(it.second);
    }

    this -> setGPA(sum / weight);
}

string Student::generateStudentID(){
    // current year
    time_t now = time(0);
    tm* timeinfo = localtime(&now);
    int currentYear = timeinfo->tm_year + 1900;

    return to_string(++studentIDs) + "/" + to_string(currentYear);
}

// getters
string Student::getStudentID() const{
    return studentID;
}

string Student::getMajor() const{
    return major;
}

string Student::getLevelOfStudy() const{
    switch(levelOfStudy){
        case 'b':
            return "Bachelor's";
        case 'm':
            return "Master's";
        case 'p':
            return "PhD";
        default:
            return "Not found";
    }
}

const unordered_map<string, tuple<double, int>>& Student::getGrades() const{
    return grades;
}

double Student::getGPA() const{
    return GPA;
}

// setters
void Student::setMajor(string major){
    this -> major = major;
}

void Student::setLevelOfStudy(string levelOfStudy){
    // check if the level of study string is either Bachelor's, Master's or PhD case insensitive
    if(strcmpi(levelOfStudy.c_str(), "bachelor's") == 0)
        this -> levelOfStudy = 'b';
    else if(strcmpi(levelOfStudy.c_str(), "master's") == 0)
        this -> levelOfStudy = 'm';
    else if(strcmpi(levelOfStudy.c_str(), "phd") == 0)
        this -> levelOfStudy = 'p';
    else
        throw invalid_argument("Level of study must be either Bachelor's, Master's or PhD.");
}

void Student::setGPA(double gpa){
    this -> GPA = gpa;
}

Student::~Student(){}

// UniversityTeacher -------------------------------------------------
class UniversityTeacher : virtual public Person, public StaffInterface{
    const string employeeID;
    string department;
    string rank;
    int salary;
    vector<string> coursesTaught;

    static int employeeIDs;

public:
    UniversityTeacher();
    UniversityTeacher(string, string, tm, string, char, string, string, string, int, vector<string>);
    UniversityTeacher(const UniversityTeacher&);
    UniversityTeacher& operator=(const UniversityTeacher&);

    virtual ostream& print(ostream&) const override;
    virtual istream& read(istream&) override;
    virtual UniversityTeacher* clone() const override;

    // utils
    virtual void addCourse(string);
    static string generateEmployeeID();

    // getters
    string getEmployeeID() const;
    string getDepartment() const;
    string getRank() const;
    int getSalary() const;
    const vector<string>& getCoursesTaught() const;

    // setters
    void setSalary(int) override;
    void setDepartment(string) override;
    void setRank(string) override;
    void setCoursesTaught(vector<string>);

    ~UniversityTeacher();
};

int UniversityTeacher::employeeIDs = 1000;

UniversityTeacher::UniversityTeacher() : employeeID(UniversityTeacher::generateEmployeeID()),
                                         department("N/A"),
                                         rank("N/A"),
                                         salary(0),
                                         coursesTaught(){}

UniversityTeacher::UniversityTeacher(string firstName, string lastName,
                                     tm dateOfBirth, string SSN,
                                     char gender, string employeeID,
                                     string department, string rank,
                                     int salary, vector<string> coursesTaught) : Person(firstName, lastName, dateOfBirth, SSN, gender),
                                                                                 employeeID(employeeID.empty() ? UniversityTeacher::generateEmployeeID() : employeeID),
                                                                                 department(department),
                                                                                 rank(rank),
                                                                                 salary(salary >= 0 ? salary : 0),
                                                                                 coursesTaught(coursesTaught){}

UniversityTeacher::UniversityTeacher(const UniversityTeacher& obj):
                                        Person(obj),
                                        employeeID(obj.employeeID),
                                        department(obj.department),
                                        rank(obj.rank),
                                        salary(obj.salary),
                                        coursesTaught(obj.coursesTaught){}

UniversityTeacher& UniversityTeacher::operator=(const UniversityTeacher& obj){
    if (this == &obj)
        return *this;

    Person::operator=(obj);
    this -> department = obj.department;
    this -> rank = obj.rank;
    this -> salary = obj.salary;
    this -> coursesTaught = obj.coursesTaught;

    return *this;
}

ostream& UniversityTeacher::print(ostream& out) const{
    // print a border around the teacher's information (ID ASCII art)
    int width = 50;
    out << ANSI_COLOR_GREEN
        << "|" <<  string(width - 2, '*') << "|\n"
        << "| " << ANSI_COLOR_RED << setw(width/2 + 2) << right << "Teacher ID" << setw(width/2 - 1) << ANSI_COLOR_GREEN << " |\n"
        << "|" <<  string(width - 2, '*') << "|\n"
        << ANSI_COLOR_RESET
        << ANSI_COLOR_GREEN << "| " << ANSI_COLOR_RESET << left << setw(width - 3) << "First Name: " + this -> getFirstName() << ANSI_COLOR_GREEN << "|\n"
        << ANSI_COLOR_GREEN << "| " << ANSI_COLOR_RESET << left << setw(width - 3) << "Last Name: " + this -> getLastName() << ANSI_COLOR_GREEN << "|\n"
        << ANSI_COLOR_GREEN << "| " << ANSI_COLOR_RESET << left << setw(width - 3) << "Date of Birth: " + this -> getDateOfBirth() << ANSI_COLOR_GREEN << "|\n"
        << ANSI_COLOR_GREEN << "| " << ANSI_COLOR_RESET << left << setw(width - 3) << "Teacher ID: " + this -> employeeID << ANSI_COLOR_GREEN << "|\n"
        << ANSI_COLOR_GREEN << "| " << ANSI_COLOR_RESET << left << setw(width - 3) << "Department: " + this -> department << ANSI_COLOR_GREEN << "|\n"
        << ANSI_COLOR_GREEN << "| " << ANSI_COLOR_RESET << left << setw(width - 3) << "Rank: " + this -> getRank() << ANSI_COLOR_GREEN << "|\n"
        << ANSI_COLOR_GREEN << "|" <<  right << setw(width) << "|\n"
        << "|" <<  string(width - 2, '*') << "|" << ANSI_COLOR_RESET << endl << endl;

    return out;
}

istream& UniversityTeacher::read(istream& in){
    string aux;
    vector<string> courses;
    Person::read(in);

    cout << "Department: ";
    getline(in, department);
    cout << "Rank: ";
    getline(in, rank);
    cout << "Salary: ";
    in >> salary;
    
    cout << "Do you want to add any course the person teaches? (Y/n): ";
    in >> aux;
    while (aux == "Y" || aux == "y"){
        cout << "Course name: ";
        in.ignore();
        getline(in, aux);
        courses.push_back(aux);
        cout << "Do you want to add another course? (Y/n): ";
        in >> aux;
    }

    in.ignore();
    return in;
}

UniversityTeacher* UniversityTeacher::clone() const{
    return new UniversityTeacher(*this);
}

// utils
void UniversityTeacher::addCourse(string course){
    this -> coursesTaught.push_back(course);
}

string UniversityTeacher::generateEmployeeID(){
    return to_string(++employeeIDs);
}

// getters
string UniversityTeacher::getEmployeeID() const{
    return this -> employeeID;
}

string UniversityTeacher::getDepartment() const{
    return this -> department;
}

string UniversityTeacher::getRank() const{
    return this -> rank;
}

int UniversityTeacher::getSalary() const{
    return this -> salary;
}

const vector<string>& UniversityTeacher::getCoursesTaught() const{
    return this -> coursesTaught;
}

// setters
void UniversityTeacher::setSalary(int salary){
    this -> salary = salary;
}

void UniversityTeacher::setDepartment(string department){
    this -> department = department;
}

void UniversityTeacher::setRank(string rank){
    this -> rank = rank;
}

void UniversityTeacher::setCoursesTaught(vector<string> coursesTaught){
    this -> coursesTaught = coursesTaught;
}

UniversityTeacher::~UniversityTeacher(){}

// StudentTeacher ----------------------------------------
class StudentTeacher : public Student, public UniversityTeacher{
    string supervisor;

public:
    StudentTeacher();
    StudentTeacher(string, string, tm, string, char,
                   string, string, char, unordered_map<string, tuple<double, int>>, double,
                   string, string, string, int, vector<string>,
                   string);
    StudentTeacher(const StudentTeacher&);
    StudentTeacher& operator=(const StudentTeacher&);

    virtual ostream& print(ostream&) const override;
    virtual istream& read(istream&) override;
    virtual StudentTeacher* clone() const override;

    // utils
    void addCourse(string) override;

    // getters
    string getSupervisor() const;

    // setters
    void setSupervisor(string);

    ~StudentTeacher();
};

StudentTeacher::StudentTeacher() : Person(), Student(), UniversityTeacher(), supervisor("N/A"){}

StudentTeacher::StudentTeacher(string firstName, string lastName, tm dateOfBirth, string SSN, char gender,
                               string studentID, string major, char levelOfStudy, unordered_map<string, tuple<double, int>> grades, double GPA,
                               string employeeID, string department, string rank, int salary, vector<string> coursesTaught,
                               string supervisor) : Person(firstName, lastName, dateOfBirth, SSN, gender),
                                                    Student(firstName, lastName, dateOfBirth, SSN, gender, studentID,
                                                            major, levelOfStudy, grades, GPA),
                                                    UniversityTeacher(firstName, lastName, dateOfBirth, SSN, gender,
                                                                      employeeID, department, rank, salary, vector<string>{}),
                                                    supervisor(supervisor)
{
    for (string course : coursesTaught){
        if (
            this->getGrades().find(course) == this->getGrades().end() || get<0>(this->getGrades().find(course)->second) < 9.5
        )
            throw invalid_argument("Student must have a grade of at least 9.5 to teach a course.");
        else
            this->UniversityTeacher::addCourse(course);
    }
}
// Student and UniversityTeacher constructors don't need the Person attributes in this context
// but I am too lazy to implement separate constructors just for this and the compiler handles it fine

StudentTeacher::StudentTeacher(const StudentTeacher& obj) : Person(obj),
                                                            Student(obj),
                                                            UniversityTeacher(obj),
                                                            supervisor(obj.supervisor){}

StudentTeacher& StudentTeacher::operator=(const StudentTeacher& obj){
    if (this == &obj)
        return *this;

    Person::operator=(obj);
    /* Student::operator=(obj); -- Might be problematic
    UniversityTeacher::operator=(obj); */

    // student part
    this -> setMajor(obj.getMajor());
    this -> setLevelOfStudy(obj.getLevelOfStudy());
    this -> setGPA(obj.getGPA());

    // university teacher part
    this -> setDepartment(obj.getDepartment());
    this -> setRank(obj.getRank());
    this -> setSalary(obj.getSalary());
    this -> setCoursesTaught(obj.getCoursesTaught());

    this -> supervisor = obj.supervisor;

    return *this;
}

ostream& StudentTeacher::print(ostream& out) const{
    // print the university ID by default
    return UniversityTeacher::print(out);
}

istream& StudentTeacher::read(istream& in){
    string aux, major;
    int salary;
    vector<tuple<double, int>> grades;
    vector<string> courses;
    Person::read(in);

    /* Student::read(in); -- Person::read(in) gets called 2 times more
    UniversityTeacher::read(in); */

    // student part
    cout << "Major: ";
    getline(in, aux);
    setMajor(aux);
    cout << "Level of study: ";
    in >> aux;
    setLevelOfStudy(aux);
    cout << "Introduce any grades? (Y/n): ";
    in >> aux;
    while (aux == "Y" || aux == "y"){
        pair<string, tuple<double, int>> grade;
        cout << "Exam name: ";
        in.ignore();
        getline(in, grade.first);
        cout << "Grade (1-10): ";
        in >> get<0>(grade.second);
        cout << "Weight: ";
        in >> get<1>(grade.second);
        addGrade(grade);
        cout << "Add another grade? (Y/n): ";
        in >> aux;
        this -> updateGPA();
    }

    in.ignore();

    // university teacher part
    cout << "Department: ";
    getline(in, aux);
    setDepartment(aux);
    cout << "Rank: ";
    getline(in, aux);
    setRank(aux);
    cout << "Salary: ";
    in >> salary;
    setSalary(salary);
    
    cout << "Do you want to add any course the person teaches? (Y/n): ";
    in >> aux;
    while (aux == "Y" || aux == "y"){
        cout << "Course name: ";
        in.ignore();
        getline(in, aux);
        courses.push_back(aux);
        cout << "Do you want to add another course? (Y/n): ";
        in >> aux;
    }

    cout << "Supervisor: ";
    in.ignore();
    getline(in, this -> supervisor);

    return in;
}

StudentTeacher* StudentTeacher::clone() const{
    return new StudentTeacher(*this);
}

// utils
void StudentTeacher::addCourse(string course){
    if (
        this -> getGrades().find(course) == this -> getGrades().end() 
        || get<0>(this -> getGrades().find(course) -> second) < 9.5
    )
        throw invalid_argument("Student must have a grade of at least 9.5 to teach a course.");
    
    this -> UniversityTeacher::addCourse(course);
}

// getters
string StudentTeacher::getSupervisor() const{
    return supervisor;
}

// setters
void StudentTeacher::setSupervisor(string supervisor){
    this -> supervisor = supervisor;
}

StudentTeacher::~StudentTeacher(){}

// University -------------------------------------
class University : public IOInterface{
    string name;
    list<Person*> personnelList;
    vector<string> activeDepartments;

public:
    University();
    University(string, list<Person*>, vector<string>);
    University(const University&);
    University& operator=(const University&);
    University operator+(const Person*) const;

    void addPerson(const Person* pers);
    void deletePerson(string identifier);
    void modifyPerson(string identifier, const Person* newPerson);

    ostream& print(ostream&) const override;
    istream& read(istream&) override;

    // student utilities
    void changeMajor(string studentID, string newMajor);
    void changeLevelOfStudy(string studentID, string newLevelOfStudy);
    void addGrade(string studentID, pair<string, tuple<double, int>> grade);

    // teacher utilities
    void changeDepartment(string employeeID, string newDepartment);
    void changeRank(string employeeID, string newRank);
    void changeSalary(string employeeID, int newSalary);
    void addCourse(string employeeID, string course);
    void removeCourse(string employeeID, string course);
    void changeSupervisor(string employeeID, string supervisor);

    // getters
    string getName() const;
    list<const Person*> getPersonnelList() const;
    const vector<string>& getActiveDepartments() const;
    const Person* getPersonByIdentifier(string idetifier) const;

    // setters
    void setName(string);
    void setPersonnelList(const list<Person*>&);
    void setActiveDepartments(vector<string>);

    ~University();
};

University::University():name("N/A"){}

University::University(string name, list<Person*> personnelList, vector<string> activeDepartments):
    name(name), activeDepartments(activeDepartments){
        for (Person* pers : personnelList)
            this -> personnelList.push_back(pers -> clone());
    }

University::University(const University& obj) : name(obj.name), activeDepartments(obj.activeDepartments){
    for (Person* pers : obj.personnelList)
        this -> personnelList.push_back(pers -> clone());
}

University& University::operator=(const University& obj){
    if (this == &obj)
        return *this;

    this -> name = obj.name;
    this -> activeDepartments = obj.activeDepartments;

    for (Person* pers : this -> personnelList)
        delete pers;
    this -> personnelList.clear();

    for (Person* pers : obj.personnelList)
        this -> personnelList.push_back(pers -> clone());

    return *this;
}

University University::operator+(const Person* pers) const{
    University aux(*this);
    aux.addPerson(pers);
    return aux;
}

University operator+(const Person* pers, const University& univ){
    return univ + pers;
}

void University::addPerson(const Person* pers){
    this -> personnelList.push_back(pers -> clone());
}

void University::deletePerson(string identifier){
    /* Person* pers = const_cast<Person*>(getPersonByIdentifier(identifier));
    this -> personnelList.remove(pers);
    delete pers;
    return; */

    for (Person* pers : this -> personnelList){
        if (pers -> getSSN() == identifier){
            delete pers;
            this -> personnelList.remove(pers);
            return;
        }
        if (dynamic_cast<Student*>(pers) && dynamic_cast<Student*>(pers) -> getStudentID() == identifier){
            delete pers;
            this -> personnelList.remove(pers);
            return;
        }
        if (dynamic_cast<UniversityTeacher*>(pers) && dynamic_cast<UniversityTeacher*>(pers) -> getEmployeeID() == identifier){
            delete pers;
            this -> personnelList.remove(pers);
            return;
        }
    }
    throw invalid_argument("Person not found.");
}

void University::modifyPerson(string identifier, const Person* newPerson) {
    for (Person*& pers : personnelList) {
        if (pers -> getSSN() == identifier){
            delete pers;
            pers = newPerson -> clone();
            return;
        }
        if (dynamic_cast<Student*>(pers) && dynamic_cast<Student*>(pers) -> getStudentID() == identifier){
            delete pers;
            pers = newPerson -> clone();
            return;
        }
        if (dynamic_cast<UniversityTeacher*>(pers) && dynamic_cast<UniversityTeacher*>(pers) -> getEmployeeID() == identifier){
            delete pers;
            pers = newPerson -> clone();
            return;
        }
    }
    throw invalid_argument("Person not found.");
}

ostream& University::print(ostream& out) const{
    /* int width = 66; -- abandoned border style
    out << ANSI_COLOR_GREEN << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ " << "University" << " +-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n"
        << ANSI_COLOR_RESET 

        << ANSI_COLOR_GREEN << "| " << ANSI_COLOR_RESET
        << left << setw(width) <<  "Name: " + this -> name
        << ANSI_COLOR_GREEN << " |" << ANSI_COLOR_RESET << endl

        << ANSI_COLOR_GREEN << "+ " << ANSI_COLOR_RESET
        << left << setw(width) << "Active departments: "
        << ANSI_COLOR_GREEN << " +" << ANSI_COLOR_RESET << endl;

        char border = '|';
        for (string department : activeDepartments){
            out << ANSI_COLOR_GREEN << border << " " << ANSI_COLOR_RESET
            << left << setw(width) << "  " + department
            << ANSI_COLOR_GREEN << " " << border << ANSI_COLOR_RESET << endl;
            if (border == '|')
                border = '+';
            else
                border = '|';
        } */

    out << ANSI_COLOR_GREEN << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ " << "University" << " +-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n"
        << ANSI_COLOR_RESET 
        << "  Name: " + this -> name << endl
        << "  Active departments: \n";
    for (string department : activeDepartments){
        out << "     " << department << endl;
    }
    out << "  Number of members: " << personnelList.size() << endl;

    return out;
}

istream& University::read(istream& in){
    string aux;

    cout << "Name: ";
    getline(in, name);
    cout << "Add an active department: ";
    getline(in, aux);
    activeDepartments.push_back(aux);
    cout << "Do you want to add another department? (Y/n): ";
    in >> aux;
    while (aux == "Y" || aux == "y"){
        cout << "Department name: ";
        in.ignore();
        getline(in, aux);
        activeDepartments.push_back(aux);
        cout << "Do you want to add another department? (Y/n): ";
        in >> aux;
    }

    cout << "Do you want to add any members to the university? (Y/n): ";
    in >> aux;
    while (aux == "Y" || aux == "y"){
        cout << "What type of person do you want to add? (Student/Teacher/StudentTeacher): ";
        in >> aux;
        in.ignore();
        if (aux == "Student"){
            Student s;
            in >> s;
            this -> addPerson(&s);
        }
        else if (aux == "Teacher"){
            UniversityTeacher t;
            in >> t;
            this -> addPerson(&t);
        }
        else if (aux == "StudentTeacher"){
            StudentTeacher st;
            in >> st;
            this -> addPerson(&st);
        }
        else
            cout << "Invalid type of person.\n";

        cout << "Do you want to add another member? (Y/n): ";
        in >> aux;
    }
    return in;
}

// student utilities
void University::changeMajor(string studentID, string newMajor){
    for (Person* pers : this -> personnelList){
        if (dynamic_cast<Student*>(pers) && dynamic_cast<Student*>(pers) -> getStudentID() == studentID){
            dynamic_cast<Student*>(pers) -> setMajor(newMajor);
            return;
        }
    }
    throw invalid_argument("Person not found.");
}

void University::changeLevelOfStudy(string studentID, string newLevelOfStudy){
    for (Person* pers : this -> personnelList){
        if (dynamic_cast<Student*>(pers) && dynamic_cast<Student*>(pers) -> getStudentID() == studentID){
            dynamic_cast<Student*>(pers) -> setLevelOfStudy(newLevelOfStudy);
            return;
        }
    }
    throw invalid_argument("Person not found.");
}

void University::addGrade(string studentID, pair<string, tuple<double, int>> grade){
    for (Person* pers : this -> personnelList){
        if (dynamic_cast<Student*>(pers) && dynamic_cast<Student*>(pers) -> getStudentID() == studentID){
            dynamic_cast<Student*>(pers) -> addGrade(grade);
            return;
        }
    }
    throw invalid_argument("Person not found.");
}

// teacher utilities
void University::changeDepartment(string employeeID, string newDepartment){
    if (find(activeDepartments.begin(), activeDepartments.end(), newDepartment) == activeDepartments.end())
        throw invalid_argument("Department not found.");
    for (Person* pers : this -> personnelList){
        if (dynamic_cast<UniversityTeacher*>(pers) && dynamic_cast<UniversityTeacher*>(pers) -> getEmployeeID() == employeeID){
            dynamic_cast<UniversityTeacher*>(pers) -> setDepartment(newDepartment);
            return;
        }
    }
    throw invalid_argument("Person not found.");
}

void University::changeRank(string employeeID, string newRank){
    for (Person* pers : this -> personnelList){
        if (dynamic_cast<UniversityTeacher*>(pers) && dynamic_cast<UniversityTeacher*>(pers) -> getEmployeeID() == employeeID){
            dynamic_cast<UniversityTeacher*>(pers) -> setRank(newRank);
            return;
        }
    }
    throw invalid_argument("Person not found.");
}

void University::changeSalary(string employeeID, int newSalary){
    for (Person* pers : this -> personnelList){
        if (dynamic_cast<UniversityTeacher*>(pers) && dynamic_cast<UniversityTeacher*>(pers) -> getEmployeeID() == employeeID){
            dynamic_cast<UniversityTeacher*>(pers) -> setSalary(newSalary);
            return;
        }
    }
    throw invalid_argument("Person not found.");
}

void University::addCourse(string employeeID, string course){
    for (Person* pers : this -> personnelList){
        if (dynamic_cast<UniversityTeacher*>(pers) && dynamic_cast<UniversityTeacher*>(pers) -> getEmployeeID() == employeeID){
            dynamic_cast<UniversityTeacher*>(pers) -> addCourse(course);
            return;
        }
    }
    throw invalid_argument("Person not found.");
}

void University::removeCourse(string employeeID, string course){
    for (Person* pers : this -> personnelList){
        if (dynamic_cast<UniversityTeacher*>(pers) && dynamic_cast<UniversityTeacher*>(pers) -> getEmployeeID() == employeeID){
            vector<string> courses = dynamic_cast<UniversityTeacher*>(pers) -> getCoursesTaught();
            auto it = find(courses.begin(), courses.end(), course);
            if (it != courses.end())
                courses.erase(it);
            else
                throw invalid_argument("Course not found.");
            dynamic_cast<UniversityTeacher*>(pers) -> setCoursesTaught(courses);
            return;
        }
    }
    throw invalid_argument("Person not found.");
}

void University::changeSupervisor(string employeeID, string supervisor){
    for (Person* pers : this -> personnelList){
        if (dynamic_cast<StudentTeacher*>(pers) && dynamic_cast<StudentTeacher*>(pers) -> getEmployeeID() == employeeID){
            dynamic_cast<StudentTeacher*>(pers) -> setSupervisor(supervisor);
            return;
        }
    }
    throw invalid_argument("Person not found.");
}

// getters
string University::getName() const{
    return this -> name;
}

list<const Person*> University::getPersonnelList() const{
    list<const Person*> constPersonnelList;
    for (Person* pers : this -> personnelList){
        constPersonnelList.push_back(pers);
    }
    return constPersonnelList;
}

const vector<string>& University::getActiveDepartments() const{
    return this -> activeDepartments;
}

// setters
void University::setName(string name){
    this -> name = name;
}

void University::setPersonnelList(const list<Person*>& personnelList){ // just trust the function to not modify the
    for (Person* pers : this -> personnelList)                         // Person pointers i guess
        delete pers;
    this -> personnelList.clear();

    for (Person* pers : personnelList)
        this -> personnelList.push_back(pers -> clone());    
}

void University::setActiveDepartments(vector<string> activeDepartments){
    this -> activeDepartments = activeDepartments;
}

const Person* University::getPersonByIdentifier(string identifier) const{
    for (Person* pers : this -> personnelList){
        if (pers -> getSSN() == identifier)
            return pers;
        if (dynamic_cast<Student*>(pers) && dynamic_cast<Student*>(pers) -> getStudentID() == identifier)
            return pers;
        if (dynamic_cast<UniversityTeacher*>(pers) && dynamic_cast<UniversityTeacher*>(pers) -> getEmployeeID() == identifier)
            return pers;
    }
    return NULL;
}

University::~University(){
    for (Person* pers : personnelList)
        delete pers;
}

University getDemoUni(){
    // generate a demo university
    University FMI("Facultatea de Matematica si Informatica", list<Person*>{}, {"Computer Science", "Mathematics", "CTI"});

    // person 1
    Person* aux = new UniversityTeacher("Kostake", "Teli", {0}, "", 'M', "", "Computer Science", "Conferentiar", 10000, vector<string>{"Geometrie"});
    aux -> setDateOfBirth("15/11/1950");
    FMI.addPerson(aux);
    delete aux;

    // person 2
    aux = new Student("Ion", "Popescu", {0}, "", 'm', "", "Computer Science", 'b', unordered_map<string, tuple<double, int>>{{"Algebra", {5, 3}}, {"OOP", {8, 6}}}, 5);
    dynamic_cast<Student*>(aux) -> updateGPA();
    FMI.addPerson(aux);
    delete aux;


    // person 3
    aux = new StudentTeacher("Mihai", "Popescu", {0}, "", 'm', "", "Computer Science", 'b', unordered_map<string, tuple<double, int>>{{"WebTech", {10, 4}}, {"Theory of Computation", {7.8, 3}}, {"Data Structures", {10, 3}}}, 10, "", "Computer Science", "Asistent", 10000, vector<string>{"Data Structures"}, "Kostake Teli");
    dynamic_cast<StudentTeacher*>(aux) -> updateGPA();
    FMI.addPerson(aux);
    delete aux;

    // person 4
    aux = new UniversityTeacher("Jhon", "Doe", {0}, "", 'm', "", "Mathematics", "Profesor", 15000, vector<string>{"Geometry"});
    FMI.addPerson(aux);
    delete aux;

    return FMI;
}

int normalMenuSpeed = 35;
int fastMenuSpeed = 15;
bool skipMenu = false;
void slowPrint(string s, int ms = normalMenuSpeed, bool resetMenuSkip = false){
    // slowly prints characters to the console
    // to give the impression of someone typing
    // it also contains logic for skipping over the slow print

    if (skipMenu)  // skip the slow print
        ms = 0;

    for (char c : s){
        if (_kbhit() && _getch() == '\r')   // if the user presses enter, skip the slow print (until the next menu is displayed)
            ms = 0, skipMenu = true;

        cout << c << flush;
        if (ms < 3 && c != '\n')
            continue;
        this_thread::sleep_for(chrono::milliseconds(ms));
    }

    // reset the skip_menu flag
    if (resetMenuSkip)
        skipMenu = false;
}

void continuePrompt(){
    // waits for the user to press any key to continue
    slowPrint("Press any key to continue...", fastMenuSpeed, true);
    _getch();
}

// This function refreshes the console
// and displays a visual element
void refreshConsole(){
    system("cls");    // no remote access to this program so it should be safe enough
    cout << ANSI_COLOR_GREEN;
    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";
    cout << "|                             |\n";
    cout << "+ Diamond Inheritance Problem +\n";
    cout << "|                             |\n";
    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n\n";
    cout << ANSI_COLOR_RESET;
}

int main(int argc, char* argv[]){
    University uni;  // our university
    char charChoice;

    // take --fast as an argument to speed up the menu
    if (argc >= 2 && strcmp(argv[1], "--fast") == 0)
        normalMenuSpeed = 3, fastMenuSpeed = 2;

    cout << ANSI_COLOR_RESET;
    refreshConsole();

    // WELCOME MENU
    slowPrint("Welcome to the setup menu!\n");
    slowPrint("This program addresses the diamond inheritance problem by handling a university's list of members.\n\n");

    slowPrint("How would you like to start?\n");
    slowPrint("1. Start with a preloaded University (Recommended)\n", fastMenuSpeed);
    slowPrint("2. Introduce the details of the university manually\n", fastMenuSpeed, true);
    charChoice = _getch();
    
    refreshConsole();
    
    if (charChoice == '1'){
        // option 1: preloaded uni
        uni = getDemoUni();
        slowPrint("A demo university has been created:\n\n");
        cout << uni << endl << endl;
        
        continuePrompt();
        refreshConsole();
    }
    else if (charChoice == '2'){
        // option 2: introduce the details of the uni manually
        slowPrint("Please introduce the details of the university.\n\n");
        cin >> uni;
        slowPrint("\nThe university has been created:\n\n");
        cout << uni << endl << endl;

        continuePrompt();
        refreshConsole();
    }
    else {
        cout << "Invalid choice!";
        return 1;
    }

    // MAIN MENU
    while (true){
        slowPrint("What would you like to do?\n\n");
        slowPrint("1. View the university\n", fastMenuSpeed);
        slowPrint("2. Add a person to the university\n", fastMenuSpeed);
        slowPrint("3. Print the details of a person\n", fastMenuSpeed);
        slowPrint("4. Print all people\n", fastMenuSpeed);
        slowPrint("5. Get student related info\n", fastMenuSpeed);
        slowPrint("6. Get teacher related info\n", fastMenuSpeed);
        slowPrint("7. Delete a person\n", fastMenuSpeed);
        slowPrint("8. Modify a person\n", fastMenuSpeed);
        slowPrint("9. Exit the application\n", fastMenuSpeed);

        slowPrint("", 0, true);     // reset menu skip speed

        charChoice = _getch();
        refreshConsole();
        string aux;
        switch(charChoice){
            case '1':{
                // view the university
                cout << uni << endl;
                break;
            }
            case '2':{
                // add a person to the university
                slowPrint("What type of person would you like to add? (Student/Teacher/StudentTeacher): ", normalMenuSpeed, true);
                cin >> aux;
                cin.ignore();
                if (strcmpi(aux.c_str(), "Student") == 0){
                    Student s;
                    cin >> s;
                    uni = uni + &s;
                    //uni.addPerson(&s);
                    slowPrint("\nThe person has been added to the university:\n\n", fastMenuSpeed);
                    s.Person::print(cout);
                    cout << s;
                }
                else if (strcmpi(aux.c_str(), "Teacher") == 0){
                    UniversityTeacher t;
                    cin >> t;
                    uni = uni + &t;
                    //uni.addPerson(&t);
                    slowPrint("\nThe person has been added to the university:\n\n", fastMenuSpeed);
                    t.Person::print(cout);
                    cout << t;
                }
                else if (strcmpi(aux.c_str(), "StudentTeacher") == 0){
                    StudentTeacher st;
                    cin >> st;
                    uni = uni + &st;
                    //uni.addPerson(&st);
                    slowPrint("\nThe person has been added to the university:\n\n", fastMenuSpeed);
                    st.Person::print(cout);
                    st.Student::print(cout);
                    cout << st;
                }
                else
                    slowPrint("Invalid choice. Please try again...\n", normalMenuSpeed, true);

                break;
            }
            case '3':{
                // print the details of a person
                slowPrint("Introduce the identifier of the person (SSN, StudentID or Employee ID): ", normalMenuSpeed, true);
                cin >> aux;
                const Person* pers = uni.getPersonByIdentifier(aux);
                if (pers != NULL){
                    pers -> Person::print(cout);  // print ID
                    if (dynamic_cast<const StudentTeacher*>(pers))
                        dynamic_cast<const StudentTeacher*>(pers) -> Student::print(cout);
                    cout << *pers;                // print Student ID or Employee ID
                }
                else 
                    slowPrint("Person not found.\n", normalMenuSpeed, true);

                break;
            }
            case '4':{
                // print all people
                slowPrint("The university has the following members:\n\n", fastMenuSpeed, true);
                int i = 0;
                for (const Person* pers : uni.getPersonnelList()){
                    cout << ANSI_COLOR_GREEN << "=== PERSON " << ++i << " ===" << ANSI_COLOR_RESET << endl;
                    cout << "Full name: " << pers -> getFullName() << endl;
                    cout << "Status: ";
                    if (dynamic_cast<const StudentTeacher*>(pers))
                        cout << "Student Teacher\n" 
                             << "Identifiers: \n"
                             << "  SSN: " << pers -> getSSN() << endl 
                             << "  Student ID: " << dynamic_cast<const StudentTeacher*>(pers) -> getStudentID() << endl
                             << "  Employee ID: " << dynamic_cast<const StudentTeacher*>(pers) -> getEmployeeID() << endl;
                    else if (dynamic_cast<const UniversityTeacher*>(pers))
                        cout << "Teacher\n" 
                             << "Identifiers: \n"
                             << "  SSN: " << pers -> getSSN() << endl 
                             << "  Employee ID: " << dynamic_cast<const UniversityTeacher*>(pers) -> getEmployeeID() << endl;
                    else if (dynamic_cast<const Student*>(pers))
                        cout << "Student\n" 
                             << "Identifiers: \n"
                             << "  SSN: " << pers -> getSSN() << endl 
                             << "  Student ID: " << dynamic_cast<const Student*>(pers) -> getStudentID() << endl;
                    cout << endl;
                }
                break;
            }
            case '5':{
                // get student related info
                slowPrint("Introduce the student's identifier (Student ID): ", normalMenuSpeed, true);
                cin >> aux;
                const Student* s = dynamic_cast<const Student*>(uni.getPersonByIdentifier(aux));
                if (s != NULL){
                    s -> Student::print(cout);
                    cout << "GPA: " << s -> getGPA() << endl;
                    cout << "Grades: \n";
                    for (auto it : s -> getGrades()){
                        cout << "  " << it.first << ": " << get<0>(it.second) << " (Weight: " << get<1>(it.second) << ")\n";
                    }
                }
                else 
                    slowPrint("Student not found.\n", normalMenuSpeed, true);

                break;
            }
            case '6':{
                // get teacher related info
                slowPrint("Introduce the teacher's identifier (Employee ID): ", normalMenuSpeed, true);
                cin >> aux;
                const UniversityTeacher* t = dynamic_cast<const UniversityTeacher*>(uni.getPersonByIdentifier(aux));
                if (t){
                    t -> UniversityTeacher::print(cout);
                    cout << "Department: " << t -> getDepartment() << endl;
                    cout << "Rank: " << t -> getRank() << endl;
                    cout << "Salary: " << t -> getSalary() << endl;
                    cout << "Courses taught: ";
                    for (string course : t -> getCoursesTaught()){
                        if (course != t -> getCoursesTaught().back())
                            cout << course << ", ";
                        else
                            cout << course << endl;
                    }

                    // if they're a student teacher
                    if (dynamic_cast<const StudentTeacher*>(t)){
                        cout << "Supervisor: " << dynamic_cast<const StudentTeacher*>(t) -> getSupervisor() << endl;
                    }
                }
                else 
                    slowPrint("Teacher not found.\n", normalMenuSpeed, true);

                break;
            }
            case '7':{
                // delete a person
                slowPrint("Introduce the identifier of the person you want to delete (SSN, StudentID or Employee ID): ", normalMenuSpeed, true);
                cin >> aux;
                const Person* pers = uni.getPersonByIdentifier(aux);

                if (pers){
                    slowPrint("Are you sure you want to delete the following person's data?\n", normalMenuSpeed, true);
                    cout << *pers;

                    slowPrint("Do you want to proceed? (Y/n): ", normalMenuSpeed, true);
                    cin >> charChoice;
                    if (charChoice == 'Y' || charChoice == 'y'){
                        uni.deletePerson(aux);
                        slowPrint("Person deleted.\n", normalMenuSpeed, true);
                    }
                    else
                        slowPrint("Operation cancelled.\n", normalMenuSpeed, true);
                }
                else
                    slowPrint("Person not found.\n", normalMenuSpeed, true);

                break;
            }
            case '8':{
                // modify a person
                slowPrint("What would you like to modify?\n", fastMenuSpeed);
                slowPrint("1. Change the major of a student\n", fastMenuSpeed);
                slowPrint("2. Change the level of study of a student\n", fastMenuSpeed);
                slowPrint("3. Add a grade to a student\n", fastMenuSpeed);
                slowPrint("4. Change the department of a teacher\n", fastMenuSpeed);
                slowPrint("5. Change the rank of a teacher\n", fastMenuSpeed);
                slowPrint("6. Change the salary of a teacher\n", fastMenuSpeed);
                slowPrint("7. Add a course to a teacher\n", fastMenuSpeed);
                slowPrint("8. Remove a course from a teacher\n", fastMenuSpeed);
                slowPrint("9. Change the supervisor of a student teacher\n", fastMenuSpeed);
                slowPrint("0. Exit the menu\n", fastMenuSpeed, true);

                charChoice = _getch();
                refreshConsole();

                // SECONDARY MENU
                switch (charChoice){
                    case '1':{
                        // change the major of a student
                        slowPrint("Introduce the student's ID: ", normalMenuSpeed, true);
                        cin >> aux;
                        slowPrint("Introduce the new major: ", normalMenuSpeed, true);
                        string newMajor;
                        cin.ignore();
                        getline(cin, newMajor);
                        try{
                            uni.changeMajor(aux, newMajor);
                            slowPrint("Major changed.\n", normalMenuSpeed, true);
                        }
                        catch(const invalid_argument& e){
                            slowPrint(e.what(), normalMenuSpeed, true);
                            cout << endl;
                        }

                        break;
                    }
                    case '2':{
                        // change the level of study of a student
                        slowPrint("Introduce the student's identifier (Student ID): ", normalMenuSpeed, true);
                        cin >> aux;
                        slowPrint("Introduce the new level of study (Bachelor's, Master's or PhD): ", normalMenuSpeed, true);
                        string newLevelOfStudy;
                        cin >> newLevelOfStudy;
                        try{
                            uni.changeLevelOfStudy(aux, newLevelOfStudy);
                            slowPrint("Level of study changed.\n", normalMenuSpeed, true);
                        }
                        catch(const invalid_argument& e){
                            slowPrint(e.what(), normalMenuSpeed, true);
                            cout << endl;
                        }

                        break;
                    }
                    case '3':{
                        // add a grade to a student
                        slowPrint("Introduce the student's identifier (Student ID): ", normalMenuSpeed, true);
                        cin >> aux;
                        slowPrint("Introduce the exam name: ", normalMenuSpeed, true);
                        string examName;
                        cin.ignore();
                        getline(cin, examName);
                        slowPrint("Introduce the grade (1-10): ", normalMenuSpeed, true);
                        double grade;
                        cin >> grade;
                        slowPrint("Introduce the weight: ", normalMenuSpeed, true);
                        int weight;
                        cin >> weight;
                        try{
                            uni.addGrade(aux, {examName, {grade, weight}});
                            slowPrint("Grade added.\n", normalMenuSpeed, true);
                        }
                        catch(const invalid_argument& e){
                            slowPrint(e.what(), normalMenuSpeed, true);
                            cout << endl;
                        }

                        break;
                    }
                    case '4':{
                        // change the department of a teacher
                        slowPrint("Introduce the teacher's identifier (Employee ID): ", normalMenuSpeed, true);
                        cin >> aux;
                        slowPrint("Introduce the new department: ", normalMenuSpeed, true);
                        string newDepartment;
                        cin.ignore();
                        getline(cin, newDepartment);
                        try{
                            uni.changeDepartment(aux, newDepartment);
                            slowPrint("Department changed.\n", normalMenuSpeed, true);
                        }
                        catch(const invalid_argument& e){
                            slowPrint(e.what(), normalMenuSpeed, true);
                            cout << endl;
                        }

                        break;
                    }
                    case '5':{
                        // change the rank of a teacher
                        slowPrint("Introduce the teacher's identifier (Employee ID): ", normalMenuSpeed, true);
                        cin >> aux;
                        slowPrint("Introduce the new rank: ", normalMenuSpeed, true);
                        string newRank;
                        cin.ignore();
                        getline(cin, newRank);
                        try{
                            uni.changeRank(aux, newRank);
                            slowPrint("Rank changed.\n", normalMenuSpeed, true);
                        }
                        catch(const invalid_argument& e){
                            slowPrint(e.what(), normalMenuSpeed, true);
                            cout << endl;
                        }

                        break;
                    }
                    case '6':{
                        // change the salary of a teacher
                        slowPrint("Introduce the teacher's identifier (Employee ID): ", normalMenuSpeed, true);
                        cin >> aux;
                        slowPrint("Introduce the new salary: ", normalMenuSpeed, true);
                        int newSalary;
                        cin >> newSalary;
                        try{
                            uni.changeSalary(aux, newSalary);
                            slowPrint("Salary changed.\n", normalMenuSpeed, true);
                        }
                        catch(const invalid_argument& e){
                            slowPrint(e.what(), normalMenuSpeed, true);
                            cout << endl;
                        }

                        break;
                    }
                    case '7':{
                        // add a course to a teacher
                        slowPrint("Introduce the teacher's identifier (Employee ID): ", normalMenuSpeed, true);
                        cin >> aux;
                        slowPrint("Introduce the course name: ", normalMenuSpeed, true);
                        string course;
                        cin.ignore();
                        getline(cin, course);
                        try{
                            uni.addCourse(aux, course);
                            slowPrint("Course added.\n", normalMenuSpeed, true);
                        }
                        catch(const invalid_argument& e){
                            slowPrint(e.what(), normalMenuSpeed, true);
                            cout << endl;
                        }

                        break;
                    }
                    case '8':{
                        // remove a course from a teacher
                        slowPrint("Introduce the teacher's identifier (Employee ID): ", normalMenuSpeed, true);
                        cin >> aux;
                        slowPrint("Introduce the course name: ", normalMenuSpeed, true);
                        string course;
                        cin.ignore();
                        getline(cin, course);
                        try{
                            uni.removeCourse(aux, course);
                            slowPrint("Course removed.\n", normalMenuSpeed, true);
                        }
                        catch(const invalid_argument& e){
                            slowPrint(e.what(), normalMenuSpeed, true);
                            cout << endl;
                        }

                        break;
                    }
                    case '9':{
                        // change the supervisor of a student teacher
                        slowPrint("Introduce the student teacher's identifier (Employee ID): ", normalMenuSpeed, true);
                        cin >> aux;
                        slowPrint("Introduce the new supervisor: ", normalMenuSpeed, true);
                        string supervisor;
                        cin.ignore();
                        getline(cin, supervisor);
                        try{
                            uni.changeSupervisor(aux, supervisor);
                            slowPrint("Supervisor changed.\n", normalMenuSpeed, true);
                        }
                        catch(const invalid_argument& e){
                            slowPrint(e.what(), normalMenuSpeed, true);
                            cout << endl;
                        }

                        break;
                    }
                    case '0':{
                        // exit the menu
                        refreshConsole();
                        continue;
                    }
                    default:{
                        slowPrint("Invalid choice. Returning to main menu...\n", normalMenuSpeed, true);
                        break;
                    }
                }
                break;
            }
            case '9':{
                // exit the application
                slowPrint("Exiting the application...\n", normalMenuSpeed, true);
                return 0;
            }
            default:{
                slowPrint("Invalid choice. Please try again...\n", normalMenuSpeed, true);
                break;
            }
        }
        continuePrompt();
        refreshConsole();
    }

    return 0;
}