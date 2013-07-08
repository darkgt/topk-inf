#include "data.h"


// DATA

DATA::DATA() {
	data = new float[DIMENSION];
	//id=-1;
}

DATA::~DATA() {
    delete [] data;
}

DATA & DATA::operator = (DATA &_d) {
	memcpy(data, _d.data, sizeof(float) * DIMENSION);
    //id=_d.id;
    key=_d.key;
    return *this;
}

void DATA::read_from_buffer(char *buffer) {
    int i = DIMENSION*sizeof(float);
    memcpy(data, buffer, i);
    
   // memcpy(&id, &buffer[i], sizeof(int));
    
   // i+=sizeof(int);
    memcpy(&key, &buffer[i], sizeof(bitmask_t));
}

void DATA::write_to_buffer(char *buffer) {
    int i = DIMENSION*sizeof(float);
    memcpy(buffer, data, i);
    
  //  memcpy(&buffer[i], &id, sizeof(int));
    
   // i+=sizeof(int);
    memcpy(&buffer[i], &key, sizeof(bitmask_t));
}

void DATA::print() {
	//printf("%d ( ",id);
	for (int i = 0; i < DIMENSION; i++)
		printf("%7.5f ",data[i]);
	printf(")\n");
}

