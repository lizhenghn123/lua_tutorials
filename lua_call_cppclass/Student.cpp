#include "Student.h"
#include <stdio.h>

Student::Student()
{
    printf("Student ctor\n");
}

Student::~Student()
{
    printf("Student dtor\n");
}

void Student::print()
{
    printf("student name = %s, age = %d\n", name_.c_str(), age_);
}