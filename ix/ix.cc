
#include "ix.h"

const int BPTOrder = 2;

class BPTNode {
    BPTNode* next;
    BPTNode* prev;

};

class BPTree {
public:

private:
    BPTNode* _root;
};

IndexManager* IndexManager::_index_manager = 0;

IndexManager* IndexManager::instance()
{
    if(!_index_manager)
        _index_manager = new IndexManager();

    return _index_manager;
}

IndexManager::IndexManager()
{
}

IndexManager::~IndexManager()
{
}

RC IndexManager::createFile(const string &fileName)
{
    struct stat stFileInfo;
    if (stat(fileName.c_str(), &stFileInfo) == 0)
        return -1;


    FILE* fp = fopen(fileName.c_str(), "a+b");
    if (fp == NULL)
        return -1;

    fclose(fp);

    return 0;
}

RC IndexManager::destroyFile(const string &fileName)
{
    if (remove(fileName.c_str()) != 0)
        return -1;
    return 0;
}

RC IndexManager::openFile(const string &fileName, IXFileHandle &ixfileHandle)
{
    return ixfileHandle.openFile(fileName);
}

RC IndexManager::closeFile(IXFileHandle &ixfileHandle)
{
    return ixfileHandle.closeFile();
}

RC IndexManager::insertEntry(IXFileHandle &ixfileHandle, const Attribute &attribute, const void *key, const RID &rid)
{
    return -1;
}

RC IndexManager::deleteEntry(IXFileHandle &ixfileHandle, const Attribute &attribute, const void *key, const RID &rid)
{
    return -1;
}


RC IndexManager::scan(IXFileHandle &ixfileHandle,
        const Attribute &attribute,
        const void      *lowKey,
        const void      *highKey,
        bool			lowKeyInclusive,
        bool        	highKeyInclusive,
        IX_ScanIterator &ix_ScanIterator)
{
    return -1;
}

void IndexManager::printBtree(IXFileHandle &ixfileHandle, const Attribute &attribute) const {
}

IX_ScanIterator::IX_ScanIterator()
{
}

IX_ScanIterator::~IX_ScanIterator()
{
}

RC IX_ScanIterator::getNextEntry(RID &rid, void *key)
{
    return -1;
}

RC IX_ScanIterator::close()
{
    return -1;
}


IXFileHandle::IXFileHandle()
{
    ixReadPageCounter = 0;
    ixWritePageCounter = 0;
    ixAppendPageCounter = 0;
    _fp = NULL;
}

IXFileHandle::~IXFileHandle()
{
}

RC IXFileHandle::collectCounterValues(unsigned &readPageCount, unsigned &writePageCount, unsigned &appendPageCount)
{
    int stat = readCountersFromFile();
    if (stat) return stat;

    readPageCount = ixReadPageCounter;
    writePageCount = ixWritePageCounter;
    appendPageCount = ixAppendPageCounter;
    
    return 0;
}


int IXFileHandle::openFile(const string &fname)
{
    struct stat stFileInfo;
    if (stat(fname.c_str(), &stFileInfo) != 0) {
        _fp = fopen(fname.c_str(), "wb");
        if (_fp == NULL)
            return -1;
        fclose(_fp);
    }

    _fp = fopen(fname.c_str(), "r+b");
    if (_fp == NULL)
        return -1;
    
    int stat = 0;
    if (getTotalNumberOfPages() == 0) {
        stat = initFile();
        if (stat) return stat;
    }  else {
        stat = readCountersFromFile();
        if (stat) return stat;
    }

    return 0;
}

int IXFileHandle::closeFile()
{
    fclose(_fp);
    _fp = NULL;
    return 0;
}

int IXFileHandle::getTotalNumberOfPages()
{
    if (NULL == _fp)
        return -1;
    if (fseek(_fp, 0, SEEK_END)) return -1;
    //    fseek(fp, 0L, SEEK_END);
    long sz = ftell(_fp);
    return ceil(sz / (double)PAGE_SIZE);
}

int IXFileHandle::updataHeaderPage()
{
    if (NULL == _fp)
        return -1;

    void* data = malloc(PAGE_SIZE);
    int offset = 0;
    memcpy((char*)data + offset, (char*)&ixReadPageCounter, sizeof(int));
    offset += sizeof(int);
    memcpy((char*)data + offset, (char*)&ixWritePageCounter, sizeof(int));
    offset += sizeof(int);
    memcpy((char*)data + offset, (char*)&ixAppendPageCounter, sizeof(int));

    rewind(_fp);
    if (fseek(_fp, HEADER_PAGE_OFFSET, SEEK_SET)) return -1;
    if (PAGE_SIZE != fwrite(data, 1, PAGE_SIZE, _fp)) return -1;
    fflush(_fp);

    free(data);

    return 0;

}

int IXFileHandle::initFile()
{
    if (NULL == _fp)
        return -1;

    return updataHeaderPage();

}

int IXFileHandle::readCountersFromFile()
{
    if (NULL == _fp)
        return -1;

    void* data = malloc(PAGE_SIZE);

    rewind(_fp);
    if (fseek(_fp, HEADER_PAGE_OFFSET, SEEK_SET)) return -1;
    if (PAGE_SIZE != fread(data, 1, PAGE_SIZE, _fp)) return -1;

    int offset = 0;
    memcpy((char*)&ixReadPageCounter, (char*)data + offset, sizeof(int));
    offset += sizeof(int);
    memcpy((char*)&ixWritePageCounter, (char*)data + offset, sizeof(int));
    offset += sizeof(int);
    memcpy((char*)&ixAppendPageCounter, (char*)data + offset, sizeof(int));

    free(data);

    return 0;

}

