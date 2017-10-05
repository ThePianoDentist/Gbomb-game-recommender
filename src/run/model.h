#include <map>
#include <cppdb/frontend.h>
#include <Eigen/Dense>
#include "../igl/slice.h"  // man fuck this include/installation
using namespace Eigen;
using std::string;
using std::map;

struct cost_grad{
    double cost;
    double grad;
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
        Model(cppdb::session, int, float, float);
        void gradient_descent();
        cost_grad cost();  // can be private

    Model(cppdb::session, int, int, float, float); 
};
