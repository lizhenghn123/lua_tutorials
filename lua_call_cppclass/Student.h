#ifndef CPP_LUA_STUDENT
#define CPP_LUA_STUDENT
#include <string>

class Student
{
public:
    Student();
    ~Student();

public:    
    std::string getName() const
    {
        return name_;
    }

    void setName(std::string name)
    {
        name_ = name;
    }

    int getAge() const
    {
        return age_;
    }

    void setAge(int age)
    {
        age_ = age;
    }

    void print();
    
private:
    std::string name_;
    int age_;
};

#endif  // CPP_LUA_STUDENT