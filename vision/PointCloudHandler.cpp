#include <stdio.h>
#include <ctime>
#include <lcm/lcm-cpp.hpp>
#include "kinect/depth_msg_t.hpp"
#include "PointCloudFactory.hpp"
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/io/io.h>
#include <pcl/io/pcd_io.h>
#include "kinect/pointcloud_t.hpp"
#include <string>
#include <iostream>
#include <fstream> 
#include "PointCloudHandler.hpp"

typedef pcl::PointCloud<pcl::PointXYZ> PointCloud;


      PointCloudHandler::PointCloudHandler(string pcdFileName) {
				PointCloudHandler();
				this->pcdOutputFile = pcdFileName;
			}

			 PointCloudHandler::PointCloudHandler() {
				// viewer.runOnVisualizationThreadOnce (viewInit);
				// viewer.runOnVisualizationThread(viewCloud); 
				received_frame = false;
			}

			///////////////////////////////////////////////////////////////
			//getters setters
			/////////////////////////////////////////////////////////////

			void  PointCloudHandler::setCurrentPointCloud(PointCloud::Ptr cloud){
				currentPointCloud = cloud;
			}
			
			const PointCloud::Ptr PointCloudHandler::getCurrentPointCloud(){
				return currentPointCloud;
			}
     		///////////////////////////////////////////////////////////////
             //I/O
             /////////////////////////////////////////////////////////////

     		//for streams to PointCloundHandler use time filtering noise techniques
     		void  PointCloudHandler::ingestDepthImage(const lcm::ReceiveBuffer* rbuf,
              const std::string& chan, 
              const kinect::depth_msg_t* depthImage) {

                //printf("Received depth message on channel \"%s\":\n", chan.c_str());
              if(!received_frame){
                	received_frame = true;
					         PointCloud::Ptr pointCloud = PointCloudFactory::depthImageToPointCloud(depthImage);
					         // std::cerr << "PointCloud size: " << pointCloud->width * pointCloud->height <<  ").\n";

					         // //pointCloud = PointCloudFactory::zPassThroughFilter(pointCloud, 0.2, 0.4);
					         // //pointCloud = PointCloudFactory::statOutlierRemoval(pointCloud, 50, 1.0);
					         // //pointCloud = PointCloudFactory::voxelDownSample(pointCloud,.01, .01, .01);
					         // std::cerr << "PointCloud after: " << pointCloud->width * pointCloud->height <<  ").\n";
					         // viewer.showCloud(pointCloud);
 			            // publishPointCloud(pointCloud);
 			     	      savePointCloud(pointCloud);
                	//readModelPCDFile("../RGB-D_TestSet/Kinect_1Box_OpenFloor.pcd");
                	// viewer.showCloud(pointCloud);
 				}
 			}

 			void  PointCloudHandler::ingestDepthImages(const lcm::ReceiveBuffer* rbuf,
                const std::string& chan, 
                const kinect::depth_msg_t* depthImage) {

                printf("Received depth message on channel \"%s\":\n", chan.c_str());
               
				        PointCloud::Ptr pointCloud = PointCloudFactory::depthImageToPointCloud(depthImage);
				        // std::cerr << "PointCloud size: " << pointCloud->width * pointCloud->height <<  ").\n";

				        // //pointCloud = PointCloudFactory::zPassThroughFilter(pointCloud, 0.2, 0.4);
				        // //pointCloud = PointCloudFactory::statOutlierRemoval(pointCloud, 50, 1.0);
				        // //pointCloud = PointCloudFactory::voxelDownSample(pointCloud,.01, .01, .01);
				        // std::cerr << "PointCloud after: " << pointCloud->width * pointCloud->height <<  ").\n";

 			          publishPointCloud(pointCloud);
                // viewer.showCloud(pointCloud);
 			}


			int PointCloudHandler::readModelPCDFile(string pathToFile){
				pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
				if (pcl::io::loadPCDFile<pcl::PointXYZ> (pathToFile, *cloud) == -1) {
   					PCL_ERROR ("Couldn't read pcd file \n");
    				return (-1);
  				}
  				std::cout << "Loaded " << pathToFile << " containing "
            	<< cloud->width * cloud->height << " points "<< std::endl;
            	currentPointCloud = cloud;
			}
 
 			int PointCloudHandler::savePointCloud(PointCloud::Ptr cloud) {
     			string extension = ".pcd";
     			string directory = "../RGB-D_TestSet/";
     			string path = directory +  pcdOutputFile + ".pcd";
     			std::cerr <<  path << std::endl;


     			if(pcl::io::savePCDFileASCII (path,  *cloud)){
        			std::cerr << "Saved " << cloud->points.size () << " data points to " << path << std::endl;
        			return 0;
      			}
    			else {
         			std::cerr << "Could not write to pcd file to "<< path << std::endl;
         			return 1;
      			}
      		}

  			void PointCloudHandler::publishPointCloud(PointCloud::Ptr cloud){
  				//botcore lcm type
  				int32_t frameSize = cloud->points.size();
    			kinect::pointcloud_t point_cloud;
    			point_cloud.n_points = frameSize;
    			point_cloud.points.resize(frameSize, std::vector<float>(3));
    			point_cloud.utime = 0;
    			point_cloud.seq = 0;
    			point_cloud.frame_id = "0";
    			point_cloud.n_channels = 1;
    			point_cloud.channel_names.resize(1);
    			point_cloud.channels.resize(1, std::vector<float>(frameSize));
    			for(size_t i = 0; i < frameSize; ++i) {
    				cloud->points[i].x = cloud->points[i].x;
    				cloud->points[i].y = cloud->points[i].y;
    				cloud->points[i].z = cloud->points[i].z;
  				}

   				messager.publish("KINECT_POINT_CLOUD_RAW", &point_cloud);
   				std::cout << "publishing KINECT_POINT_CLOUD_RAW" << std::endl;
  			}

  			///////////////////////////////////////////////////////
  			//viz
  			/////////////////////////////////////////////////////
  			
  			static void viewInit (pcl::visualization::PCLVisualizer& viewer){
    			viewer.setBackgroundColor (1.0, 0.5, 1.0);
    		}
    		static void viewCloud (pcl::visualization::PCLVisualizer& viewer) {
    			std::stringstream ss;
				//viewer.addText (ss.str(), 200, 300, "text", 0);
    		}

  			///////////////////////////////////////////////////////
  			//util
  			/////////////////////////////////////////////////////
  			static string getCurrentTimeStamp (){
  				time_t now = time(0);
  				char* dt = ctime(&now);
  				std::string date(dt);
  				date.erase(std::remove(date.begin(),date.end(),' '),date.end()-2);

  				return date;

  			}