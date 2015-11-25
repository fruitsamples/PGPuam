//	TPGPkey.h - PGP Key Object  // // Apple Macintosh Developer Technical Support// Written by:  Vinnie Moscaritolo////  Copyright (work in progress)  Apple Computer, Inc All rights reserved.//// You may incorporate this sample code into your applications without// restriction, though the sample code has been provided "AS IS" and the// responsibility for its operation is 100% yours.  However, what you are// not permitted to do is to redistribute the source as "DSC Sample Code"// after having made changes. If you're going to re-distribute the source,// we require that you make it clear in the source that the code was// descended from Apple Sample Code, but that you've made changes.// #ifndef _H_TPGPKEY#define _H_TPGPKEY#define PGP_MACINTOSH 1#include "pgpErrors.h"#include "pgpKeys.h"#include "pgpUtilities.h"// ---------------------------------------------------------------------------//	 TPGPkey - PGP Key Object // ---------------------------------------------------------------------------// class TPGPkey{public:	//	ENUMS AND TYPEDEFS			enum EKeyIcons{			kAlertIcon_ID			= 4000, /*129 ?? */			kIconRSA_ID				= 3000,			kIconRSA_Revoked_ID		= 3004,			kIconRSA_Expired_ID		= 3005,			kIconRSA_Disabled_ID	= 3010,			kIconDH_ID				= 3007,			kIconDH_RevokedID		= 3012,			kIconDH_Expired_ID		= 3013,			kIconDH_Disabled_ID		= 3006,			kIcon_KeyNotfound_ID	= 3028		}; // 	CONSTRUCTORS AND DESTRUCTORS			 TPGPkey()	:			 			fInitialized(false),			 			fKeyRef(kInvalidPGPKeyRef) {};			 			// 	virtual  ~TPGPkey();			// MAIN INTERFACE	virtual void Initialize(PGPKeyRef);			// initialize	 // ACCESSSORS	virtual Boolean IsExpired() 		{ return  fExpired;   };	virtual Boolean IsRevoked() 		{ return  fRevoked;   };	virtual Boolean IsDisabled() 		{ return  fDisabled;  };	virtual Boolean IsAxiomatic() 		{ return  fAxiomatic; };	virtual Boolean IsOperational() 	{ return ( PGPContextRefIsValid(fgContext)												/*	&& PGPKeySetRefIsValid(fgPGPKeySetRef)  */													&& PGPKeyRefIsValid(fKeyRef) ); };																											virtual Boolean CanSign();	virtual Boolean CanVerify();														virtual Boolean IsInitialized()		{ return  fInitialized; };		virtual PGPPublicKeyAlgorithm GetPublicKeyAlgorithm()	{ return  fPublicKeyAlgorithm;   };	virtual PGPHashAlgorithm	  GetHashAlgorithm()	 	{ return  fHashAlgorithm;   };	virtual	PGPValidity			  GetValidity()				{ return  fValidity;   };	virtual PGPInt32			  GetTrustLevel()			{ return  fTrustLevel; };	virtual PGPSize				  GetKeySize()				{ return  fKeySize; };	virtual PGPSize			      GetHashSize()				{ return  fHashSize; };	 	 	virtual short	GetIconID();  	virtual long	GetCreateDate()		{ return  fCreatedTime;   }; 	virtual long	GetExpireDate()		{ return  fExpireTime;   }; 	 	virtual void	GetFingerprintPString(StringPtr buf); 	virtual void	GetFingerprintBinaryPString (void* outBuf); 	virtual void	GetPrimaryUserNamePString(StringPtr buf); 	virtual void 	GetPublicKeyAlgorithmPstring(StringPtr buf);	virtual void	Export		(void** buf, PGPSize *bufSize ); 	 		virtual Boolean	Verify( void* inbuf, PGPSize inBufSize, void* sigbuf,  PGPSize sigBufSize );	virtual Boolean	Sign( void* inbuf, PGPSize inBufSize, void* sigbuf,  PGPSize *sigBufSize,const char *passPhrase);	  	PGPKeyRef		fKeyRef;			//  	// PRIVATE FIELDSprotected: 	Boolean			fInitialized;	 	Boolean			fExpired; 	Boolean			fRevoked; 	Boolean			fDisabled; 	Boolean			fAxiomatic;	PGPValidity		fValidity;	PGPInt32		fTrustLevel; 	long 			fCreatedTime; 	long 			fExpireTime; 	 	PGPPublicKeyAlgorithm fPublicKeyAlgorithm;	PGPHashAlgorithm	  fHashAlgorithm;	PGPSize 			  fKeySize;	PGPSize			  	  fHashSize;	// CLASS VARIABLES public: 	static 	PGPContextRef	fgContext;		  	static 	PGPKeySetRef 	fgPGPKeySetRef; 	// CLASS FUNCTIONSpublic:	static 	void	Initialize ();	static 	void	Initialize (PGPMemoryMgrRef memMgr);	static	void	Finalize(); 	static	void	OpenKeyDefaultRing();	static  void 	CloseKeyRing();};#endif