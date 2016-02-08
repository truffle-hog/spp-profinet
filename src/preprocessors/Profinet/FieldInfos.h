#ifndef __FIELD_INFOS__
#define __FIELD_INFOS__

#include "plugbase.h"


enum HeaderType {

    ETHERNET = 0xfffffffe, IP = 0xfffffffd,
	FRAME = 0

};

enum DataType {

	STRING = 0, INT = 1, UINT = 2, FLOAT = 3, DOUBLE = 4, BOOL = 5, RAW = 6

};


struct FieldInfo {

    char *key; /** the key name for refering to this field in code **/
    char *name; /** Name as it could be displayed as information **/
    enum HeaderType headerType;
    enum DataType dataType;
	/** The length of this data in Bits **/
	int length;

};

//struct FieldInfo* FrameInfo_new(Packet *p) {

	struct FieldInfo *frameInfo = malloc(sizeof(struct FieldInfo));
	frameInfo->key = "frame_info";
	frameInfo->name = "Frame Information";	
	frameInfo->headerType = FRAME;	


//}

#endif
