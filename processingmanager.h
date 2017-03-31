#ifndef COMPORTMANAGER_H
#define COMPORTMANAGER_H
// http://stackoverflow.com/questions/1008019/c-singleton-design-pattern
//#include <QtSerialPort/QSerialPort>
#include <QTimer>
class ProcessingManager
{
  public:
    static ProcessingManager& Instance() {
        static ProcessingManager s; // Guaranteed to be destroyed.
                                 // Instantiated on first use.
        return s;
    }

    bool getProcessingPerformed() {
        return processingPerformed;
    }
    void setProcessingPerformed(bool value) {
        processingPerformed = value;
    }

    void startPooling() {

    }

private:
    bool processingPerformed;

    ProcessingManager() {
        //may i call constructor?
    }  // constructor unavailable
    ~ProcessingManager() {
       // ...
    } // and destructor

    // copying not allowed
    ProcessingManager(ProcessingManager const&); // without implementation
    ProcessingManager& operator= (ProcessingManager const&);  // and here
};
#endif // COMPORTMANAGER_H
