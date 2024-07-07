//In case there are several files in final submission, all universal datatypes here to be accessed by all
typedef enum {type1, type2, type3} Types;

struct IPCDataStruct {
    int num;
    float fl;
    char str[30];
    Types type;
}; 