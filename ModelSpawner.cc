#include <iostream>
#include <fstream>
#include <string>

#include <ignition/math/Pose3.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/physics/World.hh>
#include <gazebo/transport/transport.hh>

#include "ModelSpawner.hh"

using namespace gazebo;

GZ_REGISTER_WORLD_PLUGIN(ModelSpawner)

/////////////////////////////////////////////////
ModelSpawner::ModelSpawner()
{
  //printf("Hello World!\n");
}

/////////////////////////////////////////////////
ModelSpawner::~ModelSpawner()
{
}

/////////////////////////////////////////////////
void ModelSpawner::Load(physics::WorldPtr _world, sdf::ElementPtr _sdf)
{
  int count = _sdf->Get<int>("counter");
  std::cout<<count<<std::endl;

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
      int attTime = child->Get<int>("time");

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
        std::string filePath = "./models/" + filename + "/model.sdf";
        myFile.open(filePath);

        // error checking
        if (!myFile)
        {
          std::cerr << "Unable to open file " << filePath;
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

      // Create a new transport node, initialize with world, and create publisher
      transport::NodePtr node(new transport::Node());
      node->Init(_world->Name());
      transport::PublisherPtr factoryPub = node->Advertise<msgs::Factory>("~/factory");

      msgs::Factory msg;
      std::string newModelStr = sdfFront + namePose + sdfBack;
      msg.set_sdf(newModelStr);
      factoryPub->Publish(msg);
    }
  }
}

/////////////////////////////////////////////////
void ModelSpawner::Update(const common::UpdateInfo &_info)
{
}
