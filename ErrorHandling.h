#include <exception>  // terminate()
#include <iostream>   // cerr
#include <sstream>    // ostringstream

class ErrorHandling
{
    public:
        enum struct Priority
        {
            Hard,
            Soft,
        };
        static void HardTest(
            const bool condition,
            const string& message,
            const string& conditionLiteral,
            const string& file,
            const int line,
            const string& function)
        {
            test(condition, message, conditionLiteral, file, line, function, Priority::Hard);
        }
        static bool SoftTest(
            const bool condition,
            const string& message,
            const string& conditionLiteral,
            const string& file,
            const int line,
            const string& function)
        {
            return test(condition, message, conditionLiteral, file, line, function, Priority::Soft);
        }
    private:
        static bool test(
            const bool condition,
            const string& message,
            const string& conditionLiteral,
            const string& file,
            const int line,
            const string& function,
            const Priority priority)
        {
            if (!condition)
            {
                std::ostringstream fullMessage;
                fullMessage <<
                    "test failed: " << conditionLiteral << std::endl <<
                    message << std::endl <<
                    file << ":" <<
                    line << " " <<
                    function << std::endl;
                switch(priority)
                {
                    case Priority::Hard:
                        MessageBox(nullptr, fullMessage.str().c_str(), "", MB_ICONERROR);
                        std::terminate();
                        break;
                    case Priority::Soft:
                        std::cerr << fullMessage.str();
                        return false;
                        break;
                }
            }
            return true;
        }
};

#define assert(check, message) ErrorHandling::HardTest((check), (message), (#check), __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define assertSoft(check, message) ErrorHandling::SoftTest((check), (message), (#check), __FILE__, __LINE__, __PRETTY_FUNCTION__)
