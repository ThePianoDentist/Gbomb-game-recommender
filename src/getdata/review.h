#include <iostream>
#include <string>
#include <cppdb/frontend.h>
using namespace std;
using namespace cppdb;

class Review{
    // Most of this fancy class stuff not really necessary
    // But Im practicing c++ so meh :P
    private:
        int score;
        string reviewer;
        string game;

        static int num_reviews;

    public:
        int get_score();
        string get_reviewer();
        string get_game();

        void set_score(int);
        void set_reviewer(int);
        void set_game(int);
        statement sql_insert(session);
        bool is_valid();
    
        static int get_num_reviews();

    Review(int, string, string);
};
