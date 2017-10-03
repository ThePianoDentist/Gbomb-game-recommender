#include <cpprest/http_client.h>
#include <cpprest/json.h>

using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace std;

pplx::task<json::value> RequestUserReviewsJson(int);
