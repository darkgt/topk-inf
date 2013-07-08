#ifndef CONFIG
#define CONFIG

#include <string>
//This file defines the constant parameters such as file paths

//File paths

//#define GOWALLA
#define BRIGHTKITA

#ifdef GOWALLA
std::string baseDir = "..\\Gowalla\\";
std::string edgeFile = baseDir + "Gowalla_edges.txt"; //directed graph
std::string hubNodesFile = baseDir + "HubNodes2.txt";
std::string clusterFile  = baseDir + "MetisGraphFile.txt.part.2048";
int nodenum = 196591;
int hublevel = 11;
#else
std::string baseDir = "..\\Brightkite\\";
std::string edgeFile = baseDir + "Brightkite_edges.txt"; //directed graph
std::string hubNodesFile = baseDir + "HubNodesB2.txt";
std::string clusterFile  = baseDir + "MetisGraphFileB.txt.part.1024";
int nodenum = 58228;
int hublevel = 10;
#endif

std::string bcaVectorDir = baseDir + "bcaVector\\";

//size_t
#define MAX_FILENAME_LEN 256

#endif
