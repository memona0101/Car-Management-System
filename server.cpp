#include "httplib.h"
#include "ParkingSystem.h"
#include "Zone.h"
#include "ParkingArea.h"
#include "ParkingSlot.h"
#include "Vehicle.h"
#include "ParkingRequest.h"
#include <iostream>
#include <string>
#include <sstream>

// Helper to manual serialization of JSON
// In a real project we would use nlohmann/json, but to avoid more deps we do simple string building
std::string toJson(const std::string& key, const std::string& value) {
    return "\"" + key + "\": \"" + value + "\"";
}

std::string toJson(const std::string& key, int value) {
    return "\"" + key + "\": " + std::to_string(value);
}

// Setup the same city as main.cpp
ParkingSystem setupCity() {
    ParkingSystem ps;
    // Zone 1
    Zone z1(1);
    ParkingArea a1(101);
    a1.addSlot(ParkingSlot(1, 1));
    a1.addSlot(ParkingSlot(2, 1));
    z1.addParkingArea(a1);
    z1.addAdjacentZone(2);
    // Zone 2
    Zone z2(2);
    ParkingArea a2(201);
    a2.addSlot(ParkingSlot(3, 2));
    z2.addParkingArea(a2);
    z2.addAdjacentZone(1);
    // Zone 3
    Zone z3(3);
    ParkingArea a3(301);
    a3.addSlot(ParkingSlot(4, 3));
    z3.addParkingArea(a3);

    ps.addZone(z1);
    ps.addZone(z2);
    ps.addZone(z3);
    return ps;
}

int main() {
    ParkingSystem ps = setupCity();
    httplib::Server svr;

    std::cout << "Starting Parking Server on port 8080..." << std::endl;

    // CORS middleware
    svr.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        if (req.method == "OPTIONS") {
            res.status = 204;
            return httplib::Server::HandlerResponse::Handled;
        }
        return httplib::Server::HandlerResponse::Unhandled;
    });

    // GET /api/data - Dump entire state
    svr.Get("/api/data", [&](const httplib::Request&, httplib::Response& res) {
        std::stringstream ss;
        ss << "{ \"zones\": [";
        const auto& zones = ps.getZones();
        for(size_t i=0; i<zones.size(); ++i) {
            const auto& z = zones[i];
            ss << "{ \"id\": " << z.getZoneId() << ", \"areas\": [";
            const auto& areas = z.getParkingAreas();
            for(size_t j=0; j<areas.size(); ++j) {
                const auto& a = areas[j];
                ss << "{ \"id\": " << a.getAreaId() << ", \"slots\": [";
                const auto& slots = a.getSlots();
                for(size_t k=0; k<slots.size(); ++k) {
                    const auto& s = slots[k];
                    ss << "{ \"id\": " << s.getSlotId() 
                       << ", \"occupied\": " << (s.isOccupied() ? "true" : "false") << " }";
                    if(k < slots.size()-1) ss << ",";
                }
                ss << "] }";
                if(j < areas.size()-1) ss << ",";
            }
            ss << "] }";
            if(i < zones.size()-1) ss << ",";
        }
        ss << "], \"requests\": [";
        const auto& reqs = ps.getRequests();
        for(size_t i=0; i<reqs.size(); ++i) {
            const auto& r = reqs[i];
            ss << "{ \"id\": " << r.getRequestId()
               << ", \"vehicleId\": \"" << r.getVehicleId() << "\""
               << ", \"zoneId\": " << r.getRequestedZoneId()
               << ", \"slotId\": " << r.getAssignedSlotId()
               << ", \"state\": \"" << r.getStateString() << "\""
               << ", \"duration\": " << r.getDuration() << " }";
            if(i < reqs.size()-1) ss << ",";
        }
        ss << "] }";
        
        res.set_content(ss.str(), "application/json");
    });

    // POST /api/request - Body: vehicleId=V1&zoneId=1
    svr.Post("/api/request", [&](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_param("vehicleId") || !req.has_param("zoneId")) {
             res.status = 400;
             res.set_content("Missing params", "text/plain");
             return;
        }
        std::string vId = req.get_param_value("vehicleId");
        int zId = std::stoi(req.get_param_value("zoneId"));
        int rId = ps.requestParking(vId, zId);
        res.set_content("{\"requestId\": " + std::to_string(rId) + "}", "application/json");
    });

    // POST /api/leave - Body: requestId=1
    svr.Post("/api/leave", [&](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_param("requestId")) {
             res.status = 400; 
             return;
        }
        int rId = std::stoi(req.get_param_value("requestId"));
        bool success = ps.leaveParking(rId);
        res.set_content(success ? "{\"status\": \"left\"}" : "{\"status\": \"failed\"}", "application/json");
    });

    // POST /api/cancel - Body: requestId=1
    svr.Post("/api/cancel", [&](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_param("requestId")) {
             res.status = 400; 
             return;
        }
        int rId = std::stoi(req.get_param_value("requestId"));
        bool success = ps.cancelRequest(rId);
        res.set_content(success ? "{\"status\": \"cancelled\"}" : "{\"status\": \"failed\"}", "application/json");
    });

    // POST /api/rollback - Body: k=1
    svr.Post("/api/rollback", [&](const httplib::Request& req, httplib::Response& res) {
         int k = 1;
         if(req.has_param("k")) k = std::stoi(req.get_param_value("k"));
         ps.rollbackOperations(k);
         res.set_content("{\"status\": \"rolled_back\"}", "application/json");
    });

    svr.listen("0.0.0.0", 8080);
    return 0;
}
