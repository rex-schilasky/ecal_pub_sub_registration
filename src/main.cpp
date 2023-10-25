#include <ecal/ecal.h>
#include <ecal/ecal_types.h>
#include <ecal/core/pb/ecal.pb.h>

#include <iostream>
#include <chrono>

#include "ecal_expmap.h"

using PubSubRegMapT = eCAL::Util::CExpMap<std::string, bool>;

int main(int argc, char **argv)
{
  // initialize eCAL API
  eCAL::Initialize(argc, argv, "monitoring registrations", eCAL::Init::None);

  // map holding the subscriber with timeout 5s
  PubSubRegMapT subscriber_registration_map;
  subscriber_registration_map.set_expiration(std::chrono::milliseconds(5000));

  // add subscriber register callback function
  auto subscriber_registration_lambda = [&subscriber_registration_map](const char* sample_, int sample_size_) {
    // remove timeout subscriber
    subscriber_registration_map.remove_deprecated();
    // parse subscriber protobuf sample
    eCAL::pb::Sample sample;
    if (sample.ParseFromArray(sample_, sample_size_))
    {
      // create unique map key
      std::string unique_entity_name = sample.topic().tname() + sample.topic().tid();
      // check for existence
      if (subscriber_registration_map.find(unique_entity_name) == subscriber_registration_map.end())
      {
        std::cout << std::endl;
        std::cout << "----------------------------------"     << std::endl;
        std::cout << "A new subscriber registration     "     << std::endl;
        std::cout << "----------------------------------"     << std::endl;
        std::cout << "TopicName : " << sample.topic().tname() << std::endl;
        std::cout << "TopicID   : " << sample.topic().tid()   << std::endl;
        std::cout << "----------------------------------"     << std::endl;
        //std::cout << sample.DebugString();
        std::cout << std::endl;
      }
      // add/update map entry
      subscriber_registration_map[unique_entity_name] = true;
    }
    };
  eCAL::Process::AddRegistrationCallback(reg_event_subscriber, subscriber_registration_lambda);

  // map holding the publisher with timeout 5s
  PubSubRegMapT publisher_registration_map;
  publisher_registration_map.set_expiration(std::chrono::milliseconds(5000));

  // add publisher register callback function
  auto publisher_registration_lambda = [&publisher_registration_map](const char* sample_, int sample_size_) {
    // remove timeout publishers
    publisher_registration_map.remove_deprecated();
    // parse publisher protobuf sample
    eCAL::pb::Sample sample;
    if (sample.ParseFromArray(sample_, sample_size_))
    {
      // create unique map key
      std::string unique_entity_name = sample.topic().tname() + sample.topic().tid();
      // check for existence
      if (publisher_registration_map.find(unique_entity_name) == publisher_registration_map.end())
      {
        std::cout << std::endl;
        std::cout << "----------------------------------"     << std::endl;
        std::cout << "A new publisher registration      "     << std::endl;
        std::cout << "----------------------------------"     << std::endl;
        std::cout << "TopicName : " << sample.topic().tname() << std::endl;
        std::cout << "TopicID   : " << sample.topic().tid()   << std::endl;
        std::cout << "----------------------------------"     << std::endl;
        //std::cout << sample.DebugString();
        std::cout << std::endl;
      }
      // add/update map entry
      publisher_registration_map[unique_entity_name] = true;
    }
    };
  eCAL::Process::AddRegistrationCallback(reg_event_publisher, publisher_registration_lambda);

  while(eCAL::Ok())
  {
    // sleep 100 ms
    eCAL::Process::SleepMS(100);
  }

  // finalize eCAL API
  eCAL::Finalize();

  return(0);
}
