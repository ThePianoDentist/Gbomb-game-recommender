#include "../core/model.h"
#include <cppdb/frontend.h>
#include "user.h"

int main(int argc, char* argv[]) {
    cppdb::session sess("postgresql:dbname=gbombreviews;user=jdog");
    if (sess.is_open()) {
     cout << "Opened database successfully: " << sess.engine() << endl;
    } else {
     cout << "Can't open database" << endl;
     return 1;
    }
    Model my_model = Model(sess, 10, 10000, 0.001, 0.01);
    my_model.gradient_descent();
    my_model.sort_our_predictions();
    my_model.save();
    return 0;
}
