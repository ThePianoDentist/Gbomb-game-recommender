#ifndef MODEL_H_
#define MODEL_H_

#include <map>
#include <iostream>
#include <fstream>
#include <cppdb/frontend.h>
#include <Eigen/Dense>
#include <boost/serialization/map.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

using namespace Eigen;
using std::map;
using std::string;
using std::ios;

struct Grad{
    MatrixXd Xgrad;
    MatrixXd thetaGrad;
};

struct CostGrad{
    double cost;
    Grad grad;
};

class Model{
    private:
        float learning_rate;
        int num_iterations;
        float regularization;

    public:
        int num_users;
        int num_games;
        int num_features;
        map<string, int> game_id;
        map<int, string> id_game;
        map<string, int> user_id;
        MatrixXd X;  // is X user_weights and theta game weights?
        MatrixXd theta;
        MatrixXd reviewM;
        MatrixXd binary_reviewM; // matrix of reviewers x games (0 where not reviewed. 1 where reviewed)
        void set_users(cppdb::session);
        void set_games(cppdb::session);
        void add_my_review(string, int);
        VectorXd our_predictions();
        void sort_our_predictions();
        // can practice generics here for float/int num-iterations
        void gradient_descent();
        MatrixXd grad_checkX(double);
        CostGrad cost_grad();  // can be private
        void write_matrix(const char*, const MatrixXd&);
        void read_matrix(const char*, MatrixXd&);
        void load();
        void save();

        template <typename T>
        void read_map(const char* filename, T& our_map){
           std::ifstream in(filename, ios::binary);
           boost::archive::binary_iarchive iarch(in);
           iarch >> our_map;
           in.close();
        };

        template <typename T>
        void write_map(const char* filename, const T& our_map){
           std::ofstream out(filename, ios::binary);
           boost::archive::binary_oarchive oarch(out);
           oarch << our_map;
           out.close();
        };

    Model(cppdb::session, int, int, float, float); 
    Model();
};

#endif
