#include <iostream>
#include <map>
#include <cppdb/frontend.h>
#include "model.h"

using namespace std;
using namespace cppdb;
using namespace Eigen;


struct grad{
    VectorXd X_grad;
    VectorXd theta_grad;
};

int main(int argc, char* argv[]) {
    cppdb::session sess("postgresql:dbname=gbombreviews;user=jdog");
    if (sess.is_open()) {
     cout << "Opened database successfully: " << sess.engine() << endl;
    } else {
     cout << "Can't open database" << endl;
     return 1;
    }
    
    Model model = Model(sess, 10, 1000, 0.000001, 0.1); 
    model.gradient_descent();
    return 0;
}
