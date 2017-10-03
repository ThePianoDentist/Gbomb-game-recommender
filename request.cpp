#include "request.h"

pplx::task<json::value> RequestUserReviewsJson(int offset){
    http_client client(U("https://www.giantbomb.com/"));

    uri_builder builder(U("/api/user_reviews/"));
    string s = "api_key=";
    s.append(getenv("GBAPI"));
    s.append("&format=json&sort=date_added:asc&field_list=score,reviewer,wikiObject&offset=");
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
};
