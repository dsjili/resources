// DynLib.h

#pragma once

#ifndef DYNLIB_H
#define DYNLIB_H

#define DYNLIB_EXPORTS
#ifdef DYNLIB_EXPORTS
#define DYNLIB_API __declspec(dllexport)
#else
#define DYNLIB_API __declspec(dllimport)
#endif // DYNLIB_EXPORTS

#include "stdafx.h"

typedef const char * TUtf8;
typedef uint64_t TId;

//! Return codes
enum TReturn
{
    KReturn_Success = 0,
    KReturn_Fail
};

//! container for array of bytes+length
//! memory is NOT owned by this object
struct SDataRef
{
    const uint8_t *data; //!< pointer to start of data
    uint64_t len; //!< in bytes
};

//! The subtype of IResource
enum TResourceType
{
    KResourceType_Unknown = 0, //!< Can only use IResource*
    KResourceType_Image, //!< Can use IImageResource*
    KResourceType_Video, //!< Can use IVideoResource*
};

//! Represents interface into a file
class DYNLIB_API IResource
{
public:
    //! @return detected file type
    virtual TResourceType Type(void)
    {
        return KResourceType_Unknown;
    }

    //! @return unique id associated with this IResource
    virtual TId Id(void) const = 0;

    //! @return path to this IResource
    virtual TUtf8 Path(void) const = 0;

    //! @return memory containing raw data of this IResource
    virtual SDataRef Raw(void) = 0;
};

#define SUPPORT_IMAGE_RESOURCE
#ifdef SUPPORT_IMAGE_RESOURCE
//! Represents interface into an image file
class DYNLIB_API IImageResource : public IResource
{
public:
    virtual TResourceType Type() const
    {
        return KResourceType_Image;
    }

    //! Represents the pixel format of the uncompressed image data
    enum TImageFormat
    {
        KImageFormat_RGB888 = 0,    //!< 24 bits per pixel
        KImageFormat_RGBA8888,      //!< 32 bits per pixel
        KImageFormat_RGB565,        //!< 16 bits per pixel
    };

    //! Properties for this image resource
    struct CImageProperties
    {
        uint32_t pixWidth; //!< number of pixels
        uint32_t pixHeight; //!< number of pixels
        uint32_t pitchBytes; //!< bytes per row
        TImageFormat format; //!< uncompressed pixel format
    };

    //! @return properties for this image
    virtual CImageProperties *Properties(void) = 0;

    //! @return memory containing uncompressed data of this image
    virtual SDataRef UncompressedData(void) = 0;
};
#endif


//! Represents into an video file
class DYNLIB_API IVideoResource : public IResource {
public:
    virtual TResourceType Type() const
    {
        return KResourceType_Video;
    }

    struct CVideoProperties {
        TUtf8 name; //!< name of the video
    };

    virtual CVideoProperties *Properties(void) = 0;
};

//! Interface representing a list of IResource* items
typedef std::vector<IResource*> TResourceList;

//! Used as a filter in IResourceCollection.ResourceList() call
struct SResourceFilter
{
    //! indicates which items in union are valid
    enum TFilterBy
    {
        KFilterBy_Path = 0,
        KFilterBy_ResourceType,
    };

    TFilterBy filterBy; //!< indicates which items in union are valid
    union
    {
        TUtf8 path; //!< path string to filter resource
        TResourceType resType; //!< type of resource needed
    };
};

//! Represents a collection of files
class DYNLIB_API IResourceCollection
{
public:
    //! Adds a file to this collection for later access/manipulation
    //! A unique TId (per collection) must be assigned to this IResource
    //! @param[in] path the path to the resource
    //! @param[out] id id unique to this collection that maps to the added IResource
    //! @return KReturn_Success if resource was successfully added to collection
    virtual TReturn AddFile(TUtf8 path, TId &id) = 0;
    
    //! Recursively searches a directory path and adds to collection
    //! Makes use of add file for each individual file
    //! @param[in] path the path of the directory to recursively search
    //! @return KReturn_Success if all resources found in directory were added to collection
    virtual TReturn AddAllFiles(TUtf8 path) = 0;

    //! Obtain IResource pointer of a resource
    //! @param[in] id id unique to this collection that maps to a previously 
    //!               created IResource
    //! @param[out] element populated with pointer to the IResource if available
    //! @return KReturn_Success if resource was successfully found
    virtual TReturn ResourceFromId(const TId id, IResource *&element) = 0;

    //! Populates list with all IResource objects matching the filter
    //! @param[out] list reference to an empty list that is modified to 
    //!             contain all IResource* instances that match the filter.
    //! @param[in] filter the filter used to match IResource, pass nullptr to match all IResource
    //! @return KReturn_Success if successfully scanned filter. An empty list return
    //! indicates no items found.
    virtual TReturn ResourceList(TResourceList &list, const SResourceFilter *filter = nullptr) = 0;

    //! Accesor to the protected database filename
    //! @return returns the database filename of this resource collection
    //! indicates no items found.
    virtual TUtf8 DB(void) = 0;
};

//! for creating an IResource collection
//! @return a new IResourceCollection
DYNLIB_API IResourceCollection *CreateResourceCollection(TUtf8 fn = nullptr);

//! for deleting an IResource collection created by CreateCollection()
//! @param[in] collection pointer to collection to be deleted
DYNLIB_API void DestroyResourceCollection(IResourceCollection *collection);

#endif // ALIBRARY_H
