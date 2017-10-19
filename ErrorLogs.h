#ifndef ERRORLOGS_H_INCLUDED
#define ERRORLOGS_H_INCLUDED

#include <fstream>
#include <cstdlib>

#ifdef _WIN32
    #include <windows.h>
    #define mySleep(mSec) Sleep(mSec);
#endif

#ifdef __linux__ 
    #include <unistd.h> 
    #define mySleep(mSec) usleep(mSec * 1000)
#endif

#ifdef DEBUG_PRINT
    void debugPrintFunc(std::string toPrint){
        std::cout << "DEBUG: " << toPrint << std::endl;
    }

    void errorPrintFunc(std::string toPrint, int errorCode, int severity){
        std::cout << " !!! ERROR: " << toPrint << " " << errorCode << " " << severity << std::endl;
    }

    #define DEBUG(toPrint) debugPrintFunc(std::string(__FUNCTION__) + " :: " + std::string(toPrint));
    #define ERROR_PRINT(toPrint, errorCode, severity) errorPrintFunc(toPrint, errorCode, severity);
    #define debugPrint std::cout
#else
    class NullBuffer : public streambuf{
    public:
        int overflow(int c) { 
            return c; 
        }
    }null_buffer;

    ostream null_stream(&null_buffer);

    #define DEBUG(toPrint);
    #define ERROR_PRINT(toPrint, errorCode, severity) ;
    #define debugPrint null_stream
#endif // DEBUG_PRINT

class ErrorLogs{
private:
    static std::ofstream fileOut;
    static bool fileOpenedAlready;

    static void openLogsFile(std::string logsFileName){
        if (!fileOpenedAlready && logsFileName != "") {
            fileOut.open(logsFileName.c_str());
        }
    }

public:
    static std::string logsFileName;

    static void printError(std::string msg, int errorCode, int severity){
        openLogsFile(logsFileName);

        fileOut << msg << " " << errorCode << " " << severity << std::endl;
        ERROR_PRINT( msg, errorCode, severity );

        if(severity == 0){
            mySleep(100); /// we sleep 100 ms in case some thread prints something
            fileOut.close();
            exit(0);        /// this error is very severe
        }
    }
};

bool ErrorLogs::fileOpenedAlready = false;
std::string ErrorLogs::logsFileName = "logs.txt";
std::ofstream ErrorLogs::fileOut;

#endif // ERRORLOGS_H_INCLUDED
