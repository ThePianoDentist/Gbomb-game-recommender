#include <vector>
#include <numeric>
#include "model.h"
#include <tuple>
using namespace Eigen;
using std::cout;
using std::endl;
using std::ios;
using std::vector;
using std::iota;
using std::tuple;

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
        this -> reviewM(this -> game_id[game], this -> user_id[user]) = score - 2.5;
        this -> binary_reviewM(this -> game_id[game], this -> user_id[user]) = 1;
    }
};

Model::Model(){
    this -> load();
}

void Model::add_my_review(string game, int score){
    this->reviewM(this->game_id[game],-1) = score;
    this->binary_reviewM(this->game_id[game],-1) = 1;
}

VectorXd Model::our_predictions(){
    return (this->X * this->theta.transpose()).col(this->num_users - 1) + VectorXd::Constant(this->num_games, 2.5);
}

//https://stackoverflow.com/a/12399290/3920439
tuple<vector<double>, vector<size_t>> sort_indexes(const VectorXd &v) {

  // initialize original index locations
  //https://stackoverflow.com/a/26094702/3920439

  vector<double> vec(v.data(), v.data() + v.rows() * v.cols());
  vector<size_t> idx(vec.size());
  iota(idx.begin(), idx.end(), 0);

  // sort indexes based on comparing values in v
  sort(idx.begin(), idx.end(),
       [&v](size_t i1, size_t i2) {return v[i2] < v[i1];});

  return std::make_tuple(vec, idx);
}
 void Model::sort_our_predictions(){
    auto predictions = this->our_predictions();
    cout << "got predcitions" << endl;
    auto sorted = sort_indexes(predictions);
    auto indices = std::get<1>(sorted);
    auto values = std::get<0>(sorted);
    for(int i=0; i<10; i++){
        cout << this->id_game[indices[i]] << ": " << values[indices[i]] <<endl;
    }
}

void Model::load(){
    this -> read_matrix("/home/jdog/projects/cpp/gbombRecommend/data/reviewM.dat", this -> reviewM);
    this -> read_matrix("/home/jdog/projects/cpp/gbombRecommend/data/binary_reviewM.dat", this -> binary_reviewM);
    this -> read_matrix("/home/jdog/projects/cpp/gbombRecommend/data/X.dat", this -> X);
    this -> read_matrix("/home/jdog/projects/cpp/gbombRecommend/data/theta.dat", this -> theta);
    
    this -> read_map("/home/jdog/projects/cpp/gbombRecommend/data/user_id.dat", this -> user_id);
    this -> read_map("/home/jdog/projects/cpp/gbombRecommend/data/game_id.dat", this -> game_id);
    this -> read_map("/home/jdog/projects/cpp/gbombRecommend/data/id_game.dat", this -> id_game);
}

void Model::save(){
    this -> write_matrix("/home/jdog/projects/cpp/gbombRecommend/data/reviewM.dat", this -> reviewM);
    this -> write_matrix("/home/jdog/projects/cpp/gbombRecommend/data/binary_reviewM.dat", this -> binary_reviewM);
    this -> write_matrix("/home/jdog/projects/cpp/gbombRecommend/data/X.dat", this -> X);
    this -> write_matrix("/home/jdog/projects/cpp/gbombRecommend/data/theta.dat", this -> theta);
    
    this -> write_map("/home/jdog/projects/cpp/gbombRecommend/data/user_id.dat", this -> user_id);
    this -> write_map("/home/jdog/projects/cpp/gbombRecommend/data/game_id.dat", this -> game_id);
    this -> write_map("/home/jdog/projects/cpp/gbombRecommend/data/id_game.dat", this -> id_game);
}

MatrixXd Model::grad_checkX(double epsilon){
    // why does autoing these matrices cause big problems?
    MatrixXd X_dash_p = this -> X;
    MatrixXd X_dash_n = this -> X;
    MatrixXd tmp = MatrixXd::Zero(this -> num_games, this -> num_features);
    for(int i=0; i<this -> num_games; i++){
        for(int j=0; j<this -> num_features; j++){
            MatrixXd epsilonM = MatrixXd::Zero(this -> num_games, this -> num_features);
            epsilonM(i, j) = epsilon;
            double J_plus = 0.5 * (this -> binary_reviewM.cwiseProduct((X_dash_n + epsilonM) * this -> theta.transpose() - this -> reviewM).array().pow(2).sum() + this -> regularization * (this -> theta.array().pow(2).sum() + (X_dash_n + epsilonM).array().pow(2).sum()));
            double J_minus = 0.5 * (this -> binary_reviewM.cwiseProduct((X_dash_n - epsilonM) * this -> theta.transpose() - this -> reviewM).array().pow(2).sum() + this -> regularization * (this -> theta.array().pow(2).sum() + (X_dash_n - epsilonM).array().pow(2).sum()));
            tmp(i, j) = (J_plus - J_minus) / (2 * epsilon);
            epsilonM(i, j) = 0.0;
        }
    }
    return tmp;
}

CostGrad Model::cost_grad(){
    auto cost = 0.0;
    cost = 0.5 * (this -> binary_reviewM.cwiseProduct(this -> X* this -> theta.transpose() - this -> reviewM).array().pow(2).sum() + this -> regularization * (this -> theta.array().pow(2).sum() + this -> X.array().pow(2).sum()));
    cout << cost << endl;
    MatrixXd X_grad = MatrixXd::Zero(this -> num_games, this -> num_features);
    MatrixXd theta_grad = MatrixXd::Zero(this -> num_users, this -> num_features);
    auto tmp = this->binary_reviewM.cwiseProduct(this -> X * theta.transpose() - this -> reviewM);
    X_grad = tmp * theta + this -> regularization * this -> X;

    //auto check = this -> grad_checkX(0.0001);
    //cout << (X_grad - check).norm() / (X_grad + check).norm() << endl;
    theta_grad = tmp.transpose() * this -> X + this -> regularization * this -> theta;
    return CostGrad{cost, Grad{X_grad, theta_grad}};
};

void Model::set_games(cppdb::session sess){
    cppdb::result game_res = sess << "SELECT DISTINCT(game) FROM (SELECT game FROM review ORDER BY RANDOM()) as Q;";
    auto counter = 0;
    while(game_res.next()){
        string game;
        game_res >> game;
        this -> id_game[counter] = game;
        this -> game_id[game] = counter++;
    };
    this -> num_games = counter;
};

void Model::set_users(cppdb::session sess){
    cppdb::result user_res = sess << "SELECT DISTINCT(reviewer) FROM (SELECT reviewer FROM review ORDER BY RANDOM()) as Q;";
    auto counter = 0;
    while(user_res.next()) {
        string user;
        user_res >> user;  // can this be one-lined?
        this -> user_id[user] = counter++;
    }
    this -> user_id["me"] = counter++; // We need to add ourselves to the matrix to get our predictions
    this -> num_users = counter;
};

// can practice generics here for float/int num-iterations
void Model::gradient_descent(){
    for(int j=0; j<this -> num_iterations; j++){
        auto cg = this -> cost_grad();
        this -> X = this -> X - this -> learning_rate * cg.grad.Xgrad;
        this -> theta = this -> theta - this -> learning_rate * cg.grad.thetaGrad;
        if (j % 100 == 0){
            cout << "Iteration: " << j << endl;
            this->sort_our_predictions();
        }
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


