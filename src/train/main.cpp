#include <iostream>
#include <cppdb/frontend.h>
#include "../core/model.h"

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
    Model model1 = Model();
    Model model = Model(sess, 10, 1000, 0.001, 1); 
    model.gradient_descent();
    return 0;
}
