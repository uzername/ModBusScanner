#ifndef COMPORTMANAGER_H
#define COMPORTMANAGER_H
// http://stackoverflow.com/questions/1008019/c-singleton-design-pattern
#include <QtSerialPort/QSerialPort>
class ComPortManager
{
  public:
    static ComPortManager& Instance()
    {
        static ComPortManager s; // Guaranteed to be destroyed.
                                 // Instantiated on first use.
        return s;
    }

  private:
    ComPortManager() {
        //...
    }  // constructor unavailable
    ~ComPortManager() {
       // ...
    } // and destructor

    // copying not allowed
    ComPortManager(ComPortManager const&); // without implementation
    ComPortManager& operator= (ComPortManager const&);  // and here
};
#endif // COMPORTMANAGER_H
