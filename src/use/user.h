#include <string>
#include <vector>
#include "../core/review.h"
#include "../core/model.h"

class User{
    public:
        string name;
        std::vector<Review> reviews;  //std array?
        Model model;
        void add_review(string, double);
        void get_recommendations();
    User(Model);
};

