#include "model.h"
using namespace Eigen;
using std::cout;
using std::endl;

Model::Model(cppdb::session sess, int num_iterations, float learning_rate, float regularization){
    this -> set_users(sess);
    this -> set_games(sess);
    this -> reviewM = MatrixXd::Constant(this -> num_games, this -> num_users, -1.0);
    this -> binary_reviewM = MatrixXd::Zero(this -> num_games, this -> num_users);
    this -> X = MatrixXd::Random(this -> num_games, this -> num_features);
    cout << (this -> X).rows() << endl;
    this -> theta = MatrixXd::Random(this -> num_users, this -> num_features);

    cppdb::result res = sess << "SELECT reviewer,game,score FROM review_copy ORDER BY reviewer";
    while(res.next()){
        string user;
        string game;
        int score;  // can I have the nice one line extraction? but still use auto?
        res >> user >> game >> score;
        this -> reviewM(this -> game_id[game], this -> user_id[user]) = score;
        this -> binary_reviewM(this -> game_id[game], this -> user_id[user]) = 1;
    }
    
};

cost_grad Model::cost(){
    auto cost = 0.0;
    //double grad;
    for(int j=0; j<this -> num_iterations; j++){

        cost = 0.5 * (pow(this -> binary_reviewM.cwiseProduct(this -> X* this -> theta.transpose() - this -> reviewM).sum(), 2) + this -> regularization * (pow(this -> theta.sum(), 2) + pow(this -> X.sum(), 2)));
        MatrixXd X_grad = MatrixXd::Zero(this -> num_games, this -> num_features);
        MatrixXd theta_grad = MatrixXd::Zero(this -> num_users, this -> num_features);
        for(int i=0; i<this -> num_games; i++){
            // How to logical index in eigen?
            auto R_row = this -> binary_reviewM.row(i).array();
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
            igl::slice(this -> theta, indices, 1, theta_dash);
            //cout << (X.row(i) * theta_dash.transpose()).rows() << endl << (X.row(i) * theta_dash.transpose()).cols() << endl;
            VectorXd Y_dash;
            VectorXd Y2 = this -> reviewM.row(i);
            igl::slice(Y2, indices, Y_dash);
            //cout << Y_dash.rows() << endl << Y_dash.cols() << endl;
            //// rofl I guess this is why people use matlab, octave or numpy!!! :D
            X_grad.row(i) = (this -> X.row(i) * theta_dash.transpose() - Y_dash.transpose()) * theta_dash + this -> regularization * this -> X.row(i);
        }

        for(int i=0; i<this -> num_users; i++){
            // How to logical index in eigen?
            auto a = this -> binary_reviewM.col(i);
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
            igl::slice(this -> X, indices, 1, X_dash);
            //cout << (X.row(i) * theta_dash.transpose()).rows() << endl << (X.row(i) * theta_dash.transpose()).cols() << endl;
            VectorXd Y_dash;
            VectorXd Y2 = this -> reviewM.col(i);
            igl::slice(Y2, indices, Y_dash);
            //cout << Y_dash.rows() << endl << Y_dash.cols() << endl;
            //// rofl I guess this is why people use matlab, octave or numpy!!! :D
            theta_grad.row(i) = (X_dash * this -> theta.transpose().col(i) - Y_dash).transpose() * X_dash + this -> regularization * this -> theta.row(i);
        }
        cout << "got theta grad" << endl;
        this -> X = this -> X - 0.001 * X_grad;
        this -> theta = this -> theta - 0.001 * theta_grad;
        cout << "Cost: " << cost << endl;
    }
    return cost_grad{cost, 0.0};
};

void Model::set_users(cppdb::session sess){
    cppdb::result game_res = sess << "SELECT DISTINCT(game) FROM review_copy;";
    auto counter = 0;
    while(game_res.next()){
        string game;
        game_res >> game;
        this -> game_id[game] = counter++;
    }
    this -> num_games = counter;
};

void Model::set_games(cppdb::session sess){
    cppdb::result user_res = sess << "SELECT DISTINCT(reviewer) FROM review_copy;";
    auto counter = 0;
    while(user_res.next()) {
        string user;
        user_res >> user;  // can this be one-lined?
        this -> user_id[user] = counter++;
    }
    this -> num_users = counter;
};

// can practice generics here for float/int num-iterations
void Model::gradient_descent(){
    cout << "hi" << endl;
    return;
};
           
