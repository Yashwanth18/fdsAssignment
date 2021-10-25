#ifndef __SERVERTHREAD_H__
#define __SERVERTHREAD_H__

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <map>

#include "Messages.h"
#include "ServerSocket.h"

struct ExpertRequest {
	LaptopInfo laptop;
	std::promise<LaptopInfo> prom;
};

class LaptopFactory {
private:
	std::queue<std::unique_ptr<ExpertRequest>> erq;
	std::mutex erq_lock;
	std::condition_variable erq_cv;

    std :: map < int , int > customer_record ;
    std :: vector < MapOp > smr_log ;
    int last_index;

	LaptopInfo CreateRegularLaptop(LaptopOrder order, int engineer_id);
	LaptopInfo CreateCustomLaptop(LaptopOrder order, int engineer_id);
    CustomerRecord RetrieveCustomerRecord(LaptopOrder order);

    void addCustomerRecord( int customer_id,  int latest_order);
    int getCustomerRecord( int customer_id);
    void addMapOpToLog(MapOp operation);

public:
    LaptopFactory();
	void EngineerThread(std::unique_ptr<ServerSocket> socket, int id);
	void ExpertThread(int id);
};

#endif // end of #ifndef __SERVERTHREAD_H__

