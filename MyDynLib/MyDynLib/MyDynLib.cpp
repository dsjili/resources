// MyDynLib.cpp : Defines the exported functions for the DLL application.
//

#include "DynLib.h"
#include "jpeglib.h"
#include "MySQL_Lib.h"
#include "MyGuid_Lib.h"


using namespace std;
namespace fs = experimental::filesystem;
/////////////////////////////////////////////////////////////

class CResource : public IResource
{
public:
    CResource(TUtf8 p, TId i) {
        path = p;
        id = i;
        cout << "Resource created." << endl;
        //Raw();
    }

    TResourceType Type(void)
    {
        return KResourceType_Unknown;
    }

    TId Id(void) const;
    TUtf8 Path(void) const;
    SDataRef Raw(void);
protected:
    TId id;
    TUtf8 path;
    SDataRef content;
};


class CImageResource : public IImageResource
{
public:
    CImageResource(TUtf8 p, TId i) {
        path = p;
        id = i;
        cout << "Resource created." << endl;
        //ReadFile();
    }

    TResourceType Type(void)
    {
        return KResourceType_Image;
    }

    TId Id(void) const;
    TUtf8 Path(void) const;
    int ReadFile(void);
    SDataRef Raw(void);
    SDataRef UncompressedData(void);
    CImageProperties *Properties(void);
protected:
    TId id;
    TUtf8 path;
    CImageProperties *properties;
    SDataRef compressed_data;
    SDataRef uncompressed_data;
};

class CVideoResource : public IVideoResource
{
public:
    CVideoResource(TUtf8 p, TId i) {
        path = p;
        id = i;
        cout << "Resource created." << endl;
    }

    TResourceType Type(void)
    {
        return KResourceType_Video;
    }

    TId Id(void) const;
    TUtf8 Path(void) const;
    SDataRef Raw(void);
    CVideoProperties *Properties(void);
protected:
    TId id;
    TUtf8 path;
    CVideoProperties *properties;
    SDataRef raw;
};


class CResourceCollection : public IResourceCollection
{
public:
    //! constructor with database filename given
    CResourceCollection(TUtf8 filename) {
        collection = new std::map<TId, IResource*>();
        if (filename != nullptr) {
            dbFilename = filename;
        }
        else {
            dbFilename = "temp.db";
        }

        sqlite3 *db;
        char *sql;
        db = openDB((char *) dbFilename);

        sql = "CREATE TABLE IF NOT EXISTS RESOURCES(" \
            "ID INT PRIMARY KEY NOT NULL," \
            "PATH TEXT NOT NULL," \
            "TYPE INT NOT NULL);";

        create(sql, db);
        closeDB(db);
        cout << "Resource Collection created." << endl;
    }
    
    TReturn AddFile(TUtf8 path, TId &id);
    TReturn AddAllFiles(TUtf8 path);
    TReturn ResourceFromId(const TId id, IResource *&element);
    TUtf8 DB(void);
    TReturn ResourceList(TResourceList &list, const SResourceFilter *filter = nullptr);
protected:
    std::map<TId, IResource*> *collection;
    TUtf8 dbFilename;
};


//////////////////////////////////////////////////////////////////



// CResource function implementations
TId CResource::Id(void) const
{
    return id;
}

TUtf8 CResource::Path(void) const
{
    return path;
}

SDataRef CResource::Raw(void)
{
    string filename = (string)path;
    vector<uint8_t> bytes;
    ifstream file1(filename, ios_base::in | ios_base::binary);
    uint8_t ch = file1.get();
    while (file1.good())
    {
        bytes.push_back(ch);
        ch = file1.get();
    }
    size_t size = bytes.size();

    uint8_t *data = (uint8_t*)malloc(sizeof(uint8_t)*size);
    int i = 0;
    for (vector<uint8_t>::iterator it = bytes.begin(); it != bytes.end(); ++it) {
        data[i++] = *it;
    }

    content.data = data;
    content.len = size;
    /*
        ifstream fin((char*)path);
        if (!fin) {
            cerr << "Unable to open file.\n";
            exit(1); // terminate with an error
        }

        content.len = fin.end;
        string *in = new string();
        uint8_t *input = (uint8_t *)malloc(sizeof(uint8_t)*fin.end);
        int i = 0;
        while (!fin.eof()) {
            in->append((char*)fin.get());
        }
        fin.close();
        content.data = (uint8_t*)in;

        printf("contents read from file.\n");
        cout << (char *)in << endl;
    */
    return content;
}

// CImageResource function implementations
TId CImageResource::Id(void) const
{
    return id;
}

TUtf8 CImageResource::Path(void) const
{
    return path;
}

