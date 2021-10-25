#include <iostream>
#include <memory>

#include "ServerThread.h"
#include "ServerStub.h"

LaptopFactory::LaptopFactory()
{
    last_index = -1;
}
LaptopInfo LaptopFactory::CreateRegularLaptop(LaptopOrder order, int engineer_id) {
	LaptopInfo laptop;
	laptop.CopyOrder(order);
	laptop.SetEngineerId(engineer_id);
	laptop.SetExpertId(-1);
	return laptop;
}

LaptopInfo LaptopFactory:: CreateCustomLaptop(LaptopOrder order, int engineer_id) {
	LaptopInfo laptop;
	laptop.CopyOrder(order);
	laptop.SetEngineerId(engineer_id);

	std::promise<LaptopInfo> prom;
	std::future<LaptopInfo> fut = prom.get_future();

	std::unique_ptr<ExpertRequest> req = std::unique_ptr<ExpertRequest>(new ExpertRequest);
	req->laptop = laptop;
	req->prom = std::move(prom);

	erq_lock.lock();
	erq.push(std::move(req));
	erq_cv.notify_one();
	erq_lock.unlock();

	laptop = fut.get();
	return laptop;
}


CustomerRecord LaptopFactory::RetrieveCustomerRecord(LaptopOrder order){
    std::cout<<"inside retrieving customer record"<<std::endl;
    CustomerRecord record;

    erq_lock.lock();
    int lastOrder = getCustomerRecord(order.GetCustomerId());
    erq_lock.unlock();
    record.SetLastOrder(lastOrder);
    if(lastOrder == -1){
        record.SetCustomerId(-1);
    }
    else{
        record.SetCustomerId(order.GetCustomerId());
    }
    return record;
}

void LaptopFactory:: EngineerThread(std::unique_ptr<ServerSocket> socket, int id) {
	int engineer_id = id;
	int laptop_type;
	LaptopOrder order;
	LaptopInfo laptop;
    CustomerRecord customerRecord;

	ServerStub stub;

	stub.Init(std::move(socket));

	while (true) {
		order = stub.ReceiveOrder();
		if (!order.IsValid()) {
			break;	
		}
		laptop_type = order.GetLaptopType();
		switch (laptop_type) {
			case 1:
				laptop = CreateCustomLaptop(order, engineer_id);
				break;
            case 2:
            case 3:
                customerRecord = RetrieveCustomerRecord(order);
                break;
			default:
				std::cout << "Undefined laptop type: "
					<< laptop_type << std::endl;

		}
//		stub.SendLaptop(laptop);
        if(laptop_type == 1){
            stub.SendLaptop(laptop);
        }
        else{
            stub.ReturnRecord(customerRecord);
        }
	}
}



void LaptopFactory::ExpertThread(int id) {
	std::unique_lock<std::mutex> ul(erq_lock, std::defer_lock);
	while (true) {
		ul.lock();

		if (erq.empty()) {
			erq_cv.wait(ul, [this]{ return !erq.empty(); });
		}

		auto req = std::move(erq.front());
		erq.pop();
        MapOp operation = {1, req->laptop.GetCustomerId(), req->laptop.GetOrderNumber()};
        addMapOpToLog(operation);
        last_index++;
        addCustomerRecord(req->laptop.GetCustomerId(), req->laptop.GetOrderNumber());
		ul.unlock();

		std::this_thread::sleep_for(std::chrono::microseconds(100));
		req->laptop.SetExpertId(id);
		req->prom.set_value(req->laptop);	
	}
}




void LaptopFactory::addCustomerRecord(const int customer_id, const int latest_order) {
    customer_record[customer_id] = latest_order;
}

int LaptopFactory::getCustomerRecord(const int customer_id) {
    int latest_order = -1;
    auto it = customer_record.find(customer_id);
    if(it != customer_record.end())
    {
        latest_order = it->second;
    }
    return latest_order;
}

void LaptopFactory::addMapOpToLog(MapOp operation){
    smr_log.push_back(operation);
}