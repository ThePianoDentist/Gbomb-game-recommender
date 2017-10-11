#include <map>
#include <cppdb/frontend.h>
#include <Eigen/Dense>
#include "../igl/slice.h"  // man fuck this include/installation
#include <boost/serialization/map.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

using namespace Eigen;
using std::string;
using std::map;
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
        map<string, int> user_id;
        MatrixXd X;  // is X user_weights and theta game weights?
        MatrixXd theta;
        MatrixXd reviewM;
        MatrixXd binary_reviewM; // matrix of reviewers x games (0 where not reviewed. 1 where reviewed)
        void set_users(cppdb::session);
        void set_games(cppdb::session);
        // can practice generics here for float/int num-iterations
        void gradient_descent();
        MatrixXd grad_checkX(double);
        CostGrad cost_grad();  // can be private
        void write_matrix(const char*, const MatrixXd&);
        void read_matrix(const char*, MatrixXd&);
        void write_map(const char*, const map<string, int>&);
        void read_map(const char*, map<string, int>&);
        void load();
        void save();

    Model(cppdb::session, int, int, float, float); 
    Model();
};
