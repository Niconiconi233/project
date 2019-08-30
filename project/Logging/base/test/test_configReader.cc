#include "ConfigReader.h"
#include <iostream>

using std::cout;
using std::endl;



int main()
{
    ConfigReader reader("1.conf");
    cout<<reader.getAsString("name")<<endl;
    cout<<reader.getAsInt("age")<<endl;
    cout<<reader.getAsString("ip")<<endl;
    cout<<reader.getAsInt("port")<<endl;
    cout<<reader.getAsString("large")<<endl;
    cout<<reader.getAsDouble("total")<<endl;
    cout<<reader.getAsChar("ch")<<endl;

    reader.setValue("name", "丁瑞肉比丘尼其");
    reader.setValue("port", 233);
    reader.remove("large");
}