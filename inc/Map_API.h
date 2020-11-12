#include <iostream>
#include <assert.h>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

struct region
{
    int id;
    string title;
};

struct city
{
    int id;
    string title;
    string area;
    string region;
};
struct point
{
    float lon;
    float lat;
};
struct square
{
    point points[2];
};
struct routing
{
    float distance;
    float duration;
};

class Map_API
{
public:
    Map_API(){};
    Map_API(const Map_API &) = delete;
    Map_API &operator=(const Map_API &) = delete;
    ~Map_API(){};

    // get_countries();
    vector<region> get_regions(long country_id = 1, string q = "");
    vector<city> get_cities(long country_id = 1, long region_id = 1, string q = "");
    square get_cities_square(string city, string area, string country = "Россия");
    routing get_routing(vector<point> points[]);
    
    point geocoder_by_title(string title);
    string geocoder_by_point(point local_point);

private:
};