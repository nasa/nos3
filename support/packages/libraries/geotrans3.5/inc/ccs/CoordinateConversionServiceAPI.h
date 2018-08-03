// CLASSIFICATION: UNCLASSIFIED

#ifndef _MSP_COORDINATE_CONVERSION_API_H
#define _MSP_COORDINATE_CONVERSION_API_H

//-----------------------------------------------------------------------------------
//    System Includes
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//    Local Includes
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//    Local Structures
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//    Enumerated Types
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//    Local Defines
//-----------------------------------------------------------------------------------

#if defined(WIN32)
#	if defined(_USRDLL)
#		if defined(MSP_COORDINATE_CONVERSION_EXPORTS)
#			define MSP_COORDINATE_CONVERSION_API __declspec(dllexport)
#                       define MSP_COORDINATE_CONVERSION_TEMPLATE_EXPORT 
#		elif defined(MSP_COORDINATE_CONVERSION_IMPORTS)
#			define MSP_COORDINATE_CONVERSION_API __declspec(dllimport)
#                       define MSP_COORDINATE_CONVERSION_TEMPLATE_EXPORT extern
#               else
#                       define MSP_COORDINATE_CONVERSION_API
#                       define MSP_COORDINATE_CONVERSION_TEMPLATE_EXPORT 
#		endif
#	else
#		define MSP_COORDINATE_CONVERSION_API
#               define MSP_COORDINATE_CONVERSION_TEMPLATE_EXPORT 
#	endif
#else
#       define MSP_COORDINATE_CONVERSION_API
#endif

//-----------------------------------------------------------------------------------
//    Function Prototypes
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//    End of File
//-----------------------------------------------------------------------------------

#endif // _MSP_COORDINATE_CONVERSIONAPI_H
