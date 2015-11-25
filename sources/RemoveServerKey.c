#include <stdio.h>#include <stdlib.h>#include <stddef.h> /* needed for offsetof */#include <string.h>#include <console.h>#include <sioux.h>#include <PLStringFuncs.h>#include "AppleShareRegistry.h"#include "AppleShareFileServerRegistry.h"void BuildObjectSpecByShortID(OAMObjectSpec *obj, OAMShortObjectSpec id){	memset(obj, 0, sizeof(OAMObjectSpec));	obj->specType = kOAMObjectSpecByShortID;	obj->u.shortID = id;}void BuildObjectSpecByNameType(OAMObjectSpec *obj, StringPtr name, OAMType type){	short len = 0;		memset(obj, 0, sizeof(OAMObjectSpec));	obj->specType = kOAMObjectSpecByNameType;	obj->objectType = type;	len = *name + 1;	memcpy(obj->u.name, name, len);}OAMStatus RemoveServerKey(OAMSessionID sessionId){	OAMStatus				err = noErr;	OAMObjectSpec			obj;		OAMAttributeDescriptor 	attr[2] = {};		unsigned char			key[4096];    	 	memset(&attr, 0, sizeof(attr)); 	attr[0].attributeSignature 			= kOAMMachine;	attr[0].attributeType 				= 'PGPs';	attr[0].bufferDescriptor.buffer 	=  key;	attr[0].bufferDescriptor.bufferLen 	=  sizeof(key);  	attr[1].attributeSignature = NULL;	memset(&obj, 0, sizeof(OAMObjectSpec));		obj.specType = kOAMObjectSpecByShortID;	obj.u.shortID = kOAMMachineShortID;    	err = OAMDeleteAttribute(sessionId, &obj,  attr, NULL);  	return err;  	}int main ( void){ 	OAMStatus 		err = noErr;	OAMSessionID	sessionID = 0;    	err = OAMInitialize(1, 0, NULL, NULL);  	 	err = OAMOpenSession(NULL, &sessionID, NULL); 	if (sessionID)	{ 		err = RemoveServerKey( sessionID); 	 		OAMCloseSession(sessionID, NULL);	}	err = OAMDeInitialize();  	return 0;  }