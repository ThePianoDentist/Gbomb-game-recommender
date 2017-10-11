#include <iostream>
#include <cppdb/frontend.h>
#include "model.h"

using std::cout;
using std::endl;

int main(int argc, char* argv[]) {
    cppdb::session sess("postgresql:dbname=gbombreviews;user=jdog");
    if (sess.is_open()) {
     cout << "Opened database successfully: " << sess.engine() << endl;
    } else {
     cout << "Can't open database" << endl;
     return 1;
    }
    
    Model model = Model(sess, 5, 100, 0.0001, 1); 
    model.gradient_descent();
    return 0;
}
