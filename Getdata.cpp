#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/json.h>
#include <cppdb/frontend.h>
#include <chrono>
#include <thread>
#include <string>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams
using namespace std;
using namespace cppdb;

template<typename T>
void show_type_abort_helper()
{
    return __PRETTY_FUNCTION__;
}

#define show_type_abort(x) show_type_abort_helper< decltype(x) >()

class Review{
    // Most of this fancy class stuff not really necessary
    // But Im practicing c++ so meh :P
    private:
        int score;
        string reviewer;
        string game;

        static int num_reviews;

    public:
        int get_score(){return score;}
        string get_reviewer(){return reviewer;}
        string get_game(){return game;}

        void set_score(int s){score = s;}
        void set_reviewer(int r){reviewer = r;}
        void set_game(int g){game = g;}
        statement sql_insert(session);
    
        static int get_num_reviews() {return num_reviews;}


    Review(int, string, string);
};

int Review::num_reviews = 0;

Review::Review(int score, string reviewer, string game){
    this -> score = score;
    this -> reviewer = reviewer;
    this -> game = game;
};

statement Review::sql_insert(session S){
    return S.prepare("INSERT INTO review (score, reviewer, game) VALUES (?, ?, ?)") << this -> score << this -> reviewer << this -> game;
}

pplx::task<json::value> RequestJSONValueAsync(int offset){
    http_client client(U("https://www.giantbomb.com/"));

    uri_builder builder(U("/api/user_reviews/"));
    string s = "api_key=";
    s.append(getenv("GBAPI"));
    s.append("&format=json&sort=date_added:desc&field_list=score,reviewer,wikiObject&offset=");
    builder.append_query(U(s.append(to_string(offset))));
    http_request req(methods::GET);
    req.headers().set_content_type(U("application/json"));
    req.set_request_uri(builder.to_uri());
    cout << req.absolute_uri().to_string() << endl;
    return client.request(req)
    
    .then([](http_response response) -> pplx::task<json::value>{
        if (response.status_code() == status_codes::OK){
            cout << "status code ok" << endl;
            response.headers().set_content_type(U("application/json"));
            return response.extract_json();
        }
        
        cout << "Something went wrong. Returning empty json" << endl; 
        return pplx::task_from_result(json::value());
    })
    
    .then([](pplx::task<json::value> previousTask) -> json::value{
        try{
            return previousTask.get();
        }
        catch (const http_exception& e)
        {
            // Print error.
            ostringstream ss;
            ss << e.what() << endl;
            cout << ss.str();
            throw;
        }
    });
}

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
        for(int i=0; i<=300; i++){
            json::value out = RequestJSONValueAsync(100 * i).get();
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
