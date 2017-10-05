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

auto cofi_cost_func(MatrixXd X, MatrixXd theta, MatrixXd Y, MatrixXd R, int user_count, int game_count, int num_features, float lambda){
    double cost;
    //double grad;
    for(int j=0; j<100; j++){

        cost = 0.5 * (pow(R.cwiseProduct(X*theta.transpose() - Y).sum(), 2) + lambda * (pow(theta.sum(), 2) + pow(X.sum(), 2)));
        MatrixXd X_grad = MatrixXd::Zero(game_count, num_features);
        MatrixXd theta_grad = MatrixXd::Zero(user_count, num_features);
        for(int i=0; i<game_count; i++){
            // How to logical index in eigen?
            auto R_row = R.row(i).array();
            // If all in R_row 0 the logical indexing will fail/except
            if (R_row.sum() == 0) {
                continue;
            }
            auto indices = (R_row != 0).cast<int>();
            // blah.operator(rowIndices, colIndices)
            //int a[2] = {0, 1};
            //int b[3] = {0, 1, 2};
            //static_assert(decltype(theta)::dummy_error, "DUMP MY TYPE" );
            MatrixXd theta_dash;
            igl::slice(theta, indices, 1, theta_dash);
            //cout << (X.row(i) * theta_dash.transpose()).rows() << endl << (X.row(i) * theta_dash.transpose()).cols() << endl;
            VectorXd Y_dash;
            VectorXd Y2 = Y.row(i);
            igl::slice(Y2, indices, Y_dash);
            //cout << Y_dash.rows() << endl << Y_dash.cols() << endl;
            //// rofl I guess this is why people use matlab, octave or numpy!!! :D
            X_grad.row(i) = (X.row(i) * theta_dash.transpose() - Y_dash.transpose()) * theta_dash + lambda * X.row(i);
        }

        for(int i=0; i<user_count; i++){
            // How to logical index in eigen?
            auto a = R.col(i);
            auto R_col = a.array();
            // If all in R_col 0 the logical indexing will fail/except
            if (R_col.sum() == 0) {
                continue;
            }
            auto indices = (R_col != 0).cast<int>();
            // blah.operator(rowIndices, colIndices)
            //int a[2] = {0, 1};
            //int b[3] = {0, 1, 2};
            //static_assert(decltype(theta)::dummy_error, "DUMP MY TYPE" );
            MatrixXd X_dash;
            igl::slice(X, indices, 1, X_dash);
            //cout << (X.row(i) * theta_dash.transpose()).rows() << endl << (X.row(i) * theta_dash.transpose()).cols() << endl;
            VectorXd Y_dash;
            VectorXd Y2 = Y.col(i);
            igl::slice(Y2, indices, Y_dash);
            //cout << Y_dash.rows() << endl << Y_dash.cols() << endl;
            //// rofl I guess this is why people use matlab, octave or numpy!!! :D
            theta_grad.row(i) = (X_dash * theta.transpose().col(i) - Y_dash).transpose() * X_dash + lambda * theta.row(i);
        }
        cout << "got theta grad" << endl;
        X = X - 0.001 * X_grad;
        theta = theta - 0.001 * theta_grad;
        cout << "Cost: " << cost << endl;
    }
    return cost_grad{cost, 0.0};
}


int main(int argc, char* argv[]) {
    cppdb::session sess("postgresql:dbname=gbombreviews;user=jdog");
    if (sess.is_open()) {
     cout << "Opened database successfully: " << sess.engine() << endl;
    } else {
     cout << "Can't open database" << endl;
     return 1;
    }
    //http://cppcms.com/sql/cppdb/query.html
    
    int num_features = 10;
    result game_res = sess << "SELECT DISTINCT(game) FROM review_copy;";
    result user_res = sess << "SELECT DISTINCT(reviewer) FROM review_copy;";
    result res = sess << "SELECT reviewer,game,score FROM review_copy ORDER BY reviewer";
    //string users[user_count];
    //string games[game_count];
    map<string, int> game_id;
    map<string, int> user_id;
    int game_count;
    int user_count;
    int counter = 0;
    while(game_res.next()){
        string game;
        game_res >> game;
        game_id[game] = counter++;
    }
    game_count = counter;
    counter = 0;
    while(user_res.next()) {
        string user;
        user_res >> user;
        user_id[user] = counter++;
    }
    user_count = counter;
    MatrixXd Mscore = MatrixXd::Constant(game_count, user_count, -1.0);
    MatrixXd R = MatrixXd::Zero(game_count, user_count);
    MatrixXd X = MatrixXd::Random(game_count, num_features);
    cout << X.rows() << endl;
    MatrixXd theta = MatrixXd::Random(user_count, num_features);

    while(res.next()){
        string user;
        string game;
        int score;
        res >> user >> game >> score;
        Mscore(game_id[game], user_id[user]) = score;
        R(game_id[game], user_id[user]) = 1;
    }
    
    cost_grad out = cofi_cost_func(X, theta, Mscore, R, user_count, game_count, num_features, 0.0);
    cout << to_string(out.cost) << endl;
    cout << "test" << endl;
    return 0;
}
