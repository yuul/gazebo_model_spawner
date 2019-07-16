#ifndef _GAZEBO_MODELSPAWNER_PLUGIN_HH_
#define _GAZEBO_MODELSPAWNER_PLUGIN_HH_

#include <gazebo/common/Plugin.hh>
#include <gazebo/common/UpdateInfo.hh>
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
  };
}

#endif
