#ifndef _GAZEBO_MODELSPAWNER_PLUGIN_HH_
#define _GAZEBO_MODELSPAWNER_PLUGIN_HH_

#include <unordered_map>
#include <gazebo/common/Plugin.hh>
#include <gazebo/common/UpdateInfo.hh>
#include <gazebo/transport/transport.hh>
#include <sdf/sdf.hh>

namespace gazebo
{
  class GAZEBO_VISIBLE ModelSpawner : public WorldPlugin
  {
    /// \brief Constructor.
    public: ModelSpawner();

    /// \brief Destructor.
    public: ~ModelSpawner();

    /// Load the simulation
    public: virtual void Load(physics::WorldPtr _world, sdf::ElementPtr _sdf);

    /// Update the plugin once per simulation iteration
    private: void Update(const common::UpdateInfo &_info);

    //connection pointer for storing updates
    private: event::ConnectionPtr updateConnection;

    // Note for connection to server
    private: transport::NodePtr node;

    // Publisher of factory messages
    private: transport::PublisherPtr factoryPub;

    // Map of list of msg strings to convert into message objects
    private: std::unordered_map<double, std::vector<std::string>> msgMap;
  };
}

#endif
