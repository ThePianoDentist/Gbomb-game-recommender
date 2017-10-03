#include <map>
#include <cppdb/frontend.h>
#include <Eigen/Dense>
#include "../igl/slice.h"  // man fuck this include/installation
using namespace Eigen;

/*class Model{
    private:
        auto learning_rate;
        auto num_iterations;
        auto regularization;
        auto cost_function();

    public:
        auto num_users;
        auto num_games;
        auto num_features;
        map<string, int> game_id;
        map<string, int> user_id;
        MatrixXd X;  // is X user_weights and theta game weights?
        MatrixXd theta;
        MatrixXd reviewM;
        MatrixXd binary_reviewM; // matrix of reviewers x games (0 where not reviewed. 1 where reviewed)
        void set_users(cppdb::Session);
        void set_games(cppdb::Session);
        // can practice generics here for float/int num-iterations
        Model(cppdb::Session, int, float, float);
        void gradient_descent();
        void cost();
               
}*/