int CImageResource::ReadFile(void)
{
    uint8_t * raw_image = NULL;
    char * filename = (char *)Path();
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    /* libjpeg data structure for storing one row, that is, scanline of an image */
    JSAMPROW row_pointer[1];

    FILE *infile = fopen(filename, "rb");
    unsigned long location = 0;
    int i = 0;

    if (!infile)
    {
        printf("Error opening jpeg file %s\n!", filename);
        return -1;
    }

    // standard libjpeg error handler
    cinfo.err = jpeg_std_error(&jerr);
    // setup decompression process and source, then read JPEG header
    jpeg_create_decompress(&cinfo);
    // this makes the library read from infile
    jpeg_stdio_src(&cinfo, infile);
    /* reading the image header which contains image information */
    jpeg_read_header(&cinfo, TRUE);

    properties = new CImageProperties();
    // set resources properties
    properties->pixWidth = cinfo.image_width;
    properties->pixHeight = cinfo.image_height;
    properties->pitchBytes = cinfo.MCUs_per_row; // this may not be it
    if (cinfo.data_precision == 8) { properties->format = KImageFormat_RGB888; }

    /* Start decompression jpeg here */
    jpeg_start_decompress(&cinfo);

    /* allocate memory to hold the uncompressed image */
    raw_image = (uint8_t *)malloc(cinfo.output_width*cinfo.output_height*cinfo.num_components);
    /* now actually read the jpeg into the raw buffer */
    row_pointer[0] = (unsigned char *)malloc(cinfo.output_width*cinfo.num_components);
    /* read one scan line at a time */
    while (cinfo.output_scanline < cinfo.image_height)
    {
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
        for (i = 0; i < cinfo.image_width*cinfo.num_components; i++)
            raw_image[location++] = (uint8_t)row_pointer[0][i];
    }
    /* wrap up decompression, destroy objects, free pointers and close open files */
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    free(row_pointer[0]);
    fclose(infile);


    // setting the uncompressed data
    uncompressed_data.data = raw_image;
    uncompressed_data.len = cinfo.image_height;

    //set compressed data, note: for now the compressed and uncompressed data are the same;
    compressed_data.data = raw_image;
    compressed_data.len = cinfo.image_height;

    return 1;
}

SDataRef CImageResource::Raw(void)
{
    return compressed_data;
}

IImageResource::CImageProperties * CImageResource::Properties(void)
{
    return properties;
}

SDataRef CImageResource::UncompressedData(void)
{
    return uncompressed_data;
}

// CResourceCollection function definitions

TReturn CResourceCollection::AddFile(TUtf8 path, TId &id)
{
    random_device rd;
    id = rd();
    string p = (string)path;
    size_t found = p.find_last_of(".");
    try
    {
        TResourceType type;
        if (p.substr(found) == ".jpg") {
            IResource *res = new CImageResource(path, id);
            type = res->Type();
            collection->insert(pair<TId, IResource*>(id, res));
            printf("Image File id:%llu, path:%s added to collection.\n", id, path);
        }
        else {
            IResource *res = new CResource(path, id);
            type = res->Type();
            collection->insert(pair<TId, IResource*>(id, res));
            printf("Unknown File id:%llu, path:%s added to collection.\n", id, path);
        }
        
        sqlite3 *db = openDB((char *) DB());
        string sql = "INSERT INTO RESOURCES VALUES (" + 
                        to_string(id) +
                        ", '" + p + "', " + to_string((int) type) + ");";
        insert((char*) sql.data(), db);
        closeDB(db);
        return KReturn_Success;
    }
    catch (bad_alloc& ba)
    {
        cerr << "bad_alloc caught: " << ba.what() << '\n';
        return KReturn_Fail;
    }
}

TReturn CResourceCollection::AddAllFiles(TUtf8 path)
{
    for (auto & p : fs::recursive_directory_iterator(path)) {
        TId id;
        //cout << p.path().u8string().data() << endl;
        AddFile((TUtf8)p.path().u8string().data(), id);
    }
        
    
    
    return KReturn_Success;
}

TReturn CResourceCollection::ResourceFromId(const TId id, IResource *&element)
{
    if (collection->count(id) > 0)
    {
        element = collection->find(id)->second;
        printf("File id:%llu, path:%s returned.\n", id, element->Path());
        return KReturn_Success;
    }
    else
    {
        return KReturn_Fail;
    }
}

TReturn CResourceCollection::ResourceList(TResourceList & list, const SResourceFilter * filter)
{
    if (filter == nullptr) {
        for (map<TId, IResource*>::iterator it = collection->begin(); it != collection->end(); ++it) {
            list.push_back(it->second);
        }

        return KReturn_Success;
    }
    else if (filter->filterBy == SResourceFilter::KFilterBy_Path) {
        for (map<TId, IResource*>::iterator it = collection->begin(); it != collection->end(); ++it) {
            string p = (string)it->second->Path();
            if (p.find(filter->path) != string::npos) {
                list.push_back(it->second);
            }
        }

        return KReturn_Success;
    }
    else if (filter->filterBy == SResourceFilter::KFilterBy_ResourceType) {
        for (map<TId, IResource*>::iterator it = collection->begin(); it != collection->end(); ++it) {
            if (it->second->Type() == filter->resType) {
                list.push_back(it->second);
            }
        }

        return KReturn_Success;
    }

    return KReturn_Fail;
}

TUtf8 CResourceCollection::DB(void)
{
    return dbFilename;
}

// Functions to deal with resource collections

IResourceCollection *CreateResourceCollection(TUtf8 fn) {
    IResourceCollection *col = new CResourceCollection(fn);
    return col;
}

void DestroyResourceCollection(IResourceCollection *collection) {
    delete collection;
    printf("Collection Deleted.\n");
    collection = nullptr;
}

// functions to deal with video resources

TId CVideoResource::Id(void) const
{
    return id;
}

TUtf8 CVideoResource::Path(void) const
{
    return path;
}

SDataRef CVideoResource::Raw(void)
{
    return raw;
}

IVideoResource::CVideoProperties * CVideoResource::Properties(void)
{
    return properties;
}

vector<TUtf8> ReadPath(TUtf8 path) {
    vector<TUtf8> pathList;
    for (auto & p : fs::recursive_directory_iterator(path)) {
        pathList.push_back(p.path().u8string().data());
        std::cout << p << std::endl;
    }
    return pathList;
}
