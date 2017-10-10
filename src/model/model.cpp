#include "model.h"
using namespace Eigen;
using std::cout;
using std::endl;

Model::Model(cppdb::session sess, int num_features, int num_iterations, float learning_rate, float regularization){
    this -> num_features = num_features;
    this -> num_iterations = num_iterations;
    this -> learning_rate = learning_rate;
    this -> regularization = regularization;
    this -> set_users(sess);
    this -> set_games(sess);
    this -> reviewM = MatrixXd::Constant(this -> num_games, this -> num_users, -1.0);
    this -> binary_reviewM = MatrixXd::Zero(this -> num_games, this -> num_users);
    this -> X = MatrixXd::Random(this -> num_games, this -> num_features);
    this -> theta = MatrixXd::Random(this -> num_users, this -> num_features);

    cppdb::result res = sess << "SELECT reviewer,game,score FROM review ORDER BY reviewer";
    while(res.next()){
        string user;
        string game;
        int score;  // can I have the nice one line extraction? but still use auto?
        res >> user >> game >> score;
        this -> reviewM(this -> game_id[game], this -> user_id[user]) = score;
        this -> binary_reviewM(this -> game_id[game], this -> user_id[user]) = 1;
    }
};

Model::Model(){
    this -> load();
}

void Model::load(){
    this -> read_matrix("/home/jdog/projects/cpp/gbombRecommend/data/reviewM.dat", this -> reviewM);
    this -> read_matrix("/home/jdog/projects/cpp/gbombRecommend/data/binary_reviewM.dat", this -> binary_reviewM);
    this -> read_matrix("/home/jdog/projects/cpp/gbombRecommend/data/X.dat", this -> X);
    this -> read_matrix("/home/jdog/projects/cpp/gbombRecommend/data/theta.dat", this -> theta);
    
    this -> read_map("/home/jdog/projects/cpp/gbombRecommend/data/user_id.dat", this -> user_id);
    this -> read_map("/home/jdog/projects/cpp/gbombRecommend/data/game_id.dat", this -> game_id);
}

void Model::save(){
    this -> write_matrix("/home/jdog/projects/cpp/gbombRecommend/data/reviewM.dat", this -> reviewM);
    this -> write_matrix("/home/jdog/projects/cpp/gbombRecommend/data/binary_reviewM.dat", this -> binary_reviewM);
    this -> write_matrix("/home/jdog/projects/cpp/gbombRecommend/data/X.dat", this -> X);
    this -> write_matrix("/home/jdog/projects/cpp/gbombRecommend/data/theta.dat", this -> theta);
    
    this -> write_map("/home/jdog/projects/cpp/gbombRecommend/data/user_id.dat", this -> user_id);
    this -> write_map("/home/jdog/projects/cpp/gbombRecommend/data/game_id.dat", this -> game_id);
}

CostGrad Model::cost_grad(){
    auto cost = 0.0;
    cost = 0.5 * (pow(this -> binary_reviewM.cwiseProduct(this -> X* this -> theta.transpose() - this -> reviewM).sum(), 2) + this -> regularization * (pow(this -> theta.sum(), 2) + pow(this -> X.sum(), 2)));
    cout << cost << endl;
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
    return CostGrad{cost, Grad{X_grad, theta_grad}};
};

void Model::set_users(cppdb::session sess){
    cppdb::result game_res = sess << "SELECT DISTINCT(game) FROM review;";
    auto counter = 0;
    while(game_res.next()){
        string game;
        game_res >> game;
        this -> game_id[game] = counter++;
    }
    this -> num_games = counter;
};

void Model::set_games(cppdb::session sess){
    cppdb::result user_res = sess << "SELECT DISTINCT(reviewer) FROM review;";
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
    for(int j=0; j<this -> num_iterations; j++){
        auto cg = this -> cost_grad();
        this -> X = this -> X - this -> learning_rate * cg.grad.Xgrad;
        this -> theta = this -> theta - this -> learning_rate * cg.grad.thetaGrad;
    };
    this -> save();
    return;
};

void Model::write_matrix(const char* filename, const MatrixXd& matrix){
    std::ofstream out(filename, ios::out | ios::binary | ios::trunc);
    typename MatrixXd::Index rows=matrix.rows(), cols=matrix.cols();
    out.write((char*) (&rows), sizeof(typename MatrixXd::Index));
    out.write((char*) (&cols), sizeof(typename MatrixXd::Index));
    out.write((char*) matrix.data(), rows*cols*sizeof(typename MatrixXd::Scalar) );
    out.close();
}

void Model::read_matrix(const char* filename, MatrixXd& matrix){
    std::ifstream in(filename, ios::in | ios::binary);
    typename MatrixXd::Index rows=0, cols=0;
    in.read((char*) (&rows), sizeof(typename MatrixXd::Index));
    in.read((char*) (&cols), sizeof(typename MatrixXd::Index));
    matrix.resize(rows, cols);
    in.read((char *) matrix.data(), rows*cols*sizeof(typename MatrixXd::Scalar) );
    in.close();
}

void Model::read_map(const char* filename, map<string, int>& our_map){
   std::ifstream in(filename, ios::binary);
   boost::archive::binary_iarchive iarch(in);
   iarch >> our_map;
   in.close();
};

void Model::write_map(const char* filename, const map<string, int>& our_map){
   std::ofstream out(filename, ios::binary);
   boost::archive::binary_oarchive oarch(out);
   oarch << our_map;
   out.close();
};
