#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include "review.h"
#include "request.h"

using namespace std;

// This is a cool hacky thing that lets me know types at compile time
// taken from https://stackoverflow.com/a/39635955
template<typename T>
void show_type_abort_helper()
{
    return __PRETTY_FUNCTION__;
}

#define show_type_abort(x) show_type_abort_helper< decltype(x) >()


int main(int argc, char* argv[]) {
    try{
        session S("postgresql:dbname=gbombreviews;user=jdog");
        if (S.is_open()) {
         cout << "Opened database successfully: " << S.engine() << endl;
        } else {
         cout << "Can't open database" << endl;
         return 1;
        }
        transaction trans(S);;
        for(int i=0; i<=288; i++){
            json::value out = RequestUserReviewsJson(100 * i).get();
            std::this_thread::sleep_for(std::chrono::seconds(1));
            auto results = out.at(U("results"));
            for(auto res: results.as_array()){
                Review rev(
                        res.at(U("score")).as_integer(),
                        res.at(U("reviewer")).as_string(),
                        res.at(U("wikiObject")).as_string()
                        );
                rev.sql_insert(S) << exec;
            }
        }
        trans.commit();
        S.close();
    } catch (const std::exception &e) {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}
