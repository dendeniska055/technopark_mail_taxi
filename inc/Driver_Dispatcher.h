#include <iostream>
#include <assert.h>
#include <string>
#include <vector>

#include "Map_API.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

class Driver_Dispatcher
{
public:
    Driver_Dispatcher(){};
    Driver_Dispatcher(const Driver_Dispatcher &) = delete;
    Driver_Dispatcher &operator=(const Driver_Dispatcher &) = delete;
    ~Driver_Dispatcher(){};

    vector<long> search_drivers(point form, long order_id); // criteria

private:
    // vector<point> get_nearest_drivers(point point);
    // vector<point> get_nearest_orders(point point);
    vector<point> get_nearest_points(point point);
    void save_find_drivers(vector<long>);
    void remove_find_driver(long driver_id);
    void remove_find_drivers(long order_id);
    void request_driver(long order_id);
};