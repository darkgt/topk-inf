#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>
//This file defines the constant parameters such as file paths

/****** File paths ******/

//#define GOWALLA
#define BRIGHTKITA

/*******  User Graph  *******/
#ifdef GOWALLA
std::string baseDir			= "..\\Gowalla\\";
std::string edgeFile		= baseDir + "Gowalla_edges.txt";	//directed graph
std::string hubNodesFile	= baseDir + "HubNodes2.txt";
std::string clusterFile		= baseDir + "MetisGraphFile.txt.part.2048";
int nodenum 				= 196591;
int hublevel				= 11;
#else
std::string baseDir			= "..\\Brightkite\\";
std::string edgeFile		= baseDir + "Brightkite_edges.txt";	//directed graph
std::string hubNodesFile	= baseDir + "HubNodesB2.txt";
std::string clusterFile		= baseDir + "MetisGraphFileB.txt.part.1024";
int nodenum					= 58228;
int hublevel				= 10;
#endif

std::string userdoc			= baseDir + "userdoc";				//text description of users
std::string userloc			= baseDir + "userloc";				//check location of users
std::string utermweightFile	= baseDir + "vsm_user_value";		//term weight of each user keyword

/*******  Location Index  *******/
std::string bcaVectorDir	= baseDir + "bcaVector\\";
std::string basepath		= baseDir + "locData\\";

std::string locFile			= basepath + "loc";					//location
std::string docFile			= basepath + "doc";					//text description of location

std::string treeFile 		= basepath + "rtree";				//rtree index
std::string leafnodeFile	= basepath + "leafnodes";			//leaf nodes
std::string indexnodeFile 	= basepath + "indexnodes";			//index nodes
std::string ltermweightFile	= basepath + "vsm_loc_value";		//vector space model based term weight
std::string vocabFolder		= basepath + "vocabulary\\";		//term weight of each node
std::string btreeFolder		= basepath + "btreeindex\\";		//B+ tree index

/****** Sizes ******/
#define MAX_FILENAME_LEN 256

/****** Constant Values ******/
int numOfEntry = 100;
int Blocksize = 4 * 1024;

#endif
