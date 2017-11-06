#include "stdafx.h"

// Test collection has been created and placed into memory
TEST(ColTest, CreateTest) {
    IResourceCollection *col = CreateResourceCollection("junk.db");
    EXPECT_TRUE(col != nullptr);
    DestroyResourceCollection(col);
}

// Test add resource to collection and return the same object back
TEST(ColTest, AddTest) {
    IResourceCollection *col = CreateResourceCollection("junk.db");
    TId temp;
    IResource *tempR;
    ASSERT_EQ(KReturn_Success, col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\hello.txt", temp));
    ASSERT_EQ(KReturn_Success, col->ResourceFromId(temp, tempR));
    EXPECT_TRUE(temp == tempR->Id());
    DestroyResourceCollection(col);
}

// Test add resource to collection and return the same object back
TEST(ColTest, AddDiffTest) {
    IResourceCollection *col = CreateResourceCollection("junk.db");
    TId temp;
    IResource *tempR;
    ASSERT_EQ(KReturn_Success, col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\hello.txt", temp));
    ASSERT_EQ(KReturn_Fail, col->ResourceFromId((TId)-1, tempR));
    DestroyResourceCollection(col);
}

// Test if resource was created
TEST(ResTest, CreateResTest) {
    IResourceCollection *col = CreateResourceCollection("junk.db");
    TId temp;
    IResource *tempR;
    ASSERT_EQ(KReturn_Success, col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\hello.txt", temp));
    ASSERT_EQ(KReturn_Success, col->ResourceFromId(temp, tempR));
    EXPECT_TRUE(tempR != nullptr);
    DestroyResourceCollection(col);
}

// Test IResource.Id() pass
TEST(ResTest, IdTestPass) {
    IResourceCollection *col = CreateResourceCollection("junk.db");
    TId temp;
    IResource *tempR;
    ASSERT_EQ(KReturn_Success, col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\hello.txt", temp));
    ASSERT_EQ(KReturn_Success, col->ResourceFromId(temp, tempR));
    ASSERT_EQ(temp, tempR->Id());
    DestroyResourceCollection(col);
}

// Test IResource.PATH()
TEST(ResTest, GetPathTest) {
    IResourceCollection *col = CreateResourceCollection("junk.db");
    TId temp;
    IResource *tempR;
    ASSERT_EQ(KReturn_Success, col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\frog1.jpg", temp));
    ASSERT_EQ(KReturn_Success, col->ResourceFromId(temp, tempR));
    ASSERT_EQ((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\frog1.jpg", tempR->Path());
    DestroyResourceCollection(col);
}

// Test that the resource list is populated with all items that were in the collection
TEST(ResListTest, DefaultFilterTest) {
    IResourceCollection *col = CreateResourceCollection("junk.db");
    TId temp;
    IResource *tempR;
    ASSERT_EQ(KReturn_Success, col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\hello.txt", temp));
    ASSERT_EQ(KReturn_Success, col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\name.txt", temp));
    ASSERT_EQ(KReturn_Success, col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\work.txt", temp));
    ASSERT_EQ(KReturn_Success, col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\frog1.jpg", temp));
    ASSERT_EQ(KReturn_Success, col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\frog2.jpg", temp));
    ASSERT_EQ(KReturn_Success, col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\frog3.jpg", temp));
    ASSERT_EQ(KReturn_Success, col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\frog4.jpg", temp));
    ASSERT_EQ(KReturn_Success, col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\frog5.jpg", temp));

    TResourceList list;
    col->ResourceList(list);
    for (TResourceList::iterator it = list.begin(); it != list.end(); ++it) {
        TResourceType type = (*it)->Type();
        EXPECT_TRUE(type == KResourceType_Unknown || type == KResourceType_Image);
    }
    DestroyResourceCollection(col);
}


// Test that the resource list is populated with only items with path filter on
TEST(ResListTest, PathFilterTest) {
    IResourceCollection *col = CreateResourceCollection("junk.db");
    TId temp;
    IResource *tempR;
    col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\hello.txt", temp);
    col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\name.txt", temp);
    col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\work.txt", temp);
    col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\frog1.jpg", temp);
    col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\frog2.jpg", temp);
    col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\frog3.jpg", temp);
    TResourceList list;
    SResourceFilter* temp_filter = new SResourceFilter();
    temp_filter->path = "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\";
    temp_filter->filterBy = SResourceFilter::KFilterBy_Path;
    const SResourceFilter* filter = temp_filter;
    col->ResourceList(list, filter);
    DestroyResourceCollection(col);

    for (TResourceList::iterator it = list.begin(); it != list.end(); ++it) {
        char * p = (char *)(*it)->Path();
        EXPECT_TRUE(strncmp(p, filter->path, strlen(filter->path)) == 0);
    }

}

// Test that the resource list is populated with only items with type unknown filter on
TEST(ResListTest, UnknownTypeFilterTest) {
    IResourceCollection *col = CreateResourceCollection("junk.db");
    TId temp;
    IResource *tempR;
    col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\hello.txt", temp);
    col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\name.txt", temp);
    col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\work.txt", temp);
    col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\frog1.jpg", temp);
    col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\frog2.jpg", temp);
    col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\frog3.jpg", temp);
    TResourceList list;
    SResourceFilter* temp_filter = new SResourceFilter();
    temp_filter->resType = KResourceType_Unknown;
    temp_filter->filterBy = SResourceFilter::KFilterBy_ResourceType;
    const SResourceFilter* filter = temp_filter;
    col->ResourceList(list, filter);
    DestroyResourceCollection(col);

    for (TResourceList::iterator it = list.begin(); it != list.end(); ++it) {
        ASSERT_EQ(KResourceType_Unknown, (*it)->Type());
    }
}

// Test that the resource list is populated with only items with type image filter on
TEST(ResListTest, ImageTypeFilterTest) {
    IResourceCollection *col = CreateResourceCollection("junk.db");
    TId temp;
    IResource *tempR;
    col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\hello.txt", temp);
    col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\name.txt", temp);
    col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\work.txt", temp);
    col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\frog1.jpg", temp);
    col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\frog2.jpg", temp);
    col->AddFile((TUtf8) "C:\\Users\\Administrator\\source\\repos\\ExecDynLib\\Debug\\frog3.jpg", temp);
    TResourceList list;
    SResourceFilter* temp_filter = new SResourceFilter();
    temp_filter->resType = KResourceType_Image;
    temp_filter->filterBy = SResourceFilter::KFilterBy_ResourceType;
    const SResourceFilter* filter = temp_filter;
    col->ResourceList(list, filter);
    DestroyResourceCollection(col);

    for (TResourceList::iterator it = list.begin(); it != list.end(); ++it) {
        ASSERT_EQ(KResourceType_Image, (*it)->Type());
    }
}
