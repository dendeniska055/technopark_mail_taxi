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

enum order_status 
{
    PIN,
    OFFER,
    ORDER,
    FIND,
    ACCEPTED,
    WAY,
    DONE,
    CANCELED
};

class Order_Dispatcher
{
public:
    Order_Dispatcher();
    Order_Dispatcher(const Order_Dispatcher &) = delete;
    Order_Dispatcher &operator=(const Order_Dispatcher &) = delete;
    ~Order_Dispatcher(){};


    void create_order(point form, point to, long user_id); // criteria
    void cancel_order(long order_id);

    void confirm_drivers_order(long order_id, long driver_id);
    void done_order(long order_id);

    void get_status(long order_id);
private:
    void notification_geoindex_start_order(long order_id, long driver_id, long user_id);
    void notification_geoindex_stop_order(long order_id, long driver_id, long user_id);
};