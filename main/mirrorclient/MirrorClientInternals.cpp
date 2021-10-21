/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  MirrorClientInternals.cpp
 *  @brief:  Implement mirror internals functions.
 */
#include "MirrorClientInternals.h"
#include "CPortManager.h"
#include <restclient-cpp/restclient.h>
#include <stdexcept>
#include <json/json.h>
#include <unistd.h>

static const int HTTPSuccess = 200;

/**
 * formatUrl
 *    @param host - host of spectcl
 *    @param port - Port of SpecTcl.
 *    @param domain - subchunk of the REST interface.,
 *    @return std::string - URL.
 */
static std::string
formatUrl(const char* host, int port, const char* domain)
{
    std::stringstream strPort;
    strPort << "http://" << host << ":" << port << "/spectcl/" << domain;
    std::string result = strPort.str();
    return result;
}
/**
 * Check a JSON status - should be OK else throw a runtime error with
 * the value in detail
 */
void
checkStatus(Json::Value& v)
{
    Json::Value s = v["status"];
    if (s.asString() != "OK") {
        Json::Value d = v["detail"];
        std::string msg = d.asString();
        throw std::runtime_error(msg);
    }
}
/**
 * GetMirrorList
 *    Get the list of current mirrors that are being maintained by
 *    a specific SpecTcl...see header.
 */
std::vector<MirrorInfo>
GetMirrorList(const char* host, int port)
{
    std::vector<MirrorInfo> result;
    auto uri = formatUrl(host, port, "mirror");
    RestClient::Response r = RestClient::get(uri);

    if (r.code != HTTPSuccess) {
        throw std::runtime_error(r.body);
    }
    // Now let's process the JSON:
    
    Json::Value root;
    std::stringstream data(r.body);
    data >> root;

    checkStatus(root);
    
    const Json::Value mirrorList = root["detail"];
    for (int i =0; i < mirrorList.size(); i++) {
        const Json::Value d = mirrorList[i];
        std::string host = d["host"].asString();
        std::string mem  = d["shmkey"].asString();
        
        MirrorInfo item = {host, mem};
        result.push_back(item);
    }
    
    return result;
}
/**
 * GetSpectrumSize
 *   Get the shared memory spectrum soup size.
 */
size_t
GetSpectrumSize(const char* host, int port)
{
    std::string uri = formatUrl(host, port, "shmem/size");
    RestClient::Response r = RestClient::get(uri);
    
    if (r.code != HTTPSuccess) {
        throw std::runtime_error(r.body);
    }
    //
    Json::Value root;
    std::stringstream data(r.body);
    data >> root;
    
    checkStatus(root);
    std::string strSize = root["detail"].asString();
    
    return atol(strSize.c_str());
}
/**
 * LookupPort
 *    See header - returns the port associated with a service name
 *    advertised in the DAQPort manager
 */

int
LookupPort(const char* host, const char* service, const char* user)
{
    if (!user) {
        user = getlogin();
        if (!user) {
            throw std::logic_error("Could not determine username");
        }
    }
    CPortManager pm(host);
    auto services = pm.getPortUsage();
    for (auto s : services) {
        if ((s.s_Application == service) && (s.s_User == user)) {
            return s.s_Port;
        }
    }
    // no match:
    
    throw std::logic_error("No port matches this service/username.");
}
