#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

#include <ignition/math/Pose3.hh>
#include <gazebo/common/common.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/physics/World.hh>
#include <gazebo/transport/transport.hh>

#include "ModelSpawner.hh"

using namespace gazebo;

GZ_REGISTER_WORLD_PLUGIN(ModelSpawner)

/////////////////////////////////////////////////
ModelSpawner::ModelSpawner(): msgMap(std::unordered_map<double, std::vector<std::string>>())
{
  //this->msgMap = std::unordered_map<double, std::vector<std::string>>() ;
}

/////////////////////////////////////////////////
ModelSpawner::~ModelSpawner()
{
}

/////////////////////////////////////////////////
void ModelSpawner::Load(physics::WorldPtr _world, sdf::ElementPtr _sdf)
{
  // iterate over every element in the plugin
  int count = _sdf->Get<int>("count");
  for(int i = 1; i <= count; ++i)
  {
    std::string newName = "include" + std::to_string(i);
    if(_sdf->HasElement(newName))
    {
      sdf::ElementPtr child = _sdf->GetElement(newName);

      // fetch data values
      std::string attName = child->Get<std::string>("name");
      std::string attURI = child->Get<std::string>("uri");
      std::string attPose = child->Get<std::string>("pose");
      double attTime = child->Get<double>("time");

      // parse pose string to integers
      double pose [6];
      {
        char str[attPose.size()+1];
        strcpy(str, attPose.c_str());
        char *token = strtok(str, " ");

        int counter = 0;
        while(token != NULL)
        {
          pose[counter] = atoi(token);
          token = strtok(NULL, " ");
          counter++;
        }
      }

      // parse URI to get the filename
      std::string filename;
      {
        char str[attURI.size()+1];
        strcpy(str, attURI.c_str());
        char *token = strtok(str, "/");

        while(token != NULL)
        {
          std::string oofName(token);
          filename = oofName;
          token = strtok(NULL, "/");
        }
      }

      // load the SDF from file, find a modification for working directory
      // currently running in the model_spawner directory
      std::string sdfOutput;
      {
        std::ifstream myFile;
        std::string filePath = "/home/yuul/Desktop/model_spawner/models/" + filename + "/model.sdf";
        myFile.open(filePath);
        char cwd [256];
        getcwd(cwd, sizeof(cwd));
        // error checking
        if (!myFile)
        {
          std::cerr << "Unable to open file " << filePath <<'\n';
          exit(1);
        }

        std::string line;
        while (std::getline(myFile, line))
        {
          sdfOutput = sdfOutput  + line;
        }
      }

      // parse the SDF into two parts so as to allow the pose and model name be generated
      std::string sdfFront;
      std::string sdfBack;
      {
        std::string delimiterStart = "<model name=";
        std::string delimiterEnd = "\">";

        size_t start = sdfOutput.find(delimiterStart);
        size_t end = sdfOutput.substr(start, sdfOutput.size()-start).find(delimiterEnd);
        sdfFront = sdfOutput.substr(0, start);
        sdfBack = sdfOutput.substr(start+end+2, sdfOutput.size()-start-end-2);
      }

      // generate the model name sring and the pose string
      std::string namePose;
      {
        namePose = "<model name=\'" + attName + "\'><pose>" + std::to_string(pose[0])
        + " " + std::to_string(pose[1]) + " " + std::to_string(pose[2]) + " "
        + std::to_string(pose[3]) + " " + std::to_string(pose[4]) + " "
        + std::to_string(pose[5]) + "</pose>";
      }
      std::string newModelStr = sdfFront + namePose + sdfBack;

      // adds to or creates a new vector
      if(this->msgMap.find(attTime) == this->msgMap.end())
      {
        std::vector<std::string> newVec;
        newVec.push_back(newModelStr);
        this->msgMap[attTime] = newVec;
      }
      else
      {
        this->msgMap[attTime].push_back(newModelStr);
      }
    }
  }

  // Create a new transport node, initialize with world, and create publisher
  this->node = transport::NodePtr(new transport::Node());
  this->node->Init(_world->Name());
  this->factoryPub = node->Advertise<msgs::Factory>("~/factory");

  this->updateConnection= event::Events::ConnectWorldUpdateBegin(
    std::bind(&ModelSpawner::Update, this, std::placeholders::_1));

}

/////////////////////////////////////////////////
void ModelSpawner::Update(const common::UpdateInfo &_info)
{
  //iterate over all members of map to find the ones to publish
  std::unordered_map<double,std::vector<std::string>>::iterator itr = this->msgMap.begin();
  while (itr != this->msgMap.end()) {

    if(itr->first <= _info.simTime.Double())
    {
      for (size_t i = 0; i < itr->second.size(); i++) {

        msgs::Factory msg;
        msg.set_sdf(itr->second[i]);
        this->factoryPub->Publish(msg);
      }
      double delVal = itr->first;
      itr++;
      this->msgMap.erase(delVal);
    } else {
      itr++;
    }
  }
}
