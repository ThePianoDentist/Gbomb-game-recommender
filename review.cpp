#include "review.h"

Review::Review(int score, string reviewer, string game){
    this -> score = score;
    this -> reviewer = reviewer;
    this -> game = game;
};

int Review::num_reviews = 0;
int Review::get_score(){return score;};
string Review::get_reviewer(){return reviewer;};
string Review::get_game(){return game;};

void Review::set_score(int s){score = s;};
void Review::set_reviewer(int r){reviewer = r;};
void Review::set_game(int g){game = g;};
bool Review::is_valid(){
    // There can be some api entries with empty wiki obects
    return (!reviewer.empty() && !game.empty() && score >= 0);
};

statement Review::sql_insert(session S){
    return S.prepare("INSERT INTO review (score, reviewer, game) VALUES (?, ?, ?)") << this -> score << this -> reviewer << this -> game;
};

int Review::get_num_reviews() {return num_reviews;};
