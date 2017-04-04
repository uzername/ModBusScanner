#ifndef DISPLAYTYPES_H
#define DISPLAYTYPES_H
#include <QString>
//contains enum with display types of information in register
//http://stackoverflow.com/questions/1102542/how-to-define-an-enumerated-type-enum-in-c
typedef enum {udecval, sigdecval, hexval, floatval} DispType;
/*
QString enumDispTypeToString (DispType inptype) {
    switch (inptype) {
    case udecval:
       { return "UnsignedDec"; break; }
    case sigdecval:
       { return "SignedDec"; break; }
    case hexval:
       { return "Hexadecimal"; break; }
    case floatval:
       { return "Float"; break; }
    default: { return "UnsignedDec"; break; }
    }
}
DispType stringToEnumDispType(QString inpLine) {
    if (inpLine == "UnsignedDec") {
        return udecval;
    } else {
        if (inpLine == "SignedDec") {
            return sigdecval;
        } else {
            if (inpLine == "Hexadecimal") {
                return hexval;
            } else {
                if (inpLine == "Float") {
                    return floatval;
                }
            }
        }
    }
}
*/
#endif // DISPLAYTYPES_H
