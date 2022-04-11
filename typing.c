#include "typing.h"

int getWidth(Type type){
    switch (type)
    {
        case Integer:
            return 4;  
        case Real:
            return 8;      
        default:
            break;
    }
}
